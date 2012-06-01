//
//
//****************************************************************************************//
// Copyright (c) 2002-2012, The MITRE Corporation
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

#include "ItemFieldEntityValue.h"

using namespace std;

//****************************************************************************************//
//								ItemFieldEntityValue Class								  //	
//****************************************************************************************//

ItemFieldEntityValue::ItemFieldEntityValue(string name, string value, OvalEnum::Datatype datatype, OvalEnum::SCStatus status) : AbsEntityValue(value) {
	this->name = name;
	this->datatype = datatype;
	this->scStatus = status;
}

ItemFieldEntityValue::ItemFieldEntityValue(const ItemFieldEntityValue& fieldEntity){
	this->name = fieldEntity.name;
	this->value = fieldEntity.value;
	this->datatype = fieldEntity.datatype;
	this->scStatus = fieldEntity.scStatus;
}

ItemFieldEntityValue::~ItemFieldEntityValue() {

}

// ***************************************************************************************	//
//								 Public members												//
// ***************************************************************************************	//
string ItemFieldEntityValue::GetName() {

	return this->name;
}

void ItemFieldEntityValue::SetName(string name) {

	this->name = name;
}

OvalEnum::Datatype ItemFieldEntityValue::GetDatatype() {

	return this->datatype;
}

void ItemFieldEntityValue::SetDatatype(OvalEnum::Datatype datatype) {

	this->datatype = datatype;
}

OvalEnum::SCStatus ItemFieldEntityValue::GetStatus() {

	return this->scStatus;
}

void ItemFieldEntityValue::SetStatus(OvalEnum::SCStatus scStatus) {
	
	this->scStatus = scStatus;
}

void ItemFieldEntityValue::Write(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* scFile, DOMElement* itemEntityElm) {
	// Create new field element
	DOMElement* newEntityValueElem = scFile->createElement(XMLString::transcode("oval-sc:field"));
	itemEntityElm->appendChild(newEntityValueElem);

	// Add the attributes
	// handling defaults in the schema
	XmlCommon::AddAttribute(newEntityValueElem, "name", this->GetName());

	string strDatatype = OvalEnum::DatatypeToString(this->GetDatatype());
	if(strDatatype.compare("string") != 0)
		XmlCommon::AddAttribute(newEntityValueElem, "datatype", strDatatype);

	string strStatus = OvalEnum::SCStatusToString(this->GetStatus());
	if(strStatus.compare("exists") != 0)
        XmlCommon::AddAttribute(newEntityValueElem, "status", strStatus);

	// Add the value
	DOMText* newEntityValueElemValue = scFile->createTextNode(XMLString::transcode(this->GetValue().c_str()));
	newEntityValueElem->appendChild(newEntityValueElemValue);
}

void ItemFieldEntityValue::Parse(DOMElement* fieldEntityElm) {
	string name = XmlCommon::GetElementPrefix(fieldEntityElm);
	name.append(":");
	name.append(XmlCommon::GetElementName(fieldEntityElm));
	this->SetName(XmlCommon::GetAttributeByName(fieldEntityElm,"name"));
	this->SetValue(XmlCommon::GetDataNodeValue(fieldEntityElm));
	this->SetDatatype(OvalEnum::ToDatatype(XmlCommon::GetAttributeByName(fieldEntityElm, "datatype")));
	this->SetStatus(OvalEnum::ToSCStatus(XmlCommon::GetAttributeByName(fieldEntityElm, "status")));
}