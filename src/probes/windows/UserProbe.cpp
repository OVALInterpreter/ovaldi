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

#include <memory>

#include <VectorPtrGuard.h>
#include "WindowsCommon.h"

#include "UserProbe.h"

using namespace std;

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
	VectorPtrGuard<Item> collectedItems(new ItemVector());

	if ( user->GetOperation() == OvalEnum::OPERATION_EQUALS ){
		StringVector users;
		//Since we are performing a lookup -- do not restrict the search scope (i.e. allow domain user lookups)
		//We are ignoring the flag for now
		/*OvalEnum::Flag flag =*/ user->GetEntityValues(users);
		ItemEntity userItemEntity("user");
		for(StringVector::iterator it = users.begin(); it != users.end(); it++){
			userItemEntity.SetValue(*it);
			if (user->GetVarRef() == NULL ||
				user->Analyze(&userItemEntity) == OvalEnum::RESULT_TRUE) {
				Item *item = this->GetUserInfo(*it);
				if (item)
					collectedItems->push_back(item);
				else {
					item = CreateItem();
					item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
					item->AppendElement(new ItemEntity("user", "", 
						OvalEnum::DATATYPE_STRING, 
						OvalEnum::STATUS_DOES_NOT_EXIST));
					collectedItems->push_back(item);
				}
			}
		}
	}else{
		//Since we are performing a search -- restrict the search scope to only built-in and local accounts
		StringSet users;
		WindowsCommon::GetAllLocalUsers(&users);
		ItemEntity userItemEntity("user","",OvalEnum::DATATYPE_STRING,OvalEnum::STATUS_EXISTS);
		for(StringSet::iterator it = users.begin(); it != users.end(); it++){
			userItemEntity.SetValue(*it);
			if ( user->Analyze(&userItemEntity) == OvalEnum::RESULT_TRUE ){
				Item *item = this->GetUserInfo(*it);
				if (item) collectedItems->push_back(item);
			}
		}
	}

	return collectedItems.release();
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

Item* UserProbe::GetUserInfo(const string &userName) {

	auto_ptr<Item> item;
	auto_ptr<ItemEntity> enabledIe, lastLogonIe;
	VectorPtrGuard<ItemEntity> groupEntities(new vector<ItemEntity*>());
	bool enabled;
	DWORD timeStamp;

	item.reset(CreateItem());

	try {
		if (!WindowsCommon::GetAccountInfo(userName, &enabled, &timeStamp))
			return NULL;

		enabledIe.reset(new ItemEntity("enabled", (enabled ? "true":"false"),
			OvalEnum::DATATYPE_BOOLEAN));
		lastLogonIe.reset(new ItemEntity("last_logon", Common::ToString(timeStamp),
			OvalEnum::DATATYPE_INTEGER));
	} catch (Exception ex) {
		enabledIe.reset(new ItemEntity("enabled", "",
			OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_ERROR));
		lastLogonIe.reset(new ItemEntity("last_logon", "",
			OvalEnum::DATATYPE_INTEGER, OvalEnum::STATUS_ERROR));
		item->AppendMessage(new OvalMessage(ex.GetErrorMessage(), 
			OvalEnum::LEVEL_ERROR));
	}

	try {
		StringSet groups;
		if (!WindowsCommon::GetGroupsForUser(userName, &groups))
			// user existed when GetAccountInfo() above was called,
			// but it has since disappeared!
			return NULL;

		if (groups.empty())
			groupEntities->push_back(new ItemEntity("group", "", 
				OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));
		else
			for (StringSet::iterator iter = groups.begin();
				iter != groups.end();
				++iter)
				groupEntities->push_back(new ItemEntity("group", *iter));
	} catch (Exception ex) {
		// make sure the vector is clear; we only want one entity,
		// with status=error.
		for (vector<ItemEntity*>::iterator iter = groupEntities->begin();
			iter != groupEntities->end();
			++iter)
			delete *iter;
		groupEntities->clear();
		item->AppendElement(new ItemEntity("group", "", OvalEnum::DATATYPE_STRING,
			OvalEnum::STATUS_ERROR));
		item->AppendMessage(new OvalMessage(ex.GetErrorMessage(), 
			OvalEnum::LEVEL_ERROR));
	}

	item->SetStatus(OvalEnum::STATUS_EXISTS);
	item->AppendElement(new ItemEntity("user", userName));
	item->AppendElement(enabledIe.release());

	vector<ItemEntity*>::iterator iterator;
	for(iterator = groupEntities->begin(); 
		iterator != groupEntities->end(); 
		iterator++)
		item->AppendElement(*iterator);
	groupEntities->clear(); // item now has ownership of the entities!

	item->AppendElement(lastLogonIe.release());

	return item.release();
}

StringSet* UserProbe::GetAllUsers() {

	StringSet* users = new StringSet();

	// just call windows common method to get local users.
	WindowsCommon::GetAllLocalUsers(users);

	return users;
}
