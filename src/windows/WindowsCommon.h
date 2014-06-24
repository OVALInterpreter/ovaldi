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

#ifndef WINDOWSCOMMON_H
#define WINDOWSCOMMON_H

#include <Windows.h> // for lots of windows types, e.g. DWORD
#include <AccCtrl.h> // for SE_OBJECT_TYPE
#include <string>

#include "Exception.h"
#include <Behavior.h>
#include <Stdtypedefs.h>
#include "BitnessView.h"

class WindowsCommon {

public:
	/** Do some one-time setup. */
	static void init();

	/**
	 * Release resources acquired in init().
	 * I'm letting this throw... so be careful when calling
	 * from destructors.
	 */
	static void uninit();
	/** Disable all the privileges associated with the current process token.
		If a specific privilege is needed later, it can be enabled by calling
		AdjustTokenPrivileges() again.
	*/
	static bool DisableAllPrivileges();

	/** Enable the specified privilege. */
	static bool EnablePrivilege(std::string);

	/** Convert the provded PSID to a string in the SID string format. 

	  A SID value includes components that provide information about the SID structure
	  and components that uniquely identify a trustee. A SID consists of the following
	  components: 
	
	   * The revision level of the SID structure 
	   * A 48-bit identifier authority value that identifies the authority that issued
	     the SID 
	   * A variable number of subauthority or relative identifier (RID) values that
	     uniquely identify the trustee relative to the authority that issued the SID
	
	  The combination of the identifier authority value and the subauthority values
	  ensures that no two SIDs will be the same, even if two different SID-issuing
	  authorities issue the same combination of RID values. Each SID-issuing authority
	  issues a given RID only once. 
	
	  SIDs are stored in binary format in a SID structure. To display a SID, you can
	  call the ConvertSidToStringSid function to convert a binary SID to string format.
	  To convert a SID string back to a valid, functional SID, call the
	  ConvertStringSidToSid function. 
	
	  These functions use the following standardized string notation for SIDs, which
	  makes it simpler to visualize their components: 
	
	  S-R-I-S-S...
	
	  In this notation, the literal character S identifies the series of digits as a
	  SID, R is the revision level, I is the identifier-authority value, and S... is one
	  or more subauthority values. 
	
	  NOTE:	
	  Windows 2000 provides the ConvertSidToStringSid and ConvertStringSidToSid functions
	  for converting a SID to and from string format. For a description of the SID string
	  format, see SID Components.
	
	  On earlier versions of Windows NT, use the following sample code to convert a SID
	  to string format.

	  SEE: http://msdn.microsoft.com/en-us/library/aa379597(VS.85).aspx

	  @param pSid the sid to convert to a string
	  @param the string to populate with the string representation of the SID.
	  @return true if the conversion was successful. otherwise return false.
	*/
	static bool GetTextualSid(PSID pSid, std::string* TextualSid);

	/** Return the string error massge for the specified error code. */
	static std::string GetErrorMessage(DWORD dwLastError);

	/** Expand the group returning all members. 
		If the group does not exist return false. 
		Look at local groups then global groups.

		@param groupName The name of the group to be expanded.
		@param members A pointer to a set that will be populated with the specified group members.
		@param includeSubGroups When true include subgroups in the set of groups that are reported.
		@param resolveSubGroup When true recurse into any subgroups and expand them too.
	*/
	static bool ExpandGroup(std::string groupName, StringSet* members, bool includeSubGroups, bool resolveSubGroup);

	/** Return true if the group exists add all the group member's SIDs to the memberSIDs parameter. 
		
		@param groupSID The SID of the group to be expanded.
		@param memberSIDs A pointer to a set that will be populated with the specified group member SIDs.
		@param includeSubGroups When true include subgroups in the set of groups that are reported.
		@param resolveSubGroup When true recurse into any subgroups and expand them too.
	*/
	static bool ExpandGroupBySID(std::string groupSID, StringSet* memberSIDs, bool includeSubGroups, bool resolveSubGroup);


	/** Get all trustee names on the system. 
		This set consists of the following:
        <ul>
		<li>Get all local user trustee names</li>
		<li>Get all local groups and expand to get all members</li>
		<li>Get all global groups</li>
        </ul>
	*/
	static StringSet* GetAllTrusteeNames();

	/** Get all the Trustee SIDs on the system.
		This method simply calls the WindowsCommon::GetAllTrusteeNames() method
		and converts each returned name to a sid string. The resulting set of sids is 
		returned as a StringSet.		
	*/
	static StringSet* GetAllTrusteeSIDs();

	/** Get the trustee name for the specified sid formatted for oval useage. */
	static std::string GetFormattedTrusteeName(PSID pSid);

	/** Get the SID for the specified trustee name. 
		TrusteeName should be a fully qualified account name. 
		For more info see:
		http://msdn.microsoft.com/en-us/library/aa379159.aspx
	*/
	static PSID GetSIDForTrusteeName(std::string trusteeName);

	/** Get the SID for the specified trustee sid string. 
		TrusteeSID is a complete sid string formated as the 
		For more info see:
		http://msdn.microsoft.com/library/default.asp?url=/library/en-us/secauthz/security/lookupaccountname.asp
	*/
	static PSID GetSIDForTrusteeSID(std::string trusteeSID);

	/** Retrieve the set of SIDS as strings from an ACL.
		
		@param pacl the ACL to extract SID string from.
		@param sids is a pointer to a set of strings that will be populated based on the input PACL.
	*/
	static void GetSidsFromPACL(PACL pacl, StringSet *sids);

    /** Retrieve the set of trustee names as strings from an ACL.
		
		@param pacl the ACL to  extract trustee names from.
		@param sids is a pointer to a set of strings that will be populated based on the input PACL.
	*/
    static void GetTrusteeNamesFromPACL(PACL pacl, StringSet *trusteeNames);

	/**
	 * Get the domain and sid string for the specified trustee name.
	 * \param[in] accountNameStr the account name to look up
	 * \param[out] sidStr the SID for \p accountNameStr, as a string
	 * \param[out] domainStr the domain of \p accountNameStr
	 * \param[out] isGroup receives true if the \p accountNameStr refers to a group, false otherwise
	 * \return true if the account was found, false if it was not found.
	 */
	static bool LookUpTrusteeName(std::string* accountNameStr, std::string* sidStr, std::string* domainStr, bool *isGroup);

	/**
	 * Get the account and domain string for the specified trustee sid.
	 * \param[in] sidStr the SID to look up, as a string
	 * \param[out] pAccountNameStr the account name for \p sidStr
	 * \param[out] pDomainStr the domain name for \p sidStr
	 * \param[out] isGroup receives true if the \p sidStr refers to a group, false otherwise
	 * \return true if the account was found, false if it was not found.
	 */
	static bool LookUpTrusteeSid(std::string sidStr, std::string* pAccountNameStr, std::string* pDomainStr, bool *isGroup);

	/**
	 * Normalizes the given trustee name.  Any of the out-params may be NULL,
	 * if you don't need that particular value.
	 *
	 * \param[in] trusteeName the name to look up
	 * \param[out] normName the normalized trustee name, or NULL
	 * \param[out] normDomain the normalized domain name, or NULL
	 * \param[out] normFormattedName the normalized formatted name, or NULL
	 * \param[out] sidStr the SID, as a string, for the given trustee, or NULL
	 * \param[out] isGroup whether the given trustee refers to a group
	 * \return true if the account was found, false if it was not found.
	 */
	static bool NormalizeTrusteeName(const std::string &trusteeName, 
		std::string *normName, std::string *normDomain, 
		std::string *normFormattedName, std::string *sidStr,
		bool *isGroup);

	/** Convert a vector of trustee names to a vector of corresponding SID strings */
	static void ConvertTrusteeNamesToSidStrings(StringSet *trusteeNames, StringSet *sidStrings);

	/** Retrieves list of sids for all local users */
	static StringSet* GetAllLocalUserSids();

	/** Return true if the SID corresponds to a group. */
	static bool IsGroupSID(std::string sid);

	/** Returns true if the trustee name corresponds to a group. */
	static bool IsGroup(std::string trusteeName);

	/** Return a StringSet* of all local groups. */
	static StringSet* GetAllLocalGroups();

	/** Retrieves list of sids for all local groups */
	static StringSet* GetAllLocalGroupSids();

	/** Return the set of all local and global groups on the local system. */
	static StringSet* GetAllGroups();

	/** Get the set of all local users. 

		@param allUsers A pointer to variable that will be populated with all the local users.
	*/
	static void GetAllLocalUsers(StringSet* all);

	/** Get local and global groups for the user. 
	    @param userName The name of the user to get groups for.
		@param groups A pointer to a StringSet that will be populated with the groups that the user is a member of.
	*/
	static bool GetGroupsForUser(std::string userName, StringSet* groups);

	/**
	 * Gets some account info via NetUserGetInfo().  Pass NULL for any values
	 * you don't need.
	 *
	 * \param[in] accountName The account name to look up.
	 * \param[out] enabled If non-NULL, receives whether the account is
	 *   enabled.
	 * \param[out] lastLogon If non-NULL, receives the time of last logon.
	 * \return true if the account exists, false if not.
	 */
	static bool GetAccountInfo(const std::string &accountName, bool *enabled, DWORD *lastLogon);

	/** Convert the FILETIME structure to an integer. */
	static std::string ToString(FILETIME fTime);

	/** Convert the PSID to a string. 
		Attempts to return a string representation of the input PSID. If
		the PSID can not be converted to a string an empty string is returned.
	*/
	//static string ToString(PSID pSid);

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

    /** Convert a std:string into a null-terminated string of Unicode characters (16-bit).
     *  @param str A std:string that want to convert to a null-terminated string of Unicode characters (16-bit).
     *  @return A null-terminated string of Unicode characters (16-bit).
     */
	static std::wstring StringToWide(std::string s);

	/** Converts a wide-character string of Unicode characters into a string of ASCII characters.
     *  @param unicodeCharStr Pointer to the wide-character string of Unicode characters that you would like to convert into a string of ASCII characters.
     *  @return A string of ASCII characters.
    */
	static std::string UnicodeToAsciiString ( const wchar_t* unicodeCharStr );

	/**
	 * Provided for symmetry with UnicodeToAsciiString(const string &).
	 * Having this also makes your code cleaner, so you don't have to
	 * call c_str() all the time.....
	 */
	static std::string UnicodeToAsciiString ( const std::wstring &wstr );

	/**
	 * This seems like a pointless method to have, but it's helpful when
	 * writing templates where you don't have a fixed char type.  It allows
	 * you to always be able to call UnicodeToAsciiString() regardless of
	 * which type of string you have, and obtain a narrow char string.
	 * <p>
	 * This method just returns \p str.
	 */
	static std::string UnicodeToAsciiString ( const std::string &str );

	/** Return true if a wide-character string of Unicode characters would convert into a string of ASCII characters.
     *  @param unicodeCharStr Pointer to the wide-character string of Unicode characters that are checked for validity as ASCII characters.
    */
	static bool UnicodeIsValidASCII(wchar_t* unicodeCharStr);

    /** Return true if the specfied trustee name exists.		
		@param trusteeNameIn The SID to look for.
	*/
    static bool TrusteeNameExists(const std::string trusteeNameIn);
    
    /** Return true if the specfied trustee SID exists.		
		@param trusteeSIDIn The SID to look for.
	*/
    static bool TrusteeSIDExists(const std::string trusteeSIDIn);

	/** Return the access mask for the Windows object for the specified SID. */
	static void GetEffectiveRightsForWindowsObject(SE_OBJECT_TYPE objectType, PSID pSid, HANDLE objHandle, PACCESS_MASK pAccessRights);

	/** Return the access mask for the Windows object for the specified SID. */
	static void GetEffectiveRightsForWindowsObjectAcl(SE_OBJECT_TYPE objectType, PSID pSid, HANDLE objHandle, PACCESS_MASK pAccessRights);

	/** Return the access mask for the Windows object for the specified SID. */
	static void GetEffectiveRightsForWindowsObjectAuthz(SE_OBJECT_TYPE objectType, PSID pSid, HANDLE objHandle, PACCESS_MASK pAccessRights);

	/** Return the successful and failure audit access masks for the Windows object for the specified SID. 
	  * @param objectType A SE_OBJECT_TYPE that specifies the type of the Windows object.
      * @param pSid A pointer to a SID structure that specifies the SID whose audit access masks that you would like to obtain.
	  * @param objectNameStr A string value that contains the name of the Windows object.
	  * @param pSuccessfulAuditPermissions A pointer to an ACCESS_MASK that contains the successful audit permissions.
	  * @param pFailedAuditPermissions A pointer to an ACCESS_MASK that contains the failure audit permissions.
	  */
	static void GetAuditedPermissionsForWindowsObject(SE_OBJECT_TYPE objectType, PSID pSid, HANDLE objHandle, PACCESS_MASK pSuccessfulAuditPermissions, PACCESS_MASK pFailedAuditPermissions);

	/** Get the string representation of a SE_OBJECT_TYPE.
		@param objectType The SE_OBJECT_TYPE value that you want to get the string representation of.
        @return A string representation of the specified SE_OBJECT_TYPE.
	*/
	static std::string GetObjectType ( SE_OBJECT_TYPE objectType );

	/**
	 * Windows (NTFS anyway, afaik) is case-insensitive, but case-aware.  We
	 * should have in our items, filenames and paths as they actually are
	 * in the filesystem, matching case.
	 * <p>
	 * Actually taking a path and resolving it to the actual path in the
	 * filesystem, with the exact case like this, is annoyingly difficult
	 * on XP.  There's simply no straightforward way to do it.  One way
	 * involves mapping the file to memory:
	 * <p>
	 * http://msdn.microsoft.com/en-us/library/aa366789%28VS.85%29.aspx
	 * <p>
	 * but according to the code on that page, it doesn't work for zero-
	 * length files, since I guess you can't memory-map those.  Another way
	 * would be to do a filesystem search with the FindFirst/FindNext
	 * functions according to the path components, and piece together the path
	 * from that.  That seems like a stupidly inefficient way to do it.  There
	 * is an undocumented way to do it which involves pasting in prototypes
	 * for hidden functions into your own code:
	 * <p>
	 * http://stackoverflow.com/questions/65170/how-to-get-name-associated-with-open-handle
	 * <p>
	 * That is also very ugly.  I found in a forum somewhere a mention that
	 * someone did it by using GetShortPathName() and GetLongPathName():
	 * just take your path and shorten it with the former function, then
	 * take the result and lengthen it with the latter.  I tried it and it
	 * worked, and the functions are available on XP.  I also read that
	 * short path names can be disabled in filesystem, so this technique
	 * may not always work.  But it's by far the simplest way I've been
	 * able to find, which works on XP.
	 * <p>
	 * On Vista+, there is a GetFileInformationByHandleEx() function, with
	 * which you can get filename info.  If that can be made to work, it would
	 * much simpler than any of the above techniques.  But we must stay
	 * compatible with XP.
	 * \return false if the file did not exist; true on success.  Throws on other
	 *   errors.
	 */
	static bool GetActualPathWithCase(const std::string &path, std::string *casedString);

	/**
	 * Returns true if this process is running under WoW64.
	 */
	static bool IsWow64Process();

	/**
	 * Detects whether this app is running on 64-bit windows.
	 */
	static bool Is64BitOS();

	/**
	 * Convenience method that converts a behavior windows_view value to one of the
	 * BitnessView enumerators.
	 * <p>
	 * This does not check the OS or app bitness to determine whether the view makes
	 * sense, it's just a straight translation.  So callers will have to take those
	 * factors into account to select the view, not just unconditionally use the
	 * return value as the view to query.
	 *
	 * \throw Exception if viewStr isn't recognized as a valid view.
	 */
	static BitnessView behavior2view(const std::string &viewStr);

	/**
	 * Convenience method that gets a BitnessView enumerator for the windows_view
	 * behavior from the given behavior vector.  If \p bv is NULL or doesn't contain
	 * the windows_view behavior, the default as specified in the schema is returned.
	 * <p>
	 * This does not check the OS or app bitness to determine whether the view makes
	 * sense, it's just a straight translation.  So callers will have to take those
	 * factors into account to select the view, not just unconditionally use the
	 * return value as the view to query.
	 *
	 * \throw Exception if \p bv contains the windows_view behavior,
	 *	but its value isn't recognized as valid.
	 */
	static BitnessView behavior2view(BehaviorVector *bv);

private:
	
	/** Split Trustee name between domain and account portion */
	static void SplitTrusteeName(std::string trusteeName, std::string *domainName, std::string *accountName);

	/** Return a StringSet* of all global groups. */
	static StringSet* GetAllGlobalGroups();

	/** Get the members of the specified local group. 

		@param groupName The name of the group to retrieve members for.
		@param members A pointer to a set of members. All members that are found for the specified group will be added to this set.
		@param includeSubGroups When true include subgroups in the set of groups that are reported.
		@param resolveSubGroup When true recurse into any subgroups and expand them too.
	*/
	static bool GetLocalGroupMembers(std::string groupName, StringSet* members, bool includeSubGroups, bool resolveSubGroup);

	/** Get the members of the specified global group.

		@param groupName The name of the group to retrieve members for.
		@param members A pointer to a set of members. All members that are found for the specified group will be added to this set.
		@param includeSubGroups When true include subgroups in the set of groups that are reported.
		@param resolveSubGroup When true recurse into any subgroups and expand them too.
        @return True if the specified group exists.
	*/
	static bool GetGlobalGroupMembers(std::string groupName, StringSet* members, bool includeSubGroups, bool resolveSubGroup);

	/** Get the set of all trustee names on the system for the well knowns SIDS.
		These are sids that are not returend by a call to NetUserEnum, 
		NetGroupEnum, or NetLocalGroupEnum.
		Selected well known sids are pulled from:
		http://support.microsoft.com/kb/243330
	*/
	static void GetWellKnownTrusteeNames();

	/** Retrieve the domain controller name for the specified domain name.
	  @param domainName A string that represents the domain name.
	  @return A LPCWSTR representing the domain controller name or NULL if the domain name is the empty string or if the domain controller name lookup failed.
	*/
	static std::wstring GetDomainControllerName(std::string domainName);

	static StringSet* allTrusteeNames;
	static StringSet* allTrusteeSIDs;
	static StringSet* wellKnownTrusteeNames;
	static StringSet* allLocalUserSIDs;
	static StringSet* allLocalGroupSIDs;

	static inline bool IsAccountGroup(SID_NAME_USE sidType, std::string accountName);	
};

#endif
