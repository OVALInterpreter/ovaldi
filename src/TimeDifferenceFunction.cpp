//
//
//****************************************************************************************//
// Copyright (c) 2002-2009, The MITRE Corporation
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

#include "TimeDifferenceFunction.h"

//****************************************************************************************//
//								Component Class											  //	
//****************************************************************************************//
TimeDifferenceFunction::TimeDifferenceFunction(OvalEnum::DateTimeFormat format1, OvalEnum::DateTimeFormat format2) : AbsFunctionComponent() {

    this->SetFormat1(format1);
    this->SetFormat2(format2);
}

TimeDifferenceFunction::~TimeDifferenceFunction() {
}

// ***************************************************************************************	//
//								 Public members												//
// ***************************************************************************************	//
OvalEnum::DateTimeFormat TimeDifferenceFunction::GetFormat1() {
	return this->format1;
}

void TimeDifferenceFunction::SetFormat1(OvalEnum::DateTimeFormat format1) {

	this->format1 = format1;
}

OvalEnum::DateTimeFormat TimeDifferenceFunction::GetFormat2() {

	return this->format2;
}

void TimeDifferenceFunction::SetFormat2(OvalEnum::DateTimeFormat format2) {

	this->format2 = format2;
}

ComponentValue* TimeDifferenceFunction::ComputeValue() {


    throw Exception("Error: unsupported function");

	// create and populate a result ComponentValue
	ComponentValue* result = new ComponentValue();
	

	return result;	
}

void TimeDifferenceFunction::Parse(DOMElement* componentElm) {

	// get the start and length attrs
	string format_1Str = XmlCommon::GetAttributeByName(componentElm, "format_1");
    this->SetFormat1(OvalEnum::ToDateTimeFormat(format_1Str));
	string format_2Str = XmlCommon::GetAttributeByName(componentElm, "format_2");
	this->SetFormat2(OvalEnum::ToDateTimeFormat(format_2Str));

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

VariableValueVector* TimeDifferenceFunction::GetVariableValues() {
	
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
