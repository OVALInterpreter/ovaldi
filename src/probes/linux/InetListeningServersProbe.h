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

#ifndef _INETLISTENINGSERVERPROBE_H_
#define _INETLISTENINGSERVERPROBE_H_

#include "AbsProbe.h"

// Include the data class
#include <unistd.h>
#include <sys/types.h>
#include <sys/termios.h>
#include <sys/wait.h>
#include <iostream>
#include <string>

/// 110 characters + 20 characters for the program name, it appears. 
#define NETSTAT_BUFLEN 200

/**
	Store the results of a call to netstat.
*/
class NetstatResult {
 public:
	NetstatResult(std::string e, std::string t) : errText(e), text(t){};
	std::string errText;
	std::string text;
};

/**
	Stor a single record from a call to netstat it its nicely parsed form
*/
class NetstatRecord {

public: 
	NetstatRecord(){};
	NetstatRecord(std::string pr, std::string la, std::string lp, std::string lfa, std::string pn, std::string fa, std::string fp, std::string ffa, int p, std::string u) :
	  protocol(pr), local_address(la), local_port(lp), local_full_address(lfa), program_name(pn), foreign_address(fa), foreign_port(fp),
		  foreign_full_address(ffa), pid(p), user_id(u){};
	~NetstatRecord(){};
	
	std::string protocol;
	std::string local_address;
	std::string local_port;
	std::string local_full_address;
	std::string program_name;
	std::string foreign_address;
	std::string foreign_port;
	std::string foreign_full_address;
	int pid;
	std::string user_id;
};

/**
	A vector for storing NetstatRecord objects.
	All objects are stored by reference.
*/
typedef std::vector < NetstatRecord* > NetstatRecordVector;

/**
	Red Hat data collector for inetlisteningservers_test.
*/
class InetListeningServersProbe : public AbsProbe {
public:
	virtual ~InetListeningServersProbe();       

	virtual ItemVector* CollectItems(Object* object);

	static AbsProbe* Instance();

private:
	InetListeningServersProbe();
    
	virtual Item* CreateItem();

	/**
		Get the set of all protocols on the system that match the object
	*/
	StringVector* GetProtocols(ObjectEntity* protocol);
	/**
		Get the set of all local addresses on the system that match the object
	*/
	StringVector* GetLocalAddresses(std::string protocolStr, ObjectEntity* localAddress);
	/**
		Get the set of all local ports on the system that match the object
	*/
	StringVector* GetLocalPorts(std::string protocolStr, std::string localAddressStr, ObjectEntity* localPort);

	/**
		Collect all information that matches the specified program name.
		@param protocol a string the protocol
		@param localAddress a string the local address
		@param localPort a string the local port 
		@param itesm a ItemVector* a vector of items that matched the specified protocol, port, and address
	*/
	Item* GetNetstat(std::string protocol, std::string localAddress, std::string localPort);

	/**
		Check that the specified protocol exists on the system.
		@param protocol a string representing the program name to look for.
		@result The result of searching for the specified program name.
	*/
	bool ProtocolExists(std::string protocol);

	/**
		Check that the specified local address exists on the system.
	*/
	bool LocalAddressExists(std::string protocolStr, std::string localAddress);

	/**
		Check that the specified local port exists on the system.
	*/
	bool LocalPortExists(std::string protocolStr, std::string localAddressStr, std::string localPort);

	/**
		Get all protocols on the system that match the specified pattern.
		@param pattern a string used that protocols are compared against.
		@param isRegex a bool that is indicates how system protocols should be compared against the specified pattern
		@return The set of matching protocols.
	*/
	StringVector* GetMatchingProtocols(std::string pattern, bool isRegex);

	/**
		Get all local addresses on the system that match the specified pattern.
		@return The set of matching local addresses.
	*/
	StringVector* GetMatchingLocalAddresses(std::string protocolStr, std::string pattern, bool isRegex);

	/**
		Get all local ports on the system that match the specified pattern.
		@return The set of matching local ports.
	*/
	StringVector* GetMatchingLocalPorts(std::string protocolStr, std::string localAddressStr, std::string pattern, bool isRegex);

	/**
		Start a child processand make a call to netstat.
		Store all netstat results in a NetStat result object. 
	*/
	void ExecNetstat();

	/**
		Child process function to execute netstat.
	*/
	void ChildExecNetstat(int, int);

	/**
		Parent process function to to get the results fromt he child process.
	*/
	NetstatResult* ParentGetChildResult(int, int, int);//, char*, char*);

	/** 
		parse the netstat results in a vector of netstar records
	*/
	void ParseNetstatResult();

	static InetListeningServersProbe *instance;

	NetstatResult* netstatResult;
	NetstatRecordVector nrv;
};

#endif
