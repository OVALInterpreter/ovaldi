//
//
//****************************************************************************************//
// Copyright (c) 2002-2008, The MITRE Corporation
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
#include "PasswordPolicyProbe.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  PasswordPolicyProbe Class  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
PasswordPolicyProbe* PasswordPolicyProbe::instance = NULL;

PasswordPolicyProbe::PasswordPolicyProbe() {
}

PasswordPolicyProbe::~PasswordPolicyProbe() {

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* PasswordPolicyProbe::Instance() {

	// Use lazy initialization
	if(instance == NULL) 
		instance = new PasswordPolicyProbe();

	return instance;	
}

ItemVector* PasswordPolicyProbe::CollectItems(Object *object) {

	ItemVector *collectedItems = NULL;
	
	DWORD dwLevel = 0;
	USER_MODALS_INFO_0 *pBuf = NULL;
	NET_API_STATUS nStatus;
	LPCWSTR pszServerName = NULL;

	nStatus = NetUserModalsGet(pszServerName,
                              dwLevel,
                              (LPBYTE *)&pBuf);

	if (nStatus == NERR_Success) {
		if (pBuf != NULL) {
			string minPasswordLen = WindowsCommon::ToString(pBuf->usrmod0_min_passwd_len);

			string maxPasswordAge = "-1";
			if(pBuf->usrmod0_max_passwd_age == TIMEQ_FOREVER) {
				maxPasswordAge = "-1";
			} else {
				maxPasswordAge = WindowsCommon::ToString(pBuf->usrmod0_max_passwd_age);
			}

			string minPasswordAge = WindowsCommon::ToString(pBuf->usrmod0_min_passwd_age);

			string passwordHistoryLen = WindowsCommon::ToString(pBuf->usrmod0_password_hist_len);

			// create a new passwordpolicy item
			Item* item = this->CreateItem();
			item->SetStatus(OvalEnum::STATUS_EXISTS);
			collectedItems = new ItemVector();
			collectedItems->push_back(item);

			item->AppendElement(new ItemEntity("max_passwd_age",  maxPasswordAge, OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));
			item->AppendElement(new ItemEntity("min_passwd_age",  minPasswordAge, OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));
			item->AppendElement(new ItemEntity("min_passwd_len",  minPasswordLen, OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));
			item->AppendElement(new ItemEntity("password_hist_len",  passwordHistoryLen, OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));
			item->AppendElement(new ItemEntity("password_complexity",  "", OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_NOT_COLLECTED));
			item->AppendElement(new ItemEntity("reversible_encryption",  "", OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_NOT_COLLECTED));
			item->AppendMessage(new OvalMessage("At this time it is not clear how to obtain password_complexity or reversible_encryption."));

			// Free the allocated memory.
			NetApiBufferFree(pBuf);
		}

	} else if(nStatus == ERROR_ACCESS_DENIED) {
		// Free the allocated memory.
		if (pBuf != NULL)
			NetApiBufferFree(pBuf);
		throw ProbeException ("Error: The user does not have access to the requested password policy information.");
	} else if(nStatus == NERR_InvalidComputer) {
		// Free the allocated memory.
		if (pBuf != NULL)
			NetApiBufferFree(pBuf);
		throw ProbeException ("Error: The computer name is invalid for requesting password policy information.");
	}	

	return collectedItems;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
Item* PasswordPolicyProbe::CreateItem() {

	Item* item = new Item(0, 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows", 
						"win-sc", 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows windows-system-characteristics-schema.xsd", 
						OvalEnum::STATUS_ERROR, 
						"passwordpolicy_item");

	return item;
}
