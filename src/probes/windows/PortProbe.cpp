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

// Including Iphlpapi.h by itself results in errors because it
// seems to use types like UCHAR without including the headers
// which define them.  I dunno what the minimum prerequisite
// includes are (WinDef.h wasn't enough); but including all of
// windows.h seemed to work.
#include <Windows.h>
#include <Iphlpapi.h>

#include "WindowsCommon.h"
#include "PortProbe.h"

//****************************************************************************************//
//                              PortProbe Class                                           //
//****************************************************************************************//

const string PortProbe::PROTOCOL_TCP = "TCP";
const string PortProbe::PROTOCOL_UDP = "UDP";

PortProbe* PortProbe::instance = NULL;

PortProbe::PortProbe() {
    ports = NULL;
}

PortProbe::~PortProbe() {
    instance = NULL;
    PortProbe::DeletePorts();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

AbsProbe* PortProbe::Instance() {
    // Use lazy initialization for instance of the shared resource probe
    if ( instance == NULL ) {
        instance = new PortProbe();
    }

    return instance;
}

ItemVector* PortProbe::CollectItems ( Object* object ) {
    ObjectEntity* localAddress = object->GetElementByName ( "local_address" );
    ObjectEntity* localPort = object->GetElementByName ( "local_port" );
    ObjectEntity* protocol = object->GetElementByName ( "protocol" );

    // check operation - only allow  equals, not equals and pattern match
    if ( localAddress->GetOperation() != OvalEnum::OPERATION_EQUALS && localAddress->GetOperation() != OvalEnum::OPERATION_PATTERN_MATCH && localAddress->GetOperation() != OvalEnum::OPERATION_NOT_EQUAL ) {
        throw ProbeException ( "Error: invalid operation specified on protocol. Found: " + OvalEnum::OperationToString ( localAddress->GetOperation() ) );
    }

    // check datatypes - only allow the string datatype
    if ( localAddress->GetDatatype() != OvalEnum::DATATYPE_STRING ) {
        throw ProbeException ( "Error: invalid data type specified on local_address. Found: " + OvalEnum::DatatypeToString ( localAddress->GetDatatype() ) );
    }

    // check operation - only allow  equals, not equal, greater than, greater than or equal, less than, less than or equal, bitwise and, and bitwise or
    if ( localPort->GetOperation() != OvalEnum::OPERATION_EQUALS &&
            localPort->GetOperation() != OvalEnum::OPERATION_NOT_EQUAL &&
            localPort->GetOperation() != OvalEnum::OPERATION_GREATER_THAN &&
            localPort->GetOperation() != OvalEnum::OPERATION_GREATER_THAN_OR_EQUAL &&
            localPort->GetOperation() != OvalEnum::OPERATION_LESS_THAN &&
            localPort->GetOperation() != OvalEnum::OPERATION_LESS_THAN_OR_EQUAL &&
            localPort->GetOperation() != OvalEnum::OPERATION_BITWISE_AND &&
            localPort->GetOperation() != OvalEnum::OPERATION_BITWISE_OR ) {
        throw ProbeException ( "Error: invalid operation specified on local_port. Found: " + OvalEnum::OperationToString ( localPort->GetOperation() ) );
    }

    // check datatypes - only allow the integer datatype
    if ( localPort->GetDatatype() != OvalEnum::DATATYPE_INTEGER ) {
        throw ProbeException ( "Error: invalid data type specified on local_port. Found: " + OvalEnum::DatatypeToString ( localPort->GetDatatype() ) );
    }

    // check operation - only allow  equals, not equals and pattern match
    if ( protocol->GetOperation() != OvalEnum::OPERATION_EQUALS && protocol->GetOperation() != OvalEnum::OPERATION_PATTERN_MATCH && protocol->GetOperation() != OvalEnum::OPERATION_NOT_EQUAL ) {
        throw ProbeException ( "Error: invalid operation specified on protocol. Found: " + OvalEnum::OperationToString ( protocol->GetOperation() ) );
    }

    // check datatypes - only allow the string datatype
    if ( protocol->GetDatatype() != OvalEnum::DATATYPE_STRING ) {
        throw ProbeException ( "Error: invalid data type specified on protocol. Found: " + OvalEnum::DatatypeToString ( protocol->GetDatatype() ) );
    }

    // Use lazy initialization to gather all of the ports on the system
    if ( ports == NULL ) {
        PortProbe::GetAllPorts();
    }

    ItemVector* collectedItems = new ItemVector();
    StringSet* protocols = this->GetProtocols ( protocol );

    if ( protocols->size() > 0 ) {
        StringSet::iterator iterator;

        for ( iterator = protocols->begin() ; iterator != protocols->end() ; iterator++ ) {
            string protocolStr = ( *iterator );
            StringSet* localAddresses = this->GetLocalAddresses ( protocolStr , localAddress );

            if ( localAddresses->size() > 0 ) {
                StringSet::iterator iterator2;

                for ( iterator2 = localAddresses->begin() ; iterator2 != localAddresses->end() ; iterator2++ ) {
                    string localAddressStr = ( *iterator2 );
                    StringSet* localPorts = this->GetLocalPorts ( protocolStr , localAddressStr , localPort );

                    if ( localPorts->size() > 0 ) {
                        StringSet::iterator iterator3;

                        for ( iterator3 = localPorts->begin() ; iterator3 != localPorts->end() ; iterator3++ ) {
                            long long portValue;
							if (!Common::FromString(*iterator3, &portValue))
								throw ProbeException("Couldn't convert to long long: "+*iterator3);

                            if ( portValue > -1 ) {
                                Item* item = PortProbe::GetPort ( localAddressStr , portValue , protocolStr );

                                if ( item != NULL ) {
                                    Item* temp = new Item ( *item );
                                    collectedItems->push_back ( temp );
                                }
                            }
                        }

                    } else {
                        if ( localPort->GetOperation() == OvalEnum::OPERATION_EQUALS ) {
                            if ( localPort->GetVarRef() == NULL ) {
                                Item* item = this->CreateItem();
                                item->SetStatus ( OvalEnum::STATUS_DOES_NOT_EXIST );
                                item->AppendElement ( new ItemEntity ( "protocol" ,  protocolStr , OvalEnum::DATATYPE_STRING , OvalEnum::STATUS_EXISTS ) );
                                item->AppendElement ( new ItemEntity ( "local_address" , localAddressStr , OvalEnum::DATATYPE_STRING , OvalEnum::STATUS_EXISTS ) );
                                item->AppendElement ( new ItemEntity ( "local_port" , "" , OvalEnum::DATATYPE_INTEGER , OvalEnum::STATUS_DOES_NOT_EXIST ) );
                                collectedItems->push_back ( item );

                            } else {
                                VariableValueVector::iterator iterator;

                                for ( iterator = localPort->GetVarRef()->GetValues()->begin() ; iterator != localPort->GetVarRef()->GetValues()->end() ; iterator++ ) {
                                    Item* item = this->CreateItem();
                                    item->SetStatus ( OvalEnum::STATUS_DOES_NOT_EXIST );
                                    item->AppendElement ( new ItemEntity ( "protocol" , protocolStr , OvalEnum::DATATYPE_STRING , OvalEnum::STATUS_EXISTS ) );
                                    item->AppendElement ( new ItemEntity ( "local_address" , localAddressStr , OvalEnum::DATATYPE_STRING , OvalEnum::STATUS_EXISTS ) );
                                    item->AppendElement ( new ItemEntity ( "local_port" , "" , OvalEnum::DATATYPE_INTEGER , OvalEnum::STATUS_DOES_NOT_EXIST ) );
                                    collectedItems->push_back ( item );
                                }
                            }
                        }
                    }

                    localPorts->clear();
                    delete localPorts;
                }

            } else {
                if ( localAddress->GetOperation() == OvalEnum::OPERATION_EQUALS ) {
                    if ( localAddress->GetVarRef() == NULL ) {
                        Item* item = this->CreateItem();
                        item->SetStatus ( OvalEnum::STATUS_DOES_NOT_EXIST );
                        item->AppendElement ( new ItemEntity ( "protocol" , protocolStr , OvalEnum::DATATYPE_STRING , OvalEnum::STATUS_EXISTS ) );
                        item->AppendElement ( new ItemEntity ( "local_address" , "" , OvalEnum::DATATYPE_STRING , OvalEnum::STATUS_DOES_NOT_EXIST ) );
                        collectedItems->push_back ( item );

                    } else {
                        VariableValueVector::iterator iterator;

                        for ( iterator = localAddress->GetVarRef()->GetValues()->begin() ; iterator != localAddress->GetVarRef()->GetValues()->end() ; iterator++ ) {
                            Item* item = this->CreateItem();
                            item->SetStatus ( OvalEnum::STATUS_DOES_NOT_EXIST );
                            item->AppendElement ( new ItemEntity ( "protocol" , protocolStr , OvalEnum::DATATYPE_STRING , OvalEnum::STATUS_EXISTS ) );
                            item->AppendElement ( new ItemEntity ( "local_address" , "" , OvalEnum::DATATYPE_STRING , OvalEnum::STATUS_DOES_NOT_EXIST ) );
                            collectedItems->push_back ( item );
                        }
                    }
                }
            }

            localAddresses->clear();
            delete localAddresses;
        }

    } else {
        if ( protocol->GetOperation() == OvalEnum::OPERATION_EQUALS ) {
            if ( protocol->GetVarRef() == NULL ) {
                Item* item = this->CreateItem();
                item->SetStatus ( OvalEnum::STATUS_DOES_NOT_EXIST );
                item->AppendElement ( new ItemEntity ( "protocol" , "" , OvalEnum::DATATYPE_STRING , OvalEnum::STATUS_DOES_NOT_EXIST ) );
                collectedItems->push_back ( item );

            } else {
                VariableValueVector::iterator iterator;

                for ( iterator = protocol->GetVarRef()->GetValues()->begin() ; iterator != protocol->GetVarRef()->GetValues()->end() ; iterator++ ) {
                    Item* item = this->CreateItem();
                    item->SetStatus ( OvalEnum::STATUS_DOES_NOT_EXIST );
                    item->AppendElement ( new ItemEntity ( "protocol" , "", OvalEnum::DATATYPE_STRING , OvalEnum::STATUS_DOES_NOT_EXIST ) );
                    collectedItems->push_back ( item );
                }
            }
        }
    }

    protocols->clear();
    delete protocols;
    return collectedItems;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

Item* PortProbe::CreateItem() {
    Item* item = new Item ( 0 ,
                            "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows" ,
                            "win-sc" ,
                            "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows windows-system-characteristics-schema.xsd" ,
                            OvalEnum::STATUS_ERROR ,
                            "port_item" );
    return item;
}

Item* PortProbe::BuildPortItem ( string localAddressStr , unsigned long localPort , string remoteAddressStr , unsigned long remotePort , string protocolStr , unsigned long pid ) {
    Item* item = this->CreateItem();
    item->SetStatus ( OvalEnum::STATUS_EXISTS );
    item->AppendElement ( new ItemEntity ( "local_address" , localAddressStr , OvalEnum::DATATYPE_STRING , OvalEnum::STATUS_EXISTS ) );
    item->AppendElement ( new ItemEntity ( "local_port" , Common::ToString ( localPort ) , OvalEnum::DATATYPE_INTEGER , OvalEnum::STATUS_EXISTS ) );
    item->AppendElement ( new ItemEntity ( "protocol" , protocolStr , OvalEnum::DATATYPE_STRING , OvalEnum::STATUS_EXISTS ) );
    item->AppendElement ( new ItemEntity ( "pid" , Common::ToString ( pid ) , OvalEnum::DATATYPE_INTEGER , OvalEnum::STATUS_EXISTS ) );
	if (!remoteAddressStr.empty()) {
		// UDP and TCP server sockets don't have the remote address info
		item->AppendElement ( new ItemEntity ( "foreign_address" , remoteAddressStr , OvalEnum::DATATYPE_STRING , OvalEnum::STATUS_EXISTS ) );
		item->AppendElement ( new ItemEntity ( "foreign_port" , Common::ToString ( remotePort ) , OvalEnum::DATATYPE_STRING , OvalEnum::STATUS_EXISTS ) );
	}
    return item;
}

void PortProbe::GetAllPorts() {
    PortProbe::ports = new ItemVector();
    // Variables for retrieving the TCP table
    PMIB_TCPTABLE_OWNER_PID tcpTable;
    DWORD tcpSize;
    TCP_TABLE_CLASS tcpTableClass = TCP_TABLE_OWNER_PID_ALL;
    // Variables for retrieving the UDP table
    PMIB_UDPTABLE_OWNER_PID udpTable;
    DWORD udpSize;
    UDP_TABLE_CLASS udpTableClass = UDP_TABLE_OWNER_PID;
    // Common variables between API function calls
    BOOL order = true;
    ULONG version = AF_INET;
    ULONG reserved = 0;
    DWORD errorValue;
    // Retrieve the information regarding the TCP ports
    tcpTable = ( PMIB_TCPTABLE_OWNER_PID ) malloc ( sizeof ( MIB_TCPTABLE_OWNER_PID ) );

    if ( tcpTable == NULL ) {
        PortProbe::DeletePorts();
        throw ProbeException ( "Error: There was an error allocating memory for the MIB_TCPTABLE_OWNER_PID data structure. Microsoft System Error " + Common::ToString ( GetLastError() ) + " - " + WindowsCommon::GetErrorMessage ( GetLastError() ) );
    }

    tcpSize = sizeof ( MIB_TCPTABLE_OWNER_PID );
    errorValue = GetExtendedTcpTable ( tcpTable , &tcpSize, order , version , tcpTableClass , reserved );

    if ( errorValue == ERROR_INVALID_PARAMETER ) {
        if ( tcpTable != NULL ) {
            free ( tcpTable );
            tcpTable = NULL;
        }

        PortProbe::DeletePorts();
        throw ProbeException ( "Error: An invalid parameter was passed to GetExtendedTcpTable(). Microsoft System Error " + Common::ToString ( GetLastError() ) + " - " + WindowsCommon::GetErrorMessage ( GetLastError() ) );
    }

    // If the error is a result of an insufficient buffer, increase the size of the buffer and try again.
    if ( errorValue == ERROR_INSUFFICIENT_BUFFER ) {
        if ( tcpTable != NULL ) {
            free ( tcpTable );
            tcpTable = NULL;
        }

        tcpTable = ( PMIB_TCPTABLE_OWNER_PID ) malloc ( tcpSize );

        if ( tcpTable == NULL ) {
            PortProbe::DeletePorts();
            throw ProbeException ( "Error: There was an error allocating memory for the MIB_TCPTABLE_OWNER_PID data structure. Microsoft System Error " + Common::ToString ( GetLastError() ) + " - " + WindowsCommon::GetErrorMessage ( GetLastError() ) );
        }
    }

    if ( GetExtendedTcpTable ( tcpTable , &tcpSize, order , version , tcpTableClass , reserved ) == NO_ERROR ) {
        int entries = tcpTable->dwNumEntries;
        struct in_addr ipAddr;
        char localAddrBuffer[16];
		char remoteAddrBuffer[16] = { 0 }; // "" means not applicable

        for ( int i = 0 ; i < entries ; i++ ) {
            ipAddr.S_un.S_addr = ( u_long ) tcpTable->table[i].dwLocalAddr;
            strcpy_s ( localAddrBuffer , sizeof ( localAddrBuffer ) , inet_ntoa ( ipAddr ) );
			if (tcpTable->table[i].dwState != MIB_TCP_STATE_LISTEN) {
				ipAddr.S_un.S_addr = ( u_long ) tcpTable->table[i].dwRemoteAddr;
				strcpy_s ( remoteAddrBuffer , sizeof ( remoteAddrBuffer ) , inet_ntoa ( ipAddr ) );
			}

            PortProbe::ports->push_back ( PortProbe::BuildPortItem ( localAddrBuffer , ntohs ( ( unsigned short ) tcpTable->table[i].dwLocalPort ) , remoteAddrBuffer, ntohs ( ( unsigned short ) tcpTable->table[i].dwRemotePort ), PROTOCOL_TCP , tcpTable->table[i].dwOwningPid ) );
        }

        if ( tcpTable != NULL ) {
            free ( tcpTable );
            tcpTable = NULL;
        }

    } else {
        if ( tcpTable != NULL ) {
            free ( tcpTable );
            tcpTable = NULL;
        }

        PortProbe::DeletePorts();
        throw ProbeException ( "Error: There was an error while retrieving information about the TCP ports using GetExtendedTcpTable(). Microsoft System Error " + Common::ToString ( GetLastError() ) + " - " + WindowsCommon::GetErrorMessage ( GetLastError() ) );
    }

    // Retrieve the information regarding the UDP ports
    udpTable = ( PMIB_UDPTABLE_OWNER_PID ) malloc ( sizeof ( MIB_UDPTABLE_OWNER_PID ) );

    if ( udpTable == NULL ) {
        PortProbe::DeletePorts();
        throw ProbeException ( "Error: There was an error allocating memory for the MIB_UDPTABLE_OWNER_PID data structure. Microsoft System Error " + Common::ToString ( GetLastError() ) + " - " + WindowsCommon::GetErrorMessage ( GetLastError() ) );
    }

    udpSize = sizeof ( MIB_UDPTABLE_OWNER_PID );
    errorValue = GetExtendedUdpTable ( udpTable , &udpSize, order , version , udpTableClass , reserved );

    if ( errorValue == ERROR_INVALID_PARAMETER ) {
        if ( udpTable != NULL ) {
            free ( udpTable );
            udpTable = NULL;
        }

        PortProbe::DeletePorts();
        throw ProbeException ( "Error: An invalid parameter was passed to GetExtendedUdpTable(). Microsoft System Error " + Common::ToString ( GetLastError() ) + " - " + WindowsCommon::GetErrorMessage ( GetLastError() ) );
    }

    // If the error is a result of an insufficient buffer, increase the size of the buffer and try again.
    if ( errorValue == ERROR_INSUFFICIENT_BUFFER ) {
        if ( udpTable != NULL ) {
            free ( udpTable );
            udpTable = NULL;
        }

        udpTable = ( PMIB_UDPTABLE_OWNER_PID ) malloc ( udpSize );

        if ( udpTable == NULL ) {
            PortProbe::DeletePorts();
            throw ProbeException ( "Error: There was an error allocating memory for the MIB_TCPTABLE_OWNER_PID data structure. Microsoft System Error " + Common::ToString ( GetLastError() ) + " - " + WindowsCommon::GetErrorMessage ( GetLastError() ) );
        }
    }

    if ( GetExtendedUdpTable ( udpTable , &udpSize, order , version , udpTableClass , reserved ) == NO_ERROR ) {
        int entries = udpTable->dwNumEntries;
        struct in_addr ipAddr;
        char localAddrBuffer[16];

        for ( int i = 0 ; i < entries ; i++ ) {
            ipAddr.S_un.S_addr = ( u_long ) udpTable->table[i].dwLocalAddr;
            strcpy_s ( localAddrBuffer , sizeof ( localAddrBuffer ) , inet_ntoa ( ipAddr ) );
            PortProbe::ports->push_back ( PortProbe::BuildPortItem ( localAddrBuffer , ntohs ( ( unsigned short ) udpTable->table[i].dwLocalPort ) , "", 0, PROTOCOL_UDP , udpTable->table[i].dwOwningPid ) );
        }

        if ( udpTable != NULL ) {
            free ( udpTable );
            udpTable = NULL;
        }

    } else {
        if ( udpTable != NULL ) {
            free ( udpTable );
            udpTable = NULL;
        }

        PortProbe::DeletePorts();
        throw ProbeException ( "Error: There was an error while retrieving information about the UDP ports using GetExtendedUdpTable(). Microsoft System Error " + Common::ToString ( GetLastError() ) + " - " + WindowsCommon::GetErrorMessage ( GetLastError() ) );
    }

    return;
}

Item* PortProbe::GetPort ( string localAddressStr , long long localPort , string protocolStr ) {
    ItemVector::iterator iterator;

    for ( iterator = PortProbe::ports->begin() ; iterator != PortProbe::ports->end() ; iterator++ ) {
        if ( ( protocolStr.compare ( ( *iterator )->GetElementByName ( "protocol" )->GetValue() ) == 0 ) && ( localAddressStr.compare ( ( *iterator )->GetElementByName ( "local_address" )->GetValue() ) == 0 ) && ( Common::ToString ( localPort ).compare ( ( *iterator )->GetElementByName ( "local_port" )->GetValue() ) == 0 ) ) {
            return ( *iterator );
        }
    }

    return NULL;
}

StringSet* PortProbe::GetProtocols ( ObjectEntity* protocol ) {
    StringSet* protocols = NULL;

    // Does this ObjectEntity use variables?
    if ( protocol->GetVarRef() == NULL ) {
        // Proceed based on operation
        if ( protocol->GetOperation() == OvalEnum::OPERATION_EQUALS ) {
            protocols = new StringSet();

            // If the protocol exists add it to the list
            if ( this->ProtocolExists ( protocol->GetValue() ) ) {
                protocols->insert ( protocol->GetValue() );
            }

        } else if ( protocol->GetOperation() == OvalEnum::OPERATION_NOT_EQUAL ) {
            protocols = this->GetMatchingProtocols ( protocol->GetValue() , false );

        } else if ( protocol->GetOperation() == OvalEnum::OPERATION_PATTERN_MATCH ) {
            protocols = this->GetMatchingProtocols ( protocol->GetValue() , true );
        }

    } else {
        protocols = new StringSet();
        // Get all protocols
        StringSet* allProtocols = new StringSet();

        if ( protocol->GetOperation() == OvalEnum::OPERATION_EQUALS ) {
            // In the case of equals simply loop through all the
            // variable values and add them to the set of all protocols
            // if they exist on the system
            VariableValueVector::iterator iterator;

            for ( iterator = protocol->GetVarRef()->GetValues()->begin() ; iterator != protocol->GetVarRef()->GetValues()->end() ; iterator++ ) {
                if ( this->ProtocolExists ( ( *iterator )->GetValue() ) ) {
                    allProtocols->insert ( ( *iterator )->GetValue() );
                }
            }

        } else {
            allProtocols = this->GetMatchingProtocols ( ".*" , true );
        }

        // Loop through all protocols on the system
        // only keep protocols that match operation and value and var check
        ItemEntity* tmp = this->CreateItemEntity ( protocol );
        StringSet::iterator it;

        for ( it = allProtocols->begin() ; it != allProtocols->end() ; it++ ) {
            tmp->SetValue ( ( *it ) );

            if ( protocol->Analyze ( tmp ) == OvalEnum::RESULT_TRUE ) {
                protocols->insert ( ( *it ) );
            }
        }
    }

    return protocols;
}

StringSet* PortProbe::GetMatchingProtocols ( string pattern , bool isRegex ) {
    StringSet* protocols = new StringSet();
    ItemVector::iterator iterator;

    for ( iterator = PortProbe::ports->begin() ; iterator != PortProbe::ports->end() ; iterator++ ) {
        if ( this->IsMatch ( pattern , ( *iterator )->GetElementByName ( "protocol" )->GetValue() , isRegex ) ) {
            protocols->insert ( ( *iterator )->GetElementByName ( "protocol" )->GetValue() );
        }
    }

    return protocols;
}

bool PortProbe::ProtocolExists ( string protocolStr ) {
    bool exists = false;
    ItemVector::iterator iterator;

    for ( iterator = PortProbe::ports->begin(); iterator != PortProbe::ports->end(); iterator++ ) {
        if ( protocolStr.compare ( ( *iterator )->GetElementByName ( "protocol" )->GetValue() ) == 0 ) {
            exists = true;
            break;
        }
    }

    return exists;
}

StringSet* PortProbe::GetLocalAddresses ( string protocolStr , ObjectEntity* localAddress ) {
    StringSet* localAddresses = NULL;

    // Does this ObjectEntity use variables?
    if ( localAddress->GetVarRef() == NULL ) {
        // Proceed based on operation
        if ( localAddress->GetOperation() == OvalEnum::OPERATION_EQUALS ) {
            localAddresses = new StringSet();

            // If the local address exists add it to the list
            if ( this->LocalAddressExists ( protocolStr , localAddress->GetValue() ) ) {
                localAddresses->insert ( localAddress->GetValue() );
            }

        } else if ( localAddress->GetOperation() == OvalEnum::OPERATION_NOT_EQUAL ) {
            localAddresses = this->GetMatchingLocalAddresses ( protocolStr , localAddress->GetValue() , false );

        } else if ( localAddress->GetOperation() == OvalEnum::OPERATION_PATTERN_MATCH ) {
            localAddresses = this->GetMatchingLocalAddresses ( protocolStr , localAddress->GetValue() , true );
        }

    } else {
        localAddresses = new StringSet();
        // Get all local addresses
        StringSet* allLocalAddresses = new StringSet();

        if ( localAddress->GetOperation() == OvalEnum::OPERATION_EQUALS ) {
            // In the case of equals simply loop through all the
            // variable values and add them to the set of all local addresses
            // if they exist on the system
            VariableValueVector::iterator iterator;

            for ( iterator = localAddress->GetVarRef()->GetValues()->begin() ; iterator != localAddress->GetVarRef()->GetValues()->end() ; iterator++ ) {
                if ( this->LocalAddressExists ( protocolStr , ( *iterator )->GetValue() ) ) {
                    allLocalAddresses->insert ( ( *iterator )->GetValue() );
                }
            }

        } else {
            allLocalAddresses = this->GetMatchingLocalAddresses ( protocolStr , ".*" , true );
        }

        // Loop through all local addresses on the system
        // only keep local addresses that match operation and value and var check
        ItemEntity* tmp = this->CreateItemEntity ( localAddress );
        StringSet::iterator it;

        for ( it = allLocalAddresses->begin() ; it != allLocalAddresses->end() ; it++ ) {
            tmp->SetValue ( ( *it ) );

            if ( localAddress->Analyze ( tmp ) == OvalEnum::RESULT_TRUE ) {
                localAddresses->insert ( ( *it ) );
            }
        }
    }

    return localAddresses;
}

StringSet* PortProbe::GetMatchingLocalAddresses ( string protocolStr , string pattern , bool isRegex ) {
    StringSet* localAddresses = new StringSet();
    ItemVector::iterator iterator;

    for ( iterator = PortProbe::ports->begin() ; iterator != PortProbe::ports->end() ; iterator++ ) {
        if ( ( protocolStr.compare ( ( *iterator )->GetElementByName ( "protocol" )->GetValue() ) == 0 ) && ( this->IsMatch ( pattern , ( *iterator )->GetElementByName ( "local_address" )->GetValue() , isRegex ) ) ) {
            localAddresses->insert ( ( *iterator )->GetElementByName ( "local_address" )->GetValue() );
        }
    }

    return localAddresses;
}

bool PortProbe::LocalAddressExists ( string protocolStr , string localAddressStr ) {
    bool exists = false;
    ItemVector::iterator it;

    for ( it = PortProbe::ports->begin() ; it != PortProbe::ports->end() ; it++ ) {
        if ( ( protocolStr.compare ( ( *it )->GetElementByName ( "protocol" )->GetValue() ) == 0 ) && ( localAddressStr.compare ( ( *it )->GetElementByName ( "local_address" )->GetValue() ) == 0 ) ) {
            exists = true;
            break;
        }
    }

    return exists;
}

StringSet* PortProbe::GetLocalPorts ( string protocolStr , string localAddressStr , ObjectEntity* localPort ) {
    StringSet* localPorts = NULL;

    // Does this ObjectEntity use variables?
    if ( localPort->GetVarRef() == NULL ) {
        // Proceed based on operation
        if ( localPort->GetOperation() == OvalEnum::OPERATION_EQUALS ) {
            localPorts = new StringSet();

            // If the localPort exists add it to the list
            if ( this->LocalPortExists ( protocolStr , localAddressStr , localPort->GetValue() ) ) {
                localPorts->insert ( localPort->GetValue() );
            }

        } else {
            localPorts = this->GetMatchingLocalPorts ( protocolStr , localAddressStr , localPort , false );
        }

    } else {
        localPorts = new StringSet();
        // Get all port values
        StringSet* allLocalPorts = new StringSet();

        if ( localPort->GetOperation() == OvalEnum::OPERATION_EQUALS ) {
            // In the case of equals simply loop through all the
            // variable values and add them to the set of all ports
            // if they exist on the system
            VariableValueVector::iterator iterator;

            for ( iterator = localPort->GetVarRef()->GetValues()->begin() ; iterator != localPort->GetVarRef()->GetValues()->end() ; iterator++ ) {
                if ( this->LocalPortExists ( protocolStr , localAddressStr , ( *iterator )->GetValue() ) ) {
                    allLocalPorts->insert ( ( *iterator )->GetValue() );
                }
            }

        } else {
            allLocalPorts = this->GetMatchingLocalPorts ( protocolStr , localAddressStr , localPort , true );
        }

        // Loop through all names on the system
        // only keep ports that match operation and value and var check
        ItemEntity* tmp = this->CreateItemEntity ( localPort );
        StringSet::iterator it;

        for ( it = allLocalPorts->begin() ; it != allLocalPorts->end() ; it++ ) {
            tmp->SetValue ( ( *it ) );

            if ( localPort->Analyze ( tmp ) == OvalEnum::RESULT_TRUE ) {
                localPorts->insert ( ( *it ) );
            }
        }
    }

    return localPorts;
}

StringSet* PortProbe::GetMatchingLocalPorts ( string protocolStr , string localAddressStr , ObjectEntity* localPort , bool loopThroughAllVariables ) {
    StringSet* localPorts = new StringSet();
    OvalEnum::Operation op = localPort->GetOperation();
    long long port1;
    long long port2;

    if ( ! ( loopThroughAllVariables ) ) {
        ItemVector::iterator iterator;

        for ( iterator = PortProbe::ports->begin() ; iterator != PortProbe::ports->end() ; iterator++ ) {
            if ( ( protocolStr.compare ( ( *iterator )->GetElementByName ( "protocol" )->GetValue() ) == 0 ) && ( localAddressStr.compare ( ( *iterator )->GetElementByName ( "local_address" )->GetValue() ) == 0 ) ) {
				Common::FromString(( *iterator )->GetElementByName ( "local_port" )->GetValue(), &port1);
				Common::FromString(localPort->GetValue(), &port2);

                if ( ( ( port1 > -1 ) && ( port2 > -1 ) ) && PortProbe::IsValidOperationAndValue ( op , port1 , port2 ) ) {
                    localPorts->insert ( Common::ToString ( port1 ) );
                }
            }
        }

    } else {
        VariableValueVector::iterator iterator1;
        ItemVector::iterator iterator2;

        for ( iterator1 = localPort->GetVarRef()->GetValues()->begin() ; iterator1 != localPort->GetVarRef()->GetValues()->end() ; iterator1++ ) {
            for ( iterator2 = PortProbe::ports->begin() ; iterator2 != PortProbe::ports->end() ; iterator2++ ) {
                if ( ( protocolStr.compare ( ( *iterator2 )->GetElementByName ( "protocol" )->GetValue() ) == 0 ) && ( localAddressStr.compare ( ( *iterator2 )->GetElementByName ( "local_address" )->GetValue() ) == 0 ) ) {
					Common::FromString(( *iterator2 )->GetElementByName ( "local_port" )->GetValue(), &port1);
					Common::FromString(( *iterator1 )->GetValue(), &port2);

                    if ( ( ( port1 > -1 ) && ( port2 > -1 ) ) && PortProbe::IsValidOperationAndValue ( op , port1 , port2 ) ) {
                        localPorts->insert ( Common::ToString ( port1 ) );
                    }
                }
            }
        }
    }

    return localPorts;
}

bool PortProbe::LocalPortExists ( string protocolStr , string localAddressStr , string localPortStr ) {
    bool exists = false;
    ItemVector::iterator it;

    for ( it = PortProbe::ports->begin() ; it != PortProbe::ports->end() ; it++ ) {
        if ( ( protocolStr.compare ( ( ( *it )->GetElementByName ( "protocol" ) )->GetValue() ) == 0 ) && ( localAddressStr.compare ( ( ( *it )->GetElementByName ( "local_address" ) )->GetValue() ) == 0 ) && ( localPortStr.compare ( ( ( *it )->GetElementByName ( "local_port" ) )->GetValue() ) ==  0 ) ) {
            exists = true;
            break;
        }
    }

    return exists;
}

bool PortProbe::IsValidOperationAndValue ( OvalEnum::Operation op , long long port1 , long long port2 ) {
    bool isValid = false;

    if ( ( op == OvalEnum::OPERATION_EQUALS ) && ( port1 == port2 ) ) {
        isValid = true;

    } else if ( op == OvalEnum::OPERATION_NOT_EQUAL && ( port1 != port2 ) ) {
        isValid = true;

    } else if ( op == OvalEnum::OPERATION_GREATER_THAN && ( port1 > port2 ) ) {
        isValid = true;

    } else if ( op == OvalEnum::OPERATION_GREATER_THAN_OR_EQUAL && ( port1 >= port2 ) ) {
        isValid = true;

    } else if ( op == OvalEnum::OPERATION_LESS_THAN && ( port1 < port2 ) ) {
        isValid = true;

    } else if ( op == OvalEnum::OPERATION_LESS_THAN_OR_EQUAL && ( port1 <= port2 ) ) {
        isValid = true;

    } else if ( op == OvalEnum::OPERATION_BITWISE_AND && ( ( port1 & port2 ) == port2 ) ) {
        isValid = true;

    } else if ( op == OvalEnum::OPERATION_BITWISE_OR && ( ( port1  | port2 ) == port2 ) ) {
        isValid = true;
    }

    return isValid;
}

void PortProbe::DeletePorts() {
    if ( PortProbe::ports != NULL ) {
        ItemVector::iterator it;

        for ( it = PortProbe::ports->begin() ; it != PortProbe::ports->end() ; it++ ) {
            if ( ( *it ) != NULL ) {
                delete ( *it );
                ( *it ) = NULL;
            }
        }

        delete PortProbe::ports;
        PortProbe::ports = NULL;
    }

    return;
}