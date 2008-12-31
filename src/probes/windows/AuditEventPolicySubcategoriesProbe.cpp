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

#include "AuditEventPolicySubcategoriesProbe.h"

typedef pair<GUID, string> GuidString;
vector<GuidString> _guidItemElementNameVector;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  AuditEventPolicyProbe Class  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AuditEventPolicySubcategoriesProbe* AuditEventPolicySubcategoriesProbe::instance = NULL;

AuditEventPolicySubcategoriesProbe::AuditEventPolicySubcategoriesProbe() {
	Init();
}

AuditEventPolicySubcategoriesProbe::~AuditEventPolicySubcategoriesProbe() {
  instance = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

/*
void AuditEventPolicySubcategoriesProbe::DeleteInstance() {
	if(instance != NULL) {
		delete instance;
		instance = NULL;
	}
}
*/
AbsProbe* AuditEventPolicySubcategoriesProbe::Instance() {

	// Use lazy initialization
	if(instance == NULL) 
		instance = new AuditEventPolicySubcategoriesProbe();

	return instance;	
}

//TODO: Split this into at least 2 methods
ItemVector* AuditEventPolicySubcategoriesProbe::CollectItems(Object *object) {

	ItemVector *collectedItems = NULL;
	Item* item = NULL;	
	LSA_OBJECT_ATTRIBUTES ObjectAttributes;
	NTSTATUS ntsResult;
	LSA_HANDLE lsahPolicyHandle;
	GUID *pAuditSubCategoryGuids = NULL;
	AUDIT_POLICY_INFORMATION *pAuditPolicies = NULL;
	PPOLICY_AUDIT_EVENTS_INFO pPAEInfo = NULL;

	try {

		// Object attributes are reserved, so initialize to zeroes.
		ZeroMemory(&ObjectAttributes, sizeof(ObjectAttributes));

		Log::Debug("AuditEventPolicySubcategoriesProbe::CollectItems() - About to call LsaOpenPolicy()");
		
		// Get a handle to the Policy object.
		ntsResult = LsaOpenPolicy(
					NULL,							// Name of the target system. NULL opens localhost
					&ObjectAttributes,				// Object attributes.
					POLICY_VIEW_AUDIT_INFORMATION , // Desired access permissions. POLICY_ALL_ACCESS
					&lsahPolicyHandle				// Receives the policy handle.
					);

		Log::Debug("AuditEventPolicySubcategoriesProbe::CollectItems() - After call to LsaOpenPolicy()");

		if (ntsResult != ERROR_SUCCESS) {
			DWORD errorCode = LsaNtStatusToWinError(ntsResult);
			if(errorCode == ERROR_MR_MID_NOT_FOUND) {
				throw ProbeException("Error obtaining audit event policy information - (win32) " + LsaNtStatusToWinError(ntsResult));
			} else {
				throw ProbeException("Error obtaining audit event policy information - (win32) " + WindowsCommon::GetErrorMessage(errorCode));
			}
		} 

		ntsResult = ERROR_SUCCESS;
		PWCHAR name = NULL;

		Log::Debug("AuditEventPolicySubcategoriesProbe::CollectItems() - About to call LsaQueryInformationPolicy()");

		ntsResult = LsaQueryInformationPolicy(
					lsahPolicyHandle,                // Open handle to a Policy object.
					PolicyAuditEventsInformation,	 // The information to get.
					(PVOID *)&pPAEInfo               // Storage for the information.
					);

		Log::Debug("AuditEventPolicySubcategoriesProbe::CollectItems() - After call to LsaQueryInformationPolicy()");

		if (ntsResult == ERROR_SUCCESS) {  

			item = this->CreateItem();
			item->SetStatus(OvalEnum::STATUS_EXISTS);
			collectedItems = new ItemVector();
			collectedItems->push_back(item);

			ULONG current = 0;			

			// if auditing is turned on loop through the auditing options
			if(pPAEInfo->AuditingMode) {
				Log::Debug("AuditEventPolicySubcategoriesProbe::CollectItems() - pPAEInfo->AuditingMode is true");

				// Cycle through event categories, 
				// in each category collect the subcategory setting and stuff into the item			
				ULONG policyAuditEventType = 0;
				while(policyAuditEventType < pPAEInfo->MaximumAuditEventCount) {
					GUID auditCategoryId;
					ULONG subCategoryCount = 0;

					Log::Debug("AuditEventPolicySubcategoriesProbe::CollectItems() - Before call to AuditLookupCategoryGuidFromCategoryId()");
					if(AuditLookupCategoryGuidFromCategoryId((POLICY_AUDIT_EVENT_TYPE)policyAuditEventType, &auditCategoryId) == FALSE) {
						throw ProbeException("Error retrieving category GUID.  ErrorCode:" + GetLastError());
					}

					Log::Debug("AuditEventPolicySubcategoriesProbe::CollectItems() - Before call to AuditLookupCategoryGuidFromCategoryId()");
					if(AuditEnumerateSubCategories(&auditCategoryId, FALSE, &pAuditSubCategoryGuids, &subCategoryCount) == FALSE) {
						throw ProbeException("Error enumerating audit event policy subcategories.  Error Code:" + GetLastError());
					}
							
					Log::Debug("AuditEventPolicySubcategoriesProbe::CollectItems() - Before call to AuditQuerySystemPolicy()");
					if(AuditQuerySystemPolicy(pAuditSubCategoryGuids, subCategoryCount, &pAuditPolicies) == FALSE) {
						throw ProbeException("Error retrieving policy information for audit event policy subcategories.  Error Code:" + GetLastError());
					}

					Log::Debug("AuditEventPolicySubcategoriesProbe::CollectItems() - Before subcategories loop.  Count=" + subCategoryCount);

					// Probe each subcategory in the category
					for(ULONG subcategoryIndex = 0; subcategoryIndex < subCategoryCount; subcategoryIndex++) {
						AUDIT_POLICY_INFORMATION currentPolicy = pAuditPolicies[subcategoryIndex];
						
						string itemEntityName = GetItemEntityNameFromGuid(currentPolicy.AuditSubCategoryGuid);

						if(itemEntityName != "") {
							Log::Debug("AuditEventPolicySubcategoriesProbe::CollectItems() - itemEntityName=" + itemEntityName);
							ItemEntity *pItemEntity = new ItemEntity(itemEntityName,  "", OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_ERROR);
							item->AppendElement(pItemEntity);

							ReadAuditOptions(item, pItemEntity, currentPolicy.AuditingInformation);
						} else {
							
							Log::Debug("AuditEventPolicySubcategoriesProbe::CollectItems() - GUID not found.  Skipping.");
						}
					}

					policyAuditEventType++;
				}
			} else {
				Log::Debug("AuditEventPolicySubcategoriesProbe::CollectItems() - pPAEInfo->AuditingMode is false");	
				
				// auditing is off so set all items to no auditing				
				for(vector<GuidString>::iterator it = _guidItemElementNameVector.begin(); it != _guidItemElementNameVector.end(); ++it) {
					item->AppendElement(new ItemEntity(it->second,  "AUDIT_NONE", OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
				}		
			}
		} else {

			DWORD errorCode = LsaNtStatusToWinError(ntsResult);
			if(errorCode == ERROR_MR_MID_NOT_FOUND) {
				throw ProbeException("Error obtaining audit event policy information - (win32) " + LsaNtStatusToWinError(ntsResult));
			} else {
				throw ProbeException("Error obtaining audit event policy information - (win32) " + WindowsCommon::GetErrorMessage(errorCode));
			}
		}
	} catch(...) {

		Log::Debug("AuditEventPolicySubcategoriesProbe: Exception in CollectItems()");

		if(pPAEInfo != NULL) {
			LsaFreeMemory(pPAEInfo);
		}
		
		ntsResult = LsaClose(lsahPolicyHandle);

		if(pAuditSubCategoryGuids != NULL) {
			AuditFree(pAuditSubCategoryGuids);
		}

		if(pAuditPolicies != NULL) {
			AuditFree(pAuditPolicies);
		}

		throw;
	}

	// Cleanup - This is a duplicate of the catch block.  Would be nice to come up with a consolidated pattern.
	if(pPAEInfo != NULL) {
		LsaFreeMemory(pPAEInfo);
	}

	ntsResult = LsaClose(lsahPolicyHandle);

	if(pAuditSubCategoryGuids != NULL) {
		AuditFree(pAuditSubCategoryGuids);
	}

	if(pAuditPolicies != NULL) {
		AuditFree(pAuditPolicies);
	}
	
	Log::Debug("AuditEventPolicySubcategoriesProbe::CollectItems - about to return");	
	return collectedItems;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
Item* AuditEventPolicySubcategoriesProbe::CreateItem() {

	Item* item = new Item(0, 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows", 
						"win-sc", 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows windows-system-characteristics-schema.xsd", 
						OvalEnum::STATUS_ERROR, 
						"auditeventpolicysubcategories_item");

	return item;
}

void AuditEventPolicySubcategoriesProbe::ReadAuditOptions(Item* item, ItemEntity* itemEntity, ULONG auditPolicy) {

	if(auditPolicy & POLICY_AUDIT_EVENT_NONE) {
        itemEntity->SetValue("AUDIT_NONE");
		itemEntity->SetStatus(OvalEnum::STATUS_EXISTS);
		Log::Debug("AuditEventPolicySubcategoriesProbe::ReadAuditOptions() - None / Status Exists");
	} else if(auditPolicy & POLICY_AUDIT_EVENT_FAILURE && auditPolicy & POLICY_AUDIT_EVENT_SUCCESS) {
		itemEntity->SetValue("AUDIT_SUCCESS_FAILURE");
		itemEntity->SetStatus(OvalEnum::STATUS_EXISTS);
		Log::Debug("AuditEventPolicySubcategoriesProbe::ReadAuditOptions() - Success Failure / Status Exists");
	} else if(auditPolicy & POLICY_AUDIT_EVENT_FAILURE) {
		itemEntity->SetValue("AUDIT_FAILURE");
		itemEntity->SetStatus(OvalEnum::STATUS_EXISTS);
		Log::Debug("AuditEventPolicySubcategoriesProbe::ReadAuditOptions() - Failure / Status Exists");
	} else if(auditPolicy & POLICY_AUDIT_EVENT_SUCCESS) {
		itemEntity->SetValue("AUDIT_SUCCESS");
		itemEntity->SetStatus(OvalEnum::STATUS_EXISTS);
		Log::Debug("AuditEventPolicySubcategoriesProbe::ReadAuditOptions() - Success / Status Exists");
	} else if (auditPolicy == POLICY_AUDIT_EVENT_UNCHANGED){
		
		//
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

	} else {
		// should never get here
		Log::Debug("Unexpected POLICY_AUDIT_EVENT_TYPE found.");
		item->AppendMessage(new OvalMessage(itemEntity->GetName() + " policy - Unexpected POLICY_AUDIT_EVENT_TYPE found.", OvalEnum::LEVEL_WARNING));
		itemEntity->SetStatus(OvalEnum::STATUS_ERROR);
		Log::Debug("AuditEventPolicySubcategoriesProbe::ReadAuditOptions() - Unexpected Event / Status Exists");
	} 
}

void AuditEventPolicySubcategoriesProbe::Init() {
	Log::Debug("AuditEventPolicySubcategoriesProbe::ReadAuditOptions() - Enter");

	InsertIntoGuidItemEntityNameMapping(Audit_System_SecurityStateChange, "security_state_change");
	InsertIntoGuidItemEntityNameMapping(Audit_AccountLogon_CredentialValidation, "credential_validation");
	InsertIntoGuidItemEntityNameMapping(Audit_AccountLogon_Kerberos, "kerberos_ticket_events"); //?
	InsertIntoGuidItemEntityNameMapping(Audit_AccountLogon_Others, "other_account_logon_events");
	InsertIntoGuidItemEntityNameMapping(Audit_AccountManagement_ApplicationGroup, "application_group_management");
	InsertIntoGuidItemEntityNameMapping(Audit_AccountManagement_ComputerAccount, "computer_account_management");
	InsertIntoGuidItemEntityNameMapping(Audit_AccountManagement_DistributionGroup, "distribution_group_management");
	InsertIntoGuidItemEntityNameMapping(Audit_AccountManagement_Others, "other_account_management_events");
	InsertIntoGuidItemEntityNameMapping(Audit_AccountManagement_SecurityGroup, "security_group_management");
	InsertIntoGuidItemEntityNameMapping(Audit_AccountManagement_UserAccount, "user_account_management");
	InsertIntoGuidItemEntityNameMapping(Audit_DetailedTracking_DpapiActivity, "dpapi_activity");
	InsertIntoGuidItemEntityNameMapping(Audit_DetailedTracking_ProcessCreation, "process_creation");
	InsertIntoGuidItemEntityNameMapping(Audit_DetailedTracking_ProcessTermination, "process_termination");
	InsertIntoGuidItemEntityNameMapping(Audit_DetailedTracking_RpcCall, "rpc_events");
	InsertIntoGuidItemEntityNameMapping(Audit_DSAccess_DSAccess, "directory_service_access");
	InsertIntoGuidItemEntityNameMapping(Audit_DsAccess_AdAuditChanges, "directory_service_changes");
	InsertIntoGuidItemEntityNameMapping(Audit_Ds_Replication, "directory_service_replication");
	InsertIntoGuidItemEntityNameMapping(Audit_Ds_DetailedReplication, "detailed_directory_service_replication");
	InsertIntoGuidItemEntityNameMapping(Audit_Logon_AccountLockout, "account_lockout");
	InsertIntoGuidItemEntityNameMapping(Audit_Logon_IPSecMainMode, "ipsec_main_mode");
	InsertIntoGuidItemEntityNameMapping(Audit_Logon_IPSecQuickMode,"ipsec_quick_mode");
	InsertIntoGuidItemEntityNameMapping(Audit_Logon_Logoff, "logoff"); 
	InsertIntoGuidItemEntityNameMapping(Audit_Logon_Logon, "logon");
	InsertIntoGuidItemEntityNameMapping(Audit_Logon_Others, "other_logon_logoff_events");
	InsertIntoGuidItemEntityNameMapping(Audit_Logon_SpecialLogon, "special_logon");
	InsertIntoGuidItemEntityNameMapping(Audit_ObjectAccess_ApplicationGenerated, "application_generated");
	InsertIntoGuidItemEntityNameMapping(Audit_ObjectAccess_CertificationServices, "certification_services");
	InsertIntoGuidItemEntityNameMapping(Audit_ObjectAccess_Share, "file_share");
	InsertIntoGuidItemEntityNameMapping(Audit_ObjectAccess_FileSystem, "file_system");
	InsertIntoGuidItemEntityNameMapping(Audit_ObjectAccess_FirewallConnection, "filtering_platform_connection");
	InsertIntoGuidItemEntityNameMapping(Audit_ObjectAccess_FirewallPacketDrops, "filtering_platform_packet_drop");
	InsertIntoGuidItemEntityNameMapping(Audit_ObjectAccess_Handle, "handle_manipulation");
	InsertIntoGuidItemEntityNameMapping(Audit_ObjectAccess_Kernel, "kernel_object");
	InsertIntoGuidItemEntityNameMapping(Audit_ObjectAccess_Other, "other_object_access_events");
	InsertIntoGuidItemEntityNameMapping(Audit_ObjectAccess_Registry, "registry");
	InsertIntoGuidItemEntityNameMapping(Audit_ObjectAccess_Sam, "sam");
	InsertIntoGuidItemEntityNameMapping(Audit_PolicyChange_AuditPolicy, "audit_policy_change"); 
	InsertIntoGuidItemEntityNameMapping(Audit_PolicyChange_AuthenticationPolicy, "authentication_policy_change");
	InsertIntoGuidItemEntityNameMapping(Audit_PolicyChange_AuthorizationPolicy, "authorization_policy_change");
	InsertIntoGuidItemEntityNameMapping(Audit_PolicyChange_MpsscvRulePolicy, "mpssvc_rule_level_policy_change");
	InsertIntoGuidItemEntityNameMapping(Audit_PolicyChange_Others, "other_policy_change_events");
	InsertIntoGuidItemEntityNameMapping(Audit_PrivilegeUse_NonSensitive, "non_sensitive_privlege_use");
	InsertIntoGuidItemEntityNameMapping(Audit_PrivilegeUse_Others, "other_privlege_use_events");
	InsertIntoGuidItemEntityNameMapping(Audit_PrivilegeUse_Sensitive, "sensitive_privilege_use");
	InsertIntoGuidItemEntityNameMapping(Audit_System_IPSecDriverEvents, "ipsec_driver");
	InsertIntoGuidItemEntityNameMapping(Audit_System_Others, "other_system_events");
	InsertIntoGuidItemEntityNameMapping(Audit_System_SecuritySubsystemExtension, "security_system_extension");
	InsertIntoGuidItemEntityNameMapping(Audit_System_Integrity, "system_integrity");
	InsertIntoGuidItemEntityNameMapping(Audit_PolicyChange_WfpIPSecPolicy, "filtering_platform_policy_change"); 
	InsertIntoGuidItemEntityNameMapping(Audit_Logon_IPSecUserMode, "ipsec_extended_mode");
}

void AuditEventPolicySubcategoriesProbe::InsertIntoGuidItemEntityNameMapping(GUID guid, string itemEntityName) {
	GuidString guidItemEntityName(guid, itemEntityName);

	_guidItemElementNameVector.push_back(guidItemEntityName);
}


string AuditEventPolicySubcategoriesProbe::GetItemEntityNameFromGuid(GUID guid) {
	for(vector<GuidString>::iterator it = _guidItemElementNameVector.begin(); it != _guidItemElementNameVector.end(); ++it) {
		if(it->first == guid) {
			return it->second;
		}		
	}

	return "";
}
