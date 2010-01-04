//
//
//****************************************************************************************//
// Copyright (c) 2002-2010, The MITRE Corporation
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

#include "LiteralComponent.h"

//****************************************************************************************//
//								LiteralComponent Class									  //	
//****************************************************************************************//
LiteralComponent::LiteralComponent(string value) : AbsComponent() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Create a complete LiteralComponent object
	//
	// -----------------------------------------------------------------------

	this->SetValue(value);
}

LiteralComponent::~LiteralComponent() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Do nothing for now
	//
	// -----------------------------------------------------------------------
}

// ***************************************************************************************	//
//								 Public members												//
// ***************************************************************************************	//
string LiteralComponent::GetValue() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the value field's value
	//
	// -----------------------------------------------------------------------

	return this->value;
}

void LiteralComponent::SetValue(string value) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the value field's value
	//
	// -----------------------------------------------------------------------

	this->value = value;
}


ComponentValue* LiteralComponent::ComputeValue() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Compute the value for this component. 
	//  Allows only one value.
	//  If the value is an empty string set the OvalEnum::Flag on the 
	//  resulting ComponentValue to OvalEnum::FLAG_ERROR
	// -----------------------------------------------------------------------
	
	ComponentValue* result = new ComponentValue();

	if(this->GetValue().compare("") != 0) {
		StringVector* values = new StringVector();
		values->push_back(this->GetValue());
		result->SetValues(values);
		result->SetFlag(OvalEnum::FLAG_COMPLETE);
	} else {
		result->SetFlag(OvalEnum::FLAG_ERROR);
		result->AppendMessage("Error the specified literal component did not have a value. Literal components are required to have values.");
	}

	return result;	
}

void LiteralComponent::Parse(DOMElement* componentElm) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	parse the component element
	// -----------------------------------------------------------------------
    
	this->SetValue(XmlCommon::GetDataNodeValue(componentElm));
}

VariableValueVector* LiteralComponent::GetVariableValues() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	return the variable values used to compute this component's value
	// in this case just an empty vector.
	// -----------------------------------------------------------------------
	
	VariableValueVector* values = new VariableValueVector();

	return values;
}
