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
#ifndef INETLISTENINGSERVER510PROBE_H
#define INETLISTENINGSERVER510PROBE_H

#include "AbsProbe.h"
#include <Item.h>
#include <Object.h>
#include <Log.h>

#define MAXLINE 1000 /* maximum input line size */

/**
	Store a single record from a call to lsof it its nicely parsed form
*/
class LsofRecord {

public: 
	LsofRecord(){};
	LsofRecord(std::string pr, std::string la, std::string lp, std::string lfa, std::string pn, std::string fa, std::string fp, std::string ffa, std::string p, std::string u) :
	  protocol(pr), local_address(la), local_port(lp), local_full_address(lfa), program_name(pn), foreign_address(fa), foreign_port(fp),
		  foreign_full_address(ffa), pid(p), user_id(u){};
	~LsofRecord(){};
	
	std::string protocol;
	std::string local_address;
	std::string local_port;
	std::string local_full_address;
	std::string program_name;
	std::string foreign_address;
	std::string foreign_port;
	std::string foreign_full_address;
	std::string pid;
	std::string user_id;
};

/**
	A vector for storing LsofRecord objects.
	All objects are stored by reference.
*/
typedef std::vector < LsofRecord* > LsofRecordVector;

/**
	This class is responsible for collecting variable information from the nvram command.
	This class should be platform independant.
*/
class InetListeningServer510Probe : public AbsProbe {
public:
	virtual ~InetListeningServer510Probe();
	
	/** Run the InetListeningServer510 probe */
	virtual ItemVector* CollectItems(Object* object);

	/** Ensure that the InetListeningServer510Probe is a singleton. */
	static AbsProbe* Instance();
	
private:

	static InetListeningServer510Probe *instance;

	InetListeningServer510Probe();

	ObjectEntity* ValidateStringOperations(ObjectEntity* stringOp);
	
	ObjectEntity* ValidateIntOperations(ObjectEntity* intOp);
	
	/** Return a new Item created for storing variable information */
	virtual Item* CreateItem();

	/** Return a new filled Item created for storing variable information */
	Item* FillItem(LsofRecord *record);
	
	void GetAllRecords();
	
	LsofRecord* ParseLine(const std::string &line);

	void DeleteRecords();
	
	LsofRecordVector* records;
};

#endif
