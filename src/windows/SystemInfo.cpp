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

#include "SystemInfo.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~		Class SystemInfo		~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

SystemInfo::SystemInfo() {

	os_name = "";
	os_version = "";
	architecture = "";
	primary_host_name = "";
	interfaces;
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

void SystemInfo::Write(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *scDoc) {

	//	Find the system_info node
	DOMElement* sysInfoNode = XmlCommon::FindElement(scDoc, "system_info");

	DOMElement* tmpElm = XmlCommon::CreateElement(scDoc, "os_name", os_name);
	sysInfoNode->appendChild(tmpElm);

	tmpElm = XmlCommon::CreateElement(scDoc, "os_version", os_version);
	sysInfoNode->appendChild(tmpElm);

	tmpElm = XmlCommon::CreateElement(scDoc, "architecture", architecture);
	sysInfoNode->appendChild(tmpElm);

	tmpElm = XmlCommon::CreateElement(scDoc, "primary_host_name", primary_host_name);
	sysInfoNode->appendChild(tmpElm);

	// Add the interfaces element and its children
	DOMElement* interfacesElm = XmlCommon::CreateElement(scDoc, "interfaces");
	sysInfoNode->appendChild(interfacesElm);

	// Loop through contents of the interfaces vector and write each IfData objet
	IfDataVector::iterator iterator;
	for (iterator=interfaces.begin(); iterator!=interfaces.end(); iterator++) {

		if((*iterator)->ipAddress.compare("127.0.0.1") != 0) { 
			// Create a new interface element
			DOMElement* interfaceElm = XmlCommon::CreateElement(scDoc, "interface");
			interfacesElm->appendChild(interfaceElm);

			// Add the childer to the inerface element
			tmpElm = XmlCommon::CreateElement(scDoc, "interface_name", (*iterator)->ifName);
			interfaceElm->appendChild(tmpElm);
			
			tmpElm = XmlCommon::CreateElement(scDoc, "ip_address", (*iterator)->ipAddress);
			interfaceElm->appendChild(tmpElm);

			tmpElm = XmlCommon::CreateElement(scDoc, "mac_address", (*iterator)->macAddress);
			interfaceElm->appendChild(tmpElm);
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
	GetSystemInfo(&siSysInfo);
	if(siSysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_UNKNOWN) {
		sysInfo->architecture = "unknown";

	} else if(siSysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL) {
		sysInfo->architecture = "INTEL32";

	} else if(siSysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64) {
		sysInfo->architecture = "IA64";

	} else if(siSysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64) {
		sysInfo->architecture = "AMD64";
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
	BOOL bOsVersionInfoEx;	// flag to indicate that the OSVERSIONINFOEX strucutre was used

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if(!(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi))) {

		osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		
		if (!GetVersionEx((OSVERSIONINFO*)&osvi)) {
			sysInfo->os_name = "unknown";
			sysInfo->os_version = "unknown";
			return;
		}
	}
	
	// Allocate memory for the version number string
	const int verNumSize = 16;
	char *verNum = (char*)malloc(sizeof(char)*verNumSize);
	if(verNum == NULL) {
		throw SystemInfoException("Error: Unable to allocate memeory while gathering Operating System information.");
	}

	switch (osvi.dwPlatformId) {

		// Test for the Windows NT product family.
		case VER_PLATFORM_WIN32_NT:

			// Test for the specific product.
			if(osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2) {
				sysInfo->os_name = "Microsoft Windows Server 2003";	
			} else if(osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1) {
				sysInfo->os_name = "Microsoft Windows XP";
			} else if(osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0) {
				sysInfo->os_name = "Microsoft Windows 2000";
			} else if(osvi.dwMajorVersion <= 4) {
					sysInfo->os_name = "Microsoft Windows NT";
			} else {
				sysInfo->os_name = "unknown";
			}


			// Test for specific product on Windows NT 4.0 SP6 and later.
			// using OSVERSIONINFOEX
			if(bOsVersionInfoEx) {

				// Test for the workstation type.
				if(osvi.wProductType == VER_NT_WORKSTATION) {
					if( osvi.dwMajorVersion == 4 ) {
						sysInfo->os_name.append(" Workstation 4.0");
					} else if(osvi.wSuiteMask & VER_SUITE_PERSONAL) {
						sysInfo->os_name.append(" Home Edition");
					} else {
						sysInfo->os_name.append(" Professional");
					}

				// Test for the server type.
				} else if(osvi.wProductType == VER_NT_SERVER || osvi.wProductType == VER_NT_DOMAIN_CONTROLLER) {
				
					if(osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2) {
						
						if(osvi.wSuiteMask & VER_SUITE_DATACENTER) {
							sysInfo->os_name.append(" Datacenter Edition");
						} else if(osvi.wSuiteMask & VER_SUITE_ENTERPRISE) {
							sysInfo->os_name.append(" Enterprise Edition");
						} else if (osvi.wSuiteMask == VER_SUITE_BLADE) {
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

				// Windows NT 4.0
				} else {
					if(osvi.wSuiteMask & VER_SUITE_ENTERPRISE) {
						sysInfo->os_name.append(" Server 4.0, Enterprise Edition");
					} else {
						sysInfo->os_name.append(" Server 4.0");
					}
				}

				// Get Version number
				_itoa(osvi.dwMajorVersion, verNum, 10);
				sysInfo->os_version = verNum;
				ZeroMemory(verNum, verNumSize);
				_itoa(osvi.dwMinorVersion, verNum, 10);
				sysInfo->os_version.append(".");
				sysInfo->os_version.append(verNum);
				ZeroMemory(verNum, verNumSize);
				_itoa(osvi.dwBuildNumber, verNum, 10);
				sysInfo->os_version.append(".");
				sysInfo->os_version.append(verNum);				
				
		
			// Test for specific product on Windows NT 4.0 SP5 and earlier
			// using OSVERSIONINFO
			} else {  

				HKEY hKey;
				const int BUFSIZE = 80;
				char szProductType[BUFSIZE];
				DWORD dwBufLen=BUFSIZE;
				LONG lRet;

				lRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\ProductOptions", 0, KEY_QUERY_VALUE, &hKey );
				if(lRet != ERROR_SUCCESS) {
					sysInfo->os_name = "unknown";
					sysInfo->os_version = "unknown";
					break;
				}

				lRet = RegQueryValueEx( hKey, "ProductType", NULL, NULL, (LPBYTE) szProductType, &dwBufLen);
				if((lRet != ERROR_SUCCESS) || (dwBufLen > BUFSIZE)) {
					sysInfo->os_name = "unknown";
					sysInfo->os_version = "unknown";
					break;
				}

				RegCloseKey(hKey);

				if(lstrcmpi( "WINNT", szProductType) == 0) {
					sysInfo->os_name.append(" Workstation");
				}else if(lstrcmpi( "LANMANNT", szProductType) == 0) {
					sysInfo->os_name.append(" Server");
				}else if(lstrcmpi( "SERVERNT", szProductType) == 0) {
					sysInfo->os_name.append(" Advanced Server");
				}

				// Get Version number
				_itoa(osvi.dwMajorVersion, verNum, 10);
				sysInfo->os_version = verNum;
				ZeroMemory(verNum, verNumSize);
				_itoa(osvi.dwMinorVersion, verNum, 10);
				sysInfo->os_version.append(".");
				sysInfo->os_version.append(verNum);
			}

			// Display service pack (if any) and build number.
			if(osvi.dwMajorVersion == 4 && lstrcmpi(osvi.szCSDVersion, "Service Pack 6") == 0) {
				HKEY hKey;
				LONG lRet;

				// Test for SP6 versus SP6a.
				lRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Hotfix\\Q246009", 0, KEY_QUERY_VALUE, &hKey );
				if(lRet == ERROR_SUCCESS) {
					sysInfo->os_name.append(" Service Pack 6a");
					
				
				// Windows NT 4.0 prior to SP6a
				} else {
					sysInfo->os_name.append(" ");
					sysInfo->os_name.append(osvi.szCSDVersion);
				}
				RegCloseKey(hKey);

				// Add the build number to the version string
				sysInfo->os_version.append(".");
				_itoa((osvi.dwBuildNumber & 0xFFFF), verNum, 10);
				sysInfo->os_version.append(verNum);       
			
			// not Windows NT 4.0 
			} else {
				sysInfo->os_name.append(" ");
				sysInfo->os_name.append(osvi.szCSDVersion);
			}

			break;

		// Test for the Windows Me/98/95.
		case VER_PLATFORM_WIN32_WINDOWS:

			// Look for Win 95
			if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0) {
				sysInfo->os_name = "Microsoft Windows 95";
			}else if (osvi.szCSDVersion[1] == 'C' || osvi.szCSDVersion[1] == 'B') {
				sysInfo->os_name.append(" OSR2");
			} 

			// Look for Win 98
			if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10) {
				sysInfo->os_name = "Microsoft Windows 98";
			
				if (osvi.szCSDVersion[1] == 'A')
					sysInfo->os_name.append(" SE");
			} 

			// Look for Win Me
			if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90) {
				sysInfo->os_name = "Microsoft Windows Millennium Edition";
			} 
			
			// Get Version number
			_itoa(osvi.dwMajorVersion, verNum, 10);
			sysInfo->os_version = verNum;
			ZeroMemory(verNum, verNumSize);
			_itoa(osvi.dwMinorVersion, verNum, 10);
			sysInfo->os_version.append(".");
			sysInfo->os_version.append(verNum);
			ZeroMemory(verNum, verNumSize);
			_itoa((osvi.dwBuildNumber & 0xFFFF), verNum, 10);
			sysInfo->os_version.append(".");
			sysInfo->os_version.append(verNum);

			break;

		case VER_PLATFORM_WIN32s:

			sysInfo->os_name = "Microsoft Win32s";

			// Get Version number
			_itoa(osvi.dwMajorVersion, verNum, 10);
			sysInfo->os_version = verNum;
			ZeroMemory(verNum, verNumSize);
			_itoa(osvi.dwMinorVersion, verNum, 10);
			sysInfo->os_version.append(".");
			sysInfo->os_version.append(verNum);
			ZeroMemory(verNum, verNumSize);
			_itoa((osvi.dwBuildNumber & 0xFFFF), verNum, 10);
			sysInfo->os_version.append(".");
			sysInfo->os_version.append(verNum);		

			break;

		default:
			sysInfo->os_name = "unknown";
			sysInfo->os_version = "unknown";
			break;

	} // end switch

	free(verNum);

	//////////////////////////////////////////////////////////
	//////////	 Get fully qulaified host name	//////////////
	//////////////////////////////////////////////////////////
	
	// NOTE: the gethostbyname() method is depricated. 
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
	char *host_name;
	const int size = 256;
	host_name = (char*)malloc(sizeof(char)*size);
	if(host_name == NULL) {
		sysInfo->primary_host_name = "unknown";	
		throw SystemInfoException("Error: Unable to allocate memeory while gathering Operating System information.");
	}

	int ret = gethostname(host_name, size);
	if (ret != 0) {
		ret =  WSAGetLastError();
	
		sysInfo->primary_host_name = "unknown";	
		//throw SystemInfoException("Error: Unable to get hostname,");

	} else {
		// next get the fully qualified host name
		hostent *remoteHost;
		remoteHost = gethostbyname(host_name);
		if(remoteHost == NULL) {
			sysInfo->primary_host_name = "unknown";	
			//throw SystemInfoException("Error: Unable to get hostname,");
		} else {
			sysInfo->primary_host_name = remoteHost->h_name;	
		}
	}

	free(host_name);
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
			DWORD dwSize = 0;
			DWORD dwRetVal = 0;

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
				char *descStr = (char*)malloc(pMibIfRow->dwDescrLen+1);
				if(descStr == NULL) {
					free(pIPAddrTable);
					throw SystemInfoException("Error: Unable to allocate memeory while gathering interface information.");
				}
				ZeroMemory(descStr, sizeof(pMibIfRow->dwDescrLen+1));
				for (unsigned int j=0;j<pMibIfRow->dwDescrLen;j++) {
					descStr[j] = (char)pMibIfRow->bDescr[j];
					//sprintf(&descStr[j],"%s",pMibIfRow->bDescr[j]);
				}
				curIf->ifName = descStr;
				free(descStr);


				// Format MAC Address
				char *macStr = (char*)malloc(sizeof(char)*30);				
				if(macStr == NULL) {
					free(pIPAddrTable);
					throw SystemInfoException("Error: Unable to allocate memeory while gathering interface information.");
				}
				ZeroMemory(macStr, 30);
				for (unsigned int i=0;i<pMibIfRow->dwPhysAddrLen;i++) {
					sprintf(&macStr[i*3],"%02X-",pMibIfRow->bPhysAddr[i]);
				}
				curIf->macAddress = macStr;
				curIf->macAddress = curIf->macAddress.substr(0, curIf->macAddress.length()-1);
				free(macStr);

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
