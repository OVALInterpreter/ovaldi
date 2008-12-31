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

#include "GroupProbe.h"

//****************************************************************************************//
//								GroupProbe Class											  //	
//****************************************************************************************//
GroupProbe* GroupProbe::instance = NULL;

GroupProbe::GroupProbe() : AbsProbe() {
}

GroupProbe::~GroupProbe() {
  instance = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* GroupProbe::Instance() {

	// Use lazy initialization
	if(instance == NULL) 
		instance = new GroupProbe();

	return instance;	
}

ItemVector* GroupProbe::CollectItems(Object *object) {

	// get the trustee_name from the provided object
	ObjectEntity* group = object->GetElementByName("group");

	// check datatypes - only allow string
	if(group->GetDatatype() != OvalEnum::DATATYPE_STRING) {
		throw ProbeException("Error: invalid data type specified on group. Found: " + OvalEnum::DatatypeToString(group->GetDatatype()));
	}	

	// check operation - only allow  equals, not equals and pattern match
	if(group->GetOperation() != OvalEnum::OPERATION_EQUALS && group->GetOperation() != OvalEnum::OPERATION_PATTERN_MATCH && group->GetOperation() != OvalEnum::OPERATION_NOT_EQUAL) {
		throw ProbeException("Error: invalid operation specified on group. Found: " + OvalEnum::OperationToString(group->GetOperation()));
	}

	// behaviors are not allowed on groups
	if(object->GetBehaviors()->size() != 0) {
		throw ProbeException("Error group_objects do not support behaviors.");		
	}

	ItemVector *collectedItems = new ItemVector();

	// get the group data
	if(group->GetVarRef() == NULL) {
		if(group->GetOperation() == OvalEnum::OPERATION_EQUALS) {
			// simply get the members of the group if it exists
			Item* item =  this->GetGroupMembers(group->GetValue());
			if(item != NULL) {
				collectedItems->push_back(item);
			}
		} else {

			bool isRegex = false;
			if(group->GetOperation() == OvalEnum::OPERATION_PATTERN_MATCH)
				isRegex = true;

			// Get all groups on the system...
			StringVector* allGroups = WindowsCommon::GetAllGroups();
			
			// Get the set of groups that match the ItemEntity.
			StringVector::iterator iterator;
			for(iterator = allGroups->begin(); iterator != allGroups->end(); iterator++) {
				string curr = (*iterator);
				if(this->IsMatch(group->GetValue(), (*iterator), isRegex)) {
					Item* item =  this->GetGroupMembers((*iterator));
					if(item != NULL) {
						collectedItems->push_back(item);
					}
				}
			}
		}

	} else {
		// Get all groups on the system...
		StringVector* allGroups = WindowsCommon::GetAllGroups();

		// loop through all groups on the system
		// only keep those that match operation and value and var check
		StringVector::iterator it;
		ItemEntity* tmp = this->CreateItemEntity(group);
		for(it = allGroups->begin(); it != allGroups->end(); it++) {
			tmp->SetValue((*it));
			if(group->Analyze(tmp) == OvalEnum::RESULT_TRUE) {
				Item* item = this->GetGroupMembers((*it));
				if(item != NULL) {
					collectedItems->push_back(item);
				}
			}
		}
	}

	return collectedItems;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
Item* GroupProbe::CreateItem() {

	Item* item = new Item(0, 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows", 
						"win-sc", 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows windows-system-characteristics-schema.xsd", 
						OvalEnum::STATUS_ERROR, 
						"group_item");

	return item;
}

Item* GroupProbe::GetGroupMembers(string groupName) {
	Item* item = NULL;

	StringVector* members = new StringVector();
	bool groupExists = WindowsCommon::ExpandGroup(groupName, members);

	if(groupExists) {
		item = this->CreateItem();
		item->SetStatus(OvalEnum::STATUS_EXISTS);
		item->AppendElement(new ItemEntity("group", groupName, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));

		StringVector::iterator iterator;
		if(members->size() > 0) {
			for(iterator = members->begin(); iterator != members->end(); iterator++) {
				item->AppendElement(new ItemEntity("user", (*iterator), OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));
			}
		} else {
			item->AppendElement(new ItemEntity("user", "", OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_DOES_NOT_EXIST));
		}

	} else {

		// create an item to report that a group was looked up 
		// and it did not exist
		item = this->CreateItem();
		item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
		item->AppendElement(new ItemEntity("group", groupName, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_DOES_NOT_EXIST));
	}

	delete members;

	return item;
}
