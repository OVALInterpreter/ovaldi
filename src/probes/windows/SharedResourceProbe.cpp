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

#include "WindowsCommon.h"
#include <lm.h>

#include "SharedResourceProbe.h"

using namespace std;

namespace {

    /** Convert a SharedResource type DWORD value into its corresponding string value.
        *  Currently, as of Version 5.5, the OVAL Language only supports STYPE_DISKTREE, STYPE_PRINTQ,
        *  STYPE_DEVICE, STYPE_IPC, STYPE_SPECIAL, and STYPE_TEMPORARY as values for shared resource types.
        *  However, as the result of implementing this Probe, the shared resource types found in the corresponding
        *  function definition that are not mentioned above may be added to the OVAL Language in Version 5.6
        *  pending approval from the OVAL Board.  For more information regarding this issue, refer to the following link.
        *  http://n2.nabble.com/Proposal-to-Amend-the-win-def%3AEntityStateSharedResourceTypeType-Definition-td2617124.html.
        *  For Version 5.6, make sure to uncomment the case statements for the newly added shared resouce types, and to remove the STYPE_SPECIAL and STYPE_TEMPORARY case statements.
        *  @param sharedType A DWORD value that contains a bitmask of flags that represent the type of the Windows shared resource.
        *  @return A string representing the type of the Windows shared resource.
        */
    string GetSharedResourceType ( DWORD sharedType );
}

//****************************************************************************************//
//                              SharedResourceProbe Class                                 //
//****************************************************************************************//
SharedResourceProbe* SharedResourceProbe::instance = NULL;

SharedResourceProbe::SharedResourceProbe() {
    resources = NULL;
}

SharedResourceProbe::~SharedResourceProbe() {
    instance = NULL;
    SharedResourceProbe::DeleteSharedResources();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

AbsProbe* SharedResourceProbe::Instance() {
    // Use lazy initialization for instance of the shared resource probe
    if ( instance == NULL ) {
        instance = new SharedResourceProbe();
    }

    return instance;
}

ItemVector* SharedResourceProbe::CollectItems ( Object* object ) {
    // Get the netname from the provided object
    ObjectEntity* netName = object->GetElementByName ( "netname" );

    // Check datatypes - only allow the string datatype
    if ( netName->GetDatatype() != OvalEnum::DATATYPE_STRING ) {
        throw ProbeException ( "Error: Invalid data type specified on netname. Found: " + OvalEnum::DatatypeToString ( netName->GetDatatype() ) );
    }

    // Use lazy initialization to gather all the shared resources on the system
    if ( resources == NULL ) {
        SharedResourceProbe::GetAllSharedResources();
    }

    ItemVector* collectedItems = new ItemVector();

    if ( netName->GetVarRef() == NULL ) {
        if ( netName->GetOperation() == OvalEnum::OPERATION_EQUALS ) {
            Item* aResource = SharedResourceProbe::GetSharedResource ( netName->GetValue() );

            if ( aResource != NULL ) {
                Item* temp = new Item ( *aResource );
                collectedItems->push_back ( temp );

            } else {
                aResource = this->CreateItem();
                aResource->SetStatus ( OvalEnum::STATUS_DOES_NOT_EXIST );
                aResource->AppendElement ( new ItemEntity ( "netname" , "", OvalEnum::DATATYPE_STRING , OvalEnum::STATUS_DOES_NOT_EXIST ) );
                collectedItems->push_back ( aResource );
            }

        } else {
            ItemVector::iterator it1;
            ItemEntityVector::iterator it2;

            // Loop through all resources if they are a regex match on netname create item an return it
            for ( it1 = SharedResourceProbe::resources->begin() ; it1 != SharedResourceProbe::resources->end() ; it1++ ) {
                ItemEntityVector* netNameVector = ( *it1 )->GetElementsByName ( "netname" );

                for ( it2 = netNameVector->begin() ; it2 != netNameVector->end() ; it2++ ) {

					boolean matched;
					switch (netName->GetOperation()) {
					case OvalEnum::OPERATION_NOT_EQUAL:
						matched = netName->GetValue() != (*it2)->GetValue();
						break;
					case OvalEnum::OPERATION_PATTERN_MATCH:
						matched = this->myMatcher->IsMatch ( netName->GetValue().c_str() , ( *it2 )->GetValue().c_str() );
						break;
					case OvalEnum::OPERATION_CASE_INSENSITIVE_EQUALS:
						matched = Common::EqualsIgnoreCase(netName->GetValue(), (*it2)->GetValue());
						break;
					case OvalEnum::OPERATION_CASE_INSENSITIVE_NOT_EQUAL:
						matched = !Common::EqualsIgnoreCase(netName->GetValue(), (*it2)->GetValue());
						break;
					default:
				        throw ProbeException ( "Error: Invalid operation specified on netname. Found: " + OvalEnum::OperationToString ( netName->GetOperation() ) );
					}

					if (matched) {
						Item* temp = new Item ( **it1 );
						collectedItems->push_back ( temp );
						break;
					}
                }
            }
        }

    } else {
        // Loop through all shared resources on the system
        // Only keep the shared resources that match operation, value, and var check
        ItemVector::iterator it1;
        ItemEntityVector::iterator it2;

        for ( it1 = SharedResourceProbe::resources->begin() ; it1 != SharedResourceProbe::resources->end() ; it1++ ) {
            ItemEntityVector* netnameVector = ( *it1 )->GetElementsByName ( "netname" );

            for ( it2 = netnameVector->begin() ; it2 != netnameVector->end() ; it2++ ) {
                if ( netName->Analyze ( *it2 ) == OvalEnum::RESULT_TRUE ) {
                    Item* temp = new Item ( **it1 );
                    collectedItems->push_back ( temp );
                }
            }
        }
    }

    return collectedItems;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

Item* SharedResourceProbe::CreateItem() {
    Item* item = new Item ( 0 ,
                            "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows" ,
                            "win-sc" ,
                            "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows windows-system-characteristics-schema.xsd" ,
                            OvalEnum::STATUS_ERROR ,
                            "sharedresource_item" );
    return item;
}

void SharedResourceProbe::GetAllSharedResources() {
    PSHARE_INFO_2 buffer, bufPtr;
    DWORD entriesRead = 0;
    DWORD totalEntries = 0;
    string typeStr;
    bool accessRead = false;
    bool accessWrite = false;
    bool accessCreate = false;
    bool accessExec = false;
    bool accessDelete = false;
    bool accessAtrib = false;
    bool accessPerm = false;
    bool accessAll = false;

    if ( NetShareEnum ( NULL , 2 , ( LPBYTE * ) &buffer , MAX_PREFERRED_LENGTH , &entriesRead , &totalEntries , NULL ) == NERR_Success ) {
        SharedResourceProbe::resources = new ItemVector();
        Item* item;
        bufPtr = buffer;

        for ( unsigned int i = 0 ; i < entriesRead ; i++ ) {
            item = this->CreateItem();
            item->SetStatus ( OvalEnum::STATUS_EXISTS );

            if ( ( bufPtr->shi2_permissions & ACCESS_ALL ) == ACCESS_ALL ) {
                accessAll = true;
            }

            if ( ( bufPtr->shi2_permissions & ACCESS_READ ) == ACCESS_READ ) {
                accessRead = true;
            }

            if ( ( bufPtr->shi2_permissions & ACCESS_WRITE ) == ACCESS_WRITE ) {
                accessWrite = true;
            }

            if ( ( bufPtr->shi2_permissions & ACCESS_CREATE ) == ACCESS_CREATE ) {
                accessCreate = true;
            }

            if ( ( bufPtr->shi2_permissions & ACCESS_EXEC ) == ACCESS_EXEC ) {
                accessExec = true;
            }

            if ( ( bufPtr->shi2_permissions & ACCESS_DELETE ) == ACCESS_DELETE ) {
                accessDelete = true;
            }

            if ( ( bufPtr->shi2_permissions & ACCESS_ATRIB ) == ACCESS_ATRIB ) {
                accessAtrib = true;
            }

            if ( ( bufPtr->shi2_permissions & ACCESS_PERM ) == ACCESS_PERM ) {
                accessPerm = true;
            }

            item->AppendElement ( new ItemEntity ( "netname" , WindowsCommon::UnicodeToAsciiString ( bufPtr->shi2_netname ) , OvalEnum::DATATYPE_STRING , OvalEnum::STATUS_EXISTS ) );
			( ( typeStr = GetSharedResourceType ( bufPtr->shi2_type ) ).compare ( "" ) == 0 ) ? item->AppendElement ( new ItemEntity ( "shared_type" , typeStr , OvalEnum::DATATYPE_STRING , OvalEnum::STATUS_ERROR ) ) : item->AppendElement ( new ItemEntity ( "shared_type" , typeStr , OvalEnum::DATATYPE_STRING , OvalEnum::STATUS_EXISTS ) );
            item->AppendElement ( new ItemEntity ( "max_uses" , Common::ToString ( bufPtr->shi2_max_uses ) , OvalEnum::DATATYPE_INTEGER , OvalEnum::STATUS_EXISTS ) );
            item->AppendElement ( new ItemEntity ( "current_uses" , Common::ToString ( bufPtr->shi2_current_uses ) , OvalEnum::DATATYPE_INTEGER , OvalEnum::STATUS_EXISTS ) );
			item->AppendElement ( new ItemEntity ( "local_path" , WindowsCommon::UnicodeToAsciiString ( bufPtr->shi2_path ) , OvalEnum::DATATYPE_STRING , OvalEnum::STATUS_EXISTS ) );
            item->AppendElement ( new ItemEntity ( "access_read_permission" , Common::ToString ( accessRead ) , OvalEnum::DATATYPE_BOOLEAN , OvalEnum::STATUS_EXISTS ) );
            item->AppendElement ( new ItemEntity ( "access_write_permission" , Common::ToString ( accessWrite ) , OvalEnum::DATATYPE_BOOLEAN , OvalEnum::STATUS_EXISTS ) );
            item->AppendElement ( new ItemEntity ( "access_create_permission" , Common::ToString ( accessCreate ) , OvalEnum::DATATYPE_BOOLEAN , OvalEnum::STATUS_EXISTS ) );
            item->AppendElement ( new ItemEntity ( "access_exec_permission" , Common::ToString ( accessExec ) , OvalEnum::DATATYPE_BOOLEAN , OvalEnum::STATUS_EXISTS ) );
            item->AppendElement ( new ItemEntity ( "access_delete_permission" , Common::ToString ( accessDelete ) , OvalEnum::DATATYPE_BOOLEAN , OvalEnum::STATUS_EXISTS ) );
            item->AppendElement ( new ItemEntity ( "access_atrib_permission" , Common::ToString ( accessAtrib ) , OvalEnum::DATATYPE_BOOLEAN , OvalEnum::STATUS_EXISTS ) );
            item->AppendElement ( new ItemEntity ( "access_perm_permission" , Common::ToString ( accessPerm ) , OvalEnum::DATATYPE_BOOLEAN , OvalEnum::STATUS_EXISTS ) );
			item->AppendElement ( new ItemEntity ( "access_all_permission" , Common::ToString ( accessAll ) , OvalEnum::DATATYPE_BOOLEAN , OvalEnum::STATUS_EXISTS ) );
            resources->push_back ( item );
            bufPtr++;
        }

        if ( NetApiBufferFree ( buffer ) != NERR_Success ) {
            SharedResourceProbe::DeleteSharedResources();
            throw ProbeException ( "Error: NetApiBufferFree() was unable to free the memory allocated for the SHARE_INFO_2 structure. Microsoft System Error " + Common::ToString ( GetLastError() ) + ") - " + WindowsCommon::GetErrorMessage ( GetLastError() ) );
        }

    } else {
        throw ProbeException ( "Error: There was an error retrieving the shared resource data using NetShareEnum(). Microsoft System Error " + Common::ToString ( GetLastError() ) + " - " + WindowsCommon::GetErrorMessage ( GetLastError() ) );
    }

    return;
}

Item* SharedResourceProbe::GetSharedResource ( string netNameStr ) {
    ItemVector::iterator it1;
    ItemEntityVector::iterator it2;

    for ( it1 = SharedResourceProbe::resources->begin() ; it1 != SharedResourceProbe::resources->end() ; it1++ ) {
        ItemEntityVector* netNameVector = ( *it1 )->GetElementsByName ( "netname" );

        for ( it2 = netNameVector->begin() ; it2 != netNameVector->end() ; it2++ ) {
            if ( ( *it2 )->GetValue().compare ( netNameStr ) == 0 ) {
                return ( *it1 );
            }
        }
    }

    return NULL;
}


void SharedResourceProbe::DeleteSharedResources() {
    if ( SharedResourceProbe::resources != NULL ) {
        ItemVector::iterator it;

        for ( it = SharedResourceProbe::resources->begin() ; it != SharedResourceProbe::resources->end() ; it++ ) {
            if ( ( *it ) != NULL ) {
                delete ( *it );
                ( *it ) = NULL;
            }
        }

        delete SharedResourceProbe::resources;
        SharedResourceProbe::resources = NULL;
    }

    return;
}

namespace {

	string GetSharedResourceType ( DWORD sharedType ) {
		string typeStr;

		switch ( sharedType ) {
			case STYPE_DISKTREE:
				typeStr = "STYPE_DISKTREE";
				break;
			case STYPE_PRINTQ:
				typeStr = "STYPE_PRINTQ";
				break;
			case STYPE_DEVICE:
				typeStr = "STYPE_DEVICE";
				break;
			case STYPE_IPC:
				typeStr = "STYPE_IPC";
				break;
			case ( STYPE_SPECIAL | STYPE_DISKTREE ) :
				typeStr = "STYPE_DISKTREE_SPECIAL";
				break;
			case ( STYPE_TEMPORARY | STYPE_DISKTREE ) :
				typeStr = "STYPE_DISKTREE_TEMPORARY";
				break;
			case ( STYPE_SPECIAL | STYPE_TEMPORARY | STYPE_DISKTREE ) :
				typeStr = "STYPE_DISKTREE_SPECIAL_TEMPORARY";
				break;
			case ( STYPE_SPECIAL | STYPE_PRINTQ ) :
				typeStr = "STYPE_PRINTQ_SPECIAL";
				break;
			case ( STYPE_TEMPORARY | STYPE_PRINTQ ) :
				typeStr = "STYPE_PRINTQ_TEMPORARY";
				break;
			case ( STYPE_SPECIAL | STYPE_TEMPORARY | STYPE_PRINTQ ) :
				typeStr = "STYPE_PRINTQ_SPECIAL_TEMPORARY";
				break;
			case ( STYPE_SPECIAL | STYPE_DEVICE ) :
				typeStr = "STYPE_DEVICE_SPECIAL";
				break;
			case ( STYPE_TEMPORARY | STYPE_DEVICE ) :
				typeStr = "STYPE_DEVICE_TEMPORARY";
				break;
			case ( STYPE_SPECIAL | STYPE_TEMPORARY | STYPE_DEVICE ) :
				typeStr = "STYPE_DEVICE_SPECIAL_TEMPORARY";
				break;
			case ( STYPE_SPECIAL | STYPE_IPC ) :
				typeStr = "STYPE_IPC_SPECIAL";
				break;
			case ( STYPE_TEMPORARY | STYPE_IPC ) :
				typeStr = "STYPE_IPC_TEMPORARY";
				break;
			case ( STYPE_SPECIAL | STYPE_TEMPORARY | STYPE_IPC ) :
				typeStr = "STYPE_IPC_SPECIAL_TEMPORARY";
				break;
			default:
				typeStr = "";
				break;
		}

		return typeStr;
	}

}
