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
//****************************************************************************************//using namespace std;

#include <memory>
#include "State.h"
#include "StateEntity.h"

using namespace std;

AbsStateMap State::processedStatesMap;

//****************************************************************************************//
//									State Class											  //	
//****************************************************************************************//

State::State(OvalEnum::Operator myOperator, int version) : AbsState(myOperator, version) {

}

State::~State() {

}

// ***************************************************************************************	//
//								 Public members												//
// ***************************************************************************************	//

OvalEnum::ResultEnumeration State::Analyze(Item* item) {

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
		// i think the vector needs deleting, but the item retains ownership
		// of the vector's contents.
		auto_ptr<ItemEntityVector> scElements(item->GetElementsByName(stateElmName));
		IntVector stateElmResults;

		if (scElements->empty())
			Log::Debug("Warning: can't find match in item, for state entity named: \""+stateElmName+"\"");
		else {

			// Analyze each matching element
			ItemEntityVector::iterator scIterator;
			for(scIterator = scElements->begin(); scIterator != scElements->end(); scIterator++) {
				ItemEntity* scElm = (ItemEntity*)(*scIterator);
				// call StateEntity->analyze method
				stateElmResults.push_back(stateElm->Analyze(scElm));
			}
		}

		// compute the overall state result
		OvalEnum::ResultEnumeration stateResult = OvalEnum::CombineResultsByCheck(&stateElmResults, stateElm->GetEntityCheck());

		// store the result for the current state element
		stateResults.push_back(stateResult);
	}
	
	OvalEnum::ResultEnumeration overallResult = OvalEnum::CombineResultsByOperator(&stateResults, this->GetOperator());

	return overallResult;
}

void State::Parse(DOMElement* stateElm) {

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

	// if the operator attribute is specified let's set it. otherwise we will use the default operator set in the constructor.
        string operatorStr = XmlCommon::GetAttributeByName(stateElm, "operator");
        if (operatorStr.compare("") != 0){
          this->SetOperator(OvalEnum::ToOperator(operatorStr));
        }

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

	State::Cache(this);
}

State* State::SearchCache(string id) {

	AbsState* cachedState = NULL;

	AbsStateMap::iterator iterator;
	iterator = State::processedStatesMap.find(id);
	if(iterator != State::processedStatesMap.end()) {
		cachedState = iterator->second;
	} 

	return (State*)cachedState;
}

void State::ClearCache() {

	AbsStateMap::iterator iterator;
	for(iterator = State::processedStatesMap.begin(); iterator != State::processedStatesMap.end(); iterator++) {
		AbsState* state = iterator->second;
		delete state;
	}
	
	State::processedStatesMap.clear();
}

void State::Cache(State* state) {

	State::processedStatesMap.insert(AbsStatePair(state->GetId(), state));
}

State* State::GetStateById(string stateId) {

	State* state = NULL;
	
	// Search the cache
	state = State::SearchCache(stateId);

	// if not found try to parse it.
	if(state == NULL) {

		DOMElement* statesElm = XmlCommon::FindElementNS(DocumentManager::GetDefinitionDocument(), "states");
		DOMElement* stateElm = XmlCommon::FindElementByAttribute(statesElm, "id", stateId);

		if(stateElm == NULL) {
			throw Exception("Unable to find specified state in oval-definition document. State id: " + stateId);
		}

		state = new State();
		state->Parse(stateElm);
	}
	
	return state;
}
