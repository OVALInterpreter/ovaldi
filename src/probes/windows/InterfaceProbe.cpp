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

#include "InterfaceProbe.h"

//****************************************************************************************//
//                              InterfaceProbe Class                                  //
//****************************************************************************************//
InterfaceProbe* InterfaceProbe::instance = NULL;

InterfaceProbe::InterfaceProbe() {
    interfaces = NULL;
}

InterfaceProbe::~InterfaceProbe() {
    instance = NULL;
    InterfaceProbe::DeleteInterfaces();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

AbsProbe* InterfaceProbe::Instance() {
    // Use lazy initialization for instance of the interface probe
    if ( instance == NULL ) {
        instance = new InterfaceProbe();
    }

    return instance;
}

ItemVector* InterfaceProbe::CollectItems ( Object* object ) {
    // Get the name from the provided object
    ObjectEntity* name = object->GetElementByName ( "name" );

    // Check datatypes - only allow string
    if ( name->GetDatatype() != OvalEnum::DATATYPE_STRING ) {
        throw ProbeException ( "Error: Invalid data type specified on name. Found: " + OvalEnum::DatatypeToString ( name->GetDatatype() ) );
    }

    //use lazy initialization to gather all the interfaces on the system
    if ( interfaces == NULL ) {
        InterfaceProbe::GetAllInterfaces();
    }

    ItemVector* collectedItems = new ItemVector();

    if ( name->GetVarRef() == NULL ) {
        if ( name->GetOperation() == OvalEnum::OPERATION_EQUALS ) {
            Item* aInterface = InterfaceProbe::GetInterface ( name->GetValue() );

            if ( aInterface != NULL ) {
                Item* temp = new Item ( *aInterface );
                collectedItems->push_back ( temp );

            } else {
                aInterface = this->CreateItem();
                aInterface->SetStatus ( OvalEnum::STATUS_DOES_NOT_EXIST );
                aInterface->AppendElement ( new ItemEntity ( "name" , name->GetValue() , OvalEnum::DATATYPE_STRING , true , OvalEnum::STATUS_DOES_NOT_EXIST ) );
                collectedItems->push_back ( aInterface );
            }

        } else {
            ItemVector::iterator it1;
            ItemEntityVector::iterator it2;

            // Loop through all interfaces if they are a regex match on netname create item an return it
            for ( it1 = InterfaceProbe::interfaces->begin() ; it1 != InterfaceProbe::interfaces->end() ; it1++ ) {
                ItemEntityVector* nameVector = ( *it1 )->GetElementsByName ( "name" );

                for ( it2 = nameVector->begin() ; it2 != nameVector->end() ; it2++ ) {
                    string first = ( *it2 )->GetValue();

					bool matched;
					switch (name->GetOperation()) {
					case OvalEnum::OPERATION_NOT_EQUAL:
						matched = name->GetValue() != first;
						break;
					case OvalEnum::OPERATION_PATTERN_MATCH:
						matched = this->myMatcher->IsMatch ( name->GetValue().c_str() , first.c_str() );
						break;
					case OvalEnum::OPERATION_CASE_INSENSITIVE_EQUALS:
						matched = Common::EqualsIgnoreCase(name->GetValue(), first);
						break;
					case OvalEnum::OPERATION_CASE_INSENSITIVE_NOT_EQUAL:
						matched = !Common::EqualsIgnoreCase(name->GetValue(), first);
						break;
					default:
				        throw ProbeException ( "Error: Invalid operation specified on name. Found: " + OvalEnum::OperationToString ( name->GetOperation() ) );
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
        // Loop through all shared interfaces on the system
        // Only keep the shared interfaces that match operation and value and var check
        ItemVector::iterator it1;
        ItemEntityVector::iterator it2;

        for ( it1 = InterfaceProbe::interfaces->begin() ; it1 != InterfaceProbe::interfaces->end() ; it1++ ) {
            ItemEntityVector* nameVector = ( *it1 )->GetElementsByName ( "name" );

            for ( it2 = nameVector->begin() ; it2 != nameVector->end() ; it2++ ) {
                if ( name->Analyze ( *it2 ) == OvalEnum::RESULT_TRUE ) {
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

Item* InterfaceProbe::CreateItem() {
    Item* item = new Item ( 0 ,
                            "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows" ,
                            "win-sc" ,
                            "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows windows-system-characteristics-schema.xsd" ,
                            OvalEnum::STATUS_ERROR ,
                            "interface_item" );
    return item;
}

Item* InterfaceProbe::GetInterface ( string nameStr ) {
    ItemVector::iterator it1;
    ItemEntityVector::iterator it2;

    for ( it1 = InterfaceProbe::interfaces->begin() ; it1 != InterfaceProbe::interfaces->end() ; it1++ ) {
        ItemEntityVector* nameVector = ( *it1 )->GetElementsByName ( "name" );

        for ( it2 = nameVector->begin() ; it2 != nameVector->end() ; it2++ ) {
            if ( ( *it2 )->GetValue().compare ( nameStr ) == 0 ) {
                return ( *it1 );
            }
        }
    }

    return NULL;
}

void InterfaceProbe::GetAllInterfaces() {
    InterfaceProbe::interfaces = new ItemVector();
    PMIB_IPADDRTABLE ipAddrTable = ( PMIB_IPADDRTABLE ) malloc ( sizeof ( MIB_IPADDRTABLE ) );
    PMIB_IFTABLE ifTable = ( PMIB_IFTABLE ) malloc ( sizeof ( MIB_IFTABLE ) );
    PMIB_IFROW ifRow = ( PMIB_IFROW ) malloc ( sizeof ( MIB_IFROW ) );
    DWORD addrSize = 0;
    DWORD ifSize = 0;
    DWORD errorIpAddrTable;
    DWORD errorIfTable = 0;

    if ( GetIpAddrTable ( ipAddrTable , &addrSize , 0 ) == ERROR_INSUFFICIENT_BUFFER ) {
        free ( ipAddrTable );
        ipAddrTable = ( PMIB_IPADDRTABLE ) malloc ( addrSize );
    }

    if ( GetIfTable ( ifTable , &ifSize , FALSE ) == ERROR_INSUFFICIENT_BUFFER ) {
        free ( ifTable );
        ifTable = ( PMIB_IFTABLE ) malloc ( ifSize );
    }

    if ( ipAddrTable == NULL || ifTable == NULL ) {
        InterfaceProbe::DeleteInterfaces();

        if ( ipAddrTable != NULL ) {
            free ( ipAddrTable );
            ipAddrTable = NULL;
        }

        if ( ifTable != NULL ) {
            free ( ifTable );
            ifTable = NULL;
        }

        if ( ifRow != NULL ) {
            free ( ifRow );
            ifRow = NULL;
        }

        string errorMessage = "";

        if ( ipAddrTable == NULL ) {
            errorMessage.append ( "Error: Unable to allocate memory for the MIB_IPADDRTABLE data structure. Microsoft System Error " + Common::ToString ( GetLastError() ) + " - " + WindowsCommon::GetErrorMessage ( GetLastError() ) );
        }

        if ( ifTable == NULL ) {
            errorMessage.append ( "Error: Unable to allocate memory for the MIB_IFTABLE data structure. Microsoft System Error " + Common::ToString ( GetLastError() ) + " - " + WindowsCommon::GetErrorMessage ( GetLastError() ) );
        }

        throw ProbeException ( errorMessage );
    }

    if ( ( ( errorIpAddrTable = GetIpAddrTable ( ipAddrTable , &addrSize , 0 ) ) == NO_ERROR ) && ( ( errorIfTable = GetIfTable ( ifTable , &ifSize , 0 ) ) == NO_ERROR ) ) {
        DWORD ifTableLength = ifTable->dwNumEntries;
        Item* item;

        for ( unsigned int i = 0 ; i < ifTableLength ; i++ ) {
            item = this->CreateItem();			
			item->SetStatus ( OvalEnum::STATUS_EXISTS );

            ifRow->dwIndex = ipAddrTable->table[i].dwIndex;

            if ( GetIfEntry ( ifRow ) == NO_ERROR ) {
                
                char *desc = ( char* ) malloc ( ifRow->dwDescrLen + 1 );

                if ( desc == NULL ) {
                    InterfaceProbe::DeleteInterfaces();

                    if ( ipAddrTable != NULL ) {
                        free ( ipAddrTable );
                        ipAddrTable = NULL;
                    }

                    if ( ifTable != NULL ) {
                        free ( ifTable );
                        ifTable = NULL;
                    }

                    if ( ifRow != NULL ) {
                        free ( ifRow );
                        ifRow = NULL;
                    }

                    throw ProbeException ( "Error: Unable to allocate memory for the interface's name.  Microsoft System Error " + Common::ToString ( GetLastError() ) + " - " + WindowsCommon::GetErrorMessage ( GetLastError() ) );
                }

                ZeroMemory ( desc , sizeof ( ifRow->dwDescrLen + 1 ) );

                for ( unsigned int k = 0 ; k < ifRow->dwDescrLen ; k++ ) {
                    desc[k] = ( char ) ifRow->bDescr[k];
                }

                item->AppendElement ( new ItemEntity ( "name" , desc , OvalEnum::DATATYPE_STRING , true , OvalEnum::STATUS_EXISTS ) );
				item->AppendElement ( new ItemEntity ( "index" , Common::ToString ( ipAddrTable->table[i].dwIndex ) , OvalEnum::DATATYPE_INTEGER , false , OvalEnum::STATUS_EXISTS ) );

				// Format MAC Address
                char *mac = ( char* ) malloc ( sizeof ( char ) * 30 );

                if ( mac == NULL ) {
                    InterfaceProbe::DeleteInterfaces();

                    if ( ipAddrTable != NULL ) {
                        free ( ipAddrTable );
                        ipAddrTable = NULL;
                    }

                    if ( ifTable != NULL ) {
                        free ( ifTable );
                        ifTable = NULL;
                    }

                    if ( ifRow != NULL ) {
                        free ( ifRow );
                        ifRow = NULL;
                    }

                    throw ProbeException ( "Error: Unable to allocate memory for the interface's MAC address. Microsoft System Error " + Common::ToString ( GetLastError() ) + " - " + WindowsCommon::GetErrorMessage ( GetLastError() ) );
                }

                ZeroMemory ( mac , 30 );

                for ( unsigned int j = 0 ; j < ifRow->dwPhysAddrLen ; j++ ) {
                    sprintf ( &mac[j*3] , "%02X-" , ifRow->bPhysAddr[j] );
                }

                string macStr = mac;
				item->AppendElement ( new ItemEntity ( "type" , InterfaceProbe::GetInterfaceType ( ifRow->dwType ) , OvalEnum::DATATYPE_STRING , false , OvalEnum::STATUS_EXISTS ) );
                item->AppendElement ( new ItemEntity ( "hardware_addr" , macStr.substr ( 0, macStr.length() - 1 ), OvalEnum::DATATYPE_STRING , false , OvalEnum::STATUS_EXISTS ) );

				in_addr addr;
				string inetAddr;
				string subnetMask;
	            
				addr.S_un.S_addr = ipAddrTable->table[i].dwAddr;
				item->AppendElement ( new ItemEntity ( "inet_addr" , inetAddr = inet_ntoa ( addr ) , OvalEnum::DATATYPE_STRING, false , OvalEnum::STATUS_EXISTS ) );
				addr.S_un.S_addr = ipAddrTable->table[i].dwMask;
				subnetMask = inet_ntoa ( addr );
				item->AppendElement ( new ItemEntity ( "broadcast_addr" , InterfaceProbe::CalculateBroadcastAddress ( inetAddr , subnetMask ) , OvalEnum::DATATYPE_STRING , false , OvalEnum::STATUS_EXISTS ) );
				item->AppendElement ( new ItemEntity ( "netmask" , subnetMask, OvalEnum::DATATYPE_STRING, false , OvalEnum::STATUS_EXISTS ) );

				StringVector* addrType = InterfaceProbe::GetInterfaceAddressType ( ipAddrTable->table[i].wType );				
				for ( StringVector::iterator it = addrType->begin(); it != addrType->end(); it++ ) {
					item->AppendElement ( new ItemEntity ( "addr_type" , *it , OvalEnum::DATATYPE_STRING , false , OvalEnum::STATUS_EXISTS ) );
				}

				if ( addrType != NULL ) {
					addrType->clear();
					delete addrType;
					addrType = NULL;
				}

                InterfaceProbe::interfaces->push_back ( item );

            } else {
                InterfaceProbe::DeleteInterfaces();

                if ( ipAddrTable != NULL ) {
                    free ( ipAddrTable );
                    ipAddrTable = NULL;
                }

                if ( ifTable != NULL ) {
                    free ( ifTable );
                    ifTable = NULL;
                }

                if ( ifRow != NULL ) {
                    free ( ifRow );
                    ifRow = NULL;
                }

                throw ProbeException ( "Error: The was an error retrieving the MIB_IFROW data structure using the GetIfEntry() function. Microsoft System Error " + Common::ToString ( GetLastError() ) + " - " + WindowsCommon::GetErrorMessage ( GetLastError() ) );
            }
        }

    } else {
        InterfaceProbe::DeleteInterfaces();

        if ( ipAddrTable != NULL ) {
            free ( ipAddrTable );
            ipAddrTable = NULL;
        }

        if ( ifTable != NULL ) {
            free ( ifTable );
            ifTable = NULL;
        }

        if ( ifRow != NULL ) {
            free ( ifRow );
            ifRow = NULL;
        }

        string errorMessage = "";

        if ( errorIpAddrTable != NO_ERROR ) {
            errorMessage.append ( "Error: Unable to retrieve the MIB_IPADDRTABLE data structure using the GetIpAddrTable() function. Microsoft System Error " + Common::ToString ( GetLastError() ) + " - " + WindowsCommon::GetErrorMessage ( GetLastError() ) );
        }

        if ( errorIfTable != NO_ERROR ) {
            errorMessage.append ( "Error: Unable to retrieve the MIB_IFTABLE data structure using the GetIfTable() function. Microsoft System Error " + Common::ToString ( GetLastError() ) + " - " + WindowsCommon::GetErrorMessage ( GetLastError() ) );
        }

        throw ProbeException ( errorMessage );
    }
}

string InterfaceProbe::GetInterfaceType ( DWORD type ) {
    string interfaceType;

    switch ( type ) {
        case MIB_IF_TYPE_ETHERNET:
            interfaceType = "MIB_IF_TYPE_ETHERNET";
            break;
        case MIB_IF_TYPE_FDDI:
            interfaceType = "MIB_IF_TYPE_FDDI";
            break;
        case MIB_IF_TYPE_LOOPBACK:
            interfaceType = "MIB_IF_TYPE_LOOPBACK";
            break;
        case MIB_IF_TYPE_OTHER:
            interfaceType = "MIB_IF_TYPE_OTHER";
            break;
        case MIB_IF_TYPE_PPP:
            interfaceType = "MIB_IF_TYPE_PPP";
            break;
        case MIB_IF_TYPE_SLIP:
            interfaceType = "MIB_IF_TYPE_SLIP";
            break;
        case MIB_IF_TYPE_TOKENRING:
            interfaceType = "MIB_IF_TYPE_TOKENRING";
            break;
        default:
            interfaceType = "";
            break;
    }

    return interfaceType;
}

StringVector * InterfaceProbe::GetInterfaceAddressType ( DWORD type ) {
    StringVector * addressType = new StringVector();

    if ( type & MIB_IPADDR_PRIMARY ) {
        addressType->push_back ( "MIB_IPADDR_PRIMARY" );
    }

    if ( type & MIB_IPADDR_DYNAMIC ) {
        addressType->push_back ( "MIB_IPADDR_DYNAMIC" );
    }

    if ( type & MIB_IPADDR_DISCONNECTED ) {
        addressType->push_back ( "MIB_IPADDR_DISCONNECTED" );
    }

    if ( type & MIB_IPADDR_DELETED ) {
        addressType->push_back ( "MIB_IPADDR_DELETED" );
    }

    if ( type & MIB_IPADDR_TRANSIENT ) {
        addressType->push_back ( "MIB_IPADDR_TRANSIENT" );
    }

    return addressType;
}

string InterfaceProbe::CalculateBroadcastAddress ( string ipAddrStr , string netMaskStr ) {
    // Restrict values to 8 bits
    union bits {

        unsigned int value;
        unsigned int bytevalue: 8;

    };
    bits b;
    string bcastaddrStr = "";
    unsigned int ip;
    unsigned int net;
    string temp1 = ipAddrStr.c_str();
    string temp2 = netMaskStr.c_str();

    // Loop through all four bytes of a IPv4 address and perform the OR operation on the IP address and the 1's complement of the subnet mask
    for ( int i = 0 ; i < 4 ; i++ ) {
        // Get first byte of the IPv4 IP address
        ip = atoi ( temp1.substr ( 0 , temp1.find_first_of ( "." ) ).c_str() );
        // Advance to the next byte in the IPv4 IP address
        temp1 = temp1.substr ( temp1.find_first_of ( "." ) + 1 ).c_str();
        // Get the 1's complement of the IPv4 subnet mask
        b.value = ~ atoi ( temp2.substr ( 0 , temp2.find_first_of ( "." ) ).c_str() );
        net = b.bytevalue;
        // Advance to the next byte in the IPv4 subnet mask
        temp2 = temp2.substr ( temp2.find_first_of ( "." ) + 1 ).c_str();
        // Add the bitwise OR result and the '.' character to the string representing the IPv4 broadcast address
        bcastaddrStr.append ( Common::ToString ( ip | net ) + "." );
    }

    // Remove the extra '.'
    bcastaddrStr = bcastaddrStr.substr ( 0 , bcastaddrStr.length() - 1 );
    return bcastaddrStr;
}

void InterfaceProbe::DeleteInterfaces() {
    if ( InterfaceProbe::interfaces != NULL ) {
        ItemVector::iterator it;

        for ( it = InterfaceProbe::interfaces->begin() ; it != InterfaceProbe::interfaces->end() ; it++ ) {
            if ( ( *it ) != NULL ) {
                delete ( *it );
                ( *it ) = NULL;
            }
        }

        delete InterfaceProbe::interfaces;
        InterfaceProbe::interfaces = NULL;
    }

    return;
}