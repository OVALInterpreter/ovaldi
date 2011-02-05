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

#include "ConcatFunction.h"

using namespace std;

//****************************************************************************************//
//								Component Class											  //	
//****************************************************************************************//
ConcatFunction::ConcatFunction() : AbsFunctionComponent() {
}

ConcatFunction::~ConcatFunction() {
}

// ***************************************************************************************	//
//								 Public members												//
// ***************************************************************************************	//
ComponentValue* ConcatFunction::ComputeValue() {

	/** 
	    Make initial call to start the recursive processing of components.
	    Call starts with the index of the first item in the set of components
	*/
	return this->ProcessComponents(0);
}

ComponentValue* ConcatFunction::ProcessComponents(unsigned int index) {
	
	if(this->GetComponents()->size()-1 == index) {
		// reached the end of the line just return all this components values in a vector
		ComponentValue* results = this->GetComponents()->at(index)->ComputeValue();
		return results;
	} else {
		// make recursive call
		ComponentValue* values = this->ProcessComponents(index+1);

		// get the set of current values
		ComponentValue* currentValues = this->GetComponents()->at(index)->ComputeValue();

		// Create a object to store the results of the concat
		ComponentValue* results = new ComponentValue();
		// combine the flag values and set the new value
		IntVector* flags = new IntVector();
		flags->push_back(values->GetFlag());
		flags->push_back(currentValues->GetFlag());
		results->SetFlag(OvalEnum::CombineFlags(flags));
		delete flags;
		
		// loop over current values
		StringVector::iterator currentIterator;
		for(currentIterator = currentValues->GetValues()->begin(); currentIterator != currentValues->GetValues()->end(); currentIterator++) {
		
			// loop over results of recursive call
			StringVector::iterator iterator;
			for(iterator = values->GetValues()->begin(); iterator != values->GetValues()->end(); iterator++) {
				// concat the values
				string tmpStr = (*currentIterator) + (*iterator);
				results->AppendValue(tmpStr);
			}
		}

		// make sure all the messages are passed along tot he results
		results->AppendMessages(values->GetMessages());
		results->AppendMessages(currentValues->GetMessages());

		delete currentValues;
		delete values;		

		return results;
	}
}

void ConcatFunction::Parse(DOMElement* componentElm) {
    
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

VariableValueVector* ConcatFunction::GetVariableValues() {
	
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
