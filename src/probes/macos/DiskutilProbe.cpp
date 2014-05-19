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

#include "DiskutilProbe.h"
#include <VectorPtrGuard.h>
#include "CommandReader.h"

using namespace std;

//****************************************************************************************//
//								DiskutilProbe Class										  //
//****************************************************************************************//
DiskutilProbe *DiskutilProbe::instance = NULL;

DiskutilProbe::DiskutilProbe() {
	records = NULL;
}

DiskutilProbe::~DiskutilProbe() {
	DiskutilProbe::DeleteRecords();
    instance = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* DiskutilProbe::Instance() {

	// Use lazy initialization
	if(instance == NULL)
		instance = new DiskutilProbe();

	return instance;
}

ItemVector* DiskutilProbe::CollectItems(Object *object) {

	// get the device from the provided object
	ObjectEntity* device = this->ValidateStringOperations(object->GetElementByName("device"));
	// get the filepath from the provided object
	ObjectEntity* filepath = this->ValidateStringOperations(object->GetElementByName("filepath"));

	StringVector* deviceNames = this->GetDeviceNames();

	VectorPtrGuard<Item> collectedItems(new ItemVector());
	StringVector::iterator it;
	for(it = deviceNames->begin(); it != deviceNames->end(); it++) {
		string deviceName = *it;

		auto_ptr<ItemEntity> deviceIe(new ItemEntity("device", deviceName));
		if (device->Analyze(deviceIe.get()) == OvalEnum::RESULT_TRUE) {
			if ((device->GetValue().compare(deviceName) != 0) || (records == NULL)) {
				records = this->GetPermissions(deviceName);
				deviceName = device->GetValue();
			}
	
			// loop through all vars if they are a regex match on name create item and return it
			PermissionsRecordVector::iterator iterator;
			for(iterator = records->begin(); iterator != records->end(); iterator++) {
				PermissionsRecord* record = *iterator;
				auto_ptr<ItemEntity> filepathIe(new ItemEntity("filepath", record->filepath));
				if (filepath->Analyze(filepathIe.get()) == OvalEnum::RESULT_TRUE) {
					collectedItems->push_back(this->FillItem(record));
				}
			}
		}
	}
	delete deviceNames;
	return collectedItems.release();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
Item* DiskutilProbe::CreateItem() {

	Item* item = new Item(0, 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#macos", 
						"macos-sc", 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#macos macos-system-characteristics-schema.xsd", 
						OvalEnum::STATUS_ERROR, 
						"diskutil_item");

	return item;
}

Item* DiskutilProbe::FillItem(PermissionsRecord* record) {
	Item* item = this->CreateItem();
	item->SetStatus(OvalEnum::STATUS_EXISTS);
	item->AppendElement(new ItemEntity("device", record->device, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
	item->AppendElement(new ItemEntity("filepath", record->filepath, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
	item->AppendElement(new ItemEntity("uread", record->uread, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
	item->AppendElement(new ItemEntity("uwrite", record->uwrite, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
	item->AppendElement(new ItemEntity("uexec", record->uexec, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
	item->AppendElement(new ItemEntity("gread", record->gread, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
	item->AppendElement(new ItemEntity("gwrite", record->gwrite, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
	item->AppendElement(new ItemEntity("gexec", record->gexec, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
	item->AppendElement(new ItemEntity("oread", record->oread, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
	item->AppendElement(new ItemEntity("owrite", record->owrite, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
	item->AppendElement(new ItemEntity("oexec", record->oexec, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
	return item;
}

ObjectEntity* DiskutilProbe::ValidateStringOperations(ObjectEntity* stringOp) {
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

StringVector* DiskutilProbe::GetDeviceNames() {
	StringVector lines = CommandReader::GetResponse("/usr/sbin/diskutil list");

	StringVector* deviceNames = new StringVector();
	StringVector::iterator it;
	for (it = lines.begin(); it != lines.end(); it++) {
		string line = *it;
		StringVector* elems = CommandReader::Split(line, " ");
		string name;
		if (elems->size() > 2) {
			StringVector::iterator elemsIt;
			for (elemsIt = elems->begin(); elemsIt != elems->end(); elemsIt++) {
				name = *elemsIt;
			}
			if (name != "IDENTIFIER")
				deviceNames->push_back(name);
		}
		delete elems;
	}
	return deviceNames;
}
PermissionsRecordVector* DiskutilProbe::GetPermissions(std::string device) {
	PermissionsRecordVector* records = new PermissionsRecordVector();
	string token, after, before, fp, ur, uw, ue, gr, gw, ge, ore, ow, oe;
	
	DiskutilProbe::DeleteRecords();
	StringVector lines = CommandReader::GetResponse("/usr/sbin/diskutil verifyPermissions " + device);
	StringVector::iterator it;
	for (it = lines.begin(); it != lines.end(); it++) {
		string line = *it;
		Log::Debug("Line: " + line);
		StringVector* elems = CommandReader::Split(line, "\"");
		if (elems->size() > 2) {
			ur = uw = ue = gr = gw = ge = ore = ow = oe = "same";
			StringVector::iterator it = elems->begin();
			token = *it++;
			if (Common::EqualsIgnoreCase(token, "Permissions differ on ")) {
				fp = *it++;
				token = *it;
				elems = CommandReader::Split(token, " ; ");
				if (elems->size() == 2) {
					it = elems->begin();
					before = *it++;
					after = *it;
					StringVector* bfElems = CommandReader::Split(before, "should be ");
					if (bfElems->size() == 2) {
						StringVector::iterator bfIt = bfElems->begin();
						*bfIt++;
						before = *bfIt;
						const char* expected = before.c_str();
						const char* permissions = after.c_str();
						ur = this->GetPermissionCompareType(permissions[10], expected[1]);
						uw = this->GetPermissionCompareType(permissions[11], expected[2]);
						ue = this->GetPermissionCompareType(permissions[12], expected[3]);
						gr = this->GetPermissionCompareType(permissions[13], expected[4]);
						gw = this->GetPermissionCompareType(permissions[14], expected[5]);
						ge = this->GetPermissionCompareType(permissions[15], expected[6]);
						ore = this->GetPermissionCompareType(permissions[16], expected[7]);
						ow = this->GetPermissionCompareType(permissions[17], expected[8]);
						oe = this->GetPermissionCompareType(permissions[18], expected[9]);
						Log::Debug("Diffs");
						records->push_back(new PermissionsRecord(device, fp, ur, uw, ue, gr, gw, ge, ore, ow, oe));
					}
					delete bfElems;
				}
				delete elems;
			} else {
				if (!Common::EqualsIgnoreCase(token, "Group differs on ")) {
					Log::Debug("All Same");
					records->push_back(new PermissionsRecord(device, fp, ur, uw, ue, gr, gw, ge, ore, ow, oe));
				}
			}
		}
	}
	return records;
}

std::string DiskutilProbe::GetPermissionCompareType(const char val, const char expected) {
	if (val == expected) {
		return "same";
	} else {
		if (val == '-') {
			return "less";
		} else {
			return "more";
		}
	}
}

void DiskutilProbe::DeleteRecords() {
	if (records == NULL) return;
	
	for(PermissionsRecordVector::iterator it = records->begin(); it != records->end(); it++)
		delete *it;

	delete records;
}