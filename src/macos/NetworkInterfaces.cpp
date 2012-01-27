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

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/sysctl.h>
#include <net/if_types.h>
#include <net/if_dl.h>

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
	 * It looks as if on macos, the SIOCGIFCONF ioctl gives both the interface
	 * names and address families?  So I'll change the GetInterfaceNames()
	 * function to return both.
	 */
	struct IfNameAndAf {
		IfNameAndAf(const string &name, int af) : name(name), af(af) {
		}

		string name;
		int af;
	};

	/**
	 * Finds the names of all network interfaces.
	 */
	list<IfNameAndAf> GetInterfaceNames();

	/**
	 * Maps from a byte value to one of the Interface::LinkType
	 * enumerators.
	 */
	NetworkInterfaces::Interface::LinkType GetLinkType(unsigned char type);

	/**
	 * Given a MAC address in a byte buffer, create the usual
	 * xx-xx-xx-xx-xx-xx formatted string from it.
	 */
	string FormatMACAddress(unsigned char *macptr);
}

namespace NetworkInterfaces {

	Interface GetInterface(const string &name) {

		/* This currently only works with interfaces with IPv4 addresses.  An
		 * exception will be thrown for other interfaces.
		 */

		struct ifreq ifr;
		short flags;
		struct if_msghdr *ifm = NULL;
		struct sockaddr_dl *sdl = NULL;
		Interface::LinkType linkType;
		char *macbuf;
		unsigned char *macptr = NULL;
		string macstr;
		int mac_length;
		int mib[6];
		struct sockaddr ipAddr;
		struct sockaddr broadAddr;
		struct sockaddr netmask;

		Log::Debug("Querying interface: "+name+"...");		

		SocketGuard socketfd(AF_INET, SOCK_DGRAM, 0);

		// error messages below will have this common prefix.
		string errorMsgPrefix = "Error querying network interface "+name+": ";

		// if I set the last char to '\0' and then always
		// copy no more than IFNAMSIZ-1 chars, I should be
		// guaranteed to have a null-terminated string.
		ifr.ifr_name[IFNAMSIZ-1] = '\0';

		strncpy(ifr.ifr_name, name.c_str(), IFNAMSIZ-1);
		if ( ioctl(socketfd,SIOCGIFFLAGS,&ifr) < 0 ){
			throw IOException(errorMsgPrefix + "ioctl SIOCGIFFLAGS: " + strerror(errno));
		}else{
			flags = ifr.ifr_flags;
		}

		mac_length = 0;

		mib[0] = CTL_NET;
		mib[1] = AF_ROUTE;
		mib[2] = 0;
		mib[3] = AF_LINK;
		mib[4] = NET_RT_IFLIST;
		mib[5] = if_nametoindex(name.c_str());

		if (mib[5] == 0){
			throw IOException(errorMsgPrefix + "if_nametoindex: " + strerror(errno));
		}

		if (sysctl(mib, 6, NULL, (size_t*)&mac_length, NULL, 0) < 0){
			throw IOException(errorMsgPrefix + "sysctl error retrieving data length: " +
							  strerror(errno));
		}

		macbuf = (char*) malloc(mac_length);

		if ( macbuf == NULL ){
			throw OutOfMemoryException("Error: couldn't allocate memory");
		}

		if (sysctl(mib, 6, macbuf, (size_t*)&mac_length, NULL, 0) < 0){
			throw IOException(errorMsgPrefix + "sysctl error retrieving data: " +
							  strerror(errno));
		}

		ifm = (struct if_msghdr *)macbuf;
		sdl = (struct sockaddr_dl *)(ifm + 1);

		linkType = GetLinkType(sdl->sdl_type);

		macptr = (unsigned char *)LLADDR(sdl);
		macstr = FormatMACAddress(macptr);

		if ( ioctl(socketfd, SIOCGIFADDR, &ifr) < 0) {
			free(macbuf);
			throw IOException(errorMsgPrefix + "ioctl SIOCGIFADDR: " + strerror(errno));
		} else {
			ipAddr = ifr.ifr_addr;
		}

		if (flags & IFF_BROADCAST) {			
			if ( ioctl(socketfd, SIOCGIFBRDADDR, &ifr) < 0) {
				free(macbuf);
				throw IOException(errorMsgPrefix + "ioctl SIOCGIFBRDADDR: " + strerror(errno));
			} else {
				broadAddr = ifr.ifr_broadaddr;
			}
		}
		
		if ( ioctl(socketfd, SIOCGIFNETMASK, &ifr) < 0) {
			free(macbuf);
			throw IOException(errorMsgPrefix + "ioctl SIOCGIFNETMASK: " + strerror(errno));
		} else {
			netmask = ifr.ifr_addr;
		}

		Interface intf(name, linkType);
		intf.SetFlags(flags);
		intf.SetHwAddr(macstr);
		intf.SetIPAddr(((struct sockaddr_in*)&ipAddr)->sin_addr);
		if (flags & IFF_BROADCAST)
			intf.SetBroadAddr(((struct sockaddr_in*)&broadAddr)->sin_addr);
		intf.SetNetmask(((struct sockaddr_in*)&netmask)->sin_addr);

		free(macbuf);

		return intf;
	}

	list<Interface> GetAllInterfaces() {

		/* This only returns IPv4 interfaces.  Those are the only interfaces
		 * that work with the GetInterface() method anyway.  Collection of info
		 * about other types of interface is not yet supported.
		 */

		list<IfNameAndAf> ifNames = GetInterfaceNames();
		list<Interface> interfaces;

		for (list<IfNameAndAf>::iterator nameIter = ifNames.begin();
			 nameIter != ifNames.end();
			 ++nameIter)
			if (nameIter->af == AF_INET)
				interfaces.push_back(GetInterface(nameIter->name));

		return interfaces;
	}
}

namespace {
	list<IfNameAndAf> GetInterfaceNames() {

		list<IfNameAndAf> names;
		struct ifconf ifc;
		struct ifreq *ifr = NULL;
		char *ptr;
		int length = 10 * sizeof(struct ifreq);
		int prev_length = 0;
		int addr_length;

		SocketGuard socketfd(AF_INET, SOCK_DGRAM, 0);

		while(true){
			ifc.ifc_buf= (char*)malloc(length);
			ifc.ifc_len = length;
			if ( ifc.ifc_buf != NULL ){
				if ( ioctl(socketfd, SIOCGIFCONF, &ifc) < 0 ){
					if ( ifc.ifc_buf != NULL ){
						free(ifc.ifc_buf);
						ifc.ifc_buf = NULL;
					}
					throw IOException("Error: SIOCGIFCONF ioctl error");
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
				throw OutOfMemoryException("Error: couldn't allocate memory");
			}
		}

		if (ioctl(socketfd, SIOCGIFCONF, &ifc) < 0)
			throw IOException("Error: SIOCGIFCONF ioctl error");

		for (ptr = ifc.ifc_buf; ptr <ifc.ifc_buf + ifc.ifc_len;) {
			ifr = (struct ifreq *) ptr;
			names.push_back(IfNameAndAf(ifr->ifr_name, ifr->ifr_addr.sa_family));
// 			Log::Debug(string("Got intf ")+ifr->ifr_name+", af="+
// 					   Common::ToString((int)ifr->ifr_addr.sa_family));
			

			addr_length = sizeof(struct sockaddr);

			if (ifr->ifr_addr.sa_len > addr_length){
				addr_length = ifr->ifr_addr.sa_len;
			}

			ptr += sizeof(ifr->ifr_name) + addr_length;
		}

		free(ifc.ifc_buf);
		return names;
	}

	NetworkInterfaces::Interface::LinkType GetLinkType(unsigned char type) {

		using namespace NetworkInterfaces;

		string hwTypeStr ="";

		switch (type) {
		case IFT_ETHER:
			return Interface::ETHERNET;
		case IFT_FDDI:
			return Interface::FDDI;
		case IFT_PPP:
			return Interface::PPP;
		case IFT_SLIP:
			return Interface::SLIP;
		case IFT_LOOP:
			return Interface::LOOPBACK;
		case IFT_P10:
		case IFT_P80:
			return Interface::PRONET;
		case IFT_OTHER:
			return Interface::UNKNOWN;
		default:
			Log::Info("Encountered an unrecognized interface type: " + Common::ToString((unsigned)type));
			return Interface::UNKNOWN;

		}
	}

	string FormatMACAddress(unsigned char *macptr) {
		ostringstream oss;

		oss << setfill('0') << hex << uppercase;
		oss << setw(2) << ((int)macptr[0] & 0xFF);
		for (int i=1; i<6; ++i)
			oss << '-' << setw(2) << ((int)macptr[i] & 0xFF);

		return oss.str();
	}
}
