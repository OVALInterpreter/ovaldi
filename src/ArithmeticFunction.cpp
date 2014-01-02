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

#include "Common.h"
#include "XmlCommon.h"

#include "ArithmeticFunction.h"

using namespace std;
using namespace xercesc;

//****************************************************************************************//
//								Component Class											  //	
//****************************************************************************************//
// ***************************************************************************************	//
//                                  Public members                                          //
// ***************************************************************************************	//

ComponentValue* ArithmeticFunction::ComputeValue() {

    AbsComponentVector *components = GetComponents();

    // assert components->size() >= 2... (xml validation should enforce this)

    ComponentValue *componentValue, *accumComponentValue;
    AbsComponentVector::iterator componentIter;

    // This algorithm works by incrementally combining all the ComponentValues
    // two at a time.  E.g. if there are 3 components with 2, 3, and 4 values
    // respectively, then first a 2x3 combination is done, resulting in a 6-value
    // ComponentValue, then that is combined with the last 4-value ComponentValue
    // resulting in a final 24-value ComponentValue.

    componentIter = components->begin();
    accumComponentValue = (*componentIter)->ComputeValue();
    ++componentIter;

    while (componentIter != components->end()) {
        componentValue = (*componentIter)->ComputeValue();
        ComponentValue *tempValue = this->CombineTwoComponentValues(accumComponentValue, componentValue);
        delete componentValue;
        delete accumComponentValue;
        accumComponentValue = tempValue;
        ++componentIter;
    }

	return accumComponentValue;
}

void ArithmeticFunction::Parse(DOMElement* componentElm) {

	// get the character attrs
	string arithmeticOperation = XmlCommon::GetAttributeByName(componentElm, "arithmetic_operation");
    this->SetArithmeticOperation(OvalEnum::ToArithmeticOperation(arithmeticOperation));

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

VariableValueVector ArithmeticFunction::GetVariableValues() {
	
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

// ***************************************************************************************	//
//                                  Private members                                         //
// ***************************************************************************************	//

ComponentValue* ArithmeticFunction::CombineTwoComponentValues(ComponentValue* componentValue1, ComponentValue* componentValue2) {

    ComponentValue *resultValue = new ComponentValue();
    OvalEnum::ArithmeticOperation op = GetArithmeticOperation();

    // Append all the component values' messages to our result value
    resultValue->AppendMessages(componentValue1->GetMessages());
    resultValue->AppendMessages(componentValue2->GetMessages());

    IntVector flags;
    flags.push_back(componentValue1->GetFlag());
    flags.push_back(componentValue2->GetFlag());
    OvalEnum::Flag combinedFlag = OvalEnum::CombineFlags(&flags);
    resultValue->SetFlag(combinedFlag);
    if (combinedFlag == OvalEnum::FLAG_ERROR)
        return resultValue;

    StringVector *values1 = componentValue1->GetValues();
    StringVector *values2 = componentValue2->GetValues();

    StringVector::iterator values1Iter, values2Iter;
    for (values1Iter = values1->begin(); values1Iter != values1->end(); ++values1Iter) {
        for (values2Iter = values2->begin(); values2Iter != values2->end(); ++values2Iter) {

            double value1 = 0;
            double value2 = 0;
            double result;
			Common::FromString(*values1Iter, &value1);
			Common::FromString(*values2Iter, &value2);

            switch (op) {
            case OvalEnum::ARITHMETIC_ADD:
                result = value1 + value2;
                break;
            case OvalEnum::ARITHMETIC_MULTIPLY:
                result = value1 * value2;
                break;
            default:
                delete resultValue;
                throw Exception(string("Unhandled operator: ")+OvalEnum::ArithmeticOperationToString(op), OvalEnum::LEVEL_ERROR);
            }

            resultValue->AppendValue(Common::ToString(result));
        }
    }

    return resultValue;
}
