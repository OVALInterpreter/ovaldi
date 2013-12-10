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
#include "ServiceEffectiveRightsProbe.h"

//****************************************************************************************//
//                              ServiceEffectiveRightsProbe Class                         //
//****************************************************************************************//
ServiceEffectiveRightsProbe* ServiceEffectiveRightsProbe::instance = NULL;

ServiceEffectiveRightsProbe::ServiceEffectiveRightsProbe() {
    services = NULL;

    SC_HANDLE serviceHandle = NULL;
    serviceHandle = OpenSCManager ( NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_ENUMERATE_SERVICE );
    if ( serviceHandle == NULL ) {
        throw ProbeException ( "Error: The function OpenSCManager() was unable to retrieve the handle for the service control manager database. Microsoft System Error " + Common::ToString ( GetLastError() ) + ") - " + WindowsCommon::GetErrorMessage ( GetLastError() ) );
    }

	this->serviceMgr.reset(
		new AutoCloser<SC_HANDLE, BOOL(WINAPI&)(SC_HANDLE)>(
			serviceHandle, CloseServiceHandle, "service control manager"));
}

ServiceEffectiveRightsProbe::~ServiceEffectiveRightsProbe() {
    instance = NULL;
	if (services)
		delete services;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* ServiceEffectiveRightsProbe::Instance() {
    // Use lazy initialization
    if ( instance == NULL )
        instance = new ServiceEffectiveRightsProbe();

    return instance;
}

ItemVector* ServiceEffectiveRightsProbe::CollectItems ( Object* object ) {
    ObjectEntity* serviceNameEntity = object->GetElementByName ( "service_name" );
    ObjectEntity* trusteeSIDEntity = object->GetElementByName ( "trustee_sid" );

    if ( serviceNameEntity->GetDatatype() != OvalEnum::DATATYPE_STRING ) {
        throw ProbeException ( "Error: invalid data type specified on service_name. Found: " + OvalEnum::DatatypeToString ( serviceNameEntity->GetDatatype() ) );
    }

    if ( serviceNameEntity->GetOperation() != OvalEnum::OPERATION_EQUALS
            && serviceNameEntity->GetOperation() != OvalEnum::OPERATION_PATTERN_MATCH
            && serviceNameEntity->GetOperation() != OvalEnum::OPERATION_NOT_EQUAL ) {
        throw ProbeException ( "Error: invalid operation specified on service_name. Found: " + OvalEnum::OperationToString ( serviceNameEntity->GetOperation() ) );
    }

    if ( trusteeSIDEntity->GetDatatype() != OvalEnum::DATATYPE_STRING ) {
        throw ProbeException ( "Error: invalid data type specified on trustee_sid. Found: " + OvalEnum::DatatypeToString ( trusteeSIDEntity->GetDatatype() ) );
    }

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
				Log::Info("Deprecated behavior found when collecting " + object->GetId() + " Found behavior: " + behavior->GetName() + " = " + behavior->GetValue());
            } else {
                Log::Info ( "Unsupported behavior found when collecting " + object->GetId() + " Found behavior: " + behavior->GetName() + " = " + behavior->GetValue() );
            }
        }
    }

    ItemVector *collectedItems = new ItemVector();

    if ( services == NULL ) {
        services = ServiceEffectiveRightsProbe::GetAllServices();
    }

    StringSet* allServices = ServiceEffectiveRightsProbe::GetServices ( serviceNameEntity );

    for ( StringSet::iterator iterator1 = allServices->begin(); iterator1 != allServices->end(); iterator1++ ) {
        try {

			SC_HANDLE serviceHandle = OpenService(serviceMgr->get(), iterator1->c_str(), GENERIC_READ);
			if (serviceHandle == NULL) {
				Log::Message("Error opening service '" + *iterator1 +
					"': " + WindowsCommon::GetErrorMessage(GetLastError()));
				continue;
			}
			AutoCloser<SC_HANDLE, BOOL(WINAPI&)(SC_HANDLE)> serviceGuard(serviceHandle,
				CloseServiceHandle, "service " + *iterator1);
            StringSet trusteeSIDs = this->GetTrusteesForWindowsObject ( 
				SE_SERVICE, serviceHandle, trusteeSIDEntity, true, 
				resolveGroupBehavior, includeGroupBehavior );

            if ( !trusteeSIDs.empty() ) {
                for ( StringSet::iterator iterator2 = trusteeSIDs.begin(); iterator2 != trusteeSIDs.end(); iterator2++ ) {
                    try {
                        Item* item = this->GetEffectiveRights ( serviceHandle, ( *iterator1 ) , ( *iterator2 ) );

                        if ( item != NULL ) {
                            collectedItems->push_back ( item );
                        }

                    } catch ( ProbeException ex ) {
                        Log::Message ( "ProbeException caught when collecting: " + object->GetId() + " " +  ex.GetErrorMessage() );

                    } catch ( Exception ex ) {
                        Log::Message ( "Exception caught when collecting: " + object->GetId() + " " +  ex.GetErrorMessage() );
                    }
                }
            } else {
                Log::Debug ( "No matching SIDs found when getting the effective rights for object: " + object->GetId() );

                if ( this->ReportTrusteeDoesNotExist ( trusteeSIDEntity, true ) ) {
                    for ( StringSet::iterator iterator2 = trusteeSIDs.begin(); iterator2 != trusteeSIDs.end(); iterator2++ ) {
                        Item* item = this->CreateItem();
                        item->SetStatus ( OvalEnum::STATUS_DOES_NOT_EXIST );
                        item->AppendElement ( new ItemEntity ( "service_name", ( *iterator1 ), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
                        item->AppendElement ( new ItemEntity ( "trustee_sid","", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST ) );
                        collectedItems->push_back ( item );
                    }
                }
            }

        } catch ( ProbeException ex ) {
            Log::Message ( "ProbeException caught when collecting: " + object->GetId() + " " +  ex.GetErrorMessage() );

        } catch ( Exception ex ) {
            Log::Message ( "Exception caught when collecting: " + object->GetId() + " " +  ex.GetErrorMessage() );

        } catch ( ... ) {
            Log::Message ( "Unknown error when collecting " + object->GetId() );
        }
    }

    allServices->clear();
    delete allServices;
    return collectedItems;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
Item* ServiceEffectiveRightsProbe::CreateItem() {
    Item* item = new Item ( 0,
                            "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows",
                            "win-sc",
                            "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows windows-system-characteristics-schema.xsd",
                            OvalEnum::STATUS_ERROR,
                            "serviceeffectiverights_item" );
    return item;
}

Item* ServiceEffectiveRightsProbe::GetEffectiveRights ( SC_HANDLE serviceHandle, string serviceNameStr, string trusteeSIDStr ) {
    Log::Debug ( "Collecting effective rights for service_name: " + serviceNameStr + " trustee_sid: " + trusteeSIDStr );
    Item* item = NULL;
    PSID pSid = NULL;
    PACCESS_MASK pAccessRights = NULL;
    string baseErrMsg = "Error unable to get effective rights for: " + serviceNameStr + " trustee_sid: " + trusteeSIDStr;

    try {
        // Get the sid for the trustee name
        pSid = WindowsCommon::GetSIDForTrusteeSID ( trusteeSIDStr );
        // The service exists and the trustee_sid looks valid so we can create the new item now.
        Log::Debug ( "Creating an item to hold the service effective rights for service_name: " + serviceNameStr + " and trustee_sid: " + trusteeSIDStr );
        item = this->CreateItem();
        item->SetStatus ( OvalEnum::STATUS_EXISTS );
        item->AppendElement ( new ItemEntity ( "service_name", serviceNameStr, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "trustee_sid", trusteeSIDStr, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
        // Build structure to hold the rights
        pAccessRights = reinterpret_cast<PACCESS_MASK> ( ::LocalAlloc ( LPTR, sizeof ( PACCESS_MASK ) + sizeof ( ACCESS_MASK ) ) );

        if ( pAccessRights == NULL ) {
            throw ProbeException ( baseErrMsg + " Out of memory! Unable to allocate memory for access rights." );
        }

        // Get the rights
        Log::Debug ( "Getting rights mask for service_name: " + serviceNameStr + " and trustee_sid: " + trusteeSIDStr );
        WindowsCommon::GetEffectiveRightsForWindowsObject ( SE_SERVICE, pSid, serviceHandle, pAccessRights );

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

        if ( ( ( *pAccessRights ) & ( STANDARD_RIGHTS_READ | SERVICE_QUERY_CONFIG | SERVICE_QUERY_STATUS | SERVICE_INTERROGATE | SERVICE_ENUMERATE_DEPENDENTS ) ) == ( STANDARD_RIGHTS_READ | SERVICE_QUERY_CONFIG | SERVICE_QUERY_STATUS | SERVICE_INTERROGATE | SERVICE_ENUMERATE_DEPENDENTS ) )
            item->AppendElement ( new ItemEntity ( "generic_read", "1", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );
        else
            item->AppendElement ( new ItemEntity ( "generic_read", "0", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        if ( ( ( *pAccessRights ) & ( STANDARD_RIGHTS_WRITE | SERVICE_CHANGE_CONFIG ) ) == ( STANDARD_RIGHTS_WRITE | SERVICE_CHANGE_CONFIG ) )
            item->AppendElement ( new ItemEntity ( "generic_write", "1", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );
        else
            item->AppendElement ( new ItemEntity ( "generic_write", "0", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        if ( ( ( *pAccessRights ) & ( STANDARD_RIGHTS_EXECUTE | SERVICE_START | SERVICE_STOP | SERVICE_PAUSE_CONTINUE | SERVICE_USER_DEFINED_CONTROL ) ) == ( STANDARD_RIGHTS_EXECUTE | SERVICE_START | SERVICE_STOP | SERVICE_PAUSE_CONTINUE | SERVICE_USER_DEFINED_CONTROL ) )
            item->AppendElement ( new ItemEntity ( "generic_execute", "1", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );
        else
            item->AppendElement ( new ItemEntity ( "generic_execute", "0", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        if ( ( *pAccessRights ) & SERVICE_QUERY_CONFIG )
            item->AppendElement ( new ItemEntity ( "service_query_conf", "1", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );
        else
            item->AppendElement ( new ItemEntity ( "service_query_conf", "0", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        if ( ( *pAccessRights ) & SERVICE_CHANGE_CONFIG )
            item->AppendElement ( new ItemEntity ( "service_change_conf", "1", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );
        else
            item->AppendElement ( new ItemEntity ( "service_change_conf", "0", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        if ( ( *pAccessRights ) & SERVICE_QUERY_STATUS )
            item->AppendElement ( new ItemEntity ( "service_query_stat", "1", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );
        else
            item->AppendElement ( new ItemEntity ( "service_query_stat", "0", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        if ( ( *pAccessRights ) & SERVICE_ENUMERATE_DEPENDENTS )
            item->AppendElement ( new ItemEntity ( "service_enum_dependents", "1", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );
        else
            item->AppendElement ( new ItemEntity ( "service_enum_dependents", "0", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        if ( ( *pAccessRights ) & SERVICE_START )
            item->AppendElement ( new ItemEntity ( "service_start", "1", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );
        else
            item->AppendElement ( new ItemEntity ( "service_start", "0", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        if ( ( *pAccessRights ) & SERVICE_STOP )
            item->AppendElement ( new ItemEntity ( "service_stop", "1", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );
        else
            item->AppendElement ( new ItemEntity ( "service_stop", "0", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        if ( ( *pAccessRights ) & SERVICE_PAUSE_CONTINUE )
            item->AppendElement ( new ItemEntity ( "service_pause", "1", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );
        else
            item->AppendElement ( new ItemEntity ( "service_pause", "0", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        if ( ( *pAccessRights ) & SERVICE_INTERROGATE )
            item->AppendElement ( new ItemEntity ( "service_interrogate", "1", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );
        else
            item->AppendElement ( new ItemEntity ( "service_interrogate", "0", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        if ( ( *pAccessRights ) & SERVICE_USER_DEFINED_CONTROL )
            item->AppendElement ( new ItemEntity ( "service_user_defined", "1", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );
        else
            item->AppendElement ( new ItemEntity ( "service_user_defined", "0", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

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

StringSet* ServiceEffectiveRightsProbe::GetServices ( ObjectEntity* serviceNameEntity ) {
    StringSet* services = NULL;

    // Does this ObjectEntity use variables?
    if ( serviceNameEntity->GetVarRef() == NULL ) {
        // Proceed based on operation
        if ( serviceNameEntity->GetOperation() == OvalEnum::OPERATION_EQUALS ) {
            services = new StringSet();

            // If the printer exists add it to the list
            if ( this->ServiceExists ( serviceNameEntity->GetValue() ) ) {
                services->insert ( serviceNameEntity->GetValue() );
            }

        } else if ( serviceNameEntity->GetOperation() == OvalEnum::OPERATION_NOT_EQUAL ) {
            services = this->GetMatchingServices ( serviceNameEntity->GetValue() , false );

        } else if ( serviceNameEntity->GetOperation() == OvalEnum::OPERATION_PATTERN_MATCH ) {
            services = this->GetMatchingServices ( serviceNameEntity->GetValue() , true );
        }

    } else {
        services = new StringSet();
        // Get all services
        StringSet* allServices = new StringSet();

        if ( serviceNameEntity->GetOperation() == OvalEnum::OPERATION_EQUALS ) {
            // In the case of equals simply loop through all the
            // variable values and add them to the set of all services
            // if they exist on the system
			VariableValueVector vals = serviceNameEntity->GetVarRef()->GetValues();
            VariableValueVector::iterator iterator;

            for ( iterator = vals.begin() ; iterator != vals.end() ; iterator++ ) {
                if ( this->ServiceExists ( iterator->GetValue() ) ) {
                    allServices->insert ( iterator->GetValue() );
                }
            }

        } else {
            allServices = this->GetMatchingServices ( ".*" , true );
        }

        // Loop through all services on the system
        // only keep services that match operation and value and var check
        ItemEntity* tmp = this->CreateItemEntity ( serviceNameEntity );
        StringSet::iterator it;

        for ( it = allServices->begin() ; it != allServices->end() ; it++ ) {
            tmp->SetValue ( ( *it ) );

            if ( serviceNameEntity->Analyze ( tmp ) == OvalEnum::RESULT_TRUE ) {
                services->insert ( ( *it ) );
            }
        }
    }

    return services;
}

StringSet* ServiceEffectiveRightsProbe::GetMatchingServices ( string patternStr , bool isRegex ) {
    StringSet* matchingServices = new StringSet();

    for ( StringSet::iterator iterator = ServiceEffectiveRightsProbe::services->begin() ; iterator != ServiceEffectiveRightsProbe::services->end() ; iterator++ ) {
        if ( this->IsMatch ( patternStr , *iterator , isRegex ) ) {
            matchingServices->insert ( *iterator );
        }
    }

    return matchingServices;
}

bool ServiceEffectiveRightsProbe::ServiceExists ( string serviceNameStr ) {
    if ( ServiceEffectiveRightsProbe::services->find ( serviceNameStr ) != ServiceEffectiveRightsProbe::services->end() ) {
        return true;
    }

    return false;
}

StringSet* ServiceEffectiveRightsProbe::GetAllServices() {
    StringSet* allServices = new StringSet();
    ENUM_SERVICE_STATUS_PROCESS* serviceDataBuffer = NULL;
    DWORD serviceBufferSize = 0;
    DWORD serviceDataSize = 0;
    DWORD numberOfServices = 0;

    if ( !EnumServicesStatusEx ( serviceMgr->get(), SC_ENUM_PROCESS_INFO, SERVICE_WIN32, SERVICE_STATE_ALL, NULL, serviceBufferSize, &serviceDataSize, &numberOfServices, 0, NULL ) ) {
        if ( serviceDataBuffer != NULL ) {
            free ( serviceDataBuffer );
            serviceDataBuffer = NULL;
        }

        serviceDataBuffer = ( ENUM_SERVICE_STATUS_PROCESS* ) malloc ( serviceDataSize );
        serviceBufferSize = serviceDataSize;
        serviceDataSize = 0;
        numberOfServices = 0;

        if ( !EnumServicesStatusEx ( serviceMgr->get(), SC_ENUM_PROCESS_INFO, SERVICE_WIN32, SERVICE_STATE_ALL, ( LPBYTE ) serviceDataBuffer, serviceBufferSize, &serviceDataSize, &numberOfServices, 0, NULL ) ) {
            throw ProbeException ( "ERROR: The function EnumServicesStatusEx() could not enumerate the services on the system. Microsoft System Error " + Common::ToString ( GetLastError() ) + ") - " + WindowsCommon::GetErrorMessage ( GetLastError() ) );
        }
    }

    for ( unsigned int i = 0; i < numberOfServices; i++ ) {
        allServices->insert ( serviceDataBuffer[i].lpServiceName );
    }

    return allServices;
}