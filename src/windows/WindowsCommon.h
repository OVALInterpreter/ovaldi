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

#ifndef WINDOWSCOMMON_H
#define WINDOWSCOMMON_H

#define STRNICMP _strnicmp

#pragma warning(disable:4786)

#include "WindowsCommon.h"
#include "Exception.h"
#include "Log.h"

#include <aclapi.h>
#include <lm.h>
#include <Ntsecapi.h>
#include <windows.h>
#define _WIN32_WINNT 0x0501
#include <Sddl.h>

using namespace std;

class WindowsCommon {

public:
	/** Disable all the privileges associated with the current process token.
		If a specific privilege is needed later, it can be enabled by calling
		AdjustTokenPrivileges() again.
	*/
	static bool DisableAllPrivileges();

	/** Enable the specified privilege. */
	static bool EnablePrivilege(string);

	/** Convert the provded PSID to a string in the SID string format. */
	static bool GetTextualSid(PSID, LPTSTR*);

	/** Return the string error massge for the specified error code. */
	static string GetErrorMessage(DWORD dwLastError);

	/** Expand the group returning all members. 
		If the group does not exist return false 
		Look at local groups then global groups
	*/
	static bool ExpandGroup(string groupName, StringVector* members);

	/** Get all trustee names on the system. 
		This set consists of the following:
		Get all local user trustee names
		Get all local groups
			- expand to get all members
		Get all global groups
			- expand to get all members
		Get the trustee name of the system.
		Resulting trustee names should be unique.
	*/
	static StringVector* GetAllTrusteeNames();

	/** Get the trustee name for the specified sid formatted for oval useage. */
	static string GetFormattedTrusteeName(PSID pSid);

	/** Get the SID for the specified trustee name. 
		TrusteeName should be a fully qualified account name. 
		For more info see:
		http://msdn.microsoft.com/library/default.asp?url=/library/en-us/secauthz/security/lookupaccountname.asp
	*/
	static PSID GetSIDForTrusteeName(string trusteeName);

	/** Get the domain and sid string for the specifeid trustee name. Return true if the trustee is a group. */
	static bool LookUpTrusteeName(string* accountNameStr, string* sidStr, string* domainStr);

	/** Return the set of all local and global groups on the local system. */
	static StringVector* GetAllGroups();

	/** Get the set of all local users. */
	static void GetAllLocalUsers(UniqueStringVector*);

	/** Convert the FILETIME strucutre to an integer. */
	static string ToString(FILETIME fTime);

	/** Return a string representation of the DWORD */
	static string ToString(DWORD dw);

private:

	/** Return a StringVector* of all local groups. */
	static StringVector* GetAllLocalGroups();

	/** Return a StringVector* of all global groups. */
	static StringVector* GetAllGlobalGroups();

	/** Get the members of the specified local group. */
	static bool GetLocalGroupMembers(string groupName, StringVector* members);

	/** Get the members of the specified global group. 	*/
	static bool GetGlobalGroupMembers(string groupName, StringVector* members);

	/** Get the set of all trustee names on the system for the well knowns SIDS.
		These are sids that are not returend by a call to NetUserEnum, 
		NetGroupEnum, or NetLocalGroupEnum.
		Selected well known sids are pulled from:
		http://support.microsoft.com/kb/243330
	*/
	static void GetWellKnownTrusteeNames();

	/** Look up the local system name. */
	static string WindowsCommon::LookUpLocalSystemName();

	static StringVector* allTrusteeNames;
	static StringVector* wellKnownTrusteeNames;
};

#endif
