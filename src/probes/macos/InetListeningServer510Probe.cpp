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

#include <VectorPtrGuard.h>
#include "CommandReader.h"

#include "InetListeningServer510Probe.h"

using namespace std;

namespace {
	/**
	 * ipv6 addresses get pulled outta 'lsof' output with square brackets around
	 * them.  This function strips them off.
	 */
	void stripBrackets(string *addr);
}


//****************************************************************************************//
//								InetListeningServer510Probe Class						  //
//****************************************************************************************//
InetListeningServer510Probe *InetListeningServer510Probe::instance = NULL;

InetListeningServer510Probe::InetListeningServer510Probe() : records(NULL) {
}

InetListeningServer510Probe::~InetListeningServer510Probe() {
	instance = NULL;
	InetListeningServer510Probe::DeleteRecords();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* InetListeningServer510Probe::Instance() {

	// Use lazy initialization
	if(instance == NULL)
		instance = new InetListeningServer510Probe();

	return instance;
}

ItemVector* InetListeningServer510Probe::CollectItems(Object *object) {
	bool protocolCheck, localAddressCheck, localPortCheck = false;

	ObjectEntity* protocol = this->ValidateStringOperations(object->GetElementByName("protocol"));
	ObjectEntity* localAddress = this->ValidateStringOperations(object->GetElementByName("local_address"));
	ObjectEntity* localPort = this->ValidateIntOperations(object->GetElementByName("local_port"));

    if ( records == NULL ) {
        InetListeningServer510Probe::GetAllRecords();
    }

	VectorPtrGuard<Item> collectedItems(new ItemVector());

	LsofRecordVector::iterator it;
	ItemEntity protocolIe("protocol");
	ItemEntity localAddressIe("local_address");
	ItemEntity localPortIe("local_port", "", OvalEnum::DATATYPE_INTEGER);
	for(it = records->begin(); it != records->end(); it++) {
		
		protocolIe.SetValue(it->protocol);
		protocolCheck = (protocol->Analyze(&protocolIe) == OvalEnum::RESULT_TRUE);

		localAddressIe.SetValue(it->local_address);
		localAddressCheck = (localAddress->Analyze(&localAddressIe) == OvalEnum::RESULT_TRUE);

		localPortIe.SetValue(it->local_port);
		localPortIe.SetStatus(it->local_port.empty() ?
							  OvalEnum::STATUS_DOES_NOT_EXIST : OvalEnum::STATUS_EXISTS);
		localPortCheck = (localPort->Analyze(&localPortIe) == OvalEnum::RESULT_TRUE);

		if (protocolCheck && localAddressCheck && localPortCheck) {
			Item* item = this->FillItem(*it);
			collectedItems->push_back(item);
		}
	}
	return collectedItems.release();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
Item* InetListeningServer510Probe::CreateItem() {

	Item* item = new Item(0, 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#macos", 
						"macos-sc", 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#macos macos-system-characteristics-schema.xsd", 
						OvalEnum::STATUS_ERROR, 
						"inetlisteningserver510_item");

	return item;
}

Item* InetListeningServer510Probe::FillItem(const LsofRecord &record) {

#define ADD_ENTITY_TYPE(name_, type_) \
	item->AppendElement(new ItemEntity(#name_, record.name_, OvalEnum::DATATYPE_ ## type_))

// a bit briefer for most of the entities, which are string-typed.
#define ADD_ENTITY(name_) ADD_ENTITY_TYPE(name_, STRING)

	Item* item = this->CreateItem();
	item->SetStatus(OvalEnum::STATUS_EXISTS);

	ADD_ENTITY(protocol);
	ADD_ENTITY(local_address);
	
	if (record.local_port.empty()) {
		item->AppendElement(new ItemEntity("local_port", record.local_port, OvalEnum::DATATYPE_INTEGER, OvalEnum::STATUS_DOES_NOT_EXIST));
	} else {
		item->AppendElement(new ItemEntity("local_port", record.local_port, OvalEnum::DATATYPE_INTEGER, OvalEnum::STATUS_EXISTS));
	}

	ADD_ENTITY(local_full_address);
	ADD_ENTITY(program_name);
	ADD_ENTITY(foreign_address);
	ADD_ENTITY_TYPE(foreign_port, INTEGER);
	ADD_ENTITY(foreign_full_address);
	ADD_ENTITY_TYPE(pid, INTEGER);
	ADD_ENTITY_TYPE(user_id, INTEGER);

	return item;

#undef ADD_ENTITY
#undef ADD_ENTITY_TYPE
}

ObjectEntity* InetListeningServer510Probe::ValidateStringOperations(ObjectEntity* stringOp) {
	if (stringOp != NULL) {
		// check operation - only allow  equals, not equals, case-insensitive and pattern match
		if(stringOp->GetOperation() != OvalEnum::OPERATION_EQUALS && stringOp->GetOperation() != OvalEnum::OPERATION_PATTERN_MATCH 
			&& stringOp->GetOperation() != OvalEnum::OPERATION_NOT_EQUAL && stringOp->GetOperation() != OvalEnum::OPERATION_CASE_INSENSITIVE_EQUALS
			&& stringOp->GetOperation() != OvalEnum::OPERATION_CASE_INSENSITIVE_NOT_EQUAL) {
			throw ProbeException("Error: invalid operation specified on name. Found: " + OvalEnum::OperationToString(stringOp->GetOperation()));
		}

		// check datatypes - only allow string
		if(stringOp->GetDatatype() != OvalEnum::DATATYPE_STRING) {
			throw ProbeException("Error: invalid data type specified. Found: " + OvalEnum::DatatypeToString(stringOp->GetDatatype()));
		}
	}
	return stringOp;
}

ObjectEntity* InetListeningServer510Probe::ValidateIntOperations(ObjectEntity* intOp) {
	if (intOp != NULL) {
	    // check operation - only allow  equals, not equal, greater than, greater than or equal, less than, less than or equal, bitwise and, and bitwise or
	    if ( intOp->GetOperation() != OvalEnum::OPERATION_EQUALS &&
	            intOp->GetOperation() != OvalEnum::OPERATION_NOT_EQUAL &&
	            intOp->GetOperation() != OvalEnum::OPERATION_GREATER_THAN &&
	            intOp->GetOperation() != OvalEnum::OPERATION_GREATER_THAN_OR_EQUAL &&
	            intOp->GetOperation() != OvalEnum::OPERATION_LESS_THAN &&
	            intOp->GetOperation() != OvalEnum::OPERATION_LESS_THAN_OR_EQUAL &&
	            intOp->GetOperation() != OvalEnum::OPERATION_BITWISE_AND &&
	            intOp->GetOperation() != OvalEnum::OPERATION_BITWISE_OR ) {
	        throw ProbeException ( "Error: invalid operation specified. Found: " + OvalEnum::OperationToString ( intOp->GetOperation() ) );
	    }

	    // check datatypes - only allow the integer datatype
	    if ( intOp->GetDatatype() != OvalEnum::DATATYPE_INTEGER ) {
	        throw ProbeException ( "Error: invalid data type specified. Found: " + OvalEnum::DatatypeToString ( intOp->GetDatatype() ) );
	    }
	}
	return intOp;
}

void InetListeningServer510Probe::GetAllRecords() {
/*
// for testing
	StringVector lines;
	lines.push_back("airportd     80        0   22u  IPv4 0xac2b914e6c494e79    0t0    UDP *:*");
*/

	StringVector lines = CommandReader::GetResponse("/usr/sbin/lsof -i -P -n -l");

	DeleteRecords(); // just in case
	records = new LsofRecordVector();
	for (StringVector::iterator it = lines.begin(); it != lines.end(); it++)
		this->ParseLine(*it);
}

void InetListeningServer510Probe::ParseLine(const string &line) {
	string pr, la, lp, lfa, pn, fa, fp="0", ffa, p, u;
	size_t arrow, colon;

	// skip the header line
	if (line.find("COMMAND") != string::npos)
		return;
	
	StringVector elems = CommandReader::Split(line, ' ');

	// unexpected format... should print these lines
	// out, I think, so I know if it happens.
	if (elems.size() < 9) {
		Log::Debug("Unexpected format in 'lsof' output line:\n\t\"" + line + '\"');
		return;
	}

	const string &name = elems[8];
    arrow = name.find("->");
	if (arrow!=string::npos) {
		lfa = name.substr(0, arrow);
		ffa = name.substr(arrow+2);
		colon = ffa.find_last_of(':');
		if (colon!=string::npos) {
			fa = ffa.substr(0, colon);
			fp = ffa.substr(colon+1);
			if (fp == "*") {
				fp = "0";
			}
		} else {
			fa = ffa;
		}
	} else {
		lfa = name;
	}
	colon = lfa.find_last_of(':');
	if (colon!=string::npos) {
		la = lfa.substr(0, colon);
		lp = lfa.substr(colon+1);
		if (lp == "*") {
			lp.clear(); // ok, other code handles this
		}
	} else {
		la = lfa;
	}

	// ipv6 addresses get surrounded in brackets;
	// strip those off.  E.g. "[::1]"
	stripBrackets(&la);
	stripBrackets(&fa);

	records->push_back(LsofRecord(
		elems[7],
		la,
		lp,
		lfa, 
		elems[0],
		fa,
		fp,
		ffa,
		elems[1],
		elems[2]));
}

void InetListeningServer510Probe::DeleteRecords() {
	if (records) {
		delete records;
		records = NULL;
	}
}

namespace {
	void stripBrackets(string *addr) {
		// assert(addr!=NULL)
		if (!addr->empty()) {
			if ((*addr)[0] == '[')
				addr->erase(0, 1);
			if (!addr->empty() && (*addr)[addr->size()-1] == ']')
				addr->erase(addr->size()-1);
		}
	}
}
