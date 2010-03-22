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
#ifndef INTERFACEPROBE_H
#define INTERFACEPROBE_H

#include "AbsProbe.h"
#include "Item.h"
#include "ItemEntity.h"
#include "Common.h"
#include "Log.h"

#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>

#ifdef DARWIN
#include <sys/sysctl.h>
#include <net/if_types.h>
#include <net/if_dl.h>
#else
#include <net/if_arp.h>
#endif

/**
	This class is responsible for collecting information about unix interface_objects.
*/
class InterfaceProbe : public AbsProbe {

	public:

	virtual ~InterfaceProbe();

	/** Get all the files on the system that match the pattern and collect their attributes. */
	virtual ItemVector* CollectItems(Object* object);

	/** Ensure that the InterfaceProbe is a singleton. */
	static AbsProbe* Instance();

	private:

	InterfaceProbe();

	/** Return a new Item created for storing file information */
	virtual Item* CreateItem();

	/**
	 * Gets an Item for the named interface, or NULL if no
	 * interface was found with that name.  This Item should
	 * not be deleted; it is a member of a shared cache of
	 * interface items.
	 */
	Item* GetInterface(const std::string &name);

	/**
	 * Caches info about all of the interfaces on the local system.
	 */
	void GetAllInterfaces();

	/**
	 * Gets the names of all network interfaces on the system.
	 */
	StringVector GetInterfaceNames();

	/** Populates the map containing hardware type names. */
	void SetupHardwareTypes();

	/**
	 * Gets the OVAL hardware type name for the given value, or ""
	 * if one is not defined.
	 */

	#ifdef DARWIN
	std::string GetHardwareTypesFromIft(unsigned char type);
	#endif
	
	std::string HardwareTypeToString(sa_family_t hwFamily);
	
	/**
	 * Creates ItemEntity objects for the given flags.
	 */
	ItemEntityVector ProcessFlags(short flags);

	/** A vector that holds cached information about all interfaces on the system */
	ItemVector interfaces;

	typedef std::map<sa_family_t,std::string> HardwareTypeNameMap;

	/** Maps hardware type constants to names */
	HardwareTypeNameMap hardwareTypeNameMap;

	/** Singleton instance */
	static InterfaceProbe* instance;
};

#endif
