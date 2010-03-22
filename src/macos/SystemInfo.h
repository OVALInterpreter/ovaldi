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

#ifndef SYSTEMINFO_H
#define SYSTEMINFO_H

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

#ifdef LINUX
#include <sys/sysinfo.h>
#endif

#include <sys/utsname.h>

#include <unistd.h>

#include <netdb.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* from net */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <net/if_arp.h>

#ifdef DARWIN
#include <sys/sysctl.h>
#include <net/if_types.h>
#include <net/if_dl.h>
#endif

#define inaddrr(x) (*(struct in_addr *) &ifr->x[sizeof sa.sin_port])
#define IFRSIZE   ((int)(size * sizeof (struct ifreq)))
/* end from net */

#include <iostream>
#include <string>
#include <vector>

XERCES_CPP_NAMESPACE_USE

/**
	This class stores interface information as strings.
	the interface name, mac address and ip address are stored.
*/
class IfData {
public:
	IfData(){};
	~IfData(){};
	IfData(std::string ifn, std::string ipAddr, std::string macAddr) : ifName(ifn), ipAddress(ipAddr), macAddress(macAddr) {}

	std::string ifName;
	std::string ipAddress;
	std::string macAddress;
};

/**	
	A vector for storing interface data dobjects. 
	Stores only pointers to the objects. 
*/
typedef std::vector < IfData* > IfDataVector;

/**
	This class stores system info as defined in the oval system characteristics schema.
	A write method is provide for writing out the system infor element as defined in the oval 
	system characteristics schema.
*/
class SystemInfo {

public:
	SystemInfo();
	~SystemInfo();
	void Write(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *scDoc);
		
	std::string os_name;
	std::string os_version;
	std::string architecture;
	std::string primary_host_name;
	IfDataVector interfaces;
};

/**
	This class is responsible for collecting system information.
*/
class SystemInfoCollector {
	public:
		static SystemInfo* CollectSystemInfo();
		
	private:
		static void GetOSInfo(SystemInfo*);
		static IfDataVector GetInterfaces();
};

/** 
	This class represents an Exception that occured while collecting system info.
*/
class SystemInfoException : public Exception {
	public:
	SystemInfoException(std::string errMsgIn = "", int severity = ERROR_FATAL, Exception* ex = NULL);
		~SystemInfoException();
};

#endif
