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

#include <Common.h>
#include "AuditEventPolicyProbe.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  AuditEventPolicyProbe Class  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AuditEventPolicyProbe* AuditEventPolicyProbe::instance = NULL;

AuditEventPolicyProbe::AuditEventPolicyProbe() {

}

AuditEventPolicyProbe::~AuditEventPolicyProbe() {
  instance = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* AuditEventPolicyProbe::Instance() {

	// Use lazy initialization
	if(instance == NULL) 
		instance = new AuditEventPolicyProbe();

	return instance;	
}

ItemVector* AuditEventPolicyProbe::CollectItems(Object* /*object*/) {
	ItemVector *collectedItems = NULL;

	Item* item = NULL;	

	//
	// First open a handle to a policy object.
	// msdn link on Opening a Policy Object Handle
	// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/secmgmt/security/opening_a_policy_object_handle.asp
	//

	LSA_OBJECT_ATTRIBUTES ObjectAttributes;
	NTSTATUS ntsResult;
	LSA_HANDLE lsahPolicyHandle;

	// Object attributes are reserved, so initialize to zeroes.
	ZeroMemory(&ObjectAttributes, sizeof(ObjectAttributes));

	// Get a handle to the Policy object.
	ntsResult = LsaOpenPolicy(
				NULL,    //Name of the target system. NULL opens localhost
				&ObjectAttributes, //Object attributes.
				POLICY_VIEW_AUDIT_INFORMATION , //Desired access permissions. POLICY_ALL_ACCESS
				&lsahPolicyHandle  // Receives the policy handle.
				);

	if (ntsResult != ERROR_SUCCESS) {
		DWORD errorCode = LsaNtStatusToWinError(ntsResult);
		if(errorCode == ERROR_MR_MID_NOT_FOUND) {
			throw ProbeException("Error obtaining audit event policy information - (win32) " + Common::ToString(LsaNtStatusToWinError(ntsResult)));
		} else {
			throw ProbeException("Error obtaining audit event policy information - (win32) " + WindowsCommon::GetErrorMessage(errorCode));
		}
	} 

	//
	// Second use the policy handle to get the audit event data.
	// msdn link on getting audit event data:
	// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/secauthn/security/lsaqueryinformationpolicy.asp
	// The call below will retrieve a POLICY_AUDIT_EVENTS_INFO structure. 
	// msdn link on the above structure:
	// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/secmgmt/security/policy_audit_events_info.asp
	// if auditing is turned on loop through the array of POLICY_AUDIT_EVENT_OPTIONS
	// msdn link on the above structure
	// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/secmgmt/security/policy_audit_event_type.asp
	// 

	ntsResult = ERROR_SUCCESS;
	PPOLICY_AUDIT_EVENTS_INFO pPAEInfo = NULL;

	ntsResult = LsaQueryInformationPolicy(
				lsahPolicyHandle,                // Open handle to a Policy object.
				PolicyAuditEventsInformation,	 // The information to get.
				(PVOID *)&pPAEInfo               // Storage for the information.
				);

	if (ntsResult == ERROR_SUCCESS) {  

		item = this->CreateItem();
		item->SetStatus(OvalEnum::STATUS_EXISTS);
		collectedItems = new ItemVector();
		collectedItems->push_back(item);

		// if auditing is turned on loop through the auditing options
		if(pPAEInfo->AuditingMode) {

			// initialize the item with all of its fields
			ItemEntity *accountLogonItem = new ItemEntity("account_logon",  "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_ERROR);
			item->AppendElement(accountLogonItem);
			ItemEntity *accountManagementItem = new ItemEntity("account_management",  "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_ERROR);
			item->AppendElement(accountManagementItem);
			ItemEntity *detailedTrackingItem = new ItemEntity("detailed_tracking",  "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_ERROR);
			item->AppendElement(detailedTrackingItem);
			ItemEntity *directoryServiceAccessItem = new ItemEntity("directory_service_access",  "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_ERROR);
			item->AppendElement(directoryServiceAccessItem);
			ItemEntity *logonItem = new ItemEntity("logon",  "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_ERROR);
			item->AppendElement(logonItem);
			ItemEntity *objectAccessItem = new ItemEntity("object_access",  "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_ERROR);
			item->AppendElement(objectAccessItem);
			ItemEntity *policyChangeItem = new ItemEntity("policy_change",  "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_ERROR);
			item->AppendElement(policyChangeItem);
			ItemEntity *privilegeUseItem = new ItemEntity("privilege_use",  "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_ERROR);
			item->AppendElement(privilegeUseItem);
			ItemEntity *systemItem = new ItemEntity("system",  "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_ERROR);
			item->AppendElement(systemItem);

			
			ULONG i = 0;
			ULONG current = 0;
			while(i < pPAEInfo->MaximumAuditEventCount) {
				current = pPAEInfo->EventAuditingOptions[i];

				switch (i) {
					case AuditCategorySystem:
						this->ReadAuditOptions(item, systemItem, current); 
						break;
					case AuditCategoryLogon:
						this->ReadAuditOptions(item, logonItem, current); 
						break;
					case AuditCategoryObjectAccess:
						this->ReadAuditOptions(item, objectAccessItem, current); 
						break;
					case AuditCategoryPrivilegeUse:
						this->ReadAuditOptions(item, privilegeUseItem, current); 
						break;
					case AuditCategoryDetailedTracking:
						this->ReadAuditOptions(item, detailedTrackingItem, current); 
						break;
					case AuditCategoryPolicyChange:
						this->ReadAuditOptions(item, policyChangeItem, current); 
						break;
					case AuditCategoryAccountManagement:
						this->ReadAuditOptions(item, accountManagementItem, current); 
						break;
					case AuditCategoryDirectoryServiceAccess:
						this->ReadAuditOptions(item, directoryServiceAccessItem, current); 
						break;
					case AuditCategoryAccountLogon:
						this->ReadAuditOptions(item, accountLogonItem, current); 
						break;
					default:
						Log::Info("Unknown POLICY_AUDIT_EVENT_TYPE. ");
						break;
				}
				i++;
			}

		} else {
			// auditing is off so set all items to no auditing
			item->AppendElement(new ItemEntity("account_logon",  "AUDIT_NONE", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
			item->AppendElement(new ItemEntity("account_management",  "AUDIT_NONE", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
			item->AppendElement(new ItemEntity("detailed_tracking",  "AUDIT_NONE", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
			item->AppendElement(new ItemEntity("directory_service_access",  "AUDIT_NONE", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
			item->AppendElement(new ItemEntity("logon",  "AUDIT_NONE", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
			item->AppendElement(new ItemEntity("object_access",  "AUDIT_NONE", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
			item->AppendElement(new ItemEntity("policy_change",  "AUDIT_NONE", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
			item->AppendElement(new ItemEntity("privilege_use",  "AUDIT_NONE", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
			item->AppendElement(new ItemEntity("system",  "AUDIT_NONE", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
		}
	} else {

		DWORD errorCode = LsaNtStatusToWinError(ntsResult);
		if(errorCode == ERROR_MR_MID_NOT_FOUND) {
			throw ProbeException("Error obtaining audit event policy information - (win32) " + Common::ToString(LsaNtStatusToWinError(ntsResult)));
		} else {
			throw ProbeException("Error obtaining audit event policy information - (win32) " + WindowsCommon::GetErrorMessage(errorCode));
		}
	}

	LsaFreeMemory(pPAEInfo);
	
	
	// close policy handle
	ntsResult = LsaClose(lsahPolicyHandle);

	return collectedItems;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
Item* AuditEventPolicyProbe::CreateItem() {

	Item* item = new Item(0, 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows", 
						"win-sc", 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows windows-system-characteristics-schema.xsd", 
						OvalEnum::STATUS_ERROR, 
						"auditeventpolicy_item");

	return item;
}

void AuditEventPolicyProbe::ReadAuditOptions(Item* item, ItemEntity* itemEntity, ULONG auditPolicy) {

	if(auditPolicy & POLICY_AUDIT_EVENT_NONE) {
        itemEntity->SetValue("AUDIT_NONE");
		itemEntity->SetStatus(OvalEnum::STATUS_EXISTS);
	} else if(auditPolicy & POLICY_AUDIT_EVENT_FAILURE && auditPolicy & POLICY_AUDIT_EVENT_SUCCESS) {
		itemEntity->SetValue("AUDIT_SUCCESS_FAILURE");
		itemEntity->SetStatus(OvalEnum::STATUS_EXISTS);
	} else if(auditPolicy & POLICY_AUDIT_EVENT_FAILURE) {
		itemEntity->SetValue("AUDIT_FAILURE");
		itemEntity->SetStatus(OvalEnum::STATUS_EXISTS);
	} else if(auditPolicy & POLICY_AUDIT_EVENT_SUCCESS) {
		itemEntity->SetValue("AUDIT_SUCCESS");
		itemEntity->SetStatus(OvalEnum::STATUS_EXISTS);
	} else if (auditPolicy == POLICY_AUDIT_EVENT_UNCHANGED){
		// Originally I thought that I should not get here. 
		// These are my original notes on this:
		// For some reason we seem to hit this condition 
		// when looking at the permissions for AuditCategoryDetailedTracking
		// As documented at the link below i would not expect to get this value 
		// after doing a query. The value should only be used when doing a set.
		// Reference url:
		// http://msdn2.microsoft.com/en-us/library/ms721901.aspx

		// I have now looked into this a bit more. This appears to be the value
		// we get when the the auditing option is not set in the gui. I verified 
		// this through testing. I have not been able to verify it through the 
		// Microsoft API documentation. For now i think it is safe to assume that 
		// this value can be treated as AUDIT_NONE.
		//

		itemEntity->SetValue("AUDIT_NONE");
		itemEntity->SetStatus(OvalEnum::STATUS_EXISTS);

		/*Log::Debug("Unexpected POLICY_AUDIT_EVENT_TYPE found: POLICY_AUDIT_EVENT_UNCHANGED.");
		item->AppendMessage(new OvalMessage(itemEntity->GetName() + " policy - Unexpected POLICY_AUDIT_EVENT_TYPE found: POLICY_AUDIT_EVENT_UNCHANGED.", OvalEnum::LEVEL_WARNING));
		itemEntity->SetStatus(OvalEnum::STATUS_ERROR);*/
	} else {
		// should never get here
		Log::Debug("Unexpected POLICY_AUDIT_EVENT_TYPE found.");
		item->AppendMessage(new OvalMessage(itemEntity->GetName() + " policy - Unexpected POLICY_AUDIT_EVENT_TYPE found.", OvalEnum::LEVEL_WARNING));
		itemEntity->SetStatus(OvalEnum::STATUS_ERROR);
	} 
}
