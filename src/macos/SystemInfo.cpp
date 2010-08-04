//
//
//****************************************************************************************//
// Copyright (c) 2002-2010, The MITRE Corporation
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
	IfData *tmpIfData = NULL;
	int socketfd, prev_length, length, addr_length, mib[6], mac_length;
        char *ptr, *macbuf, *i, *macstr, *ip;
	unsigned char *macptr = NULL;
	struct ifconf ifc;
        struct in_addr ip4addr;
	struct ifreq *ifr = NULL;
	struct sockaddr_in *sinptr = NULL;
	struct if_msghdr *ifm = NULL;
	struct sockaddr_dl *sdl = NULL;
      
	ptr = NULL;
	macbuf = NULL;
	i = NULL;
	macstr = NULL;
	prev_length = 0;
	length = 10 * sizeof(struct ifreq);
	
	if ( (socketfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
	  throw SystemInfoException("Error: failed to open socket");
	}
     
	while(true){
	  ifc.ifc_buf= (char*)malloc(length);
	  ifc.ifc_len = length;
	  if ( ifc.ifc_buf != NULL ){
	    if ( ioctl(socketfd, SIOCGIFCONF, &ifc) < 0 ){
	      if ( ifc.ifc_buf != NULL ){
		free(ifc.ifc_buf);
		ifc.ifc_buf = NULL;
	      }
	      throw SystemInfoException("Error: SIOCGIFCONF ioctl error");
	    }else{
	      if ( ifc.ifc_len == prev_length){
		ifc.ifc_len = length;
		break;
	      }else{
		prev_length = ifc.ifc_len;
		length = length + 10 * sizeof(struct ifreq);
		free(ifc.ifc_buf);
	      }
	    }
	  }else{
	    throw SystemInfoException("Error: couldn't allocate memory");
	  }	  
	}
	
	if (ioctl(socketfd, SIOCGIFCONF, &ifc) < 0){
          throw SystemInfoException("Error: SIOCGIFCONF ioctl error");
	}
	
	for (ptr = ifc.ifc_buf; ptr <ifc.ifc_buf + ifc.ifc_len; ) {
          ifr = (struct ifreq *) ptr;
          addr_length = sizeof(struct sockaddr);
	  
          if (ifr->ifr_addr.sa_len > addr_length){
            addr_length = ifr->ifr_addr.sa_len;
          }
	  
          ptr += sizeof(ifr->ifr_name) + addr_length;
	  
	  if ( ifr->ifr_addr.sa_family == AF_INET ){
	    
	    if ( ioctl(socketfd,SIOCGIFFLAGS,ifr) ){
	      continue;
	    }
	    
	    if ( !(ifr->ifr_flags & IFF_LOOPBACK) ) {
	      
	      sinptr = (struct sockaddr_in *) &ifr->ifr_addr;
	      mac_length = 0;
	      
	      mib[0] = CTL_NET;
	      mib[1] = AF_ROUTE;
	      mib[2] = 0;
	      mib[3] = AF_LINK;
	      mib[4] = NET_RT_IFLIST;
	      mib[5] = if_nametoindex(ifr->ifr_name);
	      
	      if (mib[5] == 0){
		throw SystemInfoException("Error: if_nametoindex error");
	      }
	      
	      if (sysctl(mib, 6, NULL, (size_t*)&mac_length, NULL, 0) < 0){
		throw SystemInfoException("Error: sysctl error - retrieving data length");
	      }
	      
	      macbuf = (char*) malloc(mac_length);
	      
	      if ( macbuf == NULL ){
		throw SystemInfoException("Error: couldn't allocate memory");
	      }      

	      if (sysctl(mib, 6, macbuf, (size_t*)&mac_length, NULL, 0) < 0){
		throw SystemInfoException("Error: sysctl error - retrieving data");
	      }
	      
	      ifm = (struct if_msghdr *)macbuf;
	      sdl = (struct sockaddr_dl *)(ifm + 1);
	      macptr = (unsigned char *)LLADDR(sdl);
	      i = NULL;
	      ip4addr = (struct in_addr)sinptr->sin_addr;
	      tmpIfData = new IfData();
	      
	      macstr = (char*)malloc(sizeof(char)*128);
	      
	      if ( macstr == NULL ){
		throw SystemInfoException("Error: couldn't allocate memory");
	      }

	      ip = (char*)malloc(sizeof(char)*64);
	      i = (char*)inet_ntop(AF_INET,(void*)(&ip4addr),ip,64);
	
	      tmpIfData->ifName = ifr->ifr_name;
	      tmpIfData->ipAddress = ip;
	      memset(macstr, 0, 128);
	      sprintf(macstr, "%2.2X-%2.2X-%2.2X-%2.2X-%2.2X-%2.2X", macptr[0], macptr[1], macptr[2], macptr[3], macptr[4], macptr[5]);
	      tmpIfData->macAddress = macstr;
	      interfaces.push_back(tmpIfData);
	      free(macbuf);
	      free(macstr);
	      free(ip);
	    }
	  }
	}
	free(ifc.ifc_buf);
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
