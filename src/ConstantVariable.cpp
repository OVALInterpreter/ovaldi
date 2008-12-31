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

#include "ConstantVariable.h"

//****************************************************************************************//
//									ConstantVariable Class								  //	
//****************************************************************************************//

ConstantVariable::ConstantVariable(string id, string name, int version, OvalEnum::Datatype datatype, StringVector* msgs) : AbsVariable (id, name, version, datatype, msgs) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Create a complete ConstantVariable
	//
	// -----------------------------------------------------------------------

}

ConstantVariable::~ConstantVariable() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	// -----------------------------------------------------------------------

}

// ***************************************************************************************	//
//								 Public members												//
// ***************************************************************************************	//
void ConstantVariable::Parse(DOMElement* constantVariableElm) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Parse the provided ConstantVariable element into a ConstantVariable
	//
	// -----------------------------------------------------------------------

	this->SetId(XmlCommon::GetAttributeByName(constantVariableElm, "id"));
	this->SetDatatype(OvalEnum::ToDatatype(XmlCommon::GetAttributeByName(constantVariableElm, "datatype")));
	string versionStr = XmlCommon::GetAttributeByName(constantVariableElm, "version");
	int version;
	if(versionStr.compare("") == 0) {
		version = 1;
	} else {
		version = atoi(versionStr.c_str());
	}
	this->SetVersion(version);

	// Get all the value elements' values
	bool foundValue = false;
	DOMNodeList *constantVariableElmChildren = constantVariableElm->getChildNodes();
	unsigned int index = 0;
	while(index < constantVariableElmChildren->getLength()) {
		DOMNode *tmpNode = constantVariableElmChildren->item(index);
		if (tmpNode->getNodeType() == DOMNode::ELEMENT_NODE) {
			DOMElement *childElm = (DOMElement*)tmpNode;

			if(XmlCommon::GetElementName(childElm).compare("value") == 0) {
				foundValue = true;
				string elmValue = XmlCommon::GetDataNodeValue(childElm);
				if(elmValue.compare("") != 0) {
					// get and save the value.
					VariableValue* varValue = new VariableValue(this->GetId(), elmValue);
					this->AppendVariableValue(varValue);
					this->SetFlag(OvalEnum::FLAG_COMPLETE);
				} else {
					this->SetFlag(OvalEnum::FLAG_ERROR);
					this->AppendMessage("Error a value element with an empty string value was found for the constant variable.");
					return;
				}
			}
		}
		index ++;
	}
	if(!foundValue) {
		this->SetFlag(OvalEnum::FLAG_ERROR);
		this->AppendMessage("Error a value was not found for the constant variable.");
	}

}

VariableValueVector* ConstantVariable::GetVariableValues() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	return the variable values used to compute this variable's value
	//	in this case just an empty vector.
	// -----------------------------------------------------------------------
	
	VariableValueVector* values = new VariableValueVector();

	return values;
}
