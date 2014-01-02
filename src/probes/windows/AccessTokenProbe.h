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
#ifndef ACCESSTOKENPROBE_H
#define ACCESSTOKENPROBE_H

#include <string>

#include "AbsProbe.h"
#include "Item.h"
#include "Object.h"

/**
	This class is responsbile for collecting registry information for windows registry_objects.
*/
class AccessTokenProbe : public AbsProbe {
public:
	virtual ~AccessTokenProbe();

	/** Run the access token probe. 
		Return a vector of Items. 

		Behaviors:
		The following behaviors are supported when collecting Access Token objects:
		 - include_group: should be paired with the "resolve_group" behavior.
		   When true, include the group in the set of sids. When false, do not
		   include the group in the set of sids.
	
		 - resolve_group: when true, if the trustee name specifies a group 
		   then return all users in the group. When false just return sid 
		   for the group.
	
	*/
	virtual ItemVector* CollectItems(Object* object);

	/** Ensure that the AccessTokenProbe is a singleton. */
	static AbsProbe* Instance();
	

private:
	AccessTokenProbe();

    /** Return a new Item created for storing accesstoken information. */
	virtual Item* CreateItem();

	static AccessTokenProbe* instance;

	/** Given an account name, gather information about it. 
		Currently always returns true.
	*/
	bool GetAccountInformation(std::string accountNameIn,  bool resolveGroupBehavior, bool includeGroupBehavior, ItemVector* items);
};

#endif
