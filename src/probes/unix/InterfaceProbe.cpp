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

// <net/if.h> doesn't compile without including <sys/socket.h>
// first, on MacOS.
#ifdef DARWIN
#  include <sys/socket.h>
#endif

#include <net/if.h>
#include <arpa/inet.h>
#include <list>

#include <Common.h>
#include <Log.h>
#include <NetworkInterfaces.h>

#include "InterfaceProbe.h"

using namespace std;
using NetworkInterfaces::Interface;

namespace {
	/**
	 * Returns the OVAL enumerator value for the given interface type.
	 */
	string InterfaceType2String(NetworkInterfaces::Interface::LinkType linkType);

	/**
	 * Checks the given flag bits and creates item entities depending on how
	 * they're set.
	 */
	ItemEntityVector ProcessFlags(short flags);
}

// the static fields
InterfaceProbe* InterfaceProbe::instance = NULL;

InterfaceProbe::InterfaceProbe() {
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

	ItemVector *collectedItems = new ItemVector();	

	for (ItemVector::iterator iter = this->interfaces.begin();
		 iter != this->interfaces.end();
		 ++iter) {
		if (object->Analyze(*iter))
			collectedItems->push_back(new Item(**iter));
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

void InterfaceProbe::GetAllInterfaces() {

	list<Interface> allInterfaces = NetworkInterfaces::GetAllInterfaces();
	for (list<Interface>::iterator iter = allInterfaces.begin();
		 iter != allInterfaces.end();
		 ++iter) {

		string type = InterfaceType2String(iter->GetType());

		Item *item = this->CreateItem();
		item->SetStatus(OvalEnum::STATUS_EXISTS);
		item->AppendElement(new ItemEntity("name", iter->GetName(), 
										   OvalEnum::DATATYPE_STRING));
		item->AppendElement(new ItemEntity("type", type));

		if (iter->GetType() == Interface::ETHERNET)
			item->AppendElement(new ItemEntity("hardware_addr", iter->GetHwAddr()));
		else
			item->AppendElement(new ItemEntity("hardware_addr", "", 
											   OvalEnum::DATATYPE_STRING,
											   OvalEnum::STATUS_DOES_NOT_EXIST));

		item->AppendElement(new ItemEntity("inet_addr", inet_ntoa(iter->GetIPAddr())));

		// if not broadcasting, there is no broadcast address.
		if (iter->GetFlags() & IFF_BROADCAST)
			item->AppendElement(new ItemEntity("broadcast_addr", 
											   inet_ntoa(iter->GetBroadAddr())));
		else
			item->AppendElement(new ItemEntity("broadcast_addr", "", 
											   OvalEnum::DATATYPE_STRING,
											   OvalEnum::STATUS_DOES_NOT_EXIST));

		item->AppendElement(new ItemEntity("netmask", inet_ntoa(iter->GetNetmask())));

		ItemEntityVector flagEntities = ProcessFlags(iter->GetFlags());
		for (ItemEntityVector::iterator flagIter = flagEntities.begin();
			 flagIter != flagEntities.end();
			 ++flagIter)
			item->AppendElement(*flagIter);

		this->interfaces.push_back(item);
	}
}

namespace {
	string InterfaceType2String(Interface::LinkType linkType) {
	
		switch (linkType) {
		case Interface::ETHERNET:
			return "ARPHRD_ETHER";
		case Interface::FDDI:
			return "ARPHRD_FDDI";
		case Interface::PPP:
			return "ARPHRD_PPP";
		case Interface::SLIP:
			return "ARPHRD_SLIP";
		case Interface::LOOPBACK:
			return "ARPHRD_LOOPBACK";
		case Interface::PRONET:
			return "ARPHRD_PRONET";
		case Interface::UNKNOWN:
			return "ARPHRD_VOID";
		default:
			Log::Debug("Unhandled interface type: " + Common::ToString(linkType));
			return "ARPHRD_VOID";
		}
	}

	ItemEntityVector ProcessFlags(short flags) {
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

#ifdef IFF_MASTER
		if (flags & IFF_MASTER)
			entities.push_back(new ItemEntity("flag", "MASTER"));
#endif

#ifdef IFF_SLAVE
		if (flags & IFF_SLAVE)
			entities.push_back(new ItemEntity("flag", "SLAVE"));
#endif

		if (flags & IFF_MULTICAST)
			entities.push_back(new ItemEntity("flag", "MULTICAST"));

#ifdef IFF_PORTSEL
		if (flags & IFF_PORTSEL)
			entities.push_back(new ItemEntity("flag", "PORTSEL"));
#endif

#ifdef IFF_AUTOMEDIA
		if (flags & IFF_AUTOMEDIA)
			entities.push_back(new ItemEntity("flag", "AUTOMEDIA"));
#endif

#ifdef IFF_DYNAMIC
		if (flags & IFF_DYNAMIC)
			entities.push_back(new ItemEntity("flag", "DYNAMIC"));
#endif

		return entities;
	}
}
