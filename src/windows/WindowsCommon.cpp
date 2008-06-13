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

#include "WindowsCommon.h"

StringVector* WindowsCommon::allTrusteeNames = NULL;
StringVector* WindowsCommon::allTrusteeSIDs = NULL;
StringVector* WindowsCommon::wellKnownTrusteeNames = NULL;

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
		cerr << errorMessage;
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
		cerr << errorMessage;
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

	if(!OpenProcessToken(hProcess,									// handle to the process
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
    if(dwBufferLength = FormatMessageA(dwFormatFlags,
										hModule, // module to get message from (NULL == system)
										dwLastError,
										MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // default language
										(LPSTR) &MessageBuffer,
										0,
										NULL
										))
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

	return errMsg;
}

bool WindowsCommon::GetTextualSid(PSID pSid, LPTSTR* TextualSid) { 
	//------------------------------------------------------------------------------------//
	//
	//  ABSTRACT
	//
	//  A SID value includes components that provide information about the SID structure
	//  and components that uniquely identify a trustee. A SID consists of the following
	//  components: 
	//
	//   * The revision level of the SID structure 
	//   * A 48-bit identifier authority value that identifies the authority that issued
	//     the SID 
	//   * A variable number of subauthority or relative identifier (RID) values that
	//     uniquely identify the trustee relative to the authority that issued the SID
	//
	//  The combination of the identifier authority value and the subauthority values
	//  ensures that no two SIDs will be the same, even if two different SID-issuing
	//  authorities issue the same combination of RID values. Each SID-issuing authority
	//  issues a given RID only once. 
	//
	//  SIDs are stored in binary format in a SID structure. To display a SID, you can
	//  call the ConvertSidToStringSid function to convert a binary SID to string format.
	//  To convert a SID string back to a valid, functional SID, call the
	//  ConvertStringSidToSid function. 
	//
	//  These functions use the following standardized string notation for SIDs, which
	//  makes it simpler to visualize their components: 
	//
	//  S-R-I-S-S...
	//
	//  In this notation, the literal character S identifies the series of digits as a
	//  SID, R is the revision level, I is the identifier-authority value, and S... is one
	//  or more subauthority values. 
	//
	//  NOTE:
	//
	//  Windows 2000 provides the ConvertSidToStringSid and ConvertStringSidToSid functions
	//  for converting a SID to and from string format. For a description of the SID string
	//  format, see SID Components.
	//
	//  On earlier versions of Windows NT, use the following sample code to convert a SID
	//  to string format.
	//
	//------------------------------------------------------------------------------------//

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

bool WindowsCommon::ExpandGroup(string groupName, StringVector* members) {

	// Need to determine if a local or global group.
	bool groupExists = false;
	groupExists = WindowsCommon::GetLocalGroupMembers(groupName, members);
	if(!groupExists)
		groupExists = WindowsCommon::GetGlobalGroupMembers(groupName, members);	
		
	return groupExists;
}

bool WindowsCommon::ExpandGroupBySID(string groupSID, StringVector* memberSIDs) {
	// to do 
	return false;
}

bool WindowsCommon::IsGroupSID(string SID) {

	// to do 
	return false;
}

StringVector* WindowsCommon::GetAllGroups() {

	StringVector* groups = WindowsCommon::GetAllLocalGroups();
	StringVector* globalGroups = WindowsCommon::GetAllLocalGroups();
	StringVector::iterator iterator;
	for(iterator = globalGroups->begin(); iterator != globalGroups->end(); iterator++) {
		groups->push_back((*iterator));
	}

    return groups;
}

bool WindowsCommon::GetLocalGroupMembers(string groupName, StringVector* members) {
	
	bool groupExists = false;

	NET_API_STATUS  res;
	LPCWSTR localgroupname;
	LPBYTE bufptr = NULL;
	DWORD entriesread;
	DWORD totalentries;
	LOCALGROUP_MEMBERS_INFO_0* userInfo;

	// convert groupName for api use
	wchar_t* wGroupName = NULL;
	size_t size = mbstowcs(NULL, groupName.c_str(), groupName.length()) + 1;
	wGroupName = new wchar_t[size];
	mbstowcs(wGroupName, groupName.c_str(), groupName.size() + 1 );
	localgroupname = wGroupName;

	res = NetLocalGroupGetMembers(NULL,					// server name NULL == localhost
								localgroupname,			// group name
  								0,						// level LOCALGROUP_MEMBERS_INFO_3
								(unsigned char**) &userInfo,
								MAX_PREFERRED_LENGTH,
								&entriesread,
								&totalentries,
								NULL);

	// was there an error?
	if(res == NERR_Success) {

			// Loop through each user.
			for (unsigned int i=0; i<entriesread; i++) {

				// get sid
				PSID pSid = userInfo[i].lgrmi0_sid;
				try {
					string userName = WindowsCommon::GetFormattedTrusteeName(pSid);
					members->push_back(userName);
				} catch (Exception ex) {
					Log::Info("Unable to get all group memebrs." + ex.GetErrorMessage());
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
			//cout << "The specified local group " + groupName + " does not exist." << endl;
		} else if(res == ERROR_ACCESS_DENIED) {
			// throw this error???
			throw Exception("Unable to expand local group: " + groupName + ". " + " The user does not have access to the requested information.");
		} else {
			throw Exception("Unable to expand local group: " + groupName + ". " + WindowsCommon::GetErrorMessage(res));
		}
	}

	NetApiBufferFree(bufptr);

	return groupExists;
}

bool WindowsCommon::GetGlobalGroupMembers(string groupName, StringVector* members) {

	bool groupExists = false;
	NET_API_STATUS  res;
	LPCWSTR globalgroupname;
	LPBYTE bufptr = NULL;
	DWORD entriesread;
	DWORD totalentries;
	GROUP_USERS_INFO_0* userInfo; 

	// convert groupName for api use
	wchar_t* wGroupName = NULL;
	size_t size = mbstowcs(NULL, groupName.c_str(), groupName.length()) + 1;
	wGroupName = new wchar_t[size];
	mbstowcs(wGroupName, groupName.c_str(), groupName.size() + 1 );
	globalgroupname = wGroupName;

	res = NetGroupGetUsers(NULL,					// server name NULL == localhost
						globalgroupname,			// group name
  						0,							// level LOCALGROUP_MEMBERS_INFO_3
						(unsigned char**) &userInfo,
						MAX_PREFERRED_LENGTH,
						&entriesread,
						&totalentries,
						NULL);

	// was there an error?
	if(res == NERR_Success) {

			char tmpUserName[512];

			// Loop through each user.
			for (unsigned int i=0; i<entriesread; i++) {
				ZeroMemory(tmpUserName, 21);
				_snprintf(tmpUserName, sizeof(tmpUserName) - 1, "%S", userInfo[i].grui0_name);
				tmpUserName[sizeof(tmpUserName)-1] = '\0';

				// Get the account information.
				string userName = tmpUserName;

				// get sid for trustee name
				PSID pSid = WindowsCommon::GetSIDForTrusteeName(userName);
				// get formatted trustee name
				userName = WindowsCommon::GetFormattedTrusteeName(pSid);
				members->push_back(userName);
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
			//cout << "The global group name: " + groupName + " could not be found." << endl;			
		} else if(res == ERROR_ACCESS_DENIED) {
			// throw this error???
			throw Exception("Unable to expand global group: " + groupName + ". The user does not have access to the requested information.");
		} else {
			throw Exception("Unable to expand global group: " + groupName + ". " + WindowsCommon::GetErrorMessage(res));
		}
	}

	NetApiBufferFree(bufptr);

	return groupExists;
}

StringVector* WindowsCommon::GetAllTrusteeNames() {

	if(WindowsCommon::allTrusteeNames == NULL) {
		WindowsCommon::allTrusteeNames = new StringVector();

		UniqueStringVector* usv = new UniqueStringVector(allTrusteeNames);

		// get the well know trustee names
		WindowsCommon::GetWellKnownTrusteeNames();
		StringVector::iterator iterator;
		for(iterator = WindowsCommon::wellKnownTrusteeNames->begin(); iterator != WindowsCommon::wellKnownTrusteeNames->end(); iterator++) {
			usv->Append((*iterator));
		}

		WindowsCommon::GetAllLocalUsers(usv);

		// local groups
		StringVector* localGroups = WindowsCommon::GetAllLocalGroups();
		for(iterator = localGroups->begin(); iterator != localGroups->end(); iterator++) {
			usv->Append((*iterator));
			// expand the group
			try {
				StringVector* members = new StringVector();
				WindowsCommon::GetLocalGroupMembers((*iterator), members);
				StringVector::iterator member;
				for(member = members->begin(); member != members->end(); member++) {
					usv->Append((*member));
				}				
				delete members;
			} catch(Exception ex) {
				Log::Debug(ex.GetErrorMessage());
			}
		}
		delete localGroups;

		// global groups
		StringVector* globalGroups = WindowsCommon::GetAllGlobalGroups();
		for(iterator = globalGroups->begin(); iterator != globalGroups->end(); iterator++) {
			usv->Append((*iterator));
			// expand the group
			try {
				StringVector* members = new StringVector();
				WindowsCommon::GetGlobalGroupMembers((*iterator), allTrusteeNames);
				StringVector::iterator member;
				for(member = members->begin(); member != members->end(); member++) {
					usv->Append((*member));
				}				
				delete members;

			} catch(Exception ex) {
				Log::Debug(ex.GetErrorMessage());
			}
		}
		delete globalGroups;

		// get the system's trustee name
		string systemName = WindowsCommon::LookUpLocalSystemName();
		usv->Append(systemName);
	}

	return WindowsCommon::allTrusteeNames;
}

StringVector* WindowsCommon::GetAllTrusteeSIDs() {

	if(WindowsCommon::allTrusteeNames == NULL) {

		StringVector* trusteeNames = WindowsCommon::GetAllTrusteeNames();
		WindowsCommon::allTrusteeSIDs = new StringVector();
		StringVector::iterator iterator;
		for(iterator = trusteeNames->begin(); iterator != trusteeNames->end(); iterator++) {
		
			PSID pSid = WindowsCommon::GetSIDForTrusteeName((*iterator));
			LPTSTR sidString;
			WindowsCommon::GetTextualSid(pSid, &sidString);
			string sidStr = sidString;
			WindowsCommon::allTrusteeSIDs->push_back(sidStr);
		}
	}

	return WindowsCommon::allTrusteeSIDs;
}

void WindowsCommon::GetWellKnownTrusteeNames() {

	if(WindowsCommon::wellKnownTrusteeNames == NULL) {
		WindowsCommon::wellKnownTrusteeNames = new StringVector();

		// create a vector of the well known sids
		StringVector wellKnownSids;
		//wellKnownSids.push_back("S-1-0");			// Null Authority
		wellKnownSids.push_back("S-1-0-0");			// Nobody
		//wellKnownSids.push_back("S-1-1");			// World Authority
		wellKnownSids.push_back("S-1-1-0");			// Everyone
		//wellKnownSids.push_back("S-1-2");			// Local Authority
		//wellKnownSids.push_back("S-1-3");			// Creator Authority
		wellKnownSids.push_back("S-1-3-0");			// Creator Owner
		wellKnownSids.push_back("S-1-3-1");			// Creator Group
		wellKnownSids.push_back("S-1-3-2");			// Creator Owner Server
		wellKnownSids.push_back("S-1-3-3");			// Creator Group Server
		//wellKnownSids.push_back("S-1-4");			// Non-unique Authority
		//wellKnownSids.push_back("S-1-5");			// NT Authority
		wellKnownSids.push_back("S-1-5-1");			// Dialup
		wellKnownSids.push_back("S-1-5-2");			// Network
		wellKnownSids.push_back("S-1-5-3");			// Batch
		wellKnownSids.push_back("S-1-5-4");			// Interactive
		wellKnownSids.push_back("S-1-5-6");			// Service
		wellKnownSids.push_back("S-1-5-7");			// Anonymous
		wellKnownSids.push_back("S-1-5-8");			// Proxy
		wellKnownSids.push_back("S-1-5-9");			// Enterprise Domain Controllers
		wellKnownSids.push_back("S-1-5-11");		// Authenticated Users
		wellKnownSids.push_back("S-1-5-13");		// Terminal Server Users
		wellKnownSids.push_back("S-1-5-18");		// Local System
		wellKnownSids.push_back("S-1-5-19");		// NT Authority - local service
		wellKnownSids.push_back("S-1-5-20");		// NT Authority - network service
		wellKnownSids.push_back("S-1-5-32-544");	// Administrators
		wellKnownSids.push_back("S-1-5-32-545");	// Users
		wellKnownSids.push_back("S-1-5-32-546");	// Guests
		wellKnownSids.push_back("S-1-5-32-547");	// Power Users
		//wellKnownSids.push_back("S-1-5-32-548");	// Account Operators
		//wellKnownSids.push_back("S-1-5-32-549");	// Server Operators
		//wellKnownSids.push_back("S-1-5-32-550");	// Print Operators
		wellKnownSids.push_back("S-1-5-32-551");	// Backup Operators
		wellKnownSids.push_back("S-1-5-32-552");	// Replicators


		// look up account names for all the sids
		StringVector::iterator iterator;
		for(iterator = wellKnownSids.begin(); iterator != wellKnownSids.end(); iterator++) {
			string currentSidStr = (*iterator);
			PSID psid = NULL;
			if(!ConvertStringSidToSid(const_cast<char*>(currentSidStr.c_str()), &psid)) {
				Log::Debug("Error converting sid string (" + currentSidStr +") to SID. " + WindowsCommon::GetErrorMessage(GetLastError()));
			} else {
			
				string trusteeName = WindowsCommon::GetFormattedTrusteeName(psid);	
				WindowsCommon::wellKnownTrusteeNames->push_back(trusteeName);
			}

			LocalFree(psid);
		}
	}
}

StringVector* WindowsCommon::GetAllLocalGroups() {

	StringVector* allGroups = new StringVector();

	NTSTATUS nts;
	LOCALGROUP_INFO_0* localGroupInfo = NULL;
	NET_API_STATUS nas;
    DWORD recordsEnumerated = 0;
    DWORD totalRecords = 0;

	// Get a handle to the policy object.
	LSA_HANDLE polHandle;
	LSA_OBJECT_ATTRIBUTES ObjectAttributes;

	ZeroMemory(&ObjectAttributes, sizeof(ObjectAttributes));

	nts = LsaOpenPolicy(NULL, &ObjectAttributes, POLICY_LOOKUP_NAMES, &polHandle);
	if (nts != ERROR_SUCCESS) {
		string errorMessage = "";
		errorMessage.append("Error unable to open a handle to the Policy object.");
		Log::Debug(errorMessage);
		return allGroups;
	}

	do { 
		nas = NetLocalGroupEnum(NULL, 
								0,
								(unsigned char**) &localGroupInfo,
								MAX_PREFERRED_LENGTH,
								&recordsEnumerated,
								&totalRecords,
								NULL);

		if ((nas == NERR_Success) || (nas==ERROR_MORE_DATA)) {
			// Group account names are limited to 256 characters.

			char tmpGroupName[257];

			// Loop through each group.
			for (unsigned int i=0; i<recordsEnumerated; i++) {
				ZeroMemory(tmpGroupName, 257);
				_snprintf(tmpGroupName, sizeof(tmpGroupName) - 1, "%S", localGroupInfo[i].lgrpi0_name);
				tmpGroupName[sizeof(tmpGroupName)-1] = '\0';

				string groupName = tmpGroupName;
				// get sid for trustee name
				PSID pSid = WindowsCommon::GetSIDForTrusteeName(groupName);
				// get formatted trustee name
				groupName = WindowsCommon::GetFormattedTrusteeName(pSid);
				allGroups->push_back(groupName);
			}
		} else {
			nts = LsaClose(polHandle);

			if(nas == ERROR_ACCESS_DENIED) { 
				throw Exception("Error unable to enumerate local groups. The user does not have access to the requested information.");
			} else if(nas == NERR_InvalidComputer) {
				throw Exception("Error unable to enumerate local groups. The computer name is invalid.");
			} else {
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

StringVector* WindowsCommon::GetAllGlobalGroups() {

	StringVector* allGroups = new StringVector();

	// Get a handle to the policy object.
	NTSTATUS nts;
	LSA_HANDLE polHandle;
	LSA_OBJECT_ATTRIBUTES ObjectAttributes;
	ZeroMemory(&ObjectAttributes, sizeof(ObjectAttributes));

	nts = LsaOpenPolicy(NULL, &ObjectAttributes, POLICY_LOOKUP_NAMES, &polHandle);
	if (nts != ERROR_SUCCESS) {
		string errorMessage = "";
		errorMessage.append("Error unable to open a handle to the Policy object.");
		Log::Debug(errorMessage);
		return allGroups;
	}

	NET_API_STATUS nas;
	GROUP_INFO_0* globalGroupInfo = NULL;
	do { 
		
		DWORD recordsEnumerated = 0;
		DWORD totalRecords = 0;
		nas = NetGroupEnum(NULL,
						   0,
						   (unsigned char**) &globalGroupInfo,
						   MAX_PREFERRED_LENGTH,
						   &recordsEnumerated,
						   &totalRecords,
						   NULL);

		if ((nas == NERR_Success) || (nas==ERROR_MORE_DATA)) {
			// Group account names are limited to 256 characters.

			char tmpGroupName[257];

			// Loop through each group.
			for (unsigned int i=0; i<recordsEnumerated; i++) {
				ZeroMemory(tmpGroupName, 257);
				_snprintf(tmpGroupName, sizeof(tmpGroupName) - 1, "%S", globalGroupInfo[i].grpi0_name);
				tmpGroupName[sizeof(tmpGroupName)-1] = '\0';

				string groupName = tmpGroupName;
				// get sid for trustee name
				PSID pSid = WindowsCommon::GetSIDForTrusteeName(groupName);
				// get formatted trustee name
				groupName = WindowsCommon::GetFormattedTrusteeName(pSid);
				allGroups->push_back(groupName);
			}
		} else {
			nts = LsaClose(polHandle);

			if(nas == ERROR_ACCESS_DENIED) { 
				throw Exception("Error unable to enumerate global groups. The user does not have access to the requested information.");
			} else if(nas == NERR_InvalidComputer) {
				throw Exception("Error unable to enumerate global groups. The computer name is invalid.");
			} else {
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
void WindowsCommon::GetAllLocalUsers(UniqueStringVector* allUsers) {

	NTSTATUS nts;

	// Get a handle to the policy object.
	LSA_HANDLE polHandle;
	LSA_OBJECT_ATTRIBUTES ObjectAttributes;
	ZeroMemory(&ObjectAttributes, sizeof(ObjectAttributes));

	nts = LsaOpenPolicy(NULL, &ObjectAttributes, POLICY_LOOKUP_NAMES, &polHandle);
	if (nts != ERROR_SUCCESS) {
		string errorMessage = "";
		errorMessage.append("Error unable to open a handle to the Policy object.");
		Log::Debug(errorMessage);
		return;
	}

	NET_API_STATUS nas;
    DWORD recordsEnumerated = 0;
    DWORD totalRecords = 0;
    USER_INFO_0* userInfo = NULL;

	do {
		// NOTE: Even though MAX_PREFERRED_LENGTH is specified, we must still check for
		// ERROR_MORE_DATA. (I think!)  I assume that if the server can not allocate the
		// total amount of space required, then it will allocate a smaller buffer and we
		// will need to make multiple calls to NetUserEnum().
		//
		// NOTE: NetUserEnum() requires us to link to Netapi32.lib.

		nas = NetUserEnum(NULL,
						  0,			// need to us this to get the name
						  0,			//FILTER_NORMAL_ACCOUNT
						  (unsigned char**) &userInfo,
						  MAX_PREFERRED_LENGTH,
						  &recordsEnumerated,
						  &totalRecords,
						  NULL);

		if ((nas == NERR_Success) || (nas == ERROR_MORE_DATA)) {
			// User account names are limited to 20 characters.

			char tmpUserName[21];

			// Loop through each user.
			for (unsigned int i=0; i<recordsEnumerated; i++) {
				ZeroMemory(tmpUserName, 21);
				_snprintf(tmpUserName, sizeof(tmpUserName) - 1, "%S", userInfo[i].usri0_name);
				tmpUserName[sizeof(tmpUserName)-1] = '\0';

				// Get the account information.
				string userName = tmpUserName;
				// get sid for trustee name
				PSID pSid = WindowsCommon::GetSIDForTrusteeName(userName);
				// get formatted trustee name
				userName = WindowsCommon::GetFormattedTrusteeName(pSid);
				allUsers->Append(userName);

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
		throw Exception("Error invlid sid found in WindowsCommon::GetFormattedTrusteeName()");
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
		throw Exception("Could not allocate space. Cannot get domain_name for.");
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
		// all occurances of this that i have seen are for the domain admins sid and the domain user's sid
		// I should be able to ignore these.
		throw Exception("Unable to look up account name for sid: " + sidStr + ". " + WindowsCommon::GetErrorMessage(GetLastError()));

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
	PSID psid = NULL;
	LPTSTR domain = NULL;

	try {
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

		} while (GetLastError() == ERROR_INSUFFICIENT_BUFFER);

	} catch(...) {
		
		Log::Debug("Error looking up sid for account: " + trusteeName + ". " + WindowsCommon::GetErrorMessage(GetLastError()));
	}

	return psid;
}

PSID WindowsCommon::GetSIDForTrusteeSID(string trusteeSID) {

	DWORD sidSize = 128;
	BOOL retVal = FALSE;
	PSID psid;

	try {
		// Call LookupAccountName to get the SID.
		retVal = ConvertStringSidToSid(const_cast<char*>(trusteeSID.c_str()),	// sid string
										&psid);									// security identifier
		
		if(retVal == FALSE) {

			LocalFree(psid);

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

	} catch(...) {
		
		Log::Debug("Error looking up sid for account: " + trusteeSID + ". " + WindowsCommon::GetErrorMessage(GetLastError()));
	}

	return psid;
}

bool WindowsCommon::LookUpTrusteeName(string* accountNameStr, string* sidStr, string* domainStr) {

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

string WindowsCommon::LookUpLocalSystemName() {

	string systemName = "";

	LPTSTR buff = NULL;
	buff = (LPTSTR) malloc(MAX_COMPUTERNAME_LENGTH + 1);
	DWORD  buffSize = MAX_COMPUTERNAME_LENGTH + 1;
 
	// Get and display the name of the computer. 
	if(!GetComputerName( buff, &buffSize )) {
		DWORD error = GetLastError();
		throw Exception("Error failed to get local computer name. " + WindowsCommon::GetErrorMessage(error));
	} else {
		systemName = buff;
		free(buff);
	}

	return systemName;
}

string WindowsCommon::ToString(FILETIME fTime) {

	return Common::ToString(fTime.dwLowDateTime) + Common::ToString(fTime.dwHighDateTime);
}

string WindowsCommon::ToString(DWORD dw) {

	char dwordBuf[12];
	ZeroMemory(dwordBuf, sizeof(dwordBuf));
	_snprintf(dwordBuf, sizeof(dwordBuf)-1, "%d", dw);
	dwordBuf[sizeof(dwordBuf)-1] = '\0';

	string dwStr = dwordBuf;
	return dwStr;
}
