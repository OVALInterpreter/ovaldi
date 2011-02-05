//
//
//****************************************************************************************//
// Copyright (c) 2002-2011, The MITRE Corporation
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

#include "UserProbe.h"

//****************************************************************************************//
//								UserProbe Class											  //	
//****************************************************************************************//
UserProbe* UserProbe::instance = NULL;

UserProbe::UserProbe() : AbsProbe() {
	
}

UserProbe::~UserProbe() {
  instance = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* UserProbe::Instance() {

	// Use lazy initialization
	if(instance == NULL) 
		instance = new UserProbe();

	return instance;	
}

ItemVector* UserProbe::CollectItems(Object *object) {

	// get the trustee_name from the provided object
	ObjectEntity* user = object->GetElementByName("user");
	ItemVector *collectedItems = new ItemVector();

	if ( user->GetOperation() == OvalEnum::OPERATION_EQUALS ){
		StringVector users;
		//Since we are performing a lookup -- do not restrict the search scope (i.e. allow domain user lookups)
		//We are ignoring the flag for now
		/*OvalEnum::Flag flag =*/ user->GetEntityValues(users);
		for(StringVector::iterator it = users.begin(); it != users.end(); it++){
			collectedItems->push_back(this->GetUserInfo(*it));
		}
	}else{
		//Since we are performing a search -- restrict the search scope to only built-in and local accounts
		StringSet users;
		WindowsCommon::GetAllLocalUsers(&users);
		ItemEntity* userItemEntity = new ItemEntity("user","",OvalEnum::DATATYPE_STRING,true,OvalEnum::STATUS_EXISTS);
		for(StringSet::iterator it = users.begin(); it != users.end(); it++){
			userItemEntity->SetValue(*it);
			if ( user->Analyze(userItemEntity) == OvalEnum::RESULT_TRUE ){
				collectedItems->push_back(this->GetUserInfo(*it));
			}
		}
		delete userItemEntity;
	}

	return collectedItems;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
Item* UserProbe::CreateItem() {

	Item* item = new Item(0, 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows", 
						"win-sc", 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows windows-system-characteristics-schema.xsd", 
						OvalEnum::STATUS_ERROR, 
						"user_item");

	return item;
}

Item* UserProbe::GetUserInfo(string userName) {

	Item* item = NULL;

	// get the groups
	StringSet* groups = new StringSet();
	bool userExists = WindowsCommon::GetGroupsForUser(userName, groups);
	if(userExists) {
		item = this->CreateItem();
		item->SetStatus(OvalEnum::STATUS_EXISTS);
		item->AppendElement(new ItemEntity("user", userName, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));

		// get the enabled flag
		try {
			bool enabled = WindowsCommon::GetEnabledFlagForUser(userName);
			item->AppendElement(new ItemEntity("enabled", Common::ToString(enabled), OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));
		} catch (Exception ex) {
			item->AppendElement(new ItemEntity("enabled", "", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_ERROR));
			item->AppendMessage(new OvalMessage(ex.GetErrorMessage(), OvalEnum::LEVEL_ERROR));
		}

		StringSet::iterator iterator;
		if(groups->size() > 0) {
			for(iterator = groups->begin(); iterator != groups->end(); iterator++) {
				item->AppendElement(new ItemEntity("group", (*iterator), OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));
			}
		} else {
			item->AppendElement(new ItemEntity("group", "", OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_DOES_NOT_EXIST));
		}
		delete groups;
	} else {
		item = this->CreateItem();
		item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
		item->AppendElement(new ItemEntity("user", userName, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_DOES_NOT_EXIST));
	}

	return item;
}

StringSet* UserProbe::GetAllUsers() {

	StringSet* users = new StringSet();

	// just call windows common method to get local users.
	WindowsCommon::GetAllLocalUsers(users);

	return users;
}
