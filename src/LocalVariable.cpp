//
// $Id: LocalVariable.cpp 4593 2008-01-03 16:39:57Z bakerj $
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

#include "LocalVariable.h"

//****************************************************************************************//
//									LocalVariable Class									  //	
//****************************************************************************************//

LocalVariable::LocalVariable(string id, string name, int version, OvalEnum::Datatype datatype, StringVector* msgs) : AbsVariable (id, name, version, datatype, msgs) {

}

LocalVariable::~LocalVariable() {

}

// ***************************************************************************************	//
//								 Public members												//
// ***************************************************************************************	//
AbsComponent* LocalVariable::GetComponent() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	return the AbsComponent
	//
	// -----------------------------------------------------------------------
	return this->component;
}

void LocalVariable::SetComponent(AbsComponent* component) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the components member
	//
	// -----------------------------------------------------------------------
	this->component = component;
}

void LocalVariable::ComputeValue() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Compute the value of the component then create a VariableValue
	//  for each value in the returned ComponentValue if the flag is set to complete
	//  or incomplete.
	// -----------------------------------------------------------------------

	ComponentValue* value = this->GetComponent()->ComputeValue();

	// set the flag for this variable
	this->SetFlag(value->GetFlag());

	// set the error message for this varaible
	this->AppendMessages(value->GetMessages());

	// if flag is complete or incomplete create VariableValues for each value.
	if(value->GetFlag() == OvalEnum::FLAG_COMPLETE || value->GetFlag() == OvalEnum::FLAG_INCOMPLETE) {
		StringVector::iterator iterator;
		for(iterator = value->GetValues()->begin(); iterator != value->GetValues()->end(); iterator++) {
			VariableValue* varValue = new VariableValue(this->GetId(), (*iterator));
			this->AppendVariableValue(varValue);
		}
	}

	delete value;
}

void LocalVariable::Parse(DOMElement* localVariableElm) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Parse the provided LocalVariable element into a LocalVariable
	//
	// -----------------------------------------------------------------------

	this->SetId(XmlCommon::GetAttributeByName(localVariableElm, "id"));
	this->SetDatatype(OvalEnum::ToDatatype(XmlCommon::GetAttributeByName(localVariableElm, "daatatype")));
	string versionStr = XmlCommon::GetAttributeByName(localVariableElm, "version");
	int version;
	if(versionStr.compare("") != 0) {
		version = atoi(versionStr.c_str());
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
		}
		index ++;
	}

	// Finally call ComputeValue
	this->ComputeValue();
}

VariableValueVector* LocalVariable::GetVariableValues() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	return the variable values used to compute this variable's value
	//	in this case just return the values used by the component.
	// -----------------------------------------------------------------------
	
	VariableValueVector* values = NULL;

	values = this->GetComponent()->GetVariableValues();

	return values;
}
