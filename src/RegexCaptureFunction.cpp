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
//****************************************************************************************//

#include <algorithm>
#include <iterator>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMNodeList.hpp>

#include "ComponentFactory.h"
#include "XmlCommon.h"

#include "RegexCaptureFunction.h"

using namespace std;
using namespace xercesc;

//****************************************************************************************//
//								Component Class											  //	
//****************************************************************************************//
// ***************************************************************************************	//
//								 Public members												//
// ***************************************************************************************	//
ComponentValue* RegexCaptureFunction::ComputeValue() {

    AbsComponentVector *components = this->GetComponents();

    //These functions only get 1 component (enforced by validation).
    //  So assume there is only 1.
    AbsComponent *comp = (*components)[0];
    string regex = this->GetPattern();
    ComponentValue *value = comp->ComputeValue();

    // create and populate a result ComponentValue
    ComponentValue* result = new ComponentValue();
    result->SetFlag(value->GetFlag());
    result->AppendMessages(value->GetMessages());
    if (result->GetFlag() == OvalEnum::FLAG_ERROR)
    {
        delete value;
        return result;
    }

    StringVector matches;
    StringVector *valueValues = value->GetValues();
    this->reUtil.Reset();
    for(StringVector::iterator valueIter = valueValues->begin(); valueIter != valueValues->end(); ++valueIter) {
        matches.clear();
        bool matchSucceeded = this->reUtil.GetMatchingSubstrings(regex.c_str(), valueIter->c_str(), &matches);
        if (matchSucceeded) {
            if (matches.empty())
                result->AppendValue("");
            else
                result->AppendValue(matches[0]); // add the first capture, ignore any others.
        }
        else
            result->AppendValue("");
    }

    delete value;
    return result;	
}

void RegexCaptureFunction::Parse(DOMElement* componentElm) {

	// get the start and length attrs
	string pattern = XmlCommon::GetAttributeByName(componentElm, "pattern");
	this->SetPattern(pattern);

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

VariableValueVector RegexCaptureFunction::GetVariableValues() {
	
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
