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

#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMNodeList.hpp>

#include "XmlCommon.h"
#include "Common.h"

#include "LocalVariable.h"

using namespace std;
using namespace xercesc;

//****************************************************************************************//
//									LocalVariable Class									  //	
//****************************************************************************************//

// ***************************************************************************************	//
//								 Public members												//
// ***************************************************************************************	//
void LocalVariable::ComputeValue() {

    ComponentValue* value = this->GetComponent()->ComputeValue();

	// set the flag for this variable
	this->SetFlag(value->GetFlag());

	// set the error message for this variable
	this->AppendMessages(value->GetMessages());

	// if flag is complete or incomplete create VariableValues for each value.
	if(value->GetFlag() == OvalEnum::FLAG_COMPLETE || value->GetFlag() == OvalEnum::FLAG_INCOMPLETE) {
		StringVector::iterator iterator;
		for(iterator = value->GetValues()->begin(); iterator != value->GetValues()->end(); iterator++) {
			this->AppendVariableValue(this->GetId(), *iterator);
		}
	}

	delete value;
}

void LocalVariable::Parse(DOMElement* localVariableElm) {

	this->SetId(XmlCommon::GetAttributeByName(localVariableElm, "id"));
	this->SetDatatype(OvalEnum::ToDatatype(XmlCommon::GetAttributeByName(localVariableElm, "datatype")));
	string versionStr = XmlCommon::GetAttributeByName(localVariableElm, "version");
	int version = 0;
	if(!versionStr.empty()) {
		Common::FromString(versionStr, &version);
		this->SetVersion(version);
	}	
   
	// now loop through all elements children
	DOMNodeList *localVariableElmChildren = localVariableElm->getChildNodes();
	unsigned int index = 0;
	while(index < localVariableElmChildren->getLength()) {
		DOMNode *tmpNode = localVariableElmChildren->item(index);
		if (tmpNode->getNodeType() == DOMNode::ELEMENT_NODE) {
			DOMElement *childElm = (DOMElement*)tmpNode;

			// Call the ComponentFactory
			AbsComponent* absComponent = ComponentFactory::GetComponent(childElm);
			this->SetComponent(absComponent);
			break; // local_variable gets no more than one component
		}
		index ++;
	}

	// Finally call ComputeValue
	this->ComputeValue();
}
