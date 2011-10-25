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

#include <cctype>
#include <cstring>
#include <fstream>
#include <list>
#include <map>
#include <memory>
#include <sstream>

#include <net/ethernet.h>
#include <net/if.h> // for if_indextoname()
#include <sys/param.h> // for PATH_MAX
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <VectorPtrGuard.h>
#include <DirGuard.h>
#include <NetworkInterfaces.h>
#include "IfListenersProbe.h"

using namespace std;

#define PROC_DIR "/proc"
#define PROC_PKT_FILE "/proc/net/packet"

namespace {

	/**
	 * A cache of packet listeners.
	 */
	list<Item> packetListeners;

	/**
	 * Maps ethernet protocol constants to string enumerator values as defined
	 * in the OVAL schema.
	 */
	map<unsigned, string> etherProtoMap;

	/**
	 * Populates etherProtoMap with values.
	 */
	void SetupEthernetProtoMap();

	Item CreateItem();

	/**
	 * Populates the packetListeners list with all iflistener items found on the
	 * system.
	 */
	void CachePacketListeners();

	/**
	 * Given the inode number of an open device/socket/etc, this finds the pid
	 * of the process that opened it.
	 */
	bool FindInodeOwner(ino_t inode, pid_t *pid);

	/**
	 * Gets the exe corresponding to the given pid.  This reads the symlink
	 * /proc/<pid>/exe.
	 */
	string GetExeForPid(pid_t pid);
}

//****************************************************************************************//
//								IfListeners Class							  //
//****************************************************************************************//
IfListenersProbe *IfListenersProbe::instance = NULL;

IfListenersProbe::IfListenersProbe() {
	SetupEthernetProtoMap();
	CachePacketListeners();
}

IfListenersProbe::~IfListenersProbe() {
	packetListeners.clear();
	instance = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* IfListenersProbe::Instance() {
	// Use lazy initialization
	if(instance == NULL)
		instance = new IfListenersProbe();

	return instance;
}

ItemVector* IfListenersProbe::CollectItems(Object* object) {

	VectorPtrGuard<Item> collectedItems(new ItemVector());

	for (list<Item>::iterator iter = packetListeners.begin();
		 iter != packetListeners.end();
		 ++iter) {
		if (object->Analyze(&*iter))
			collectedItems->push_back(new Item(*iter));
	}

	return collectedItems.release();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

/** This is never called */
Item* IfListenersProbe::CreateItem() {
	return NULL;
}

namespace {

	void SetupEthernetProtoMap() {
		etherProtoMap[ETH_P_LOOP] = "ETH_P_LOOP";
		etherProtoMap[ETH_P_PUP] = "ETH_P_PUP";
		etherProtoMap[ETH_P_PUPAT] = "ETH_P_PUPAT";
		etherProtoMap[ETH_P_IP] = "ETH_P_IP";
		etherProtoMap[ETH_P_X25] = "ETH_P_X25";
		etherProtoMap[ETH_P_ARP] = "ETH_P_ARP";
		etherProtoMap[ETH_P_BPQ] = "ETH_P_BPQ";
		etherProtoMap[ETH_P_IEEEPUP] = "ETH_P_IEEEPUP";
		etherProtoMap[ETH_P_IEEEPUPAT] = "ETH_P_IEEEPUPAT";
		etherProtoMap[ETH_P_DEC] = "ETH_P_DEC";
		etherProtoMap[ETH_P_DNA_DL] = "ETH_P_DNA_DL";
		etherProtoMap[ETH_P_DNA_RC] = "ETH_P_DNA_RC";
		etherProtoMap[ETH_P_DNA_RT] = "ETH_P_DNA_RT";
		etherProtoMap[ETH_P_LAT] = "ETH_P_LAT";
		etherProtoMap[ETH_P_DIAG] = "ETH_P_DIAG";
		etherProtoMap[ETH_P_CUST] = "ETH_P_CUST";
		etherProtoMap[ETH_P_SCA] = "ETH_P_SCA";
		etherProtoMap[ETH_P_RARP] = "ETH_P_RARP";
		etherProtoMap[ETH_P_ATALK] = "ETH_P_ATALK";
		etherProtoMap[ETH_P_AARP] = "ETH_P_AARP";
		etherProtoMap[ETH_P_8021Q] = "ETH_P_8021Q";
		etherProtoMap[ETH_P_IPX] = "ETH_P_IPX";
		etherProtoMap[ETH_P_IPV6] = "ETH_P_IPV6";
		etherProtoMap[ETH_P_SLOW] = "ETH_P_SLOW";
		etherProtoMap[ETH_P_WCCP] = "ETH_P_WCCP";
		etherProtoMap[ETH_P_PPP_DISC] = "ETH_P_PPP_DISC";
		etherProtoMap[ETH_P_PPP_SES] = "ETH_P_PPP_SES";
		etherProtoMap[ETH_P_MPLS_UC] = "ETH_P_MPLS_UC";
		etherProtoMap[ETH_P_ATMMPOA] = "ETH_P_ATMMPOA";
		etherProtoMap[ETH_P_ATMFATE] = "ETH_P_ATMFATE";
		etherProtoMap[ETH_P_AOE] = "ETH_P_AOE";
		etherProtoMap[ETH_P_TIPC] = "ETH_P_TIPC";
		etherProtoMap[ETH_P_802_3] = "ETH_P_802_3";
		etherProtoMap[ETH_P_AX25] = "ETH_P_AX25";
		etherProtoMap[ETH_P_ALL] = "ETH_P_ALL";
		etherProtoMap[ETH_P_802_2] = "ETH_P_802_2";
		etherProtoMap[ETH_P_SNAP] = "ETH_P_SNAP";
		etherProtoMap[ETH_P_DDCMP] = "ETH_P_DDCMP";
		etherProtoMap[ETH_P_WAN_PPP] = "ETH_P_WAN_PPP";
		etherProtoMap[ETH_P_PPP_MP] = "ETH_P_PPP_MP";
		etherProtoMap[ETH_P_PPPTALK] = "ETH_P_PPPTALK";
		etherProtoMap[ETH_P_LOCALTALK] = "ETH_P_LOCALTALK";
		etherProtoMap[ETH_P_TR_802_2] = "ETH_P_TR_802_2";
		etherProtoMap[ETH_P_MOBITEX] = "ETH_P_MOBITEX";
		etherProtoMap[ETH_P_CONTROL] = "ETH_P_CONTROL";
		etherProtoMap[ETH_P_IRDA] = "ETH_P_IRDA";
		etherProtoMap[ETH_P_ECONET] = "ETH_P_ECONET";
		etherProtoMap[ETH_P_HDLC] = "ETH_P_HDLC";
		etherProtoMap[ETH_P_ARCNET] = "ETH_P_ARCNET";
	}

	Item CreateItem() {
		return Item(0,
					"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#linux",
					"linux-sc",
					"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#linux linux-system-characteristics-schema.xsd",
					OvalEnum::STATUS_ERROR,
					"iflisteners_item");

	}

	void CachePacketListeners() {
		ifstream pktFile(PROC_PKT_FILE);
		string line;

		if (!pktFile)
			throw ProbeException("Couldn't open " PROC_PKT_FILE "!");

		// skip the header line
		getline(pktFile, line);

		while (getline(pktFile, line)) {
			istringstream iss(line);

			string ignore, exePath;
			unsigned proto, ifIdx;
			uid_t uid;
			ino_t inode;
			pid_t pid;
			string protoName;

			iss >> ignore >> ignore >> ignore >> 
				hex >> proto >> dec >> // the protocol # is hexadecimal.
				ifIdx >> ignore >> ignore >> uid >> inode;
/*
  Log::Debug("Found packet listener, proto=" + Common::ToString(proto) + ", idx=" +
  Common::ToString(ifIdx) + ", uid=" + Common::ToString(uid)
  + ", inode=" + Common::ToString(inode));
*/

			if (!iss)
				throw ProbeException("Unexpected line format in packet listener file: " + line);

			char ifName[IF_NAMESIZE];
			if (!if_indextoname(ifIdx, ifName))
				throw ProbeException("Couldn't resolve interface number " +
									 Common::ToString(ifIdx) + " to a name: " +
									 strerror(errno));

			if (!FindInodeOwner(inode, &pid))
				throw ProbeException("Could not find owner of packet listener with inode #" +
									 Common::ToString(inode));

			exePath = GetExeForPid(pid);

			map<unsigned,string>::iterator protoPos;
			if ((protoPos = etherProtoMap.find(proto))
				!= etherProtoMap.end())
				protoName = protoPos->second;

			Item item(CreateItem());
			item.SetStatus(OvalEnum::STATUS_EXISTS);
			item.AppendElement(new ItemEntity("interface_name", ifName, 
											  OvalEnum::DATATYPE_STRING, true));

			if (protoName.empty()) {
				item.AppendElement(new ItemEntity("protocol", "", 
												  OvalEnum::DATATYPE_STRING, 
												  false, OvalEnum::STATUS_ERROR));
				item.AppendMessage(new OvalMessage("Unrecognized protocol value: " +
												   Common::ToString(proto),
												   OvalEnum::LEVEL_ERROR));
			} else
				item.AppendElement(new ItemEntity("protocol", protoName));

			try {
				using namespace NetworkInterfaces;
				Interface intf = GetInterface(ifName);
				// assume all interfaces are ethernet for now.
				item.AppendElement(new ItemEntity("hw_address", 
												  intf.GetHwAddr()));
			} catch(Exception &e) {
				item.AppendElement(new ItemEntity("hw_address", "", 
												  OvalEnum::DATATYPE_STRING, 
												  false, OvalEnum::STATUS_ERROR));
				item.AppendMessage(new OvalMessage("Error getting hardware address: "+
												   e.GetErrorMessage(), 
												   OvalEnum::LEVEL_ERROR));
			}

			item.AppendElement(new ItemEntity("program_name", exePath));
			item.AppendElement(new ItemEntity("pid", Common::ToString(pid), OvalEnum::DATATYPE_INTEGER));
			item.AppendElement(new ItemEntity("user_id", Common::ToString(uid), OvalEnum::DATATYPE_INTEGER));

			packetListeners.push_back(item);
		}
	}

	bool FindInodeOwner(ino_t inode, pid_t *pid) {
		DirGuard procDir(PROC_DIR);
		dirent *entry;
		bool found = false;

		// I am not throwing on most errors here; the real error will be if an
		// owner can't be located, which the caller can check.  I keep flipping
		// errno back to zero after each error, since I recall reading somewhere
		// that you can't trust functions to reset it if there is no error.

		errno = 0;
		while(!found && (entry = readdir(procDir)) != NULL) {
			if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
				continue;

			// find the names made up of all digits
			size_t i;
			for (i=0; i<strlen(entry->d_name) && isdigit(entry->d_name[i]); ++i)
				;

			if (i < strlen(entry->d_name))
				continue;

			string fdDirPath = Common::BuildFilePath(
				Common::BuildFilePath(PROC_DIR, entry->d_name),
				"fd");

			// inside the /proc/<pid>/fd directory, look at the inodes of all
			// opened files and see if any match the given inode.
			DirGuard fdDir(fdDirPath);
			dirent *fdEntry;
			errno = 0;
			while (!found && (fdEntry = readdir(fdDir)) != NULL) {
				if (!strcmp(fdEntry->d_name, ".") || !strcmp(fdEntry->d_name, ".."))
					continue;

				string fdPath = Common::BuildFilePath(fdDirPath, fdEntry->d_name);
				struct stat s;
				if (stat(fdPath.c_str(), &s)) {
					Log::Info("Error stat'ing " + fdPath + ": " + strerror(errno));
					errno = 0;
					continue;
				}

				if (s.st_ino == inode) {
					if (Common::FromString(entry->d_name, pid))
						found = true;
					else
						// I think I have to throw here... cause I found a
						// file with matching inode, so it wouldn't make any
						// sense to just log an error and keep searching.
						// Or I suppose I could log and then immediately return
						// false...
						throw ProbeException(string("Could not convert to pid_t: ") + entry->d_name);
				}
			}

			if (errno) {
				Log::Info("Error while reading " + fdDirPath + ": " + strerror(errno));
				errno = 0;
			}
		}

		if (errno)
			Log::Info(string("Error while reading " PROC_DIR ": ") + strerror(errno));

		return found;
	}

	string GetExeForPid(pid_t pid) {
		string procExeFile = Common::BuildFilePath(
			Common::BuildFilePath(PROC_DIR, Common::ToString(pid)),
			"exe");

		char buf[PATH_MAX];
		// leave room for a null char...
		ssize_t sz = readlink(procExeFile.c_str(), buf, sizeof(buf)-1);
		if (sz < 0)
			throw ProbeException("readlink("+procExeFile+") error: " + strerror(errno));
		buf[sz] = '\0';
		return buf;
	}
}
