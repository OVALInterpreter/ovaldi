//
//
//****************************************************************************************//
// Copyright (c) 2002-2013, The MITRE Corporation
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

#ifndef NETWORKINTERFACES_H
#define NETWORKINTERFACES_H

#include <netinet/in.h>
#include <list>
#include <string>

/**
 * A platform-independent interface for getting information about network
 * interfaces.  The implementation of this interface is platform-specific.
 */
namespace NetworkInterfaces {

	/**
	 * Represents a network interface.  For now, all the different types of
	 * info for different types of interfaces are lumped into one class.
	 */
	class Interface {
	public:

		/**
		 * Contains the link types described in the unix schemas...
		 * and I added TOKENRING from the windows schema for a good
		 * measure.
		 */
		enum LinkType {
			UNKNOWN,
			LOOPBACK,
			ETHERNET,
			PPP,
			SLIP,
			TOKENRING,
			FDDI,
			PRONET
		};

		Interface() {
		}

		Interface(const std::string &name, LinkType type) :
			name(name),
			type(type) {
		}

		const std::string &GetName() const {
			return name;
		}

		LinkType GetType() const {
			return type;
		}

		/**
		 * These flags should be interpreted relative to
		 * platform-specific flag constants.
		 */
		short GetFlags() const {
			return flags;
		}

		void SetFlags(short flags) {
			this->flags = flags;
		}

		/**
		 * Returns the interface's MAC address.  Only applicable to
		 * ethernet interfaces.
		 */
		const std::string &GetHwAddr() const {
			return hwAddr;
		}

		void SetHwAddr(const std::string &hwAddr) {
			this->hwAddr = hwAddr;
		}

		/**
		 * \name IPv4
		 * These are only applicable to IPv4 interfaces.
		 */
		///\{

		in_addr GetIPAddr() const {
			return ipAddr;
		}

		void SetIPAddr(in_addr ipAddr) {
			this->ipAddr = ipAddr;
		}

		in_addr GetBroadAddr() const {
			return broadAddr;
		}

		void SetBroadAddr(in_addr broadAddr) {
			this->broadAddr = broadAddr;
		}

		in_addr GetNetmask() const {
			return netmask;
		}

		void SetNetmask(in_addr netmask) {
			this->netmask = netmask;
		}

		///\}

	private:
		std::string name;
		LinkType type;
		short flags;

		std::string hwAddr;
		in_addr ipAddr;
		in_addr broadAddr;
		in_addr netmask;

		// I don't think we support getting ipv6 interface info yet...?
/*
		in6_addr ipAddr6;
		in6_addr broadAddr6;
		in6_addr netmask6;
*/
	};

	/**
	 * Get an interface by name.
	 */
	Interface GetInterface(const std::string &name);

	/**
	 * Get all interfaces.
	 */
	std::list<Interface> GetAllInterfaces();
}

#endif
