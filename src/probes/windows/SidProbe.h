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
#ifndef SIDPROBE_H
#define SIDPROBE_H

#include <string>

#include "AbsProbe.h"

/**
	This class is responsible for collecting information for windows sid_objects.
*/
class SidProbe : public AbsProbe {
public:
	virtual ~SidProbe();
	
	virtual ItemVector* CollectItems(Object* object);

	/** Ensure that the SidProbe is a singleton. */
	static AbsProbe* Instance();

private:
	SidProbe();

	static SidProbe* instance;

	/** Return a new Item created for storing sid information. */
	virtual Item* CreateItem();

	/** Get account information for the specified account name.
		Support behaviors. Resulting Items are pushed on to the 
		items vector.
		Return false if the set of items in not complete.

		TODO: how can I set the colleced object flag correctly?
	*/
	bool GetAccountInformation(std::string accountName,  bool resolveGroupBehavior, bool includeGroupBehavior, ItemVector* items);

};

#endif
