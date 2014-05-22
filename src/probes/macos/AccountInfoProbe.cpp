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

#include "AccountInfoProbe.h"
#include <sstream>
#include <VectorPtrGuard.h>
#include <Log.h>
#include "CommandReader.h"

using namespace std;

//****************************************************************************************//
//								AccountInfoProbe Class									  //
//****************************************************************************************//
AccountInfoProbe *AccountInfoProbe::instance = NULL;

AccountInfoProbe::AccountInfoProbe() {
}

AccountInfoProbe::~AccountInfoProbe() {
    instance = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* AccountInfoProbe::Instance() {

	// Use lazy initialization
	if(instance == NULL)
		instance = new AccountInfoProbe();

	return instance;
}

ItemVector* AccountInfoProbe::CollectItems(Object *object) {

	// get the name from the provided object
	ObjectEntity* name = this->ValidateStringOperations(object->GetElementByName("username"));

	if (names.size() == 0) {
		names = CommandReader::GetResponse("/usr/bin/dscl . -list /Users");
	}

	VectorPtrGuard<Item> collectedItems(new ItemVector());
	StringVector::iterator it;
	for(it = names.begin(); it != names.end(); it++) {
		string username = *it;
		Item* item = this->FillItem(username);
		if (item != NULL) {
			auto_ptr<ItemEntity> nameIe(new ItemEntity("username", username));
			if (name->Analyze(nameIe.get()) == OvalEnum::RESULT_TRUE) {
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
Item* AccountInfoProbe::CreateItem() {

	Item* item = new Item(0, 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#macos", 
						"macos-sc", 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#macos macos-system-characteristics-schema.xsd", 
						OvalEnum::STATUS_ERROR, 
						"accountinfo_item");

	return item;
}

Item* AccountInfoProbe::FillItem(string username) {

    std::string name, value, passwd, uid, gid, realname, home, shell;

	StringVector lines = CommandReader::GetResponse("/usr/bin/dscl . -read /Users/" + username + " passwd uid gid realname home shell");
	
	StringVector::iterator it;
	unsigned int multiline = 0;
	for (it = lines.begin(); it != lines.end(); it++) {
		string line = *it;
		if (multiline == 1) {
			multiline = 0;
			realname = line;
			Common::TrimString(realname);
		}
		StringVector elems = CommandReader::Split(line, ' ');
		StringVector::iterator it = elems.begin();
		name = *it;
		it++;
		Common::TrimString(name);
		if (it != elems.end()) {
			if (name.compare("dsAttrTypeNative:gid:") == 0)
				gid = *it;
			if (name.compare("dsAttrTypeNative:home:") == 0)
				home = *it;
			if (name.compare("dsAttrTypeNative:uid:") == 0)
				uid = *it;
			if (name.compare("dsAttrTypeNative:passwd:") == 0)
				passwd = *it;
			if (name.compare("dsAttrTypeNative:shell:") == 0)
				shell = *it;
		} else {
			if (name.compare("dsAttrTypeNative:realname:") == 0) {
				multiline = 1;
			}
		}
	}

	Item* item = this->CreateItem();
	item->SetStatus(OvalEnum::STATUS_EXISTS);
	item->AppendElement(new ItemEntity("username", username, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
	item->AppendElement(new ItemEntity("password", passwd, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
	item->AppendElement(new ItemEntity("uid", uid, OvalEnum::DATATYPE_INTEGER, OvalEnum::STATUS_EXISTS));
	item->AppendElement(new ItemEntity("gid", gid, OvalEnum::DATATYPE_INTEGER, OvalEnum::STATUS_EXISTS));
	item->AppendElement(new ItemEntity("realname", realname, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
	item->AppendElement(new ItemEntity("home_dir", home, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
	item->AppendElement(new ItemEntity("login_shell", shell, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
	return item;
}

ObjectEntity* AccountInfoProbe::ValidateStringOperations(ObjectEntity* stringOp) {
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
