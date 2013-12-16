//
//
//****************************************************************************************//
// Copyright (c) 2002-2013, The MITRE Corporation
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

#ifndef EXTERNALVARIABLE_H
#define EXTERNALVARIABLE_H

#include <xercesc/dom/DOMElement.hpp>

#include "PossibleValueType.h"
#include "PossibleRestrictionType.h"

/**
	This class represents an external_variable in the oval definition schema.
*/
class ExternalVariable : public AbsVariable {
public:

	ExternalVariable(std::string id = "", std::string name = "external_variable", int version = 1, OvalEnum::Datatype datatype = OvalEnum::DATATYPE_STRING, StringVector* msgs = new StringVector());
	~ExternalVariable();

	/** Parse the provided ExternalVariable element into a ExternalVariable. */
	void Parse(xercesc::DOMElement* externalVariableElm);

	/** Fetch the variable from the external-variables.xml file.
		Then get each value associated with the variable.
		Then validate each value then create a VariableValue. 
	*/
	void ComputeValue();

	/** Validate that a given external variable value matches the possible values and possible restrictions for this ExternalVariable. */
	bool ValidateValue(OvalEnum::Datatype datatype, std::string externalValue);

	/** Get the set of PossibleValueType objects for this ExternalVariable. */
	PossibleValueTypeVector* GetPossibleValueTypes();

	/** Append a PossibleValueType to the set of PossibleValueType objects for this ExternalVariable. */
	void AppendPossibleValueType(PossibleValueType* pv);

	/** Get the set of PossibleRestrictionType objects for this ExternalVariable. */
	PossibleRestrictionTypeVector* GetPossibleRestrictionTypes();

	/** Append a PossibleRestrictionType to the set of PossibleRestrictionType objects for this ExternalVariable. */
	void AppendPossibleRestrictionType(PossibleRestrictionType* pr);

	/** Return the variable values used to compute this variable's value. In this case just an empty vector. */
	VariableValueVector* GetVariableValues();

	PossibleValueTypeVector possibleValueTypes;
	PossibleRestrictionTypeVector possibleRestrictionTypes;
};

#endif
