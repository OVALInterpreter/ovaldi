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

	// check datatypes - only allow string
	if(user->GetDatatype() != OvalEnum::DATATYPE_STRING) {
		throw ProbeException("Error: invalid data type specified on user. Found: " + OvalEnum::DatatypeToString(user->GetDatatype()));
	}	

	// check operation - only allow  equals, not equals and pattern match
	if(user->GetOperation() != OvalEnum::OPERATION_EQUALS && user->GetOperation() != OvalEnum::OPERATION_PATTERN_MATCH && user->GetOperation() != OvalEnum::OPERATION_NOT_EQUAL) {
		throw ProbeException("Error: invalid operation specified on user. Found: " + OvalEnum::OperationToString(user->GetOperation()));
	}

	// behaviors are not allowed on users

	if(object->GetBehaviors()->size() != 0) {
		throw ProbeException("Error user_objects do not support behaviors.");		
	}

	ItemVector *collectedItems = new ItemVector();

	// get the user data
	if(user->GetVarRef() == NULL) {
		if(user->GetOperation() == OvalEnum::OPERATION_EQUALS) {
			// simply get user if it exists
			Item* item=  this->GetUserInfo(user->GetValue());
			if(item != NULL) {
				collectedItems->push_back(item);
			} 
		} else {

			bool isRegex = false;
			if(user->GetOperation() == OvalEnum::OPERATION_PATTERN_MATCH)
				isRegex = true;

			// Get all users on the system...
			StringVector* allUsers = this->GetAllUsers();
			
			// Get the set of users that match the ItemEntity.
			StringVector::iterator iterator;
			for(iterator = allUsers->begin(); iterator != allUsers->end(); iterator++) {
				string curr = (*iterator);
				if(this->IsMatch(user->GetValue(), (*iterator), isRegex)) {
					Item* item =  this->GetUserInfo((*iterator));
					if(item != NULL) {
						collectedItems->push_back(item);
					}
				}
			}
		}

	} else {
		// Get all users on the system...
		StringVector* allUsers = this->GetAllUsers();

		// loop through all users on the system
		// only keep those that match operation and value and var check
		StringVector::iterator it;
		ItemEntity* tmp = this->CreateItemEntity(user);
		for(it = allUsers->begin(); it != allUsers->end(); it++) {
			tmp->SetValue((*it));
			if(user->Analyze(tmp) == OvalEnum::RESULT_TRUE) {
				Item* item = this->GetUserInfo((*it));
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
	StringVector* groups = new StringVector();
	bool userExists = this->GetGroupsForUser(userName, groups);
	if(userExists) {
		item = this->CreateItem();
		item->SetStatus(OvalEnum::STATUS_EXISTS);
		item->AppendElement(new ItemEntity("user", userName, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));

		// get the enabled flag
		try {
			bool enabled = this->GetEnabledFlag(userName);
			item->AppendElement(new ItemEntity("enabled", Common::ToString(enabled), OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));
		} catch (Exception ex) {
			item->AppendElement(new ItemEntity("enabled", "", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_ERROR));
			item->AppendMessage(new OvalMessage(ex.GetErrorMessage(), OvalEnum::LEVEL_ERROR));
		}

		StringVector::iterator iterator;
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

bool UserProbe::GetGroupsForUser(string userName, StringVector* groups) {
	// -----------------------------------------------------------------------
	//
	//  ABSTRACT
	//
	//  Return a the set of groups this user is a member of.
	//
	// -----------------------------------------------------------------------

	bool userExists = false;
	UniqueStringVector* usv = new UniqueStringVector(groups);

	LPCWSTR userNameApi;
	// convert groupName for api use
	wchar_t* wUserName = NULL;
	size_t size = mbstowcs(NULL, userName.c_str(), userName.length()) + 1;
	wUserName = new wchar_t[size];
	mbstowcs(wUserName, userName.c_str(), userName.size() + 1 );
	userNameApi = wUserName;

	// get the global groups
	LPGROUP_USERS_INFO_0 pBuf = NULL;
	DWORD dwLevel = 0;
	DWORD dwPrefMaxLen = MAX_PREFERRED_LENGTH;
	DWORD dwEntriesRead = 0;
	DWORD dwTotalEntries = 0;
	NET_API_STATUS nStatus;
	
	// Call the NetUserGetGroups function, specifying level 0.
	nStatus = NetUserGetGroups(NULL,
								userNameApi,
								dwLevel,
								(LPBYTE*)&pBuf,
								dwPrefMaxLen,
								&dwEntriesRead,
								&dwTotalEntries);
	// If the call succeeds,
	if (nStatus == NERR_Success) {
		userExists = true;

		LPGROUP_USERS_INFO_0 pTmpBuf;
		DWORD i;
		DWORD dwTotalCount = 0;
		char tmpGroupName[512];

		if ((pTmpBuf = pBuf) != NULL) {

			// Loop through the entries; 
			//  print the name of the global groups 
			//  to which the user belongs.
			for (i = 0; i < dwEntriesRead; i++) {
				
				if (pTmpBuf == NULL) {
					// Free the allocated buffer.
					if (pBuf != NULL)
						NetApiBufferFree(pBuf);
					throw ProbeException("An access violation has occurred while getting groups for user: " + userName);
				}

				ZeroMemory(tmpGroupName, 21);
				_snprintf(tmpGroupName, sizeof(tmpGroupName) - 1, "%S", pTmpBuf->grui0_name);
				tmpGroupName[sizeof(tmpGroupName)-1] = '\0';
				usv->Append(tmpGroupName);

				pTmpBuf++;
				dwTotalCount++;
			}
		}

		// report an error if all groups are not listed.
		if (dwEntriesRead < dwTotalEntries) {
			// Free the allocated buffer.
			if (pBuf != NULL)
				NetApiBufferFree(pBuf);
			throw ProbeException("Unable to get all global groups for user: " + userName);
		}
	
	} else if(nStatus == NERR_UserNotFound) {
		userExists = false;
	} else {
		throw ProbeException("A system error has occurred unable to get all global groups for user: " + userName);
	}

	// Free the allocated buffer.
	if (pBuf != NULL)
		NetApiBufferFree(pBuf);


	// only proceed if the user exists
	if(userExists) {

		//
		// get the local groups for the user
		//
		LPLOCALGROUP_USERS_INFO_0 pLocalBuf = NULL;
		dwLevel = 0;
		DWORD dwFlags = LG_INCLUDE_INDIRECT ;
		dwPrefMaxLen = MAX_PREFERRED_LENGTH;
		dwEntriesRead = 0;
		dwTotalEntries = 0;

		//  Call the NetUserGetLocalGroups function 
		//  specifying information level 0.
		//
		//   The LG_INCLUDE_INDIRECT flag specifies that the 
		//   function should also return the names of the local 
		//   groups in which the user is indirectly a member.
		nStatus = NetUserGetLocalGroups(NULL,
										userNameApi,
										dwLevel,
										dwFlags,
										(LPBYTE *) &pLocalBuf,
										dwPrefMaxLen,
										&dwEntriesRead,
										&dwTotalEntries);
		// If the call succeeds
		if (nStatus == NERR_Success) {
			LPLOCALGROUP_USERS_INFO_0 pLocalTmpBuf;
			DWORD i;
			DWORD dwTotalCount = 0;
			char tmpGroupName[512];

			if ((pLocalTmpBuf = pLocalBuf) != NULL) {

				//  Loop through the entries and 
				//  print the names of the local groups 
				//  to which the user belongs. 
				for (i = 0; i < dwEntriesRead; i++) {

					if (pLocalTmpBuf == NULL) {
						// Free the allocated memory.
						if (pLocalBuf != NULL)
							NetApiBufferFree(pLocalBuf);
						throw ProbeException("An access violation has occurred while getting local groups for user: " + userName);
					}

					ZeroMemory(tmpGroupName, 21);
					_snprintf(tmpGroupName, sizeof(tmpGroupName) - 1, "%S", pLocalTmpBuf->lgrui0_name);
					tmpGroupName[sizeof(tmpGroupName)-1] = '\0';
					usv->Append(tmpGroupName);

					pLocalTmpBuf++;
					dwTotalCount++;
				}
			}

			// report an error if all groups are not listed
			if (dwEntriesRead < dwTotalEntries) {
				// Free the allocated memory.
				if (pLocalBuf != NULL)
					NetApiBufferFree(pLocalBuf);
				throw ProbeException("Unable to get all local groups for user: " + userName);
			}

		} else if (nStatus == NERR_UserNotFound){
			// do nothing
		} else {
			throw ProbeException("A system error has occurred unable to get all local groups for user: " + userName);
		}

		// Free the allocated memory.
		if (pLocalBuf != NULL)
			NetApiBufferFree(pLocalBuf);
	}

	delete usv;

	return userExists;
}

bool UserProbe::GetEnabledFlag(string userName) {
	// -----------------------------------------------------------------------
	//
	//  ABSTRACT
	//
	//  Return the enabled flag for this user.
	//
	// -----------------------------------------------------------------------

	bool enabled = true;

	LPCWSTR userNameApi;
	// convert groupName for api use
	wchar_t* wUserName = NULL;
	size_t size = mbstowcs(NULL, userName.c_str(), userName.length()) + 1;
	wUserName = new wchar_t[size];
	mbstowcs(wUserName, userName.c_str(), userName.size() + 1 );
	userNameApi = wUserName;

	DWORD dwLevel = 23; // need USER_INFO_23  to get enabled flag
	LPUSER_INFO_23  pBuf = NULL;
	NET_API_STATUS nStatus;

	// Call the NetUserGetInfo function
	nStatus = NetUserGetInfo(NULL,
							userNameApi,
							dwLevel,
							(LPBYTE *)&pBuf);

	// If the call succeeds, print the user information.
	if (nStatus == NERR_Success) {
		if (pBuf != NULL) {			
			// now read the flags
			if(pBuf->usri23_flags & UF_ACCOUNTDISABLE) {
				enabled = false;
			}
		}
	} else {
		throw ProbeException("A system error has occurred while getting user enabmed flag.");
	}
	
	// Free the allocated memory.
	if (pBuf != NULL)
		NetApiBufferFree(pBuf);

    return enabled;
}

StringVector* UserProbe::GetAllUsers() {
	// -----------------------------------------------------------------------
	//
	//  ABSTRACT
	//
	//  Return a the set of users on the local host
	//
	//	Do we need to get users other than local users?
	// -----------------------------------------------------------------------

	StringVector* users = new StringVector();
	UniqueStringVector* allUsers = new UniqueStringVector(users);

	// just call windows common method to get local users.
	WindowsCommon::GetAllLocalUsers(allUsers);


	return allUsers->GetUniqueStrings();
}
