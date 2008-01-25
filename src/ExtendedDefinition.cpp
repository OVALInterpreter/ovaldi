//
// $Id: ExtendedDefinition.cpp 4579 2008-01-02 17:39:07Z bakerj $
//
//****************************************************************************************//
// Copyright (c) 2002-2008, The MITRE Corporation
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

#include "ExtendedDefinition.h"

//****************************************************************************************//
//								ExtendedDefinition Class								  //	
//****************************************************************************************//
ExtendedDefinition::ExtendedDefinition(bool negate, OvalEnum::ResultEnumeration result, Definition* definitionRef) 
					: AbsCriteria(negate, result) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Create a complete ExtendedDefinition object
	//
	// -----------------------------------------------------------------------

	this->SetDefinitionRef(definitionRef);
}

ExtendedDefinition::~ExtendedDefinition() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Do nothing for now
	//
	// -----------------------------------------------------------------------
}

// ***************************************************************************************	//
//								 Public members												//
// ***************************************************************************************	//

Definition* ExtendedDefinition::GetDefinitionRef() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the definitionRef field's value
	//
	// -----------------------------------------------------------------------

	return this->definitionRef;
}

void ExtendedDefinition::SetDefinitionRef(Definition* definitionRef) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the definitionRef field's value
	//
	// -----------------------------------------------------------------------

	this->definitionRef = definitionRef;
}

void ExtendedDefinition::Write(DOMElement* parentElm) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	writes a ExtendedDefinition element
	//	calls Definition->Write() on the test ref
	//
	// -----------------------------------------------------------------------

	// get the parent document
	XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* resultDoc = parentElm->getOwnerDocument();

	// create a new criteria element
	DOMElement* extendedDefinitionElm = XmlCommon::AddChildElement(resultDoc, parentElm, "extend_definition");

	// add the attributes
	XmlCommon::AddAttribute(extendedDefinitionElm, "definition_ref", this->GetDefinitionRef()->GetId());

	XmlCommon::AddAttribute(extendedDefinitionElm, "version", Common::ToString(this->GetDefinitionRef()->GetVersion()));

	if(this->GetDefinitionRef()->GetVariableInstance() != 1) {
		XmlCommon::AddAttribute(extendedDefinitionElm, "variable_instance", Common::ToString(this->GetDefinitionRef()->GetVariableInstance()));
	}

	string negateStr = Common::ToString(this->GetNegate());
	if(negateStr.compare("true") == 0) {
		XmlCommon::AddAttribute(extendedDefinitionElm, "negate", negateStr);
	}

	XmlCommon::AddAttribute(extendedDefinitionElm, "result", OvalEnum::ResultToString(this->GetResult()));

	// write the definition ref
	this->GetDefinitionRef()->Write(Analyzer::GetResultsDefinitionsElm());
}

void ExtendedDefinition::Parse(DOMElement* extendedDefinitionElm) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	parses ExtendedDefinition elm to a ExtendedDefinition obj
	//	calls definition->Parse() on the definition ref
	//
	// -----------------------------------------------------------------------

	// get the attributes
	string negateStr = XmlCommon::GetAttributeByName(extendedDefinitionElm, "negate");
	if(negateStr.compare("") == 0 || negateStr.compare("false") == 0) {
        this->SetNegate(false);
	} else {
	    this->SetNegate(true);
	}

	// get the definition ref
	string definitionRefStr = XmlCommon::GetAttributeByName(extendedDefinitionElm, "definition_ref");
	Definition* defRef = NULL;
	defRef = Definition::SearchCache(definitionRefStr);
	if(defRef == NULL) {
		DOMElement* definitionsElm = XmlCommon::FindElementNS(extendedDefinitionElm->getOwnerDocument(), "definitions");
		DOMElement* definitionElm = XmlCommon::FindElementByAttribute(definitionsElm, "id", definitionRefStr);	
		defRef = new Definition();
		defRef->Parse(definitionElm);
	}
	this->SetDefinitionRef(defRef);
}

OvalEnum::ResultEnumeration ExtendedDefinition::Analyze() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	calls definition->Analyze()
	//	applies negate attribute
	//	saves and return the result
	// -----------------------------------------------------------------------

	// analyze the test
	OvalEnum::ResultEnumeration currentResult = this->GetDefinitionRef()->Analyze();

	// apply the negate attribute and save the result
	if(this->GetNegate()) {
		this->SetResult(OvalEnum::NegateResult(currentResult));
	} else {
		this->SetResult(currentResult);
	}

	return this->GetResult();
}

OvalEnum::ResultEnumeration ExtendedDefinition::NotEvaluated() {
	OvalEnum::ResultEnumeration currentResult = this->GetDefinitionRef()->NotEvaluated();

	// apply the negate attribute and save the result
	if(this->GetNegate()) {
		this->SetResult(OvalEnum::NegateResult(currentResult));
	} else {
		this->SetResult(currentResult);
	}

	return this->GetResult();
}

