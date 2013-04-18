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

#include <sys/utsname.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstdlib>

//	required xerces includes
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMElement.hpp>

#include "XmlCommon.h"
#include "Common.h"
#include "Log.h"
#include <NetworkInterfaces.h>

#include "SystemInfo.h"

using namespace std;
using namespace xercesc;

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
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

void SystemInfo::Write(DOMDocument *scDoc)
{
	//------------------------------------------------------------------------------------//
	//
	//  ABSTRACT
	//
	//	Write the system_info node to the sc file.
	//------------------------------------------------------------------------------------//

	//	Find the system_info node
	DOMElement* sysInfoNode = XmlCommon::FindElement(scDoc, "system_info");

	DOMElement* tmpElm = XmlCommon::CreateElementNS(scDoc, XmlCommon::scNS, "os_name", os_name);
	sysInfoNode->appendChild(tmpElm);

	tmpElm = XmlCommon::CreateElementNS(scDoc, XmlCommon::scNS, "os_version", os_version);
	sysInfoNode->appendChild(tmpElm);

	tmpElm = XmlCommon::CreateElementNS(scDoc, XmlCommon::scNS, "architecture", architecture);
	sysInfoNode->appendChild(tmpElm);

	tmpElm = XmlCommon::CreateElementNS(scDoc, XmlCommon::scNS, "primary_host_name", primary_host_name);
	sysInfoNode->appendChild(tmpElm);

	// Add the interfaces element and its children
	DOMElement* interfacesElm = XmlCommon::CreateElementNS(scDoc, XmlCommon::scNS, "interfaces");
	sysInfoNode->appendChild(interfacesElm);

	// Loop through contents of the interfaces vector and write each IfData objet
	IntfList::iterator iterator;
	for (iterator=interfaces.begin(); iterator!=interfaces.end(); iterator++) {

		// Create a new interface element
		DOMElement* interfaceElm = XmlCommon::CreateElementNS(scDoc, XmlCommon::scNS, "interface");
		interfacesElm->appendChild(interfaceElm);

		// Add the childer to the inerface element
		tmpElm = XmlCommon::CreateElementNS(scDoc, XmlCommon::scNS, "interface_name", iterator->GetName());
		interfaceElm->appendChild(tmpElm);
			
		tmpElm = XmlCommon::CreateElementNS(scDoc, XmlCommon::scNS, "ip_address", inet_ntoa(iterator->GetIPAddr()));
		interfaceElm->appendChild(tmpElm);

		tmpElm = XmlCommon::CreateElementNS(scDoc, XmlCommon::scNS, "mac_address", iterator->GetHwAddr());
		interfaceElm->appendChild(tmpElm);
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

IntfList SystemInfoCollector::GetInterfaces() {
	//------------------------------------------------------------------------------------//
	//
	//  ABSTRACT
	//
	//  Create a vector of IfData objects that will represent all the available
	//	interfaces on the system.
	//
	//	Must get interface_name, ip_address, and mac_address for each interface
	//------------------------------------------------------------------------------------//

	using namespace NetworkInterfaces;
	list<Interface> intfs = GetAllInterfaces();
	for (list<Interface>::iterator iter = intfs.begin();
		 iter != intfs.end();
		 ) {
		// only ethernet interfaces are supported for now, filter out others.
		if (iter->GetType() == Interface::ETHERNET)
			++iter;
		else
			iter = intfs.erase(iter);
	}

	return intfs;
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
