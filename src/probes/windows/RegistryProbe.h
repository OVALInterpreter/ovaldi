//
//
//****************************************************************************************//
// Copyright (c) 2002-2009, The MITRE Corporation
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

#include "AbsProbe.h"
#include "WindowsCommon.h"

#include <windows.h>

using namespace std;

/**
	This class is responsbile for collecting registry information for windows registry_objects.
*/
class RegistryProbe : public AbsProbe {
public:
	~RegistryProbe();
	
	/** Run the registry probe. Return a vector of Items. */
	ItemVector* CollectItems(Object* object);

	/** Ensure that the RegistryProbe is a singleton. */
	static AbsProbe* Instance();
	

private:
	RegistryProbe();

	static RegistryProbe* instance;

	/** Return a new Item created for storing registry information. */
	Item* CreateItem();

	/** Return the set of hives to be collected. 
	    If the operation is set to equals simply return the hive as an ItemEntity.
	*/
	ItemEntityVector* GetHives(ObjectEntity* hive);
	/** Return the set of keys to be collected. 
		If the operation is set to equals simply return the key as an ItemEntity.
	*/
	ItemEntityVector* GetKeys(ObjectEntity* key, ItemEntity* hive);
	/** Return the set of names to be collected. 
		If the operation is set to equals simply return the name as an ItemEntity.
	*/
	ItemEntityVector* GetNames(ObjectEntity* name, ItemEntity* hive, ItemEntity* key);
	
	/** Gather new data and put in a Item object.
		Return NULL if the key is not found.
	*/
	Item* GetRegistryKey(ItemEntity*, ItemEntity*, ItemEntity*);
	/** Return a handle to a predefined root key on the system.
		An application must open a key before it can add data to the registry. To open a
		key, an application must supply a handle to another key in the registry that is
		already open. The system defines predefined keys that are always open.  Determine
		if one of these predefined keys can be used, and if so, return it.
	
		NOTE: We have used _strnicmp() instead of the string compare() function as we need
		to do a compare without regard to case.
	*/
	HKEY GetRootKey(string);
	/** Return a vector of hives as DefinitionEntities that match the pattern 
		Supported hives:
		"HKEY_CLASSES_ROOT"
		"HKEY_CURRENT_CONFIG"
		"HKEY_CURRENT_USER"
		"HKEY_LOCAL_MACHINE"
		"HKEY_USERS"
		May throw REGEXException if the pattern is invalid.
	*/
	void GetMatchingHives(string, ItemEntityVector*, bool isRegex = true);
	/** Search the registry for keys under the designated hive and input key 
		that match the pattern. Add all matches to the keys vector. For every sub key
		found make recursive call. Stop when all subkeys have been searched.
	*/
	void GetMatchingKeys(string hiveIn, string keyIn, string pattern, ItemEntityVector* keys, bool isRegex = true);
	/** Search the registry for names under the designated hive and input key 
		that match the pattern. Add all matches to the names vector. 
		Stop when all names have been searched
	*/
	void GetMatchingNames(string hiveIn, string keyIn, string pattern, ItemEntityVector* names, bool isRegex = true);
	/** Get keys foir the specifed regex.
		Attempt to locate any constant portion of the registry key. If a constant
		portion can be found then start the pattern matching search from the end of the 
		constant portion to save time. This cuts the time to 1/20th of the old time, nad
		reduces the memory usage of this probe to 1/30th of the old memory usage.
	*/
	void KeyPatternMatch(ItemEntity* hive, string pattern, ItemEntityVector* keys, bool isRegex = true);
	/** Convert the registry data to string representations and add to the provided Item. */
	void RetrieveInfo(string, string, string, DWORD, LPBYTE, DWORD, Item*);

	/** Return true if the specified key exists under the specified hive. */
	bool KeyExists(string hive, string key);
	/** Return true if the specified name exists under the specified hive and key. */
	bool NameExists(string hive, string key, string name);
};

#endif
