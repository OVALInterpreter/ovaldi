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

#include "VariableComponent.h"

using namespace std;

//****************************************************************************************//
//								VariableComponent Class									  //	
//****************************************************************************************//
VariableComponent::VariableComponent(AbsVariable* varRef) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Create a complete VariableComponent object
	//
	// -----------------------------------------------------------------------

	this->SetVarRef(varRef);
}

VariableComponent::~VariableComponent() {

}

// ***************************************************************************************	//
//								 Public members												//
// ***************************************************************************************	//
AbsVariable* VariableComponent::GetVarRef() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the varRef field's value
	//
	// -----------------------------------------------------------------------

	return this->varRef;
}

void VariableComponent::SetVarRef(AbsVariable* varRef) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the varRef field's value
	//
	// -----------------------------------------------------------------------

	this->varRef = varRef;
}

ComponentValue* VariableComponent::ComputeValue() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	// Compute the value for this component. 
	// -----------------------------------------------------------------------

	VariableValueVector varValues = this->GetVarRef()->GetValues();
	StringVector* values = new StringVector();

	VariableValueVector::iterator iterator;
	for(iterator = varValues.begin(); iterator != varValues.end(); iterator++) {
		values->push_back(iterator->GetValue());
	}

	// need to create a copy of the messages.
	StringVector* msg = this->GetVarRef()->GetMessages();
	StringVector* messages = new StringVector();
	StringVector::iterator iterator1;
	for(iterator1 = msg->begin(); iterator1 != msg->end(); iterator1++) {
		messages->push_back((*iterator1));
	}

	// create the ComponentValue to return. 
	// Always passing the flag associated with this var ref and the values
	ComponentValue* result = new ComponentValue(this->GetVarRef()->GetFlag(), values, messages);

	return result;
}

void VariableComponent::Parse(DOMElement* componentElm) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	parse the VariableComponent element
	// -----------------------------------------------------------------------

	string varId = XmlCommon::GetAttributeByName(componentElm, "var_ref");
	AbsVariable* var = VariableFactory::GetVariable(varId);
	this->SetVarRef(var);
}

VariableValueVector VariableComponent::GetVariableValues() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	return the variable values used to compute this component's value
	// -----------------------------------------------------------------------
	
	VariableValueVector values;
	
	// loop through the variable values used by the var ref
    VariableValueVector varRefDeps = this->GetVarRef()->GetVariableValues();
	VariableValueVector::iterator iterator;
	for(iterator = varRefDeps.begin(); iterator != varRefDeps.end(); iterator++) {
		values.push_back(*iterator);
	}

	// Add the values associated with the var ref
	VariableValueVector varRefValues = this->GetVarRef()->GetValues();
	VariableValueVector::iterator iterator2;
	for(iterator2 = varRefValues.begin(); iterator2 != varRefValues.end(); iterator2++) {
		values.push_back(*iterator2);
	}

	return values;
}
