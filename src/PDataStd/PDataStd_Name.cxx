// Copyright (c) 1997-1999 Matra Datavision
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

#include <PDataStd_Name.ixx>

PDataStd_Name::PDataStd_Name() {}

PDataStd_Name::PDataStd_Name(const Handle(PCollection_HExtendedString)& V)
: myValue (V) {}

Handle(PCollection_HExtendedString) PDataStd_Name::Get() const
{ return myValue; }

void PDataStd_Name::Set(const Handle(PCollection_HExtendedString)& V) 
{ myValue = V; }

