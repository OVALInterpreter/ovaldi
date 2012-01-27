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

#include "PasswordProbe.h"
#include "ItemEntity.h"
#include "ObjectEntity.h"
#include "OvalEnum.h"
#include <errno.h>

using namespace std;

PasswordProbe* PasswordProbe::instance = NULL;

PasswordProbe::PasswordProbe() {
}

PasswordProbe::~PasswordProbe() {
	PasswordProbe::instance = NULL;
}

Item* PasswordProbe::CreateItem() {

	Item* item = new Item(0,
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#unix",
						"unix-sc",
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#unix unix-system-characteristics-schema.xsd",
						OvalEnum::STATUS_ERROR,
						"password_item");

	return item;
}

ItemVector* PasswordProbe::CollectItems(Object* object) {
	ObjectEntity* usernameEntity = object->GetElementByName ( "username" );
	OvalEnum::Operation op = usernameEntity->GetOperation();
	ItemVector *items = NULL;
	StringVector usernameEntityValues;

	switch(op) {
	case OvalEnum::OPERATION_EQUALS:

		// match literal names
		/*OvalEnum::Flag flag =*/ usernameEntity->GetEntityValues(usernameEntityValues);
		
		// According to Jon, we don't currently check the flag.  So ignore it!

		items = new ItemVector();
		for (StringVector::iterator iter = usernameEntityValues.begin();
			 iter != usernameEntityValues.end();
			 ++iter) {
			Item *item = this->GetSingleItem(*iter);
			items->push_back(item);
		}
		break;

	case OvalEnum::OPERATION_NOT_EQUAL:
	case OvalEnum::OPERATION_PATTERN_MATCH:
	case OvalEnum::OPERATION_CASE_INSENSITIVE_EQUALS:
	case OvalEnum::OPERATION_CASE_INSENSITIVE_NOT_EQUAL:
		// in these cases, we must loop thru all names to find matches.
		items = this->GetMultipleItems(object);
		break;

	default:
		throw ProbeException(string("Invalid operation: ") + OvalEnum::OperationToString(op));
	}

	return items;
}

Item *PasswordProbe::CreateItemFromPasswd(const struct passwd *pwInfo) {
	Item *item = this->CreateItem();
	item->SetStatus(OvalEnum::STATUS_EXISTS);
	
	ItemEntity *nameEntity = new ItemEntity("username", pwInfo->pw_name,
											OvalEnum::DATATYPE_STRING, true);
	ItemEntity *passwordEntity = new ItemEntity("password", pwInfo->pw_passwd);
	ItemEntity *useridEntity = new ItemEntity("user_id", Common::ToString(pwInfo->pw_uid), OvalEnum::DATATYPE_INTEGER);
	ItemEntity *groupidEntity = new ItemEntity("group_id", Common::ToString(pwInfo->pw_gid), OvalEnum::DATATYPE_INTEGER);
	ItemEntity *gcosEntity = new ItemEntity("gcos", pwInfo->pw_gecos);
	ItemEntity *homeDirEntity = new ItemEntity("home_dir", pwInfo->pw_dir);
	ItemEntity *loginShellEntity = new ItemEntity("login_shell", pwInfo->pw_shell);

	item->AppendElement(nameEntity);
	item->AppendElement(passwordEntity);
	item->AppendElement(useridEntity);
	item->AppendElement(groupidEntity);
	item->AppendElement(gcosEntity);
	item->AppendElement(homeDirEntity);
	item->AppendElement(loginShellEntity);

	return item;
}

Item *PasswordProbe::GetSingleItem(const string& username) {
	errno = 0;
	struct passwd *pwInfo = getpwnam(username.c_str());
	Item *item;

	if (pwInfo == NULL) {
		item = this->CreateItem();
		ItemEntity *nameEntity = new ItemEntity("username", username, OvalEnum::DATATYPE_STRING, true);
		item->AppendElement(nameEntity);

		if (errno == 0) {
			// for a simple not-found condition, return the dummy item with
			// appropriate status.
			item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
		} else {
			// if some other error occurred, set error status and append a message
			item->SetStatus(OvalEnum::STATUS_ERROR);
			item->AppendMessage(new OvalMessage(string("Error getting user info: ")+strerror(errno),
												OvalEnum::LEVEL_ERROR));
		}
	} else
		item = this->CreateItemFromPasswd(pwInfo);

	return item;
}

ItemVector *PasswordProbe::GetMultipleItems(Object *passwordObject) {
	struct passwd *pwInfo;
	ItemVector *items = new ItemVector();

	errno = 0;
	while ((pwInfo = getpwent()) != NULL) {
		Item *item = this->CreateItemFromPasswd(pwInfo);

		if (passwordObject->Analyze(item))
			items->push_back(item);
		else
			delete item;
	}

	endpwent();

	if (errno != 0) {
		// error occurred... delete all items we found and throw
		// an exception
		while (!items->empty()) {
			Item *item = items->back();
			items->pop_back();
			delete item;
		}
		
		delete items;
		throw ProbeException(string("Error getting user info: ")+strerror(errno),
							 ERROR_WARN);
	}

	return items;
}

AbsProbe* PasswordProbe::Instance() {
	if (PasswordProbe::instance == NULL)
		PasswordProbe::instance = new PasswordProbe();

	return PasswordProbe::instance;
}
