//
// $Id: ItemEntity.cpp 4579 2008-01-02 17:39:07Z bakerj $
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

#include "ItemEntity.h"

//****************************************************************************************//
//								ItemEntity Class										  //	
//****************************************************************************************//
ItemEntity::ItemEntity(string name, string value, OvalEnum::Datatype datatype, bool isObjectEntity, OvalEnum::SCStatus status) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Create a complete ItemEntity object
	//
	// -----------------------------------------------------------------------

	this->SetName(name);
	this->SetValue(value);
	this->SetDatatype(datatype);
	this->SetIsObjectEntity(isObjectEntity);
	this->SetStatus(status);
}

ItemEntity::~ItemEntity() {
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
string ItemEntity::GetName() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the name field's value
	//
	// -----------------------------------------------------------------------

	return this->name;
}

void ItemEntity::SetName(string name) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the name field's value
	//
	// -----------------------------------------------------------------------

	this->name = name;
}

string ItemEntity::GetValue() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the value field's value
	//
	// -----------------------------------------------------------------------

	return this->value;
}

void ItemEntity::SetValue(string value) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the value field's value
	//
	// -----------------------------------------------------------------------

	this->value = value;
}

OvalEnum::Datatype ItemEntity::GetDatatype() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the datatype field's value
	//
	// -----------------------------------------------------------------------

	return this->datatype;
}

void ItemEntity::SetDatatype(OvalEnum::Datatype datatype) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the datatype field's value
	//
	// -----------------------------------------------------------------------

	this->datatype = datatype;
}

bool ItemEntity::GetIsObjectEntity() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the isObjectEntity field's value
	//
	// -----------------------------------------------------------------------

	return this->isObjectEntity;
}

void ItemEntity::SetIsObjectEntity(bool isObjectEntity) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the isObjectEntity field's value
	//
	// -----------------------------------------------------------------------

	this->isObjectEntity = isObjectEntity;
}

OvalEnum::SCStatus ItemEntity::GetStatus() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the status field's value
	//
	// -----------------------------------------------------------------------

	return this->scStatus;
}

void ItemEntity::SetStatus(OvalEnum::SCStatus scStatus) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the scStatus field's value
	//
	// -----------------------------------------------------------------------

	this->scStatus = scStatus;
}

bool ItemEntity::Equals(ItemEntity* entity) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return true iff this ItemEntity is equal to the provided ItemEntity
	//	Note: Status is not compared.
	// -----------------------------------------------------------------------
	
	bool isEqual = false;
	try {
	
		if(this->GetDatatype() == entity->GetDatatype()) {
			if(this->GetName().compare(entity->GetName()) == 0) {
				if(this->GetValue().compare(entity->GetValue()) == 0) {
					isEqual = true;	
				}
			}
		}
	} catch (Exception ex) {
		throw Exception("Error: An error occured with comparing two ItemEntities", ERROR_FATAL, &ex);
	} catch(...) {
		throw Exception("Error: An error occured with comparing two ItemEntities", ERROR_FATAL);
	}

	return isEqual;
}

void ItemEntity::Write(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* scFile, DOMElement* itemElm) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Write this ItemEntity to the sc file
	// -----------------------------------------------------------------------

	// Create new item element
	DOMElement* newItemEntityElem = scFile->createElement(XMLString::transcode(this->GetName().c_str()));
	itemElm->appendChild(newItemEntityElem);

	// Add the attributes
	// handling defaults in the schema
	string strDatatype = OvalEnum::DatatypeToString(this->GetDatatype());
	if(strDatatype.compare("string") != 0)
		XmlCommon::AddAttribute(newItemEntityElem, "datatype", strDatatype);

	string strStatus = OvalEnum::SCStatusToString(this->GetStatus());
	if(strStatus.compare("exists") != 0)
        XmlCommon::AddAttribute(newItemEntityElem, "status", strStatus);

	// Add the value
	if(this->GetValue().compare("") != 0) {
		DOMText* newItemEntityElemValue = scFile->createTextNode(XMLString::transcode(this->GetValue().c_str()));
		newItemEntityElem->appendChild(newItemEntityElemValue);
	}
}

void ItemEntity::Parse(DOMElement* itemEntityElm) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Parse the provided entity element
	// -----------------------------------------------------------------------
	
	this->SetName(XmlCommon::GetElementName(itemEntityElm));
	this->SetValue(XmlCommon::GetDataNodeValue(itemEntityElm));
	this->SetDatatype(OvalEnum::ToDatatype(XmlCommon::GetAttributeByName(itemEntityElm, "datatype")));
	this->SetStatus(OvalEnum::ToSCStatus(XmlCommon::GetAttributeByName(itemEntityElm, "status")));
}
