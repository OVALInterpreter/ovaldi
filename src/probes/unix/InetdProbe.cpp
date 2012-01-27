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
#include "InetdProbe.h"

#include <cctype>
#include <Common.h>
#include <fstream>
#include <OvalEnum.h>

using namespace std;

static const string INETD_CONF_FILENAME = "/etc/inetd.conf";

InetdProbe* InetdProbe::instance = NULL;

InetdProbe::InetdProbe() {
	allowedEndpointTypes.insert("stream");
	allowedEndpointTypes.insert("dgram");
	allowedEndpointTypes.insert("raw");
	allowedEndpointTypes.insert("seqpacket");
	allowedEndpointTypes.insert("tli");

	allowedWaitValues.insert("wait");
	allowedWaitValues.insert("nowait");
}

InetdProbe::~InetdProbe() {
	InetdProbe::instance = NULL;
}

InetdProbe* InetdProbe::Instance() {
	if (InetdProbe::instance == NULL)
		InetdProbe::instance = new InetdProbe();

	return InetdProbe::instance;
}

Item* InetdProbe::CreateItem() {
	Item* item = new Item(0,
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#unix",
						"unix-sc",
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#unix unix-system-characteristics-schema.xsd",
						OvalEnum::STATUS_ERROR,
						"inetd_item");

	return item;
}

ItemVector* InetdProbe::CollectItems(Object* object) {
	ObjectEntity* nameEntity = object->GetElementByName("service_name");
	ObjectEntity* protocolEntity = object->GetElementByName("protocol");

	ifstream in(INETD_CONF_FILENAME.c_str());
	if (!in)
		throw ProbeException(string("Couldn't open file: ")+INETD_CONF_FILENAME);

	ItemVector* items = new ItemVector();
	string line;

	//make one dummy call at the beginning to set up our read-ahead.
	//  (This will also cause any dangling continuation lines at the
	//   beginning of the file to be dropped.. which should be ok.)
	this->NextVirtualLine(in, line);

	while(in) {
		this->NextVirtualLine(in, line);
		Item* item = this->Line2Item(line, nameEntity, protocolEntity);

		if (item != NULL)
			items->push_back(item);
	}

	return items;
}

void InetdProbe::NextVirtualLine(istream& in, string& line) {
	line.clear();

	if (!this->tmpLine.empty())
		line = this->tmpLine;

	this->NextLine(in, tmpLine);
	while (!tmpLine.empty() && isspace(tmpLine[0])) {
		line += this->tmpLine;
		this->NextLine(in, this->tmpLine);
	}
}

void InetdProbe::NextLine(istream& in, string& line) {
	string tmp;

	do {
		getline(in, tmp);
		Common::TrimEnd(tmp);
	} while (in && (tmp.empty() || tmp[0] == '#'));

	if (!tmp.empty() && tmp[0] != '#')
		line = tmp;
	else
		line.clear();
}

Item* InetdProbe::Line2Item(const string& line, ObjectEntity *nameEntity, ObjectEntity* protocolEntity) {

	string serviceName, protocol, serverProg, serverArgs, socketType, user, wait;
	vector<StringVector> fieldMatches;

	re.GetAllMatchingSubstrings("\\S+", line, fieldMatches);
		
	// the conf file allows a 1-field entry which gives a default host name
	//   in the form 'hostname:'.  We can ignore this.  All lines we care
	//   about should have at least 6 fields.  Extras are treated as server
	//   arguments.
	if (fieldMatches.size() >= 6) {
			
		// first part of service field can be a hostname...
		const string& serviceField = fieldMatches[0][0];
		string::size_type idx = serviceField.find(':');
		if (idx == string::npos)
			serviceName = serviceField;
		else
			serviceName = serviceField.substr(idx+1);

		socketType = fieldMatches[1][0];
		protocol = fieldMatches[2][0];

		// last part of wait field can be a max thread count
		const string& waitField = fieldMatches[3][0];
		idx = waitField.find('.');
		if (idx == string::npos)
			wait = waitField;
		else
			wait = waitField.substr(0, idx);

		// last part of user field can be a group
		const string& userField = fieldMatches[4][0];
		idx = userField.find_first_of(":.");
		if (idx == string::npos)
			user = userField;
		else
			user = userField.substr(0, idx);

		serverProg = fieldMatches[5][0];

		// collect up server args, if any
		if (fieldMatches.size() > 6) {
			serverArgs = fieldMatches[6][0];
			for (vector<StringVector>::size_type i=7; i<fieldMatches.size(); ++i) {
				serverArgs += " ";
				serverArgs += fieldMatches[i][0];
			}
		}
	} else
		return NULL;

	ItemEntity* nameItemEntity = new ItemEntity("service_name", serviceName, OvalEnum::DATATYPE_STRING, true);
	if (nameEntity->Analyze(nameItemEntity) != OvalEnum::RESULT_TRUE) {
		delete nameItemEntity;
		return NULL;
	}

	ItemEntity* protItemEntity = new ItemEntity("protocol", protocol, OvalEnum::DATATYPE_STRING, true);
	if (protocolEntity->Analyze(protItemEntity) != OvalEnum::RESULT_TRUE) {
		delete nameItemEntity;
		delete protItemEntity;
		return NULL;
	}

	Item *item = this->CreateItem();
	item->AppendElement(protItemEntity);
	item->AppendElement(nameItemEntity);
	item->AppendElement(new ItemEntity("server_program", serverProg));
	if (!serverArgs.empty())
		item->AppendElement(new ItemEntity("server_arguments", serverArgs));

	ItemEntity *endPointTypeEntity;
	if (allowedEndpointTypes.find(socketType) == allowedEndpointTypes.end()) {
		endPointTypeEntity = new ItemEntity("endpoint_type", "", OvalEnum::DATATYPE_STRING, false, 
											OvalEnum::STATUS_NOT_COLLECTED);
		item->AppendMessage(new OvalMessage(string("endpoint_type '") + socketType +
											"' is not one of the enumerated legal values."));
	} else
		endPointTypeEntity = new ItemEntity("endpoint_type", socketType);

	item->AppendElement(endPointTypeEntity);
	item->AppendElement(new ItemEntity("exec_as_user", user));

	ItemEntity *waitEntity;
	if (allowedWaitValues.find(wait) == allowedWaitValues.end()) {
		waitEntity = new ItemEntity("wait_status", "", OvalEnum::DATATYPE_STRING, false, 
									OvalEnum::STATUS_NOT_COLLECTED);
		item->AppendMessage(new OvalMessage(string("wait_status '") + wait +
											"' is not one of the enumerated legal values."));
	} else
		waitEntity = new ItemEntity("wait_status", wait);

	item->AppendElement(waitEntity);
	item->SetStatus(OvalEnum::STATUS_EXISTS);

	return item;
}
