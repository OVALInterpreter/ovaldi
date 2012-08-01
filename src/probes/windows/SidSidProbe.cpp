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

#include "SidSidProbe.h"

//****************************************************************************************//
//								SidSidProbe Class										  //	
//****************************************************************************************//
SidSidProbe* SidSidProbe::instance = NULL;

SidSidProbe::SidSidProbe() : AbsProbe() {

}

SidSidProbe::~SidSidProbe() {
  instance = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* SidSidProbe::Instance() {

	// Use lazy initialization
	if(instance == NULL) 
		instance = new SidSidProbe();

	return instance;	
}

ItemVector* SidSidProbe::CollectItems(Object *object) {
	// -----------------------------------------------------------------------
	//
	//  ABSTRACT
	//
	//  Run the sid_sid probe. Return a vector of Items that match 
	//
	//	if not using variables
	//		if operation == equals
	//			- call LookupAccountName and return collected item
	//				http://msdn.microsoft.com/library/default.asp?url=/library/en-us/secauthz/security/lookupaccountname.asp
	//
	//		if operation == not equals || operation == pattern match
	//			- get all sids see note below...
	//			- determine set of sids that match the specified criteria
	//			- for each match call LookupAccountName and return collected item
	//				http://msdn.microsoft.com/library/default.asp?url=/library/en-us/secauthz/security/lookupaccountname.asp
	//
	//	Behaviors:
	//	The following behaviors are supported when collecting sid objects:
	//	 - include_group: should be paired with the "resolve_group" behavior.
	//	   When true, include the group in the set of sids. When false, do not
	//	   include the group in the set of sids.
	//
	//	 - resolve_group: when true, if the trustee name specifies a group 
	//	   then return all users in the group. When false just return sid 
	//	   for the group.
	//	
	//
	//
	//	NOTE:
	//	I have not yet found a way to get all the SIDs on a system. To handle this
	//	for now I am getting all the sids of the local users, local groups, and
	//	global groups. Then expanding all the groups found above to get all of their
	//	members. Additionally I look up all well known trustee names by their sid string
	//	to ensure all well know sids are captured. For now this will have to suffice as
	//	the set of all SIDs on a system. 
	//
	//	NOTE: 
	//	Trustee name is expected to be a fully qualified name like:
	//		domain_name\user_name
	//	or a well known account name like:
	//		Administrators, or SYSTEM, or Administrator, or Users
	//
	//	TODO: Ensure that a given account is only looked up once
	//		
	//	Get All Local users with:
	//	http://msdn.microsoft.com/library/default.asp?url=/library/en-us/netmgmt/netmgmt/netuserenum.asp
	//	
	//	Get all global groups with:
	//	http://msdn.microsoft.com/library/default.asp?url=/library/en-us/netmgmt/netmgmt/netgroupenum.asp
	//	Get global group members with:
	//	http://msdn.microsoft.com/library/default.asp?url=/library/en-us/netmgmt/netmgmt/netgroupgetusers.asp
	//
	//	Get all local groups with:
	//	http://msdn.microsoft.com/library/default.asp?url=/library/en-us/netmgmt/netmgmt/netlocalgroupenum.asp
	//	Get local group members with:
	//	http://msdn.microsoft.com/library/default.asp?url=/library/en-us/netmgmt/netmgmt/netlocalgroupgetmembers.asp
	//
	// -----------------------------------------------------------------------
	
	ItemVector *collectedItems = new ItemVector();
	
	// get the trustee_sid from the provided object
	ObjectEntity* trusteeSid = object->GetElementByName("trustee_sid");

	// check datatypes - only allow string
	if(trusteeSid->GetDatatype() != OvalEnum::DATATYPE_STRING) {
		throw ProbeException("Error: invalid data type specified on trustee_sid. Found: " + OvalEnum::DatatypeToString(trusteeSid->GetDatatype()));
	}	

	// check operation - only allow  equals, not equals and pattern match
	if(trusteeSid->GetOperation() != OvalEnum::OPERATION_EQUALS && trusteeSid->GetOperation() != OvalEnum::OPERATION_PATTERN_MATCH && trusteeSid->GetOperation() != OvalEnum::OPERATION_NOT_EQUAL) {
		throw ProbeException("Error: invalid operation specified on trustee_sid. Found: " + OvalEnum::OperationToString(trusteeSid->GetOperation()));
	}

	// support behaviors - init with defaults.
	bool includeGroupBehavior = true;
	bool resolveGroupBehavior = false;
	if(object->GetBehaviors()->size() != 0) {
		BehaviorVector* behaviors = object->GetBehaviors();
		BehaviorVector::iterator iterator;
		for(iterator = behaviors->begin(); iterator != behaviors->end(); iterator++) {
			Behavior* behavior = (*iterator);
			if(behavior->GetName().compare("include_group") == 0)  {
                if(behavior->GetValue().compare("false") == 0) {
				    includeGroupBehavior = false;
                }
            } else if(behavior->GetName().compare("resolve_group") == 0) {
                if(behavior->GetValue().compare("true") == 0) {
				    resolveGroupBehavior = true;
                }
			} else {
                Log::Info("Unsupported behavior found when collecting " + object->GetId() + " Found behavior: " + behavior->GetName() + " = " + behavior->GetValue());
			}
		}		
	}

	// get the SID data
	if(trusteeSid->GetVarRef() == NULL) {
		if(trusteeSid->GetOperation() == OvalEnum::OPERATION_EQUALS) {
			this->GetAccountInformation(trusteeSid->GetValue(), resolveGroupBehavior, includeGroupBehavior, collectedItems);
		} else {

			bool isRegex = false;
			if(trusteeSid->GetOperation() == OvalEnum::OPERATION_PATTERN_MATCH)
				isRegex = true;
			
			// Get all trustee_sids on the system...
			StringSet* allTrusteeSids = WindowsCommon::GetAllTrusteeSIDs();	
			
			// Get the set of trustee names that match the ItemEntity.
			StringSet::iterator iterator;
			for(iterator = allTrusteeSids->begin(); iterator != allTrusteeSids->end(); iterator++) {
				string curr = (*iterator);
				if(this->IsMatch(trusteeSid->GetValue(), (*iterator), isRegex)) {
					this->GetAccountInformation((*iterator), resolveGroupBehavior, includeGroupBehavior, collectedItems);
				}
			}
		}

	} else {			
		// Get all trustee_sids on the system...
		StringSet* allTrusteeSids = WindowsCommon::GetAllTrusteeSIDs();

		// loop through all trustee sids on the system
		// only keep those that match operation and value and var check
		StringSet::iterator it;
		ItemEntity* tmp = this->CreateItemEntity(trusteeSid);
		for(it = allTrusteeSids->begin(); it != allTrusteeSids->end(); it++) {
			tmp->SetValue((*it));
			if(trusteeSid->Analyze(tmp) == OvalEnum::RESULT_TRUE) {
				this->GetAccountInformation((*it), resolveGroupBehavior, includeGroupBehavior, collectedItems);
			}
		}
	}

	return collectedItems;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
Item* SidSidProbe::CreateItem() {

	Item* item = new Item(0, 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows", 
						"win-sc", 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows windows-system-characteristics-schema.xsd", 
						OvalEnum::STATUS_ERROR, 
						"sid_sid_item");

	return item;
}

bool SidSidProbe::GetAccountInformation(string sidStr,  bool resolveGroupBehavior, bool includeGroupBehavior, ItemVector* items) {

	bool isComplete = true;

	// lookup the trustee name
	try {
		string accountNameStr = "";
		string domainStr = "";		
		bool isGroup = WindowsCommon::LookUpTrusteeSid(sidStr, &accountNameStr, &domainStr);
		
		// if a group
		// handle behaviors
		if(isGroup && resolveGroupBehavior) {

			if(includeGroupBehavior) {
				Item* item = this->CreateItem();
				item->SetStatus(OvalEnum::STATUS_EXISTS);
				item->AppendElement(new ItemEntity("trustee_sid", sidStr, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
				item->AppendElement(new ItemEntity("trustee_name", accountNameStr, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
				item->AppendElement(new ItemEntity("trustee_domain", domainStr, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
				items->push_back(item);
			} 
			
			// Get all the accounts in the group
			StringSet* groupMemberSids = new StringSet();
			WindowsCommon::ExpandGroupBySID(sidStr, groupMemberSids, includeGroupBehavior, resolveGroupBehavior);
			StringSet::iterator iterator;
			for(iterator = groupMemberSids->begin(); iterator != groupMemberSids->end(); iterator++) {
				// make recursive call...
				try {
					isComplete = this->GetAccountInformation((*iterator), resolveGroupBehavior, includeGroupBehavior, items);
				} catch (Exception ex) {
					isComplete = false;
					Log::Debug(ex.GetErrorMessage());
				}
			}
			delete groupMemberSids;

		} else {
			Item* item = this->CreateItem();
			item->SetStatus(OvalEnum::STATUS_EXISTS);
			item->AppendElement(new ItemEntity("trustee_sid", sidStr, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
			item->AppendElement(new ItemEntity("trustee_name", accountNameStr, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
			item->AppendElement(new ItemEntity("trustee_domain", domainStr, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
			items->push_back(item);
		}

	} catch(ProbeException ex) {
		// only way to have a notice level exception thrown here 
		// is for the account to not be found. In that case return an 
		// item with a status of does not exist.
		if(ex.GetSeverity() == ERROR_NOTICE) {
			Item* item = this->CreateItem();
			item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
			item->AppendElement(new ItemEntity("trustee_sid", sidStr, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));
			items->push_back(item);
		} else {
			throw ex;
		}
	}

	return isComplete;
}
