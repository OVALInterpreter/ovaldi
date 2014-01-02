//
//
//****************************************************************************************//
// Copyright (c) 2002-2014, The MITRE Corporation
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

#include "EnvironmentVariableProbe.h"

using namespace std;

//****************************************************************************************//
//								EnvironmentVariableProbe Class							  //	
//****************************************************************************************//
EnvironmentVariableProbe *EnvironmentVariableProbe::instance = NULL;

EnvironmentVariableProbe::EnvironmentVariableProbe() {
}

EnvironmentVariableProbe::~EnvironmentVariableProbe() {
  instance = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* EnvironmentVariableProbe::Instance() {

	// Use lazy initialization
	if(instance == NULL) 
		instance = new EnvironmentVariableProbe();

	return instance;	
}

ItemVector* EnvironmentVariableProbe::CollectItems(Object *object) {
	ItemVector *collectedItems = new ItemVector();

	// get the name from the provided object
	ObjectEntity* name = object->GetElementByName("name");

	// check datatypes - only allow string
	if(name->GetDatatype() != OvalEnum::DATATYPE_STRING) {
		throw ProbeException("Error: invalid data type specified on name. Found: " + OvalEnum::DatatypeToString(name->GetDatatype()));
	}

	// check operation - only allow  equals, not equals and pattern match
	if(name->GetOperation() != OvalEnum::OPERATION_EQUALS && name->GetOperation() != OvalEnum::OPERATION_PATTERN_MATCH && name->GetOperation() != OvalEnum::OPERATION_NOT_EQUAL) {
		throw ProbeException("Error: invalid operation specified on name. Found: " + OvalEnum::OperationToString(name->GetOperation()));
	}

	if(name->GetVarRef() == NULL) {
		if(name->GetOperation() == OvalEnum::OPERATION_EQUALS) {
			char * envValue = getenv(name->GetValue().c_str());

			//	Check the value 
			if(envValue != NULL) {
				Item* item = this->CreateItem();
				item->SetStatus(OvalEnum::STATUS_EXISTS);
				item->AppendElement(new ItemEntity("name", name->GetValue(), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
				item->AppendElement(new ItemEntity("value", envValue, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
				collectedItems->push_back(item);
			} else {
				Item* item = this->CreateItem();
				item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
				item->AppendElement(new ItemEntity("name", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));
				collectedItems->push_back(item);
			}
		} else {			
			StringPairVector* envs = this->GetAllEnvs();

			// loop through all envs if they are a regex match on name create item and return it
			StringPairVector::iterator iterator;
			for(iterator = envs->begin(); iterator != envs->end(); iterator++) {
				string first = (*iterator)->first;
				if(name->GetOperation() == OvalEnum::OPERATION_NOT_EQUAL) {
					if(name->GetValue().compare(first) != 0) {
						string second = (*iterator)->second;
						Item* item = this->CreateItem();
						item->SetStatus(OvalEnum::STATUS_EXISTS);
						item->AppendElement(new ItemEntity("name", first, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
						item->AppendElement(new ItemEntity("value", second, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
						collectedItems->push_back(item);
					}
				} else {
					if(this->myMatcher->IsMatch(name->GetValue().c_str(), first.c_str())) {
						string second = (*iterator)->second;
						Item* item = this->CreateItem();
						item->SetStatus(OvalEnum::STATUS_EXISTS);
						item->AppendElement(new ItemEntity("name", first, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
						item->AppendElement(new ItemEntity("value", second, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
						collectedItems->push_back(item);
					}
				}
				
			}
			delete envs;
		}
	} else {
		StringPairVector* envs = this->GetAllEnvs();

	
		// loop through all envs on the system
		// only keep envs that match operation and value and var check
		StringPairVector::iterator it;
		for(it = envs->begin(); it != envs->end(); it++) {
			ItemEntity* tmp = new ItemEntity("name", (*it)->first, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS);
			if(name->Analyze(tmp) == OvalEnum::RESULT_TRUE) {
				Item* item = this->CreateItem();
				item->SetStatus(OvalEnum::STATUS_EXISTS);
				item->AppendElement(new ItemEntity("name", (*it)->first, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
				item->AppendElement(new ItemEntity("value", (*it)->second, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
				collectedItems->push_back(item);
			}
			delete tmp;
			tmp = NULL;
		}
		delete envs;
	}

	return collectedItems;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
Item* EnvironmentVariableProbe::CreateItem() {

	Item* item = new Item(0, 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#independent", 
						"ind-sc", 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#independent independent-system-characteristics-schema.xsd", 
						OvalEnum::STATUS_ERROR, 
						"environmentvariable_item");

	return item;
}

StringPairVector* EnvironmentVariableProbe::GetAllEnvs() {
	
	StringPairVector* envs = new StringPairVector();

	unsigned int i = 0;
	while(_environ[i] != NULL) {
		string strVarAndValue = _environ[i++];
		unsigned int eq = strVarAndValue.find("=", 0);
		string name = strVarAndValue.substr(0, eq);
		string value = strVarAndValue.substr(eq+1, strVarAndValue.length()-eq);
		StringPair* pair = new StringPair();
		pair->first = name;
		pair->second = value;
		envs->push_back(pair);
	} 
	return envs;
}
