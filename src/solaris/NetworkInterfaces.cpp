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

#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <iomanip>
#include <sstream>

#include <Noncopyable.h>
#include <IOException.h>
#include <OutOfMemoryException.h>
#include <SocketGuard.h>
#include <Log.h>

#include <NetworkInterfaces.h>

using namespace std;

namespace {
	/**
	 * Finds the names of all network interfaces.
	 */
	list<string> GetInterfaceNames();

	/**
	 * Maps from an sa_family_t value to one of the Interface::LinkType
	 * enumerators.
	 */
	NetworkInterfaces::Interface::LinkType GetLinkType(sa_family_t family);

	/**
	 * Given a MAC address in a sockaddr struct, create the usual
	 * xx-xx-xx-xx-xx-xx formatted string from it.
	 */
	string FormatMACAddress(struct sockaddr hwAddr);
}

namespace NetworkInterfaces {

	Interface GetInterface(const std::string &name) {
		struct ifreq req;
		int result;

		short flags = 0;           // the type of struct ifreq.ifr_flags
		struct sockaddr hwAddr;    // the type of struct ifreq.ifr_hwaddr (or arp.arp_ha on solaris)
		struct sockaddr ipAddr;    // the type of struct ifreq.ifr_addr
		struct sockaddr broadAddr; // the type of struct ifreq.ifr_broadaddr
		struct sockaddr netmask;   // the type of struct ifreq.ifr_netmask

		// All the ioctl() calls we make require a real socket to work off of.
		SocketGuard s(AF_INET, SOCK_DGRAM, 0);

		Log::Debug("Querying interface " + name + "...");

		// error messages below will have this common prefix.
		string errorMsgPrefix = "Error querying network interface "+name+": ";

		// the ifconfig source copies the interface name in
		// before every ioctl() call.

		// get interface flags
		strcpy(req.ifr_name, name.c_str());
		result = ioctl(s, SIOCGIFFLAGS, &req);
		if (result >= 0) {
			flags = req.ifr_flags;
		} else {
			throw IOException(errorMsgPrefix + "ioctl SIOCGIFFLAGS: " + strerror(errno));
		}

		// The following requests require an ipv4 interface.

		// see if we can get ipv4 address
		strcpy(req.ifr_name, name.c_str());
		req.ifr_addr.sa_family = AF_INET;
		result = ioctl(s, SIOCGIFADDR, &req);
		if (result >= 0) {

			ipAddr = req.ifr_addr;

			// get broadcast address, if valid
			if (flags & IFF_BROADCAST) {
				strcpy(req.ifr_name, name.c_str());
				result = ioctl(s, SIOCGIFBRDADDR, &req);

				if (result >= 0) {
					broadAddr = req.ifr_broadaddr;
				} else {
					throw IOException(errorMsgPrefix + "ioctl SIOCGIFBRDADDR: " + strerror(errno));
				}
			}

			// get netmask
			strcpy(req.ifr_name, name.c_str());
			result = ioctl(s, SIOCGIFNETMASK, &req);
			if (result >= 0) {
				// solaris doesn't have ifr_netmask... ifr_addr yields
				// the same thing and works on both solaris and linux, so I will use
				// that instead.
				//netmask = req.ifr_netmask;
				netmask = req.ifr_addr;
			} else {
				throw IOException(errorMsgPrefix + "ioctl SIOCGIFNETMASK error: "+strerror(errno));
			}
		}

		// hardware address/type
		Interface::LinkType type;
		// on solaris, this requires having obtained the interface address
		// first.
		if (flags & IFF_LOOPBACK)
			// on solaris, the SIOCGARP ioctl fails on the loopback... so check
			// the flags and don't even bother if the loopback flag is set.
			type = Interface::LOOPBACK;
		else {	
			// gotta get MAC address via an ARP ioctl on solaris
			struct arpreq arp;

			memset(&arp, 0, sizeof(arp));
			arp.arp_pa = ipAddr;
			result = ioctl(s, SIOCGARP, &arp);
			if (result >= 0) {
				// I don't know how to get the hardware type on solaris.  (The types
				// are defined in the if_arp header and named ARPHRD_*, so you'd think
				// it might have something to do with the ARP ioctl(), but
				// arp_ha.sa_family doesn't seem to be filled out.)  So if the ioctl
				// succeeds at all, I will assume that the hw type is ethernet...
				// Is there a better way of doing this??
				hwAddr = arp.arp_ha;
				type = Interface::ETHERNET;
			} else
				throw IOException(errorMsgPrefix +
								  "ioctl SIOCGARP: " +
								  strerror(errno));
		}	

		string macAddr;
		if (type == Interface::ETHERNET)
			macAddr = FormatMACAddress(hwAddr);

		Interface intf(name, type);
		intf.SetFlags(flags);
		intf.SetHwAddr(macAddr);
		intf.SetIPAddr(((struct sockaddr_in*)&ipAddr)->sin_addr);
		if (flags & IFF_BROADCAST)
			intf.SetBroadAddr(((struct sockaddr_in*)&broadAddr)->sin_addr);
		intf.SetNetmask(((struct sockaddr_in*)&netmask)->sin_addr);

		return intf;
	}

	list<Interface> GetAllInterfaces() {
		list<string> ifNames = GetInterfaceNames();
		list<Interface> interfaces;

		for (list<string>::iterator nameIter = ifNames.begin();
			 nameIter != ifNames.end();
			 ++nameIter)
			interfaces.push_back(GetInterface(*nameIter));

		return interfaces;
	}
}

namespace {
	list<string> GetInterfaceNames() {

		// This technique was taken from the implementation of the linux
		// 'ifconfig' utility.  An initial buffer is allocated for the
		// results of an ioctl SIOCGIFCONF request and repeatedly increased
		// in size until the ioctl request does not fill it up.  That way
		// we know we got all the interfaces.

		list<string> names;

		SocketGuard s(AF_INET, SOCK_DGRAM, 0);

		struct ifconf ifc;
		ifc.ifc_buf = NULL;
		size_t recordsAllocated = 0; //how much I allocated (in records)
		size_t bytesUsed; //how much did ioctl actually use (in bytes)
		size_t recordsUsed; //how much did ioctl actually use (in records)
		do {
			recordsAllocated += 5;
			ifc.ifc_len = recordsAllocated * sizeof(struct ifreq);
			caddr_t tmpBuf = (caddr_t)realloc(ifc.ifc_buf, ifc.ifc_len);

			// make sure the allocation succeeded
			if (tmpBuf == NULL) {
				if (ifc.ifc_buf != NULL)
					free(ifc.ifc_buf);
				throw OutOfMemoryException("Unable to allocate "+
										   Common::ToString(ifc.ifc_len) +
										   " bytes of memory for ioctl SIOCGIFCONF request!");
			}

			ifc.ifc_buf = tmpBuf;

			int res = ioctl(s, SIOCGIFCONF, &ifc);
			if (res < 0) {
				free(ifc.ifc_buf);
				throw IOException(string("ioctl SIOCGIFCONF error: ")+strerror(errno));
			}

			bytesUsed = ifc.ifc_len;
			recordsUsed = bytesUsed / sizeof(struct ifreq);
		} while(recordsUsed == recordsAllocated);

		struct ifreq *reqPtr = ifc.ifc_req;
		size_t i;
		for (i=0; i<recordsUsed; i++) {

			// From my experiments, other fields of the ifreq struct were
			// not valid.  So ioctl SIOCGIFCONF is just used to get names.

			names.push_back(reqPtr->ifr_name);
			reqPtr++;
		}

		free(ifc.ifc_buf);

		return names;
	}

	NetworkInterfaces::Interface::LinkType GetLinkType(sa_family_t family) {

		using namespace NetworkInterfaces;

		switch(family) {
#ifdef ARPHRD_ETHER
		case ARPHRD_ETHER: return Interface::ETHERNET;
#endif

#ifdef ARPHRD_FDDI
		case ARPHRD_FDDI: return Interface::FDDI;
#endif

#ifdef ARPHRD_LOOPBACK
		case ARPHRD_LOOPBACK: return Interface::LOOPBACK;
#endif

#ifdef ARPHRD_VOID
		case ARPHRD_VOID: return Interface::UNKNOWN;
#endif

#ifdef ARPHRD_PPP
		case ARPHRD_PPP: return Interface::PPP;
#endif

#ifdef ARPHRD_SLIP
		case ARPHRD_SLIP: return Interface::SLIP;
#endif

#ifdef ARPHRD_PRONET
		case ARPHRD_PRONET: return Interface::PRONET;
#endif
		default:
			Log::Info("Encountered an unrecognized interface type: " + Common::ToString(family));
			return Interface::UNKNOWN;
		}
	}

	string FormatMACAddress(struct sockaddr hwAddr) {
		char *hwaddr_data = hwAddr.sa_data;

		// enough space for a mac address (6 2-char bytes, 5 separating colons and a null char)
		ostringstream oss;

		oss << setfill('0') << hex << uppercase;
		oss << setw(2) << (hwaddr_data[0] & 0xFF);
		for (int i=1; i<6; ++i)
			oss << '-' << setw(2) << ((int)hwaddr_data[i] & 0xFF);

		return oss.str();
	}
}
