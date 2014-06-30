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

#include "WindowsCommon.h"
#include <VectorPtrGuard.h>

#include "UserSidProbe.h"

using namespace std;

//****************************************************************************************//
//								UserSidProbe Class										  //	
//****************************************************************************************//
UserSidProbe* UserSidProbe::instance = NULL;

UserSidProbe::UserSidProbe() : AbsProbe() {

}

UserSidProbe::~UserSidProbe() {
	instance = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* UserSidProbe::Instance() {

	// Use lazy initialization
	if(instance == NULL) {
		instance = new UserSidProbe();
	}

	return instance;	
}

ItemVector* UserSidProbe::CollectItems(Object *object) {

	ObjectEntity* userSid = object->GetElementByName("user");
	ItemVector *collectedItems = new ItemVector();

	if ( userSid->GetOperation() == OvalEnum::OPERATION_EQUALS ){
		StringVector userSids;
		//Since we are performing a lookup -- do not restrict the search scope (i.e. allow domain user lookups)
		//We are ignoring the flag for now
		/*OvalEnum::Flag flag =*/ userSid->GetEntityValues(userSids);
		ItemEntity userIe("user");
		for(StringVector::iterator it = userSids.begin(); it != userSids.end(); it++){
			userIe.SetValue(*it);
			if (userSid->GetVarRef() == NULL ||
				userSid->Analyze(&userIe) == OvalEnum::RESULT_TRUE) {
				Item *item = this->GetUserSidInfo(*it);
				if (item) 
					collectedItems->push_back(item);
				else {
					item = this->CreateItem();
					item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
					item->AppendElement(new ItemEntity("user_sid", "",
						OvalEnum::DATATYPE_STRING, 
						OvalEnum::STATUS_DOES_NOT_EXIST));
					collectedItems->push_back(item);
				}
			}
		}
	}else{
		//Since we are performing a search -- restrict the search scope to only built-in and local accounts
		StringSet* userSids = WindowsCommon::GetAllLocalUserSids();
		ItemEntity userSidItemEntity("user","",OvalEnum::DATATYPE_STRING,OvalEnum::STATUS_EXISTS);
		for(StringSet::iterator it = userSids->begin(); it != userSids->end(); it++){
			userSidItemEntity.SetValue(*it);
			if ( userSid->Analyze(&userSidItemEntity) == OvalEnum::RESULT_TRUE ){
				Item *item = this->GetUserSidInfo(*it);
				if (item) collectedItems->push_back(item);
			}
		}
	}


	return collectedItems;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
Item* UserSidProbe::CreateItem() {

	Item* item = new Item(0, 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows", 
						"win-sc", 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows windows-system-characteristics-schema.xsd", 
						OvalEnum::STATUS_ERROR, 
						"user_sid_item");

	return item;
}

Item* UserSidProbe::GetUserSidInfo(string userSid) {

	auto_ptr<Item> item;
	auto_ptr<ItemEntity> enabledIe, lastLogonIe;
	VectorPtrGuard<ItemEntity> groupSidEntities(new vector<ItemEntity*>());
	string userName;
	string domain;
	bool isGroup;

	if (!WindowsCommon::LookUpTrusteeSid(userSid, &userName, &domain, &isGroup))
		return NULL;

	item.reset(CreateItem());

	// get the enabled flag
	try {
		bool enabled;
		DWORD lastLogon;
		if (!WindowsCommon::GetAccountInfo(userName, &enabled, &lastLogon))
			return NULL;

		enabledIe.reset(new ItemEntity("enabled", 
			Common::ToString(enabled), OvalEnum::DATATYPE_BOOLEAN));
		lastLogonIe.reset(new ItemEntity("last_logon",
			Common::ToString(lastLogon), OvalEnum::DATATYPE_INTEGER));
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
			return NULL;

		StringSet groupSids;
		WindowsCommon::ConvertTrusteeNamesToSidStrings(&groups, &groupSids);

		if (groupSids.empty())
			groupSidEntities->push_back(new ItemEntity("group_sid", "",
				OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));
		else
			for (StringSet::iterator iter = groupSids.begin();
				iter != groupSids.end();
				++iter)
				groupSidEntities->push_back(
					new ItemEntity("group_sid", *iter));
	} catch (Exception ex) {
		// make sure the vector is clear; we only want one entity,
		// with status=error.
		for (vector<ItemEntity*>::iterator iter = groupSidEntities->begin();
			iter != groupSidEntities->end();
			++iter)
			if (*iter) delete *iter;
		groupSidEntities->clear();
		groupSidEntities->push_back(new ItemEntity("group_sid", "", 
			OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_ERROR));
		item->AppendMessage(new OvalMessage(ex.GetErrorMessage(),
			OvalEnum::LEVEL_ERROR));
	}


	item->SetStatus(OvalEnum::STATUS_EXISTS);
	item->AppendElement(new ItemEntity("user_sid", userSid));
	item->AppendElement(enabledIe.release());

	for (vector<ItemEntity*>::iterator iter = groupSidEntities->begin();
		iter != groupSidEntities->end();
		++iter)
		item->AppendElement(*iter);
	groupSidEntities->clear(); // item now owns the entities

	/* Uncomment this when the schemas bundled with
	   OVALDI are updated to include this entity.
	item->AppendElement(lastLogonIe.release());
	*/

	return item.release();
}