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

#include <Log.h>
#include <VectorPtrGuard.h>
#include "CommandReader.h"

#include "PwPolicy59Probe.h"

using namespace std;

//****************************************************************************************//
//								PwPolicy59Probe Class									  //
//****************************************************************************************//
PwPolicy59Probe *PwPolicy59Probe::instance = NULL;

PwPolicy59Probe::PwPolicy59Probe() {
	// put these in the order they must appear in items!
	propTypes.push_back(make_pair("maxChars", OvalEnum::DATATYPE_INTEGER));
	propTypes.push_back(make_pair("maxFailedLoginAttempts", OvalEnum::DATATYPE_INTEGER));
	propTypes.push_back(make_pair("minChars", OvalEnum::DATATYPE_INTEGER));
	propTypes.push_back(make_pair("passwordCannotBeName", OvalEnum::DATATYPE_BOOLEAN));
	propTypes.push_back(make_pair("requiresAlpha", OvalEnum::DATATYPE_BOOLEAN));
	propTypes.push_back(make_pair("requiresNumeric", OvalEnum::DATATYPE_BOOLEAN));
	propTypes.push_back(make_pair("maxMinutesUntilChangePassword", OvalEnum::DATATYPE_INTEGER));
	propTypes.push_back(make_pair("minMinutesUntilChangePassword", OvalEnum::DATATYPE_INTEGER));
	propTypes.push_back(make_pair("requiresMixedCase", OvalEnum::DATATYPE_BOOLEAN));
	propTypes.push_back(make_pair("requiresSymbol", OvalEnum::DATATYPE_BOOLEAN));
	propTypes.push_back(make_pair("minutesUntilFailedLoginReset", OvalEnum::DATATYPE_INTEGER));
	propTypes.push_back(make_pair("usingHistory", OvalEnum::DATATYPE_INTEGER));
	propTypes.push_back(make_pair("canModifyPasswordforSelf", OvalEnum::DATATYPE_BOOLEAN));
	propTypes.push_back(make_pair("usingExpirationDate", OvalEnum::DATATYPE_BOOLEAN));
	propTypes.push_back(make_pair("usingHardExpirationDate", OvalEnum::DATATYPE_BOOLEAN));
	propTypes.push_back(make_pair("expirationDateGMT", OvalEnum::DATATYPE_STRING));
	propTypes.push_back(make_pair("hardExpireDateGMT", OvalEnum::DATATYPE_STRING));
	propTypes.push_back(make_pair("maxMinutesUntilDisabled", OvalEnum::DATATYPE_INTEGER));
	propTypes.push_back(make_pair("maxMinutesOfNonUse", OvalEnum::DATATYPE_INTEGER));
	propTypes.push_back(make_pair("newPasswordRequired", OvalEnum::DATATYPE_BOOLEAN));
	propTypes.push_back(make_pair("notGuessablePattern", OvalEnum::DATATYPE_BOOLEAN));
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

	if (names.empty()) {
		names = CommandReader::GetResponse("/usr/bin/dscl . -list /Users");
	}

	VectorPtrGuard<Item> collectedItems(new ItemVector());
	StringVector::iterator it;
	ItemEntity nameIe("target_user");
	for(it = names.begin(); it != names.end(); it++) {
		string targetUser = *it;
		nameIe.SetValue(targetUser);
		if (name->Analyze(&nameIe) == OvalEnum::RESULT_TRUE) {
			Item *item = this->FillItem(this->GetRecord(targetUser));
			if (!username->GetNil() || !userpass->GetNil() || !directoryNode->GetNil())
				item->AppendMessage(new OvalMessage("Warning: due to security "
					"concerns, non-nil username, userpass, and directory_node "
					"object entities are ignored.", OvalEnum::LEVEL_WARNING));
			collectedItems->push_back(item);
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

Item* PwPolicy59Probe::FillItem(const PwPolicyMap &record) {

	auto_ptr<Item> item(this->CreateItem());
	item->SetStatus(OvalEnum::STATUS_EXISTS);

	// record should always have target_user!
	PwPolicyMap::const_iterator mapIter = record.find("target_user");
	if (mapIter == record.end()) throw ProbeException("Internal error, target_user missing!");
	item->AppendElement(new ItemEntity("target_user", mapIter->second));

	// these are always nil in this implementation
	item->AppendElement(new ItemEntity("username", "", OvalEnum::DATATYPE_STRING,
									   OvalEnum::STATUS_NOT_COLLECTED, true));
	item->AppendElement(new ItemEntity("userpass", "", OvalEnum::DATATYPE_STRING,
									   OvalEnum::STATUS_NOT_COLLECTED, true));
	item->AppendElement(new ItemEntity("directory_node", "", OvalEnum::DATATYPE_STRING,
									   OvalEnum::STATUS_NOT_COLLECTED, true));

	// validate the formatting of values as we go.  There was a report that the
	// 'pwpolicy' command can sometimes return weird values for properties,
	// e.g. "true" or "false" for integer-valued properties.
	int tmpInt;
	bool tmpBool;
	for (size_t i=0; i<propTypes.size(); ++i) {
		mapIter = record.find(propTypes[i].first);

		if (mapIter != record.end()) {
			switch (propTypes[i].second) {
			case OvalEnum::DATATYPE_INTEGER:
				if (Common::FromString(mapIter->second, &tmpInt)) {
					item->AppendElement(new ItemEntity(mapIter->first,
													   mapIter->second,
													   OvalEnum::DATATYPE_INTEGER));
				} else {
					
					item->AppendMessage(new OvalMessage(
						"Invalid integer value for password policy property \"" + 
						mapIter->first + "\": " + mapIter->second, 
						OvalEnum::LEVEL_ERROR));

					item->AppendElement(new ItemEntity(mapIter->first, 
													   "",
													   OvalEnum::DATATYPE_INTEGER,
													   OvalEnum::STATUS_ERROR));
				}

				break;

			case OvalEnum::DATATYPE_BOOLEAN:
				if (Common::FromString(mapIter->second, &tmpBool)) {
					item->AppendElement(new ItemEntity(mapIter->first,
													   tmpBool ? "true":"false",
													   OvalEnum::DATATYPE_BOOLEAN));
				} else {
					
					item->AppendMessage(new OvalMessage(
						"Invalid boolean value for password policy property \"" + 
						mapIter->first + "\": " + mapIter->second, 
						OvalEnum::LEVEL_ERROR));

					item->AppendElement(new ItemEntity(mapIter->first, 
													   "",
													   OvalEnum::DATATYPE_BOOLEAN,
													   OvalEnum::STATUS_ERROR));
				}


				break;

			default:
				// strings: anything's ok.
				item->AppendElement(new ItemEntity(mapIter->first, mapIter->second,
												   propTypes[i].second));
			}

		} else
			item->AppendElement(new ItemEntity(propTypes[i].first, "",
											   propTypes[i].second,
											   OvalEnum::STATUS_DOES_NOT_EXIST));
		
	}

	return item.release();
}

PwPolicy59Probe::PwPolicyMap PwPolicy59Probe::GetRecord(const string &targetUser) {

	string command = "/usr/bin/pwpolicy --get-effective-policy -u " + targetUser;
	PwPolicyMap record;
	REGEX regex;

	record["target_user"] = targetUser;	
	StringVector lines = CommandReader::GetResponse(command);

	if (!lines.empty()) {
		string &line = lines[0];
		StringVector elems = CommandReader::Split(line, " ");
		for (StringVector::iterator it = elems.begin(); it != elems.end(); it++) {
			StringVector keyvalues = CommandReader::Split(*it, "=");
			if (keyvalues.size() > 1) {
				string &key = keyvalues[0];
				string &value = keyvalues[1];
				record[key] = value;
			} else
				Log::Debug("Unexpected format in pwpolicy output:\nkey/value:\n\t" +
						   *it+"\nFrom line:\n\t" + line);
		}
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
