//
//
//****************************************************************************************//
// Copyright (c) 2002-2009, The MITRE Corporation
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

#ifndef ABSCOMPONENT_H
#define ABSCOMPONENT_H


#include "Common.h"
#include "VariableValue.h"
#include "ComponentValue.h"


XERCES_CPP_NAMESPACE_USE
using namespace std;

/**
	This calss is the base class for all types of variable components.
	The oval definition schema defines several types of components that ay be grouped 
	together to form complex local_varaibles. The commonalities among all components
	are encapsulated in this class.
*/
class AbsComponent {
public:
	/** Create complete AbsComponent. */
	AbsComponent();

	virtual ~AbsComponent();

	/** Parse a compnent element.
		Each concrete component has a slightly different corresponding element to parse.
	*/
	virtual void Parse(DOMElement* componentElm) = 0;
	/** Compute the value for the AbsComponent.
		Each concrete component computes its value differently.
	*/
	virtual ComponentValue* ComputeValue() = 0;
	/** Return the variable values used to compute the concrete component's value.
		Each concrete component uses variables differently and will return this set differently.
	*/
	virtual VariableValueVector* GetVariableValues() = 0;
};

/**
	A vector for storing AbsComponent objects. 
	Stores only pointers to the objects. 
*/
typedef vector < AbsComponent*, allocator<AbsComponent*> > AbsComponentVector;

#endif
