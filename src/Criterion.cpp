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

#include "Criterion.h"

using namespace std;

//****************************************************************************************//
//								Criterion Class											  //	
//****************************************************************************************//
Criterion::Criterion(bool negate, OvalEnum::ResultEnumeration result, Test* testRef) 
					: AbsCriteria(negate, result) {

	this->SetTestRef(testRef);
}

Criterion::~Criterion() {

}

// ***************************************************************************************	//
//								 Public members												//
// ***************************************************************************************	//

Test* Criterion::GetTestRef() {

	return this->testRef;
}

void Criterion::SetTestRef(Test* testRef) {

	this->testRef = testRef;
}

void Criterion::Write(DOMElement* parentElm) {

	// get the parent document
	XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* resultDoc = parentElm->getOwnerDocument();

	// create a new criteria element
	DOMElement* criterionElm = XmlCommon::AddChildElement(resultDoc, parentElm, "criterion");

	// add the attributes
	XmlCommon::AddAttribute(criterionElm, "test_ref", this->GetTestRef()->GetId());

	XmlCommon::AddAttribute(criterionElm, "version", Common::ToString(this->GetTestRef()->GetVersion()));

	if(this->GetTestRef()->GetVariableInstance() != 1) {
		XmlCommon::AddAttribute(criterionElm, "variable_instance", Common::ToString(this->GetTestRef()->GetVariableInstance()));
	}

	if (this->GetNegate()){
		XmlCommon::AddAttribute(criterionElm, "negate", Common::ToString(this->GetNegate()));
	}

	XmlCommon::AddAttribute(criterionElm, "result", OvalEnum::ResultToString(this->GetResult()));

	// write the test ref
	this->GetTestRef()->Write(Analyzer::GetResultsSystemTestsElm());
}

void Criterion::Parse(DOMElement* criterionElm) {
	
	// get the attributes
	string negateStr = XmlCommon::GetAttributeByName(criterionElm, "negate");
	if(negateStr.compare("") == 0 || negateStr.compare("false") == 0) {
        this->SetNegate(false);
	} else {
	    this->SetNegate(true);
	}

	string testRefStr = XmlCommon::GetAttributeByName(criterionElm, "test_ref");
	Test* testRef = Test::GetTestById(testRefStr);
	
	this->SetTestRef(testRef);
}

OvalEnum::ResultEnumeration Criterion::Analyze() {

	// analyze the test
	OvalEnum::ResultEnumeration currentResult = this->GetTestRef()->Analyze();

	// apply the negate attribute and save the result
	if(this->GetNegate()) {
		this->SetResult(OvalEnum::NegateResult(currentResult));
	} else {
		this->SetResult(currentResult);
	}

	return this->GetResult();
}

OvalEnum::ResultEnumeration Criterion::NotEvaluated() {

	// analyze the test
	OvalEnum::ResultEnumeration currentResult = this->GetTestRef()->NotEvaluated();

	// apply the negate attribute and save the result
	if(this->GetNegate()) {
		this->SetResult(OvalEnum::NegateResult(currentResult));
	} else {
		this->SetResult(currentResult);
	}

	return this->GetResult();
}
