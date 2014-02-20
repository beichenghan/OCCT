// Created by: Peter KURNEV
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <BOPTest.ixx>

#include <stdio.h>
#include <string.h>

#include <NCollection_IncAllocator.hxx>

#include <DBRep.hxx>
#include <Draw.hxx>
#include <Draw_Color.hxx>
#include <DrawTrSurf.hxx>

#include <TopoDS_Shape.hxx>
//
#include <BOPAlgo_Builder.hxx>
#include <BOPAlgo_PaveFiller.hxx>
#include <BOPAlgo_Operation.hxx>
#include <BOPAlgo_BOP.hxx>
//
#include <BOPTest_DrawableShape.hxx>
#include <BOPTest_Objects.hxx>

//
#ifdef HAVE_TBB
#include <BOPCol_TBB.hxx>
//=======================================================================
//class : BOPTime_Chronometer
//purpose  : 
//=======================================================================
class BOPTime_Chronometer {
 public:
  BOPTime_Chronometer() {
  }
  //
  ~BOPTime_Chronometer() {
  }
  //
  void Start() {
    myT0 = tick_count::now();
  }
  //
  void Stop() {
    myTime=(tick_count::now() - myT0).seconds();
  }
  //
  double Time() const{
    return myTime;
  };
  //
 protected:
  tick_count myT0;
  double myTime;
};
////////////////////////////////////////////////////////////////////////
#else
#include <OSD_Chronometer.hxx>
//=======================================================================
//class    : BOPTime_Chronometer
//purpose  : 
//=======================================================================
class BOPTime_Chronometer {
 public:
  BOPTime_Chronometer() {
  }
  //
  ~BOPTime_Chronometer() {
  }
  //
  void Start() {
    myChronometer.Reset();
    myChronometer.Start();
  }
  //
  void Stop() {
    myChronometer.Stop();
    myChronometer.Show(myTime);
  }
  //
  double Time() const{
    return myTime;
  };
  //
 protected:
  OSD_Chronometer myChronometer;
  double myTime;
};
#endif



static Standard_Integer bfillds  (Draw_Interpretor&, Standard_Integer, const char**); 
static Standard_Integer bbuild   (Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bbop     (Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bclear   (Draw_Interpretor&, Standard_Integer, const char**);
//=======================================================================
//function : PartitionCommands
//purpose  : 
//=======================================================================
  void BOPTest::PartitionCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;
  // Chapter's name
  const char* g = "Partition commands";
  // Commands  
  theCommands.Add("bfillds"  , "use bfillds"           , __FILE__, bfillds  , g);
  theCommands.Add("bbuild"   , " use bbuild r [-s -t]" , __FILE__, bbuild, g);
  theCommands.Add("bbop"     , "use bbop r op"         , __FILE__, bbop, g);
  theCommands.Add("bclear"   , "use bclear"            , __FILE__, bclear, g);
}

//=======================================================================
//function : bclear
//purpose  : 
//=======================================================================
Standard_Integer bclear(Draw_Interpretor& di, Standard_Integer n, const char** ) 
{
  if (n!=1) {
    di << " use bclear\n";
    return 0;
  }
  //
  BOPTest_Objects::Clear(); 
  return 0;
}
//=======================================================================
//function : bfillds
//purpose  : 
//=======================================================================
Standard_Integer bfillds(Draw_Interpretor& di, Standard_Integer n, const char** ) 
{ 
  if (n!=1) {
    di << " Use bfillds\n";
    return 0;
  }
  //
  char buf[32];
  Standard_Integer aNbS, iErr;
  BOPCol_ListIteratorOfListOfShape aIt;
  BOPCol_ListOfShape aLC;
  
  BOPCol_ListOfShape& aLS=BOPTest_Objects::Shapes();
  aNbS=aLS.Extent();
  if (!aNbS) {
    di << " no objects to process\n";
    return 0;
  }
  //
  BOPCol_ListOfShape& aLT=BOPTest_Objects::Tools();
  //
  aIt.Initialize(aLS);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS=aIt.Value();
    aLC.Append(aS);
  }
  //
  aIt.Initialize(aLT);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS=aIt.Value();
     aLC.Append(aS);
  }
  //
  BOPAlgo_PaveFiller& aPF=BOPTest_Objects::PaveFiller();
  //
  aPF.SetArguments(aLC);
  //
  aPF.Perform();
  iErr=aPF.ErrorStatus();
  if (iErr) {
    Sprintf(buf, " error: %d\n",  iErr);
    di << buf;
    return 0;
  }
  //
  return 0;
}
//=======================================================================
//function : bbuild
//purpose  : 
//=======================================================================
Standard_Integer bbuild(Draw_Interpretor& di, Standard_Integer n, const char** a) 
{ 
  if (n<2) {
    di << " use bbuild r [-s -t]\n";
    return 0;
  }
  //
  BOPDS_PDS pDS=BOPTest_Objects::PDS();
  if (!pDS) {
    di << " prepare PaveFiller first\n";
    return 0;
  }
  //
  char buf[128];
  Standard_Boolean bRunParallel, bShowTime;
  Standard_Integer i, iErr;
  
  BOPTime_Chronometer aChrono;
  BOPCol_ListIteratorOfListOfShape aIt;
  //
  
  //
  BOPAlgo_PaveFiller& aPF=BOPTest_Objects::PaveFiller();
  //
  BOPAlgo_Builder& aBuilder=BOPTest_Objects::Builder();
  aBuilder.Clear();
  //
  BOPCol_ListOfShape& aLSObj=BOPTest_Objects::Shapes();
  aIt.Initialize(aLSObj);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS=aIt.Value();
    aBuilder.AddArgument(aS);
  }
  //
  BOPCol_ListOfShape& aLSTool=BOPTest_Objects::Tools();
  aIt.Initialize(aLSTool);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS=aIt.Value();
    aBuilder.AddArgument(aS);
  }
  //
  bShowTime=Standard_False;
  bRunParallel=Standard_True;
  for (i=2; i<n; ++i) {
    if (!strcmp(a[i], "-s")) {
      bRunParallel=Standard_False;
    }
    else if (!strcmp(a[i], "-t")) {
      bShowTime=Standard_True;
    }
  }
  aBuilder.SetRunParallel(bRunParallel);
  //
  //
  aChrono.Start();
  //
  aBuilder.PerformWithFiller(aPF); 
  iErr=aBuilder.ErrorStatus();
  if (iErr) {
    Sprintf(buf, " error: %d\n",  iErr);
    di << buf;
    return 0;
  }
  //
  aChrono.Stop();
  //
  if (bShowTime) {
    Standard_Real aTime;
    //
    aTime=aChrono.Time();
    Sprintf(buf, "  Tps: %7.2lf\n", aTime);
    di << buf;
  }
  //
  const TopoDS_Shape& aR=aBuilder.Shape();
  if (aR.IsNull()) {
    di << " null shape\n";
    return 0;
  }
  //
  DBRep::Set(a[1], aR);
  return 0;
}

//=======================================================================
//function : bbop
//purpose  : 
//=======================================================================
Standard_Integer bbop(Draw_Interpretor& di, Standard_Integer n, const char** a) 
{ 
  if (n!=3) {
    di << " use bbop r op\n";
    return 0;
  }
  //
  BOPDS_PDS pDS=BOPTest_Objects::PDS();
  if (!pDS) {
    di << " prepare PaveFiller first\n";
    return 0;
  }
  //
  char buf[32];
  Standard_Integer iErr, iOp;
  BOPAlgo_Operation aOp;
  BOPCol_ListIteratorOfListOfShape aIt;
  //
  iOp=Draw::Atoi(a[2]);
  if (iOp<0 || iOp>4) {
    di << " invalid operation type\n";
  }
  aOp=(BOPAlgo_Operation)iOp;
  //
  BOPAlgo_PaveFiller& aPF=BOPTest_Objects::PaveFiller();
  //
  BOPAlgo_BOP& aBOP=BOPTest_Objects::BOP();
  aBOP.Clear();
  //
  BOPCol_ListOfShape& aLSObj=BOPTest_Objects::Shapes();
  aIt.Initialize(aLSObj);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS=aIt.Value();
    aBOP.AddArgument(aS);
  }
  //
  BOPCol_ListOfShape& aLSTools=BOPTest_Objects::Tools();
  aIt.Initialize(aLSTools);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS=aIt.Value();
    aBOP.AddTool(aS);
  }
  //
  aBOP.SetOperation(aOp);
  //
  aBOP.PerformWithFiller(aPF);
  iErr=aBOP.ErrorStatus();
  if (iErr) {
    Sprintf(buf, " error: %d\n",  iErr);
    di << buf;
    return 0;
  }
  //
  const TopoDS_Shape& aR=aBOP.Shape();
  if (aR.IsNull()) {
    di << " null shape\n";
    return 0;
  }
  //
  DBRep::Set(a[1], aR);
  return 0;
}

