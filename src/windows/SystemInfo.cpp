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

#include <sstream>
#include <iomanip>
#include <cassert>
#include <windows.h>
#include <Iphlpapi.h>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMElement.hpp>

#include "AutoCloser.h"
#include "WindowsCommon.h"
#include "XmlCommon.h"
#include "Common.h"
#include "Log.h"

#include "SystemInfo.h"

using namespace std;
using namespace xercesc;


namespace {
	/**
	 * Gets version and build from the registry.  Key
	 *   HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion
	 * is consulted, with names "CurrentBuild" and "CurrentVersion".
	 * 
	 * \param[out] build receives the build number, must be non-NULL
	 * \param[out] version receives the version, must be non-NULL
	 * \return true on success, false on failure (check GetLastError()).
	 */
	bool getBuildAndVersionFromRegistry(string *build, string *version);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~		Class SystemInfo		~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

SystemInfo::SystemInfo() {

	os_name = "";
	os_version = "";
	architecture = "";
	primary_host_name = "";
}

SystemInfo::~SystemInfo() {

	IfData *tmp	= NULL;
	while(interfaces.size() !=0) {

		tmp = interfaces[interfaces.size()-1];
		interfaces.pop_back();
		delete tmp;
		tmp = NULL;
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

void SystemInfo::Write(xercesc::DOMDocument *scDoc) {

	//	Find the system_info node
	DOMElement* sysInfoNode = XmlCommon::FindElement(scDoc, "system_info");

	XmlCommon::AddChildElementNS(scDoc, sysInfoNode, XmlCommon::scNS, "os_name", os_name);
	XmlCommon::AddChildElementNS(scDoc, sysInfoNode, XmlCommon::scNS, "os_version", os_version);
	XmlCommon::AddChildElementNS(scDoc, sysInfoNode, XmlCommon::scNS, "architecture", architecture);
	XmlCommon::AddChildElementNS(scDoc, sysInfoNode, XmlCommon::scNS, "primary_host_name", primary_host_name);

	// Add the interfaces element and its children
	DOMElement* interfacesElm = XmlCommon::AddChildElementNS(scDoc, sysInfoNode, XmlCommon::scNS, "interfaces");

	// Loop through contents of the interfaces vector and write each IfData objet
	IfDataVector::iterator iterator;
	for (iterator=interfaces.begin(); iterator!=interfaces.end(); iterator++) {

		if((*iterator)->ipAddress.compare("127.0.0.1") != 0) { 
			// Create a new interface element
			DOMElement* interfaceElm = XmlCommon::AddChildElementNS(scDoc, interfacesElm, XmlCommon::scNS, "interface");

			// Add the child to the interface element
			XmlCommon::AddChildElementNS(scDoc, interfaceElm, XmlCommon::scNS, "interface_name", (*iterator)->ifName);
			XmlCommon::AddChildElementNS(scDoc, interfaceElm, XmlCommon::scNS, "ip_address", (*iterator)->ipAddress);
			XmlCommon::AddChildElementNS(scDoc, interfaceElm, XmlCommon::scNS, "mac_address", (*iterator)->macAddress);
		}
	}
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~		 Class SystemInfoCollector		~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

SystemInfo* SystemInfoCollector::CollectSystemInfo() {

	SystemInfo *sysInfo = new SystemInfo();
	SystemInfoCollector::GetOSInfo(sysInfo);
	sysInfo->interfaces = SystemInfoCollector::GetInterfaces();

	return sysInfo;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

void SystemInfoCollector::GetOSInfo(SystemInfo *sysInfo) {

	//////////////////////////////////////////////////////////
	////////////////	Get Archtecture	//////////////////////
	//////////////////////////////////////////////////////////
	// Make a call GetSystemInfo to get architecture
	SYSTEM_INFO siSysInfo;
	GetNativeSystemInfo(&siSysInfo);
	switch(siSysInfo.wProcessorArchitecture) {
	case PROCESSOR_ARCHITECTURE_INTEL:
		sysInfo->architecture = "INTEL32";
		break;
	case PROCESSOR_ARCHITECTURE_IA64:
		sysInfo->architecture = "IA64";
		break;
	case PROCESSOR_ARCHITECTURE_AMD64:
		sysInfo->architecture = "AMD64";
		break;
	default:
		sysInfo->architecture = "unknown";
	}

	//////////////////////////////////////////////////////////
	/////////////	Get os version and name	//////////////////
	//////////////////////////////////////////////////////////
	// Make a call to GetVersionEx() to get the version 
	// information for the OS.
	//
	// Try calling GetVersionEx using the OSVERSIONINFOEX structure.
	// If that fails, try using the OSVERSIONINFO structure.
	OSVERSIONINFOEX osvi;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	BOOL bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi);
	if(!bOsVersionInfoEx) {
		sysInfo->os_name = "unknown";
		sysInfo->os_version = "unknown";
		Log::Message("Couldn't get Windows version: " + WindowsCommon::GetErrorMessage(GetLastError()));
		return;
	}

	if (osvi.dwPlatformId != VER_PLATFORM_WIN32_NT || osvi.dwMajorVersion < 5) {
		sysInfo->os_name = "unknown";
		sysInfo->os_version = "unknown";
		Log::Message("This version of Windows is too old!");
		return;
	}

	// The additional checks which depend on registry values were contributed
	// by a user.
	string regBuild, regVersion;
	if (!getBuildAndVersionFromRegistry(&regBuild, &regVersion)) {
		regBuild.clear();
		regVersion.clear();
		// we'll just have to handle empty values below...
		// not sure we need to give up entirely, just because the
		// registry lookups failed...
	}

// a server version check, intended to be used on 
// OSVERSIONINFOEX::wProductType.  Possibly another way to do this is just
// check _x != VER_NT_WORKSTATION... but I am trying to keep my changes
//minimal...
#define IS_SERVER(_x) ((_x) == VER_NT_SERVER || (_x) == VER_NT_DOMAIN_CONTROLLER)

	// Test for the specific product.
	if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 2 && osvi.wProductType == VER_NT_WORKSTATION) {
		sysInfo->os_name = "Microsoft Windows 8";
		if (regVersion == "6.3") // in case GetVersionEx() lied...
			sysInfo->os_name += ".1";
	} else if(osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 2 && IS_SERVER(osvi.wProductType)) {
		sysInfo->os_name = "Microsoft Windows Server 2012";
		if (regVersion == "6.3")
			sysInfo->os_name += " R2";
	} else if(osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 1 && osvi.wProductType == VER_NT_WORKSTATION) {
		sysInfo->os_name = "Microsoft Windows 7";	
	} else if(osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 1 && IS_SERVER(osvi.wProductType)) {
		sysInfo->os_name = "Microsoft Windows Server 2008 R2";	
	} else if(osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0 && IS_SERVER(osvi.wProductType)) {
		sysInfo->os_name = "Microsoft Windows Server 2008";	
	} else if(osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0 && osvi.wProductType == VER_NT_WORKSTATION) {
		sysInfo->os_name = "Microsoft Windows Vista";	
	} else if(osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 && IS_SERVER(osvi.wProductType)) {
		sysInfo->os_name = "Microsoft Windows Server 2003";	
	} else if(osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1) {
		sysInfo->os_name = "Microsoft Windows XP";
	} else if(osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0) {
		sysInfo->os_name = "Microsoft Windows 2000";
	} else {
		sysInfo->os_name = "unknown";
		sysInfo->os_version = "unknown";
		Log::Message("This version of Windows is unrecognized.");
		return;
	}

	// Test for the workstation type.
	if(osvi.wProductType == VER_NT_WORKSTATION) {
		if(osvi.wSuiteMask & VER_SUITE_PERSONAL) {
			sysInfo->os_name.append(" Home Edition");
		} else {
			sysInfo->os_name.append(" Professional");
		}

	// Test for the server type.
	} else if(IS_SERVER(osvi.wProductType)) {

		if((osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2) ||
			(osvi.dwMajorVersion == 6 && 0 <= osvi.dwMinorVersion && osvi.dwMinorVersion <= 2)) {

			if(osvi.wSuiteMask & VER_SUITE_DATACENTER) {
				sysInfo->os_name.append(" Datacenter Edition");
			} else if(osvi.wSuiteMask & VER_SUITE_ENTERPRISE) {
				sysInfo->os_name.append(" Enterprise Edition");
			} else if (osvi.wSuiteMask & VER_SUITE_BLADE) {
				sysInfo->os_name.append(" Web Edition");
			} else {
				sysInfo->os_name.append(" Standard Edition");
			}

		} else if( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 ) {
			if(osvi.wSuiteMask & VER_SUITE_DATACENTER) {
				sysInfo->os_name.append(" Datacenter Server");
			} else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE) {
				sysInfo->os_name.append(" Advanced Server");
			} else {
				sysInfo->os_name.append(" Server");
			}
		}
	} // else, just leave os_name as-is, I guess

#undef IS_SERVER

	// Get Version number
	if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 2 && regVersion == "6.3") {
		// ignore GetVersionEx() if it lied...
		sysInfo->os_version = regVersion;
		if (!regBuild.empty())
			sysInfo->os_version += "." + regBuild;
	} else {
		sysInfo->os_version =
			Common::ToString(osvi.dwMajorVersion) + '.' +
			Common::ToString(osvi.dwMinorVersion) + '.' +
			Common::ToString(osvi.dwBuildNumber);
	}

	if (osvi.szCSDVersion[0] != 0) {
		sysInfo->os_version += string(" ") +
		osvi.szCSDVersion;
	}

	//////////////////////////////////////////////////////////
	//////////	 Get fully qualified host name	//////////////
	//////////////////////////////////////////////////////////
	
	// NOTE: the gethostbyname() method is deprecated. 
	// This should be replaced with the new methods
  
	WORD version = MAKEWORD(2,2);
	WSADATA wsaData;
	
	if(WSAStartup(version, &wsaData) != 0) {
		sysInfo->primary_host_name = "unknown";	
		throw SystemInfoException("Error: Unable to get hostname. Sockets could not be initialized.");
	}

	// Confirm that the WinSock DLL supports 2.2.  Note that if the DLL supports versions
	// greater than 2.2 in addition to 2.2, it will still return 2.2 in wVersion since that
	// is the version we requested.
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2 ) {
		sysInfo->primary_host_name = "unknown";	
		WSACleanup();
		throw SystemInfoException("Error: Unable to get hostname. Could not find a usable version of the WinSock DLL.");
	}

	// First get the host name
	char host_name[256];
	int res = gethostname(host_name, sizeof(host_name));
	if (res != 0) {
		res = WSAGetLastError();
		sysInfo->primary_host_name = "unknown";
		Log::Debug("Unable to get short hostname from system");
	} else {
		// next get the fully qualified host name
		hostent *remoteHost;
		remoteHost = gethostbyname(host_name);
		if(remoteHost != NULL) {
			sysInfo->primary_host_name = remoteHost->h_name;	
		} else {
			sysInfo->primary_host_name = host_name;
			Log::Debug("Unable to resolve system FQDN.  Using short hostname instead.");
		}
	}
}

IfDataVector SystemInfoCollector::GetInterfaces() {

	IfDataVector interfaces;


	PMIB_IPADDRTABLE pIPAddrTable;
	DWORD dwSize = 0;
	pIPAddrTable = (MIB_IPADDRTABLE*) malloc( sizeof( MIB_IPADDRTABLE) );
	if(pIPAddrTable == NULL) {
		throw SystemInfoException("Error: Unable to allocate memeory while gathering interface information.");
	}

	// First call to GetIpAddrTable to get the required size
	if (GetIpAddrTable(pIPAddrTable, &dwSize, 0) == ERROR_INSUFFICIENT_BUFFER) {
		free(pIPAddrTable);
		pIPAddrTable = (MIB_IPADDRTABLE *) malloc ( dwSize );
	}

	// Make a second call to GetIpAddrTable to get the
	// actual data we want
	if (GetIpAddrTable(pIPAddrTable, &dwSize, 0) == NO_ERROR) { 

		// Now loop through the entries in the table to get the available interfaces.
		unsigned int index = 0; 
		while(index < pIPAddrTable->dwNumEntries) {
			IfData *curIf = new IfData();

			// Convert the ip adddress to a string
			in_addr addr;
			addr.S_un.S_addr = pIPAddrTable->table[index].dwAddr;
			curIf->ipAddress = inet_ntoa(addr);


			// next get the interface name and mac address by calling GetIfEntry
			// with the specified interface index of this addr
			// Declare and initialize variables.
			PMIB_IFROW pMibIfRow;

			// Allocate memory for our pointers.
			pMibIfRow = (MIB_IFROW*) malloc(sizeof(MIB_IFROW));
			// Set index to the current index
			pMibIfRow->dwIndex = pIPAddrTable->table[index].dwIndex;
			
			if (GetIfEntry(pMibIfRow) == NO_ERROR) {
				
				// Get interface name
				//
				// TODO:
				// I think the correct place to look for the interface name is to 
				// look at the value of pMibIfRow->wszName. However, when I inspect
				// this variable's value it appears to be unused. I have been unable
				// to find any samples of this value being successfully used.
				//
				// For now I have decided to use the value of the description as the 
				// name for an interface. I will leave a chunk of code commented out
				// below that should be capable of retreiving the interface name.
				//
				//
				
				/*
				// This is a consise convertion of the name
				int ifNameLen = wcslen(pMibIfRow->wszName);
				char *charIfName = (char*)malloc(ifNameLen + 1);
				memset(charIfName, 0, ifNameLen + 1);
				if(wcstombs(charIfName, pMibIfRow->wszName, ifNameLen) == (size_t)-1) {
					curIf->ifName = "unknown";
				} else {
					curIf->ifName = charIfName;
				}
					
				// here is a more verbose conversion.
				char a[2] = "j";
				int ret = WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, pMibIfRow->wszName, ifNameLen, charIfName, ifNameLen, a, NULL);
				cout << "ret " << ret << endl;
				if (ret == 0) {
					DWORD err = GetLastError();
						switch(err) {
					case ERROR_INSUFFICIENT_BUFFER:
						cout << "1" << endl;
						break;
					case ERROR_INVALID_FLAGS:
						cout << "2" << endl;
						break;
					case ERROR_INVALID_PARAMETER:
						cout << "3" << endl;
						break;
					default:
						cout << "what?" << endl;
						break;
					}	
				}
  
				// Cleanup
				free(charIfName);
				*/

				// Here I am using the description value as the name
				curIf->ifName = string((char*)&pMibIfRow->bDescr[0], pMibIfRow->dwDescrLen);

				ostringstream macOss;
				macOss << hex << uppercase << setfill('0');
				if (pMibIfRow->dwPhysAddrLen > 0)
					macOss << setw(2) << (int)pMibIfRow->bPhysAddr[0];
				for (DWORD i = 1; i < pMibIfRow->dwPhysAddrLen; ++i)
					macOss << '-' << setw(2) << (int)pMibIfRow->bPhysAddr[i];

				curIf->macAddress = macOss.str();
			} else {

				curIf->ifName = "unknown";
				curIf->macAddress = "unknown";
			}
			free(pMibIfRow);
			
			interfaces.push_back(curIf);
			index ++;
		}
		
		free(pIPAddrTable);

	} else {
		free(pIPAddrTable);
		throw SystemInfoException("Error: Call to GetIpAddrTable failed. Unable to get interfaces information.");
	}

	
	return interfaces;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~  Class SystemInfoException  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
SystemInfoException::SystemInfoException(string errMsgIn, int severity, Exception* ex) : Exception(errMsgIn, severity, ex) {
	// Set the error message and then set the severity to ERROR_FATAL. This is done with
	// the explicit call to the Exception class constructor that takes a single string
	// param.
}

SystemInfoException::~SystemInfoException() {
	// Do nothing for now
}

namespace {
	bool getBuildAndVersionFromRegistry(string *build, string *version) {

		assert(build && version);

#define CV_KEY_PATH "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"

		HKEY cvKey;
		LONG err = RegOpenKeyExA(HKEY_LOCAL_MACHINE,
			CV_KEY_PATH,
			0,
			KEY_READ,
			&cvKey);

		if (err) {
			Log::Message("Couldn't open registry key HKLM\\" CV_KEY_PATH ": "+
				WindowsCommon::GetErrorMessage(err));
			return false;
		}

		AutoCloser<HKEY, LONG(WINAPI&)(HKEY)> keyCloser(
			cvKey,
			RegCloseKey,
			"Registry key HKLM\\" CV_KEY_PATH);


		DWORD valType;
		BYTE val[20];
		DWORD valSize;

		// --- CurrentBuild
		valSize = sizeof(val);
		err = RegQueryValueExA(
			cvKey,
			"CurrentBuild",
			NULL,
			&valType,
			val,
			&valSize
			);

		if (err) {
			Log::Message("Couldn't read from registry key HKLM\\" CV_KEY_PATH " value \"CurrentBuild\": "+
				WindowsCommon::GetErrorMessage(err));
			return false;
		}

		if (valSize == 0)
			build->clear();
		else if (val[valSize-1] == 0)
			// careful of null-termination...
			build->assign(reinterpret_cast<char*>(&val[0]), valSize-1);
		else
			build->assign(reinterpret_cast<char*>(&val[0]), valSize);


		// --- CurrentVersion
		valSize = sizeof(val);
		err = RegQueryValueExA(
			cvKey,
			"CurrentVersion",
			NULL,
			&valType,
			val,
			&valSize
			);

		if (err) {
			Log::Message("Couldn't read from registry key HKLM\\" CV_KEY_PATH " value \"CurrentVersion\": "+
				WindowsCommon::GetErrorMessage(err));
			return false;
		}

		if (valSize == 0)
			version->clear();
		else if (val[valSize-1] == 0)
			// careful of null-termination...
			version->assign(reinterpret_cast<char*>(&val[0]), valSize-1);
		else
			version->assign(reinterpret_cast<char*>(&val[0]), valSize);

		return true;

#undef CV_KEY_PATH

	}
}