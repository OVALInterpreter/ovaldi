//
//
//****************************************************************************************//
// Copyright (c) 2002-2014, The MITRE Corporation
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

#include <algorithm>
#include <iterator>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMNodeList.hpp>

#include "ComponentFactory.h"
#include "Common.h"
#include "XmlCommon.h"

#include "SubstringFunction.h"

using namespace std;
using namespace xercesc;

//****************************************************************************************//
//								Component Class											  //	
//****************************************************************************************//
// ***************************************************************************************	//
//								 Public members												//
// ***************************************************************************************	//

ComponentValue* SubstringFunction::ComputeValue() {

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
			
			// Initialize substring_start to the first character in the string.  This case applies if substring_start is less than or equal to 1.
			int start = 0;

			// Initialize substring_length to the length of the string.  This case applies if substring_length is less than 0 or greater than or equal to the length of the string.
			// It is important that this is a signed type, because the start
			// index may be negative, which will compare wrong with an unsigned type.
			// (and we hope these strings aren't so gigantic a signed int can't hold
			//  its length!)
			int len = currentValue.length();
			
			// If start is larger than the length of the string, throw exception
			if ( this->GetStart() > len)
				throw Exception("Substring start index too large ("+Common::ToString(this->GetStart())+") for string value:\n"+currentValue, OvalEnum::LEVEL_ERROR);
			// otherwise, if substring_start is greater than 1 use the specified value.
			else if ( this->GetStart() > 1 )
				start = this->GetStart() - 1;

			// If substring_length is greater than -1 and less than the the length of the string use the specified value.
			if ( ( this->GetLength() > -1 ) && ( this->GetLength() < len ) ) len = this->GetLength(); 

			newValue = currentValue.substr(start,len);		
			values->push_back(newValue);
		}
		result->SetValues(values);
	}

	delete componentValue;

	return result;	
}

void SubstringFunction::Parse(DOMElement* componentElm) {

	int tmp;
	// get the start and length attrs
	string start = XmlCommon::GetAttributeByName(componentElm, "substring_start");
	tmp = 0;
	Common::FromString(start, &tmp);
	this->SetStart(tmp);	
	string length = XmlCommon::GetAttributeByName(componentElm, "substring_length");
	tmp = 0;
	Common::FromString(length, &tmp);
	this->SetLength(tmp);

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

VariableValueVector SubstringFunction::GetVariableValues() {
	
	VariableValueVector values;
	AbsComponentVector* components = this->GetComponents();
	AbsComponentVector::iterator iterator;
	for(iterator = components->begin(); iterator != components->end(); iterator++) {
		AbsComponent* component = (AbsComponent*)(*iterator);
		VariableValueVector tmp = component->GetVariableValues();
		copy(tmp.begin(), tmp.end(), back_inserter(values));
	}

	return values;
}
