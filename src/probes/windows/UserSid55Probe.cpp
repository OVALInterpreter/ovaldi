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

#include "UserSid55Probe.h"

//****************************************************************************************//
//								UserSid55Probe Class									  //	
//****************************************************************************************//
UserSid55Probe* UserSid55Probe::instance = NULL;

UserSid55Probe::UserSid55Probe() : AbsProbe() {

}

UserSid55Probe::~UserSid55Probe() {
	instance = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* UserSid55Probe::Instance() {

	// Use lazy initialization
	if(instance == NULL) {
		instance = new UserSid55Probe();
	}

	return instance;	
}

ItemVector* UserSid55Probe::CollectItems(Object *object) {

	ObjectEntity* userSid = object->GetElementByName("user_sid");
	ItemVector *collectedItems = new ItemVector();

	if ( userSid->GetOperation() == OvalEnum::OPERATION_EQUALS ){
		StringVector userSids;
		//Since we are performing a lookup -- do not restrict the search scope (i.e. allow domain user lookups)
		//We are ignoring the flag for now
		/*OvalEnum::Flag flag =*/ userSid->GetEntityValues(userSids);
		for(StringVector::iterator it = userSids.begin(); it != userSids.end(); it++){
			collectedItems->push_back(this->GetUserSidInfo(*it));
		}
	}else{
		//Since we are performing a search -- restrict the search scope to only built-in and local accounts
		StringSet* userSids = WindowsCommon::GetAllLocalUserSids();
		ItemEntity* userSidItemEntity = new ItemEntity("user_sid","",OvalEnum::DATATYPE_STRING,true,OvalEnum::STATUS_EXISTS);

		for(StringSet::iterator it = userSids->begin(); it != userSids->end(); it++){
			userSidItemEntity->SetValue(*it);
			if ( userSid->Analyze(userSidItemEntity) == OvalEnum::RESULT_TRUE ){
				collectedItems->push_back(this->GetUserSidInfo(*it));
			}
		}
		delete userSidItemEntity;
	}

	return collectedItems;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
Item* UserSid55Probe::CreateItem() {

	Item* item = new Item(0, 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows", 
						"win-sc", 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows windows-system-characteristics-schema.xsd", 
						OvalEnum::STATUS_ERROR, 
						"user_sid_item");

	return item;
}

Item* UserSid55Probe::GetUserSidInfo(string userSid) {

	Item* item = NULL;

	string userName;
	string domain;

	WindowsCommon::LookUpTrusteeSid(userSid, &userName, &domain);

	// User is ?always? from local box so be sure to remove the "\" separation character
	size_t idx = userName.rfind('\\', 0);

	if((idx != string::npos) && (userName.length() != idx + 1)) {
		
		userName = userName.substr(idx + 1, string::npos);
	}

	StringSet* groups = new StringSet();
	
	bool userExists = WindowsCommon::GetGroupsForUser(userName, groups);		

	if(userExists) {
		StringSet* groupSids = new StringSet();
	
		WindowsCommon::ConvertTrusteeNamesToSidStrings(groups, groupSids);

		item = this->CreateItem();
		item->SetStatus(OvalEnum::STATUS_EXISTS);
		item->AppendElement(new ItemEntity("user_sid", userSid, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));

		// get the enabled flag
		try {
			bool enabled = WindowsCommon::GetEnabledFlagForUser(userName);
			item->AppendElement(new ItemEntity("enabled", Common::ToString(enabled), OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));
		} catch (Exception ex) {
			item->AppendElement(new ItemEntity("enabled", "", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_ERROR));
			item->AppendMessage(new OvalMessage(ex.GetErrorMessage(), OvalEnum::LEVEL_ERROR));
		}

		StringSet::iterator iterator;
		if(groupSids->size() > 0) {
			for(iterator = groupSids->begin(); iterator != groupSids->end(); iterator++) {
				item->AppendElement(new ItemEntity("group_sid", (*iterator), OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));
			}
		} else {
			item->AppendElement(new ItemEntity("group_sid", "", OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_DOES_NOT_EXIST));
		}

		delete groupSids;
	} else {
		item = this->CreateItem();
		item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
		item->AppendElement(new ItemEntity("user_sid", userSid, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_DOES_NOT_EXIST));
	}
		
	delete groups;

	return item;
}