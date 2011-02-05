//
//
//****************************************************************************************//
// Copyright (c) 2002-2011, The MITRE Corporation
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

#ifndef REGEXCAPTUREFUNCTION_H
#define REGEXCAPTUREFUNCTION_H

#include "AbsFunctionComponent.h"
#include "ComponentFactory.h"
#include "REGEX.h"

XERCES_CPP_NAMESPACE_USE

/**
	This class represents a RegexCaptureFunction component in a local_variable in the oval definition schema.

    The regex_capture function accepts one string component input and returns the first 
    capture in each component value, as matched against the regex pattern specified in the
    'pattern' attribute.

*/
class RegexCaptureFunction : public AbsFunctionComponent {
public:

	/** Create a complete RegexCaptureFunction object. */
	RegexCaptureFunction(std::string pattern = "");
	~RegexCaptureFunction();

	/** Parse the substring element and its child component element. */
	void Parse(DOMElement* componentElm); 

	/** Compute the desired substring and return the value. */
	ComponentValue* ComputeValue();

	/** Return the variable values used to compute this function's value. */
	VariableValueVector* GetVariableValues();

	/** Get the pattern field's value. */
	std::string GetPattern();
	/** Set the pattern field's value. */
	void SetPattern(std::string pattern);

private:
	std::string pattern;
    REGEX reUtil;
};

#endif
