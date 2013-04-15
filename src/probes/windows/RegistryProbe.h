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
#ifndef REGISTRYPROBE_H
#define REGISTRYPROBE_H

#pragma warning(disable:4786)
#include "RegistryFinder.h"
#include "AbsProbe.h"
#include <windows.h>

/**
	This class is responsible for collecting registry information for windows registry_objects.
*/
class RegistryProbe : public AbsProbe {
public:
	/** RegistryProbe destructor. */
	virtual ~RegistryProbe();
	
	/** Run the registry probe. Return a vector of Items. */
	virtual ItemVector* CollectItems(Object* object);

	/** Ensure that the RegistryProbe is a singleton. */
	static AbsProbe* Instance();
	
private:
	/** RegistryProbe constructor. */
	RegistryProbe();

	/** Return a new Item created for storing registry information. */
	virtual Item* CreateItem();

	/** Return a new Item containing the data for the specified registry item.
	 *  @param hive A string that contains the hive of the registry key that you want to obtain.
	 *  @param key A string that contains the key of the registry key that you want to obtain.
     *  @param name A string that contains the name of the registry key value that you want to obtain.
     *  @return The item that represents the registry key that uses the specified hive, key, and name.
     */
	Item *GetRegistryKey(std::string hive, std::string key, std::string name, RegistryFinder &registryFinder);

	/** Convert the registry data to string representations and add to the provided Item.
	 *  @param hiveIn A string that contains the hive of the registry key that you want to obtain information for.
	 *  @param keyIn A string that contains the key of the registry key that you want to obtain information for.
     *  @param nameIn A string that contains the name of the registry key value that you want to obtain information for.
	 *  @param typeIn A DWORD value that specifies the type of the registry key value.
	 *  @param valueIn A LBYTE value that contains the value of the registry key value.
	 *  @param valuelenIn DWORD value that contains the length of the registry key value.
	 *  @param item A item that is used to store the registry data.
     *  @return Void.
     */
	void RetrieveInfo(std::string hiveIn, std::string keyIn, std::string nameIn, DWORD typeIn, LPBYTE valueIn, DWORD valuelenIn, Item* item);

	/** The static instance of the RegistryProbe.
	*  All Probes are singletons. The ProbeFactory is responsible for managing instances of Probes.
	*/
	static RegistryProbe* instance;
};

#endif
