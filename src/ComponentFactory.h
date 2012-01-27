//
//
//****************************************************************************************//
// Copyright (c) 2002-2012, The MITRE Corporation
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright notice, this list
//       of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright notice, this 
//       list of conditions and the following disclaimer in the documentation and/or other
//       materials provided with the distribution.
//     * Neither the name of The MITRE Corporation nor the names of its contributors may be
//       used to endorse or promote products derived from this software without specific 
//       prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
// SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
// OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
// TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//****************************************************************************************//

#ifndef COMPONENTFACTORY_H
#define COMPONENTFACTORY_H

#include "VariableComponent.h"
#include "LiteralComponent.h"
#include "ObjectComponent.h"
#include "SubstringFunction.h"
#include "ConcatFunction.h"
#include "EscapeRegexFunction.h"
#include "BeginFunction.h"
#include "EndFunction.h"
#include "SplitFunction.h"
#include "RegexCaptureFunction.h"
#include "ArithmeticFunction.h"
#include "TimeDifferenceFunction.h"
#include "CountFunction.h"
#include "UniqueFunction.h"

XERCES_CPP_NAMESPACE_USE

/**
	This class is a Factory class for getting parsed components of a local variable.
	This class allows for a single interface to all types of components defined 
	in the oval definitions schema.
*/
class ComponentFactory {

public:
	/** Return the appropriate component based on the specified component element.*/
	static AbsComponent* GetComponent(DOMElement* componentElm);
};

#endif
