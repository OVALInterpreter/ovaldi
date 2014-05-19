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

#include "PwPolicy59Probe.h"
#include <VectorPtrGuard.h>
#include "CommandReader.h"

using namespace std;

//****************************************************************************************//
//								PwPolicy59Probe Class									  //
//****************************************************************************************//
PwPolicy59Probe *PwPolicy59Probe::instance = NULL;

PwPolicy59Probe::PwPolicy59Probe() {
}

PwPolicy59Probe::~PwPolicy59Probe() {
    instance = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* PwPolicy59Probe::Instance() {

	// Use lazy initialization
	if(instance == NULL)
		instance = new PwPolicy59Probe();

	return instance;
}

ItemVector* PwPolicy59Probe::CollectItems(Object *object) {

	// get the name from the provided object
	ObjectEntity* name = this->ValidateStringOperations(object->GetElementByName("target_user"));
	ObjectEntity* username = this->ValidateStringOperations(object->GetElementByName("username"));
	ObjectEntity* userpass = this->ValidateStringOperations(object->GetElementByName("userpass"));
	ObjectEntity* directoryNode = this->ValidateStringOperations(object->GetElementByName("directory_node"));

	if (names.size() == 0) {
		names = CommandReader::GetResponse("/usr/bin/dscl . -list /Users");
	}

	VectorPtrGuard<Item> collectedItems(new ItemVector());
	StringVector::iterator it;
	for(it = names.begin(); it != names.end(); it++) {
		string targetUser = *it;
		auto_ptr<ItemEntity> nameIe(new ItemEntity("target_user", targetUser));
		if (name->Analyze(nameIe.get()) == OvalEnum::RESULT_TRUE) {
			collectedItems->push_back(this->FillItem(this->GetRecord(targetUser, username, userpass, directoryNode)));
		}
	}
	return collectedItems.release();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
Item* PwPolicy59Probe::CreateItem() {

	Item* item = new Item(0, 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#macos", 
						"macos-sc", 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#macos macos-system-characteristics-schema.xsd", 
						OvalEnum::STATUS_ERROR, 
						"pwpolicy59_item");

	return item;
}

Item* PwPolicy59Probe::FillItem(PwPolicyMap record) {
	Item* item = this->CreateItem();
	item->SetStatus(OvalEnum::STATUS_EXISTS);
	item->AppendElement(this->FillItemEntity("target_user", record["target_user"], OvalEnum::DATATYPE_STRING));
	item->AppendElement(this->FillItemEntity("username", record["username"], OvalEnum::DATATYPE_STRING));
	item->AppendElement(this->FillItemEntity("userpass", record["userpass"], OvalEnum::DATATYPE_STRING));
	item->AppendElement(this->FillItemEntity("directory_node", record["directory_node"], OvalEnum::DATATYPE_STRING));
	item->AppendElement(this->FillItemEntity("maxChars", record["maxChars"], OvalEnum::DATATYPE_INTEGER));
	item->AppendElement(this->FillItemEntity("maxFailedLoginAttempts", record["maxFailedLoginAttempts"], OvalEnum::DATATYPE_INTEGER));
	item->AppendElement(this->FillItemEntity("minChars", record["minChars"], OvalEnum::DATATYPE_INTEGER));
	item->AppendElement(this->FillItemEntity("passwordCannotBeName", record["passwordCannotBeName"], OvalEnum::DATATYPE_BOOLEAN));
	item->AppendElement(this->FillItemEntity("requiresAlpha", record["requiresAlpha"], OvalEnum::DATATYPE_BOOLEAN));
	item->AppendElement(this->FillItemEntity("requiresNumeric", record["requiresNumeric"], OvalEnum::DATATYPE_BOOLEAN));
	item->AppendElement(this->FillItemEntity("maxMinutesUntilChangePassword", record["maxMinutesUntilChangePassword"], OvalEnum::DATATYPE_INTEGER));
	item->AppendElement(this->FillItemEntity("minMinutesUntilChangePassword", record["minMinutesUntilChangePassword"], OvalEnum::DATATYPE_INTEGER));
	item->AppendElement(this->FillItemEntity("requiresMixedCase", record["requiresMixedCase"], OvalEnum::DATATYPE_BOOLEAN));
	item->AppendElement(this->FillItemEntity("requiresSymbol", record["requiresSymbol"], OvalEnum::DATATYPE_BOOLEAN));
	item->AppendElement(this->FillItemEntity("minutesUntilFailedLoginReset", record["minutesUntilFailedLoginReset"], OvalEnum::DATATYPE_INTEGER));
	item->AppendElement(this->FillItemEntity("usingHistory", record["usingHistory"], OvalEnum::DATATYPE_INTEGER));
	item->AppendElement(this->FillItemEntity("canModifyPasswordforSelf", record["canModifyPasswordforSelf"], OvalEnum::DATATYPE_BOOLEAN));
	item->AppendElement(this->FillItemEntity("usingExpirationDate", record["usingExpirationDate"], OvalEnum::DATATYPE_BOOLEAN));
	item->AppendElement(this->FillItemEntity("usingHardExpirationDate", record["usingHardExpirationDate"], OvalEnum::DATATYPE_BOOLEAN));
	item->AppendElement(this->FillItemEntity("expirationDateGMT", record["expirationDateGMT"], OvalEnum::DATATYPE_STRING));
	item->AppendElement(this->FillItemEntity("hardExpireDateGMT", record["hardExpireDateGMT"], OvalEnum::DATATYPE_STRING));
	item->AppendElement(this->FillItemEntity("maxMinutesUntilDisabled", record["maxMinutesUntilDisabled"], OvalEnum::DATATYPE_INTEGER));
	item->AppendElement(this->FillItemEntity("maxMinutesOfNonUse", record["maxMinutesOfNonUse"], OvalEnum::DATATYPE_INTEGER));
	item->AppendElement(this->FillItemEntity("newPasswordRequired", record["newPasswordRequired"], OvalEnum::DATATYPE_BOOLEAN));
	item->AppendElement(this->FillItemEntity("notGuessablePattern", record["notGuessablePattern"], OvalEnum::DATATYPE_BOOLEAN));
	return item;
}

ItemEntity* PwPolicy59Probe::FillItemEntity(string name, string value, OvalEnum::Datatype datatype) {
	ItemEntity* itemEntity;
	if (value == "") {
		itemEntity = new ItemEntity(name, value, datatype, OvalEnum::STATUS_DOES_NOT_EXIST);
	} else {
		itemEntity = new ItemEntity(name, value, datatype, OvalEnum::STATUS_EXISTS);
	}
	return itemEntity;
}

PwPolicyMap PwPolicy59Probe::GetRecord(string targetUser, ObjectEntity* username, ObjectEntity* userpass, ObjectEntity* directoryNode) {

	string command = "/usr/bin/pwpolicy --get-effective-policy -u " + targetUser;
	PwPolicyMap record;
	REGEX regex;


	record["target_user"] = targetUser;	
	if (!(username->GetNil() || userpass->GetNil())) {
		command += " -a " + username->GetValue() + " -p " + userpass->GetValue();
		record["username"] = username->GetValue();
		record["userpass"] = userpass->GetValue();
	}
	if (!directoryNode->GetNil()) {
		command += " -n " + directoryNode->GetValue();
		record["directory_node"] = directoryNode->GetValue();
	}

	StringVector lines = CommandReader::GetResponse(command);

	if (lines.size() > 0) {
		StringVector::iterator it = lines.begin();
		string line = *it;
		StringVector* elems = CommandReader::Split(line, " ");
		for (it = elems->begin(); it != elems->end(); it++) {
			string elem = *it;
			StringVector* keyvalues = CommandReader::Split(elem, "=");
			if (keyvalues->size() > 1) {
				StringVector::iterator keyit = keyvalues->begin();
				string key = *keyit++;
				string value = *keyit;
				// convert booleans to integers for elements that expect integer values
				if ((regex.IsMatch("^max.*", key.c_str()) == true) || (regex.IsMatch("^min.*", key.c_str()) == true)) {
					if (value.compare("false") == 0) {
						value = "0";
					}
					if (value.compare("true") == 0) {
						value = "1";
					}
				}
				record[key] = value;
			}
			delete keyvalues;
		}
		delete elems;
	}
	return record;
}

ObjectEntity* PwPolicy59Probe::ValidateStringOperations(ObjectEntity* stringOp) {
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

