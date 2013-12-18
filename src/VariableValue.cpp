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

#include <xercesc/util/XMLString.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMText.hpp>

#include "XmlCommon.h"

#include "VariableValue.h"

using namespace std;
using namespace xercesc;

//****************************************************************************************//
//								VariableValue Class										  //	
//****************************************************************************************//
VariableValue::VariableValue(string id, string value) : id(id), value(value) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Create a complete VariableValue object
	//
	// -----------------------------------------------------------------------
}

VariableValue::~VariableValue() {

}

// ***************************************************************************************	//
//								 Public members												//
// ***************************************************************************************	//
string VariableValue::GetId() const {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the id field's value
	//
	// -----------------------------------------------------------------------

	return this->id;
}

void VariableValue::SetId(string id) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the id field's value
	//
	// -----------------------------------------------------------------------

	this->id = id;
}

string VariableValue::GetValue() const {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the value field's value
	//
	// -----------------------------------------------------------------------

	return this->value;
}

void VariableValue::SetValue(string value) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the value field's value
	//
	// -----------------------------------------------------------------------

	this->value = value;
}

void VariableValue::Write(DOMElement* collectedObjectElm) const {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Write this VariableValue to the sc file
	// -----------------------------------------------------------------------

	// Create new item element
	DOMDocument* scFile = collectedObjectElm->getOwnerDocument();
	DOMElement* newVariableValueElem = XmlCommon::CreateElementNS(scFile, XmlCommon::scNS, "variable_value");
	collectedObjectElm->appendChild(newVariableValueElem);

	// Add the attributes
	// handling defaults in the schema
	XmlCommon::AddAttribute(newVariableValueElem, "variable_id", this->GetId());

	// Add the value
	if(!this->GetValue().empty()) {
		XMLCh* value = XMLString::transcode(this->GetValue().c_str());
		DOMText* newVariableValueElemValue = scFile->createTextNode(value);
		//Free memory allocated by XMLString::transcode(char*)
		XMLString::release(&value);
		newVariableValueElem->appendChild(newVariableValueElemValue);
	}
}

void VariableValue::WriteTestedVariable(DOMElement* parentElm) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Write this VariableValue to the sc file
	// -----------------------------------------------------------------------

	// get the parent document
	DOMDocument* resultDoc = parentElm->getOwnerDocument();

	// create a new tested_item element
	DOMElement* testedVarElm = XmlCommon::AddChildElementNS(resultDoc, parentElm, XmlCommon::resNS, "tested_variable", this->GetValue());

	// add the attributes
	XmlCommon::AddAttribute(testedVarElm, "variable_id", this->GetId());
}


void VariableValue::Parse(DOMElement* variableValueElm) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	parses variable_value elm to a VariableValue obj
	//
	// -----------------------------------------------------------------------

	// get the attributes
	this->SetId(XmlCommon::GetAttributeByName(variableValueElm, "variable_id"));

	// get the value
	this->SetValue(XmlCommon::GetDataNodeValue(variableValueElm));
}

bool VariableValue::operator<(const VariableValue &other) const {
	int cmp = id.compare(other.id);
	if (cmp < 0)
		return true;
	else if (cmp > 0)
		return false;

	cmp = value.compare(other.value);
	if (cmp < 0)
		return true;
	return false;
}
