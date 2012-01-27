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

#include "ShadowProbe.h"
#include "ItemEntity.h"
#include "Log.h"
#include "ObjectEntity.h"
#include "OvalEnum.h"
#include <errno.h>

using namespace std;

namespace {
	/**
	 * Used to manage locking/unlocking of password/shadow files in a
	 * safe way.  Simply stack-allocate an object of this class, and
	 * the files will be locked.  Unlocking will occur automatically
	 * when the object goes out of scope.
	 *
	 * The outer anonymous namespace makes this class private to this
	 * compilation unit.
	 */
	class ShadowFileGuard {
		public:
		ShadowFileGuard();
		~ShadowFileGuard();
	};

	ShadowFileGuard::ShadowFileGuard() {
		// It was never clear to me which of the shadow API functions
		// set errno values.  The man pages on my dev system don't say any
		// of them do.  Several web pages I looked at indicated the *_r
		// variants of the functions set errno but didn't say anything 
		// about the non _r variants.  So this is coded defensively,
		// adding extra description text if errno was set, and omitting it
		// otherwise.
		errno = 0;
		if (lckpwdf() == -1) {
			string msg = "Error locking password files";
			if (errno != 0)
				msg += string(": ") + strerror(errno);
			
			throw ProbeException(msg);
		}
	}

	ShadowFileGuard::~ShadowFileGuard() {
		// This does not throw an exception if unlocking failed.
		// You never know if the guard went out of scope due to
		// another exception being thrown, which you don't want
		// to mask.  So it just logs a message instead.
		errno = 0;
		if (ulckpwdf() == -1) {
			string msg = "Error unlocking password files";
			if (errno != 0)
				msg += string(": ") + strerror(errno);
			
			Log::Info(msg);
		}
	}
}


ShadowProbe* ShadowProbe::instance = NULL;
map<string, string> ShadowProbe::dollarEncryptMethodTypes;

ShadowProbe::ShadowProbe() {
}

ShadowProbe::~ShadowProbe() {
	ShadowProbe::instance = NULL;
}

Item* ShadowProbe::CreateItem() {

	Item* item = new Item(0,
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#unix",
						"unix-sc",
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#unix unix-system-characteristics-schema.xsd",
						OvalEnum::STATUS_ERROR,
						"shadow_item");

	return item;
}

ItemVector* ShadowProbe::CollectItems(Object* object) {
	ShadowFileGuard guard;

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
		throw ProbeException(string("Invalid operator: ") + OvalEnum::OperationToString(op));
	}

	return items;
}

Item *ShadowProbe::CreateItemFromPasswd(const struct spwd *pwInfo) {
	Item *item = this->CreateItem();
	item->SetStatus(OvalEnum::STATUS_EXISTS);
	
	ItemEntity *nameEntity = new ItemEntity("username", pwInfo->sp_namp,
											OvalEnum::DATATYPE_STRING, true);
	ItemEntity *passwordEntity = new ItemEntity("password", pwInfo->sp_pwdp);
	ItemEntity *lastChangedEntity = new ItemEntity("chg_lst", Common::ToString(pwInfo->sp_lstchg), OvalEnum::DATATYPE_INTEGER);
	ItemEntity *changeAllowedEntity = new ItemEntity("chg_allow", Common::ToString(pwInfo->sp_min), OvalEnum::DATATYPE_INTEGER);
	ItemEntity *changeRequiredEntity = new ItemEntity("chg_req", Common::ToString(pwInfo->sp_max), OvalEnum::DATATYPE_INTEGER);
	ItemEntity *expirationWarningEntity = new ItemEntity("exp_warn", Common::ToString(pwInfo->sp_warn), OvalEnum::DATATYPE_INTEGER);
	ItemEntity *accountInactiveEntity = new ItemEntity("exp_inact", Common::ToString(pwInfo->sp_inact), OvalEnum::DATATYPE_INTEGER);
	ItemEntity *expirationDateEntity = new ItemEntity("exp_date", Common::ToString(pwInfo->sp_expire), OvalEnum::DATATYPE_INTEGER);
	ItemEntity *flagsEntity = new ItemEntity("flag", Common::ToString(pwInfo->sp_flag));

	item->AppendElement(nameEntity);
	item->AppendElement(passwordEntity);
	item->AppendElement(lastChangedEntity);
	item->AppendElement(changeAllowedEntity);
	item->AppendElement(changeRequiredEntity);
	item->AppendElement(expirationWarningEntity);
	item->AppendElement(accountInactiveEntity);
	item->AppendElement(expirationDateEntity);
	item->AppendElement(flagsEntity);

	string method = this->FindPasswordEncryptMethod(pwInfo->sp_pwdp);
	ItemEntity *encryptMethodEntity;
	if (method.empty()) {
		item->AppendMessage(new OvalMessage("Couldn't determine password encryption method"));
		encryptMethodEntity = new ItemEntity("encrypt_method", "", OvalEnum::DATATYPE_STRING,
											 false, OvalEnum::STATUS_ERROR);
	} else
		encryptMethodEntity = new ItemEntity("encrypt_method", method);

	item->AppendElement(encryptMethodEntity);

	return item;
}

string ShadowProbe::FindPasswordEncryptMethod(const string &password) {
	REGEX re;
	StringVector matches;

	// BSDi identified by leading _
	if (!password.empty() && password[0]=='_')
		return "BSDi";

	// check for the $...$ prefix
	if (re.GetMatchingSubstrings("^\\$([^$]+)\\$", password.c_str(), &matches)) {
		map<string,string>::iterator iter = 
			ShadowProbe::dollarEncryptMethodTypes.find(matches[0]);

		if (iter != ShadowProbe::dollarEncryptMethodTypes.end())
			return iter->second;

		return "";
	}

	// default to DES
	return "DES";
}

Item *ShadowProbe::GetSingleItem(const string& username) {
	errno = 0;
	
	struct spwd *pwInfo = getspnam(username.c_str());
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
			item->AppendMessage(new OvalMessage(string("Error getting shadow info: ")+strerror(errno),
												OvalEnum::LEVEL_ERROR));
		}
	} else
		item = this->CreateItemFromPasswd(pwInfo);

	return item;
}

ItemVector *ShadowProbe::GetMultipleItems(Object *shadowObject) {
	struct spwd *pwInfo;
	ItemVector *items = new ItemVector();

	errno = 0;
	while ((pwInfo = getspent()) != NULL) {
		Item *item = this->CreateItemFromPasswd(pwInfo);

		if (shadowObject->Analyze(item))
			items->push_back(item);
		else
			delete item;
	}

	endspent();

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

AbsProbe* ShadowProbe::Instance() {
	if (ShadowProbe::instance == NULL)
		ShadowProbe::instance = new ShadowProbe();

	if (ShadowProbe::dollarEncryptMethodTypes.empty())
		PopulateDollarEncryptMethodTypes();

	return ShadowProbe::instance;
}

void ShadowProbe::PopulateDollarEncryptMethodTypes() {
	dollarEncryptMethodTypes["1"] = "MD5";
	dollarEncryptMethodTypes["2"] = "Blowfish";
	dollarEncryptMethodTypes["2a"] = "Blowfish";
	dollarEncryptMethodTypes["md5"] = "Sun MD5";
	dollarEncryptMethodTypes["5"] = "SHA-256";
	dollarEncryptMethodTypes["6"] = "SHA-512";
}
