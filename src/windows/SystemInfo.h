//
// $Id: SystemInfo.h 4579 2008-01-02 17:39:07Z bakerj $
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

#ifndef SYSTEMINFO_H
#define SYSTEMINFO_H

#pragma warning(disable:4786)

//	required xerces includes
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMAttr.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/parsers/AbstractDOMParser.hpp>

#include "XmlCommon.h"
#include "Common.h"
#include "Exception.h"
#include "Log.h"

#include <iostream>
#include <string>
#include <vector>

//#undef _MAC	// This is a bit messy. windows.h includes winsock.h if _MAC is defined. this causes a conflict with winsock2.h

//#include "winsock2.h"
#ifndef _INC_WINDOWS
#include <windows.h>
#endif
//#include "Ws2tcpip.h"
#include "Iphlpapi.h"

//#define _MAC // see note above

XERCES_CPP_NAMESPACE_USE
using namespace std;

/**
	This class stores interface infocmetion as strings.
	the interface name, mac address and ip address are stored.
*/
class IfData {
public:
	IfData(){};
	~IfData(){};
	IfData(string ifn, string ipAddr, string macAddr) : ifName(ifn), ipAddress(ipAddr), macAddress(macAddr) {}

	string ifName;
	string ipAddress;
	string macAddress;
};

/**	
	A vector for storing interface data dobjects. 
	Stores only pointers to the objects. 
*/
typedef vector < IfData*, allocator<IfData*> > IfDataVector;

/**
	This class stores system info as defined in the oval system characteristics schema.
	A write method is provide for writing out the system infor element as defined in the oval 
	system characteristics schema.
*/
class SystemInfo {

public:
	/** Initialize data memebers. */
	SystemInfo();
	/** Delete all objects in the interfaces vector. */
	~SystemInfo();

	/** Write the system_info node to the sc file. */
	void Write(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *scDoc);
		
	string os_name;
	string os_version;
	string architecture;
	string primary_host_name;
	IfDataVector interfaces;
};

/**
	This class is responsible for collecting system information.
*/
class SystemInfoCollector {
	public:
		/** Run the system info collector. Return a SystemInfo object. */
		static SystemInfo* CollectSystemInfo();
		
	private:
		/** Get the OS name and version, the architecture, and the primary host name for the system. */
		static void GetOSInfo(SystemInfo*);

		/** Create a vector of IfData object that will represent all the available
		 *	interfaces on the system. 		
	     *	Must get interface_name, ip_address, and mac_address for each interface
         */
		static IfDataVector GetInterfaces();
};

/** 
	This class represents an Exception that occured while collecting system info.
*/
class SystemInfoException : public Exception {
	public:
		SystemInfoException(string errMsgIn = "", int severity = ERROR_FATAL, Exception* ex = NULL);
		~SystemInfoException();
};

#endif
