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

#include "RegKeyAuditedPermissions53Probe.h"

//****************************************************************************************//
//                              RegKeyAuditedPermissions53Probe Class                     //
//****************************************************************************************//
RegKeyAuditedPermissions53Probe* RegKeyAuditedPermissions53Probe::instance = NULL;

RegKeyAuditedPermissions53Probe::RegKeyAuditedPermissions53Probe() {
}

RegKeyAuditedPermissions53Probe::~RegKeyAuditedPermissions53Probe() {
    instance = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* RegKeyAuditedPermissions53Probe::Instance() {
    // Use lazy initialization.
    if ( instance == NULL )
        instance = new RegKeyAuditedPermissions53Probe();

    return instance;
}

ItemVector* RegKeyAuditedPermissions53Probe::CollectItems ( Object* object ) {
    // Get the hive, key, and trustee.
    ObjectEntity* hiveEntity = object->GetElementByName ( "hive" );
    ObjectEntity* keyEntity = object->GetElementByName ( "key" );
    ObjectEntity* trusteeSIDEntity = object->GetElementByName ( "trustee_sid" );

    // Check trustee datatypes - only allow string.
    if ( trusteeSIDEntity->GetDatatype() != OvalEnum::DATATYPE_STRING ) {
        throw ProbeException ( "Error: Invalid data type specified on trustee_sid. Found: " + OvalEnum::DatatypeToString ( trusteeSIDEntity->GetDatatype() ) );
    }

    // Check trustee operation - only allow  equals, not equals and pattern match.
    if ( trusteeSIDEntity->GetOperation() != OvalEnum::OPERATION_EQUALS
            && trusteeSIDEntity->GetOperation() != OvalEnum::OPERATION_PATTERN_MATCH
            && trusteeSIDEntity->GetOperation() != OvalEnum::OPERATION_NOT_EQUAL ) {
        throw ProbeException ( "Error: Invalid operation specified on trustee_sid. Found: " + OvalEnum::OperationToString ( trusteeSIDEntity->GetOperation() ) );
    }

    // Support behaviors - init with defaults.
    bool includeGroupBehavior = true;
    bool resolveGroupBehavior = false;

    if ( object->GetBehaviors()->size() != 0 ) {
        BehaviorVector* behaviors = object->GetBehaviors();
        BehaviorVector::iterator iterator;

        for ( iterator = behaviors->begin(); iterator != behaviors->end(); iterator++ ) {
            Behavior* behavior = ( *iterator );

            if ( behavior->GetName().compare ( "include_group" ) == 0 )  {
                if ( behavior->GetValue().compare ( "false" ) == 0 ) {
                    includeGroupBehavior = false;
                }

            } else if ( behavior->GetName().compare ( "resolve_group" ) == 0 ) {
                if ( behavior->GetValue().compare ( "true" ) == 0 ) {
                    resolveGroupBehavior = true;
                }

                Log::Info ( "Deprecated behavior found when collecting " + object->GetId() + ". Found behavior: " + behavior->GetName() + " = " + behavior->GetValue() );

            } else if ( behavior->GetName().compare ( "max_depth" ) == 0 || behavior->GetName().compare ( "recurse_direction" ) == 0 ) {
                // Skip these as they are supported in the RegistryFinder class.
            } else {
                Log::Info ( "Unsupported behavior found when collecting " + object->GetId() + ". Found behavior: " + behavior->GetName() + " = " + behavior->GetValue() );
            }
        }
    }

    ItemVector *collectedItems = new ItemVector();
    RegistryFinder registryFinder;
    // Create a name ObjectEntity to provide to the RegistryFinder as it expects one, however, have the ObjectEntity set to NIL as its value does not matter.
    // Only the hive and key are relevant for this probe.
    ObjectEntity* nameEntity = new ObjectEntity();
    nameEntity->SetNil ( true );
    RegKeyVector* registryKeys = registryFinder.SearchRegistries ( hiveEntity, keyEntity, nameEntity, object->GetBehaviors() );

    if ( registryKeys->size() > 0 ) {
        RegKeyVector::iterator iterator;

        for ( iterator = registryKeys->begin(); iterator != registryKeys->end(); iterator++ ) {
            RegKey* registryKey = ( *iterator );
            Item* item = NULL;

            try {
                string registryKeyStr = RegistryFinder::BuildRegistryKey ( RegistryFinder::ConvertHiveForWindowsObjectName ( registryKey->GetHive() ), registryKey->GetKey() );
                StringSet* trusteeSIDs = this->GetTrusteesForWindowsObject ( SE_REGISTRY_KEY, registryKeyStr, trusteeSIDEntity, true, resolveGroupBehavior, includeGroupBehavior );

                if ( !trusteeSIDs->empty() ) {
                    for ( StringSet::iterator iterator = trusteeSIDs->begin(); iterator != trusteeSIDs->end(); iterator++ ) {
                        try {
                            Item* item = this->GetAuditedPermissions ( registryKey->GetHive(), registryKey->GetKey(), ( *iterator ) );

                            if ( item != NULL ) {
                                collectedItems->push_back ( item );
                            }

                        } catch ( ProbeException ex ) {
                            Log::Message ( "ProbeException caught when collecting: " + object->GetId() + " " +  ex.GetErrorMessage() );

                        } catch ( Exception ex ) {
                            Log::Message ( "Exception caught when collecting: " + object->GetId() + " " +  ex.GetErrorMessage() );
                        }
                    }

                    trusteeSIDs->clear();
                    delete trusteeSIDs;
                    trusteeSIDs = NULL;

                } else {
                    Log::Debug ( "No matching trustees found when getting audited permissions for object: " + object->GetId() );
                    StringSet* trusteeSIDs = new StringSet();

                    if ( this->ReportTrusteeDoesNotExist ( trusteeSIDEntity, trusteeSIDs, true ) ) {
                        for ( StringSet::iterator iterator = trusteeSIDs->begin(); iterator != trusteeSIDs->end(); iterator++ ) {
                            Item* item = this->CreateItem();
                            item->SetStatus ( OvalEnum::STATUS_DOES_NOT_EXIST );
                            item->AppendElement ( new ItemEntity ( "hive", registryKey->GetHive(), OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS ) );
                            item->AppendElement ( new ItemEntity ( "key", registryKey->GetKey(), OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS ) );
                            item->AppendElement ( new ItemEntity ( "trustee_sid", ( *iterator ), OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_DOES_NOT_EXIST ) );
                            collectedItems->push_back ( item );
                        }
                    }

                    trusteeSIDs->clear();
                    delete trusteeSIDs;
                    trusteeSIDs = NULL;
                }

            } catch ( ProbeException ex ) {
                Log::Message ( "ProbeException caught when collecting: " + object->GetId() + " " +  ex.GetErrorMessage() );

            } catch ( Exception ex ) {
                Log::Message ( "Exception caught when collecting: " + object->GetId() + " " +  ex.GetErrorMessage() );

            } catch ( ... ) {
                Log::Message ( "Unknown error when collecting " + object->GetId() );
            }

            delete registryKey;
            registryKey = NULL;
        }

    } else {
        StringSet* hives = NULL;

        if ( ( hives = registryFinder.ReportHiveDoesNotExist ( hiveEntity ) ) != NULL ) {
            for ( StringSet::iterator iterator1 = hives->begin(); iterator1 != hives->end(); iterator1++ ) {
                Item* item = NULL;
                item = this->CreateItem();
                item->SetStatus ( OvalEnum::STATUS_DOES_NOT_EXIST );
                item->AppendElement ( new ItemEntity ( "hive", ( *iterator1 ), OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_DOES_NOT_EXIST ) );
                collectedItems->push_back ( item );
            }

            if ( hives != NULL ) {
                hives->clear();
                delete hives;
                hives = NULL;
            }

        } else {
            Item* item = NULL;
            StringSet* hives = registryFinder.GetHives ( hiveEntity );

            for ( StringSet::iterator iterator1 = hives->begin(); iterator1 != hives->end() ; iterator1++ ) {
                StringSet* keys = NULL;

                if ( ( keys = registryFinder.ReportKeyDoesNotExist ( *iterator1, keyEntity ) ) != NULL ) {
                    for ( StringSet::iterator iterator2 = keys->begin(); iterator2 != keys->end(); iterator2++ ) {
                        item = this->CreateItem();
                        item->SetStatus ( OvalEnum::STATUS_DOES_NOT_EXIST );
                        item->AppendElement ( new ItemEntity ( "hive", ( *iterator1 ), OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS ) );
                        item->AppendElement ( new ItemEntity ( "key", ( *iterator2 ), OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_DOES_NOT_EXIST ) );
                        collectedItems->push_back ( item );
                    }
                }

                if ( keys != NULL ) {
                    keys->clear();
                    delete keys;
                    keys = NULL;
                }
            }

            hives->clear();
            delete hives;
            hives = NULL;
        }
    }

    registryKeys->clear();
    delete registryKeys;
    registryKeys = NULL;
    return collectedItems;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
Item* RegKeyAuditedPermissions53Probe::CreateItem() {
    Item* item = new Item ( 0,
                            "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows",
                            "win-sc",
                            "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows windows-system-characteristics-schema.xsd",
                            OvalEnum::STATUS_ERROR,
                            "regkeyauditedpermissions_item" );
    return item;
}

Item* RegKeyAuditedPermissions53Probe::GetAuditedPermissions ( string hiveStr, string keyStr, string trusteeSIDStr ) {
    Item* item = NULL;
    PSID pSid = NULL;
    PACCESS_MASK pSuccessfulAuditedRights = NULL;
    PACCESS_MASK pFailedAuditRights = NULL;
    // Build the registry key.
    string registryKey = RegistryFinder::BuildRegistryKey ( ( const string ) RegistryFinder::ConvertHiveForWindowsObjectName ( hiveStr ), ( const string ) keyStr );
    string baseErrMsg = "Error: Unable to get audited permissions for trustee: " + trusteeSIDStr + " from sacl for registry key: " + RegistryFinder::BuildRegistryKey ( ( const string ) hiveStr, ( const string ) keyStr );

    try {
        // Verify that the registry key exists.
        if ( RegistryFinder::GetHKeyHandle ( hiveStr, keyStr ) == NULL ) {
            string systemErrMsg = WindowsCommon::GetErrorMessage ( GetLastError() );
            throw ProbeException ( baseErrMsg + " because the registry key does not exist. " + systemErrMsg );
        }

        // Get the sid for the trustee.
        pSid = WindowsCommon::GetSIDForTrusteeSID ( trusteeSIDStr );
        // The registry key exists and trustee SID seems good so we can create the new item now.
        item = this->CreateItem();
        item->SetStatus ( OvalEnum::STATUS_EXISTS );
        item->AppendElement ( new ItemEntity ( "hive", hiveStr, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "key", keyStr, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "trustee_sid", trusteeSIDStr, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS ) );
        // Build structure to hold the successful audited rights.
        pSuccessfulAuditedRights = reinterpret_cast<PACCESS_MASK> ( ::LocalAlloc ( LPTR, sizeof ( PACCESS_MASK ) + sizeof ( ACCESS_MASK ) ) );

        if ( pSuccessfulAuditedRights == NULL ) {
            if ( pSid != NULL ) {
                LocalFree ( pSid );
                pSid = NULL;
            }

            throw ProbeException ( baseErrMsg + " Out of memory! Unable to allocate memory for successful audited rights." );
        }

        // Build structure to hold the failed audit rights.
        pFailedAuditRights = reinterpret_cast<PACCESS_MASK> ( ::LocalAlloc ( LPTR, sizeof ( PACCESS_MASK ) + sizeof ( ACCESS_MASK ) ) );

        if ( pFailedAuditRights == NULL ) {
            if ( pSid != NULL ) {
                LocalFree ( pSid );
                pSid = NULL;
            }

            if ( pSuccessfulAuditedRights != NULL ) {
                LocalFree ( pSuccessfulAuditedRights );
                pSuccessfulAuditedRights = NULL;
            }

            throw ProbeException ( baseErrMsg + " Out of memory! Unable to allocate memory for failed audit rights." );
        }

        // Get the audited rights.
        Log::Debug ( "Getting audited permissions masks for registry key: " + hiveStr + " key: " + keyStr + " trustee_sid: " + trusteeSIDStr );
        WindowsCommon::GetAuditedPermissionsForWindowsObject ( SE_REGISTRY_KEY, pSid, &registryKey, pSuccessfulAuditedRights, pFailedAuditRights );
        item->AppendElement ( new ItemEntity ( "standard_delete", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedRights ) & DELETE ), ( ( *pFailedAuditRights ) & DELETE ) ), OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "standard_read_control", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedRights ) & READ_CONTROL ), ( ( *pFailedAuditRights ) & READ_CONTROL ) ), OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "standard_write_dac", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedRights ) & WRITE_DAC ), ( ( *pFailedAuditRights ) & WRITE_DAC ) ), OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "standard_write_owner", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedRights ) & WRITE_OWNER ), ( ( *pFailedAuditRights ) & WRITE_OWNER ) ), OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "standard_synchronize", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedRights ) & SYNCHRONIZE ), ( ( *pFailedAuditRights ) & SYNCHRONIZE ) ), OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "access_system_security", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedRights ) & ACCESS_SYSTEM_SECURITY ), ( ( *pFailedAuditRights ) & ACCESS_SYSTEM_SECURITY ) ), OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "generic_read", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedRights ) & GENERIC_READ ), ( ( *pFailedAuditRights ) & GENERIC_READ ) ), OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "generic_write", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedRights ) & GENERIC_WRITE ), ( ( *pFailedAuditRights ) & GENERIC_WRITE ) ), OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "generic_execute", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedRights ) & GENERIC_EXECUTE ), ( ( *pFailedAuditRights ) & GENERIC_EXECUTE ) ), OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "generic_all", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedRights ) & GENERIC_ALL ), ( ( *pFailedAuditRights ) & GENERIC_ALL ) ), OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "key_query_value", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedRights ) & KEY_QUERY_VALUE ), ( ( *pFailedAuditRights ) & FILE_READ_DATA ) ), OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "key_set_value", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedRights ) & KEY_SET_VALUE ), ( ( *pFailedAuditRights ) & KEY_SET_VALUE ) ), OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "key_create_sub_key", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedRights ) & KEY_CREATE_SUB_KEY ), ( ( *pFailedAuditRights ) & KEY_CREATE_SUB_KEY ) ), OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "key_enumerate_sub_keys", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedRights ) & KEY_ENUMERATE_SUB_KEYS ), ( ( *pFailedAuditRights ) & KEY_ENUMERATE_SUB_KEYS ) ), OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "key_notify", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedRights ) & KEY_NOTIFY ), ( ( *pFailedAuditRights ) & KEY_NOTIFY ) ), OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "key_create_link", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedRights ) & KEY_CREATE_LINK ), ( ( *pFailedAuditRights ) & KEY_CREATE_LINK ) ), OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "key_wow64_64key", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedRights ) & KEY_WOW64_64KEY ), ( ( *pFailedAuditRights ) & KEY_WOW64_64KEY ) ), OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "key_wow64_32key", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedRights ) & KEY_WOW64_32KEY ), ( ( *pFailedAuditRights ) & KEY_WOW64_32KEY ) ), OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "key_wow64_res", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedRights ) & KEY_WOW64_RES ), ( ( *pFailedAuditRights ) & KEY_WOW64_RES ) ), OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS ) );

    } catch ( Exception ex ) {
        if ( item != NULL ) {
            item->SetStatus ( OvalEnum::STATUS_ERROR );
            item->AppendMessage ( new OvalMessage ( ex.GetErrorMessage(), OvalEnum::LEVEL_ERROR ) );

        } else {
            if ( pSuccessfulAuditedRights != NULL ) {
                LocalFree ( pSuccessfulAuditedRights );
                pSuccessfulAuditedRights = NULL;
            }

            if ( pFailedAuditRights != NULL ) {
                LocalFree ( pFailedAuditRights );
                pFailedAuditRights = NULL;
            }

            if ( pSid != NULL ) {
                LocalFree ( pSid );
                pSid = NULL;
            }

            throw ex;
        }
    }

    if ( pSuccessfulAuditedRights != NULL ) {
        LocalFree ( pSuccessfulAuditedRights );
        pSuccessfulAuditedRights = NULL;
    }

    if ( pFailedAuditRights != NULL ) {
        LocalFree ( pFailedAuditRights );
        pFailedAuditRights = NULL;
    }

    if ( pSid != NULL ) {
        LocalFree ( pSid );
        pSid = NULL;
    }

    return item;
}

string RegKeyAuditedPermissions53Probe::ConvertPermissionsToStringValue ( ACCESS_MASK success , ACCESS_MASK failure ) {
    if ( success && failure ) return "AUDIT_SUCCESS_FAILURE";

    else if ( success && !failure ) return "AUDIT_SUCCESS";

    else if ( !success && failure ) return "AUDIT_FAILURE";

    else if ( !success && !failure ) return "AUDIT_NONE";

    else return "";
}