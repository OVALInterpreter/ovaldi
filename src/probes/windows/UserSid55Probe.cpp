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

	// check datatypes - only allow string
	if(userSid->GetDatatype() != OvalEnum::DATATYPE_STRING) {
		throw ProbeException("Error: invalid data type specified on user_sid. Found: " + OvalEnum::DatatypeToString(userSid->GetDatatype()));
	}	

	// check operation - only allow  equals, not equals and pattern match
	if(userSid->GetOperation() != OvalEnum::OPERATION_EQUALS && userSid->GetOperation() != OvalEnum::OPERATION_PATTERN_MATCH && userSid->GetOperation() != OvalEnum::OPERATION_NOT_EQUAL) {
		throw ProbeException("Error: invalid operation specified on user_sid. Found: " + OvalEnum::OperationToString(userSid->GetOperation()));
	}

	// behaviors are not allowed on user_sids
	if(object->GetBehaviors()->size() != 0) {
		throw ProbeException("Error user_sid55_objects do not support behaviors.");		
	}

	ItemVector *collectedItems = new ItemVector();

	// get the user data
	if(userSid->GetVarRef() == NULL) {
		if(userSid->GetOperation() == OvalEnum::OPERATION_EQUALS) {
			// simply get user sid if it exists
			Item* item=  this->GetUserSidInfo(userSid->GetValue());
			if(item != NULL) {
				collectedItems->push_back(item);
			} 
		} else {

			bool isRegex = false;
			if(userSid->GetOperation() == OvalEnum::OPERATION_PATTERN_MATCH)
				isRegex = true;

			// Get all users SIDs on the system...
			StringVector* allUserSids = WindowsCommon::GetAllLocalUserSids();
			
			// Get the set of users that match the ItemEntity.
			StringVector::iterator iterator;
			for(iterator = allUserSids->begin(); iterator != allUserSids->end(); iterator++) {
				string curr = (*iterator);
				if(this->IsMatch(userSid->GetValue(), (*iterator), isRegex)) {
					Item* item =  this->GetUserSidInfo(*iterator);
					if(item != NULL) {
						collectedItems->push_back(item);
					}
				}
			}
		}
	} else {
		// Get all users on the system...
		StringVector* allLocalUserSids = WindowsCommon::GetAllLocalUserSids();

		// loop through all users on the system
		// only keep those that match operation and value and var check
		StringVector::iterator it;
		ItemEntity* tmp = this->CreateItemEntity(userSid);
		for(it = allLocalUserSids->begin(); it != allLocalUserSids->end(); it++) {
			tmp->SetValue((*it));
			if(userSid->Analyze(tmp) == OvalEnum::RESULT_TRUE) {
				Item* item = this->GetUserSidInfo((*it));
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
Item* UserSid55Probe::CreateItem() {

	Item* item = new Item(0, 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows", 
						"win-sc", 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows windows-system-characteristics-schema.xsd", 
						OvalEnum::STATUS_ERROR, 
						"user_sid55_item");

	return item;
}

Item* UserSid55Probe::GetUserSidInfo(string userSid) {

	Item* item = NULL;

	string userName;
	string domain;

	WindowsCommon::LookUpTrusteeSid(userSid, &userName, &domain);

	// User is ?always? from local box so be sure to remove the "\" separation character
	size_t idx = userName.find('\\', 0);

	if((idx != string::npos) && (userName.length() != idx + 1)) {
		
		userName = userName.substr(idx + 1, string::npos);
	}

	StringVector* groups = new StringVector();
	
	bool userExists = WindowsCommon::GetGroupsForUser(userName, groups);		

	if(userExists) {
		StringVector* groupSids = new StringVector();
	
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

		StringVector::iterator iterator;
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