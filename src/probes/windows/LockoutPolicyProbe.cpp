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
#include "LockoutPolicyProbe.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  LockoutPolicyProbe Class  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
LockoutPolicyProbe* LockoutPolicyProbe::instance = NULL;

LockoutPolicyProbe::LockoutPolicyProbe() {

}

LockoutPolicyProbe::~LockoutPolicyProbe() {
  instance = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* LockoutPolicyProbe::Instance() {

	// Use lazy initialization
	if(instance == NULL) 
		instance = new LockoutPolicyProbe();

	return instance;	
}

ItemVector* LockoutPolicyProbe::CollectItems(Object *object) {

	ItemVector *collectedItems = NULL;

	Item* item = NULL;

	//
	// Get the force_logoff value
	//
	DWORD dwLevel = 0;
	USER_MODALS_INFO_0 *pBufM0 = NULL;
	NET_API_STATUS nStatus;
	LPCWSTR pszServerName = NULL;

	nStatus = NetUserModalsGet(pszServerName,
                              dwLevel,
                              (LPBYTE *)&pBufM0);

	if (nStatus == NERR_Success) {
		if (pBufM0 != NULL) {
			string forceLogoff = WindowsCommon::ToString(pBufM0->usrmod0_force_logoff);

			// create a new passwordpolicy item
			item = this->CreateItem();
			item->SetStatus(OvalEnum::STATUS_EXISTS);
			collectedItems = new ItemVector();
			collectedItems->push_back(item);

			item->AppendElement(new ItemEntity("force_logoff",  forceLogoff, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));

			// Free the allocated memory.
			NetApiBufferFree(pBufM0);
		}

	} else if(nStatus == ERROR_ACCESS_DENIED) {
		// Free the allocated memory.
		if (pBufM0 != NULL)
			NetApiBufferFree(pBufM0);
		throw ProbeException ("Error: The user does not have access to the requested lockout policy information.");
	} else if(nStatus == NERR_InvalidComputer) {
		// Free the allocated memory.
		if (pBufM0 != NULL)
			NetApiBufferFree(pBufM0);
		throw ProbeException ("Error: The computer name is invalid for requesting lockout policy information.");
	}	

	//
	// Get the remaining lockout policy data
	//
	dwLevel = 3;
	USER_MODALS_INFO_3 *pBufM3 = NULL;

	nStatus = NetUserModalsGet(pszServerName,
                              dwLevel,
                              (LPBYTE *)&pBufM3);

	if (nStatus == NERR_Success) {
		if (pBufM3 != NULL) {

			string lockoutDuration = WindowsCommon::ToString(pBufM3->usrmod3_lockout_duration);
			string lockoutObservation = WindowsCommon::ToString(pBufM3->usrmod3_lockout_observation_window);
			string lockoutThreshold = WindowsCommon::ToString(pBufM3->usrmod3_lockout_threshold);


			// create a new passwordpolicy item
			if(item == NULL) 
				item = this->CreateItem();

			item->SetStatus(OvalEnum::STATUS_EXISTS);
			collectedItems = new ItemVector();
			collectedItems->push_back(item);

			item->AppendElement(new ItemEntity("lockout_duration",  lockoutDuration, OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));
			item->AppendElement(new ItemEntity("lockout_observation_window",  lockoutObservation, OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));
			item->AppendElement(new ItemEntity("lockout_threshold",  lockoutThreshold, OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));

			// Free the allocated memory.
			NetApiBufferFree(pBufM3);
		}

	} else if(nStatus == ERROR_ACCESS_DENIED) {
		// Free the allocated memory.
		if (pBufM3 != NULL)
			NetApiBufferFree(pBufM3);
		throw ProbeException ("Error: The user does not have access to the requested lockout policy information.");
	} else if(nStatus == NERR_InvalidComputer) {
		// Free the allocated memory.
		if (pBufM3 != NULL)
			NetApiBufferFree(pBufM3);
		throw ProbeException ("Error: The computer name is invalid for requesting lockout policy information.");
	}

	return collectedItems;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
Item* LockoutPolicyProbe::CreateItem() {

	Item* item = new Item(0, 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows", 
						"win-sc", 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows windows-system-characteristics-schema.xsd", 
						OvalEnum::STATUS_ERROR, 
						"lockoutpolicy_item");

	return item;
}
