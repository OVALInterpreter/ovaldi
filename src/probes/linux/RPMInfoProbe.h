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

#ifndef RPMInfoProbe_H
#define RPMInfoProbe_H

#include "AbsProbe.h"

#include <rpm/rpmlib.h>
#include <rpm/rpmio.h>
#include <rpm/rpmts.h>
#include <rpm/rpmdb.h>
#include <rpm/header.h>
#include <rpm/rpmcli.h> // added for rpm query function
#include <rpm/rpmds.h> // added for rpm query function
#include <popt.h> // added for rpm query function

#include <string>

/**
	Data collector for rpm info test.
*/
class RPMInfoProbe : public AbsProbe {
public:
	virtual ~RPMInfoProbe();       

	virtual ItemVector* CollectItems(Object* object);

	/** Ensure that the RPMInfoProbe is a singleton. */
	static AbsProbe* Instance();

private:
	RPMInfoProbe();
    
	virtual Item* CreateItem();

	/**
		Return the name of all rpms on the system that match the specified Object entity's criteria.
		All names that are returned have been checked and exist on the system.
		@param[in] name an ObjectEntity* that represents the objects to collect on the ssytem
		@param[out] names The vector to which the names are appended.
		@return The matching names
	*/
	void GetRPMNames(ObjectEntity* name, StringVector *names);

	/**
		Get all rpm names on the system that match the specified pattern.
		@param[in] pattern a string used that rpm names are compared against.
		@param[in] isRegex a bool that is indicates how system rpm names should
		be compared against the specified pattern
		@param[out] names The vector to which the names are appended.
	*/
	void GetMatchingRPMNames(std::string pattern, bool isRegex, StringVector *names);

	/**
		Return true if the specifeid rpm exists on the system.
		@param name a string that hold the name of the rpm to check for.
		@result The result of checking for the specified rpm on the system.
	*/
	bool RPMExists(std::string name);

	/**
		Get all the information for the named rpm.
		@param[in] name a string representing the name of an rpm on the system.
		@param[out] items The vector to which the item is appended.
		@param[in] beh The list of behaviors associated with the rpm check.
	*/
	void GetRPMInfo(std::string name, ItemVector* items, BehaviorVector* beh);

	/**
	 * Gets the signature_keyid entity value of the given RPM.  If
	 * one is not found, an empty string is returned.
	 */
	std::string GetSigKeyId(std::string rpmName);

	/**
	 * Reads a string value from the given header.  val receives the result.  If
	 * the header is not found, false is returned and val is not changed.
	 */
	bool readHeaderString(Header header, int_32 tag_id, std::string *val);
	int_32 readHeaderInt32(Header header, int_32 tag_id);

	static RPMInfoProbe *instance;
};

#endif

