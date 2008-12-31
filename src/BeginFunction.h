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

#ifndef BEGINFUNCTION_H
#define BEGINFUNCTION_H

#include "AbsFunctionComponent.h"
#include "ComponentFactory.h"

XERCES_CPP_NAMESPACE_USE
using namespace std;

/**
	This class represents a BeginFunction component in a local_variable in the oval definition schema.

    The begin function takes a single string component and defines a character (or string) that the 
    component string should start with. The character attribute defines the specific character 
    (or string). The character (or string) is only added to the component string if the component 
    string doesn't already start with the specified character (or string).
*/
class BeginFunction : public AbsFunctionComponent {
public:

	/** Create a complete BeginFunction object. */
	BeginFunction(string charIn = "");
	~BeginFunction();

	/** Parse the begin element and its child component element. */
	void Parse(DOMElement* componentElm); 

	/** Compute and return the value. */
	ComponentValue* ComputeValue();

	/** Return the variable values used to compute this function's value. */
	VariableValueVector* GetVariableValues();

	/** Get the character field's value. */
	string GetCharacter();
	/** Set the character field's value. */
	void SetCharacter(string charIn);

private:
	string character;
};

#endif
