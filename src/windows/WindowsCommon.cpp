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

#include <aclapi.h>
#include <lm.h>
#include <Ntsecapi.h>
#include <Sddl.h>
#include <Authz.h>
#include <comdef.h>
#include <DelayImp.h>
#include <time.h>
#include <tchar.h>

#include <cassert>
#include <cctype>
#include <cstring>
#include <map>
#include <memory>

#include "Log.h"
#include <ArrayGuard.h>
#include <FreeGuard.h>
#include <Common.h>
#include "WindowsCommon.h"

using namespace std;

StringSet* WindowsCommon::allLocalUserSIDs = NULL;
StringSet* WindowsCommon::allLocalGroupSIDs = NULL;
StringSet* WindowsCommon::allTrusteeNames = NULL;
StringSet* WindowsCommon::allTrusteeSIDs = NULL;
StringSet* WindowsCommon::wellKnownTrusteeNames = NULL;
const int NAME_BUFFER_SIZE = 255;

namespace {

	/**
	 * Contains some info obtained once and cached, so we don't have
	 * to re-acquire it over and over...
	 */
	namespace globals {
		/**
		 * This computer's name.
		 */
		wstring computerName;

		/**
		 * This computer's SID.
		 */
		PSID computerSID = NULL;

		/**
		 * Domain name of this computer.  If empty, this computer is not
		 * joined to a domain.
		 */
		wstring domainName;

		/**
		 * The SID of this computer's domain.  If NULL, this computer is not
		 * joined to a domain.
		 */
		PSID domainSID = NULL;

		typedef map<wstring, wstring> DomainControllerMap;
		/**
		 * Maps domain names to domain controller hostnames.  This way, we
		 * can look them up once and cache the results.  If the mapped-to
		 * domain controller hostname is the empty string, then a controller
		 * name couldn't be found.  This can happen e.g. when the network
		 * is not available.
		 */
		DomainControllerMap domainControllerHostnames;
	}

	/** yet another guard.... */
	class NetApiBufferFreeGuard {
		void *ptr_;
	public:
		NetApiBufferFreeGuard(void *ptr=NULL):ptr_(ptr){}
		~NetApiBufferFreeGuard(){if(ptr_)NetApiBufferFree(ptr_);}
	};

	/**
	 * Determines whether the given SID is "local".  A local SID
	 * is a non-account SID, or is prefixed by the local computer SID.
	 */
	bool isLocal(PSID sid);

	/**
	 * A copy of WindowsCommon::GetSIDForTrusteeName() which uses
	 * wide strings.  This simplifies code which has a wide string
	 * trustee name: you don't have to narrow it.  Also, this version
	 * returns NULL if the trustee isn't found, rather than throwing
	 * an exception.  Other errors still cause an exception.
	 *
	 * \param trusteeName the trustee to look up
	 * \return The SID for the trustee, or NULL if none was found.  If
	 *   a SID is returned, the caller must free() it.
	 */
	PSID GetSIDForTrusteeNameW(const wstring &trusteeName);
}

void WindowsCommon::init() {

	/** Make it easy to rollback changes on error. */
	class ErrorRollback {
		bool success_;
	public:
		ErrorRollback():success_(false){}
		~ErrorRollback() {
			if (success_) return;
			// For now, this should work even on partial initialization.
			// CHANGEME if this is no longer the case!
			try{ WindowsCommon::uninit(); }
			// An exception here only would occur while rolling back
			// partial initialization.  And initialization only partially
			// completes and requires rollback when there's already 
			// been another error.  So let's not confuse the issue by
			// printing more errors about failure to rollback partial
			// initialization... make sense??
			catch(...) { }
		}
		void success(){success_=true;}
	} errorRollback;

	// Get computer name
	DWORD err;
	FreeGuard<WCHAR> compName;
	DWORD compNameSz = 0;
	if (!GetComputerNameExW(ComputerNamePhysicalDnsHostname,
		NULL, &compNameSz)) {
		err = GetLastError();
		if (err != ERROR_MORE_DATA)
			throw Exception("Couldn't get computer name: GetComputerNameEx: " +
				GetErrorMessage(err));

		if (!compName.realloc(compNameSz * sizeof(WCHAR)))
			throw Exception("Out of memory!");

		if (!GetComputerNameExW(ComputerNamePhysicalDnsHostname,
			compName.get(), &compNameSz))
			throw Exception("Couldn't get computer name: GetComputerNameEx: " +
				GetErrorMessage(GetLastError()));

		globals::computerName = compName.get();
	}

	// Get computer SID
	globals::computerSID = GetSIDForTrusteeNameW(globals::computerName);
	if (!globals::computerSID)
		// can this actually happen?
		throw Exception("Couldn't find SID for computer name: " +
		UnicodeToAsciiString(globals::computerName));

	// Get join info
	LPWSTR joinInfo;
	NETSETUP_JOIN_STATUS njs;

	NET_API_STATUS nas = NetGetJoinInformation(
		NULL,
		&joinInfo,
		&njs);

	if (nas)
		throw Exception("Couldn't get domain join info: NetGetJoinInformation: " +
			GetErrorMessage(nas));

	if (njs == NetSetupDomainName) {
		globals::domainName = joinInfo;
		NetApiBufferFree(joinInfo);
	} else if (njs == NetSetupWorkgroupName)
		// don't care about workgroups!
		NetApiBufferFree(joinInfo);

	// stop here if not joined to a domain.
	if (njs != NetSetupDomainName)
	{
		errorRollback.success();
		return;
	}

	// Get domain SID
	globals::domainSID = GetSIDForTrusteeNameW(globals::domainName);
	if (!globals::domainSID)
		// can this actually happen?
		throw Exception("Couldn't find SID for domain name: " +
		UnicodeToAsciiString(globals::domainName));

	// disable rollback, since we finished successfully
	errorRollback.success();
}

void WindowsCommon::uninit() {
	globals::computerName.clear();
	if (globals::computerSID) {
		free(globals::computerSID);
		globals::computerSID = NULL;
	}

	globals::domainName.clear();
	if (globals::domainSID) {
		free(globals::domainSID);
		globals::domainSID = NULL;
	}

	globals::domainControllerHostnames.clear();
}

bool WindowsCommon::DisableAllPrivileges() {

	HANDLE hToken = NULL;

	// Get a handle to the current process.

	if (OpenProcessToken(GetCurrentProcess(),						// handle to the process
						 TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,		// requested access types 
						 &hToken) == FALSE)							// new access token 
	{
		char buffer[33];
		_itoa(GetLastError(), buffer, 10);

		string errorMessage = "";
		errorMessage.append("\nERROR: Unable to get a handle to the current process.  Error # - ");
		errorMessage.append(buffer);
		errorMessage.append("\n");
		Log::Fatal(errorMessage);

		return false;
	}

	// Disable all the privileges for this token.

	if (AdjustTokenPrivileges(hToken,					// handle to token
							  TRUE,						// disabling option
							  NULL,						// privilege information
							  0,						// size of buffer
							  NULL,						// original state buffer
							  NULL) == FALSE)			// required buffer size
	{
		char buffer[33];
		_itoa(GetLastError(), buffer, 10);

		string errorMessage = "";
		errorMessage.append("\nERROR: Unable to disable token privileges.  Error # - ");
		errorMessage.append(buffer);
		errorMessage.append("\n");
		Log::Fatal(errorMessage);

		CloseHandle(hToken);
		return false;
	}

	CloseHandle(hToken);

	return true;
}

bool WindowsCommon::EnablePrivilege(string privilegeIn) {

	TOKEN_PRIVILEGES tp;
	HANDLE hProcess = NULL;
	HANDLE hAccessToken = NULL;

	hProcess = GetCurrentProcess();
	
	if(OpenProcessToken(hProcess,									// handle to the process
						(TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES),	// requested access types 
						&hAccessToken) == FALSE)					// new access token 
	{
		return false;
	}

	tp.PrivilegeCount = 1;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    
	if (LookupPrivilegeValue(NULL, privilegeIn.c_str(), &tp.Privileges[0].Luid) == 0)
	{
		return false;
	}

	if (AdjustTokenPrivileges(hAccessToken, FALSE, &tp, NULL, NULL, NULL) == 0)
	{
		return false;
	}
	 
	if(GetLastError() == ERROR_NOT_ALL_ASSIGNED)
	{
		// The token for the current process does not have the privilege specified. The
		// AdjustTokenPrivileges() function may succeed with this error value even if no
		// privileges were adjusted.  The privilege parameter can specify privileges that
		// the token does not have, without causing the function to fail. In this case, 
		// the function adjusts the privileges that the token does have and ignores the 
		// other privileges so that the function succeeds.

		CloseHandle(hAccessToken);
		return false;
	}
	else
	{
		CloseHandle(hAccessToken);
		return true;
	}
}

string WindowsCommon::GetErrorMessage(DWORD dwLastError) {

	string errMsg = "";

    HMODULE hModule = NULL; // default to system source
    LPSTR MessageBuffer;
    DWORD dwBufferLength;

    DWORD dwFormatFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_IGNORE_INSERTS |
        FORMAT_MESSAGE_FROM_SYSTEM ;

    //
    // If dwLastError is in the network range, 
    //  load the message source.
    //
    if(dwLastError >= NERR_BASE && dwLastError <= MAX_NERR) {
        hModule = LoadLibraryEx(
            TEXT("netmsg.dll"),
            NULL,
            LOAD_LIBRARY_AS_DATAFILE
            );

        if(hModule != NULL)
            dwFormatFlags |= FORMAT_MESSAGE_FROM_HMODULE;
    }

    //
    // Call FormatMessage() to allow for message 
    //  text to be acquired from the system 
    //  or from the supplied module handle.
    //
	dwBufferLength = FormatMessageA(dwFormatFlags,
									hModule, // module to get message from (NULL == system)
									dwLastError,
									MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // default language
									(LPSTR) &MessageBuffer,
									0,
									NULL
									);
    if(dwBufferLength)
    {
		errMsg = MessageBuffer;
        //
        // Free the buffer allocated by the system.
        //
        LocalFree(MessageBuffer);
    }

    //
    // If we loaded a message source, unload it.
    //
    if(hModule != NULL)
        FreeLibrary(hModule);

    // make sure there is no trailing new line
	Common::TrimEnd(errMsg);

	return errMsg;
}

bool WindowsCommon::GetTextualSid(PSID pSid, string* TextualSid) { 

	LPTSTR sidCStr;
	if (ConvertSidToStringSid(pSid, &sidCStr)) {
#ifdef _UNICODE
		*TextualSid = UnicodeToAsciiString(sidCStr);
#else
		*TextualSid = sidCStr;
#endif
		LocalFree(sidCStr);
		return true;	
	}

	return false;
}

bool WindowsCommon::ExpandGroup(string groupName, StringSet* members, bool includeSubGroups, bool resolveSubGroup) {

	FreeGuard<> groupSid(GetSIDForTrusteeName(groupName));

	if (!groupSid.get())
		return false;
	
	// Need to determine if a local or global group.
	bool isLocalGroup = isLocal(groupSid.get());

	bool groupExists;

	if (isLocalGroup)
		groupExists = WindowsCommon::GetLocalGroupMembers(groupName, members, includeSubGroups, resolveSubGroup);
	else
		groupExists = WindowsCommon::GetGlobalGroupMembers(groupName, members, includeSubGroups, resolveSubGroup);	

	return groupExists;
}

bool WindowsCommon::ExpandGroupBySID(string groupSidStr, StringSet* memberSids, bool includeSubGroups, bool resolveSubGroup) {	

	bool groupExists = false;
	PSID pSid;

	BOOL retVal = ConvertStringSidToSid(const_cast<char*>(groupSidStr.c_str()),	&pSid);									
		
	if(retVal != FALSE) {		
		string groupName = WindowsCommon::GetFormattedTrusteeName(pSid);

		StringSet *memberNames = new StringSet();

		groupExists = WindowsCommon::ExpandGroup(groupName, memberNames, includeSubGroups, resolveSubGroup);

		if(groupExists) {
			WindowsCommon::ConvertTrusteeNamesToSidStrings(memberNames, memberSids);
		} 

		delete memberNames;
		LocalFree(pSid);
	}

	return groupExists;
}

bool WindowsCommon::IsGroupSID(string SID) {

	PSID pSID = NULL;
	DWORD nameBufferSize = NAME_BUFFER_SIZE;	
	DWORD domainNameBufferSize = NAME_BUFFER_SIZE;
	char nameBuffer[NAME_BUFFER_SIZE];
	char domainNameBuffer[NAME_BUFFER_SIZE];
	SID_NAME_USE sidType;
	bool isAccountGroup = false;

	try {
		
        if(ConvertStringSidToSid(SID.c_str(), &pSID) == 0) {
			throw Exception("Error encountered converting group SID string to a SID");
		}

		if(!IsValidSid(pSID)) {
			throw Exception("Invalid SID found in WindowsCommon::IsGroupSID()");
		}

		if(LookupAccountSid(NULL, pSID, nameBuffer, &nameBufferSize, domainNameBuffer, &domainNameBufferSize, &sidType) != 0)  {
			isAccountGroup = IsAccountGroup(sidType, string(nameBuffer));
		} else {
            throw Exception("Error looking up SID. " + WindowsCommon::GetErrorMessage(GetLastError()));
		}

	} catch(...) {
		if(pSID != NULL) {
			LocalFree(pSID);
		}

		throw;
	}

	if(pSID != NULL) {
		LocalFree(pSID);
	}

	return isAccountGroup;
}

StringSet* WindowsCommon::GetAllGroups() {

	auto_ptr<StringSet> groups(WindowsCommon::GetAllLocalGroups());
	auto_ptr<StringSet> globalGroups(WindowsCommon::GetAllGlobalGroups());
	groups->insert(globalGroups->begin(), globalGroups->end());

	return groups.release();
}

bool WindowsCommon::IsGroup(string trusteeName) {
	DWORD sidSize = 128;
	DWORD domainSize = 128;
	DWORD error;
	//DWORD zero = 0;
	SID_NAME_USE sidUse;
	BOOL retVal = FALSE;
	FreeGuard<> psid;
	FreeGuard<TCHAR> domain;
	bool isGroup = false;

	do {
		// Initial memory allocations for the SID and DOMAIN.
		if (!psid.realloc(sidSize))
			throw Exception(string("realloc: ")+strerror(errno));

		if (!domain.realloc(domainSize))
			throw Exception(string("realloc: ")+strerror(errno));

		// Call LookupAccountName to get the SID.
		retVal = LookupAccountName(NULL,								// system name NULL == localhost
								trusteeName.c_str(),	// account name
								psid.get(),									// security identifier
								&sidSize,								// size of security identifier
								domain.get(),									// domain name
								&domainSize,							// size of domain name
								&sidUse);								// SID-type indicator
		error = GetLastError();
	} while (!retVal && error == ERROR_INSUFFICIENT_BUFFER);

	if(retVal == FALSE) {	
		if(error == ERROR_TRUSTED_RELATIONSHIP_FAILURE) {
			throw Exception("Unable to locate account: " + trusteeName + ". " + WindowsCommon::GetErrorMessage(error), ERROR_NOTICE);
		} else {
			throw Exception("Error failed to look up account: " + trusteeName + ". " + WindowsCommon::GetErrorMessage(error));
		}
	}
		
	isGroup = WindowsCommon::IsAccountGroup(sidUse, trusteeName);

	return isGroup;
}

wstring WindowsCommon::StringToWide(string s) {
	
	size_t size = mbstowcs(NULL, s.c_str(), s.length());
	// why would docs say -1 is returned on error, when the
	// return types are all unsigned?!?  Guessing I have to
	// compare to -1 cast to size_t...
	if (size == (size_t)-1)
		throw Exception("Couldn't widen string: " + s);
	// mbstowcs ret value doesn't count the terminal NULL...
	LPWSTR wide = new WCHAR[size + 1];
	mbstowcs(wide, s.c_str(), size);
	wide[size] = 0;
	wstring ws(wide);
	delete[] wide;

	return ws;
}

void WindowsCommon::SplitTrusteeName(string trusteeName, string *domainName, string *accountName) {
	
	size_t idx = trusteeName.find('\\', 0);

	if((idx != string::npos) && (trusteeName.length() != idx + 1)) {
		
		*domainName = trusteeName.substr(0, idx);
		*accountName = trusteeName.substr(idx + 1, string::npos);
	} else {
		*domainName = "";
		*accountName = trusteeName;
	}
}

bool WindowsCommon::GetLocalGroupMembers(string groupName, StringSet* members, bool includeSubGroups, bool resolveSubGroup) {
	
	Log::Debug("Getting group members for local group: " + groupName + " resolveSubGroup = " + Common::ToString(resolveSubGroup));

	bool groupExists = false;

	NET_API_STATUS  res;
	wstring localGroupName;
	DWORD entriesread;
	DWORD totalentries;
	LOCALGROUP_MEMBERS_INFO_0* userInfo = NULL;
	string shortGroupName;
	string groupDomain;
	
	// Split the group name into its domain and trustee name components and only use the name component in the group name.
	// The remarks section for the documentation regarding the NetLocalGroupGetMembers() function states that the characters
	// ", /, \, [, ], :, |, <, >, +, =, ;, ?, * cannot be present in the account name (i.e. domain\trustee_name) violates this.
	// Also, by specifying NULL for the servername you are indicating that it should look for the particular group name on the local
	// system where the domain component isn't needed.
	WindowsCommon::SplitTrusteeName(groupName,&groupDomain,&shortGroupName);
	localGroupName = WindowsCommon::StringToWide(shortGroupName);
	
	try {

		res = NetLocalGroupGetMembers(NULL,						// server name NULL == localhost
									  localGroupName.c_str(),	// group name
  									  0,						// level LOCALGROUP_MEMBERS_INFO_3
									  (unsigned char**) &userInfo,
									  MAX_PREFERRED_LENGTH,
									  &entriesread,
									  &totalentries,
									  NULL);

		// was there an error?
		if(res == NERR_Success) {

				// Loop through each member.
				for (unsigned int i=0; i<entriesread; i++) {

					// get sid
					PSID pSid = userInfo[i].lgrmi0_sid;
					try {
						string trusteeName = WindowsCommon::GetFormattedTrusteeName(pSid);
					
						bool isGroup = WindowsCommon::IsGroup(trusteeName);

						if(isGroup && resolveSubGroup)
							ExpandGroup(trusteeName, members, includeSubGroups, resolveSubGroup);

						if(!isGroup || includeSubGroups) {
							members->insert(trusteeName);
						}
					} catch (Exception ex) {
						Log::Info("Unable to get all group members." + ex.GetErrorMessage());
					}
				}

				groupExists = true;

		} else {
			if(res == NERR_InvalidComputer) {
				// throw this error
				throw Exception("Unable to expand local group: " + groupName + ". The computer name is invalid.");
			} else if(res == ERROR_MORE_DATA) {
				// throw this error
				throw Exception("Unable to expand local group: " + groupName + ". More entries are available. Specify a large enough buffer to receive all entries. This error message should never occure since the api call is made with MAX_PREFERRED_LENGTH for the size of the buffer.");
			} else if(res == ERROR_NO_SUCH_ALIAS || res == NERR_GroupNotFound) {
				// ignore this error
				Log::Debug("GetLocalGroupMembers - The local group name: " + groupName + " could not be found.");
			} else if(res == ERROR_ACCESS_DENIED) {
				// throw this error???
				throw Exception("Unable to expand local group: " + groupName + ". " + " The user does not have access to the requested information.");
			} else {
				throw Exception("Unable to expand local group: " + groupName + ". " + WindowsCommon::GetErrorMessage(res));
			}
		}

	} catch(...) {
	
		if(userInfo != NULL) {
			NetApiBufferFree(userInfo);
		}

		throw;
	}

	if(userInfo != NULL) {
		NetApiBufferFree(userInfo);
	}

	return groupExists;
}

bool WindowsCommon::GetGlobalGroupMembers(string groupName, StringSet* members, bool includeSubGroups, bool resolveSubGroup) {

	Log::Debug("Getting group members for global group: " + groupName + " resolveSubGroup = " + Common::ToString(resolveSubGroup));

	string domainName;
	DWORD entriesread;
	DWORD totalentries;
	NET_API_STATUS res;
	string shortGroupName;
	bool groupExists = false;
	wstring globalgroupname;
	wstring domainControllerName;
	GROUP_USERS_INFO_0* userInfo = NULL; 

	WindowsCommon::SplitTrusteeName(groupName, &domainName, &shortGroupName);

	domainControllerName = WindowsCommon::GetDomainControllerName(domainName);
	globalgroupname = WindowsCommon::StringToWide(shortGroupName);

	res = NetGroupGetUsers(
		domainControllerName.empty() ? NULL : domainControllerName.c_str(),
		globalgroupname.c_str(),
		0,
		(LPBYTE*) &userInfo,
		MAX_PREFERRED_LENGTH,
		&entriesread,
		&totalentries,
		NULL);

	NetApiBufferFreeGuard nabfg1(userInfo);

	// was there an error?
	if(res == NERR_Success) {

		// Loop through each user.
		for (DWORD i=0; i<entriesread; i++) {
					
			FreeGuard<> pSid(WindowsCommon::GetSIDForTrusteeName(domainName +
				'\\' + UnicodeToAsciiString(userInfo[i].grui0_name)));
			if (!pSid.get())
				continue;

			string trusteeName = WindowsCommon::GetFormattedTrusteeName(pSid.get());

			bool isGroup = WindowsCommon::IsGroup(trusteeName);

			if(isGroup && resolveSubGroup)
				ExpandGroup(trusteeName, members, includeSubGroups, resolveSubGroup);
					
			if(!isGroup || includeSubGroups)
				members->insert(trusteeName);
		}

		groupExists = true;
	} else {
		if(res == NERR_InvalidComputer) {
			// throw this error
			throw Exception("Unable to expand global group: " + groupName + ". The computer name is invalid.");
		} else if(res == ERROR_MORE_DATA) {
			// throw this error
			throw Exception("Unable to expand global group: " + groupName + ". More entries are available. Specify a large enough buffer to receive all entries. This error message should never occure since the api call is made with MAX_PREFERRED_LENGTH for the size of the buffer.");
		} else if(res == NERR_GroupNotFound) {
			groupExists = false;
			// no action here
			Log::Debug("GetGlobalGroupMembers - The global group name: " + groupName + " could not be found.");			
		} else if(res == ERROR_ACCESS_DENIED) {
			// throw this error???
			throw Exception("Unable to expand global group: " + groupName + ". The user does not have access to the requested information.");			
		} else {
			throw Exception("Unable to expand global group: " + groupName + ". " + WindowsCommon::GetErrorMessage(res));
		}
	}


	return groupExists;
}

StringSet* WindowsCommon::GetAllTrusteeNames() {

	if(WindowsCommon::allTrusteeNames == NULL) {

		Log::Debug("Getting all trustee names for the first time.");

		WindowsCommon::allTrusteeNames = new StringSet();

		// get the well know trustee names
		WindowsCommon::GetWellKnownTrusteeNames();
		StringSet::iterator iterator;
		for(iterator = WindowsCommon::wellKnownTrusteeNames->begin(); iterator != WindowsCommon::wellKnownTrusteeNames->end(); iterator++) {
			allTrusteeNames->insert((*iterator));
		}

		WindowsCommon::GetAllLocalUsers(allTrusteeNames);

		// local groups		
		StringSet* localGroups = WindowsCommon::GetAllLocalGroups();
		for(iterator = localGroups->begin(); iterator != localGroups->end(); iterator++) {
			allTrusteeNames->insert((*iterator));
			StringSet *members = new StringSet();

			// expand the group
			try {
				
				/**
					Changing the resolvegroup flag to false should fix the recursion issue. 
					We already have the list of all local groups. We just need to get the members.
				*/
				WindowsCommon::GetLocalGroupMembers((*iterator), members, true, false);
				StringSet::iterator member;
				for(member = members->begin(); member != members->end(); member++) {
					allTrusteeNames->insert((*member));
				}		
			} catch(Exception ex) {
				Log::Debug(ex.GetErrorMessage());
			}

			delete members;
		}
		delete localGroups;

		// global groups
		StringSet* globalGroups = WindowsCommon::GetAllGlobalGroups();
		for(iterator = globalGroups->begin(); iterator != globalGroups->end(); iterator++) {
			allTrusteeNames->insert((*iterator));
		}
		delete globalGroups;

		Log::Debug("Completed getting all trustee names and found " + Common::ToString(WindowsCommon::allTrusteeNames->size()) + " names.");
	}

	return WindowsCommon::allTrusteeNames;
}

StringSet* WindowsCommon::GetAllTrusteeSIDs() {

	if(WindowsCommon::allTrusteeSIDs == NULL) {

		StringSet* trusteeNames = WindowsCommon::GetAllTrusteeNames();
		
		Log::Debug("GetAllTrusteeSIDs() - Found " + Common::ToString(trusteeNames->size()) + " trustee names when searching for all names.");

		WindowsCommon::allTrusteeSIDs = new StringSet();
		StringSet::iterator iterator;
		for(iterator = trusteeNames->begin(); iterator != trusteeNames->end(); iterator++) {
		
			FreeGuard<> pSid(WindowsCommon::GetSIDForTrusteeName((*iterator)));
			if (!pSid.get())
				continue;

			string sidStr;
			if (WindowsCommon::GetTextualSid(pSid.get(), &sidStr))
				WindowsCommon::allTrusteeSIDs->insert(sidStr);
			else {
				delete WindowsCommon::allTrusteeSIDs;
				WindowsCommon::allTrusteeSIDs = NULL;
				throw Exception("Couldn't convert SID to string: " + GetErrorMessage(GetLastError()));
			}
		}
	}

	return WindowsCommon::allTrusteeSIDs;
}

void WindowsCommon::GetWellKnownTrusteeNames() {

	if(WindowsCommon::wellKnownTrusteeNames == NULL) {
		WindowsCommon::wellKnownTrusteeNames = new StringSet();

		// create a vector of the well known sids
		StringSet wellKnownSids;
		//wellKnownSids.insert("S-1-0");			// Null Authority
		//wellKnownSids.insert("S-1-0-0");			// Nobody
		//wellKnownSids.insert("S-1-1");			// World Authority
		wellKnownSids.insert("S-1-1-0");			// Everyone
		//wellKnownSids.insert("S-1-2");			// Local Authority
		//wellKnownSids.insert("S-1-3");			// Creator Authority
		wellKnownSids.insert("S-1-3-0");			// Creator Owner
		wellKnownSids.insert("S-1-3-1");			// Creator Group
		wellKnownSids.insert("S-1-3-2");			// Creator Owner Server
		wellKnownSids.insert("S-1-3-3");			// Creator Group Server
		//wellKnownSids.insert("S-1-4");			// Non-unique Authority
		//wellKnownSids.insert("S-1-5");			// NT Authority
		wellKnownSids.insert("S-1-5-1");			// Dialup
		wellKnownSids.insert("S-1-5-2");			// Network
		wellKnownSids.insert("S-1-5-3");			// Batch
		wellKnownSids.insert("S-1-5-4");			// Interactive
		wellKnownSids.insert("S-1-5-6");			// Service
		wellKnownSids.insert("S-1-5-7");			// Anonymous
		wellKnownSids.insert("S-1-5-8");			// Proxy
		wellKnownSids.insert("S-1-5-9");			// Enterprise Domain Controllers
		wellKnownSids.insert("S-1-5-11");			// Authenticated Users
		wellKnownSids.insert("S-1-5-13");			// Terminal Server Users
		wellKnownSids.insert("S-1-5-18");			// Local System
		wellKnownSids.insert("S-1-5-19");			// NT Authority - local service
		wellKnownSids.insert("S-1-5-20");			// NT Authority - network service
		wellKnownSids.insert("S-1-5-32-544");		// Administrators
		wellKnownSids.insert("S-1-5-32-545");		// Users
		wellKnownSids.insert("S-1-5-32-546");		// Guests
		wellKnownSids.insert("S-1-5-32-547");		// Power Users
		//wellKnownSids.insert("S-1-5-32-548");		// Account Operators
		//wellKnownSids.insert("S-1-5-32-549");		// Server Operators
		//wellKnownSids.insert("S-1-5-32-550");		// Print Operators
		wellKnownSids.insert("S-1-5-32-551");		// Backup Operators
		wellKnownSids.insert("S-1-5-32-552");		// Replicators

		Log::Debug("Found " + Common::ToString(wellKnownSids.size()) + " well known SIDs.");

		// look up account names for all the sids
		StringSet::iterator iterator;
		for(iterator = wellKnownSids.begin(); iterator != wellKnownSids.end(); iterator++) {
			string currentSidStr = (*iterator);
			PSID psid = NULL;
			if(!ConvertStringSidToSid(const_cast<char*>(currentSidStr.c_str()), &psid)) {
				Log::Debug("Error converting sid string (" + currentSidStr +") to SID. " + WindowsCommon::GetErrorMessage(GetLastError()));
			} else {
			
				string trusteeName = WindowsCommon::GetFormattedTrusteeName(psid);	
				WindowsCommon::wellKnownTrusteeNames->insert(trusteeName);
			}

			LocalFree(psid);
		}

		Log::Debug("Found " + Common::ToString(WindowsCommon::wellKnownTrusteeNames->size()) + " well known trustee names.");
	}
}

StringSet* WindowsCommon::GetAllLocalGroups() {

	StringSet* allGroups = new StringSet();

	LOCALGROUP_INFO_0* localGroupInfo = NULL;
	NET_API_STATUS nas;
    DWORD recordsEnumerated = 0;
    DWORD totalRecords = 0;
	DWORD_PTR resumeHandle = 0;

	do { 
		nas = NetLocalGroupEnum(NULL, 
								0,
								(unsigned char**) &localGroupInfo,
								MAX_PREFERRED_LENGTH,
								&recordsEnumerated,
								&totalRecords,
								&resumeHandle);

		if ((nas == NERR_Success) || (nas==ERROR_MORE_DATA)) {
			// Group account names are limited to 256 characters.

			// Loop through each group.
			for (DWORD i=0; i<recordsEnumerated; i++) {
				string groupName = UnicodeToAsciiString(localGroupInfo[i].lgrpi0_name);
				// get sid for trustee name
				PSID pSid = WindowsCommon::GetSIDForTrusteeName(groupName);
				if (!pSid)
					continue;
				// get formatted trustee name
				groupName = WindowsCommon::GetFormattedTrusteeName(pSid);
				allGroups->insert(groupName);
			}
		} else {

			if(nas == ERROR_ACCESS_DENIED) { 
				delete allGroups;
				throw Exception("Error unable to enumerate local groups. The user does not have access to the requested information.");
			} else if(nas == NERR_InvalidComputer) {
				delete allGroups;
				throw Exception("Error unable to enumerate local groups. The computer name is invalid.");
			} else {
				delete allGroups;
				throw Exception("Error unable to enumerate local groups. " + WindowsCommon::GetErrorMessage(GetLastError()));
			}

		}

		// Free the allocated buffer.
		if (localGroupInfo != NULL) {
			NetApiBufferFree(localGroupInfo);
			localGroupInfo = NULL;
		}
	} while (nas==ERROR_MORE_DATA); 

	// Check again for allocated memory.
	if (localGroupInfo != NULL) NetApiBufferFree(localGroupInfo);

	return allGroups;
}

StringSet* WindowsCommon::GetAllLocalGroupSids() {

	if(WindowsCommon::allLocalGroupSIDs == NULL) {
		WindowsCommon::allLocalGroupSIDs = new StringSet();

		StringSet *pGroups = WindowsCommon::GetAllLocalGroups();

		WindowsCommon::ConvertTrusteeNamesToSidStrings(pGroups, WindowsCommon::allLocalGroupSIDs);

		delete pGroups;
	}

	return WindowsCommon::allLocalGroupSIDs;
}

StringSet* WindowsCommon::GetAllGlobalGroups() {

	StringSet* allGroups = new StringSet();

	// Get a handle to the policy object.
	NTSTATUS nts;
	LSA_HANDLE polHandle;
	LSA_OBJECT_ATTRIBUTES ObjectAttributes;
	ZeroMemory(&ObjectAttributes, sizeof(ObjectAttributes));

	nts = LsaOpenPolicy(NULL, &ObjectAttributes, POLICY_LOOKUP_NAMES, &polHandle);
	if (nts != ERROR_SUCCESS) {
		Log::Debug("Error unable to open a handle to the Policy object when trying to get all global groups.");
		return allGroups;
	}

	NET_API_STATUS nas;
	GROUP_INFO_0* globalGroupInfo = NULL;
	DWORD_PTR resumeHandle = 0;
	do { 
		
		DWORD recordsEnumerated = 0;
		DWORD totalRecords = 0;
		nas = NetGroupEnum(NULL,
						   0,
						   (unsigned char**) &globalGroupInfo,
						   MAX_PREFERRED_LENGTH,
						   &recordsEnumerated,
						   &totalRecords,
						   &resumeHandle);

		if ((nas == NERR_Success) || (nas==ERROR_MORE_DATA)) {
			// Group account names are limited to 256 characters.

			// Loop through each group.
			for (DWORD i=0; i<recordsEnumerated; i++) {
				string groupName = UnicodeToAsciiString(globalGroupInfo[i].grpi0_name);
				// get sid for trustee name
				PSID pSid = WindowsCommon::GetSIDForTrusteeName(groupName);
				if (!pSid)
					continue;
				// get formatted trustee name
				groupName = WindowsCommon::GetFormattedTrusteeName(pSid);
				allGroups->insert(groupName);
			}
		} else {
			nts = LsaClose(polHandle);

			if(nas == ERROR_ACCESS_DENIED) { 
				delete allGroups;
				throw Exception("Error unable to enumerate global groups. The user does not have access to the requested information.");
			} else if(nas == NERR_InvalidComputer) {
				delete allGroups;
				throw Exception("Error unable to enumerate global groups. The computer name is invalid.");
			} else {
				delete allGroups;
				throw Exception("Error unable to enumerate global groups. " + WindowsCommon::GetErrorMessage(GetLastError()));
			}
		}

		// Free the allocated buffer.
		if (globalGroupInfo != NULL) {
			NetApiBufferFree(globalGroupInfo);
			globalGroupInfo = NULL;
		}
	} while (nas==ERROR_MORE_DATA); 

	// Check again for allocated memory.
	if (globalGroupInfo != NULL) NetApiBufferFree(globalGroupInfo);

	// Close the handle to the open policy object.
	nts = LsaClose(polHandle);

	return allGroups;
}

void WindowsCommon::ConvertTrusteeNamesToSidStrings(StringSet *trusteeNames, StringSet *sidStrings) {

	StringSet::iterator iterator;
	for(iterator = trusteeNames->begin(); iterator != trusteeNames->end(); iterator++) {
	
		FreeGuard<> pSid(WindowsCommon::GetSIDForTrusteeName(*iterator));
		if (!pSid.get()) {
			Log::Info("ConvertTrusteeNamesToSidStrings: trustee not found: " + *iterator);
			continue;
		}
		string sidStr;

		if (!WindowsCommon::GetTextualSid(pSid.get(), &sidStr)) {
			Log::Info("Error converting SID to string: " + GetErrorMessage(GetLastError()));
			continue;
		}

		sidStrings->insert(sidStr);
	}
}

StringSet* WindowsCommon::GetAllLocalUserSids() {

	if(WindowsCommon::allLocalUserSIDs == NULL) {
		WindowsCommon::allLocalUserSIDs = new StringSet();

		StringSet *pUsers = new StringSet();

		WindowsCommon::GetAllLocalUsers(pUsers);

		WindowsCommon::ConvertTrusteeNamesToSidStrings(pUsers, WindowsCommon::allLocalUserSIDs);

		delete pUsers;
	}

	return WindowsCommon::allLocalUserSIDs;
}

void WindowsCommon::GetAllLocalUsers(StringSet* allUsers) {

	NET_API_STATUS nas;
    DWORD recordsEnumerated = 0;
    DWORD totalRecords = 0;
    USER_INFO_0* userInfo = NULL;
	
	DWORD resumeHandle = 0;

	do {
		// NOTE: Even though MAX_PREFERRED_LENGTH is specified, we must still check for
		// ERROR_MORE_DATA. (I think!)  I assume that if the server can not allocate the
		// total amount of space required, then it will allocate a smaller buffer and we
		// will need to make multiple calls to NetUserEnum().
		//
		// NOTE: NetUserEnum() requires us to link to Netapi32.lib.
		
		nas = NetUserEnum(NULL,
						  0,			// need to us this to get the name
						  0,			// filter 
						  (unsigned char**) &userInfo,
						  MAX_PREFERRED_LENGTH,
						  &recordsEnumerated,
						  &totalRecords,
						  &resumeHandle);
		
		if ((nas == NERR_Success) || (nas == ERROR_MORE_DATA)) {

			Log::Debug("Found " + Common::ToString(recordsEnumerated) + " local users.");

			// Loop through each user.
			for (unsigned int i=0; i<recordsEnumerated; i++) {
				// Get the account information.
				string userName = UnicodeToAsciiString(userInfo[i].usri0_name);
				// get sid for trustee name
				FreeGuard<> pSid(WindowsCommon::GetSIDForTrusteeName(userName));
				if (!pSid.get())
					continue;
				// get formatted trustee name
				userName = WindowsCommon::GetFormattedTrusteeName(pSid.get());
				allUsers->insert(userName);
			}

		} else {
			if(nas == ERROR_ACCESS_DENIED) { 
				throw Exception("Error unable to enumerate local users. The user does not have access to the requested information.");
			} else if(nas == NERR_InvalidComputer) {
				throw Exception("Error unable to enumerate local users. The computer name is invalid.");
			} else {
				throw Exception("Error unable to enumerate local users. " + WindowsCommon::GetErrorMessage(GetLastError()));
			}
		}

		// Free the allocated buffer.
		if (userInfo != NULL) {
			NetApiBufferFree(userInfo);
			userInfo = NULL;
		}
		

	} while (nas==ERROR_MORE_DATA); 

	// Check again for allocated memory.
	if (userInfo != NULL) NetApiBufferFree(userInfo);
}

string WindowsCommon::GetFormattedTrusteeName(PSID pSid) {

	// validate the sid
	if(!IsValidSid(pSid)) {
		throw Exception("GetFormattedTrusteeName() - Error invalid sid found.");
	}

	// get the account info for the sid
	string trusteeDomain = "";
	string trusteeName = "";
	SID_NAME_USE sid_type;
	LPTSTR trustee_name = NULL;
	LPTSTR domain_name = NULL;
	DWORD trustee_name_size = 0;
	DWORD domain_name_size = 0;

	LookupAccountSid(NULL,						// name of local or remote computer
						pSid,					// security identifier
						trustee_name,			// account name buffer
						&trustee_name_size,		// size of account name buffer
						domain_name,			// domain name
						&domain_name_size,		// size of domain name buffer
						&sid_type);				// SID type

	trustee_name_size++;
	trustee_name = (LPTSTR)realloc(trustee_name, trustee_name_size * sizeof(TCHAR));
	if (trustee_name == NULL) {
		throw Exception("Could not allocate space. Cannot get trustee_name for sid.");
	}

	domain_name_size++;
	domain_name = (LPTSTR)realloc(domain_name, domain_name_size * sizeof(TCHAR));
	if (domain_name == NULL) {
		free(trustee_name);
		throw Exception("Could not allocate space. Cannot get domain_name for sid.");
	}
	
	// Call LookupAccountSid again to retrieve the name of the account and the
	// name of the first domain on which this SID is found.
	if (LookupAccountSid(NULL,					// name of local or remote computer
						pSid,					// security identifier
						trustee_name,			// account name buffer
						&trustee_name_size,		// size of account name buffer
						domain_name,			// domain name
						&domain_name_size,		// size of domain name buffer
						&sid_type) == 0)		// SID type
	{
		string errMsg = WindowsCommon::GetErrorMessage(GetLastError());
		string sidStr;
		if (!WindowsCommon::GetTextualSid(pSid, &sidStr)) {
			Log::Info("Error converting a SID to a string: " + GetErrorMessage(GetLastError()));
			sidStr = "(could not convert sid)";
		}

		// all occurrences of this that i have seen are for the domain admins sid and the domain user's sid
		// I should be able to ignore these.
		Log::Info("Unable to look up account name for sid: " + sidStr + ". " + errMsg);

	} else {
		trusteeDomain = domain_name;
		if(trusteeDomain.compare("") != 0 && trusteeDomain.compare("NT AUTHORITY") != 0 && trusteeDomain.compare("BUILTIN") != 0) {
			trusteeName.append(domain_name);
			trusteeName.append("\\");
		}
		trusteeName.append(trustee_name);
	}

	free(domain_name);
	free(trustee_name);

	return trusteeName;
}

PSID WindowsCommon::GetSIDForTrusteeName(string trusteeName) {

	DWORD sidSize = 128;
	DWORD domainSize = 128;
	SID_NAME_USE sidUse;
	BOOL retVal = FALSE;
	FreeGuard<> pSid; // PSID is actually void*...
	FreeGuard<TCHAR> domain;
	DWORD err;
	
	do {
		// Initial memory allocations for the SID and DOMAIN.
		if (!pSid.realloc(sidSize))
			throw Exception(string("realloc: ")+strerror(errno));

		if (!domain.realloc(domainSize))
			throw Exception(string("realloc: ")+strerror(errno));

		// Call LookupAccountName to get the SID.
		retVal = LookupAccountName(NULL,								// system name NULL == localhost
								trusteeName.c_str(),	// account name
								pSid.get(),									// security identifier
								&sidSize,								// size of security identifier
								domain.get(),									// domain name
								&domainSize,							// size of domain name
								&sidUse);								// SID-type indicator
		err = GetLastError();

	} while (!retVal && err == ERROR_INSUFFICIENT_BUFFER);

	if (!retVal) {
		if (err == ERROR_NONE_MAPPED)
			return NULL;
		throw Exception("LookupAccountName(" + trusteeName + ") failed: " +
			GetErrorMessage(GetLastError()));
	}

	return pSid.release();
}

PSID WindowsCommon::GetSIDForTrusteeSID(string trusteeSID) {

	Log::Debug("Starting GetSIDForTrusteeSID with: " + trusteeSID);

	BOOL retVal = FALSE;
	PSID pSid = NULL;

	try {
		// Call LookupAccountName to get the SID.
		retVal = ConvertStringSidToSid(const_cast<char*>(trusteeSID.c_str()),	// sid string
										&pSid);									// security identifier
		
		if(retVal == FALSE) {

			LocalFree(pSid);
			pSid = NULL;

			DWORD errCode = GetLastError();
			string errMsg = WindowsCommon::GetErrorMessage(errCode);

			if(errCode == ERROR_INVALID_PARAMETER) {
				throw Exception("Invalid parameter specified for call to ConvertStringSidToSid()");
			} else if(errCode == ERROR_INVALID_SID) {
				throw Exception("Invalid sid specified for call to ConvertStringSidToSid()");
			} else {
				throw Exception("Error looking up SID for sid string. " + errMsg);
			}
		}

	} catch(Exception ex) {
		if(pSid != NULL) 
			LocalFree(pSid);
	
		pSid = NULL;
		
		Log::Debug("Error looking up sid for account: " + trusteeSID + ". " + ex.GetErrorMessage());
		throw Exception("Error looking up sid for account: " + trusteeSID + ". " + ex.GetErrorMessage());

	} catch(...) {
		if(pSid != NULL) 
			LocalFree(pSid);

		pSid = NULL;		

		Log::Debug("Error looking up sid for account: " + trusteeSID + ". " + WindowsCommon::GetErrorMessage(GetLastError()));
		throw Exception("Error looking up sid for account: " + trusteeSID + ". " + WindowsCommon::GetErrorMessage(GetLastError()));
	}

	// check the sid
	if(!IsValidSid(pSid)) {
		
		if(pSid != NULL)
			LocalFree(pSid);

		pSid = NULL;

		throw Exception("Error looking up sid for account: " + trusteeSID + ". Invalid sid found.");
	}

	return pSid;
}

void WindowsCommon::GetSidsFromPACL(PACL pacl, StringSet *sids) {

	ACL_SIZE_INFORMATION size_info;

	if (GetAclInformation(pacl, &size_info, sizeof(size_info), AclSizeInformation)) {

		for(unsigned int aceIdx = 0; aceIdx < size_info.AceCount; aceIdx++) {
			void *ace = NULL;

			if(GetAce(pacl, aceIdx, &ace)) {

				ACE_HEADER *header = (ACE_HEADER *)ace;
				PSID psid = NULL;

				// obviously this is not exhaustive.  We will need to add more
				// ACE types as we discover them in use.  Or be really complete
				// and handle them all...
				switch (header->AceType) {
				case ACCESS_ALLOWED_ACE_TYPE:
					psid = (PSID)&((ACCESS_ALLOWED_ACE *)ace)->SidStart;
					break;
				case ACCESS_DENIED_ACE_TYPE:
					psid = (PSID)&((ACCESS_DENIED_ACE *)ace)->SidStart;
					break;
				case SYSTEM_AUDIT_ACE_TYPE:
					psid = (PSID)&((SYSTEM_AUDIT_ACE *)ace)->SidStart;
					break;
				case SYSTEM_MANDATORY_LABEL_ACE_TYPE:
					psid = (PSID)&((SYSTEM_MANDATORY_LABEL_ACE *)ace)->SidStart;
					break;
				default:
					//TODO skip for now
					Log::Debug("Unsupported AceType found when getting sids from acl.");
				}

				if(IsValidSid(psid)) {

					char *buffer = NULL;
					if(ConvertSidToStringSid(psid, &buffer) == 0) {

						string errMessage = WindowsCommon::GetErrorMessage(GetLastError());

						if(buffer != NULL) {
							LocalFree(buffer);
						}

						throw Exception("Can't convert sid to string sid. " + errMessage);

					} else {
						string strSid = buffer;
						sids->insert(strSid);
						LocalFree(buffer);
					}
				} else {
					throw Exception("Invalid Sid found when getting SIDs from an ACL.");
				}

			} else {

                string errMessage = WindowsCommon::GetErrorMessage(GetLastError());
				throw Exception("Error while getting the ACE from the ACL. " + errMessage);
            }

		}

	} else {
        string errMessage = WindowsCommon::GetErrorMessage(GetLastError());
		throw Exception("Could not retrieve ace information from acl " + errMessage);
	}
}

void WindowsCommon::GetTrusteeNamesFromPACL(PACL pacl, StringSet *trusteeNames) {

	ACL_SIZE_INFORMATION size_info;

	if (GetAclInformation(pacl, &size_info, sizeof(size_info), AclSizeInformation)) {

		for(unsigned int aceIdx = 0; aceIdx < size_info.AceCount; aceIdx++) {
			void *ace = NULL;

			if(GetAce(pacl, aceIdx, &ace)) {

				ACE_HEADER *header = (ACE_HEADER *)ace;
				PSID psid = NULL;

				// obviously this is not exhaustive.  We will need to add more
				// ACE types as we discover them in use.  Or be really complete
				// and handle them all...
				switch (header->AceType) {
				case ACCESS_ALLOWED_ACE_TYPE:
					psid = (PSID)&((ACCESS_ALLOWED_ACE *)ace)->SidStart;
					break;
				case ACCESS_DENIED_ACE_TYPE:
					psid = (PSID)&((ACCESS_DENIED_ACE *)ace)->SidStart;
					break;
				case SYSTEM_AUDIT_ACE_TYPE:
					psid = (PSID)&((SYSTEM_AUDIT_ACE *)ace)->SidStart;
					break;
				case SYSTEM_MANDATORY_LABEL_ACE_TYPE:
					psid = (PSID)&((SYSTEM_MANDATORY_LABEL_ACE *)ace)->SidStart;
					break;
				default:
					//TODO skip for now
					Log::Debug("Unsupported AceType found when getting sids from acl.");
				}

				if(IsValidSid(psid)) {

                    trusteeNames->insert(WindowsCommon::GetFormattedTrusteeName(psid));

				} else {
					throw Exception("Invalid Sid found when getting Trustee Names from an ACL.");
				}

			} else {

                string errMessage = WindowsCommon::GetErrorMessage(GetLastError());
				throw Exception("Error while getting the ACE from the ACL. " + errMessage);
            }
		}

	} else {
        string errMessage = WindowsCommon::GetErrorMessage(GetLastError());
		throw Exception("Could not retrieve ace information from acl " + errMessage);
	}
}

bool WindowsCommon::LookUpTrusteeName(string* accountNameStr, string* sidStr, string* domainStr, bool *isGroup) {

	FreeGuard<> psid;
	FreeGuard<TCHAR> domain;
	DWORD sidSize = 128;
	DWORD domainSize = 128;
	DWORD error;
	SID_NAME_USE sid_type = SidTypeUnknown;
	BOOL retVal = FALSE;

	do {
		// Initial memory allocations for the SID and DOMAIN.
		if (!psid.realloc(sidSize))
			throw Exception(string("realloc: ")+strerror(errno));

		if (!domain.realloc(domainSize))
			throw Exception(string("realloc: ")+strerror(errno));

		// Call LookupAccountName to get the SID.
		retVal = LookupAccountName(NULL,										// system name
								   accountNameStr->c_str(),	// account name
								   psid.get(),										// security identifier
								   &sidSize,									// size of security identifier
								   domain.get(),										// domain name
								   &domainSize,									// size of domain name
								   &sid_type);									// SID-type indicator
		error = GetLastError();
	} while (!retVal && error == ERROR_INSUFFICIENT_BUFFER);

	if(!retVal) {
		if (error == ERROR_NONE_MAPPED)
			return false;
		if(error == ERROR_TRUSTED_RELATIONSHIP_FAILURE) {
			throw Exception("Unable to locate account: " + (*accountNameStr) + ". " + WindowsCommon::GetErrorMessage(error), ERROR_NOTICE);
		} else {
			throw Exception("Error failed to look up account: " + (*accountNameStr) + ". " + WindowsCommon::GetErrorMessage(error));
		}
	}

	(*domainStr) = domain.get();

	if (!WindowsCommon::GetTextualSid(psid.get(), sidStr))
		throw Exception("Error converting SID to string: " + GetErrorMessage(GetLastError()));

	// make sure account names are consistently formated
	if(sid_type == SidTypeUser) {
		// make sure all user accounts are prefixed by their domain or the local system name.
		if((*accountNameStr).find("\\") == string::npos && (*domainStr).compare("") != 0)
			(*accountNameStr) = (*domainStr) + "\\" + (*accountNameStr);

	} else if(sid_type == SidTypeDomain) {
		// do not prepend the domain if it is a domain...

	} else {
		// make sure all local group accounts are prefixed by their domain
		// do not prefix if domain is "BUILTIN" "NT AUTHORITY"
		if((*domainStr).compare("BUILTIN") != 0 && (*domainStr).compare("NT AUTHORITY") != 0) {
			if((*accountNameStr).find("\\") == string::npos && (*domainStr).compare("") != 0)
				(*accountNameStr) = (*domainStr) + "\\" + (*accountNameStr);
		}
	}

	*isGroup = IsAccountGroup(sid_type, *accountNameStr);

	return true;
}

bool WindowsCommon::IsAccountGroup(SID_NAME_USE sidType, string accountName) {
	
	// No solid quick way found to determine if a sid is a group - using this along with local/global group member apis which flag an error

	if ((sidType == SidTypeGroup) || (sidType == SidTypeWellKnownGroup) || (sidType == SidTypeAlias)) {	
		return ((accountName.compare("SYSTEM") != 0) && (accountName.compare("NETWORK SERVICE"))); // special cases...
	}
	
	return false;
}

bool WindowsCommon::LookUpTrusteeSid(string sidStr, string* pAccountNameStr, string* pDomainStr, bool *isGroup) {

	PSID pSid = NULL;
	FreeGuard<TCHAR> pDomain;
	FreeGuard<TCHAR> pAccountName;
	DWORD accountNameSize = 128;
	DWORD domainSize = 128;
	SID_NAME_USE sid_type = SidTypeUnknown;
	BOOL retVal = FALSE;

	if(ConvertStringSidToSid(sidStr.c_str(), &pSid) == 0)
		throw Exception("Error encountered converting SID string to a SID.");

	do {
		// Initial memory allocations for the ACCOUNT and DOMAIN.
		if (!pAccountName.realloc(accountNameSize)) {
			LocalFree(pSid);
			throw Exception(string("realloc: ")+strerror(errno));
		}

		if (!pDomain.realloc(domainSize)) {
			LocalFree(pSid);
			throw Exception(string("realloc: ")+strerror(errno));
		}

		// Call LookupAccountSid to get the account name and domain.
		retVal = LookupAccountSid(NULL,							// system name
								  pSid,							// security identifier
								  pAccountName.get(),					// account name
								  &accountNameSize,				// security identifier
								  pDomain.get(),						// domain name
								  &domainSize,					// size of domain name
								  &sid_type);					// SID-type indicator

	} while (!retVal && GetLastError() == ERROR_INSUFFICIENT_BUFFER);

	LocalFree(pSid);
	if(!retVal) {
		DWORD error = GetLastError();
		if (error == ERROR_NONE_MAPPED)
			return false;
		if(error == ERROR_TRUSTED_RELATIONSHIP_FAILURE) {
			throw Exception("Unable to locate account: " + sidStr + ". " + WindowsCommon::GetErrorMessage(error), ERROR_NOTICE);
		} else {
			throw Exception("Error failed to look up account: " + sidStr + ". " + WindowsCommon::GetErrorMessage(error));
		}
	}

	(*pAccountNameStr) = pAccountName.get();
	(*pDomainStr) = pDomain.get();

	// make sure account names are consistently formated
	if(sid_type == SidTypeUser) {
		// make sure all user accounts are prefixed by their domain or the local system name.
		if((*pAccountNameStr).find("\\") == string::npos && (*pDomainStr).compare("") != 0)
			(*pAccountNameStr) = (*pDomainStr) + "\\" + (*pAccountNameStr);

	} else if(sid_type == SidTypeDomain) {
		// do not prepend the domain if it is a domain...

	} else {
		// make sure all local group accounts are prefixed by their domain
		// do not prefix if domain is "BUILTIN" "NT AUTHORITY"
		if((*pDomainStr).compare("BUILTIN") != 0 && (*pDomainStr).compare("NT AUTHORITY") != 0) {
			if((*pAccountNameStr).find("\\") == string::npos && (*pDomainStr).compare("") != 0)
				(*pAccountNameStr) = (*pDomainStr) + "\\" + (*pAccountNameStr);
		}
	}

	*isGroup = IsAccountGroup(sid_type, *pAccountNameStr);
	return true;
}

bool WindowsCommon::NormalizeTrusteeName(const string &trusteeName, 
	string *normName, string *normDomain, string *normFormattedName,
	string *sidStr, bool *isGroup) {

	FreeGuard<> sid;
	DWORD sidSizeBytes = 128;
	FreeGuard<TCHAR> domain;
	DWORD domainSizeChars = 128;
	FreeGuard<TCHAR> normNameBuf;
	DWORD normNameSizeChars = 128;
	SID_NAME_USE trusteeType, notused2;
	BOOL retVal;
	DWORD err;

	// API weirdness...  Docs say that LookupAccount*() will return
	// the required domain buffer size, in TCHARS, if the call fails
	// (with error ERROR_INSUFFICIENT_BUFFER).  It does not say what
	// happens when the call succeeds.  From what I've observed, I think
	// it returns the length of the domain string (not including the 
	// terminal null).  Therefore the sixth param can get two different 
	// values depending on whether the call succeeded.  Now, lets say 
	// you want to call the LookupAccount*() function twice, like I'm 
	// doing here.  You can't pass the address of your real buffer size
	// var, because it'll get clobbered, and if the call succeeds, it 
	// won't even get clobbered with a value you can use to pass into
	// the second call (it'll be one TCHAR short).  So I'm passing in the
	// address of a temp variable, and only updating the real buffer size
	// variable if the call failed with ERROR_INSUFFICIENT_BUFFER.
	// Because on success I'll get an undocumented value I don't really
	// need (and don't trust).

	// name -> sid
	do {
		if (!domain.realloc(domainSizeChars * sizeof(TCHAR)))
			throw Exception(string("realloc: ")+strerror(errno));
		if (!sid.realloc(sidSizeBytes))
			throw Exception(string("realloc: ")+strerror(errno));

		DWORD tmp = domainSizeChars;
		retVal = LookupAccountName(NULL,
								  trusteeName.c_str(),
								  sid.get(),
								  &sidSizeBytes,
								  domain.get(),
								  &tmp,
								  &trusteeType);

		err = GetLastError();
		if (!retVal && err == ERROR_INSUFFICIENT_BUFFER)
			domainSizeChars = tmp;

	} while (!retVal && err == ERROR_INSUFFICIENT_BUFFER);

	if (!retVal) {
		if (err == ERROR_NONE_MAPPED)
			return false;

		throw Exception("Normalization of trustee name \""+trusteeName+"\" failed: " + WindowsCommon::GetErrorMessage(err));
	}

	if (normDomain)
		*normDomain = domain.get();

	if (sidStr)
		if (!GetTextualSid(sid.get(), sidStr))
			throw Exception("Couldn't convert SID to string: " + GetErrorMessage(GetLastError()));

	// if no further normalization requested, we're done.
	if (!normName && !normFormattedName && !isGroup)
		return true;

	// sid -> name
	do {
		if (!normNameBuf.realloc(normNameSizeChars * sizeof(TCHAR)))
			throw Exception(string("realloc: ")+strerror(errno));

		// if the domain buf was big enough for LookupAccountName() to
		// succeed, it ought to be big enough for this to succeed!  So
		// I shouldn't need to realloc that buffer...
		retVal = LookupAccountSid(NULL,
								  sid.get(),
								  normNameBuf.get(),
								  &normNameSizeChars,
								  domain.get(),
								  &domainSizeChars,
								  &notused2);
		err = GetLastError();

	} while (!retVal && err == ERROR_INSUFFICIENT_BUFFER);

	if (!retVal) {
		if (err == ERROR_NONE_MAPPED)
			return false;

		throw Exception("Normalization of trustee name \""+trusteeName+"\" failed: " + WindowsCommon::GetErrorMessage(err));
	}

	if (normName)
		*normName = normNameBuf.get();

	if (normFormattedName) {
		if(trusteeType == SidTypeUser) {
			// all user accounts are prefixed by their domain or the local system name.
			// ... except for this requested hackage: leave the domain part off the
			// local standard Administrator and Guest accounts!
			if ((IsWellKnownSid(sid.get(), WinAccountAdministratorSid) ||
				IsWellKnownSid(sid.get(), WinAccountGuestSid)) && isLocal(sid.get()))
				*normFormattedName = normNameBuf.get();
			else
				*normFormattedName = string(domain.get()) + '\\' + normNameBuf.get();
		} else if(trusteeType == SidTypeDomain) {
			// do not prepend the domain if it is a domain...
			*normFormattedName = domain.get();
		} else {
			// make sure all local group accounts are prefixed by their domain
			// except if domain is "BUILTIN" or "NT AUTHORITY"
			if(!strcmp(domain.get(), "BUILTIN") || !strcmp(domain.get(), "NT AUTHORITY"))
				*normFormattedName = normNameBuf.get();
			else
				*normFormattedName = string(domain.get()) + '\\' + normNameBuf.get();
		}
	}

	if (isGroup)
		*isGroup = IsAccountGroup(trusteeType, normNameBuf.get());

	return true;
}

string WindowsCommon::ToString(FILETIME fTime) {
	ULONGLONG fTimeResult = (((ULONGLONG)fTime.dwHighDateTime)<<32) + fTime.dwLowDateTime;
	return Common::ToString(fTimeResult);
}

bool WindowsCommon::TrusteeNameExists(const string trusteeNameIn) {

    bool trusteeNameExists = false;

	FreeGuard<> psid;
	FreeGuard<TCHAR> domain;
	DWORD sidSize = 128;
	DWORD domainSize = 128;
	SID_NAME_USE sid_type;
	BOOL retVal = FALSE;

	do {
		// Initial memory allocations for the SID and DOMAIN.
		if (!psid.realloc(sidSize))
			throw Exception(string("realloc: ")+strerror(errno));

		if (!domain.realloc(domainSize))
			throw Exception(string("realloc: ")+strerror(errno));

		// Call LookupAccountName to get the SID.
		retVal = LookupAccountName(NULL,										// system name
								   trusteeNameIn.c_str(),	// account name
								   psid.get(),										// security identifier
								   &sidSize,									// size of security identifier
								   domain.get(),										// domain name
								   &domainSize,									// size of domain name
								   &sid_type);									// SID-type indicator
		
	} while (!retVal && GetLastError() == ERROR_INSUFFICIENT_BUFFER);

	if(retVal == TRUE) {
	    trusteeNameExists = true;
	} else {

        DWORD error = GetLastError();
        if(error == ERROR_NONE_MAPPED || error == ERROR_INVALID_HANDLE) {
            trusteeNameExists = false;
        } else if(error == ERROR_TRUSTED_RELATIONSHIP_FAILURE) {
			throw Exception("Unable to locate account: " + trusteeNameIn + ". " + WindowsCommon::GetErrorMessage(error), ERROR_NOTICE);
		} else {
            string err = WindowsCommon::GetErrorMessage(error) ;
			throw Exception("Error failed to look up account: " + trusteeNameIn + ". " + WindowsCommon::GetErrorMessage(error));
		}
	}

    return trusteeNameExists;
}

bool WindowsCommon::TrusteeSIDExists(const string trusteeSIDIn) {

    bool sidExists = false;

    PSID pSid = NULL;
	FreeGuard<TCHAR> pAccountName;
	DWORD accountNameSize = 128;
	DWORD domainSize = 128;
	SID_NAME_USE sid_type;
	BOOL retVal = FALSE;

	if(ConvertStringSidToSid(trusteeSIDIn.c_str(), &pSid) == 0)
		throw Exception("Error encountered converting SID string to a SID.");

	do {
		// Initial memory allocations for the ACCOUNT and DOMAIN.
		if (!pAccountName.realloc(accountNameSize)) {
			LocalFree(pSid);
			throw Exception(string("realloc: ")+strerror(errno));
		}

		// Call LookupAccountSid to get the account name and domain.
		retVal = LookupAccountSid(NULL,							// system name
								  pSid,							// security identifier
								  pAccountName.get(),					// account name
								  &accountNameSize,				// security identifier
								  NULL,						// domain name
								  &domainSize,					// size of domain name
								  &sid_type);					// SID-type indicator

	} while (!retVal && GetLastError() == ERROR_INSUFFICIENT_BUFFER);

	LocalFree(pSid);

	if(retVal == TRUE) {
        sidExists = true;
	} else {
			
		DWORD error = GetLastError();
        if(error == ERROR_NONE_MAPPED) {
            sidExists = false;
        } else if(error == ERROR_TRUSTED_RELATIONSHIP_FAILURE) {
			throw Exception("Unable to locate account: " + trusteeSIDIn + ". " + WindowsCommon::GetErrorMessage(error), ERROR_NOTICE);
		} else {
			throw Exception("Error failed to look up account: " + trusteeSIDIn + ". " + WindowsCommon::GetErrorMessage(error));
		}
	}

    return sidExists;
}

bool WindowsCommon::GetGroupsForUser(string userNameIn, StringSet* groups) {

	bool userExists = false;

	// Split username into domain and account name components
	string domain;
	string userName;
	WindowsCommon::SplitTrusteeName(userNameIn,&domain,&userName);
	
	// original form might be qualified or unqualified.
	wstring originalFormUserName = WindowsCommon::StringToWide(userNameIn);
	// ... so this might be the same as originalFormUserName.
	wstring unqualifiedUserName = WindowsCommon::StringToWide(userName);
	wstring serverName;
	DWORD dwEntriesRead;
	DWORD dwTotalEntries;
	LPGROUP_USERS_INFO_0 pBuf = NULL;
	string shortGroupName;

	// don't get a controller if the domain is the local computer.
	// There is no controller for that.
	if (!domain.empty()) {
		wstring domainWide = StringToWide(domain);
		if (!Common::EqualsIgnoreCase(domainWide, globals::computerName))
			 serverName = WindowsCommon::GetDomainControllerName(domain);
	}

	// Call the NetUserGetGroups function, specifying level 0.
	// This function doesn't support a domain prefix on the
	// username.
	NET_API_STATUS nStatus = NetUserGetGroups(
		serverName.empty() ? NULL : serverName.c_str(),
		unqualifiedUserName.c_str(),
		0,
		(LPBYTE*)&pBuf,
		MAX_PREFERRED_LENGTH,
		&dwEntriesRead,
		&dwTotalEntries);

	NetApiBufferFreeGuard globalGrpDataGuard(pBuf);

	// If the call succeeds,
	if (nStatus == NERR_Success) {

		// report an error if all groups are not listed.
		if (dwEntriesRead < dwTotalEntries)
			throw Exception("Unable to get all global groups for user: " + userNameIn);

		userExists = true;
		if (pBuf != NULL) {

			// Loop through the entries; 
			//  print the name of the global groups 
			//  to which the user belongs.
			for (DWORD i = 0; i < dwEntriesRead; i++) {
				// These trustees were found on the domain controller,
				// if one was given, so surely they're all domain groups
				// which require the same domain-qualification.
				string tmpName = UnicodeToAsciiString(pBuf[i].grui0_name);
				if (!domain.empty())
					tmpName = domain + '\\' + tmpName;

				string normName;
				if (NormalizeTrusteeName(tmpName, NULL, NULL, &normName,
					NULL, NULL))
					groups->insert(normName);
			}
		}
	} else if(nStatus != NERR_UserNotFound) {
		throw Exception("Unable to get all global groups for user " + userNameIn + ": NetUserGetGroups: " +
			GetErrorMessage(nStatus));
	}

	//
	// get the local groups for the user
	//
	LPLOCALGROUP_USERS_INFO_0 pLocalBuf = NULL;
	dwEntriesRead = 0;
	dwTotalEntries = 0;

	//  Call the NetUserGetLocalGroups function 
	//  specifying information level 0.
	//
	//   The LG_INCLUDE_INDIRECT flag specifies that the 
	//   function should also return the names of the local 
	//   groups in which the user is indirectly a member.
	//
	// This function does support a domain prefix on the username.
	nStatus = NetUserGetLocalGroups(NULL,
									originalFormUserName.c_str(),
									0,
									LG_INCLUDE_INDIRECT,
									(LPBYTE *) &pLocalBuf,
									MAX_PREFERRED_LENGTH,
									&dwEntriesRead,
									&dwTotalEntries);
	
	NetApiBufferFreeGuard localGrpDataGuard(pLocalBuf);

	// If the call succeeds
	if (nStatus == NERR_Success) {

		// report an error if all groups are not listed
		if (dwEntriesRead < dwTotalEntries) {
			throw Exception("Unable to get all local groups for user: " + userName);
		}

		userExists = true;
		if (pLocalBuf != NULL) {

			//  Loop through the entries and 
			//  print the names of the local groups 
			//  to which the user belongs. 
			for (DWORD i = 0; i < dwEntriesRead; i++) {
				string normName;
				if (NormalizeTrusteeName(
					UnicodeToAsciiString(pLocalBuf[i].lgrui0_name),
					NULL, NULL, &normName, NULL, NULL))
					groups->insert(normName);
			}
		}
		
	} else if (nStatus != NERR_UserNotFound &&
				nStatus != ERROR_NO_SUCH_DOMAIN) {
		throw Exception("Unable to get all local groups for user " + 
			userName + ": NetUserGetLocalGroups: " + GetErrorMessage(nStatus));
	}

	return userExists;
}

bool WindowsCommon::GetAccountInfo(const string &accountName, bool *enabled, DWORD *lastLogon) {
	string domain;
	string username;
	WindowsCommon::SplitTrusteeName(accountName,&domain,&username);
	wstring serverName;
	wstring usernameWide = WindowsCommon::StringToWide(username);
	LPUSER_INFO_2 pBuf = NULL;

	// don't get a controller if the domain is the local computer.
	// There is no controller for that.
	if (!domain.empty()) {
		wstring domainWide = StringToWide(domain);
		if (!Common::EqualsIgnoreCase(domainWide, globals::computerName))
			 serverName = WindowsCommon::GetDomainControllerName(domain);
	}

	NET_API_STATUS nStatus = NetUserGetInfo(
		serverName.empty() ? NULL : serverName.c_str(),
		usernameWide.c_str(),
		2,
		(LPBYTE *)&pBuf);

	NetApiBufferFreeGuard userInfoGuard(pBuf);

	if (nStatus) {
		if (nStatus == NERR_UserNotFound)
			return false;
		throw Exception("Error getting account info for " + 
			accountName + ": NetUserGetInfo: " + GetErrorMessage(nStatus));
	}

	if (enabled)
		*enabled = !(pBuf->usri2_flags & UF_ACCOUNTDISABLE);
	if (lastLogon)
		*lastLogon = pBuf->usri2_last_logon;

	return true;
}


void WindowsCommon::GetEffectiveRightsForWindowsObject(SE_OBJECT_TYPE objectType, PSID pSid, HANDLE objHandle, PACCESS_MASK pAccessRights) {	

	if(WindowsCommon::IsXPOrLater()) {
		WindowsCommon::GetEffectiveRightsForWindowsObjectAuthz(objectType, pSid, objHandle, pAccessRights);
	} else {
		WindowsCommon::GetEffectiveRightsForWindowsObjectAcl(objectType, pSid, objHandle, pAccessRights);
	}
}



void WindowsCommon::GetEffectiveRightsForWindowsObjectAcl(SE_OBJECT_TYPE objectType, PSID pSid, HANDLE objHandle, PACCESS_MASK pAccessRights) {
	// -----------------------------------------------------------------------
	//
	//  ABSTRACT
	//
	//	Return a populated item for the specified trustees on the specified file.
	//
	//	- Call GetNamedSecurityInfo to get a DACL Security Descriptor for the file
	//	  http://msdn2.microsoft.com/en-us/library/aa446645.aspx
	//	- Use provided trustee name and call LsaLookupNames to get the sid
	//	  http://msdn2.microsoft.com/en-us/library/ms721797.aspx
	//	- Then call GetEffectiveRightsFromAcl with the dacl and the sid found in the earlier calls
	//	  http://msdn2.microsoft.com/en-us/library/aa446637.aspx
	// -----------------------------------------------------------------------

	Log::Debug("Calling the acl api to get effective rights");

	string sidStr;
	if (!GetTextualSid(pSid, &sidStr)) {
		Log::Message("GetEffectiveRightsForWindowsObjectAcl: Couldn't convert SID to string: " + 
			GetErrorMessage(GetLastError()));
		sidStr = "(could not convert sid)";
	}

	string baseErrMsg = "Error unable to get effective rights for trustee: " + sidStr;

	DWORD res;
	PACL pdacl;
	PSECURITY_DESCRIPTOR pSD;

	res = GetSecurityInfo(objHandle,							// object name
						  objectType,							// object type
						  DACL_SECURITY_INFORMATION |			// information type
						  PROTECTED_DACL_SECURITY_INFORMATION |
						  UNPROTECTED_DACL_SECURITY_INFORMATION, 			
						  NULL,									// owner SID
						  NULL,									// primary group SID
						  &pdacl,								// DACL
						  NULL,									// SACL
						  &pSD);								// Security Descriptor

	if (res != ERROR_SUCCESS) {	
		throw Exception( baseErrMsg + " Unable to retrieve a copy of the security descriptor. Microsoft System Error " + Common::ToString ( res ) + ") - " + WindowsCommon::GetErrorMessage ( res ) );
	} 


	// Check to see if a valid security descriptor was returned.  
    if ((IsValidSecurityDescriptor(pSD) == 0) || (IsValidAcl(pdacl) == 0)) {
		LocalFree(pSD);
		throw Exception(baseErrMsg + " Invalid data returned from call to GetSecurityInfo().");
	}


	// build the trustee structure
	TRUSTEE trustee = {0};
	BuildTrusteeWithSid(&trustee, pSid);

	// get the rights
	res = GetEffectiveRightsFromAcl(pdacl,
									&trustee,
									pAccessRights);
	if (res != ERROR_SUCCESS) {
		
		string errMsg = WindowsCommon::GetErrorMessage(res);		

		LocalFree(pSD);
		
		throw Exception(baseErrMsg + " System error message: " + errMsg); 		
	} 
		
	LocalFree(pSD);
	pSD = NULL;

	Log::Debug("Finished calling the acl api to get effective rights");
}

void WindowsCommon::GetEffectiveRightsForWindowsObjectAuthz(SE_OBJECT_TYPE objectType, PSID pSid, HANDLE objHandle, PACCESS_MASK pAccessRights) {	
	// -----------------------------------------------------------------------
	//
	//  ABSTRACT
	//
	//	Return a populated item for the specified trustees on the specified file.
	//
	//	- Call GetNamedSecurityInfo to get a Security Descriptor for the file
	//	  http://msdn2.microsoft.com/en-us/library/aa446645.aspx
	//	- Initialize resource manager through call to AuthzInitializeResourceManager
	//	  http://msdn.microsoft.com/en-us/library/aa376313(VS.85).aspx
	//	- Initialize client context using supplied SID through call to AuthzInitializeContextFromSid
	//	  http://msdn.microsoft.com/en-us/library/aa376309(VS.85).aspx
	//  - Finally retrieve the rights mask throuogh a call to AuthzAccessCheck
	//    http://msdn.microsoft.com/en-us/library/aa375788(VS.85).aspx
	//
	// -----------------------------------------------------------------------

	Log::Debug("Calling the authz api to get effective rights");

	PSECURITY_DESCRIPTOR pSD = NULL;	
	AUTHZ_CLIENT_CONTEXT_HANDLE hClientContext = NULL;	
	AUTHZ_RESOURCE_MANAGER_HANDLE hAuthzResourceManager = NULL;	

	DWORD res = GetSecurityInfo(objHandle,// object name
								objectType,						// object type
								DACL_SECURITY_INFORMATION |			// information type
								GROUP_SECURITY_INFORMATION |
								OWNER_SECURITY_INFORMATION,
								NULL,								   // owner SID
								NULL,								   // primary group SID
								NULL,								   // DACL
								NULL,								   // SACL
								&pSD);								   // Security Descriptor

	if (res != ERROR_SUCCESS) {
		throw Exception("Unable to retrieve a copy of the security descriptor. Microsoft System Error " + Common::ToString ( res ) + ") - " + WindowsCommon::GetErrorMessage ( res ) );
	} 

	// Check to see if a valid security descriptor was returned.  
    if ((IsValidSecurityDescriptor(pSD) == 0)) {
		LocalFree(pSD);
		throw Exception("Invalid data returned from call to GetSecurityInfo().");
	}

	try	{
	
		if(AuthzInitializeResourceManager(AUTHZ_RM_FLAG_NO_AUDIT, NULL, NULL, NULL, NULL, &hAuthzResourceManager) != 0) {

			LUID id = { 0 };

			// From description AUTHZ_SKIP_TOKEN_GROUPS <seems> correct
			if(AuthzInitializeContextFromSid(AUTHZ_SKIP_TOKEN_GROUPS, pSid, hAuthzResourceManager, NULL, id, NULL, &hClientContext) != 0) {						

				AUTHZ_ACCESS_REQUEST request = {0};
				DWORD errorNumber  = ERROR_ACCESS_DENIED; 

				request.DesiredAccess = MAXIMUM_ALLOWED;

				request.PrincipalSelfSid = NULL;
				request.ObjectTypeList = NULL;
				request.ObjectTypeListLength = 0;
				request.OptionalArguments = NULL;

				AUTHZ_ACCESS_REPLY reply = {0};
				ACCESS_MASK replyMask = 0;
				reply.GrantedAccessMask = &replyMask;
				reply.ResultListLength = 1;
				reply.Error = &errorNumber;				

				if(AuthzAccessCheck(0, hClientContext, &request, NULL, pSD, NULL, 0, &reply, NULL) == TRUE) {

					*pAccessRights = reply.GrantedAccessMask[0];
				} else {
					throw Exception("Failure to perform Authz Check: " + string(_com_error(GetLastError()).ErrorMessage()));
				}

			} else { 
				throw Exception("Failure to initialize context from SID: " + string(_com_error(GetLastError()).ErrorMessage()));
			}

		} else {
			throw Exception("Can't init the resource manager");
		}

	} catch(...) {

		if(hAuthzResourceManager != NULL) {
			AuthzFreeResourceManager(hAuthzResourceManager);
		}

		if(hClientContext != NULL) {
			AuthzFreeContext(hClientContext);
		}

		if(pSD != NULL) {
			LocalFree(pSD);
		}

		throw;
	}

	if(hAuthzResourceManager != NULL) {
		AuthzFreeResourceManager(hAuthzResourceManager);
	}
	
	if(hClientContext != NULL) {
		AuthzFreeContext(hClientContext);
	}

	if(pSD != NULL) {
		LocalFree(pSD);
	}

	Log::Debug("Finished calling the authz api to get effective rights");
}

void WindowsCommon::GetAuditedPermissionsForWindowsObject ( SE_OBJECT_TYPE objectType, PSID pSid, HANDLE objHandle, PACCESS_MASK pSuccessfulAuditedPermissions, PACCESS_MASK pFailedAuditPermissions ) {
    Log::Debug ( "Calling the ACL API to get the audited permissions" );
	string sidStr;
	if (!GetTextualSid(pSid, &sidStr)) {
		Log::Message("GetAuditedPermissionsForWindowsObject: Couldn't convert SID to string: " + 
			GetErrorMessage(GetLastError()));
		sidStr = "(could not convert sid)";
	}

	string baseErrMsg = "Error: Unable to get audited permissions for trustee: " + sidStr + ".";
    DWORD res;
    PACL psacl;
	PSECURITY_DESCRIPTOR sd;

    res = GetSecurityInfo ( objHandle,                                        // object name
                            objectType,								          // object type
                            SACL_SECURITY_INFORMATION |					      // information type
                            PROTECTED_SACL_SECURITY_INFORMATION |
                            UNPROTECTED_SACL_SECURITY_INFORMATION,
                            NULL,                                             // owner SID
                            NULL,                                             // primary group SID
                            NULL,                                             // DACL
                            &psacl,                                           // SACL
                            &sd );                                           // Security Descriptor

    if ( res != ERROR_SUCCESS ) {
        throw Exception ( baseErrMsg + " Unable to retrieve a copy of the security descriptor. Microsoft System Error (" + Common::ToString ( res ) + ") - " + WindowsCommon::GetErrorMessage ( res ) );
    }
	
    ULONG size;
    EXPLICIT_ACCESS* entries = NULL;

    if ( ( res = GetExplicitEntriesFromAcl ( psacl, &size, &entries ) ) == ERROR_SUCCESS ) {

        for ( unsigned int i = 0 ; i < size ; i++ ) {
            PSID sid = entries[i].Trustee.ptstrName;

			if ( entries[i].Trustee.TrusteeForm == TRUSTEE_IS_SID && EqualSid(pSid, sid) ) {
                if ( entries[i].grfAccessMode == SET_AUDIT_SUCCESS ) {
                    *pSuccessfulAuditedPermissions = entries[i].grfAccessPermissions;
                }

                if ( entries[i].grfAccessMode == SET_AUDIT_FAILURE ) {
                    *pFailedAuditPermissions = entries[i].grfAccessPermissions;
                }
            }
        }

		LocalFree ( entries );
        entries = NULL;

    } else {
        string errMsg = WindowsCommon::GetErrorMessage ( res );
		throw Exception ( baseErrMsg + " System error message: " + errMsg );
    }

    Log::Debug ( "Finished calling the ACL API to get the audited permissions" );
}

bool WindowsCommon::IsVistaOrLater() {

	OSVERSIONINFOEX osvi;
	DWORDLONG dwlConditionMask = 0;

	// Initialize the OSVERSIONINFOEX structure.

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	osvi.dwMajorVersion = 6;
	osvi.dwMinorVersion = 0;
	osvi.wServicePackMajor = 0;
	osvi.wServicePackMinor = 0;

	// Initialize the condition mask.

	VER_SET_CONDITION( dwlConditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL );
	VER_SET_CONDITION( dwlConditionMask, VER_MINORVERSION, VER_GREATER_EQUAL );
	VER_SET_CONDITION( dwlConditionMask, VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL );
	VER_SET_CONDITION( dwlConditionMask, VER_SERVICEPACKMINOR, VER_GREATER_EQUAL );

	// Perform the test.
    BOOL retVal = VerifyVersionInfo( &osvi, 
									  VER_MAJORVERSION | VER_MINORVERSION | 
									  VER_SERVICEPACKMAJOR | VER_SERVICEPACKMINOR,
									  dwlConditionMask);

	
	if(retVal == TRUE) {
		return true;
	} else {
		DWORD error = GetLastError();
		if(error == ERROR_OLD_WIN_VERSION) {
			return false;
		} else {
			throw Exception("IsVistaOrLater - Error while checking windows version. " + WindowsCommon::GetErrorMessage(error));
		}
	}
}

bool WindowsCommon::IsXPOrLater() {
	
	OSVERSIONINFOEX osvi;
	DWORDLONG dwlConditionMask = 0;

	// Initialize the OSVERSIONINFOEX structure.

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	osvi.dwMajorVersion = 5;
	osvi.dwMinorVersion = 1;
	osvi.wServicePackMajor = 0;
	osvi.wServicePackMinor = 0;

	// Initialize the condition mask.

	VER_SET_CONDITION( dwlConditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL );
	VER_SET_CONDITION( dwlConditionMask, VER_MINORVERSION, VER_GREATER_EQUAL );
	VER_SET_CONDITION( dwlConditionMask, VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL );
	VER_SET_CONDITION( dwlConditionMask, VER_SERVICEPACKMINOR, VER_GREATER_EQUAL );

	// Perform the test.
	BOOL retVal = VerifyVersionInfo( &osvi, 
									  VER_MAJORVERSION | VER_MINORVERSION | 
									  VER_SERVICEPACKMAJOR | VER_SERVICEPACKMINOR,
									  dwlConditionMask);

	
	if(retVal == TRUE) {
		Log::Debug("The version of windows is xp or later.");
		return true;
	} else {
		DWORD error = GetLastError();
		if(error == ERROR_OLD_WIN_VERSION) {
			Log::Debug("The version of windows is earlier than xp.");
			return false;
		} else {
			throw Exception("IsXPOrLater - Error while checking windows version. " + WindowsCommon::GetErrorMessage(error));
		}
	}
}

string WindowsCommon::UnicodeToAsciiString ( const wchar_t* unicodeCharStr ) {
    string asciiStr;
    size_t length = wcslen ( unicodeCharStr ) + 1;
    char* buffer = ( char* ) malloc ( sizeof ( char ) * length );

    if ( _snprintf ( buffer , length - 1 , "%S" , unicodeCharStr ) < 0 ) {
        asciiStr = "";

    } else {
        buffer[length-1] = '\0';
        asciiStr = buffer;
    }

    if ( buffer != NULL ) {
        free ( buffer );
        buffer = NULL;
    }

    return asciiStr;
}

string WindowsCommon::UnicodeToAsciiString ( const wstring &wstr ) {
	return UnicodeToAsciiString(wstr.c_str());
}

string WindowsCommon::UnicodeToAsciiString ( const string &str ) {
	return str;
}


bool WindowsCommon::UnicodeIsValidASCII(wchar_t* unicodeCharStr){
	for(unsigned int wcharInd = 0; wcharInd < wcslen(unicodeCharStr); wcharInd++) {
		if(!(iswascii(unicodeCharStr[wcharInd]))) {  //if not wide ASCII char
			return false;
		}
	}
	return true;
}

string WindowsCommon::GetObjectType ( SE_OBJECT_TYPE objectType ) {
    switch ( objectType ) {
        case SE_UNKNOWN_OBJECT_TYPE:
            return "SE_UNKNOWN_OBJECT_TYPE";
        case SE_FILE_OBJECT:
            return "SE_FILE_OBJECT";
        case SE_SERVICE:
            return "SE_SERVICE";
        case SE_PRINTER:
            return "SE_PRINTER";
        case SE_REGISTRY_KEY:
            return "SE_REGISTRY_KEY";
        case SE_LMSHARE:
            return "SE_LMSHARE";
        case SE_KERNEL_OBJECT:
            return "SE_KERNEL_OBJECT";
        case SE_WINDOW_OBJECT:
            return "SE_WINDOW_OBJECT";
        case SE_DS_OBJECT:
            return "SE_DS_OBJECT";
        case SE_DS_OBJECT_ALL:
            return "SE_DS_OBJECT_ALL";
        case SE_PROVIDER_DEFINED_OBJECT:
            return "SE_PROVIDER_DEFINED_OBJECT";
        case SE_WMIGUID_OBJECT:
            return "SE_WMIGUID_OBJECT";
        case SE_REGISTRY_WOW64_32KEY:
            return "SE_REGISTRY_WOW64_32KEY";
        default:
            return "";
    }
}

wstring WindowsCommon::GetDomainControllerName(string domainName){

	// backward compatibility... but maybe it doesn't make sense
	// to call this method on an empty string??
	if (domainName.empty())
		return L"";

	wstring domainNameWide = StringToWide(domainName);
	globals::DomainControllerMap::const_iterator iter = 
		globals::domainControllerHostnames.find(domainNameWide);
	if (iter != globals::domainControllerHostnames.end())
		return iter->second;

	wstring dc;
	LPBYTE domainControllerName = NULL;
	NET_API_STATUS nas;
	nas = NetGetAnyDCName(NULL, domainNameWide.c_str(), &domainControllerName);
	if(nas == NERR_Success) {
		dc =  (LPWSTR)domainControllerName;
		NetApiBufferFree(domainControllerName);
	} else
		Log::Debug("Couldn't get controller for domain \"" + domainName + 
			"\": NetGetAnyDCName: " + GetErrorMessage(nas));

	globals::domainControllerHostnames[domainNameWide] = dc;

	return dc;
}

bool WindowsCommon::GetActualPathWithCase(const string &path, string *casedPath) {

	if (path.empty()) {
		casedPath->clear();
		return true;
	}

// make not-found error detection consistent and cleaner...
// would be good to make use of this idea in the
// filefinder too, I think...
#define IS_NOT_FOUND_ERROR(err) \
	((err) == ERROR_FILE_NOT_FOUND || \
	(err) == ERROR_PATH_NOT_FOUND || \
	(err) == ERROR_NOT_READY)

	DWORD sz = 100, sz2;
	DWORD err;
	ArrayGuard<char> shortBuf(new char[sz]);

	// This is hard-coded to use the 'A' versions of the functions... since we
	// don't need wide-char paths I don't think.
	sz2 = GetShortPathNameA(path.c_str(), shortBuf.get(), sz);
	if (sz2 > sz) {
		sz = sz2;
		shortBuf.reset(new char[sz]);
		sz2 = GetShortPathNameA(path.c_str(), shortBuf.get(), sz);
		if (sz2 == 0) {
			err = GetLastError();
			if (IS_NOT_FOUND_ERROR(err))
				return false;
			throw Exception("GetShortPathName("+path+"): "+
				WindowsCommon::GetErrorMessage(err));
		} else if (sz2 > sz)
			// really shouldn't happen right???
			throw Exception("GetShortPathName("+path+
				"): incorrectly predicted space requirements");
	} else if (sz2 == 0) {
		err = GetLastError();
		if (IS_NOT_FOUND_ERROR(err))
			return false;
		throw Exception("GetShortPathName("+path+"): "+
			WindowsCommon::GetErrorMessage(err));
	}

	sz = 100;
	ArrayGuard<char> longBuf(new char[sz]);

	sz2 = GetLongPathNameA(shortBuf.get(), longBuf.get(), sz);
	if (sz2 > sz) {
		sz = sz2;
		longBuf.reset(new char[sz]);
		sz2 = GetLongPathNameA(shortBuf.get(), longBuf.get(), sz);
		if (sz2 == 0) {
			err = GetLastError();
			if (IS_NOT_FOUND_ERROR(err))
				return false;
			throw Exception("GetLongPathName("+path+"): "+
				WindowsCommon::GetErrorMessage(err));
		} else if (sz2 > sz)
			// really shouldn't happen right???
			throw Exception("GetLongPathName("+path+
				"): incorrectly predicted space requirements");
	} else if (sz2 == 0) {
		err = GetLastError();
		if (IS_NOT_FOUND_ERROR(err))
			return false;
		throw Exception("GetLongPathName("+path+"): "+
			WindowsCommon::GetErrorMessage(err));
	}

	// The short->long conversion doesn't seem to affect drive letters.
	// So let's just decide they should all be uppercase.
	if (strlen(longBuf.get()) >= 2 &&
		isalpha(longBuf[0]) &&
		longBuf[1] == ':')
		longBuf[0] = static_cast<char>(toupper(longBuf[0]));

	*casedPath = longBuf.get();
	return true;

#undef IS_NOT_FOUND_ERROR
}

bool WindowsCommon::IsWow64Process()
{
	typedef BOOL (WINAPI *WowFuncType)(HANDLE, PBOOL);
	WowFuncType wowFunc = (WowFuncType)GetProcAddress(
		GetModuleHandle(_T("kernel32")),
		"IsWow64Process");

	// treat unknown as false for now...
	if (!wowFunc)
	{
		Log::Debug("Couldn't determine WoW64 status: IsWow64Process() not supported.  Assuming no WoW64.");
		return false;
	}

	BOOL isWow64;
	BOOL result = wowFunc(GetCurrentProcess(), &isWow64);

	if (!result) {
		Log::Debug("Couldn't determine WoW64 status: " + GetErrorMessage(GetLastError()));
		return false;
	}

	return isWow64 != FALSE;
}

bool WindowsCommon::Is64BitOS()
{
#ifdef _WIN64
	// 64-bit apps can't run on other than 64-bit OS's.
	return true;
#else
	// lets cache this so I'm not computing it over and over...
	// It will never change during a run of the program.
	static bool cachedValue, valueCached = false;

	if (!valueCached) {
		// if we are running under WoW, this must be 64-bit OS.
		// Otherwise, 32-bit app + no WoW => it is 32-bit OS.
		cachedValue = IsWow64Process();
		valueCached = true;
	}

	return cachedValue;
#endif
}

BitnessView WindowsCommon::behavior2view(const string &viewStr) {
	if (viewStr == "32_bit")
		return BIT_32;
	if (viewStr == "64_bit")
		return BIT_64;
	throw Exception("Unrecognized windows_view value: "+viewStr);
}

BitnessView WindowsCommon::behavior2view(BehaviorVector *bv) {
	BitnessView schemaDefault = BIT_64;

	if (!bv)
		return schemaDefault;

	string viewStr = Behavior::GetBehaviorValue(bv, "windows_view");

	if (viewStr.empty())
		return schemaDefault;

	return behavior2view(viewStr);
}

namespace {
	bool isLocal(PSID sid) {
		// initialization must have completed first!
		assert(globals::computerSID);

		DWORD err;
		FreeGuard<> thisSidsDomain;
		DWORD thisSidsDomainSize = 0;
		if (!GetWindowsAccountDomainSid(sid, NULL, &thisSidsDomainSize))
		{
			err = GetLastError();
			if (err == ERROR_NON_ACCOUNT_SID)
				return true;
			else if (err != ERROR_INSUFFICIENT_BUFFER)
				throw Exception("GetWindowsAccountDomainSid: " +
					WindowsCommon::GetErrorMessage(err));
			
			if (!thisSidsDomain.realloc(thisSidsDomainSize))
				throw Exception("Out of memory!");
		}

		bool local;
		if (GetWindowsAccountDomainSid(sid, thisSidsDomain.get(), &thisSidsDomainSize))
			// !! eliminates warning C4800.  Docs also say you could append "!= 0"...
			local = !!EqualSid(globals::computerSID, thisSidsDomain.get());

		else {
			err = GetLastError();
			// but wouldn't we have already caught this error above??
			if (err != ERROR_NON_ACCOUNT_SID)
				throw Exception("GetWindowsAccountDomainSid: " +
					WindowsCommon::GetErrorMessage(err));

			local = true;
		}

		return local;
	}

	PSID GetSIDForTrusteeNameW(const wstring &trusteeName) {

		DWORD sidSize = 128;
		DWORD domainSize = 128;
		SID_NAME_USE sidUse;
		BOOL retVal = FALSE;
		FreeGuard<> pSid; // PSID is actually void*...
		FreeGuard<WCHAR> domain;
		DWORD err;
	
		do {
			// Initial memory allocations for the SID and DOMAIN.
			if (!pSid.realloc(sidSize))
				throw Exception(string("realloc: ")+strerror(errno));

			if (!domain.realloc(domainSize * sizeof(WCHAR)))
				throw Exception(string("realloc: ")+strerror(errno));

			// Call LookupAccountName to get the SID.
			retVal = LookupAccountNameW(NULL,				// system name NULL == localhost
									trusteeName.c_str(),	// account name
									pSid.get(),				// security identifier
									&sidSize,				// size of security identifier
									domain.get(),			// domain name
									&domainSize,			// size of domain name
									&sidUse);				// SID-type indicator
			err = GetLastError();

		} while (!retVal && err == ERROR_INSUFFICIENT_BUFFER);

		if (!retVal) {
			if (err == ERROR_NONE_MAPPED)
				return NULL;
			throw Exception("LookupAccountName failed: " + 
				WindowsCommon::GetErrorMessage(err));
		}

		return pSid.release();
	}
}
