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
#ifndef SHADOWPROBE_H
#define SHADOWPROBE_H

#include "AbsProbe.h"
#include "Item.h"
#include "Object.h"
#include <shadow.h>
#include <string>
#include <map>


/**
	This class is responsible for collecting information about unix shadow_objects.
*/
class ShadowProbe : public AbsProbe {

	public:

	virtual ~ShadowProbe();

	/** Get all the files on the system that match the pattern and collect their attributes. */
	virtual ItemVector* CollectItems(Object* object);

	/** Return a new Item created for storing file information */
	virtual Item* CreateItem();

	/** Ensure that the ShadowProbe is a singleton. */
	static AbsProbe* Instance();

	private:

	ShadowProbe();

	/** Creates an item from a spwd struct */
	Item *CreateItemFromPasswd(const struct spwd *pwInfo);

	/**
	 * Attempts to determine the encryption method used on the given
	 * encrypted password.  An empty string is returned if the method
	 * could not be determined.
	 */
	std::string FindPasswordEncryptMethod(const std::string &password);

	/** Finds a single item by name. */
	Item *GetSingleItem(const std::string& username);

	/** Finds multiple items according to the given object entity. */
	ItemVector *GetMultipleItems(Object *shadowObject);

	/** Singleton instance */
	static ShadowProbe* instance;

	/**
	 * A lookup table for names of encryption methods which are specified
	 * via an encrypted password that starts with "$...$".  The value in
	 * between the dollar signs is the key lookup into this map.
	 */
	static std::map<std::string, std::string> dollarEncryptMethodTypes;

	/** Populates the #dollarEncryptMethodTypes mapping */
	static void PopulateDollarEncryptMethodTypes();
};

#endif
