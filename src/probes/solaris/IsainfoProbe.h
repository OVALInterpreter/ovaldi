//
//
//****************************************************************************************//
// Copyright (c) 2002-2012, The MITRE Corporation
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

#ifndef ISAINFOPROBE_H
#define ISAINFOPROBE_H

#include "AbsProbe.h"
#include "Item.h"
#include "Object.h"

/**
 * Probe for getting instruction set architecture (ISA) information.
 */
class IsainfoProbe : public AbsProbe {
public:
	virtual ~IsainfoProbe();
	
	virtual ItemVector* CollectItems(Object* object);
		
	/** Ensure that the IsainfoProbe is a singleton. */
	static AbsProbe* Instance();

private:
	IsainfoProbe();

	/** Return a new Item created for storing uname information */
	virtual Item* CreateItem();

	/**
	 * Calls the sysinfo() function with the given command. A
	 * string is returned with the results.  It must be freed
	 * with free().  If sysinfo() returned an error value,
	 * NULL is returned.
	 * \param cmd the command describing what info to get
	 * \return the results of the call, or NULL
	 */
	char* RunSysinfo(int cmd);
	static IsainfoProbe *instance;
};

#endif
