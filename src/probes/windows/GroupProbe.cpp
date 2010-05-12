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
	ItemVector *collectedItems = new ItemVector();

	if ( group->GetOperation() == OvalEnum::OPERATION_EQUALS ){
		StringVector groups;
		//Since we are performing a lookup -- do not restrict the search scope (i.e. allow domain group lookups)
		//We are ignoring the flag for now
		/*OvalEnum::Flag flag =*/ group->GetEntityValues(groups);
		for(StringVector::iterator it = groups.begin(); it != groups.end(); it++){
			collectedItems->push_back(this->GetGroupMembers(*it));
		}
	}else{
		//Since we are performing a search -- restrict the search scope to only built-in and local accounts
		StringSet* groups = WindowsCommon::GetAllLocalGroups();
		ItemEntity* groupItemEntity = new ItemEntity("group","",OvalEnum::DATATYPE_STRING,true,OvalEnum::STATUS_EXISTS);
		for(StringSet::iterator it = groups->begin(); it != groups->end(); it++){
			groupItemEntity->SetValue(*it);
			if ( group->Analyze(groupItemEntity) == OvalEnum::RESULT_TRUE ){
				collectedItems->push_back(this->GetGroupMembers(*it));
			}
		}
		delete groups;
		delete groupItemEntity;
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

	StringSet* members = new StringSet();
	bool groupExists = WindowsCommon::ExpandGroup(groupName, members, true, false);

	if(groupExists) {
		item = this->CreateItem();
		item->SetStatus(OvalEnum::STATUS_EXISTS);

		int usersCount = 0;
		int subgroupsCount = 0;
		ItemEntityVector *entityVector = new ItemEntityVector();

		for(StringSet::iterator iterator = members->begin(); iterator != members->end(); iterator++) {
			if ( WindowsCommon::IsGroup(*iterator) ){
				// put subgroup entities at the back of the ItemEntityVector
				entityVector->push_back(new ItemEntity("subgroup", (*iterator), OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));
				++subgroupsCount;
			}else{
				// put user entities at the beginning of the ItemEntityVector
				entityVector->insert(entityVector->begin(),new ItemEntity("user", (*iterator), OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));
				++usersCount;
			}
		}

		// if there are no user members add an user entity with a status of 'does not exist'
		if ( usersCount == 0 ){
			entityVector->insert(entityVector->begin(),new ItemEntity("user", "", OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_DOES_NOT_EXIST));
		}

		// if there are no subgroup members add an subgroup entity with a status of 'does not exist'
		if ( subgroupsCount == 0 ){
			entityVector->push_back(new ItemEntity("subgroup", "", OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_DOES_NOT_EXIST));
		}

		// add the group item entity to the beginning of the group_item
		entityVector->insert(entityVector->begin(),new ItemEntity("group", groupName, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));

		item->SetElements(entityVector);
		entityVector->clear();
		delete entityVector;

	} else {

		// create an item to report that a group was looked up and it did not exist
		item = this->CreateItem();
		item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
		item->AppendElement(new ItemEntity("group", groupName, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_DOES_NOT_EXIST));
	}

	delete members;

	return item;
}
