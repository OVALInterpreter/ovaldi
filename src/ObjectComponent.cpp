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

#include "ObjectComponent.h"

//****************************************************************************************//
//								ObjectComponent Class									  //	
//****************************************************************************************//
ObjectComponent::ObjectComponent(string objectId, string itemField) : AbsComponent() {
	this->SetObjectId(objectId);
	this->SetItemField(itemField);
}

ObjectComponent::~ObjectComponent() {
}

// ***************************************************************************************	//
//								 Public members												//
// ***************************************************************************************	//
string ObjectComponent::GetObjectId() {
	return this->objectId;
}

void ObjectComponent::SetObjectId(string objectId) {
	this->objectId = objectId;
}

string ObjectComponent::GetItemField() {
	return this->itemField;
}

void ObjectComponent::SetItemField(string itemField) {
	this->itemField = itemField;
}

ComponentValue* ObjectComponent::ComputeValue() {

    bool foundElm = false;
	ItemVector* items = NULL;
	string errorMsg = "";
	OvalEnum::Flag collectedObjFlag = OvalEnum::FLAG_ERROR;
	if(AbsDataCollector::GetIsRunning()) {
		CollectedObject* collectedObject = AbsObjectCollector::Instance()->Run(this->GetObjectId());
		collectedObjFlag = collectedObject->GetFlag();
		items = collectedObject->GetReferences();
		if(collectedObject->GetMessages()->size() > 0) {
			errorMsg = collectedObject->GetMessages()->at(1)->GetValue();
		}

	} else {
		collectedObjFlag = ObjectReader::GetCollectedObjectFlag(this->GetObjectId());
		items = ObjectReader::GetItemsForObject(this->GetObjectId());
	}

	ComponentValue* result = new ComponentValue();
	// set the flag to the flag value of the collected object.
	result->SetFlag(collectedObjFlag);

	// only process the items if the flag on the colelcted obj is COMPLETE or INCOMPLETE
	if(collectedObjFlag == OvalEnum::FLAG_COMPLETE || collectedObjFlag == OvalEnum::FLAG_INCOMPLETE) {

		if(items->size() > 0) {
			
			ItemVector::iterator iterator;
			for(iterator = items->begin(); iterator != items->end(); iterator++) {
				Item* item = (*iterator);
			
				ItemEntityVector* elements = item->GetElementsByName(this->GetItemField());
				ItemEntityVector::iterator iterator;
				for(iterator = elements->begin(); iterator != elements->end(); iterator++) {
					// need to check status on item entity
					if((*iterator)->GetStatus() == OvalEnum::STATUS_DOES_NOT_EXIST) {
						result->SetFlag(OvalEnum::FLAG_DOES_NOT_EXIST);
					} else if((*iterator)->GetStatus() == OvalEnum::STATUS_NOT_COLLECTED) {
						result->SetFlag(OvalEnum::FLAG_NOT_COLLECTED);
					} else if((*iterator)->GetStatus() == OvalEnum::STATUS_ERROR) {
						result->SetFlag(OvalEnum::FLAG_ERROR);
					} else { // exists status
						result->SetFlag(OvalEnum::FLAG_COMPLETE);
						string value = (*iterator)->GetValue();
						result->AppendValue(value);
					}
					foundElm = true;
				}
				elements->clear();
				delete elements;
			}

			if(!foundElm) {
				result->SetFlag(OvalEnum::FLAG_NOT_COLLECTED);
				result->AppendMessage("ObjectComponent::ComputeValue() - Unable to compute variable value. No elements found for the specified object_component id: " + this->GetObjectId() + " that match the specified field name: " + this->GetItemField());
			}

		} else {

			// This is an error because the collected object flag is set to COMPLETE or INCOMPLETE
			// and there are no items associated with it.
			result->SetFlag(OvalEnum::FLAG_ERROR);
			result->AppendMessage("Error no items are associated with the specified collected object: " + this->GetObjectId());
		}

	} else {

		if(errorMsg.compare("") != 0) {
			result->AppendMessage("ObjectComponent::ComputeValue() - Unable to compute variable value. No items found for the specified object_component id: " + this->GetObjectId() + errorMsg);			
		} else {
			result->AppendMessage("ObjectComponent::ComputeValue() - Unable to compute variable value. No items found for the specified object_component id: " + this->GetObjectId());
		}
	}

	return result;	
}

void ObjectComponent::Parse(DOMElement* ObjectComponentElm) {
	
	this->SetObjectId(XmlCommon::GetAttributeByName(ObjectComponentElm, "object_ref"));
	this->SetItemField(XmlCommon::GetAttributeByName(ObjectComponentElm, "item_field"));
}

VariableValueVector* ObjectComponent::GetVariableValues() {

    VariableValueVector* values = NULL;
	if(AbsDataCollector::GetIsRunning()) {
		CollectedObject* collectedObject = AbsObjectCollector::Instance()->Run(this->GetObjectId());
		values = collectedObject->GetVariableValues();
	} else {
		values = ObjectReader::GetVariableValuesForObject(this->GetObjectId());
	}
	
	return values;
}
