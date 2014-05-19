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

#include "InetListeningServer510Probe.h"
#include <VectorPtrGuard.h>
#include "CommandReader.h"

using namespace std;

//****************************************************************************************//
//								InetListeningServer510Probe Class						  //
//****************************************************************************************//
InetListeningServer510Probe *InetListeningServer510Probe::instance = NULL;

InetListeningServer510Probe::InetListeningServer510Probe() {
	records = NULL;
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
	long long port1, port2;

	ObjectEntity* protocol = this->ValidateStringOperations(object->GetElementByName("protocol"));
	ObjectEntity* localAddress = this->ValidateStringOperations(object->GetElementByName("local_address"));
	ObjectEntity* localPort = this->ValidateIntOperations(object->GetElementByName("local_port"));

    if ( records == NULL ) {
        InetListeningServer510Probe::GetAllRecords();
    }

	VectorPtrGuard<Item> collectedItems(new ItemVector());

	LsofRecordVector::iterator it;
	for(it = records->begin(); it != records->end(); it++) {
		LsofRecord* record = *it;
		Item* item = this->FillItem(record);
		
		if (item != NULL) {
			Common::FromString(record->local_port, &port1);
			Common::FromString(localPort->GetValue(), &port2);
			
			auto_ptr<ItemEntity> protocolIe(new ItemEntity("protocol", record->protocol));
			protocolCheck = (protocol->Analyze(protocolIe.get()) == OvalEnum::RESULT_TRUE);

			auto_ptr<ItemEntity> localAddressIe(new ItemEntity("local_address", record->local_address));
			localAddressCheck = (localAddress->Analyze(localAddressIe.get()) == OvalEnum::RESULT_TRUE);

			auto_ptr<ItemEntity> localPortIe(new ItemEntity("local_port", record->local_port, OvalEnum::DATATYPE_INTEGER));
			ItemEntity* tmp = this->CreateItemEntity(localPort);
			tmp->SetValue(record->local_port);
			if (record->local_port == "") {
				tmp->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
			}
			localPortCheck = (localPort->Analyze(tmp) == OvalEnum::RESULT_TRUE);
			if (protocolCheck && localAddressCheck && localPortCheck) {
				collectedItems->push_back(item);
			} else {
				delete item;
			}
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

Item* InetListeningServer510Probe::FillItem(LsofRecord *record) {

	Item* item = this->CreateItem();
	item->SetStatus(OvalEnum::STATUS_EXISTS);
	item->AppendElement(new ItemEntity("protocol", record->protocol, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
	item->AppendElement(new ItemEntity("local_address", record->local_address, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
	if (record->local_port == "") {
		item->AppendElement(new ItemEntity("local_port", record->local_port, OvalEnum::DATATYPE_INTEGER, OvalEnum::STATUS_DOES_NOT_EXIST));
	} else {
		item->AppendElement(new ItemEntity("local_port", record->local_port, OvalEnum::DATATYPE_INTEGER, OvalEnum::STATUS_EXISTS));
	}
	item->AppendElement(new ItemEntity("local_full_address", record->local_full_address, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
	item->AppendElement(new ItemEntity("program_name", record->program_name, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
	item->AppendElement(new ItemEntity("foreign_address", record->foreign_address, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
	item->AppendElement(new ItemEntity("foreign_port", record->foreign_port, OvalEnum::DATATYPE_INTEGER, OvalEnum::STATUS_EXISTS));
	item->AppendElement(new ItemEntity("foreign_full_address", record->foreign_full_address, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
	item->AppendElement(new ItemEntity("pid", record->pid, OvalEnum::DATATYPE_INTEGER, OvalEnum::STATUS_EXISTS));
	item->AppendElement(new ItemEntity("user_id", record->user_id, OvalEnum::DATATYPE_INTEGER, OvalEnum::STATUS_EXISTS));
	return item;
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
	StringVector lines = CommandReader::GetResponse("/usr/sbin/lsof -i -P -n -l");
	REGEX regex;

	records = new LsofRecordVector();
	for (StringVector::iterator it = lines.begin(); it != lines.end(); it++) {
		string line = *it;
		if (regex.IsMatch("COMMAND", line.c_str()) == false) {
			records->push_back(this->ParseLine(line));
		}
	}
}

LsofRecord* InetListeningServer510Probe::ParseLine(const std::string &line) {
	std::string name, pr, la, lp, lfa, pn, fa, fp, ffa, p, u, tmp1, tmp2, tmp3, tmp4;
	std::size_t found, fport;

	StringVector* elems = CommandReader::Split(line, ' ');
	StringVector::iterator it = elems->begin();
	pn = *it++;
	p = *it++;
	u = *it++;
	tmp1 = *it++;
	tmp2 = *it++;
	tmp3 = *it++;
	tmp4 = *it++;
	pr = *it++;
	name = *it;
	
	delete elems;
	
    found = name.find("->");
	if (found!=std::string::npos) {
		lfa = name.substr(0, found);
		ffa = name.substr(found+2);
		fport = ffa.find_last_of(":");
		if (fport!=std::string::npos) {
			fa = ffa.substr(0, fport);
			fp = ffa.substr(fport+1);
			if (fp.compare("*") == 0) {
				fp = "";
			}
		} else {
			fa = ffa;
			fp = "";
		}
	} else {
		lfa = name;
	}
	fport = lfa.find_last_of(":");
	if (fport!=std::string::npos) {
		la = lfa.substr(0, fport);
		lp = lfa.substr(fport+1);
		if (lp.compare("*") == 0) {
			lp = "";
		}
	} else {
		la = lfa;
		lp = "";
	}
	LsofRecord* record = new LsofRecord(pr, la, lp, lfa, pn, fa, fp, ffa, p, u);
	return record;
}

void InetListeningServer510Probe::DeleteRecords() {
	if (records == NULL) return;

	for(LsofRecordVector::iterator it = records->begin(); it != records->end(); it++)
		delete *it;

	delete records;
}