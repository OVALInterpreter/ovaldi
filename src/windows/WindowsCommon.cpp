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

#include <cctype>
#include <cstring>
#include <memory>
#include <ArrayGuard.h>
#include "WindowsCommon.h"

StringSet* WindowsCommon::allLocalUserSIDs = NULL;
StringSet* WindowsCommon::allLocalGroupSIDs = NULL;
StringSet* WindowsCommon::allTrusteeNames = NULL;
StringSet* WindowsCommon::allTrusteeSIDs = NULL;
StringSet* WindowsCommon::wellKnownTrusteeNames = NULL;
const int NAME_BUFFER_SIZE = 255;

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
    while(errMsg.at(errMsg.length()-1) == '\n' || errMsg.at(errMsg.length()-1) == '\r')
        errMsg = errMsg.substr(0, errMsg.length()-1);

	return errMsg;
}

bool WindowsCommon::GetTextualSid(PSID pSid, LPTSTR* TextualSid) { 

    PSID_IDENTIFIER_AUTHORITY psia;
    DWORD dwSubAuthorities;
    DWORD dwSidRev = SID_REVISION;
    DWORD dwCounter;
    DWORD dwSidSize;

    // Validate the binary SID.

	if(!IsValidSid(pSid)) return false;

    // Get the identifier authority value from the SID.
	
	psia = GetSidIdentifierAuthority(pSid);

	// Get the number of subauthorities in the SID.

    dwSubAuthorities = *GetSidSubAuthorityCount(pSid);

    // compute buffer length
    // S-SID_REVISION- + identifierauthority- + subauthorities- + NULL

    dwSidSize=(15 + 12 + (12 * dwSubAuthorities) + 1) * sizeof(TCHAR);

    // allocate memory
 
	*TextualSid = (LPTSTR)malloc(dwSidSize);
	if(*TextualSid == NULL)
	{
		return false;
	}

    // Add 'S' prefix and revision number to the string.

    dwSidSize = wsprintf(*TextualSid, TEXT("S-%lu-"), dwSidRev);

    // Add SID identifier authority to the string.
 
    if ((psia->Value[0] != 0) || (psia->Value[1] != 0))
    {
        dwSidSize += wsprintf(*TextualSid + lstrlen(*TextualSid),
							  TEXT("0x%02hx%02hx%02hx%02hx%02hx%02hx"),
							  (USHORT)psia->Value[0],
							  (USHORT)psia->Value[1],
							  (USHORT)psia->Value[2],
							  (USHORT)psia->Value[3],
							  (USHORT)psia->Value[4],
							  (USHORT)psia->Value[5]);
    }
    else
    {
        dwSidSize += wsprintf(*TextualSid + lstrlen(*TextualSid),
							  TEXT("%lu"),
							  (ULONG)(psia->Value[5]) +
							  (ULONG)(psia->Value[4] << 8) +
							  (ULONG)(psia->Value[3] << 16) +
							  (ULONG)(psia->Value[2] << 24));
    }

    // Loop through SidSubAuthorities and add them to the string.

    for (dwCounter=0; dwCounter<dwSubAuthorities; dwCounter++)
    {
        dwSidSize += wsprintf(*TextualSid + dwSidSize,
							  TEXT("-%lu"),
			                  *GetSidSubAuthority(pSid, dwCounter));
    }

    return true;
}

bool WindowsCommon::ExpandGroup(string groupName, StringSet* members, bool includeSubGroups, bool resolveSubGroup) {

	// Need to determine if a local or global group.
	bool groupExists = false;
	
	try {
		groupExists = WindowsCommon::GetLocalGroupMembers(groupName, members, includeSubGroups, resolveSubGroup);
	} catch(Exception lex) {
		Log::Info("Problem retrieving local group members for " + groupName + ":" + lex.GetErrorMessage());
	}

	if(!groupExists) {
		
		try {
			groupExists = WindowsCommon::GetGlobalGroupMembers(groupName, members, includeSubGroups, resolveSubGroup);	
		} catch(Exception gex) {
			Log::Info("Problem retrieving global group members for " + groupName + ":" + gex.GetErrorMessage());
		}			
	}	

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
	}

	LocalFree(pSid);

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
	SID_NAME_USE sidUse;
	BOOL retVal = FALSE;
	PSID psid = NULL;
	LPTSTR domain = NULL;
	bool isGroup = false;

	do {
		// Initial memory allocations for the SID and DOMAIN.
		psid = (PSID)realloc(psid, sidSize);
		if (psid == NULL) {
			retVal = FALSE;
			break;
		}

		domain = (LPTSTR)realloc(domain, domainSize);
		if (domain == NULL) {
			retVal = FALSE;
			break;
		}

		// Call LookupAccountName to get the SID.
		retVal = LookupAccountName(NULL,								// system name NULL == localhost
								const_cast<char*>(trusteeName.c_str()),	// account name
								psid,									// security identifier
								&sidSize,								// size of security identifier
								domain,									// domain name
								&domainSize,							// size of domain name
								&sidUse);								// SID-type indicator
		
		if(retVal != FALSE) {
			isGroup = WindowsCommon::IsAccountGroup(sidUse, trusteeName);
			break;
		}
	} while (GetLastError() == ERROR_INSUFFICIENT_BUFFER);

	free(psid);
	free(domain);

	if(retVal == FALSE) {	
		DWORD error = GetLastError();
		if(error == ERROR_TRUSTED_RELATIONSHIP_FAILURE) {
			throw Exception("Unable to locate account: " + trusteeName + ". " + WindowsCommon::GetErrorMessage(error), ERROR_NOTICE);
		} else {
			throw Exception("Error failed to look up account: " + trusteeName + ". " + WindowsCommon::GetErrorMessage(error));
		}
	}

	return isGroup;
}

LPWSTR WindowsCommon::StringToWide(string s) {
	
	size_t size = mbstowcs(NULL, s.c_str(), s.length()) + 1;
	LPWSTR wide = new wchar_t[size];
	mbstowcs(wide, s.c_str(), s.size() + 1 );

	return wide;
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
	LPCWSTR localGroupName = NULL;
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
									  localGroupName,			// group name
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

						if(isGroup && resolveSubGroup) {

							if(!WindowsCommon::GetLocalGroupMembers(trusteeName, members, includeSubGroups, resolveSubGroup)) {
								
								// May be a global group so go that route
								if(!WindowsCommon::GetGlobalGroupMembers(trusteeName, members, includeSubGroups, resolveSubGroup)) {
									isGroup = false;
									Log::Debug("Could not find group " + trusteeName + " when looking up group members.");	
								}
							}
						} 

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
		if(localGroupName != NULL) {
			delete localGroupName;
		}
		
		if(userInfo != NULL) {
			NetApiBufferFree(userInfo);
		}

		throw;
	}

	if(localGroupName != NULL) {
		delete localGroupName;
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
	LPWSTR wDomainName = NULL;
	LPWSTR globalgroupname = NULL;
	LPCWSTR domainControllerName = NULL;
	GROUP_USERS_INFO_0* userInfo = NULL; 

	WindowsCommon::SplitTrusteeName(groupName, &domainName, &shortGroupName);

	wDomainName = WindowsCommon::StringToWide(domainName);

	try {

			domainControllerName = WindowsCommon::GetDomainControllerName(domainName);
			globalgroupname = WindowsCommon::StringToWide(shortGroupName);

			res = NetGroupGetUsers((LPCWSTR)domainControllerName,		// server name NULL == localhost
								   globalgroupname,						// group name
								   0,									// level LOCALGROUP_MEMBERS_INFO_3
								   (unsigned char**) &userInfo,
								   MAX_PREFERRED_LENGTH,
								   &entriesread,
								   &totalentries,
								   NULL);

			delete globalgroupname;

			// was there an error?
			if(res == NERR_Success) {

				char tmpUserName[512];

				// Loop through each user.
				for (unsigned int i=0; i<entriesread; i++) {
					ZeroMemory(tmpUserName, sizeof(tmpUserName));
					_snprintf(tmpUserName, sizeof(tmpUserName) - 1, "%S", userInfo[i].grui0_name);
					tmpUserName[sizeof(tmpUserName)-1] = '\0';

					// Get the account information.
					string trusteeName = tmpUserName;

					// get sid for trustee name
					PSID pSid = WindowsCommon::GetSIDForTrusteeName(trusteeName);

					// get formatted trustee name
					trusteeName = WindowsCommon::GetFormattedTrusteeName(pSid);

					bool isGroup = WindowsCommon::IsGroup(trusteeName);

					if(isGroup && resolveSubGroup) {
						try {
							if(!WindowsCommon::GetGlobalGroupMembers(trusteeName, members, includeSubGroups, resolveSubGroup)) {
									
								if(!WindowsCommon::GetLocalGroupMembers(trusteeName, members, includeSubGroups, resolveSubGroup)) {
									isGroup = false;
									Log::Debug("Could not find group " + trusteeName + " when looking up group members.");
								}
							}
						} catch (Exception gEx) {
							Log::Info("Error retrieving group members for global group " + trusteeName + ":" + gEx.GetErrorMessage());
						}
					}
					
					if(!isGroup || includeSubGroups) {
						members->insert(trusteeName);
					}
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

	} catch(...) {
		if(domainControllerName != NULL) {
			NetApiBufferFree((LPVOID)domainControllerName);
		}

		if(userInfo != NULL) {
			NetApiBufferFree(userInfo);
		}

		if(wDomainName != NULL) {
			delete wDomainName;	
		}

		throw;
	}

	if(domainControllerName != NULL) {
		NetApiBufferFree((LPVOID)domainControllerName);
	}

	if(userInfo != NULL) {
		NetApiBufferFree(userInfo);
	}

	if(wDomainName != NULL) {
		delete wDomainName;
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
		
			PSID pSid = WindowsCommon::GetSIDForTrusteeName((*iterator));
			LPTSTR sidString;
			WindowsCommon::GetTextualSid(pSid, &sidString);
			string sidStr = sidString;
			WindowsCommon::allTrusteeSIDs->insert(sidStr);
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

		Log::Debug("Found " + Common::ToString(WindowsCommon::wellKnownTrusteeNames->size()) + " well known truseee names.");
	}
}

StringSet* WindowsCommon::GetAllLocalGroups() {

	StringSet* allGroups = new StringSet();

	NTSTATUS nts;
	LOCALGROUP_INFO_0* localGroupInfo = NULL;
	NET_API_STATUS nas;
    DWORD recordsEnumerated = 0;
    DWORD totalRecords = 0;
	DWORD_PTR resumeHandle = 0;

	// Get a handle to the policy object.
	LSA_HANDLE polHandle;
	LSA_OBJECT_ATTRIBUTES ObjectAttributes;

	ZeroMemory(&ObjectAttributes, sizeof(ObjectAttributes));

	nts = LsaOpenPolicy(NULL, &ObjectAttributes, POLICY_LOOKUP_NAMES, &polHandle);
	if (nts != ERROR_SUCCESS) {
		Log::Debug("Error unable to open a handle to the Policy object when getting all local groups.");
		return allGroups;
	}

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

			char tmpGroupName[257];

			// Loop through each group.
			for (unsigned int i=0; i<recordsEnumerated; i++) {
				ZeroMemory(tmpGroupName, sizeof(tmpGroupName));
				_snprintf(tmpGroupName, sizeof(tmpGroupName) - 1, "%S", localGroupInfo[i].lgrpi0_name);
				tmpGroupName[sizeof(tmpGroupName)-1] = '\0';

				string groupName = tmpGroupName;
				// get sid for trustee name
				PSID pSid = WindowsCommon::GetSIDForTrusteeName(groupName);
				// get formatted trustee name
				groupName = WindowsCommon::GetFormattedTrusteeName(pSid);
				allGroups->insert(groupName);
			}
		} else {
			nts = LsaClose(polHandle);

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

	// Close the handle to the open policy object.
	nts = LsaClose(polHandle);

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

			char tmpGroupName[257];

			// Loop through each group.
			for (unsigned int i=0; i<recordsEnumerated; i++) {
				ZeroMemory(tmpGroupName, sizeof(tmpGroupName));
				_snprintf(tmpGroupName, sizeof(tmpGroupName) - 1, "%S", globalGroupInfo[i].grpi0_name);
				tmpGroupName[sizeof(tmpGroupName)-1] = '\0';

				string groupName = tmpGroupName;
				// get sid for trustee name
				PSID pSid = WindowsCommon::GetSIDForTrusteeName(groupName);
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
	
		PSID pSid = WindowsCommon::GetSIDForTrusteeName(*iterator);
		LPTSTR sidString;

		WindowsCommon::GetTextualSid(pSid, &sidString);
		string sidStr = sidString;
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

	NTSTATUS nts;

	// Get a handle to the policy object.
	LSA_HANDLE polHandle;
	LSA_OBJECT_ATTRIBUTES ObjectAttributes;
	ZeroMemory(&ObjectAttributes, sizeof(ObjectAttributes));

	nts = LsaOpenPolicy(NULL, &ObjectAttributes, POLICY_LOOKUP_NAMES, &polHandle);
	if (nts != ERROR_SUCCESS) {
		Log::Fatal("Error unable to open a handle to the Policy object when trying to get all local users.");
		return;
	}

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
						  0,			// FILTER_NORMAL_ACCOUNT
						  (unsigned char**) &userInfo,
						  MAX_PREFERRED_LENGTH,
						  &recordsEnumerated,
						  &totalRecords,
						  &resumeHandle);

		if ((nas == NERR_Success) || (nas == ERROR_MORE_DATA)) {

			// User account names are limited to 20 characters.
			char tmpUserName[21];

			Log::Debug("Found " + WindowsCommon::ToString(recordsEnumerated) + " local users.");

			// Loop through each user.
			for (unsigned int i=0; i<recordsEnumerated; i++) {
				ZeroMemory(tmpUserName, sizeof(tmpUserName));
				_snprintf(tmpUserName, sizeof(tmpUserName) - 1, "%S", userInfo[i].usri0_name);
				tmpUserName[sizeof(tmpUserName)-1] = '\0';

				// Get the account information.
				string userName = tmpUserName;
				// get sid for trustee name
				PSID pSid = WindowsCommon::GetSIDForTrusteeName(userName);
				// get formatted trustee name
				userName = WindowsCommon::GetFormattedTrusteeName(pSid);
				allUsers->insert(userName);
			}

		} else {
			nts = LsaClose(polHandle);

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

	// Close the handle to the open policy object.
	nts = LsaClose(polHandle);
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
		LPTSTR sidString;
		WindowsCommon::GetTextualSid(pSid, &sidString);
		string sidStr = sidString;
		free(sidString);
		sidString = NULL;

		// all occurances of this that i have seen are for the domain admins sid and the domain user's sid
		// I should be able to ignore these.
		Log::Info("Unable to look up account name for sid: " + sidStr + ". " + WindowsCommon::GetErrorMessage(GetLastError()));

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
	PSID pSid = NULL;
	LPTSTR domain = NULL;

	try {
		do {
			// Initial memory allocations for the SID and DOMAIN.
			pSid = (PSID)realloc(pSid, sidSize);
			if (pSid == NULL) {
				retVal = FALSE;
				break;
			}

			domain = (LPTSTR)realloc(domain, domainSize);
			if (domain == NULL) {
				retVal = FALSE;
				break;
			}

			// Call LookupAccountName to get the SID.
			retVal = LookupAccountName(NULL,								// system name NULL == localhost
									const_cast<char*>(trusteeName.c_str()),	// account name
									pSid,									// security identifier
									&sidSize,								// size of security identifier
									domain,									// domain name
									&domainSize,							// size of domain name
									&sidUse);								// SID-type indicator

			if ( retVal )
				break;

		} while (GetLastError() == ERROR_INSUFFICIENT_BUFFER);

		if (domain != NULL)
			free(domain);

	} catch(...) {
		if (pSid != NULL)
			free(pSid);
		
		if (domain != NULL)
			free(domain);

		Log::Debug("Error looking up sid for account: " + trusteeName + ". " + WindowsCommon::GetErrorMessage(GetLastError()));
		throw Exception("Error looking up sid for account: " + trusteeName + ". " + WindowsCommon::GetErrorMessage(GetLastError()));
	}

	// check the sid
	if(!IsValidSid(pSid)) {
		
		if(pSid != NULL)
			free(pSid);

		if (domain != NULL)
			free(domain);

		throw Exception("Error looking up sid for account: " + trusteeName + ". Invalid sid found.");
	}

	return pSid;
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

				if(header->AceType == ACCESS_ALLOWED_ACE_TYPE) {
					psid = (PSID)&((ACCESS_ALLOWED_ACE *)ace)->SidStart;
				} else if(header->AceType == ACCESS_DENIED_ACE_TYPE) {
					psid = (PSID)&((ACCESS_DENIED_ACE *)ace)->SidStart;
				} else {
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

				if(header->AceType == ACCESS_ALLOWED_ACE_TYPE) {
					psid = (PSID)&((ACCESS_ALLOWED_ACE *)ace)->SidStart;
				} else if(header->AceType == ACCESS_DENIED_ACE_TYPE) {
					psid = (PSID)&((ACCESS_DENIED_ACE *)ace)->SidStart;
				} else {
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

bool WindowsCommon::LookUpTrusteeName(string* accountNameStr, string* sidStr, string* domainStr) {

	PSID psid = NULL;
	LPTSTR domain = NULL;
	DWORD sidSize = 128;
	DWORD domainSize = 128;
	SID_NAME_USE sid_type = SidTypeUnknown;
	BOOL retVal = FALSE;

	do {
		// Initial memory allocations for the SID and DOMAIN.
		psid = (PSID)realloc(psid, sidSize);
		if (psid == NULL) {
			retVal = FALSE;
			break;
		}

		domain = (LPTSTR)realloc(domain, domainSize);
		if (domain == NULL) {
			free(psid);
			retVal = FALSE;
			break;
		}

		// Call LookupAccountName to get the SID.
		retVal = LookupAccountName(NULL,										// system name
								   const_cast<char*>((*accountNameStr).c_str()),	// account name
								   psid,										// security identifier
								   &sidSize,									// size of security identifier
								   domain,										// domain name
								   &domainSize,									// size of domain name
								   &sid_type);									// SID-type indicator

		if ( retVal )
			break;

	} while (GetLastError() == ERROR_INSUFFICIENT_BUFFER);

	if(retVal == TRUE) {

		LPTSTR sidString;
		WindowsCommon::GetTextualSid(psid, &sidString);
		(*sidStr) = sidString;
		(*domainStr) = domain;
		free(sidString);
		free(psid);
		free(domain);	
	} else {
		free(psid);
		free(domain);	
		DWORD error = GetLastError();
		if(error == ERROR_TRUSTED_RELATIONSHIP_FAILURE) {
			throw Exception("Unable to locate account: " + (*accountNameStr) + ". " + WindowsCommon::GetErrorMessage(error), ERROR_NOTICE);
		} else {
			throw Exception("Error failed to look up account: " + (*accountNameStr) + ". " + WindowsCommon::GetErrorMessage(error));
		}
	}

	// determin if this is a group
	bool isGroup = false;
	if(sid_type == SidTypeGroup || sid_type == SidTypeWellKnownGroup || sid_type == SidTypeAlias) {
		if((*accountNameStr).compare("SYSTEM") != 0) // special case...
			isGroup = true;
	} 
	// make sure account names are consistantly formated
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

	return isGroup;
}

bool WindowsCommon::IsAccountGroup(SID_NAME_USE sidType, string accountName) {
	
	// No solid quick way found to determine if a sid is a group - using this along with local/global group member apis which flag an error

	if ((sidType == SidTypeGroup) || (sidType == SidTypeWellKnownGroup) || (sidType == SidTypeAlias)) {	
		return ((accountName.compare("SYSTEM") != 0) && (accountName.compare("NETWORK SERVICE"))); // special cases...
	}
	
	return false;
}

bool WindowsCommon::LookUpTrusteeSid(string sidStr, string* pAccountNameStr, string* pDomainStr) {

	PSID pSid = NULL;
	LPTSTR pDomain = NULL;
	LPTSTR pAccountName = NULL;
	DWORD accountNameSize = 128;
	DWORD domainSize = 128;
	SID_NAME_USE sid_type = SidTypeUnknown;
	BOOL retVal = FALSE;

	do {
		// Initial memory allocations for the ACCOUNT and DOMAIN.
		pAccountName = (LPTSTR)realloc(pAccountName, accountNameSize);
		if (pAccountName == NULL) {
			retVal = FALSE;
			break;
		}

		pDomain = (LPTSTR)realloc(pDomain, domainSize);
		if (pDomain == NULL) {
			retVal = FALSE;
			break;
		}

		if(pSid == NULL) {
			if(ConvertStringSidToSid(sidStr.c_str(), &pSid) == 0) {
				throw Exception("Error encountered converting SID string to a SID.");
			}
		}

		// Call LookupAccountSid to get the account name and domain.
		retVal = LookupAccountSid(NULL,							// system name
								  pSid,							// security identifier
								  pAccountName,					// account name
								  &accountNameSize,				// security identifier
								  pDomain,						// domain name
								  &domainSize,					// size of domain name
								  &sid_type);					// SID-type indicator

		if ( retVal )
			break;

	} while (GetLastError() == ERROR_INSUFFICIENT_BUFFER);

	LocalFree(pSid);
	if(retVal == TRUE) {
		(*pAccountNameStr) = pAccountName;
		(*pDomainStr) = pDomain;
		free(pAccountName);
		free(pDomain);	
	} else {
		if(pAccountName != NULL){
			free(pAccountName);
		}

		if(pDomain != NULL){
			free(pDomain);
		}
	
		DWORD error = GetLastError();
		if(error == ERROR_TRUSTED_RELATIONSHIP_FAILURE) {
			throw Exception("Unable to locate account: " + sidStr + ". " + WindowsCommon::GetErrorMessage(error), ERROR_NOTICE);
		} else {
			throw Exception("Error failed to look up account: " + sidStr + ". " + WindowsCommon::GetErrorMessage(error));
		}
	}

	// make sure account names are consistantly formated
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

	return IsAccountGroup(sid_type, *pAccountNameStr);
}

string WindowsCommon::LookUpLocalSystemName() {

	string systemName = "";

	LPTSTR buff = NULL;
	buff = (LPTSTR) malloc(MAX_COMPUTERNAME_LENGTH + 1);
	DWORD  buffSize = MAX_COMPUTERNAME_LENGTH + 1;
 
	// Get and display the name of the computer. 
	if(!GetComputerName( buff, &buffSize )) {
		free(buff);
		DWORD error = GetLastError();
		throw Exception("Error failed to get local computer name. " + WindowsCommon::GetErrorMessage(error));
	} else {
		systemName = buff;
		free(buff);
	}

	return systemName;
}

string WindowsCommon::ToString(FILETIME fTime) {
	ULONGLONG fTimeResult = (((ULONGLONG)fTime.dwHighDateTime)<<32) + fTime.dwLowDateTime;
	return WindowsCommon::ToString(fTimeResult);
}

string WindowsCommon::ToString(DWORD dw) {

	char dwordBuf[12];
	ZeroMemory(dwordBuf, sizeof(dwordBuf));
	_snprintf(dwordBuf, sizeof(dwordBuf)-1, "%lu", dw);
	dwordBuf[sizeof(dwordBuf)-1] = '\0';

	string dwStr = dwordBuf;
	return dwStr;
}

string WindowsCommon::ToString(ULONGLONG ul){
	//ULONGLONG (unsigned long long) 0 to 18,446,744,073,709,551,615 => 20 characters + null byte => 21 characters
	char ulonglongBuf[21];
	ZeroMemory(ulonglongBuf,sizeof(ulonglongBuf));
	_snprintf(ulonglongBuf,sizeof(ulonglongBuf)-1,"%llu",ul);
	ulonglongBuf[sizeof(ulonglongBuf)-1] = '\0';
	
	string ulStr = ulonglongBuf;
	return ulStr;
}

string WindowsCommon::ToString(PSID pSID) {

	LPTSTR sidString;
	WindowsCommon::GetTextualSid(pSID, &sidString);
	string sidStr = sidString;
	free(sidString);
	sidString = NULL;

	return sidStr;
}


bool WindowsCommon::TrusteeNameExists(const string trusteeNameIn) {

    bool trusteeNameExists = false;

    PSID psid = NULL;
	LPTSTR domain = NULL;
	DWORD sidSize = 128;
	DWORD domainSize = 128;
	SID_NAME_USE sid_type;
	BOOL retVal = FALSE;

	do {
		// Initial memory allocations for the SID and DOMAIN.
		psid = (PSID)realloc(psid, sidSize);
		if (psid == NULL) {
			retVal = FALSE;
			break;
		}

		domain = (LPTSTR)realloc(domain, domainSize);
		if (domain == NULL) {
			free(psid);
			retVal = FALSE;
			break;
		}

		// Call LookupAccountName to get the SID.
		retVal = LookupAccountName(NULL,										// system name
								   const_cast<char*>(trusteeNameIn.c_str()),	// account name
								   psid,										// security identifier
								   &sidSize,									// size of security identifier
								   domain,										// domain name
								   &domainSize,									// size of domain name
								   &sid_type);									// SID-type indicator
		
		if ( retVal )
			break;

	} while (GetLastError() == ERROR_INSUFFICIENT_BUFFER);

   	free(psid);
    free(domain);


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
	LPTSTR pAccountName = NULL;
	DWORD accountNameSize = 128;
	DWORD domainSize = 128;
	SID_NAME_USE sid_type;
	BOOL retVal = FALSE;

	do {
		// Initial memory allocations for the ACCOUNT and DOMAIN.
		pAccountName = (LPTSTR)realloc(pAccountName, accountNameSize);
		if (pAccountName == NULL) {
			retVal = FALSE;
			break;
		}

		if(pSid == NULL) {
			if(ConvertStringSidToSid(trusteeSIDIn.c_str(), &pSid) == 0) {
				throw Exception("Error encountered converting SID string to a SID.");
			}
		}

		// Call LookupAccountSid to get the account name and domain.
		retVal = LookupAccountSid(NULL,							// system name
								  pSid,							// security identifier
								  pAccountName,					// account name
								  &accountNameSize,				// security identifier
								  NULL,						// domain name
								  &domainSize,					// size of domain name
								  &sid_type);					// SID-type indicator
	
		if ( retVal )
			break;

	} while (GetLastError() == ERROR_INSUFFICIENT_BUFFER);

	LocalFree(pSid);
    if(pAccountName != NULL){
		free(pAccountName);
	}

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
	
	LPCWSTR userNameApi = WindowsCommon::StringToWide(userNameIn);
	LPCWSTR serverName = WindowsCommon::GetDomainControllerName(domain);	
	DWORD dwEntriesRead;
	DWORD dwTotalEntries;
	DWORD dwLevel=0;
	DWORD dwPrefMaxLen=MAX_PREFERRED_LENGTH;
	LPGROUP_USERS_INFO_0 pBuf = NULL;
	string shortGroupName;

	
	// Call the NetUserGetGroups function, specifying level 0.
	NET_API_STATUS nStatus = NetUserGetGroups(serverName,
								userNameApi,
								dwLevel,
								(LPBYTE*)&pBuf,
								dwPrefMaxLen,
								&dwEntriesRead,
								&dwTotalEntries);

	// If the call succeeds,
	if (nStatus == NERR_Success) {
		userExists = true;

		LPGROUP_USERS_INFO_0 pTmpBuf;
		DWORD i;
		DWORD dwTotalCount = 0;
		char tmpGroupName[512];

		if ((pTmpBuf = pBuf) != NULL) {

			// Loop through the entries; 
			//  print the name of the global groups 
			//  to which the user belongs.
			for (i = 0; i < dwEntriesRead; i++) {
				
				if (pTmpBuf == NULL) {
					// Free the allocated buffer.
					if (pBuf != NULL) {
						NetApiBufferFree(pBuf);
						pBuf = NULL;
					}

					throw Exception("An access violation has occurred while getting groups for user: " + userName);
				}

				ZeroMemory(tmpGroupName, 21);
				_snprintf(tmpGroupName, sizeof(tmpGroupName) - 1, "%S", pTmpBuf->grui0_name);
				tmpGroupName[sizeof(tmpGroupName)-1] = '\0';
				groups->insert(tmpGroupName);

				pTmpBuf++;
				dwTotalCount++;
			}
		}

		// report an error if all groups are not listed.
		if (dwEntriesRead < dwTotalEntries) {
			
			// Free the allocated buffer.
			if (serverName != NULL) {
				NetApiBufferFree((LPVOID)serverName);
				serverName = NULL;
			}
			
			// Free the allocated buffer.
			if (pBuf != NULL) {
				NetApiBufferFree(pBuf);
				pBuf = NULL;
			}

			throw Exception("Unable to get all global groups for user: " + userName);
		}
	
	} else if(nStatus == NERR_UserNotFound) {
		// do nothing
	} else {

		// Free the allocated buffer.
		if (serverName != NULL) {
			NetApiBufferFree((LPVOID)serverName);
			serverName = NULL;
		}

		if (pBuf != NULL) {
			NetApiBufferFree(pBuf);
			pBuf = NULL;
		}

		string errMsg = "Unable to get all global groups for user: " + userName + ". Windows Api NetUserGetGroups failed with error: ";

		if(nStatus == ERROR_ACCESS_DENIED) {
			errMsg = errMsg + "The user does not have access rights to the requested information.";
 
		} else if(nStatus == ERROR_BAD_NETPATH) {
			errMsg = errMsg + "The network path was not found. This error is returned if the servername parameter could not be found.";
 
		} else if(nStatus == ERROR_INVALID_LEVEL) {
			errMsg = errMsg + "The system call level is not correct. This error is returned if the level parameter was specified as a value other than 0 or 1.";
 
		} else if(nStatus == NERR_InvalidComputer) {
			errMsg = errMsg + "The computer name is invalid."; 
 
		} else if(nStatus == ERROR_INVALID_NAME) {
			errMsg = errMsg + "The name syntax is incorrect. This error is returned if the servername parameter has leading or trailing blanks or contains an illegal character.";

		} else if(nStatus == ERROR_MORE_DATA) {
			errMsg = errMsg + "More entries are available. Specify a large enough buffer to receive all entries.";

		} else if(nStatus == ERROR_NOT_ENOUGH_MEMORY) {
			errMsg = errMsg + "Insufficient memory was available to complete the operation.";

		} else if(nStatus == NERR_InternalError) {
			errMsg = errMsg + "An internal error occurred.";

		} else if (nStatus == ERROR_NO_SUCH_DOMAIN) {
			errMsg = errMsg + "The domain '"+domain+"' does not exist or could not be contacted.";
		}else {
			errMsg = errMsg + "Unknown error.";
		} 

		throw Exception(errMsg);
	}

	// Free the allocated buffer.
	if (pBuf != NULL) {
		NetApiBufferFree(pBuf);
		pBuf = NULL;
	}

	//
	// get the local groups for the user
	//
	LPLOCALGROUP_USERS_INFO_0 pLocalBuf = NULL;
	dwLevel = 0;
	DWORD dwFlags = LG_INCLUDE_INDIRECT ;
	dwPrefMaxLen = MAX_PREFERRED_LENGTH;
	dwEntriesRead = 0;
	dwTotalEntries = 0;

	//  Call the NetUserGetLocalGroups function 
	//  specifying information level 0.
	//
	//   The LG_INCLUDE_INDIRECT flag specifies that the 
	//   function should also return the names of the local 
	//   groups in which the user is indirectly a member.
	nStatus = NetUserGetLocalGroups(NULL,
									userNameApi,
									dwLevel,
									dwFlags,
									(LPBYTE *) &pLocalBuf,
									dwPrefMaxLen,
									&dwEntriesRead,
									&dwTotalEntries);

	// If the call succeeds
	if (nStatus == NERR_Success) {
		userExists = true;

		LPLOCALGROUP_USERS_INFO_0 pLocalTmpBuf;
		DWORD i;
		DWORD dwTotalCount = 0;
		char tmpGroupName[512];

		if ((pLocalTmpBuf = pLocalBuf) != NULL) {

			//  Loop through the entries and 
			//  print the names of the local groups 
			//  to which the user belongs. 
			for (i = 0; i < dwEntriesRead; i++) {
				
				if (pLocalTmpBuf == NULL) {
					
					// Free the allocated buffer.
					if (serverName != NULL) {
						NetApiBufferFree((LPVOID)serverName);
						serverName = NULL;
					}

					// Free the allocated memory.
					if (pBuf != NULL) {
						NetApiBufferFree(pBuf);
						pBuf = NULL;
					}

					throw Exception("An access violation has occurred while getting local groups for user: " + userName);
				}

				ZeroMemory(tmpGroupName, 21);
				_snprintf(tmpGroupName, sizeof(tmpGroupName) - 1, "%S", pLocalTmpBuf->lgrui0_name);
				tmpGroupName[sizeof(tmpGroupName)-1] = '\0';
				groups->insert(tmpGroupName);

				pLocalTmpBuf++;
				dwTotalCount++;
			}
		}
		
		// report an error if all groups are not listed
		if (dwEntriesRead < dwTotalEntries) {

			// Free the allocated buffer.
			if (serverName != NULL) {
				NetApiBufferFree((LPVOID)serverName);
				serverName = NULL;
			}

			// Free the allocated memory.
			if (pBuf != NULL) {
				NetApiBufferFree(pBuf);
				pBuf = NULL;
			}

			throw Exception("Unable to get all local groups for user: " + userName);
		}

	} else if (nStatus == NERR_UserNotFound){
		// do nothing
	} else {

		// Free the allocated buffer.
		if (serverName != NULL) {
			NetApiBufferFree((LPVOID)serverName);
			serverName = NULL;
		}

		if (pBuf != NULL) {
			NetApiBufferFree(pBuf);
			pBuf = NULL;
		}

		string errMsg = "Unable to get all local groups for user: " + userName + ". Windows Api NetUserGetLocalGroups failed with error: ";

		if(nStatus == ERROR_ACCESS_DENIED) {
			errMsg = errMsg + "The user does not have access rights to the requested information. This error is also returned if the servername parameter has a trailing blank.";
 
		} else if(nStatus == ERROR_INVALID_LEVEL) {
			errMsg = errMsg + "The system call level is not correct. This error is returned if the level parameter was not specified as 0.";

		} else if(nStatus == ERROR_INVALID_PARAMETER) {
			errMsg = errMsg + "A parameter is incorrect. This error is returned if the flags parameter contains a value other than LG_INCLUDE_INDIRECT.";

		} else if(nStatus == ERROR_MORE_DATA) {
			errMsg = errMsg + "More entries are available. Specify a large enough buffer to receive all entries.";

		} else if(nStatus == ERROR_NOT_ENOUGH_MEMORY) {
			errMsg = errMsg + "Insufficient memory was available to complete the operation.";

		} else if(nStatus == NERR_DCNotFound) {
			errMsg = errMsg + "The domain controller could not be found."; 

		} else if(nStatus == NERR_InvalidComputer) {
			errMsg = errMsg + "The computer name is not valid. This error is returned on Windows NT 4.0 and earlier if the servername parameter does not begin with \\."; 
  
		} else if(nStatus == NERR_InvalidComputer) {
			errMsg = errMsg + "The computer name is not valid. This error is returned on Windows NT 4.0 and earlier if the servername parameter does not begin with \\."; 
 
		} else if(nStatus == RPC_S_SERVER_UNAVAILABLE) {
			errMsg = errMsg + "The RPC server is unavailable. This error is returned if the servername parameter could not be found.";

		} else if (nStatus == ERROR_NO_SUCH_DOMAIN) {
			errMsg = errMsg + "The domain '"+domain+"' does not exist or could not be contacted.";
		}else {
			errMsg = errMsg + "Unknown error.";
		} 

		throw Exception(errMsg);
	}

	// Free the allocated buffer.
	if (serverName != NULL) {
		NetApiBufferFree((LPVOID)serverName);
		serverName = NULL;
	}

	// Free the allocated memory.
	if (pBuf != NULL) {
		NetApiBufferFree(pBuf);
		pBuf = NULL;
	}

	return userExists;
}

bool WindowsCommon::GetEnabledFlagForUser(string userNameIn) {
	string domain;                                                    // Used to hold the domain portion of the username
	string accountName;                                               // Used to hold the account name portion of the username
	bool enabled = true;											  // Initialize user enabled to true
	WindowsCommon::SplitTrusteeName(userNameIn,&domain,&accountName); // Split into domain and account name components
	LPCWSTR serverName = WindowsCommon::GetDomainControllerName(domain);  // Retrieve the server name for the specified domain
	LPCWSTR userNameApi = WindowsCommon::StringToWide(accountName);   // Convert account name into wide string for use in the api
	DWORD dwLevel = 23;                                               // Need USER_INFO_23  to get enabled flag
	LPUSER_INFO_23 pBuf = NULL;                                       // Will be used to hold the user info 

	// Call the NetUserGetInfo function
	//
	// Pass in NULL for the server portion since we are running on the local
	// host only. This will prevent the interpreter from trying to get user
	// information for users that are not defined on the local host.
	//
	NET_API_STATUS nStatus = NetUserGetInfo(serverName,
							userNameApi,
							dwLevel,
							(LPBYTE *)&pBuf);

	// Free the allocated buffer.
	if (serverName != NULL) {
		NetApiBufferFree((LPVOID)serverName);
		serverName = NULL;
	}

	// If the call succeeds, print the user information.
	if (nStatus == NERR_Success) {
		if (pBuf != NULL) {			
			// now read the flags
			if(pBuf->usri23_flags & UF_ACCOUNTDISABLE) {
				enabled = false;
			}
		}
	} else {
		string errMsg = "Windows Api NetUserGetinfo failed with error: ";

		if(nStatus == ERROR_ACCESS_DENIED) {
			errMsg = errMsg + "The user does not have access to the requested information.";
 
		} else if(nStatus == ERROR_BAD_NETPATH) {
			errMsg = errMsg + "The network path specified in the servername parameter was not found.";
 
		} else if(nStatus == ERROR_INVALID_LEVEL) {
			errMsg = errMsg + "The value specified for the level parameter is invalid.";
 
		} else if(nStatus == NERR_InvalidComputer) {
			errMsg = errMsg + "he computer name is invalid."; 
 
		} else if(nStatus == NERR_UserNotFound) {
			errMsg = errMsg + "The user name could not be found.";

		} else {
			errMsg = errMsg + "Unknown error.";
		}

		throw Exception("Error while getting user enabled flag. " + errMsg);
	}
	
	// Free the allocated memory.
	if (pBuf != NULL)
		NetApiBufferFree(pBuf);

    return enabled;
}

void WindowsCommon::GetEffectiveRightsForWindowsObject(SE_OBJECT_TYPE objectType, PSID pSid, string* objectNameStr, PACCESS_MASK pAccessRights) {	

	if(WindowsCommon::IsXPOrLater()) {
		WindowsCommon::GetEffectiveRightsForWindowsObjectAuthz(objectType, pSid, objectNameStr, pAccessRights);
	} else {
		WindowsCommon::GetEffectiveRightsForWindowsObjectAcl(objectType, pSid, objectNameStr, pAccessRights);
	}
}



void WindowsCommon::GetEffectiveRightsForWindowsObjectAcl(SE_OBJECT_TYPE objectType, PSID pSid, string* objectNameStr, PACCESS_MASK pAccessRights) {
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


	string baseErrMsg = "Error unable to get effective rights for trustee: " + WindowsCommon::ToString(pSid) + " from dacl for Windows object: " + (*objectNameStr);

	DWORD res;
	PACL pdacl;
	PSECURITY_DESCRIPTOR pSD;

	res = GetNamedSecurityInfo(const_cast<char*>((*objectNameStr).c_str()),	// object name
							   objectType,							// object type
							   DACL_SECURITY_INFORMATION |				// information type
							   PROTECTED_DACL_SECURITY_INFORMATION |
							   UNPROTECTED_DACL_SECURITY_INFORMATION, 			
							   NULL,									// owner SID
							   NULL,									// primary group SID
							   &pdacl,									// DACL
							   NULL,									// SACL
							   &pSD);									// Security Descriptor

	if (res != ERROR_SUCCESS) {	
		throw Exception( baseErrMsg + " Unable to retrieve a copy of the security descriptor. Microsoft System Error " + Common::ToString ( GetLastError() ) + ") - " + WindowsCommon::GetErrorMessage ( GetLastError() ) );
	} 


	// Check to see if a valid security descriptor was returned.  
    if ((IsValidSecurityDescriptor(pSD) == 0) || (IsValidAcl(pdacl) == 0)) {
		LocalFree(pSD);
		throw Exception(baseErrMsg + " Invalid data returned from call to GetNamedSecurityInfo().");
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

void WindowsCommon::GetEffectiveRightsForWindowsObjectAuthz(SE_OBJECT_TYPE objectType, PSID pSid, string* objectNameStr, PACCESS_MASK pAccessRights) {	
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

	DWORD res = GetNamedSecurityInfo(const_cast<char*>((*objectNameStr).c_str()),// object name
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
		throw Exception("Unable to retrieve a copy of the security descriptor. Microsoft System Error " + Common::ToString ( GetLastError() ) + ") - " + WindowsCommon::GetErrorMessage ( GetLastError() ) );
	} 

	// Check to see if a valid security descriptor was returned.  
    if ((IsValidSecurityDescriptor(pSD) == 0)) {
		LocalFree(pSD);
		throw Exception("Invalid data returned from call to GetNamedSecurityInfo().");
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

void WindowsCommon::GetAuditedPermissionsForWindowsObject ( SE_OBJECT_TYPE objectType, PSID pSid, string* objectNameStr, PACCESS_MASK pSuccessfulAuditedPermissions, PACCESS_MASK pFailedAuditPermissions ) {
    Log::Debug ( "Calling the ACL API to get the audited permissions" );
    string baseErrMsg = "Error: Unable to get audited permissions for trustee: " + WindowsCommon::ToString ( pSid ) + " from sacl for Windows object: " + ( *objectNameStr ) + ".";
    DWORD res;
    PACL psacl;

    // The SE_SECURITY_NAME privilege is needed to read the SACL.
    if ( !WindowsCommon::EnablePrivilege ( SE_SECURITY_NAME ) ) {
        throw Exception ( "Error: The SeSecurityPrivilege could not be enabled. Microsoft System Error " + Common::ToString ( GetLastError() ) + " - " + WindowsCommon::GetErrorMessage ( GetLastError() ) );
    }
	
    res = GetNamedSecurityInfo ( const_cast<char*> ( ( *objectNameStr ).c_str() ), // object name
                                 objectType,									   // object type
                                 SACL_SECURITY_INFORMATION |					   // information type
                                 PROTECTED_SACL_SECURITY_INFORMATION |
                                 UNPROTECTED_SACL_SECURITY_INFORMATION,
                                 NULL,                                             // owner SID
                                 NULL,                                             // primary group SID
                                 NULL,                                             // DACL
                                 &psacl,                                           // SACL
                                 NULL );                                           // Security Descriptor

    if ( res != ERROR_SUCCESS ) {
        throw Exception ( baseErrMsg + " Unable to retrieve a copy of the security descriptor. Microsoft System Error " + Common::ToString ( GetLastError() ) + ") - " + WindowsCommon::GetErrorMessage ( GetLastError() ) );
    }
	
    ULONG size;
    EXPLICIT_ACCESS* entries = NULL;

    if ( ( res = GetExplicitEntriesFromAcl ( psacl, &size, &entries ) ) == ERROR_SUCCESS ) {
        string pSidStr = WindowsCommon::ToString ( pSid );

        for ( unsigned int i = 0 ; i < size ; i++ ) {
            PSID sid = entries[i].Trustee.ptstrName;

            if ( pSidStr.compare ( WindowsCommon::ToString ( sid ) ) == 0 ) {
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

    if ( !WindowsCommon::DisableAllPrivileges() ) {
        throw Exception ( "Error: All of the privileges could not be disabled. Microsoft System Error " + Common::ToString ( GetLastError() ) + " - " + WindowsCommon::GetErrorMessage ( GetLastError() ) );
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

string WindowsCommon::UnicodeToAsciiString ( wchar_t* unicodeCharStr ) {
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

LPCWSTR WindowsCommon::GetDomainControllerName(string domainName){

	LPBYTE domainControllerName = NULL;
	
	if( domainName.compare("") == 0 || NetGetAnyDCName(NULL, WindowsCommon::StringToWide(domainName), &domainControllerName) != NERR_Success) {
		domainControllerName = NULL;
	}
	
	return (LPCWSTR)domainControllerName;
}

string WindowsCommon::GetActualPathWithCase(const string &path) {

	if (path.empty())
		return path;

	DWORD sz = 100, sz2;
	ArrayGuard<char> shortBuf(new char[sz]);

	// This is hard-coded to use the 'A' versions of the functions... since we
	// don't actually use wide-char strings anywhere do we?
	sz2 = GetShortPathNameA(path.c_str(), shortBuf.get(), sz);
	if (sz2 > sz) {
		sz = sz2;
		shortBuf.reset(new char[sz]);
		sz2 = GetShortPathNameA(path.c_str(), shortBuf.get(), sz);
		if (sz2 == 0)
			throw Exception("GetShortPathName("+path+"): "+
				WindowsCommon::GetErrorMessage(GetLastError()));
		else if (sz2 > sz)
			// really shouldn't happen right???
			throw Exception("GetShortPathName("+path+
				"): incorrectly predicted space requirements");
	} else if (sz2 == 0)
		throw Exception("GetShortPathName("+path+"): "+
			WindowsCommon::GetErrorMessage(GetLastError()));

	sz = 100;
	ArrayGuard<char> longBuf(new char[sz]);

	sz2 = GetLongPathNameA(shortBuf.get(), longBuf.get(), sz);
	if (sz2 > sz) {
		sz = sz2;
		longBuf.reset(new char[sz]);
		sz2 = GetLongPathNameA(shortBuf.get(), longBuf.get(), sz);
		if (sz2 == 0)
			throw Exception("GetLongPathName("+path+"): "+
				WindowsCommon::GetErrorMessage(GetLastError()));
		else if (sz2 > sz)
			// really shouldn't happen right???
			throw Exception("GetLongPathName("+path+
				"): incorrectly predicted space requirements");
	} else if (sz2 == 0)
		throw Exception("GetLongPathName("+path+"): "+
			WindowsCommon::GetErrorMessage(GetLastError()));

	// The short->long conversion doesn't seem to affect drive letters.
	// So let's just decide they should all be uppercase.
	if (strlen(longBuf.get()) >= 2 &&
		isalpha(longBuf[0]) &&
		longBuf[1] == ':')
		longBuf[0] = static_cast<char>(toupper(longBuf[0]));

	return string(longBuf.get());
}
