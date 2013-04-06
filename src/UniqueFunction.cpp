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

#include <memory>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMNodeList.hpp>

#include "Common.h"
#include "ComponentFactory.h"
#include "UniqueFunction.h"

using namespace std;
using namespace xercesc;

//****************************************************************************************//
//								Component Class											  //	
//****************************************************************************************//
UniqueFunction::UniqueFunction() : AbsFunctionComponent() {
}

UniqueFunction::~UniqueFunction() {
}

// ***************************************************************************************	//
//								 Public members												//
// ***************************************************************************************	//
ComponentValue* UniqueFunction::ComputeValue() {

	AbsComponentVector *components = GetComponents();
	auto_ptr<ComponentValue> newValue(new ComponentValue());
	IntVector flags;
	StringSet valueSet; // for uniquefying values

	for (AbsComponentVector::iterator iter = components->begin();
		iter != components->end();
		++iter) {

		auto_ptr<ComponentValue> subComponentValue((*iter)->ComputeValue());
		valueSet.insert(subComponentValue->GetValues()->begin(),
			subComponentValue->GetValues()->end());

		flags.push_back(subComponentValue->GetFlag());
		newValue->AppendMessages(subComponentValue->GetMessages());
	}

	newValue->SetValues(new StringVector(valueSet.begin(), valueSet.end()));
	newValue->SetFlag(OvalEnum::CombineFlags(&flags));

	return newValue.release();
}

void UniqueFunction::Parse(DOMElement* componentElm) {
    
	// Loop through all child elements
	DOMNodeList *componentElms = componentElm->getChildNodes();
	unsigned int index = 0;
	while(index < componentElms->getLength()) {
		DOMNode *tmpNode = componentElms->item(index);
		if (tmpNode->getNodeType() == DOMNode::ELEMENT_NODE) {
			DOMElement *childElm = (DOMElement*)tmpNode;

			// Call the ComponentFactory
			AbsComponent* absComponent = ComponentFactory::GetComponent(childElm);
			// store the returned component
			this->AppendComponent(absComponent);
		}
		index ++;
	}
}

VariableValueVector* UniqueFunction::GetVariableValues() {
	
	VariableValueVector* values = new VariableValueVector();
	AbsComponentVector* components = this->GetComponents();
	AbsComponentVector::iterator iterator;
	for(iterator = components->begin(); iterator != components->end(); iterator++) {
		AbsComponent* component = (AbsComponent*)(*iterator);
		VariableValueVector* tmp = component->GetVariableValues();
		VariableValueVector::iterator varIterator;
		for(varIterator = tmp->begin(); varIterator != tmp->end(); varIterator++) {
			values->push_back((*varIterator));
		}
		// BUG - These can not currenrtly be deleted. 
		// The code is not consistant here. In places a new vector is returned
		// in others a reference to a vector that is managed by other code is returned.
		//delete tmp;
		//tmp = NULL;
	}

	return values;
}
