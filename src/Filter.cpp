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

#include "Filter.h"

AbsStateMap Filter::processedFiltersMap;

//****************************************************************************************//
//									Filter Class										  //	
//****************************************************************************************//
Filter::Filter(string id) : AbsState(), excluding(true) {

	// get the specified state element
	DOMElement* statesElm = XmlCommon::FindElementNS(DocumentManager::GetDefinitionDocument(), "states");
	DOMElement* stateElm = XmlCommon::FindElementByAttribute(statesElm, "id", id);
	this->Parse(stateElm);
}

Filter::Filter(OvalEnum::Operator myOperator, bool excluding, int version) : AbsState(myOperator, version), excluding(excluding)  {

}

Filter::Filter(string id, string name, string xmlns, OvalEnum::Operator myOperator, bool excluding, int version) : AbsState(id, name, xmlns, myOperator, version), excluding(excluding) {

}

Filter::~Filter() {

}

// ***************************************************************************************	//
//								 Public static members										//
// ***************************************************************************************	//
Filter* Filter::GetFilter(string stateId) {
	
	Filter* tmpFilter = Filter::SearchCache(stateId);
	if(tmpFilter == NULL) {
		tmpFilter = new Filter(stateId);
		Filter::Cache(tmpFilter);
	}
	return tmpFilter;
}

// ***************************************************************************************	//
//								 Public members												//
// ***************************************************************************************	//
bool Filter::Analyze(Item* item) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Analyze the specified Item return the Result value for the Item.
	//
	//	1 - create a vector of Item elements that match each element in the state.
	//	2 - pass the vector to the AbsEntity analyze method
	//	3 - build a vector of results for each element in the state.
	//	4 - combine the results to a single value based on the states operator
    // -----------------------------------------------------------------------

	OvalEnum::ResultEnumeration overallResult = OvalEnum::RESULT_ERROR;

	// Check the status of the Item
	if(item->GetStatus() == OvalEnum::STATUS_ERROR) {
		overallResult = OvalEnum::RESULT_ERROR;
	} else if(item->GetStatus() == OvalEnum::STATUS_NOT_COLLECTED) {
		overallResult = OvalEnum::RESULT_ERROR;
	} else if(item->GetStatus() == OvalEnum::STATUS_DOES_NOT_EXIST) {
		overallResult = OvalEnum::RESULT_FALSE;
	} else {

		// check data before analysis
		if(this->GetElements()->size() == 0) {
			overallResult = OvalEnum::RESULT_TRUE;
		} else {

			// vector of result values before the state operator is applied
			IntVector filterResults;

			// Loop through all elements in the state
			AbsEntityVector::iterator iterator;
			for(iterator = this->GetElements()->begin(); iterator != this->GetElements()->end(); iterator++) {
				FilterEntity* filterElm = (FilterEntity*)(*iterator);

				// locate matching elements in the item
				string filterElmName = filterElm->GetName(); 
				ItemEntityVector* scElements = item->GetElementsByName(filterElmName);

				// Analyze each matching element
				ItemEntityVector::iterator scIterator;
				IntVector filterElmResults;
				for(scIterator = scElements->begin(); scIterator != scElements->end(); scIterator++) {
					ItemEntity* scElm = (ItemEntity*)(*scIterator);
					// call FilterEntity->analyze method
					filterElmResults.push_back(filterElm->Analyze(scElm));
				}
				scElements->clear();
				delete scElements;

				// compute the overall filter result
				OvalEnum::ResultEnumeration filterResult = OvalEnum::RESULT_UNKNOWN; // default to unknown;
				if(filterElmResults.size() > 0) {
					filterResult = OvalEnum::CombineResultsByCheck(&filterElmResults, filterElm->GetEntityCheck());
				}
				// store the result for the current state element
				filterResults.push_back(filterResult);

				overallResult = OvalEnum::CombineResultsByOperator(&filterResults, this->GetOperator());
			}
		}
	}

	// for a filter really want to convert the result to a boolean
	bool isMatch = false;
	if(overallResult == OvalEnum::RESULT_TRUE) {
		isMatch = true;
	} else if(overallResult == OvalEnum::RESULT_FALSE) {
		isMatch = false;	
	} else {
		throw AbsStateException("Filter::ApplyFilter method unable to convert result value to a boolean. Found result: " + OvalEnum::ResultToString(overallResult));
	}
	return isMatch;
}

void Filter::Parse(DOMElement* stateElm) {

	this->SetName(XmlCommon::GetElementName(stateElm));
	this->SetId(XmlCommon::GetAttributeByName(stateElm, "id"));
	this->SetXmlns(XmlCommon::GetNamespace(stateElm));
	string versionStr = XmlCommon::GetAttributeByName(stateElm, "version");
	int version;
	if(versionStr.compare("") == 0) {
		version = 1;
	} else {
		version = atoi(versionStr.c_str());
	}
	this->SetVersion(version);

	// loop over all elements
	DOMNodeList *stateChildren = stateElm->getChildNodes();
	unsigned int index = 0;
	while(index < stateChildren->getLength()) {
		DOMNode *tmpNode = stateChildren->item(index++);

		//	only concerned with ELEMENT_NODEs
		if (tmpNode->getNodeType() == DOMNode::ELEMENT_NODE) {
			DOMElement *stateChild = (DOMElement*)tmpNode;

			//	get the name of the child
			string childName = XmlCommon::GetElementName(stateChild);
			if(childName.compare("notes") == 0) {
				continue;
			} else {
                FilterEntity* filterEntity = new FilterEntity();
				filterEntity->Parse(stateChild);
				this->AppendElement(filterEntity);
			}
		}
	}
}

Filter* Filter::SearchCache(string id) {

	AbsState* cachedFilter = NULL;

	AbsStateMap::iterator iterator;
	iterator = Filter::processedFiltersMap.find(id);
	if(iterator != Filter::processedFiltersMap.end()) {
		cachedFilter = iterator->second;
	} 

	return (Filter*)cachedFilter;
}

void Filter::ClearCache() {

	AbsStateMap::iterator iterator;
	for(iterator = Filter::processedFiltersMap.begin(); iterator != Filter::processedFiltersMap.end(); iterator++) {
		AbsState* state = iterator->second;
		delete state;
	}
	
	Filter::processedFiltersMap.clear();
}

void Filter::Cache(Filter* filter) {

	//	TODO - do i need to add protection to this cache

	Filter::processedFiltersMap.insert(AbsStatePair(filter->GetId(), filter));
}
