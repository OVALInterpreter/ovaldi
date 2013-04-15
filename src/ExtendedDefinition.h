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

#ifndef EXTENDEDDEFINITION_H
#define EXTENDEDDEFINITION_H

#include "Definition.h"
#include "AbsCriteria.h"
#include "Analyzer.h"

XERCES_CPP_NAMESPACE_USE

class AbsCriteria;
class Definition;

/**
	This class represents an extended_definition in the oval defintiion schema.
	Extended definitions are a type of AbsCriteria.
*/
class ExtendedDefinition : public AbsCriteria {

public:
	ExtendedDefinition(bool negate = false, ApplicabilityCheck appCheck = APPLICABILITY_CHECK_UNKNOWN, OvalEnum::ResultEnumeration result = OvalEnum::RESULT_ERROR, Definition* definitionRef = NULL);
	~ExtendedDefinition();

	/** Writes a ExtendedDefinition element as a child of the parentElm.
		Calls Definition->Write() on the Definition Ref to ensure that
		it is also written to the results document.
	*/
	void Write(DOMElement* parent);
	
	/** Parse ExtendedDefinition element into a ExtendedDefinition object.
		Search the cach
		calls definition->Parse() on the definition ref to 
	*/
	void Parse(DOMElement* ExtendedDefinitionElm);

	/** Analyze the ExtendedDefinition object.
		Calls definition->Analyze() to get the result for the definition.
		Then applies the negate attribute. 
		Finally saves and returns the result
	*/
	OvalEnum::ResultEnumeration Analyze();

	/** Mark this definition as not evaluated. */
	OvalEnum::ResultEnumeration NotEvaluated();

	/** Return the definitionRef field's value. */
	Definition* GetDefinitionRef();
	/** Set the definitionRef field's value. */
	void SetDefinitionRef(Definition* definitionRef);

private:
	Definition* definitionRef;
};

#endif
