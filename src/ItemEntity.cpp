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

#include <stdlib.h>

#include "Common.h"
#include "ItemFieldEntityValue.h"
#include "StringEntityValue.h"

#include "ItemEntity.h"

using namespace std;

//****************************************************************************************//
//								ItemEntity Class										  //	
//****************************************************************************************//
ItemEntity::ItemEntity(string name, string value, OvalEnum::Datatype datatype, bool isObjectEntity, OvalEnum::SCStatus status, bool isNil) {
	this->name = name;
	this->value.push_back(new StringEntityValue(value));
	this->datatype = datatype;
	this->isObjectEntity = isObjectEntity;
	this->scStatus = status;
	this->nil = isNil;
}

ItemEntity::ItemEntity(string name, AbsEntityValueVector value, OvalEnum::Datatype datatype, bool isObjectEntity, OvalEnum::SCStatus status, bool isNil) {
	this->name = name;
	this->value = value;
	this->datatype = datatype;
	this->isObjectEntity = isObjectEntity;
	this->scStatus = status;
	this->nil = isNil;
}

ItemEntity::ItemEntity(const ItemEntity& itemEntity){
    this->name = itemEntity.name;
    this->datatype = itemEntity.datatype;
    this->isObjectEntity = itemEntity.isObjectEntity;
	this->scStatus = itemEntity.scStatus;
	this->nil = itemEntity.nil;

	for (AbsEntityValueVector::const_iterator iter = itemEntity.value.begin();
		 iter != itemEntity.value.end();
		 ++iter) {

		// Afaik, these are the only two subclasses of AbsEntityValue which can
		// be used in an ItemEntity.
		const StringEntityValue *sv;
		const ItemFieldEntityValue *iv;

		sv = dynamic_cast<const StringEntityValue*>(*iter);
		iv = dynamic_cast<const ItemFieldEntityValue*>(*iter);

		if (sv)
			this->value.push_back(new StringEntityValue(*sv));
		else if (iv)
			this->value.push_back(new ItemFieldEntityValue(*iv));
		else
			throw Exception("Encountered an unsupported type of item entity value!  "
							"Please update me to support the new type.");
	}
}

ItemEntity::~ItemEntity() {
	for (AbsEntityValueVector::iterator iter = this->value.begin();
		 iter != this->value.end();
		 ++iter)
		delete *iter;
}

// ***************************************************************************************	//
//								 Public members												//
// ***************************************************************************************	//
string ItemEntity::GetName() {

	return this->name;
}

void ItemEntity::SetName(string name) {

	this->name = name;
}

string ItemEntity::GetValue() {
	if ( this->value.empty() ){
		return "";
	}else{
		return this->value.front()->GetValue();
	}
}

void ItemEntity::SetValue(string value) {
	if ( this->value.empty() ){
		this->value.push_back(new StringEntityValue(value));
	}else{
		this->value.front()->SetValue(value);
	}
}

void ItemEntity::SetValues(AbsEntityValueVector value){
	this->value = value;
}

AbsEntityValueVector ItemEntity::GetValues(){
	return this->value;
}

OvalEnum::Datatype ItemEntity::GetDatatype() {

	return this->datatype;
}

void ItemEntity::SetDatatype(OvalEnum::Datatype datatype) {

	this->datatype = datatype;
}

bool ItemEntity::GetIsObjectEntity() {

	return this->isObjectEntity;
}

void ItemEntity::SetIsObjectEntity(bool isObjectEntity) {

	this->isObjectEntity = isObjectEntity;
}

OvalEnum::SCStatus ItemEntity::GetStatus() {

	return this->scStatus;
}

void ItemEntity::SetStatus(OvalEnum::SCStatus scStatus) {
	
	this->scStatus = scStatus;
}

void ItemEntity::Write(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* scFile, DOMElement* itemElm) {

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

	if (this->GetNil()) {
		XmlCommon::AddAttribute(newItemEntityElem, "xsi:nil", "true");
	}

	// Add the value(s)
	AbsEntityValueVector v = this->GetValues();
	for(AbsEntityValueVector::iterator it = v.begin(); it != v.end();it++){
		if ( this->GetDatatype() != OvalEnum::DATATYPE_RECORD ){
			((StringEntityValue*)*it)->Write(scFile,newItemEntityElem);
		}else{
			// Don't add field entities unless the the entity exists
			if ( this->GetStatus() == OvalEnum::STATUS_EXISTS )
				((ItemFieldEntityValue*)*it)->Write(scFile,newItemEntityElem);
		}
	}
}

string ItemEntity::UniqueString() {
	string valueStr = "";
	AbsEntityValueVector allValues = this->GetValues();
	for(AbsEntityValueVector::iterator it = allValues.begin(); it != allValues.end(); it++){
		// If a record append the name of each field before its value
		if ( this->GetDatatype() == OvalEnum::DATATYPE_RECORD ){
			valueStr.append(((ItemFieldEntityValue*)(*it))->GetName());
		}
		valueStr.append((*it)->GetValue());
	}
	return this->GetName() + valueStr;
}

void ItemEntity::Parse(DOMElement* itemEntityElm) {
	
	this->SetName(XmlCommon::GetElementName(itemEntityElm));
	this->SetDatatype(OvalEnum::ToDatatype(XmlCommon::GetAttributeByName(itemEntityElm, "datatype")));
	this->SetStatus(OvalEnum::ToSCStatus(XmlCommon::GetAttributeByName(itemEntityElm, "status")));
	
	// The datatype is not 'record' so we can just grab the string value in the element.
	if ( this->GetDatatype() != OvalEnum::DATATYPE_RECORD ){
		this->SetValue(XmlCommon::GetDataNodeValue(itemEntityElm));
	}else{
		// The datatype is 'record' so we need to loop over all of the elements (which are the fields).
		DOMNodeList *itemChildren = itemEntityElm->getChildNodes();
		unsigned int index = 0;
		AbsEntityValueVector aevv;
		while(index < itemChildren->getLength()) {
			DOMNode* tmpNode = itemChildren->item(index);

			//	Only concerned with ELEMENT_NODEs
			if (tmpNode->getNodeType() == DOMNode::ELEMENT_NODE) {
				DOMElement* itemChild = (DOMElement*)tmpNode;

				//	Get the name of the child
				string childName = XmlCommon::GetElementName(itemChild);
				if ( childName.compare("field") == 0 || childName.compare("oval-sc:field") == 0 ){
					ItemFieldEntityValue* fev = new ItemFieldEntityValue();
					fev->Parse(itemChild);
					aevv.push_back(fev);
				}else{
					StringEntityValue* sev = new StringEntityValue();
					sev->Parse(itemChild);
					aevv.push_back(sev);
				}
			}
			index ++;
		}	
		this->SetValues(aevv);
	}
}

void ItemEntity::SetNil(bool isNil) {
	this->nil = isNil;
}

bool ItemEntity::GetNil() {
	return this->nil;
}
