//
//
//****************************************************************************************//
// Copyright (c) 2002-2014, The MITRE Corporation
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
#include "AbsDataCollector.h"
#include "XmlCommon.h"
#include "AbsObjectCollector.h"
#include "ObjectReader.h"

#include "ObjectComponent.h"

using namespace std;
using namespace xercesc;

//****************************************************************************************//
//								ObjectComponent Class									  //	
//****************************************************************************************//
// ***************************************************************************************	//
//								 Public members												//
// ***************************************************************************************	//

ComponentValue* ObjectComponent::ComputeValue() {

	ItemVector* items = NULL;
	string errorMsg = "";
	OvalEnum::Flag collectedObjFlag = OvalEnum::FLAG_ERROR;
	StringVector* messages = NULL;
	if(AbsDataCollector::GetIsRunning()) {
		CollectedObject* collectedObject = AbsObjectCollector::Instance()->Run(this->GetObjectId());
		collectedObjFlag = collectedObject->GetFlag();
		items = collectedObject->GetReferences();
		OvalMessageVector* ovalMessages = collectedObject->GetMessages();
		messages = new StringVector();
		for(OvalMessageVector::iterator it = ovalMessages->begin(); it != ovalMessages->end() ; it++){
			messages->push_back((*it)->GetValue());	
		}
	} else {
		collectedObjFlag = ObjectReader::GetCollectedObjectFlag(this->GetObjectId());
		items = ObjectReader::GetItemsForObject(this->GetObjectId());	
		messages = ObjectReader::GetMessagesForObject(this->GetObjectId());
	}
	
	ComponentValue* result = new ComponentValue();
	// set the flag to the flag value of the collected object.
	result->SetFlag(collectedObjFlag);
	result->SetMessages(messages);

	// only process the items if the flag on the collected obj is COMPLETE or INCOMPLETE
	if(collectedObjFlag == OvalEnum::FLAG_COMPLETE || collectedObjFlag == OvalEnum::FLAG_INCOMPLETE) {

		if(items->size() > 0) {
			bool foundItemField = false;
			bool foundRecordField = false;
			bool recordFieldProhibited = false;
			bool recordFieldMissing = false;
			bool isRecord = false;
			for(ItemVector::iterator iterator = items->begin(); iterator != items->end(); iterator++) {
				Item* item = (*iterator);
			
				ItemEntityVector* elements = item->GetElementsByName(this->GetItemField());
				ItemEntityVector::iterator iterator1;

				for(iterator1 = elements->begin(); iterator1 != elements->end(); iterator1++) {
					// need to check status on item entity
					if((*iterator1)->GetStatus() == OvalEnum::STATUS_DOES_NOT_EXIST) {
						result->SetFlag(OvalEnum::FLAG_DOES_NOT_EXIST);
					} else if((*iterator1)->GetStatus() == OvalEnum::STATUS_NOT_COLLECTED) {
						result->SetFlag(OvalEnum::FLAG_NOT_COLLECTED);
					} else if((*iterator1)->GetStatus() == OvalEnum::STATUS_ERROR) {
						result->SetFlag(OvalEnum::FLAG_ERROR);
					} else { // exists status
						result->SetFlag(OvalEnum::FLAG_COMPLETE);
						if ( (*iterator1)->GetDatatype() != OvalEnum::DATATYPE_RECORD ){
							if ( this->GetRecordField().compare("") == 0 ){
								result->AppendValue((*iterator1)->GetValue());
							}else{
								//For item entities that are not of type record should
								//not have a value specified for the record_field attribute.
								recordFieldProhibited = true;
							}
						}else{
							isRecord = true;
							if ( this->GetRecordField().compare("") != 0 ){
								AbsEntityValueVector values = (*iterator1)->GetValues();
								for(AbsEntityValueVector::iterator iterator2 = values.begin() ; iterator2 != values.end() ; iterator2++){
									ItemFieldEntityValue *ifev = dynamic_cast<ItemFieldEntityValue*>(*iterator2);
									if (ifev) {	
										if( this->GetRecordField().compare(ifev->GetName()) == 0 ){
											result->AppendValue(ifev->GetValue());	
											foundRecordField = true;
										}
									} else
										throw Exception("Found an item entity of record datatype, with a value which is not an ItemEntityFieldValue!");
								}
							
							}else{
								//For item entities that are of type record a value must be
								//specified for the record_field attribute.
								recordFieldMissing = true;
							}
						}

					}
					foundItemField = true;
				}
				elements->clear();
				delete elements;
			}

			if(!foundItemField) {
				//If we didn't find the item entity we were looking for report an error.
				result->SetFlag(OvalEnum::FLAG_ERROR);
				result->AppendMessage("ObjectComponent::ComputeValue() - Unable to compute variable value. No elements found for the specified object_component id: " + this->GetObjectId() + " that match the specified field name: " + this->GetItemField());
			}else if ( recordFieldProhibited ){
				//For item entities that are not of type record should not have a value specified 
				//for the record_field attribute. Doing error handling like this so that we will not
				//have an error message for each matching item entity in each item.
				result->SetFlag(OvalEnum::FLAG_ERROR);
				result->AppendMessage("ObjectComponent::ComputeValue() - Unable to compute variable value. The record_field attribute can only be used with entities that are of type record.");
			}else if ( recordFieldMissing ){
				//For item entities that are of type record a value must be specified for the
				//record_field attribute. Doing error handling like this so that we will not have
				//an error message for each matching item entity in each item.
				result->SetFlag(OvalEnum::FLAG_ERROR);
				result->AppendMessage("ObjectComponent::ComputeValue() - Unable to compute variable value. A value for the record_field attribute must be specified to retrieve values from entities of type record.");
			}else if (!foundRecordField && isRecord){
				//We know that we found the item entity we were looking for now check to see if
				//we found a field with the name attribute value we were looking for. If not we
				//should report an error.								
				result->SetFlag(OvalEnum::FLAG_ERROR);
				result->AppendMessage("ObjectComponent::ComputeValue() - Unable to compute variable value. No elements found for the specified object_component id: " + this->GetObjectId() + " that match the specified item field name: " + this->GetItemField() + " and that match the specified record field name: " + this->GetRecordField() );
			}	

		} else {

			// This is an error because the collected object flag is set to COMPLETE or INCOMPLETE
			// and there are no items associated with it.
			result->SetFlag(OvalEnum::FLAG_ERROR);
			result->AppendMessage("Error no items are associated with the specified collected object: " + this->GetObjectId());
		}

	} else {

		if(errorMsg.compare("") != 0) {
			result->SetFlag(OvalEnum::FLAG_ERROR);
			result->AppendMessage("ObjectComponent::ComputeValue() - Unable to compute variable value. No items found for the specified object_component id: " + this->GetObjectId() + errorMsg);			
		} else {
			result->SetFlag(OvalEnum::FLAG_ERROR);
			result->AppendMessage("ObjectComponent::ComputeValue() - Unable to compute variable value. No items found for the specified object_component id: " + this->GetObjectId());
		}
	}

	return result;	
}

void ObjectComponent::Parse(DOMElement* ObjectComponentElm) {
	
	this->SetObjectId(XmlCommon::GetAttributeByName(ObjectComponentElm, "object_ref"));
	this->SetItemField(XmlCommon::GetAttributeByName(ObjectComponentElm, "item_field"));
	this->SetRecordField(XmlCommon::GetAttributeByName(ObjectComponentElm, "record_field"));
}

VariableValueVector ObjectComponent::GetVariableValues() {

	if(AbsDataCollector::GetIsRunning()) {
		CollectedObject* collectedObject = AbsObjectCollector::Instance()->Run(this->GetObjectId());
		return collectedObject->GetVariableValues();
	} else {
		return ObjectReader::GetVariableValuesForObject(this->GetObjectId());
	}
}
