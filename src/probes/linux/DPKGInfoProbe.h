//
// $Id: DPKGInfoProbe.h 4194 2007-07-03 15:33:35Z blaze $
//
//****************************************************************************************//
// Copyright (c) 2002-2008, The MITRE Corporation
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

#ifndef DPKGInfoProbe_H
#define DPKGInfoProbe_H

#include "AbsProbe.h"

#include <apt-pkg/progress.h> 	//OpTextProgress
#include <apt-pkg/cachefile.h> 	//pkgCacheFile
#include <apt-pkg/configuration.h> //_config
#include <apt-pkg/tagfile.h>	//pkgTagFile, pkgTagSection 
#include <apt-pkg/error.h> 	//_error

#include <unistd.h>
#include <sys/types.h>
#include <sys/termios.h>
#include <sys/wait.h>

#include <fcntl.h>

#include <iostream>
#include <string>
#include <vector>

using namespace std;
/**
	Data collector for dpkg info test.
*/
class DPKGInfoProbe : public AbsProbe {
public:
	~DPKGInfoProbe();       

	ItemVector* CollectItems(Object* object);
	Item* CreateItem();

	/** Ensure that the DPKGInfoProbe is a singleton. */
	static AbsProbe* Instance();

private:
	string StatusFile;

	DPKGInfoProbe();

	/**
		Return the name of all debs on the system that match the specirfied Object entity's criteria.
		All names that are return have been checked and exist on the system.
		@param name an ObjectEntity* that represents the objects to collect on the system
		@return The matching names
	*/
	StringVector* GetDPKGNames(ObjectEntity* name);

	/**
		Get all deb names on the system that match the specified pattern.
		@param pattern a string used that deb names are compared against.
		@param isRegex a bool that is indicates how system deb names should be compared against the specifed pattern
		@return The set of matching names.
	*/
	StringVector* GetMatchingDPKGNames(string pattern, bool isRegex);

	/**
		Return true if the specifeid deb exists on the system.
		@param name a string that hold the name of the deb to check for.
		@result The result of checking for the specifed deb on the system.
	*/
	bool DPKGExists(string name);

	/**
		Get all the information for the named deb.
		@param name a string representing the name of an deb on the system.
		@param items a vector of items that matched the deb name.
	*/
	void GetDPKGInfo(string name, ItemVector* items);

	string readHeaderString(pkgTagSection section, char* sectionName);

	static DPKGInfoProbe *instance;
};

#endif

