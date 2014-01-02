//
//
//****************************************************************************************//
// Copyright (c) 2002-2014, The MITRE Corporation
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

#include <Winsock2.h>
#include <iphlpapi.h>
#include <memory>
#include <iomanip>

#include <FreeGuard.h>
#include <VectorPtrGuard.h>
#include "WindowsCommon.h"

#include "InterfaceProbe.h"

using namespace std;

namespace {
	/**
	 * A simple helper function to convert ipv4 addresses as
	 * DWORD's in network byte order, to a string.
	 */
	string dwordIpToString(DWORD addr);
}

//****************************************************************************************//
//                              InterfaceProbe Class                                  //
//****************************************************************************************//
InterfaceProbe* InterfaceProbe::instance = NULL;

InterfaceProbe::InterfaceProbe() {
    interfaces = NULL;
}

InterfaceProbe::~InterfaceProbe() {
    instance = NULL;
    this->DeleteInterfaces();
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
                aInterface->AppendElement ( new ItemEntity ( "name" ,"" , OvalEnum::DATATYPE_STRING , OvalEnum::STATUS_DOES_NOT_EXIST ) );
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
	VectorPtrGuard<Item> interfaces(new ItemVector());

	// These won't hold exactly 1 of the specified type...
	// There is typical C wishywashyness with typing going
	// on here... but at least the first part of the memory
	// block ought to hold a struct of the given type.
	FreeGuard<MIB_IFTABLE> ifTable;
	FreeGuard<MIB_IPADDRTABLE> ipTable;
	DWORD ifSize = 0, ipSize = 0;
	DWORD err;

	// get buffer sizes first, then make the "real" call to get the data.
    if ((err = GetIpAddrTable(NULL, &ipSize, FALSE)) == ERROR_INSUFFICIENT_BUFFER) {
        ipTable.reset(malloc(ipSize));
		if (!ipTable.get())
			throw ProbeException("malloc(" + Common::ToString(ipSize) + 
				"): " + strerror(errno));
    } else
		throw ProbeException("GetIpAddrTable() error: " +
			WindowsCommon::GetErrorMessage(err));

	if ((err = GetIpAddrTable(ipTable.get(), &ipSize, FALSE)) != NO_ERROR)
		throw ProbeException("GetIpAddrTable() error: " +
			WindowsCommon::GetErrorMessage(err));

    if ((err = GetIfTable(NULL, &ifSize, FALSE)) == ERROR_INSUFFICIENT_BUFFER) {
        ifTable.reset(malloc(ifSize));
		if (!ifTable.get())
			throw ProbeException("malloc(" + Common::ToString(ifSize) + 
				"): " + strerror(errno));
    } else
		throw ProbeException("GetIfTable() error: " +
			WindowsCommon::GetErrorMessage(err));

	if ((err = GetIfTable(ifTable.get(), &ifSize, FALSE)) != NO_ERROR)
		throw ProbeException("GetIfTable() error: " +
			WindowsCommon::GetErrorMessage(err));

	for (DWORD ifIdx = 0; ifIdx < ifTable->dwNumEntries; ++ifIdx) {
		// for each interface, try to find a matching entry in ipTable.
		DWORD ipIdx;
		for (ipIdx = 0; ipIdx < ipTable->dwNumEntries; ++ipIdx)
			if (ipTable->table[ipIdx].dwIndex == ifTable->table[ifIdx].dwIndex)
				break;

		Item *item = CreateItem();
		item->SetStatus(OvalEnum::STATUS_EXISTS);
		interfaces->push_back(item);

		PMIB_IFROW ifRow = &ifTable->table[ifIdx];
		item->AppendElement(new ItemEntity("name",
			string((char*)&ifRow->bDescr[0], ifRow->dwDescrLen)));
		item->AppendElement(new ItemEntity("index", 
			Common::ToString(ifRow->dwIndex), OvalEnum::DATATYPE_INTEGER));
		item->AppendElement(new ItemEntity("type", 
			InterfaceProbe::GetInterfaceType(ifRow->dwType)));

		// MAC addresses don't apply to non-ethernet interfaces.
		if (ifRow->dwType == IF_TYPE_ETHERNET_CSMACD) {
			ostringstream macOss;
			macOss << hex << uppercase << setfill('0');
			if (ifRow->dwPhysAddrLen > 0)
				macOss << setw(2) << (int)ifRow->bPhysAddr[0];
			for (DWORD i = 1; i < ifRow->dwPhysAddrLen; ++i)
				macOss << '-' << setw(2) << (int)ifRow->bPhysAddr[i];

			item->AppendElement(new ItemEntity("hardware_addr", macOss.str()));
		} else
			item->AppendElement(new ItemEntity("hardware_addr", "",
				OvalEnum::DATATYPE_STRING,
				OvalEnum::STATUS_DOES_NOT_EXIST));

		// Not all interfaces have IPv4 addresses.  So
		// set status="does not exist" on all the IP-related
		// entities if we don't have one.  (This probe
		// doesn't yet support IPv6.)
		if (ipIdx < ipTable->dwNumEntries) {
			PMIB_IPADDRROW ipRow = &ipTable->table[ipIdx];
			item->AppendElement(new ItemEntity("inet_addr",
				dwordIpToString(ipRow->dwAddr)));
			item->AppendElement(new ItemEntity("broadcast_addr",
				dwordIpToString(ipRow->dwAddr | ~ipRow->dwMask)));
			item->AppendElement(new ItemEntity("netmask",
				dwordIpToString(ipRow->dwMask)));
			auto_ptr<StringVector> addrType(
				InterfaceProbe::GetInterfaceAddressType(ipRow->wType));
			if (addrType->empty())
				item->AppendElement(new ItemEntity("addr_type", "",
				OvalEnum::DATATYPE_STRING,
				OvalEnum::STATUS_DOES_NOT_EXIST));
			else
				for (StringVector::iterator it = addrType->begin();
					it != addrType->end();
					it++)
					item->AppendElement(new ItemEntity("addr_type", *it));
		} else {
			item->AppendElement(new ItemEntity("inet_addr", "",
				OvalEnum::DATATYPE_STRING,
				OvalEnum::STATUS_DOES_NOT_EXIST));
			item->AppendElement(new ItemEntity("broadcast_addr", "",
				OvalEnum::DATATYPE_STRING,
				OvalEnum::STATUS_DOES_NOT_EXIST));
			item->AppendElement(new ItemEntity("netmask", "",
				OvalEnum::DATATYPE_STRING,
				OvalEnum::STATUS_DOES_NOT_EXIST));
			item->AppendElement(new ItemEntity("addr_type", "",
				OvalEnum::DATATYPE_STRING,
				OvalEnum::STATUS_DOES_NOT_EXIST));
		}
	}

	this->interfaces = interfaces.release();
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
        case MIB_IF_TYPE_PPP:
            interfaceType = "MIB_IF_TYPE_PPP";
            break;
        case MIB_IF_TYPE_SLIP:
            interfaceType = "MIB_IF_TYPE_SLIP";
            break;
        case MIB_IF_TYPE_TOKENRING:
            interfaceType = "MIB_IF_TYPE_TOKENRING";
            break;
        case MIB_IF_TYPE_OTHER:
        default:
            interfaceType = "MIB_IF_TYPE_OTHER";
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

void InterfaceProbe::DeleteInterfaces() {
    if ( this->interfaces != NULL ) {
        ItemVector::iterator it;

        for ( it = this->interfaces->begin() ; it != this->interfaces->end() ; it++ ) {
            if ( ( *it ) != NULL ) {
                delete ( *it );
                ( *it ) = NULL;
            }
        }

        delete this->interfaces;
        this->interfaces = NULL;
    }

    return;
}

namespace {
	string dwordIpToString(DWORD addr) {
		in_addr inaddr;
		inaddr.s_addr = addr;
		return inet_ntoa(inaddr);
	}
}