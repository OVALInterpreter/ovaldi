//
//
//****************************************************************************************//
// Copyright (c) 2002-2010, The MITRE Corporation
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

#include "Log.h"
#include <stdlib.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <netinet/in.h>
#include <sys/ioctl.h>

using namespace std;

// the static fields
InterfaceProbe* InterfaceProbe::instance = NULL;

InterfaceProbe::InterfaceProbe() {
	this->SetupHardwareTypes();
}

InterfaceProbe::~InterfaceProbe() {
	// the probe factory deletes the singletons
	InterfaceProbe::instance = NULL;

	// Delete all our cached data when the singleton is destroyed
	for (ItemVector::iterator iter = this->interfaces.begin();
		 iter != this->interfaces.end();
		 ++iter)
		delete *iter;
}

ItemVector* InterfaceProbe::CollectItems(Object* object) {

	// Get the name from the provided object
	ObjectEntity* name = object->GetElementByName ( "name" );

	// Check datatypes - only allow string
	if ( name->GetDatatype() != OvalEnum::DATATYPE_STRING ) {
		throw ProbeException ( "Error: Invalid data type specified on name. Found: " + OvalEnum::DatatypeToString ( name->GetDatatype() ) );
	}

	// Check operation - only allow  equals, not equals and pattern match
	if ( name->GetOperation() != OvalEnum::OPERATION_EQUALS && name->GetOperation() != OvalEnum::OPERATION_PATTERN_MATCH && name->GetOperation() != OvalEnum::OPERATION_NOT_EQUAL ) {
		throw ProbeException ( "Error: Invalid operation specified on name. Found: " + OvalEnum::OperationToString ( name->GetOperation() ) );
	}

	ItemVector *collectedItems = new ItemVector();

	if ( name->GetVarRef() == NULL ) {
		if ( name->GetOperation() == OvalEnum::OPERATION_EQUALS ) {
			Item* aInterface = this->GetInterface ( name->GetValue() );

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
			for ( it1 = this->interfaces.begin() ; it1 != this->interfaces.end() ; it1++ ) {
				ItemEntityVector* nameVector = ( *it1 )->GetElementsByName ( "name" );

				for ( it2 = nameVector->begin() ; it2 != nameVector->end() ; it2++ ) {
					string intfName = ( *it2 )->GetValue();

					if ( name->GetOperation() == OvalEnum::OPERATION_NOT_EQUAL ) {
						if ( name->GetValue().compare ( intfName ) != 0 ) {
							Item* temp = new Item ( **it1 );
							collectedItems->push_back ( temp );
						}

					} else {
						if ( this->myMatcher->IsMatch ( name->GetValue().c_str() , intfName.c_str() ) ) {
							Item* temp = new Item ( **it1 );
							collectedItems->push_back ( temp );
						}
					}
				}
			}
		}

	} else {
		// Loop through all shared interfaces on the system
		// Only keep the shared interfaces that match operation and value and var check
		ItemVector::iterator it1;
		ItemEntityVector::iterator it2;

		for ( it1 = this->interfaces.begin() ; it1 != this->interfaces.end() ; it1++ ) {
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

Item* InterfaceProbe::CreateItem() {

	Item* item = new Item(0,
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#unix",
						"unix-sc",
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#unix unix-system-characteristics-schema.xsd",
						OvalEnum::STATUS_ERROR,
						"interface_item");

	return item;
}

AbsProbe* InterfaceProbe::Instance() {
	if (InterfaceProbe::instance == NULL) {
		InterfaceProbe::instance = new InterfaceProbe();
		try {
			InterfaceProbe::instance->GetAllInterfaces();
		} catch(ProbeException e) {
			// initialization of our singleton failed, so lets
			//   delete it.
			delete InterfaceProbe::instance;
			InterfaceProbe::instance = NULL;

			// Add a more helpful prefix to the error message
			throw ProbeException(string("Error getting network interfaces: ") +
								 e.GetErrorMessage(), e.GetSeverity(), e.GetCause());
		}
	}

	return InterfaceProbe::instance;
}

Item* InterfaceProbe::GetInterface(const string &name) {
	for(ItemVector::iterator iter = this->interfaces.begin();
		iter != this->interfaces.end();
		++iter) {

		ItemEntity *entity = (*iter)->GetElementByName("name");

		// Item::GetElementByName() doesn't ever return NULL.  It
		// returns a default-constructed ItemEntity if no element
		// with the given name was found.  So I will just
		// check for an empty name instead...
		if (entity->GetName().empty())
			throw ProbeException("Encountered interface_item without a name entity!");

		if (entity->GetValue() == name)
			return *iter;
	}

	return NULL;
}

void InterfaceProbe::GetAllInterfaces() {
	StringVector interfaceNames = GetInterfaceNames();

	struct ifreq req;
	int result;

	short flags = 0;           // the type of struct ifreq.ifr_flags
	struct sockaddr hwAddr;    // the type of struct ifreq.ifr_hwaddr
	struct sockaddr ipAddr;    // the type of struct ifreq.ifr_addr
	struct sockaddr broadAddr; // the type of struct ifreq.ifr_broadaddr
	struct sockaddr netmask;   // the type of struct ifreq.ifr_netmask

	ItemEntity *nameEntity = NULL, *hwTypeEntity = NULL, *hwAddrEntity = NULL,
		*ipAddrEntity = NULL, *broadAddrEntity = NULL, *netmaskEntity = NULL;
	ItemEntityVector flagEntities;

	// All the ioctl() calls we make require a real socket to work off of.
	int s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s<0)
		throw ProbeException(string("Can't create socket: ")+strerror(errno));

	for (StringVector::iterator iter = interfaceNames.begin();
		 iter != interfaceNames.end();
		 ++iter) {

		Log::Debug(string("Querying interface ") + (*iter) + "...");

		// error messages below will have this common prefix.
		string errorMsgPrefix = string("Error querying network interface ")+*iter+": ";

		// must create the Item here so it can accumulate error messages as we do the probing.
		Item *item = this->CreateItem();
		item->SetStatus(OvalEnum::STATUS_EXISTS); // will be reset on error

		// We already have the name... so no further work required for this entity
		nameEntity = new ItemEntity("name", *iter, OvalEnum::DATATYPE_STRING, true);

		// the ifconfig source copies the interface name in
		// before every ioctl() call.

		// get interface flags
		strcpy(req.ifr_name, iter->c_str());
		result = ioctl(s, SIOCGIFFLAGS, &req);
		if (result >= 0) {
			flags = req.ifr_flags;
			flagEntities = this->ProcessFlags(flags);
		} else {
			ItemEntity *tmpEntity = new ItemEntity("flag", "", OvalEnum::DATATYPE_STRING,
												   false, OvalEnum::STATUS_ERROR);
			flagEntities.push_back(tmpEntity);
			item->AppendMessage(new OvalMessage(errorMsgPrefix + "ioctl SIOCGIFFLAGS: " + strerror(errno),
												OvalEnum::LEVEL_ERROR));
			item->SetStatus(OvalEnum::STATUS_ERROR);
		}

		// hardware address/type
		strcpy(req.ifr_name, iter->c_str());
		result = ioctl(s, SIOCGIFHWADDR, &req);
		if (result >= 0) {

			hwAddr = req.ifr_hwaddr;

			sa_family_t hwType = hwAddr.sa_family;

			string hwTypeStr = this->HardwareTypeToString(hwType);
			if (!hwTypeStr.empty())
				hwTypeEntity = new ItemEntity("type", hwTypeStr);

			if (hwType == ARPHRD_ETHER) {
				char *hwaddr_data = hwAddr.sa_data;

				// enough space for a mac address (6 2-char bytes, 5 separating colons and a null char)
				char macAddress[20];

				sprintf(macAddress, "%02X-%02X-%02X-%02X-%02X-%02X",
						hwaddr_data[0] & 0xFF, hwaddr_data[1] & 0xFF, hwaddr_data[2] & 0xFF,
						hwaddr_data[3] & 0xFF, hwaddr_data[4] & 0xFF, hwaddr_data[5] & 0xFF);
				hwAddrEntity = new ItemEntity("hardware_addr", macAddress);
			}

		} else {
			hwAddrEntity = new ItemEntity("hardware_addr", "", OvalEnum::DATATYPE_STRING,
										  false, OvalEnum::STATUS_ERROR);
			item->AppendMessage(new OvalMessage(errorMsgPrefix + "ioctl SIOCGIFHWADDR: " + strerror(errno),
												OvalEnum::LEVEL_ERROR));
			item->SetStatus(OvalEnum::STATUS_ERROR);
		}


		// The following requests require an ipv4 interface.  If the initial
		// ioctl fails, we won't throw an exception; we'll just leave off
		// the applicable entities.

		// see if we can get ipv4 address
		strcpy(req.ifr_name, iter->c_str());
		req.ifr_addr.sa_family = AF_INET;
		result = ioctl(s, SIOCGIFADDR, &req);
		if (result >= 0) {

			struct sockaddr_in *addrPtr;

			ipAddr = req.ifr_addr;
			addrPtr = (struct sockaddr_in *)&ipAddr;
			ipAddrEntity = new ItemEntity("inet_addr", inet_ntoa(addrPtr->sin_addr));

			// get broadcast address, if valid
			if (flags & IFF_BROADCAST) {
				strcpy(req.ifr_name, iter->c_str());
				result = ioctl(s, SIOCGIFBRDADDR, &req);

				if (result >= 0) {
					broadAddr = req.ifr_broadaddr;
					addrPtr = (struct sockaddr_in *)&broadAddr;
					broadAddrEntity = new ItemEntity("broadcast_addr", inet_ntoa(addrPtr->sin_addr));
				} else {
					broadAddrEntity = new ItemEntity("broadcast_addr", "", OvalEnum::DATATYPE_STRING,
													 false, OvalEnum::STATUS_ERROR);
					item->AppendMessage(new OvalMessage(errorMsgPrefix + "ioctl SIOCGIFBRDADDR: " + strerror(errno),
														OvalEnum::LEVEL_ERROR));
					item->SetStatus(OvalEnum::STATUS_ERROR);
				}
			}

			// get netmask
			strcpy(req.ifr_name, iter->c_str());
			result = ioctl(s, SIOCGIFNETMASK, &req);
			if (result >= 0) {
				netmask = req.ifr_netmask;
				addrPtr = (struct sockaddr_in *)&netmask;
				netmaskEntity = new ItemEntity("netmask", inet_ntoa(addrPtr->sin_addr));
			} else {
				netmaskEntity = new ItemEntity("netmask", "", OvalEnum::DATATYPE_STRING,
											   false, OvalEnum::STATUS_ERROR);
				item->AppendMessage(new OvalMessage(errorMsgPrefix + "ioctl SIOCGIFNETMASK error: "+strerror(errno),
													OvalEnum::LEVEL_ERROR));
				item->SetStatus(OvalEnum::STATUS_ERROR);
			}
		}

		// Now, add all the entities we made to our item, in the proper order
		// according to the schema.

		item->AppendElement(nameEntity);
		if (hwTypeEntity != NULL)
			item->AppendElement(hwTypeEntity);
		if (hwAddrEntity != NULL)
			item->AppendElement(hwAddrEntity);
		if (ipAddrEntity != NULL)
			item->AppendElement(ipAddrEntity);
		if (broadAddrEntity != NULL)
			item->AppendElement(broadAddrEntity);
		if (netmaskEntity != NULL)
			item->AppendElement(netmaskEntity);

		for (ItemEntityVector::iterator iter = flagEntities.begin();
			 iter != flagEntities.end();
			 ++iter)
			item->AppendElement(*iter);

		this->interfaces.push_back(item);
	}

	close(s);
}

StringVector InterfaceProbe::GetInterfaceNames() {

	// This technique was taken from the implementation of the linux
	// 'ifconfig' utility.  An initial buffer is allocated for the
	// results of an ioctl SIOCGIFCONF request and repeatedly increased
	// in size until the ioctl request does not fill it up.  That way
	// we know we got all the interfaces.

	StringVector names;

	int s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s<0)
		throw ProbeException(strerror(errno));

	struct ifconf ifc;
	ifc.ifc_buf = NULL;
	size_t recordsAllocated = 0; //how much I allocated (in records)
	size_t bytesUsed; //how much did ioctl actually use (in bytes)
	size_t recordsUsed; //how much did ioctl actually use (in records)
	do {
		recordsAllocated += 5;
		ifc.ifc_len = recordsAllocated * sizeof(struct ifreq);
		__caddr_t tmpBuf  = (__caddr_t)realloc(ifc.ifc_buf, ifc.ifc_len);

		// make sure the allocation succeeded
		if (tmpBuf == NULL) {
			if (ifc.ifc_buf != NULL)
				free(ifc.ifc_buf);
			close(s);
			throw ProbeException(string("Unable to allocate ")+Common::ToString(ifc.ifc_len) +
								 " bytes of memory for ioctl SIOCGIFCONF request!");
		}

		ifc.ifc_buf = tmpBuf;

		int res = ioctl(s, SIOCGIFCONF, &ifc);
		if (res < 0) {
			free(ifc.ifc_buf);
			close(s);
			throw ProbeException(string("ioctl SIOCGIFCONF error: ")+strerror(errno));
		}

		bytesUsed = ifc.ifc_len;
		recordsUsed = bytesUsed / sizeof(struct ifreq);
	} while(recordsUsed == recordsAllocated);

	struct ifreq *reqPtr = ifc.ifc_req;
	size_t i;
	for (i=0; i<recordsUsed; i++) {

		// From my experiments, other fields of the ifreq struct were
		// not valid.  So ioctl SIOCGIFCONF is just used to get names.

		names.push_back(reqPtr->ifr_name);
		reqPtr++;
	}

	free(ifc.ifc_buf);
	close(s);

	return names;
}

void InterfaceProbe::SetupHardwareTypes() {
	this->hardwareTypeNameMap[ARPHRD_ETHER] = "ARPHRD_ETHER";
	this->hardwareTypeNameMap[ARPHRD_FDDI] = "ARPHRD_FDDI";
	this->hardwareTypeNameMap[ARPHRD_LOOPBACK] = "ARPHRD_LOOPBACK";
	this->hardwareTypeNameMap[ARPHRD_VOID] = "ARPHRD_VOID";
	this->hardwareTypeNameMap[ARPHRD_PPP] = "ARPHRD_PPP";
	this->hardwareTypeNameMap[ARPHRD_SLIP] = "ARPHRD_SLIP";
	this->hardwareTypeNameMap[ARPHRD_PRONET] = "ARPHRD_PRONET";
}

string InterfaceProbe::HardwareTypeToString(sa_family_t hwFamily) {
	HardwareTypeNameMap::iterator iter = this->hardwareTypeNameMap.find(hwFamily);
	if (iter == this->hardwareTypeNameMap.end())
		return "";
	return iter->second;
}

ItemEntityVector InterfaceProbe::ProcessFlags(short flags) {
	ItemEntityVector entities;

	if (flags & IFF_UP)
		entities.push_back(new ItemEntity("flag", "UP"));
	if (flags & IFF_BROADCAST)
		entities.push_back(new ItemEntity("flag", "BROADCAST"));
	if (flags & IFF_DEBUG)
		entities.push_back(new ItemEntity("flag", "DEBUG"));
	if (flags & IFF_LOOPBACK)
		entities.push_back(new ItemEntity("flag", "LOOPBACK"));
	if (flags & IFF_POINTOPOINT) // yes, it's really spelled like that...
		entities.push_back(new ItemEntity("flag", "POINTOPOINT"));
	if (flags & IFF_NOTRAILERS)
		entities.push_back(new ItemEntity("flag", "NOTRAILERS"));
	if (flags & IFF_RUNNING)
		entities.push_back(new ItemEntity("flag", "RUNNING"));
	if (flags & IFF_NOARP)
		entities.push_back(new ItemEntity("flag", "NOARP"));
	if (flags & IFF_PROMISC)
		entities.push_back(new ItemEntity("flag", "PROMISC"));

	// the header file says this one not supported... won't hurt to handle it though
	if (flags & IFF_ALLMULTI)
		entities.push_back(new ItemEntity("flag", "ALLMULTI"));

	if (flags & IFF_MASTER)
		entities.push_back(new ItemEntity("flag", "MASTER"));
	if (flags & IFF_SLAVE)
		entities.push_back(new ItemEntity("flag", "SLAVE"));
	if (flags & IFF_MULTICAST)
		entities.push_back(new ItemEntity("flag", "MULTICAST"));
	if (flags & IFF_PORTSEL)
		entities.push_back(new ItemEntity("flag", "PORTSEL"));
	if (flags & IFF_AUTOMEDIA)
		entities.push_back(new ItemEntity("flag", "AUTOMEDIA"));
	if (flags & IFF_DYNAMIC)
		entities.push_back(new ItemEntity("flag", "DYNAMIC"));

	return entities;
}
