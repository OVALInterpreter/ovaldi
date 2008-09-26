//
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

#include "EndFunction.h"

//****************************************************************************************//
//								Component Class											  //	
//****************************************************************************************//
EndFunction::EndFunction(string charIn) : AbsFunctionComponent() {

	this->SetCharacter(charIn);
}

EndFunction::~EndFunction() {
}

// ***************************************************************************************	//
//								 Public members												//
// ***************************************************************************************	//
string EndFunction::GetCharacter() {
	return this->character;
}

void EndFunction::SetCharacter(string charIn) {

	this->character = charIn;
}

ComponentValue* EndFunction::ComputeValue() {

	StringVector* values = NULL;
	
	AbsComponent* component = this->GetComponents()->at(0);

	ComponentValue* componentValue = component->ComputeValue();

	// create and populate a result ComponentValue
	ComponentValue* result = new ComponentValue();
	result->SetFlag(componentValue->GetFlag());
	result->AppendMessages(componentValue->GetMessages());

	if(componentValue->GetFlag() == OvalEnum::FLAG_COMPLETE) {
		values = new StringVector();;

		StringVector::iterator iterator;
		for(iterator = componentValue->GetValues()->begin(); iterator != componentValue->GetValues()->end(); iterator++) {
			string currentValue = (*iterator);
            string newValue = "";

            if(currentValue.length() > this->GetCharacter().length()) {
                unsigned int charLength = this->GetCharacter().length();
                string endSubStr = currentValue.substr(currentValue.length() - charLength, currentValue.length());
                if(this->GetCharacter().compare(endSubStr) != 0) {
                    newValue = currentValue.append(this->GetCharacter());
                } else {
                    newValue = currentValue;
                }

            } else {
                newValue = currentValue.append(this->GetCharacter());
            }			
            values->push_back(newValue);
		}
		result->SetValues(values);
	}

	delete componentValue;

	return result;	
}

void EndFunction::Parse(DOMElement* componentElm) {

	// get the character attrs
	string charAttr = XmlCommon::GetAttributeByName(componentElm, "character");
    this->SetCharacter(charAttr);

	// Loop through all child elements
	// there should only ever be one 
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

VariableValueVector* EndFunction::GetVariableValues() {
	
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
		// The code is no consistant here. In places a new vector is returned
		// in others a reference to a vector that is managed by other code is returned.
		//delete tmp;
		//tmp = NULL;
	}

	return values;
}
