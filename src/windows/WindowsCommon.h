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

#ifndef WINDOWSCOMMON_H
#define WINDOWSCOMMON_H

#define STRNICMP _strnicmp

#pragma warning(disable:4786)

#include "Exception.h"
#include "Log.h"

#include <aclapi.h>
#include <lm.h>
#include <Ntsecapi.h>
#include <windows.h>
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif
#include <Sddl.h>
#include <Authz.h>
#undef __DOMDocument_FWD_DEFINED__
#include <comdef.h>
#include <DelayImp.h>


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
		If the group does not exist return false. 
		Look at local groups then global groups.

		@param groupName The name of the group to be expanded.
		@param members A pointer to a set that will be populated with the specified group members.
		@param includeSubGroups When true include subgroups in the set of groups that are reported.
		@param resolveSubGroup When true recurse into any subgroups and expand them too.
	*/
	static bool ExpandGroup(string groupName, StringSet* members, bool includeSubGroups, bool resolveSubGroup);

	/** Return true if the group exists add all the group member's SIDs to the memberSIDs parameter. 
		
		@param groupSID The SID of the group to be expanded.
		@param memberSIDs A pointer to a set that will be populated with the specified group member SIDs.
		@param includeSubGroups When true include subgroups in the set of groups that are reported.
		@param resolveSubGroup When true recurse into any subgroups and expand them too.
	*/
	static bool ExpandGroupBySID(string groupSID, StringSet* memberSIDs, bool includeSubGroups, bool resolveSubGroup);


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
	static StringSet* GetAllTrusteeNames();

	/** Get all the Trustee SIDs on the system.
		This method simply calls the WindowsCommon::GetAllTrusteeNames() method
		and converts each returned name to a sid string. THe resulting set of sids is 
		returned as a StringSet.		
	*/
	static StringSet* GetAllTrusteeSIDs();

	/** Get the trustee name for the specified sid formatted for oval useage. */
	static string GetFormattedTrusteeName(PSID pSid);

	/** Get the SID for the specified trustee name. 
		TrusteeName should be a fully qualified account name. 
		For more info see:
		http://msdn.microsoft.com/en-us/library/aa379159.aspx
	*/
	static PSID GetSIDForTrusteeName(string trusteeName);

	/** Get the SID for the specified trustee sid string. 
		TrusteeSID is a complete sid string formated as the 
		For more info see:
		http://msdn.microsoft.com/library/default.asp?url=/library/en-us/secauthz/security/lookupaccountname.asp
	*/
	static PSID GetSIDForTrusteeSID(string trusteeSID);

	/** Get the domain and sid string for the specifeid trustee name. Return true if the trustee is a group. */
	static bool LookUpTrusteeName(string* accountNameStr, string* sidStr, string* domainStr);

	/** Get the account and domain string for the specified trustee sid. Return true if the trustee is a group. */
	static bool LookUpTrusteeSid(string sidStr, string* pAccountNameStr, string* pDomainStr);

	/** Convert a vector of trustee names to a vector of corresponding SID strings */
	static void ConvertTrusteeNamesToSidStrings(StringSet *trusteeNames, StringSet *sidStrings);

	/** Retrieves list of sids for all local users */
	static StringSet* WindowsCommon::GetAllLocalUserSids();

	/** Return true if the SID corresponds to a group. */
	static bool IsGroupSID(string sid);

	/** Returns true if the trustee name corresponds to a group. */
	static bool IsGroup(string trusteeName);

	/** Return the set of all local and global groups on the local system. */
	static StringSet* GetAllGroups();

	/** Get the set of all local users. 

		@param allUsers A pointer to variable that will be populated with all the local users.
	*/
	static void GetAllLocalUsers(StringSet* all);

	/** Get local and global groups for the user. */
	static bool GetGroupsForUser(string userName, StringSet* groups);

	/** Get 'enabled flag' for user with specified name 
		If the user name is provided with a leading doamin name or host name
		the user name is extracted and provided to the NetUserGetInfo api.
	*/
	static bool GetEnabledFlagForUser(string userNameIn);

	/** Convert the FILETIME strucutre to an integer. */
	static string ToString(FILETIME fTime);

	/** Return a string representation of the DWORD */
	static string ToString(DWORD dw);

	/** Convert the PSID to a string. 
		Attempts to return a string representation of the input PSID. If
		the PSID can not be converted to a string an empty string is returned.
	*/
	static string ToString(PSID pSid);

	/** Return the access mask for the file for the specifed SID. **/
	static void GetEffectiveRightsForFile(PSID pSid, string* filePath, PACCESS_MASK pAccessRights);

	/** Return true if the current os is Vista or later. 
		The return value is calculated once and the result is stored in a static private variable.

		See: http://msdn.microsoft.com/en-us/library/ms725491(VS.85).aspx
	**/
	static bool IsVistaOrLater();

	/** Return true if the current os is XP or later. 
		The return value is calculated once and the result is stored in a static private variable.

		See: http://msdn.microsoft.com/en-us/library/ms725491(VS.85).aspx
	**/
	static bool IsXPOrLater();

private:
	
	//static LONG WINAPI DelayLoadDllExceptionFilter(PEXCEPTION_POINTERS pExcPointers);

	/** Return the access mask for the file for the specifed SID. **/
	static void GetEffectiveRightsForFileAcl(PSID pSid, string* filePath, PACCESS_MASK pAccessRights);

	/** Return the access mask for the file for the specifed SID. **/
	static void GetEffectiveRightsForFileAuthz(PSID pSid, string* filePath, PACCESS_MASK pAccessRights);

	/** Split Trustee name between domain and account portion */
	static void SplitTrusteeName(string trusteeName, string *domainName, string *accountName);

	/** Return a StringSet* of all local groups. */
	static StringSet* GetAllLocalGroups();

	/** Return a StringSet* of all global groups. */
	static StringSet* GetAllGlobalGroups();

	/** Get the members of the specified local group. 

		@param groupName The name of the group to retrieve members for.
		@param members A pointer to a set of members. All members that are found for the specified group will be added to this set.
		@param includeSubGroups When true include subgroups in the set of groups that are reported.
		@param resolveSubGroup When true recurse into any subgroups and expand them too.
	*/
	static bool GetLocalGroupMembers(string groupName, StringSet* members, bool includeSubGroups, bool resolveSubGroup);

	/** Get the members of the specified global group.

		@param groupName The name of the group to retrieve members for.
		@param members A pointer to a set of members. All members that are found for the specified group will be added to this set.
		@param includeSubGroups When true include subgroups in the set of groups that are reported.
		@param resolveSubGroup When true recurse into any subgroups and expand them too.
	*/
	static bool GetGlobalGroupMembers(string groupName, StringSet* members, bool includeSubGroups, bool resolveSubGroup);

	/** Get the set of all trustee names on the system for the well knowns SIDS.
		These are sids that are not returend by a call to NetUserEnum, 
		NetGroupEnum, or NetLocalGroupEnum.
		Selected well known sids are pulled from:
		http://support.microsoft.com/kb/243330
	*/
	static void GetWellKnownTrusteeNames();

	/** Look up the local system name. */
	static string LookUpLocalSystemName();

	/** Convert string to wide */
	static LPWSTR StringToWide(string s);

	static StringSet* allTrusteeNames;
	static StringSet* allTrusteeSIDs;
	static StringSet* wellKnownTrusteeNames;
	static StringSet* allLocalUserSIDs;

	static inline bool IsAccountGroup(SID_NAME_USE sidType, string accountName);	
};

#endif
