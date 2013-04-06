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

#include "Analyzer.h"
#include "XmlCommon.h"
#include "Common.h"

#include "ExtendedDefinition.h"

using namespace std;
using namespace xercesc;

//****************************************************************************************//
//								ExtendedDefinition Class								  //	
//****************************************************************************************//
ExtendedDefinition::ExtendedDefinition(bool negate, ApplicabilityCheck appCheck, OvalEnum::ResultEnumeration result, Definition* definitionRef)
					: AbsCriteria(negate, appCheck, result) {

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

	return this->definitionRef;
}

void ExtendedDefinition::SetDefinitionRef(Definition* definitionRef) {

	this->definitionRef = definitionRef;
}

void ExtendedDefinition::Write(DOMElement* parentElm) {

	// get the parent document
	XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* resultDoc = parentElm->getOwnerDocument();

	// create a new criteria element
	DOMElement* extendedDefinitionElm = XmlCommon::AddChildElement(resultDoc, parentElm, "extend_definition");

	// add the attributes
	XmlCommon::AddAttribute(extendedDefinitionElm, "definition_ref", this->GetDefinitionRef()->GetId());

	XmlCommon::AddAttribute(extendedDefinitionElm, "version", Common::ToString(this->GetDefinitionRef()->GetVersion()));

	if (this->GetApplicabilityCheck() == APPLICABILITY_CHECK_TRUE)
		XmlCommon::AddAttribute(extendedDefinitionElm, "applicability_check", "true");
	else if (this->GetApplicabilityCheck() == APPLICABILITY_CHECK_FALSE)
		XmlCommon::AddAttribute(extendedDefinitionElm, "applicability_check", "false");
	// else, leave the attribute off

	if(this->GetDefinitionRef()->GetVariableInstance() != 1) {
		XmlCommon::AddAttribute(extendedDefinitionElm, "variable_instance", Common::ToString(this->GetDefinitionRef()->GetVariableInstance()));
	}

	if(this->GetNegate()) {
		XmlCommon::AddAttribute(extendedDefinitionElm, "negate", Common::ToString(this->GetNegate()));
	}

	XmlCommon::AddAttribute(extendedDefinitionElm, "result", OvalEnum::ResultToString(this->GetResult()));

	// write the definition ref
	this->GetDefinitionRef()->Write(Analyzer::GetResultsSystemDefinitionsElm());
}

void ExtendedDefinition::Parse(DOMElement* extendedDefinitionElm) {

	// get the attributes from the extended definition element
	string negateStr = XmlCommon::GetAttributeByName(extendedDefinitionElm, "negate");
	if(negateStr.compare("") == 0 || negateStr.compare("false") == 0) {
        this->SetNegate(false);
	} else {
	    this->SetNegate(true);
	}

	string appCheckStr = XmlCommon::GetAttributeByName(extendedDefinitionElm,
		"applicability_check");
	if (appCheckStr.empty())
		this->SetApplicabilityCheck(APPLICABILITY_CHECK_UNKNOWN);
	else {
		bool appCheckBool;
		if (!Common::FromString(appCheckStr, &appCheckBool))
			throw Exception("Can't interpret "+appCheckStr+" as an xsd:boolean!");
		this->SetApplicabilityCheck(appCheckBool ? APPLICABILITY_CHECK_TRUE :
			APPLICABILITY_CHECK_FALSE);
	}

	// get the definition ref
	string definitionRefStr = XmlCommon::GetAttributeByName(extendedDefinitionElm, "definition_ref");
	Definition* defRef = Definition::GetDefinitionById(definitionRefStr);
	this->SetDefinitionRef(defRef);
}

OvalEnum::ResultEnumeration ExtendedDefinition::Analyze() {

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

