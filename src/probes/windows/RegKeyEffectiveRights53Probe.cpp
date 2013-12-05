//
//
//****************************************************************************************//
// Copyright (c) 2002-2013, The MITRE Corporation
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

#include <AutoCloser.h>
#include "RegKeyEffectiveRights53Probe.h"


//****************************************************************************************//
//                              RegKeyEffectiveRights53Probe Class                            //
//****************************************************************************************//
RegKeyEffectiveRights53Probe* RegKeyEffectiveRights53Probe::instance = NULL;

RegKeyEffectiveRights53Probe::RegKeyEffectiveRights53Probe() {
}

RegKeyEffectiveRights53Probe::~RegKeyEffectiveRights53Probe() {
    instance = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* RegKeyEffectiveRights53Probe::Instance() {
    // Use lazy initialization
    if ( instance == NULL )
        instance = new RegKeyEffectiveRights53Probe();

    return instance;
}

ItemVector* RegKeyEffectiveRights53Probe::CollectItems ( Object* object ) {
    // -----------------------------------------------------------------------
    //
    //  ABSTRACT
    //
    //  Get all the registry keys on the system that match the pattern reusing the RegistryFinder.
    //  This probe operates a bit differently than the others. After locating matching
    //  registry keys as the other registry key related probes do the trustee SID is handled as follows:
    //
    //  if not using Variables
    //   - if operation == equals
    //      - call GetEffectiveRights method
    //
    //   - operation == not equal || operation == pattern match
    //      - Get all trustee names on the system. Reuse the logic in SidProbe::GetAllTrusteeSIDs()
    //      - Get the set of matching trustee SIDs
    //
    //  - if using variables
    //      - Get all trustee names on the system. Reuse the logic in SidProbe::GetAllTrusteeSIDs()
    //      - Get the set of matching trustee SIDs
    //      - call GetEffectiveRights method
    //
    // -----------------------------------------------------------------------
    // Get the hive, key, and trustee
    ObjectEntity* hiveEntity = object->GetElementByName ( "hive" );
    ObjectEntity* keyEntity = object->GetElementByName ( "key" );
    ObjectEntity* trusteeSIDEntity = object->GetElementByName ( "trustee_sid" );

    // Check trustee datatypes - only allow string
    if ( trusteeSIDEntity->GetDatatype() != OvalEnum::DATATYPE_STRING ) {
        throw ProbeException ( "Error: invalid data type specified on trustee_sid. Found: " + OvalEnum::DatatypeToString ( trusteeSIDEntity->GetDatatype() ) );
    }

    // Check trustee operation - only allow  equals, not equals and pattern match
    if ( trusteeSIDEntity->GetOperation() != OvalEnum::OPERATION_EQUALS
            && trusteeSIDEntity->GetOperation() != OvalEnum::OPERATION_PATTERN_MATCH
            && trusteeSIDEntity->GetOperation() != OvalEnum::OPERATION_NOT_EQUAL ) {
        throw ProbeException ( "Error: invalid operation specified on trustee_sid. Found: " + OvalEnum::OperationToString ( trusteeSIDEntity->GetOperation() ) );
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
				Log::Info("Deprecated behavior found when collecting " + object->GetId() + " Found behavior: " + behavior->GetName() + " = " + behavior->GetValue());
            } else if ( behavior->GetName().compare ( "resolve_group" ) == 0 ) {
                if ( behavior->GetValue().compare ( "true" ) == 0 ) {
                    resolveGroupBehavior = true;
                }

                Log::Info ( "Deprecated behavior found when collecting " + object->GetId() + " Found behavior: " + behavior->GetName() + " = " + behavior->GetValue() );

            } else if ( behavior->GetName() == "max_depth" || behavior->GetName() == "recurse_direction" || behavior->GetName() == "windows_view" ) {
                // Skip these as they are supported in the RegistryFinder class.
            } else {
                Log::Info ( "Unsupported behavior found when collecting " + object->GetId() + " Found behavior: " + behavior->GetName() + " = " + behavior->GetValue() );
            }
        }
    }

    ItemVector *collectedItems = new ItemVector();
    RegistryFinder registryFinder(WindowsCommon::behavior2view(object->GetBehaviors()));
    // Create a name ObjectEntity to provide to the RegistryFinder as it expects one, however, have the ObjectEntity set to NIL as its value does not matter.
    // Only the hive and key are relevant for this probe.
    ObjectEntity* nameEntity = new ObjectEntity();
    nameEntity->SetNil ( true );
    RegKeyVector* registryKeys = registryFinder.SearchRegistries ( hiveEntity, keyEntity, nameEntity, object->GetBehaviors() );

    if ( registryKeys->size() > 0 ) {
        RegKeyVector::iterator iterator;

        for ( iterator = registryKeys->begin(); iterator != registryKeys->end(); iterator++ ) {
            RegKey* registryKey = ( *iterator );

            try {
				HKEY keyHandle = NULL;
				DWORD err;

				if ((err = registryFinder.GetHKeyHandle(&keyHandle,
					registryKey->GetHive(), registryKey->GetKey())) != ERROR_SUCCESS) {
					if (keyHandle != NULL) // maybe this is paranoia...
						RegCloseKey(keyHandle);
					throw ProbeException("Error: unable to open registry key: " +
						registryKey->ToString() + ": " +
						WindowsCommon::GetErrorMessage(err));
				}

				AutoCloser<HKEY, LONG(WINAPI&)(HKEY)> keyGuard(keyHandle, RegCloseKey,
					"reg key " + registryKey->ToString());

				StringSet trusteeSIDs = this->GetTrusteesForWindowsObject(
					SE_REGISTRY_KEY, keyHandle, 
					trusteeSIDEntity, true, resolveGroupBehavior, 
					includeGroupBehavior);

                if ( !trusteeSIDs.empty() ) {
                    StringSet::iterator iterator;

                    for ( iterator = trusteeSIDs.begin(); iterator != trusteeSIDs.end(); iterator++ ) {
                        try {
                            Item* item = this->GetEffectiveRights ( keyHandle, registryKey, ( *iterator ));

                            if ( item != NULL ) {
								if (keyEntity->GetNil()) {
									ItemEntityVector* keyVector = item->GetElementsByName("key");
									if (keyVector->size() > 0) {
										keyVector->at(0)->SetNil(true);
										keyVector->at(0)->SetStatus(OvalEnum::STATUS_NOT_COLLECTED);
									}
								}

								item->AppendElement(new ItemEntity("windows_view",
									(registryFinder.GetView() == BIT_32 ? "32_bit" : "64_bit")));

								collectedItems->push_back ( item );
                            }

                        } catch ( ProbeException ex ) {
                            Log::Message ( "ProbeException caught when collecting: " + object->GetId() + " " +  ex.GetErrorMessage() );

                        } catch ( Exception ex ) {
                            Log::Message ( "Exception caught when collecting: " + object->GetId() + " " +  ex.GetErrorMessage() );
                        }
                    }
                } else {
                    Log::Debug ( "No matching trustees found when getting effective rights for object: " + object->GetId() );

                    if ( this->ReportTrusteeDoesNotExist ( trusteeSIDEntity, true ) ) {
                        Item* item = this->CreateItem();
                        item->SetStatus ( OvalEnum::STATUS_DOES_NOT_EXIST );
                        item->AppendElement ( new ItemEntity ( "hive", registryKey->GetHive(), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
						if (keyEntity->GetNil()){
							item->AppendElement(new ItemEntity("key", registryKey->GetKey(), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_NOT_COLLECTED, true)); //GetNil is true
						}
						else
						{
							item->AppendElement(new ItemEntity("key", registryKey->GetKey(), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS, false)); //GetNil is false
						}
                        item->AppendElement ( new ItemEntity ( "trustee_sid","", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST ) );
						item->AppendElement(new ItemEntity("windows_view",
							(registryFinder.GetView() == BIT_32 ? "32_bit" : "64_bit")));
                        collectedItems->push_back ( item );
                    }
                }

            } catch ( ProbeException ex ) {
                Log::Message ( "ProbeException caught when collecting: " + object->GetId() + " " +  ex.GetErrorMessage() );

            } catch ( Exception ex ) {
                Log::Message ( "Exception caught when collecting: " + object->GetId() + " " +  ex.GetErrorMessage() );

            } catch ( ... ) {
                Log::Message ( "Unknown error when collecting " + object->GetId() );
            }

            delete registryKey;
        }

    } else {
        if ( registryFinder.ReportHiveDoesNotExist ( hiveEntity ) ) {
            Item* item = NULL;
            item = this->CreateItem();
            item->SetStatus ( OvalEnum::STATUS_DOES_NOT_EXIST );
            item->AppendElement ( new ItemEntity ( "hive", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST ) );
			item->AppendElement(new ItemEntity("windows_view",
				(registryFinder.GetView() == BIT_32 ? "32_bit" : "64_bit")));
            collectedItems->push_back ( item );

        } else {
            Item* item = NULL;
            StringSet* hives = registryFinder.GetHives ( hiveEntity );

            for ( StringSet::iterator iterator1 = hives->begin(); iterator1 != hives->end() ; iterator1++ ) {
                if ( registryFinder.ReportKeyDoesNotExist ( *iterator1, keyEntity ) ) {
                    item = this->CreateItem();
                    item->SetStatus ( OvalEnum::STATUS_DOES_NOT_EXIST );
                    item->AppendElement ( new ItemEntity ( "hive", ( *iterator1 ), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
                    item->AppendElement ( new ItemEntity ( "key", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST, keyEntity->GetNil() ) );
					item->AppendElement(new ItemEntity("windows_view",
						(registryFinder.GetView() == BIT_32 ? "32_bit" : "64_bit")));
                    collectedItems->push_back ( item );
                }
            }

            hives->clear();
            delete hives;
        }
    }

    registryKeys->clear();
    delete registryKeys;
    return collectedItems;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
Item* RegKeyEffectiveRights53Probe::CreateItem() {
    Item* item = new Item ( 0,
                            "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows",
                            "win-sc",
                            "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows windows-system-characteristics-schema.xsd",
                            OvalEnum::STATUS_ERROR,
                            "regkeyeffectiverights_item" );
    return item;
}

Item* RegKeyEffectiveRights53Probe::GetEffectiveRights ( HKEY keyHandle, const RegKey *regKey, string trusteeSIDStr ) {
    Item* item = NULL;
    PSID pSid = NULL;
    PACCESS_MASK pAccessRights = NULL;
    string baseErrMsg = "Error unable to get effective rights for trustee: " +
		trusteeSIDStr + " from dacl for registry key: " + regKey->ToString();

    try {
        // Get the sid for the trustee name
        pSid = WindowsCommon::GetSIDForTrusteeSID ( trusteeSIDStr );
        // The registry key exists and trustee name seems good so we can create the new item now.
        item = this->CreateItem();
        item->SetStatus ( OvalEnum::STATUS_EXISTS );
        item->AppendElement ( new ItemEntity ( "hive", regKey->GetHive(), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "key", regKey->GetKey(), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "trustee_sid", trusteeSIDStr, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
        // Build structure to hold the rights
        pAccessRights = reinterpret_cast<PACCESS_MASK> ( ::LocalAlloc ( LPTR, sizeof ( PACCESS_MASK ) + sizeof ( ACCESS_MASK ) ) );

        if ( pAccessRights == NULL ) {
            throw ProbeException ( baseErrMsg + " Out of memory! Unable to allocate memory for access rights." );
        }

        // Get the rights
        Log::Debug ( "Getting rights mask for registry key: " + regKey->ToString() + " trustee_sid: " + trusteeSIDStr );
        WindowsCommon::GetEffectiveRightsForWindowsObject ( SE_REGISTRY_KEY, pSid, keyHandle, pAccessRights );

        if ( ( *pAccessRights ) & DELETE )
            item->AppendElement ( new ItemEntity ( "standard_delete", "1", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        else
            item->AppendElement ( new ItemEntity ( "standard_delete", "0", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        if ( ( *pAccessRights ) & READ_CONTROL )
            item->AppendElement ( new ItemEntity ( "standard_read_control", "1", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        else
            item->AppendElement ( new ItemEntity ( "standard_read_control", "0", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        if ( ( *pAccessRights ) & WRITE_DAC )
            item->AppendElement ( new ItemEntity ( "standard_write_dac", "1", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        else
            item->AppendElement ( new ItemEntity ( "standard_write_dac", "0", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        if ( ( *pAccessRights ) & WRITE_OWNER )
            item->AppendElement ( new ItemEntity ( "standard_write_owner", "1", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        else
            item->AppendElement ( new ItemEntity ( "standard_write_owner", "0", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        if ( ( *pAccessRights ) & SYNCHRONIZE )
            item->AppendElement ( new ItemEntity ( "standard_synchronize", "1", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        else
            item->AppendElement ( new ItemEntity ( "standard_synchronize", "0", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        if ( ( *pAccessRights ) & ACCESS_SYSTEM_SECURITY )
            item->AppendElement ( new ItemEntity ( "access_system_security", "1", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        else
            item->AppendElement ( new ItemEntity ( "access_system_security", "0", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        if ( ( *pAccessRights ) & GENERIC_READ )
            item->AppendElement ( new ItemEntity ( "generic_read", "1", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        else
            item->AppendElement ( new ItemEntity ( "generic_read", "0", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        if ( ( *pAccessRights ) & GENERIC_WRITE )
            item->AppendElement ( new ItemEntity ( "generic_write", "1", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        else
            item->AppendElement ( new ItemEntity ( "generic_write", "0", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        if ( ( *pAccessRights ) & GENERIC_EXECUTE )
            item->AppendElement ( new ItemEntity ( "generic_execute", "1", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        else
            item->AppendElement ( new ItemEntity ( "generic_execute", "0", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        if ( ( *pAccessRights ) & GENERIC_ALL )
            item->AppendElement ( new ItemEntity ( "generic_all", "1", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        else
            item->AppendElement ( new ItemEntity ( "generic_all", "0", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        if ( ( *pAccessRights ) & KEY_QUERY_VALUE )
            item->AppendElement ( new ItemEntity ( "key_query_value", "1", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        else
            item->AppendElement ( new ItemEntity ( "key_query_value", "0", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        if ( ( *pAccessRights ) & KEY_SET_VALUE )
            item->AppendElement ( new ItemEntity ( "key_set_value", "1", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        else
            item->AppendElement ( new ItemEntity ( "key_set_value", "0", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        if ( ( *pAccessRights ) & KEY_CREATE_SUB_KEY )
            item->AppendElement ( new ItemEntity ( "key_create_sub_key", "1", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        else
            item->AppendElement ( new ItemEntity ( "key_create_sub_key", "0", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        if ( ( *pAccessRights ) & KEY_ENUMERATE_SUB_KEYS )
            item->AppendElement ( new ItemEntity ( "key_enumerate_sub_keys", "1", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        else
            item->AppendElement ( new ItemEntity ( "key_enumerate_sub_keys", "0", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        if ( ( *pAccessRights ) & KEY_NOTIFY )
            item->AppendElement ( new ItemEntity ( "key_notify", "1", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        else
            item->AppendElement ( new ItemEntity ( "key_notify", "0", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        if ( ( *pAccessRights ) & KEY_CREATE_LINK )
            item->AppendElement ( new ItemEntity ( "key_create_link", "1", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        else
            item->AppendElement ( new ItemEntity ( "key_create_link", "0", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        if ( ( *pAccessRights ) & KEY_WOW64_64KEY )
            item->AppendElement ( new ItemEntity ( "key_wow64_64key", "1", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        else
            item->AppendElement ( new ItemEntity ( "key_wow64_64key", "0", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        if ( ( *pAccessRights ) & KEY_WOW64_32KEY )
            item->AppendElement ( new ItemEntity ( "key_wow64_32key", "1", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        else
            item->AppendElement ( new ItemEntity ( "key_wow64_32key", "0", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        if ( ( *pAccessRights ) & KEY_WOW64_RES )
            item->AppendElement ( new ItemEntity ( "key_wow64_res", "1", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        else
            item->AppendElement ( new ItemEntity ( "key_wow64_res", "0", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

    } catch ( Exception ex ) {
        if ( item != NULL ) {
            item->SetStatus ( OvalEnum::STATUS_ERROR );
            item->AppendMessage ( new OvalMessage ( ex.GetErrorMessage(), OvalEnum::LEVEL_ERROR ) );

        } else {
            if ( pAccessRights != NULL ) {
                LocalFree ( pAccessRights );
                pAccessRights = NULL;
            }

            if ( pSid != NULL ) {
                LocalFree ( pSid );
                pSid = NULL;
            }

            throw ex;
        }
    }

    if ( pAccessRights != NULL ) {
        LocalFree ( pAccessRights );
        pAccessRights = NULL;
    }

    if ( pSid != NULL ) {
        LocalFree ( pSid );
        pSid = NULL;
    }

    return item;
}