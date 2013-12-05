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

#include "Criteria.h"

using namespace std;

//****************************************************************************************//
//								AbsCriteria Class										  //	
//****************************************************************************************//
Criteria::Criteria(bool negate, ApplicabilityCheck appCheck,
		OvalEnum::ResultEnumeration result, std::string comment,
		OvalEnum::Operator op)
					: AbsCriteria(negate, appCheck, result) {
//Criteria::Criteria(bool negate, OvalEnum::ResultEnumeration result, string comment, OvalEnum::Operator op) 
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Create a complete Criteria object
	//
	// -----------------------------------------------------------------------

	this->SetOperator(op);
}

Criteria::~Criteria() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Delete all child criteria
	//
	// -----------------------------------------------------------------------

	AbsCriteria* current = NULL;
	while(this->childCriteria.size() != 0) {
		current = this->childCriteria[this->childCriteria.size()-1];
	  	this->childCriteria.pop_back();
	  	delete current;
	  	current = NULL;
	}
}

// ***************************************************************************************	//
//								 Public members												//
// ***************************************************************************************	//
OvalEnum::Operator Criteria::GetOperator() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the operator field's value
	//
	// -----------------------------------------------------------------------

	return this->op;
}

void Criteria::SetOperator(OvalEnum::Operator op) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the operator field's value
	//
	// -----------------------------------------------------------------------

	this->op = op;
}

AbsCriteriaVector* Criteria::GetChildCriteria() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the childCriteria field's value
	//
	// -----------------------------------------------------------------------

	return &this->childCriteria;
}

void Criteria::SetChildCriteria(AbsCriteriaVector* childCriteria) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the childCriteria field's value
	//
	// -----------------------------------------------------------------------

	this->childCriteria = (*childCriteria);
}

void Criteria::AppendChildCriteria(AbsCriteria* childCriteria) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the childCriteria field's value
	//
	// -----------------------------------------------------------------------

	this->childCriteria.push_back(childCriteria);
}

void Criteria::Write(DOMElement* parentElm) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	writes valid criteria to result file
	//	calls AbsCritiera->Write() on all child criteria
	//
	// -----------------------------------------------------------------------

	// get the parent document
	XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* resultDoc = parentElm->getOwnerDocument();

	// create a new criteria element
	DOMElement* criteriaElm = XmlCommon::AddChildElement(resultDoc, parentElm, "criteria");

	// add the attributes
	XmlCommon::AddAttribute(criteriaElm, "operator", OvalEnum::OperatorToString(this->GetOperator()));
	
	if(this->GetNegate()) {
		XmlCommon::AddAttribute(criteriaElm, "negate", Common::ToString(this->GetNegate()));
	}	

	XmlCommon::AddAttribute(criteriaElm, "result", OvalEnum::ResultToString(this->GetResult()));
	
	if (this->GetApplicabilityCheck() == APPLICABILITY_CHECK_TRUE)
		XmlCommon::AddAttribute(criteriaElm, "applicability_check", "true");
	else if (this->GetApplicabilityCheck() == APPLICABILITY_CHECK_FALSE)
		XmlCommon::AddAttribute(criteriaElm, "applicability_check", "false");
	// else, leave the attribute off

	// loop through all childCriteria and call write method
	AbsCriteriaVector::iterator iterator;
	for(iterator = this->GetChildCriteria()->begin(); iterator != this->GetChildCriteria()->end(); iterator++) {
		(*iterator)->Write(criteriaElm);
	}
}

void Criteria::Parse(DOMElement* criteriaElm) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	parses criteria elm to a criteria obj
	//	calls AbsCriteria->Parse() on all child elements
	//
	// -----------------------------------------------------------------------

	// get the attributes
	string negateStr = XmlCommon::GetAttributeByName(criteriaElm, "negate");
	if(negateStr.compare("") == 0 || negateStr.compare("false") == 0) {
        this->SetNegate(false);
	} else {
	    this->SetNegate(true);
	}
	
	this->SetOperator(OvalEnum::ToOperator(XmlCommon::GetAttributeByName(criteriaElm, "operator")));

	string appCheckStr = XmlCommon::GetAttributeByName(criteriaElm,
		"applicability_check");
	if (appCheckStr.empty())
		// actually a little fib... I think the value is deducible if
		// not explicitly given, according to rules I've proposed, but
		// I don't think anyone wants me to try to deduce it. :-P
		this->SetApplicabilityCheck(APPLICABILITY_CHECK_UNKNOWN);
	else {
		bool appCheckBool;
		if (!Common::FromString(appCheckStr, &appCheckBool))
			throw Exception("Can't interpret "+appCheckStr+" as an xsd:boolean!");
		this->SetApplicabilityCheck(appCheckBool ? APPLICABILITY_CHECK_TRUE :
			APPLICABILITY_CHECK_FALSE);
	}

	// loop over all child elements and call AbsCriteria-.Parse
	DOMNodeList *criteriaChildren = criteriaElm->getChildNodes();
	unsigned int index = 0;
	while(index < criteriaChildren->getLength()) {
		DOMNode *tmpNode = criteriaChildren->item(index);

		//	only concerned with ELEMENT_NODEs
		if (tmpNode->getNodeType() == DOMNode::ELEMENT_NODE) {
			DOMElement *criteriaChildElm = (DOMElement*)tmpNode;
			
			AbsCriteria* childCriteria = NULL;

			//	get the name of the child and construct the appropriate criteria type
			string childName = XmlCommon::GetElementName(criteriaChildElm);
			if(childName.compare("extend_definition") == 0) {
				childCriteria = new ExtendedDefinition();
			} else if(childName.compare("criterion") == 0) {
				childCriteria = new Criterion();
			} else if(childName.compare("criteria") == 0) {
				childCriteria = new Criteria();
			}
			childCriteria->Parse(criteriaChildElm);
			this->AppendChildCriteria(childCriteria);
		}
		index ++;
	}
}

OvalEnum::ResultEnumeration Criteria::Analyze() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	calls AbsCriteria->Analyze() on all child criteria
	//	applies operator 
	//	applies negate attribute
	//	saves and returns result
	// -----------------------------------------------------------------------

	IntVector results;

	// loop through all childCriteria and call write method
	AbsCriteriaVector::iterator iterator;
	for(iterator = this->GetChildCriteria()->begin(); iterator != this->GetChildCriteria()->end(); iterator++) {
		results.push_back((*iterator)->Analyze());
	}

	// apply the operator
	OvalEnum::ResultEnumeration currentResult = OvalEnum::CombineResultsByOperator(&results, this->GetOperator());

	// apply the negate attribute and save the result
	if(this->GetNegate()) {
		this->SetResult(OvalEnum::NegateResult(currentResult));
	} else {
		this->SetResult(currentResult);
	}

	return this->GetResult();
}

OvalEnum::ResultEnumeration Criteria::NotEvaluated() {

	IntVector results;

	// loop through all childCriteria and call write method
	AbsCriteriaVector::iterator iterator;
	for(iterator = this->GetChildCriteria()->begin(); iterator != this->GetChildCriteria()->end(); iterator++) {
		results.push_back((*iterator)->NotEvaluated());
	}

	// apply the operator
	OvalEnum::ResultEnumeration currentResult = OvalEnum::CombineResultsByOperator(&results, this->GetOperator());

	// apply the negate attribute and save the result
	if(this->GetNegate()) {
		this->SetResult(OvalEnum::NegateResult(currentResult));
	} else {
		this->SetResult(currentResult);
	}

	return this->GetResult();
}
