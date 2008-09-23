//
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

#ifndef FILEEFFECTIVRIGHTS53PROBE_H
#define FILEEFFECTIVRIGHTS53PROBE_H

#pragma warning(disable:4786)

#include "FileFinder.h"
#include "AbsProbe.h"
#include "WindowsCommon.h"

#include <aclapi.h>
#include <windows.h>

using namespace std;


/**
	This class is responsible for collecting file information for windows fileeffictiverights_objects.
*/
class FileEffectiveRights53Probe : public AbsProbe {

public:
	~FileEffectiveRights53Probe();
	
	ItemVector* CollectItems(Object* object);

	/** Return a new Item created for storing file information */
	Item* CreateItem();

	/** Ensure that the FileEffectiveRights53Probe is a singleton. */
	static AbsProbe* Instance();

private:
	FileEffectiveRights53Probe();

	static FileEffectiveRights53Probe* instance;

	/** Get the effective rights for a trustee SID for the specified file. */
	Item* GetEffectiveRights(string path, string fileName, string trusteeSID);

	/** Identify all trustee SIDs for the specified trusteeSID ObjectEntity. */ 
	StringSet* GetTrusteeSIDs(string path, string fileName, ObjectEntity* trusteeSID,  bool resolveGroupBehavior, bool includeGroupBehavior);

	/** Search all trustee SIDs on the the specifed file and return the set of SIDs the match the specified criteria. */
	void GetMatchingTrusteeSIDs(string trusteeSIDPattern, StringVector* allTrusteeSIDs, StringSet* trusteeSIDs, bool isRegex = true);

	/** Return true if the specfied trustee SID exists in the set of trustee SIDs */
	bool TrusteeSIDExists(string trusteeSID, StringVector* trusteeSIDs);

	/** Get the account name, domain, and sid string for the specified SID. */
	//void GetTrusteeInfoForSid(PSID psid, string* trusteeName, string* trusteeDomain, string* trusteeSid);

	// 8/28/2006
	//PSID GetSIDForTrusteeName(string trusteeName);

	Item* GetEffectiveRightsForSID(string path, string fileName, PSID psid);

	/** Return true if the calling probe should report that the trustee SID does not exist.
		If a trustee SID's operator is set to OPERATOR_EQUALS and the trustee SID does not exist
		the caller should report that the trustee SID was not found. When getting the value of the
		trustee SID to check existence need to look for either a simple element value or a variable
		with one or more values. If the return value is true the trusteeSIDs StringVector* paremeter
		contains the set of trustee SIDs to report as not existing. Otherwise the the trusteeSIDs 
		paramter is NULL. The caller is responsible for making sure that any memory allocated for the
		trusteeSIDs parameter is cleaned up. The trusteeSIDs paramter should be input as NULL when the
		function is called.
	*/
	bool ReportTrusteeSIDDoesNotExist(ObjectEntity* trusteeSID, StringVector* trusteeSIDs);
};

#endif