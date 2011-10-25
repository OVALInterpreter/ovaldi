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

#include "AccessTokenProbe.h"

//****************************************************************************************//
//								AccessTokenProbe Class									  //	
//****************************************************************************************//
AccessTokenProbe* AccessTokenProbe::instance = NULL;

AccessTokenProbe::AccessTokenProbe() : AbsProbe() {

}

AccessTokenProbe::~AccessTokenProbe() {
  instance = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* AccessTokenProbe::Instance() {
	// Use lazy initialization
	if(instance == NULL) 
		instance = new AccessTokenProbe();

	return instance;	
}

ItemVector* AccessTokenProbe::CollectItems(Object *object) {

	// get the security_principle from the provided object
	ObjectEntity* securityPrinciple = object->GetElementByName("security_principle");

	// check datatypes - only allow string
	if(securityPrinciple->GetDatatype() != OvalEnum::DATATYPE_STRING) {
		throw ProbeException("Error: invalid data type specified on security_principle. Found: " + OvalEnum::DatatypeToString(securityPrinciple->GetDatatype()));
	}
	
	// check operation - only allow  equals, not equals and pattern match
	if(securityPrinciple->GetOperation() != OvalEnum::OPERATION_EQUALS && securityPrinciple->GetOperation() != OvalEnum::OPERATION_PATTERN_MATCH && securityPrinciple->GetOperation() != OvalEnum::OPERATION_NOT_EQUAL) {
		throw ProbeException("Error: invalid operation specified on security_principle. Found: securityPrinciple " + OvalEnum::OperationToString(securityPrinciple->GetOperation()));
	}

	ItemVector *collectedItems = new ItemVector();
	
	// support behaviors - init with defaults.
	bool includeGroupBehavior = true;
	bool resolveGroupBehavior = false;
	if(object->GetBehaviors()->size() != 0) {
		BehaviorVector* behaviors = object->GetBehaviors();
		BehaviorVector::iterator iterator;
		for(iterator = behaviors->begin(); iterator != behaviors->end(); iterator++) {
			Behavior* behavior = (*iterator);
			if(behavior->GetName().compare("include_group") == 0) {
				if(behavior->GetValue().compare("false") == 0) {
					includeGroupBehavior = false;
				}
				Log::Info("Deprecated behavior found when collecting " + object->GetId() + " Found behavior: " + behavior->GetName() + " = " + behavior->GetValue());
			} else if(behavior->GetName().compare("resolve_group") == 0) {
				if(behavior->GetValue().compare("true") == 0) {
					resolveGroupBehavior = true;
				}
                Log::Info("Deprecated behavior found when collecting " + object->GetId() + " Found behavior: " + behavior->GetName() + " = " + behavior->GetValue());
			} else {
				Log::Info("Unsupported behavior found when collecting " + object->GetId() + " Found behavior: " + behavior->GetName() + " = " + behavior->GetValue());
			}
		}		
	}

	// get the data
	if(securityPrinciple->GetVarRef() == NULL) {
		if(securityPrinciple->GetOperation() == OvalEnum::OPERATION_EQUALS) {
			this->GetAccountInformation(securityPrinciple->GetValue(), resolveGroupBehavior, includeGroupBehavior, collectedItems);
		} else {

			bool isRegex = false;
			if(securityPrinciple->GetOperation() == OvalEnum::OPERATION_PATTERN_MATCH)
				isRegex = true;

			// Get all trustee_names on the system...
			StringSet* allTrusteeNames = WindowsCommon::GetAllTrusteeNames();
			
			// Get the set of trustee names that match the ItemEntity.
			StringSet::iterator iterator;
			for(iterator = allTrusteeNames->begin(); iterator != allTrusteeNames->end(); iterator++) {
				string curr = (*iterator);
				if(this->IsMatch(securityPrinciple->GetValue(), (*iterator), isRegex)) {
					this->GetAccountInformation((*iterator), resolveGroupBehavior, includeGroupBehavior, collectedItems);
				}
			}
		}

	} else {
		// Get all trustee_names on the system...
		StringSet* allTrusteeNames = WindowsCommon::GetAllTrusteeNames();

		// loop through all trustee names on the system
		// only keep those that match operation and value and var check
		StringSet::iterator it;
		ItemEntity* tmp = this->CreateItemEntity(securityPrinciple);
		for(it = allTrusteeNames->begin(); it != allTrusteeNames->end(); it++) {
			tmp->SetValue((*it));
			if(securityPrinciple->Analyze(tmp) == OvalEnum::RESULT_TRUE) {
				this->GetAccountInformation((*it), resolveGroupBehavior, includeGroupBehavior, collectedItems);
			}
		}
	}

	return collectedItems;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
Item* AccessTokenProbe::CreateItem() {

	Item* item = new Item(0, 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows", 
						"win-sc", 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows windows-system-characteristics-schema.xsd", 
						OvalEnum::STATUS_ERROR, 
						"accesstoken_item");

	return item;
}


bool AccessTokenProbe::GetAccountInformation(string accountNameIn,  bool resolveGroupBehavior, bool includeGroupBehavior, ItemVector* items) {

	bool isComplete = true;

	// for the account name and behaviors create a set of account names to loop over and get account information for
	StringVector* accountNames = new StringVector();

	// is this a group
	string domainStr = "";
	string sidStr = "";
	bool isGroup = WindowsCommon::LookUpTrusteeName(&accountNameIn, &sidStr, &domainStr);

	if(isGroup && resolveGroupBehavior) {

		if(includeGroupBehavior) {
			accountNames->push_back(accountNameIn);
		}

		StringSet* groupMembers = new StringSet();
		WindowsCommon::ExpandGroup(accountNameIn, groupMembers, includeGroupBehavior, resolveGroupBehavior);
		StringSet::iterator iterator;
		for(iterator = groupMembers->begin(); iterator != groupMembers->end(); iterator++) {
			accountNames->push_back((*iterator));
		}
	} else {
		accountNames->push_back(accountNameIn);
	}

	NTSTATUS nts;

	// Get a handle to the policy object.
	LSA_HANDLE polHandle;
	LSA_OBJECT_ATTRIBUTES ObjectAttributes;
	ZeroMemory(&ObjectAttributes, sizeof(ObjectAttributes));

	nts = LsaOpenPolicy(NULL, &ObjectAttributes, POLICY_LOOKUP_NAMES, &polHandle);
	if (nts != ERROR_SUCCESS) {
		LsaClose(polHandle);
		throw ProbeException("Error: (AccessTokenProbe) Unable to open a handle to the Policy object.");		
	}

	if(accountNames->size() > 0) {
		StringVector::iterator it;
		for(it = accountNames->begin(); it != accountNames->end(); it++) {
			string currentAccountName = (*it);

			// Get the SID.
			PSID psid = WindowsCommon::GetSIDForTrusteeName(currentAccountName);

			// Enumerate Access Rights
			PLSA_UNICODE_STRING userRights = NULL;
			ULONG CountOfRights = 0;

			nts = LsaEnumerateAccountRights(polHandle, psid, &userRights, &CountOfRights);
			if (nts != ERROR_SUCCESS) {
				CountOfRights = 0;
			}

			// Create the item
			Item* item = this->CreateItem();
			item->SetStatus(OvalEnum::STATUS_EXISTS);
			item->AppendElement(new ItemEntity("security_principle", currentAccountName, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
			items->push_back(item);

			// init all the rights
			ItemEntity* seassignprimarytokenprivilege = new ItemEntity("seassignprimarytokenprivilege", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS);
			item->AppendElement(seassignprimarytokenprivilege);
			ItemEntity* seauditprivilege = new ItemEntity("seauditprivilege", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS);
			item->AppendElement(seauditprivilege);
			ItemEntity* sebackupprivilege = new ItemEntity("sebackupprivilege", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS);
			item->AppendElement(sebackupprivilege);
			ItemEntity* sechangenotifyprivilege = new ItemEntity("sechangenotifyprivilege", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS);
			item->AppendElement(sechangenotifyprivilege);
			ItemEntity* secreateglobalprivilege = new ItemEntity("secreateglobalprivilege", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS);
			item->AppendElement(secreateglobalprivilege);
			ItemEntity* secreatepagefileprivilege = new ItemEntity("secreatepagefileprivilege", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS);
			item->AppendElement(secreatepagefileprivilege);
			ItemEntity* secreatepermanentprivilege = new ItemEntity("secreatepermanentprivilege", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS);
			item->AppendElement(secreatepermanentprivilege);
			ItemEntity* secreatesymboliclinkprivilege = new ItemEntity("secreatesymboliclinkprivilege", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS);
			item->AppendElement(secreatesymboliclinkprivilege);
			ItemEntity* secreatetokenprivilege = new ItemEntity("secreatetokenprivilege", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS);
			item->AppendElement(secreatetokenprivilege);
			ItemEntity* sedebugprivilege = new ItemEntity("sedebugprivilege", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS);
			item->AppendElement(sedebugprivilege);
			ItemEntity* seenabledelegationprivilege = new ItemEntity("seenabledelegationprivilege", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS);
			item->AppendElement(seenabledelegationprivilege);
			ItemEntity* seimpersonateprivilege = new ItemEntity("seimpersonateprivilege", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS);
			item->AppendElement(seimpersonateprivilege);
			ItemEntity* seincreasebasepriorityprivilege = new ItemEntity("seincreasebasepriorityprivilege", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS);
			item->AppendElement(seincreasebasepriorityprivilege);
			ItemEntity* seincreasequotaprivilege = new ItemEntity("seincreasequotaprivilege", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS);
			item->AppendElement(seincreasequotaprivilege);
			ItemEntity* seincreaseworkingsetprivilege = new ItemEntity("seincreaseworkingsetprivilege", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS);
			item->AppendElement(seincreaseworkingsetprivilege);
			ItemEntity* seloaddriverprivilege = new ItemEntity("seloaddriverprivilege", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS);
			item->AppendElement(seloaddriverprivilege);
			ItemEntity* selockmemoryprivilege = new ItemEntity("selockmemoryprivilege", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS);
			item->AppendElement(selockmemoryprivilege);
			ItemEntity* semachineaccountprivilege = new ItemEntity("semachineaccountprivilege", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS);
			item->AppendElement(semachineaccountprivilege);
			ItemEntity* semanagevolumeprivilege = new ItemEntity("semanagevolumeprivilege", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS);
			item->AppendElement(semanagevolumeprivilege);
			ItemEntity* seprofilesingleprocessprivilege = new ItemEntity("seprofilesingleprocessprivilege", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS);
			item->AppendElement(seprofilesingleprocessprivilege);
			ItemEntity* serelabelprivilege = new ItemEntity("serelabelprivilege", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS);
			item->AppendElement(serelabelprivilege);
			ItemEntity* seremoteshutdownprivilege = new ItemEntity("seremoteshutdownprivilege", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS);
			item->AppendElement(seremoteshutdownprivilege);
			ItemEntity* serestoreprivilege = new ItemEntity("serestoreprivilege", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS);
			item->AppendElement(serestoreprivilege);
			ItemEntity* sesecurityprivilege = new ItemEntity("sesecurityprivilege", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS);
			item->AppendElement(sesecurityprivilege);
			ItemEntity* seshutdownprivilege = new ItemEntity("seshutdownprivilege", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS);
			item->AppendElement(seshutdownprivilege);
			ItemEntity* sesyncagentprivilege = new ItemEntity("sesyncagentprivilege", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS);
			item->AppendElement(sesyncagentprivilege);
			ItemEntity* sesystemenvironmentprivilege = new ItemEntity("sesystemenvironmentprivilege", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS);
			item->AppendElement(sesystemenvironmentprivilege);
			ItemEntity* sesystemprofileprivilege = new ItemEntity("sesystemprofileprivilege", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS);
			item->AppendElement(sesystemprofileprivilege);
			ItemEntity* sesystemtimeprivilege = new ItemEntity("sesystemtimeprivilege", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS);
			item->AppendElement(sesystemtimeprivilege);
			ItemEntity* setakeownershipprivilege = new ItemEntity("setakeownershipprivilege", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS);
			item->AppendElement(setakeownershipprivilege);
			ItemEntity* setcbprivilege = new ItemEntity("setcbprivilege", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS);
			item->AppendElement(setcbprivilege);
			ItemEntity* setimezoneprivilege = new ItemEntity("setimezoneprivilege", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS);
			item->AppendElement(setimezoneprivilege);
			ItemEntity* seundockprivilege = new ItemEntity("seundockprivilege", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS);
			item->AppendElement(seundockprivilege);
			ItemEntity* seunsolicitedinputprivilege = new ItemEntity("seunsolicitedinputprivilege", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS);
			item->AppendElement(seunsolicitedinputprivilege);
			ItemEntity* sebatchlogonright = new ItemEntity("sebatchlogonright", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS);
			item->AppendElement(sebatchlogonright);
			ItemEntity* seinteractivelogonright = new ItemEntity("seinteractivelogonright", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS);
			item->AppendElement(seinteractivelogonright);
			ItemEntity* senetworklogonright = new ItemEntity("senetworklogonright", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS);
			item->AppendElement(senetworklogonright);
			ItemEntity* seremoteinteractivelogonright = new ItemEntity("seremoteinteractivelogonright", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS);
			item->AppendElement(seremoteinteractivelogonright);
			ItemEntity* seservicelogonright = new ItemEntity("seservicelogonright", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS);
			item->AppendElement(seservicelogonright);
			ItemEntity* sedenybatchLogonright = new ItemEntity("sedenybatchLogonright", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS);
			item->AppendElement(sedenybatchLogonright);
			ItemEntity* sedenyinteractivelogonright = new ItemEntity("sedenyinteractivelogonright", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS);
			item->AppendElement(sedenyinteractivelogonright);
			ItemEntity* sedenynetworklogonright = new ItemEntity("sedenynetworklogonright", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS);
			item->AppendElement(sedenynetworklogonright);
			ItemEntity* sedenyremoteInteractivelogonright = new ItemEntity("sedenyremoteInteractivelogonright", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS);
			item->AppendElement(sedenyremoteInteractivelogonright);
			ItemEntity* sedenyservicelogonright = new ItemEntity("sedenyservicelogonright", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS);
			item->AppendElement(sedenyservicelogonright);
			ItemEntity* setrustedcredmanaccessnameright = new ItemEntity("setrustedcredmanaccessnameright", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS);
			item->AppendElement(setrustedcredmanaccessnameright);

			// Alter the access mask to show the correct rights.
			char tmpPrivBuf[128];
			for (ULONG i=0; i<CountOfRights; i++) {
				ZeroMemory(tmpPrivBuf, 128);

				WideCharToMultiByte(CP_ACP,							// code page
									0,								// performance and mapping flags
									userRights[i].Buffer,			// wide-character string
									wcslen(userRights[i].Buffer),	// number of chars in string
									tmpPrivBuf,						// buffer for new string
									128,							// size of buffer
									NULL,							// default for unmappable chars
									NULL);							// set when default char used

				if (_strnicmp(tmpPrivBuf, "SeAssignPrimaryTokenPrivilege", 29) == 0)
					seassignprimarytokenprivilege->SetValue("1");
				else if (_strnicmp(tmpPrivBuf, "SeAuditPrivilege", 16) == 0)
					seauditprivilege->SetValue("1");
				else if (_strnicmp(tmpPrivBuf, "SeBackupPrivilege", 17) == 0)
					sebackupprivilege->SetValue("1");
				else if (_strnicmp(tmpPrivBuf, "SeChangeNotifyPrivilege", 23) == 0)
					sechangenotifyprivilege->SetValue("1");
				else if (_strnicmp(tmpPrivBuf, "SeCreateGlobalPrivilege", 23) == 0)
					secreateglobalprivilege->SetValue("1");
				else if (_strnicmp(tmpPrivBuf, "SeCreatePagefilePrivilege", 25) == 0)
					secreatepagefileprivilege->SetValue("1");
				else if (_strnicmp(tmpPrivBuf, "SeCreatePermanentPrivilege", 26) == 0)
					secreatepermanentprivilege->SetValue("1");
				else if (_strnicmp(tmpPrivBuf, "SeCreateTokenPrivilege", 22) == 0)
					secreatetokenprivilege->SetValue("1");
				else if (_strnicmp(tmpPrivBuf, "SeDebugPrivilege", 16) == 0)
					sedebugprivilege->SetValue("1");
				else if (_strnicmp(tmpPrivBuf, "SeEnableDelegationPrivilege", 27) == 0)
					seenabledelegationprivilege->SetValue("1");
				else if (_strnicmp(tmpPrivBuf, "SeImpersonatePrivilege", 22) == 0)
					seimpersonateprivilege->SetValue("1");
				else if (_strnicmp(tmpPrivBuf, "SeIncreaseBasePriorityPrivilege", 31) == 0)
					seincreasebasepriorityprivilege->SetValue("1");
				else if (_strnicmp(tmpPrivBuf, "SeIncreaseQuotaPrivilege", 24) == 0)
					seincreasequotaprivilege->SetValue("1");
				else if (_strnicmp(tmpPrivBuf, "SeLoadDriverPrivilege", 21) == 0)
					seloaddriverprivilege->SetValue("1");
				else if (_strnicmp(tmpPrivBuf, "SeLockMemoryPrivilege", 21) == 0)
					selockmemoryprivilege->SetValue("1");
				else if (_strnicmp(tmpPrivBuf, "SeMachineAccountPrivilege", 25) == 0)
					semachineaccountprivilege->SetValue("1");
				else if (_strnicmp(tmpPrivBuf, "SeManageVolumePrivilege", 23) == 0)
					semanagevolumeprivilege->SetValue("1");
				else if (_strnicmp(tmpPrivBuf, "SeProfileSingleProcessPrivilege", 31) == 0)
					seprofilesingleprocessprivilege->SetValue("1");
				else if (_strnicmp(tmpPrivBuf, "SeRemoteShutdownPrivilege", 25) == 0)
					seremoteshutdownprivilege->SetValue("1");
				else if (_strnicmp(tmpPrivBuf, "SeRestorePrivilege", 18) == 0)
					serestoreprivilege->SetValue("1");
				else if (_strnicmp(tmpPrivBuf, "SeSecurityPrivilege", 19) == 0)
					sesecurityprivilege->SetValue("1");
				else if (_strnicmp(tmpPrivBuf, "SeShutdownPrivilege", 19) == 0)
					seshutdownprivilege->SetValue("1");
				else if (_strnicmp(tmpPrivBuf, "SeSyncAgentPrivilege", 20) == 0)
					sesyncagentprivilege->SetValue("1");
				else if (_strnicmp(tmpPrivBuf, "SeSystemEnvironmentPrivilege", 28) == 0)
					sesystemenvironmentprivilege->SetValue("1");
				else if (_strnicmp(tmpPrivBuf, "SeSystemProfilePrivilege", 24) == 0)
					sesystemprofileprivilege->SetValue("1");
				else if (_strnicmp(tmpPrivBuf, "SeSystemtimePrivilege", 21) == 0)
					sesystemtimeprivilege->SetValue("1");
				else if (_strnicmp(tmpPrivBuf, "SeTakeOwnershipPrivilege", 24) == 0)
					setakeownershipprivilege->SetValue("1");
				else if (_strnicmp(tmpPrivBuf, "SeTcbPrivilege", 14) == 0)
					setcbprivilege->SetValue("1");
				else if (_strnicmp(tmpPrivBuf, "SeUndockPrivilege", 17) == 0)
					seundockprivilege->SetValue("1");
				else if (_strnicmp(tmpPrivBuf, "SeUnsolicitedInputPrivilege", 27) == 0)
					seunsolicitedinputprivilege->SetValue("1");
				else if (_strnicmp(tmpPrivBuf, "SeBatchLogonRight", 17) == 0)
					sebatchlogonright->SetValue("1");
				else if (_strnicmp(tmpPrivBuf, "SeDenyBatchLogonRight", 21) == 0)
					sedenybatchLogonright->SetValue("1");
				else if (_strnicmp(tmpPrivBuf, "SeDenyInteractiveLogonRight", 27) == 0)
					sedenyinteractivelogonright->SetValue("1");
				else if (_strnicmp(tmpPrivBuf, "SeDenyNetworkLogonRight", 23) == 0)
					sedenynetworklogonright->SetValue("1");
				else if (_strnicmp(tmpPrivBuf, "SeDenyServiceLogonRight", 23) == 0)
					sedenyservicelogonright->SetValue("1");
				else if (_strnicmp(tmpPrivBuf, "SeTrustedCredManAccessPrivilege", 31) == 0)
					setrustedcredmanaccessnameright->SetValue("1");
				else if (_strnicmp(tmpPrivBuf, "SeDenyRemoteInteractiveLogonRight", 33) == 0)
					sedenyremoteInteractivelogonright->SetValue("1");
				else if (_strnicmp(tmpPrivBuf, "SeInteractiveLogonRight", 23) == 0)
					seinteractivelogonright->SetValue("1");
				else if (_strnicmp(tmpPrivBuf, "SeNetworkLogonRight", 19) == 0)
					senetworklogonright->SetValue("1");
				else if (_strnicmp(tmpPrivBuf, "SeRemoteInteractiveLogonRight", 29) == 0)
					seremoteinteractivelogonright->SetValue("1");
				else if (_strnicmp(tmpPrivBuf, "SeServiceLogonRight", 19) == 0)
					seservicelogonright->SetValue("1");
				// new in 5.3
				else if (_strnicmp(tmpPrivBuf, "SeTimeZonePrivilege", 19) == 0)
					setimezoneprivilege->SetValue("1");
				else if (_strnicmp(tmpPrivBuf, "SeCreateSymbolicLinkPrivilege", 29) == 0)
					secreatesymboliclinkprivilege->SetValue("1");
				else if (_strnicmp(tmpPrivBuf, "SeIncreaseWorkingSetPrivilege", 29) == 0)
					seincreaseworkingsetprivilege->SetValue("1");
				else if (_strnicmp(tmpPrivBuf, "SeRelabelPrivilege", 18) == 0)
					serelabelprivilege->SetValue("1");
				else { 
					/*
					 I commented this code out because the probe should not fail to collect an object simply 
					 because there are additional priveleges. Instead just log the additional privelages with messages
					 on the item.
					delete item;
					LsaFreeMemory(userRights);
					LsaClose(polHandle);
					string errMsg = "Unknown account privilege: ";
					errMsg.append(tmpPrivBuf);
					throw ProbeException(errMsg);*/
					string msgStr = "Unknown account privilege: ";
					msgStr.append(tmpPrivBuf); 
					item->AppendMessage(new OvalMessage(msgStr));
				}
			}

			// Free memory.
			LsaFreeMemory(userRights);		
		}
	} else {

		Item* item = this->CreateItem();
		item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
		item->AppendElement(new ItemEntity("security_principle", accountNameIn, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_DOES_NOT_EXIST));
		items->push_back(item);

	}

	// close handle
	LsaClose(polHandle);

	return isComplete;
}
