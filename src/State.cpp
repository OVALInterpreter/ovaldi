//
// $Id: State.cpp 4579 2008-01-02 17:39:07Z bakerj $
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

#include "State.h"

AbsStateMap State::processedStatesMap;
//****************************************************************************************//
//									State Class											  //	
//****************************************************************************************//
State::State(string id) : AbsState() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Create a complete State object
	//
	// -----------------------------------------------------------------------

	// get the specified state element
	DOMElement* statesElm = XmlCommon::FindElement(DocumentManager::GetDefinitionDocument(), "states");
	DOMElement* stateElm = XmlCommon::FindElementByAttribute(statesElm, "id", id);
	this->Parse(stateElm);
}

State::State(OvalEnum::Operator myOperator, int version) : AbsState(myOperator, version) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Create a complete State object
	//
	// -----------------------------------------------------------------------
}

State::State(string id, string name, string xmlns, OvalEnum::Operator myOperator, int version) : AbsState(id, name, xmlns, myOperator, version) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Create a complete State object
	//
	// -----------------------------------------------------------------------
}

State::~State() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	AbsState class handles deleting all elements
	// -----------------------------------------------------------------------
}

// ***************************************************************************************	//
//								 Public members												//
// ***************************************************************************************	//

OvalEnum::ResultEnumeration State::Analyze(Item* item) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Analyze the specified Item return the Result value for the Item.
	//
	//	1 - create a vector of Item elements that match each element in the state.
	//	2 - pass the vector to the StateEntity analyze method
	//	3 - build a vector of results for each element in the state.
	//	4 - combine the results to a single value based on the states operator
    // -----------------------------------------------------------------------

	// Check the status of the Item
	if(item->GetStatus() == OvalEnum::STATUS_ERROR) {
		return OvalEnum::RESULT_ERROR;
	} else if(item->GetStatus() == OvalEnum::STATUS_NOT_COLLECTED) {
		return OvalEnum::RESULT_ERROR;
	} else if(item->GetStatus() == OvalEnum::STATUS_DOES_NOT_EXIST) {
		return OvalEnum::RESULT_FALSE;
	}

	// check data before analysis
	if(this->GetElements()->size() == 0) {
		return OvalEnum::RESULT_TRUE;
	}

	// vector of result values before the state operator is applied
	IntVector stateResults;

	// Loop through all elements in the state
	AbsEntityVector::iterator stateElements;
	for(stateElements = this->GetElements()->begin(); stateElements != this->GetElements()->end(); stateElements++) {
		StateEntity* stateElm = (StateEntity*)(*stateElements);

		// locate matching elements in the item
		string stateElmName = stateElm->GetName(); 
		ItemEntityVector* scElements = item->GetElementsByName(stateElmName);

		// Analyze each matching element
		ItemEntityVector::iterator scIterator;
		IntVector stateElmResults;
		for(scIterator = scElements->begin(); scIterator != scElements->end(); scIterator++) {
			ItemEntity* scElm = (ItemEntity*)(*scIterator);
			// call StateEntity->analyze method
			stateElmResults.push_back(stateElm->Analyze(scElm));
		}

		// compute the overall state result
		OvalEnum::ResultEnumeration stateResult = OvalEnum::RESULT_UNKNOWN; // default to unknown;
		if(stateElmResults.size() > 0) {
			stateResult = OvalEnum::CombineResultsByCheck(&stateElmResults, stateElm->GetEntityCheck());
		}
		// store the result for the current state element
		stateResults.push_back(stateResult);
	}
	
	OvalEnum::ResultEnumeration overallResult = OvalEnum::CombineResultsByOperator(&stateResults, this->GetOperator());

	return overallResult;
}

void State::Parse(DOMElement* stateElm) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Parse the provided state from a definition file into an State object
	//
	// -----------------------------------------------------------------------

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
                StateEntity* stateEntity = new StateEntity();
				stateEntity->Parse(stateChild);
				this->AppendElement(stateEntity);
			}
		}
	}
	State::processedStatesMap.insert(AbsStatePair(this->GetId(), this));
}

State* State::SearchCache(string id) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	search the cache of Tests for the specifed Test. 
	//	return NULL if not found
	//
	// -----------------------------------------------------------------------

	AbsState* cachedState = NULL;

	//AbsStateVector::iterator iterator;
	//for(iterator = State::processedStates.begin(); iterator != State::processedStates.end(); iterator++) {
	//	if((*iterator)->GetId().compare(id) == 0) {
	//		cachedState = (*iterator);
	//		break;
	//	}
	//}

	AbsStateMap::iterator iterator;
	iterator = State::processedStatesMap.find(id);
	if(iterator != State::processedStatesMap.end()) {
		cachedState = iterator->second;
	} 

	return (State*)cachedState;
}

void State::ClearCache() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	delete all items in the cache
	//
	// -----------------------------------------------------------------------

	AbsStateMap::iterator iterator;
	for(iterator = State::processedStatesMap.begin(); iterator != State::processedStatesMap.end(); iterator++) {
		AbsState* state = iterator->second;
		delete state;
	}
	
	State::processedStatesMap.clear();
}

void State::Cache(State* state) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	cache the specified state
	//	TODO - do i need to add protection to this cache? 
	// -----------------------------------------------------------------------
	
	State::processedStatesMap.insert(AbsStatePair(state->GetId(), state));
}
