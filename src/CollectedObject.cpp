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

#include "CollectedObject.h"

//****************************************************************************************//
//								CollectedObject Class									  //	
//****************************************************************************************//
CollectedObjectMap CollectedObject::collectedObjectsMap;


CollectedObject::CollectedObject(string id, string comment, int version, int variableInstance, OvalEnum::Flag flag) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Create a complete CollectedObject object
	//
	// -----------------------------------------------------------------------

	this->SetId(id);
	this->SetComment(comment);
	this->SetVariableInstance(variableInstance);
	this->SetVersion(version);
	this->SetFlag(flag);
}

CollectedObject::~CollectedObject() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	clean up all the elements
	//
	// -----------------------------------------------------------------------
	
	OvalMessage* msg = NULL;
	while(this->messages.size() != 0) {
	  	msg = messages[messages.size()-1];
	  	messages.pop_back();
	  	delete msg;
	  	msg = NULL;
	}

	// Don't delete variable values they are shared and cached. There is a clear 
	// cache method in the var value class taht is called at the end of run time
}

// ***************************************************************************************	//
//								Static Public members										//
// ***************************************************************************************	//
CollectedObject* CollectedObject::Create(AbsObject* absObject) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Create a new ColelctedObject ans store a reference to it.
	//
	// -----------------------------------------------------------------------

	CollectedObject* collectedObject = new CollectedObject(absObject->GetId(), absObject->GetComment(), absObject->GetVersion());
	CollectedObject::Cache(collectedObject);
	return collectedObject;
}

CollectedObject* CollectedObject::CreateError(string objectId) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Create a new ColelctedObject based on the specified AbsObject
	//	Set it up as not applicable.
	//
	// -----------------------------------------------------------------------

	CollectedObject* collectedObject = new CollectedObject(objectId);
	collectedObject->SetFlag(OvalEnum::FLAG_ERROR);
	CollectedObject::Cache(collectedObject);
	return collectedObject;
}

CollectedObject* CollectedObject::CreateNotApplicable(AbsObject* absObject) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Create a new ColelctedObject based on the specified AbsObject
	//	Set it up as not applicable.
	//
	// -----------------------------------------------------------------------

	CollectedObject* collectedObject = CollectedObject::Create(absObject);
	collectedObject->SetFlag(OvalEnum::FLAG_NOT_APPLICABLE);

	OvalMessage* message = new OvalMessage("This object was determined to be not applicable by this OVAL Definition Interpreter data collector.");
	collectedObject->AppendOvalMessage(message);
	
	return collectedObject;
}

CollectedObject* CollectedObject::CreateNotSupported(AbsObject* absObject) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Create a new CollectedObject based on the specified AbsObject
	//	Set it up as not supported.
	//
	// -----------------------------------------------------------------------

	CollectedObject* collectedObject = CollectedObject::Create(absObject);
	collectedObject->SetFlag(OvalEnum::FLAG_NOT_COLLECTED);

	OvalMessage* message = new OvalMessage("This type of object (" + absObject->GetName() + ") is not currently supported by this OVAL Definition Interpreter data collector.");
	collectedObject->AppendOvalMessage(message);
	return collectedObject;
}

void CollectedObject::WriteCollectedObjects() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Write all collected objects in the vector of collected objects.
	//	After writing delete the object to free memory.
	//
	// -----------------------------------------------------------------------

	CollectedObjectMap::iterator iterator;
	for(iterator = CollectedObject::collectedObjectsMap.begin(); iterator != CollectedObject::collectedObjectsMap.end(); iterator++) {
		
		CollectedObject* obj = iterator->second;
		obj->Write(DocumentManager::GetSystemCharacterisitcsDocument(), AbsDataCollector::Instance()->GetSCCollectedObjectsElm());
		delete obj;
		obj = NULL;
	}
	
	CollectedObject::collectedObjectsMap.clear();
}

CollectedObject* CollectedObject::GetCollectedObject(string objectId) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	return the collected object that corresponds to the object id 
	//	specified. Return NULL if the object id is not found.
	// -----------------------------------------------------------------------

	CollectedObject* colelctedObject = NULL;

	CollectedObjectMap::iterator iterator;
	iterator = CollectedObject::collectedObjectsMap.find(objectId);
	if(iterator != CollectedObject::collectedObjectsMap.end()) {
		colelctedObject = iterator->second;
	} 

	return colelctedObject;
}

// ***************************************************************************************	//
//								 Public members												//
// ***************************************************************************************	//
OvalEnum::Flag CollectedObject::GetFlag() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the flag field's value
	//
	// -----------------------------------------------------------------------

	return this->flag;
}

void CollectedObject::SetFlag(OvalEnum::Flag flag) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the flag field's value
	//
	// -----------------------------------------------------------------------

	this->flag = flag;
}

string CollectedObject::GetId() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the id field's value
	//
	// -----------------------------------------------------------------------

	return this->id;
}

void CollectedObject::SetId(string id) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the id field's value
	//
	// -----------------------------------------------------------------------

	this->id = id;
}

string CollectedObject::GetComment() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the comment field's value
	//
	// -----------------------------------------------------------------------

	return this->comment;
}

void CollectedObject::SetComment(string comment) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the comment field's value
	//
	// -----------------------------------------------------------------------

	this->comment = comment;
}

OvalMessageVector* CollectedObject::GetMessages() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the messages field's value
	//
	// -----------------------------------------------------------------------
	return &this->messages;
}
void CollectedObject::SetMessages(OvalMessageVector* messages) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the messages field's value
	//
	// -----------------------------------------------------------------------
	
	this->messages = (*messages);
}

ItemVector* CollectedObject::GetReferences() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the references field's value
	//
	// -----------------------------------------------------------------------

	return &this->references;
}

void CollectedObject::SetReferences(ItemVector* references) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the references field's value
	//
	// -----------------------------------------------------------------------

	this->references = (*references);
}

int CollectedObject::GetVariableInstance() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the variableInstance field's value
	//
	// -----------------------------------------------------------------------

	return this->variableInstance;
}

void CollectedObject::SetVariableInstance(int variableInstance) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the variableInstance field's value
	//
	// -----------------------------------------------------------------------

	this->variableInstance = variableInstance;
}

VariableValueVector* CollectedObject::GetVariableValues() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the variableValues field's value
	//
	// -----------------------------------------------------------------------

	return &this->variableValues;
}

void CollectedObject::SetVariableValues(VariableValueVector* variableValues) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the variableValues field's value
	//
	// -----------------------------------------------------------------------

	this->variableValues = (*variableValues);
}

int CollectedObject::GetVersion() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the version field's value
	//
	// -----------------------------------------------------------------------

	return this->version;
}

void CollectedObject::SetVersion(int version) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the version field's value
	//
	// -----------------------------------------------------------------------

	this->version = version;
}

void CollectedObject::AppendOvalMessage(OvalMessage* message) {

	this->messages.push_back(message);
}

void CollectedObject::AppendReferencesAndComputeFlag(ItemVector* references) {

	this->AppendReferences(references);

	// compute the flag value for the collected obj beased on the set of items

	// get counts of each status value set on the items in the list
	unsigned int errorCnt = 0;
	unsigned int existsCnt = 0;
	unsigned int doesNotExistCnt = 0;
	unsigned int notCollectedCnt = 0;

	// now loop through the items and examine their status values.
	ItemVector::iterator iterator;
	for(iterator = references->begin(); iterator != references->end(); iterator++) {
		Item* tmp = (*iterator);

		if(tmp->GetStatus() == OvalEnum::STATUS_ERROR) {
			errorCnt++;
		} else if(tmp->GetStatus() == OvalEnum::STATUS_EXISTS) {
			existsCnt++;
		} else if(tmp->GetStatus() == OvalEnum::STATUS_DOES_NOT_EXIST) {
			doesNotExistCnt++;
		} else if(tmp->GetStatus() == OvalEnum::STATUS_NOT_COLLECTED) {
			notCollectedCnt++;
		}
	}

	// status values -  error, exists, does not exist, not collected

	// Flag == error
	// if the set contains any item marked as error
	if(errorCnt > 0) {
		this->SetFlag(OvalEnum::FLAG_ERROR);

	// Flag == complete
	// if the set contains
	//   at least one item with a status of exists and
	//   no items with a status of not collected and
	//   no items with a status of error
	} else if(errorCnt == 0 && notCollectedCnt == 0 && existsCnt > 0) {
		this->SetFlag(OvalEnum::FLAG_COMPLETE);

	// Flag == incomplete
	// if the set contains
	//  one ore more items with a status of not collected
	//  no items with a status of error
	} else if(errorCnt == 0 && notCollectedCnt > 0) {
		this->SetFlag(OvalEnum::FLAG_INCOMPLETE);

	// Flag == does not exist
	// if the set is empty, or the set contains
	//   no items with a status of exists
	//   no items with a status of error
	//   no items with a status of not collected
	//   all items have a status of does not exist
	} else if((errorCnt == 0 && notCollectedCnt == 0 && existsCnt == 0) || references->size() == 0) {
		this->SetFlag(OvalEnum::FLAG_DOES_NOT_EXIST);

	// Flag == not collected
	// if all items in the set have a status of not collected and
	// the set has atleast one item
	//
	// NOTE: This condition is currently handled elsewhere in 
	// the code see: CollectedObject* AbsObjectCollector::ProcessObject(Object* object)
	//
	} else if(references->size() == notCollectedCnt && notCollectedCnt > 0) {
		this->SetFlag(OvalEnum::FLAG_DOES_NOT_EXIST);
	}

	// Flag == not applicable
	// 
	// handled elsewhere in the code see: CollectedObject* AbsObjectCollector::ProcessObject(Object* object)

}

void CollectedObject::AppendReferences(ItemVector* references) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Add the items to the end of the references vector
	//
	// -----------------------------------------------------------------------

	ItemVector::iterator iterator;
	for(iterator = references->begin(); iterator != references->end(); iterator++) {
		Item* item = (*iterator);
		this->references.push_back(item);
	}
}

void CollectedObject::AppendVariableValue(VariableValue* variableValue) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Add a varaible value to the end of the variable values vector
	//
	// -----------------------------------------------------------------------

	this->variableValues.push_back(variableValue);
}

void CollectedObject::AppendVariableValues(VariableValueVector* vars) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Add a variable value to the end of the variable values vector
	//
	// -----------------------------------------------------------------------

	VariableValueVector::iterator iterator;
	for(iterator = vars->begin(); iterator != vars->end(); iterator++) {
		VariableValue* var = (*iterator);
		this->variableValues.push_back(var);
	}
}

void CollectedObject::Write(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* scFile, DOMElement* collectedObjectsElm) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Write this item to a sc file. Make sure the item has not already been 
	//	written to the sc file
	//
	// -----------------------------------------------------------------------

	// Create a new object element
	DOMElement *newCollectedObjectElem = XmlCommon::AddChildElement(scFile, collectedObjectsElm, "object");

	// Add the attributes
	// handling defaults in the schema
	XmlCommon::AddAttribute(newCollectedObjectElem, "id", this->GetId());	

	string strFlag = OvalEnum::FlagToString(this->GetFlag());
	XmlCommon::AddAttribute(newCollectedObjectElem, "flag", strFlag);

	if(this->GetComment().compare("") != 0) {
		XmlCommon::AddAttribute(newCollectedObjectElem, "comment", this->GetComment());	
	}

	string version = Common::ToString(this->GetVersion());
	XmlCommon::AddAttribute(newCollectedObjectElem, "version", version);

	// Add any messges
	if(this->GetMessages()->size() > 0) {
		OvalMessageVector::iterator messageIterator;
		for(messageIterator = this->GetMessages()->begin(); messageIterator != this->GetMessages()->end(); messageIterator++) {
			OvalMessage* message = (*messageIterator);
			message->Write(scFile, newCollectedObjectElem, "oval-sc");
		}
	}

	// Call the write method for each variable_value - ensure that each var value is only written once
	if(this->GetVariableValues()->size() > 0) {
		StringPairVector varIdValuePairs;
		VariableValueVector::iterator variableValueIterator;
		for(variableValueIterator = this->GetVariableValues()->begin(); variableValueIterator != this->GetVariableValues()->end(); variableValueIterator++) {
			VariableValue* variableValue = (*variableValueIterator);
			if(!this->IsWritten(&varIdValuePairs, variableValue->GetId(), variableValue->GetValue())) {
				StringPair* pair = new StringPair();
				pair->first = variableValue->GetId();
				pair->second = variableValue->GetValue();
				varIdValuePairs.push_back(pair);
				variableValue->Write(newCollectedObjectElem);
			}
		}
	}

	// Add each reference - ensure taht each refernec is only written once.
	if(this->GetReferences()->size() > 0) {
		IntVector referenceIds;
		ItemVector::iterator referenceIterator;
		for(referenceIterator = this->GetReferences()->begin(); referenceIterator != this->GetReferences()->end(); referenceIterator++) {
			Item* reference = (*referenceIterator);
			
			if(!this->IsWritten(&referenceIds, reference->GetId())) {
				referenceIds.push_back(reference->GetId());

				// add the item to the sc file
				reference->Write(scFile, AbsDataCollector::Instance()->GetSCSystemDataElm());

				// add the reference tot he collected obj element
				string refElementName = "reference";
				DOMElement *newReferenceElm = scFile->createElement(XMLString::transcode(refElementName.c_str()));
				newCollectedObjectElem->appendChild(newReferenceElm);
				string idStr = Common::ToString(reference->GetId());
				XmlCommon::AddAttribute(newReferenceElm, "item_ref", idStr);
			}
		}
	}
}

// ***************************************************************************************	//
//								 Private members											//
// ***************************************************************************************	//
bool CollectedObject::IsWritten(StringPairVector* varIdValuePairs, string varId, string varValue) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	return true if the specified variable value has not already been written
	//  must check the pair of var id and value.
	// -----------------------------------------------------------------------

	bool result = false;

	StringPairVector::iterator iterator;
	for(iterator = varIdValuePairs->begin(); iterator != varIdValuePairs->end(); iterator++) {
		string id = (*iterator)->first;
		if(id.compare(varId) == 0) {
			string value = (*iterator)->second;
			if(value.compare(varValue) == 0) {
				result = true;
                break;
			}
		}
	}
	return result;
}

bool CollectedObject::IsWritten(IntVector* itemIds, int itemId) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	return true if the specified item id has already been 
	//	added as a reference
	// -----------------------------------------------------------------------

	bool result = false;

	IntVector::iterator iterator;
	for(iterator = itemIds->begin(); iterator != itemIds->end(); iterator++) {
		int id = (*iterator);
		if(id == itemId) {
			result = true;
               break;
		}
	}
	return result;
}

void CollectedObject::Cache(CollectedObject* collectedObject) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Add the provided collected object to the cache of collected objects 
	//	if it does not already exist
	// -----------------------------------------------------------------------

	if(CollectedObject::GetCollectedObject(collectedObject->GetId()) == NULL) {
		// does not already exist so add it
		CollectedObject::collectedObjectsMap.insert(CollectedObjectPair(collectedObject->GetId(), collectedObject));
	}
}

//****************************************************************************************//
//								CollectedObjectException Class							  //	
//****************************************************************************************//
CollectedObjectException::CollectedObjectException(string errMsgIn, int severity, Exception* ex) : Exception(errMsgIn, severity, ex) {

}

CollectedObjectException::~CollectedObjectException() {

}

