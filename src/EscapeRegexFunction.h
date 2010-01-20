//
//
//****************************************************************************************//
// Copyright (c) 2002-2010, The MITRE Corporation
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

#ifndef ESCAPEREGEXFUNCTION_H
#define ESCAPEREGEXFUNCTION_H

#include "AbsFunctionComponent.h"
#include "ComponentFactory.h"

XERCES_CPP_NAMESPACE_USE

/**
	This class represents a EscapeRegexFunction component in a local_variable in the oval definition schema.
	The schema describes this functions as follows:

	The escape regex function takes a single string component and escapes all the 
	regular expression characters. The purpose for this is that many times, a component
	used in pattern match needs to be treated a literal string and not regular expression.
	For example assume a basic component element that pulls a file path out of the Windows
	registry. This path is a string that might contain regular expression characters but 
	these characters are not intended to be such, so they need to be escaped. This function
	allows a definition	writer to mark which components are in regular expression format and
	which aren't.
*/
class EscapeRegexFunction : public AbsFunctionComponent {
public:

	/** Create a complete Component object. */
	EscapeRegexFunction();
	~EscapeRegexFunction();

	/** parse the component element. */
	void Parse(DOMElement* componentElm); 

	/** Compute the value by escaping all the values of the associated component. */
	ComponentValue* ComputeValue();

	/** Return the variable values used to compute this function's value. */
	VariableValueVector* GetVariableValues();
};

#endif
