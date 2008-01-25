//
// $Id: Set.cpp 4599 2008-01-03 16:47:09Z bakerj $
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

#include "Set.h"

//****************************************************************************************//
//									Set Class											  //	
//****************************************************************************************//
Set::Set(DOMElement* setElm) {

	this->SetReferenceOne(NULL);
	this->SetReferenceTwo(NULL);
	this->SetSetOne(NULL);
	this->SetSetTwo(NULL);
	this->SetIsSimpleSet(true);
	this->Parse(setElm);
}

Set::Set() {

	this->SetReferenceOne(NULL);
	this->SetReferenceTwo(NULL);
	this->SetSetOne(NULL);
	this->SetSetTwo(NULL);
	this->SetIsSimpleSet(true);
}

Set::~Set() {

	Filter* filter = NULL;
	while(filters.size() != 0) {
	  	filter = (Filter*)filters[filters.size()-1];
	  	filters.pop_back();

		// JB - Removed because filters are now cached. 
		// The cache is managed by the Filter class.
	  	//delete filter;
	  	//filter = NULL;
	}

	if(this->referenceOne != NULL) {
		delete(this->referenceOne);
	}
	if(this->referenceTwo != NULL) {
        delete(this->referenceTwo);
	}

	if(this->setOne != NULL) {
        delete(this->setOne);
	}
	if(this->setTwo != NULL){
		delete(this->setTwo);
	}
}

// ***************************************************************************************	//
//								 Public members												//
// ***************************************************************************************	//
AbsStateVector* Set::GetFilters() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the filters field's value
	//
	// -----------------------------------------------------------------------

	return &this->filters;
}

void Set::SetFilters(AbsStateVector* filters) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the filters field's value
	//
	// -----------------------------------------------------------------------

	this->filters = (*filters);
}

bool Set::GetIsSimpleSet() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Get the isSimpleSet field's value
	//
	// -----------------------------------------------------------------------

	return this->isSimpleSet;
}


void Set::SetIsSimpleSet(bool isSimpleSet) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the isSimpleSet field's value
	//
	// -----------------------------------------------------------------------

	this->isSimpleSet = isSimpleSet;	
}

AbsObject* Set::GetReferenceOne() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the referenceOne field's value
	//
	// -----------------------------------------------------------------------

	return this->referenceOne;
}

void Set::SetReferenceOne(AbsObject* object) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the referenceOne field's value
	//
	// -----------------------------------------------------------------------

	this->referenceOne = object;
}

AbsObject* Set::GetReferenceTwo() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the referenceTwo field's value
	//
	// -----------------------------------------------------------------------

	return this->referenceTwo;
}

void Set::SetReferenceTwo(AbsObject* object) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the referenceTwo field's value
	//
	// -----------------------------------------------------------------------

	this->referenceTwo = object;
}

Set* Set::GetSetOne() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the setOne field's value
	//
	// -----------------------------------------------------------------------

	return this->setOne;
}

void Set::SetSetOne(Set* set) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the setOne field's value
	//
	// -----------------------------------------------------------------------

	this->setOne = set;
}

Set* Set::GetSetTwo() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the setTwo field's value
	//
	// -----------------------------------------------------------------------

	return this->setTwo;
}

void Set::SetSetTwo(Set* set) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the setTwo field's value
	//
	// -----------------------------------------------------------------------

	this->setTwo = set;
}

OvalEnum::SetOperator Set::GetSetOperator() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the setOperator field's value
	//
	// -----------------------------------------------------------------------

	return this->setOperator;
}

void Set::SetSetOperator(OvalEnum::SetOperator setOperator) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the setOperator field's value
	//
	// -----------------------------------------------------------------------

	this->setOperator = setOperator;
}

void Set::AppendFilter(Filter* filter) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Add a filter to the end of the filters vector
	//
	// -----------------------------------------------------------------------

	this->filters.push_back(filter);
}

VariableValueVector* Set::GetVariableValues() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return a vector of all variable values used for this set
	//
	// -----------------------------------------------------------------------

	VariableValueVector* varValues = new VariableValueVector();
	VariableValue* varValue = NULL;
    
	if(this->GetIsSimpleSet()) {
		// get the variable values used in each filter 
		AbsStateVector::iterator iterator;
		for(iterator = this->GetFilters()->end(); iterator != this->GetFilters()->end(); iterator++) {
			Filter* filter = (Filter*)(*iterator);
			VariableValueVector* filterVarValues = filter->GetVariableValues();
			// copy the state's var values to the set's vector of var values
			VariableValueVector::iterator vit;
			for(vit = filterVarValues->end(); vit != filterVarValues->end(); vit++) {
				varValue = (*vit);
				varValues->push_back(varValue);
			}
			
			delete filterVarValues;
			filterVarValues = NULL;
		}

		// get the variable values used by reference one if it exists
		VariableValueVector* refVarValues = NULL;
		VariableValueVector::iterator rit;
		if(this->GetReferenceOne() != NULL) {
			refVarValues = this->GetReferenceOne()->GetVariableValues();
			// copy the reference's var values to the set's vector of var values
			for(rit = refVarValues->end(); rit != refVarValues->end(); rit++) {
				varValue = (*rit);
				varValues->push_back(varValue);
			}
			
			delete refVarValues;
			refVarValues = NULL;
		}

		// get the variable values used by reference 2 if it exists
		if(this->GetReferenceTwo() != NULL) {
			refVarValues = this->GetReferenceTwo()->GetVariableValues();
			// copy the reference's var values to the set's vector of var values
			for(rit = refVarValues->end(); rit != refVarValues->end(); rit++) {
				varValue = (*rit);
				varValues->push_back(varValue);
			}
			
			delete refVarValues;
			refVarValues = NULL;
		}

	} else {

		// Get the variable values used by set one if it exists
		VariableValueVector* setVarValues = NULL;
		VariableValueVector::iterator sit;
		if(this->GetSetOne() != NULL) {
			setVarValues = this->GetSetOne()->GetVariableValues();
			// copy the child set's var values to the set's vector of var values
			for(sit = setVarValues->end(); sit != setVarValues->end(); sit++) {
				varValue = (*sit);
				varValues->push_back(varValue);
			}
			
			delete setVarValues;
			setVarValues = NULL;
		}

		// Get the variable values used by set two if it exists
		if(this->GetSetTwo() != NULL) {
			setVarValues = this->GetSetTwo()->GetVariableValues();
			// copy the child set's var values to the set's vector of var values
			for(sit = setVarValues->end(); sit != setVarValues->end(); sit++) {
				varValue = (*sit);
				varValues->push_back(varValue);
			}
			
			delete setVarValues;
			setVarValues = NULL;
		}
	}

	return varValues;
}

void Set::Parse(DOMElement* setObjectElm) {

	string setOperatorStr = XmlCommon::GetAttributeByName(setObjectElm, "set_operator");
	this->SetSetOperator(OvalEnum::ToSetOperator(setOperatorStr));

	// loop over all child elements
	DOMNodeList *setObjectChildren = setObjectElm->getChildNodes();
	unsigned int index = 0;
	while(index < setObjectChildren->getLength()) {
		DOMNode *tmpNode = setObjectChildren->item(index);

		//	only concerned with ELEMENT_NODEs
		if (tmpNode->getNodeType() == DOMNode::ELEMENT_NODE) {
			DOMElement *setChild = (DOMElement*)tmpNode;

			//	get the name of the child
			string childName = XmlCommon::GetElementName(setChild);
			if(childName.compare("set") == 0) {

				this->SetIsSimpleSet(false);

				// create a new set object based on this element
				if(this->GetSetOne() == NULL) {
					Set* newSetObj = new Set(setChild);
					//newSetObj->SetIsSimpleSet(false);
					this->SetSetOne(newSetObj);
				} else {
					Set* newSetObj = new Set(setChild);
					//newSetObj->SetIsSimpleSet(false);
					this->SetSetTwo(newSetObj);
				}

			} else if(childName.compare("object_reference") == 0) {
				this->SetIsSimpleSet(true);
				string objId = XmlCommon::GetDataNodeValue(setChild);
				AbsObject* tmpObj = ObjectFactory::GetObjectById(objId);
				if(this->GetReferenceOne() == NULL) {
					this->SetReferenceOne(tmpObj);
				} else {
					this->SetReferenceTwo(tmpObj);
				}
			} else if(childName.compare("filter") == 0) {
				this->SetIsSimpleSet(true);
				string stateId = XmlCommon::GetDataNodeValue(setChild);


				Filter* tmpFilter = Filter::GetFilter(stateId);
				//Filter* tmpFilter = Filter::SearchCache();
				//if(tmpFilter == NULL) {
				//	tmpFilter = new Filter(stateId);
				//	Filter::Cache(tmpFilter);
				//}
				this->AppendFilter(tmpFilter);
			}
		}

		index ++;
	}
}

//****************************************************************************************//
//								SetException Class										  //	
//****************************************************************************************//
SetException::SetException(string errMsgIn, int severity, Exception* ex) : Exception(errMsgIn, severity, ex) {

}

SetException::~SetException() {

}

