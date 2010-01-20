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

#ifndef VARIABLEVALUE_H
#define VARIABLEVALUE_H

#include "XmlCommon.h"

XERCES_CPP_NAMESPACE_USE

class VariableValue;
typedef std::vector < VariableValue* > VariableValueVector;

/**
	This class represents a variable value.
	Vairable values are used in the oval system characterisitcs schema and the oval results schema.
*/
class VariableValue {
public:
	VariableValue(std::string id = "", std::string value = "");
	~VariableValue();

	void Parse(DOMElement* variableValueElm);
	void Write(DOMElement* collectedObjectElm);
	void WriteTestedVariable(DOMElement* parentElm);

	std::string GetId();
	void SetId(std::string id);

	std::string GetValue();
	void SetValue(std::string value);

	static void ClearCache();

private:
	std::string id;
	std::string value;

	static VariableValueVector vars;
};

typedef std::vector < VariableValue* > VariableValueVector;

#endif
