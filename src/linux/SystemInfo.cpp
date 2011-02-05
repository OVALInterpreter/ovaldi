//
//
//****************************************************************************************//
// Copyright (c) 2002-2011, The MITRE Corporation
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

using namespace std;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~		Class SystemInfo		~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

SystemInfo::SystemInfo() {
	// -----------------------------------------------------------------------
	//	ABSTRACT
	//
	//	Initialize data memebres
	//
	// -----------------------------------------------------------------------

	os_name = "";
	os_version = "";
	architecture = "";
	primary_host_name = "";
}

SystemInfo::~SystemInfo() {
	// -----------------------------------------------------------------------
	//	ABSTRACT
	//
	//	Delete all objects in the interfaces vector.
	//
	// -----------------------------------------------------------------------

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

void SystemInfo::Write(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *scDoc)
{
	//------------------------------------------------------------------------------------//
	//
	//  ABSTRACT
	//
	//	Write the system_info node to the sc file.
	//------------------------------------------------------------------------------------//

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
	//------------------------------------------------------------------------------------//
	//
	//  ABSTRACT
	//
	//  Run the system info collector. Return a SystemInfo object. 
	//
	//------------------------------------------------------------------------------------//

	SystemInfo *sysInfo = new SystemInfo();
	SystemInfoCollector::GetOSInfo(sysInfo);
	sysInfo->interfaces = SystemInfoCollector::GetInterfaces();

	return sysInfo;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
#define MAXHOSTNAMELENGTH 256
void SystemInfoCollector::GetOSInfo(SystemInfo *sysInfo) {
	//------------------------------------------------------------------------------------//
	//
	//  ABSTRACT
	//
	//  Get the OS name and version, the architecture, and the primary host name
	//	for the system.
	//
	//------------------------------------------------------------------------------------/

	struct utsname tmpName;
	
	if (uname(&tmpName) > -1) {
		sysInfo->os_name = tmpName.sysname;
		sysInfo->os_version = tmpName.version;
		sysInfo->architecture = tmpName.machine;
	} else {
		// error
		throw SystemInfoException("Error: Unable to get uname information.");
	}

	//////////////////////////////////////////////////////////
	////////// Get fully qulaified host name     /////////////
	//////////////////////////////////////////////////////////

	// First make a call to gethostname()
	string strHostName = "";
	char *chHostName = (char*)malloc(sizeof(char)*(MAXHOSTNAMELENGTH + 1));
	int res = gethostname(chHostName, MAXHOSTNAMELENGTH);

	if(res != 0) {
		free(chHostName);	
		sysInfo->primary_host_name = "unknown";
		Log::Debug("Unable to get short hostname from system");
		return;
	}

	strHostName = chHostName;
	// Next get the fqdn with a call to gethostbyname
	struct hostent *hostData = NULL;
	hostData = gethostbyname((const char*)chHostName); 
	if(hostData != NULL) {
		// Process the hostData structure
		strHostName = hostData->h_name;
	} else {
		Log::Debug("Unable to resolve system FQDN.  Using short hostname instead.");
	}

	sysInfo->primary_host_name = strHostName;
	free(chHostName);
}

IfDataVector SystemInfoCollector::GetInterfaces() {
	//------------------------------------------------------------------------------------//
	//
	//  ABSTRACT
	//
	//  Create a vector of IfData objects that will represent all the available
	//	interfaces on the system.
	//
	//	Must get interface_name, ip_address, and mac_address for each interface
	//------------------------------------------------------------------------------------//

	IfDataVector interfaces;
	
	unsigned char      *u;
	int                sockfd, size  = 1;
	struct ifreq       *ifr;
	struct ifconf      ifc;
	struct sockaddr_in sa;

	if (0 > (sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP))) {
	  throw SystemInfoException("Error: Unable to open socket.");
	}	

	ifc.ifc_len = IFRSIZE;
	ifc.ifc_req = NULL;

	do {
	++size;
	/* realloc buffer size until no overflow occurs  */

		if (NULL == (ifc.ifc_req = (ifreq*)realloc(ifc.ifc_req, IFRSIZE))) {
		  throw SystemInfoException("Error: Unable to allocate mememory.");
		}
		ifc.ifc_len = IFRSIZE;
		if (ioctl(sockfd, SIOCGIFCONF, &ifc)) {
		  throw SystemInfoException("Error: ioctl SIOCFIFCONF.");
		}
	} while  (IFRSIZE <= ifc.ifc_len);

	ifr = ifc.ifc_req;

	for (;(char *) ifr < (char *) ifc.ifc_req + ifc.ifc_len; ++ifr) {

		if (ifr->ifr_addr.sa_data == (ifr+1)->ifr_addr.sa_data) {
		  continue;  // duplicate, skip it
		}

		if (ioctl(sockfd, SIOCGIFFLAGS, ifr)) {
		  continue;  // failed to get flags, skip it
		}

		//printf("Interface:  %s\n", ifr->ifr_name);
		//printf("IP Address: %s\n", inet_ntoa(inaddrr(ifr_addr.sa_data)));
		IfData *tmpIfData = new IfData();
		tmpIfData->ifName = ifr->ifr_name;
		tmpIfData->ipAddress = inet_ntoa(inaddrr(ifr_addr.sa_data));

		if (0 == ioctl(sockfd, SIOCGIFHWADDR, ifr)) {

			/* Select which  hardware types to process.
			*
			*    See list in system include file included from
			*    /usr/include/net/if_arp.h  (For example, on
			*    Linux see file /usr/include/linux/if_arp.h to
			*    get the list.)
			*/
		  /* No used 
			switch (ifr->ifr_hwaddr.sa_family) {

				default:
					printf("\n");
					continue;
				case  ARPHRD_NETROM:  case  ARPHRD_ETHER:  case  ARPHRD_PPP:
				case  ARPHRD_EETHER:  case  ARPHRD_IEEE802: break;
			}
		  */

		  
			u = (unsigned char *) &ifr->ifr_addr.sa_data;
			char *macStr = (char*)malloc(sizeof(char)*128);
			memset(macStr, 0, 128);
			if (u[0] + u[1] + u[2] + u[3] + u[4] + u[5]) {
			  //printf("HW Address: %2.2x.%2.2x.%2.2x.%2.2x.%2.2x.%2.2x\n", u[0], u[1], u[2], u[3], u[4], u[5]);
			  sprintf(macStr, "%2.2X-%2.2X-%2.2X-%2.2X-%2.2X-%2.2X", u[0], u[1], u[2], u[3], u[4], u[5]); 
			  tmpIfData->macAddress = macStr;
			}
			free(macStr);

			interfaces.push_back(tmpIfData);
		}

		/* netmask 
		if (0 == ioctl(sockfd, SIOCGIFNETMASK, ifr) && strcmp("255.255.255.255", inet_ntoa(inaddrr(ifr_addr.sa_data)))) {

			printf("Netmask:    %s\n", inet_ntoa(inaddrr(ifr_addr.sa_data)));
		}
		*/

		/* broadcast
		if (ifr->ifr_flags & IFF_BROADCAST) {

			if (0 == ioctl(sockfd, SIOCGIFBRDADDR, ifr) && strcmp("0.0.0.0", inet_ntoa(inaddrr(ifr_addr.sa_data)))) {
				printf("Broadcast:  %s\n", inet_ntoa(inaddrr(ifr_addr.sa_data)));
			}
		}
		*/
		
		/* MTU
		if (0 == ioctl(sockfd, SIOCGIFMTU, ifr)) {

			printf("MTU:        %u\n",  ifr->ifr_mtu);
		}
		*/

		/* Metric
		if (0 == ioctl(sockfd, SIOCGIFMETRIC, ifr)) {
			printf("Metric:     %u\n",  ifr->ifr_metric);
		}
		printf("\n");
		*/
	}

	close(sockfd);

	return interfaces;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~  Class SystemInfoException  ~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
SystemInfoException::SystemInfoException(string errMsgIn, int severity, Exception* ex) : Exception(errMsgIn, severity, ex) {
	// Set the error message and then set the severity to ERROR_FATAL. This is done with
	// the explicit call to the Exception class constructor that takes a single string
	// param.
}

SystemInfoException::~SystemInfoException() {
	// Do nothing for now
}
