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

#include <iomanip>
#include <sstream>

#include "RegistryProbe.h"

using namespace std;

//****************************************************************************************//
//								RegistryProbe Class										  //	
//****************************************************************************************//
RegistryProbe* RegistryProbe::instance = NULL;

RegistryProbe::RegistryProbe() : AbsProbe() {

}

RegistryProbe::~RegistryProbe() {
  instance = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* RegistryProbe::Instance() {

	// Use lazy initialization
	if(instance == NULL) 
		instance = new RegistryProbe();

	return instance;	
}

ItemVector* RegistryProbe::CollectItems(Object *object) {

	ObjectEntity* hive = object->GetElementByName("hive");
	ObjectEntity* key = object->GetElementByName("key");
	ObjectEntity* name = object->GetElementByName("name");

	// check datatypes - only allow string
	if(hive->GetDatatype() != OvalEnum::DATATYPE_STRING) {
		throw ProbeException("Error: invalid data type specified on hive. Found: " + OvalEnum::DatatypeToString(hive->GetDatatype()));
	}
	if(key->GetDatatype() != OvalEnum::DATATYPE_STRING) {
		throw ProbeException("Error: invalid data type specified on key. Found: " + OvalEnum::DatatypeToString(key->GetDatatype()));
	}
	if(name->GetDatatype() != OvalEnum::DATATYPE_STRING) {
		throw ProbeException("Error: invalid data type specified on name. Found: " + OvalEnum::DatatypeToString(name->GetDatatype()));
	}

	// check operation - only allow  equals, not equals and pattern match
	if(hive->GetOperation() != OvalEnum::OPERATION_EQUALS && hive->GetOperation() != OvalEnum::OPERATION_PATTERN_MATCH && hive->GetOperation() != OvalEnum::OPERATION_NOT_EQUAL) {
		throw ProbeException("Error: invalid operation specified on hive. Found: " + OvalEnum::OperationToString(hive->GetOperation()));
	}
	if(key->GetOperation() != OvalEnum::OPERATION_EQUALS && key->GetOperation() != OvalEnum::OPERATION_PATTERN_MATCH && key->GetOperation() != OvalEnum::OPERATION_NOT_EQUAL) {
		throw ProbeException("Error: invalid operation specified on key. Found: " + OvalEnum::OperationToString(key->GetOperation()));
	}
	if(name->GetOperation() != OvalEnum::OPERATION_EQUALS && name->GetOperation() != OvalEnum::OPERATION_PATTERN_MATCH && name->GetOperation() != OvalEnum::OPERATION_NOT_EQUAL) {
		throw ProbeException("Error: invalid operation specified on name. Found: " + OvalEnum::OperationToString(name->GetOperation()));
	}

	ItemVector *collectedItems = new ItemVector();
	RegistryFinder registryFinder(RegistryFinder::behavior2view(object->GetBehaviors()));

	RegKeyVector* registryKeys = registryFinder.SearchRegistries(hive, key, name, object->GetBehaviors());

	if(registryKeys->size() > 0) {
		RegKeyVector::iterator iterator;
		for(iterator = registryKeys->begin(); iterator != registryKeys->end(); iterator++) {

			RegKey* registryKey = (*iterator);
			
			if ( (registryKey->GetKey()).compare("") == 0 ){
				Item* item = NULL;

				StringSet* keys = NULL;
				if( !(key->GetNil()) && (keys = registryFinder.ReportKeyDoesNotExist(registryKey->GetHive(), key)) != NULL ) {
					for(StringSet::iterator iterator = keys->begin(); iterator != keys->end(); iterator++) {
						item = this->CreateItem();
						item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
						item->AppendElement(new ItemEntity("hive", registryKey->GetHive(), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
						item->AppendElement(new ItemEntity("key", (*iterator), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST, key->GetNil()));
						
						item->AppendElement(new ItemEntity("windows_view",
							(registryFinder.GetView()==RegistryFinder::BIT_32 ? "32_bit" : "64_bit")));
						collectedItems->push_back(item);
					}
				} else {
					item = this->CreateItem();
					item->SetStatus(OvalEnum::STATUS_EXISTS);
					item->AppendElement(new ItemEntity("hive", registryKey->GetHive(), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
					if (key->GetNil()){
						item->AppendElement(new ItemEntity("key", registryKey->GetKey(), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_NOT_COLLECTED, true)); //GetNil is true
					}
					else
					{
						item->AppendElement(new ItemEntity("key", registryKey->GetKey(), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS,false)); //GetNil is false
					}
					if (name->GetNil()){
						item->AppendElement(new ItemEntity("name", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_NOT_COLLECTED, true)); //GetNil is true
					}
					else
					{
						item->AppendElement(new ItemEntity("name", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS, false)); //GetNil is false
					}

					item->AppendElement(new ItemEntity("windows_view",
						(registryFinder.GetView()==RegistryFinder::BIT_32 ? "32_bit" : "64_bit")));

					collectedItems->push_back(item);
				}

				if ( keys != NULL ){
					keys->clear();
					delete keys;
					keys = NULL;
				}

			}else{			
				if((registryKey->GetName()).compare("") == 0 && name->GetNil() ) {
					Item* item = NULL;

					StringSet* names = NULL;
					if((names = registryFinder.ReportNameDoesNotExist(registryKey->GetHive(), registryKey->GetKey(), name)) != NULL) {
						for(StringSet::iterator iterator = names->begin(); iterator != names->end(); iterator++) {
							item = this->CreateItem();
							item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
							item->AppendElement(new ItemEntity("hive", registryKey->GetHive(), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
							if (key->GetNil()){
								item->AppendElement(new ItemEntity("key", registryKey->GetKey(), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_NOT_COLLECTED, true)); //GetNil is true
							}
							else
							{
								item->AppendElement(new ItemEntity("key", registryKey->GetKey(), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS,false)); //GetNil is false
							}
							item->AppendElement(new ItemEntity("name", (*iterator), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST, true));
							
							item->AppendElement(new ItemEntity("windows_view",
								(registryFinder.GetView()==RegistryFinder::BIT_32 ? "32_bit" : "64_bit")));
							collectedItems->push_back(item);
						}
					} else {
						item = this->CreateItem();
						item->SetStatus(OvalEnum::STATUS_EXISTS);
						item->AppendElement(new ItemEntity("hive", registryKey->GetHive(), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
						if (key->GetNil()){
							item->AppendElement(new ItemEntity("key", registryKey->GetKey(), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_NOT_COLLECTED, true)); //GetNil is true
						}
						else
						{
							item->AppendElement(new ItemEntity("key", registryKey->GetKey(), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS,false)); //GetNil is false
						}
						item->AppendElement(new ItemEntity("name", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_NOT_COLLECTED, true)); //GetNil is true as checked above
						
						item->AppendElement(new ItemEntity("windows_view",
							(registryFinder.GetView()==RegistryFinder::BIT_32 ? "32_bit" : "64_bit")));
						collectedItems->push_back(item);
					}
					if ( names != NULL ){
						names->clear();
						delete names;
						names = NULL;
					}
				} else {
					Item* item = this->GetRegistryKey( registryKey->GetHive(), registryKey->GetKey(), registryKey->GetName(), registryFinder);
					if(item != NULL) {
						item->AppendElement(new ItemEntity("windows_view",
							(registryFinder.GetView()==RegistryFinder::BIT_32 ? "32_bit" : "64_bit")));
						collectedItems->push_back(item);
					}
					item = NULL;
				}
			}

			delete registryKey;
		}
	}else {
		StringSet* hives = NULL;
		if ( (hives = registryFinder.ReportHiveDoesNotExist(hive)) != NULL ){
			for(StringSet::iterator iterator1 = hives->begin(); iterator1 != hives->end(); iterator1++) {
				Item* item = NULL;
				item = this->CreateItem();
				item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
				item->AppendElement(new ItemEntity("hive", (*iterator1), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));
				
				item->AppendElement(new ItemEntity("windows_view",
					(registryFinder.GetView()==RegistryFinder::BIT_32 ? "32_bit" : "64_bit")));
				collectedItems->push_back(item);
			}
			if ( hives != NULL ){
				hives->clear();
				delete hives;
				hives = NULL;
			}
		}else{
			Item* item = NULL;
			StringSet* hives = registryFinder.GetHives(hive);
			for(StringSet::iterator iterator1 = hives->begin(); iterator1 != hives->end() ; iterator1++){
				StringSet* keys = NULL;
				if((keys = registryFinder.ReportKeyDoesNotExist(*iterator1, key)) != NULL) {
					for(StringSet::iterator iterator2 = keys->begin(); iterator2 != keys->end(); iterator2++) {
						item = this->CreateItem();
						item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
						item->AppendElement(new ItemEntity("hive", (*iterator1), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
						item->AppendElement(new ItemEntity("key", (*iterator2), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST, key->GetNil()));	
					
						item->AppendElement(new ItemEntity("windows_view",
							(registryFinder.GetView()==RegistryFinder::BIT_32 ? "32_bit" : "64_bit")));
						collectedItems->push_back(item);
					}
				} else {
					StringSet* keys = registryFinder.GetKeys(*iterator1,key,object->GetBehaviors());
					for(StringSet::iterator iterator2 = keys->begin() ; iterator2 != keys->end() ; iterator2++){
						StringSet* names = NULL;
						if((names = registryFinder.ReportNameDoesNotExist(*iterator1, *iterator2, name)) != NULL) {			
							for(StringSet::iterator iterator3 = names->begin(); iterator3 != names->end(); iterator3++) {
								item = this->CreateItem();
								item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
								item->AppendElement(new ItemEntity("hive", (*iterator1), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
								item->AppendElement(new ItemEntity("key", (*iterator2), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS, key->GetNil()));
								item->AppendElement(new ItemEntity("name", (*iterator3), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST, name->GetNil()));
								
								item->AppendElement(new ItemEntity("windows_view",
									(registryFinder.GetView()==RegistryFinder::BIT_32 ? "32_bit" : "64_bit")));
								collectedItems->push_back(item);
							}
						}
						if ( names != NULL ){
							names->clear();
							delete names;
							names = NULL;
						}
					}
				}

                if ( keys != NULL ){
					keys->clear();
					delete keys;
					keys = NULL;
				}
			}
		}
	}

	registryKeys->clear();
	delete registryKeys;

	return collectedItems;
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
Item* RegistryProbe::CreateItem() {

	Item* item = new Item(0, 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows", 
						"win-sc", 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows windows-system-characteristics-schema.xsd", 
						OvalEnum::STATUS_ERROR, 
						"registry_item");

	return item;
}

Item* RegistryProbe::GetRegistryKey(string hive, string key, string name, RegistryFinder &registryFinder) {
	
    HKEY hkey, rootKey;
	LONG res;
	Item* item = NULL;

	// Check hive
	res = registryFinder.GetHKeyHandle(&rootKey, hive, "");		
	if(res) {
	
		item->AppendElement(new ItemEntity("hive", hive, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));
		item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
		return item;

	} else {

		item = this->CreateItem();
		item->AppendElement(new ItemEntity("hive", hive, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));

		res = registryFinder.GetHKeyHandle(&hkey, rootKey, key);

		if (res != ERROR_SUCCESS) {
			if (res == ERROR_FILE_NOT_FOUND || res == ERROR_BAD_PATHNAME) {

				item->AppendElement(new ItemEntity("key", key, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));
				item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);

			} else if (res == ERROR_INVALID_HANDLE) {

				string errorMessage = "";
				errorMessage.append("(RegistryProbe) The handle for the registry key '");
				errorMessage.append(key);
				errorMessage.append("' is not valid.");
						
				item->AppendMessage(new OvalMessage(errorMessage, OvalEnum::LEVEL_ERROR));
				item->AppendElement(new ItemEntity("key", key, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_ERROR));
				item->SetStatus(OvalEnum::STATUS_ERROR);
				// I chose to make this an item returned with an error because at a minimum the 
				// hive was found. Note that the other option is to throw an exception which 
				// would result in an error on the collected object.

			} else {
				
				string systemErrMsg = WindowsCommon::GetErrorMessage(res);

				string errorMessage = "(RegistryProbe) Unable to get values for registry key '" +
					key + "'.  Error Code - " + Common::ToString(res) + " - " + systemErrMsg;
				
				item->AppendMessage(new OvalMessage(errorMessage, OvalEnum::LEVEL_ERROR));
				item->AppendElement(new ItemEntity("key", key, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_ERROR));
				item->SetStatus(OvalEnum::STATUS_ERROR);
				// I chose to make this an item returned with an error because at a minimum the 
				// hive was found. Note that the other option is to throw and exception which 
				// would result in an error on the collected object. 
				
			}		

		} else {

			// Add the key to the result item
			item->AppendElement(new ItemEntity("key", key, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
			
			DWORD type = 0;
			DWORD valuelen = 0;

			// Determine how big the buffer must be to store the data.  By specifying NULL for the
			// buffer size parameter, the function returns the value ERROR_MORE_DATA, and stores
			// the required buffer size, in bytes, into valuelen.
			res = RegQueryValueEx(hkey,						// handle to key
								name.c_str(),	// value name
								NULL,						// reserved
								NULL,						// type buffer
								NULL,						// data buffer
								&valuelen);					// size of data buffer

			// Allocate space for the buffer.
			LPBYTE value = (LPBYTE) malloc(valuelen);

			// Retrieve the type and value for the specified name associated with an open registry
			// key.
			res = RegQueryValueEx(hkey,						// handle to key
								name.c_str(),	// value name
								NULL,						// reserved
								&type,						// type buffer
								value,						// data buffer
								&valuelen);					// size of data buffer

			if (res == ERROR_FILE_NOT_FOUND || res == ERROR_BAD_PATHNAME) {
				
				item->AppendElement(new ItemEntity("name", name, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));
				item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
			} else if (res != ERROR_SUCCESS) {

				string systemErrMsg = WindowsCommon::GetErrorMessage(res);

				string errorMessage = "Unable to get type and value for the specified name: '" +
					name + "'.  Error Code - " + Common::ToString(res) + " - " + systemErrMsg;

				item->AppendMessage(new OvalMessage(errorMessage, OvalEnum::LEVEL_ERROR));
				item->AppendElement(new ItemEntity("name", name, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
				item->AppendElement(new ItemEntity("type",  "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_ERROR));
				item->AppendElement(new ItemEntity("value",  "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_ERROR));
				item->SetStatus(OvalEnum::STATUS_ERROR);

			//	Only call RetrieveInfo() if res == ERROR_SUCCESS
			} else {

				// Now add the name entity.
				item->AppendElement(new ItemEntity("name", name, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
				item->SetStatus(OvalEnum::STATUS_EXISTS);

				FILETIME lastWriteTime = {0};				
				long status = RegQueryInfoKey(hkey, NULL, NULL, NULL, 0, 0, NULL, NULL, NULL, NULL, NULL, &lastWriteTime);
					
				if(status == ERROR_SUCCESS){
					item->AppendElement(new ItemEntity("last_write_time", WindowsCommon::ToString(lastWriteTime), OvalEnum::DATATYPE_INTEGER, OvalEnum::STATUS_EXISTS));
				}else{
					item->AppendElement(new ItemEntity("last_write_time", WindowsCommon::ToString(lastWriteTime), OvalEnum::DATATYPE_INTEGER, OvalEnum::STATUS_ERROR));
					item->SetStatus(OvalEnum::STATUS_ERROR);
				}

				// We now have all the info we need.
				this->RetrieveInfo(hive, key, name, type, value, valuelen, item);
			}

			free(value);
			RegCloseKey(hkey);
		} 
	}

	return item;
}

void RegistryProbe::RetrieveInfo(string hiveIn, string keyIn, string nameIn, 
									  DWORD typeIn, LPBYTE valueIn, DWORD valuelenIn, Item* item) {

	switch (typeIn) {

		case REG_BINARY:
			{
				item->AppendElement(new ItemEntity("type", "reg_binary", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));

				ostringstream oss;
				oss << hex << setfill('0');
				for (DWORD i = 0; i<valuelenIn; ++i)
					oss << setw(2) << (int)valueIn[i];
				item->AppendElement(new ItemEntity("value", oss.str(), OvalEnum::DATATYPE_BINARY));
				break;
			}

		case REG_DWORD:
			{
				item->AppendElement(new ItemEntity("type", "reg_dword", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
				item->AppendElement(new ItemEntity("value", Common::ToString(*(DWORD*)valueIn),
					OvalEnum::DATATYPE_INTEGER, OvalEnum::STATUS_EXISTS));
				break;
			}

		case REG_EXPAND_SZ:
			{
				item->AppendElement(new ItemEntity("type", "reg_expand_sz", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
				// don't trust there is a terminal null...
				// Use the value length.
				item->AppendElement(new ItemEntity("value", string((const char*)valueIn, valuelenIn)));
				break;
			}

		case REG_MULTI_SZ:
			{
				item->AppendElement(new ItemEntity("type", "reg_multi_sz", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));

				// The following loop stops when an empty string is encountered.
				// This means if there are embedded empty strings, anything
				// following will be skipped.  MSDN docs say it is not possible
				// to include an empty string within the sequence
				// (http://msdn.microsoft.com/en-us/library/windows/desktop/ms724884%28v=vs.85%29.aspx)
				// so nothing should be skipped unless the REG_MULTI_SZ value is
				// malformed.  This could have been coded more simply, but
				// I wanted to make sure there was no overrun if the string
				// wasn't null-terminated.
				LPBYTE beg = valueIn, end = valueIn + valuelenIn;
				LPBYTE substrEnd;
				bool addedValue = false;
				while (beg < end && *beg) { // while not an empty string
					addedValue = true;
					// don't trust we have null termination... find the real end!
					substrEnd = beg;
					while (substrEnd < end && *substrEnd) ++substrEnd;
					item->AppendElement(new ItemEntity("value", 
						string((const char*)beg, substrEnd - beg)));
					// if we found end-of-string instead of end-of-buffer, skip
					// to the beginning of the next string.
					if (substrEnd < end) ++substrEnd;
					beg = substrEnd;
				}

				// Safety check: make sure to add a value entity with
				// status="does not exist" in case we didn't find any values.
				if (!addedValue)
					item->AppendElement(new ItemEntity("value", "", 
						OvalEnum::DATATYPE_STRING,
						OvalEnum::STATUS_DOES_NOT_EXIST));

				break;
			}

		case REG_SZ:
			{
				item->AppendElement(new ItemEntity("type", "reg_sz", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
				// if the value ends with 0, as REG_SZ values should, make sure
				// that 0 doesn't get into our entity value...
				if (valuelenIn > 0 && valueIn[valuelenIn-1] == 0)
					--valuelenIn;
				item->AppendElement(new ItemEntity("value", string((const char*)valueIn, valuelenIn)));
				break;
			}
			
		case REG_NONE:
			{
				item->AppendMessage(new OvalMessage("Values of type reg_none do not have a defined type. As a result, we are representing them as hexadecimal values."));
				item->AppendElement(new ItemEntity("type", "reg_none", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));

				ostringstream oss;
				oss << hex << setfill('0');
				for (DWORD i = 0; i<valuelenIn; ++i)
					oss << setw(2) << (int)valueIn[i];
				item->AppendElement(new ItemEntity("value", oss.str(), OvalEnum::DATATYPE_BINARY));

				break;
			}

		case REG_QWORD:
			{
				item->AppendElement(new ItemEntity("type", "reg_qword", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
				item->AppendElement(new ItemEntity("value", Common::ToString(*(ULONG64*)valueIn),
					OvalEnum::DATATYPE_INTEGER, OvalEnum::STATUS_EXISTS));
				break;
			}

		default:
			{
				item->AppendMessage(new OvalMessage("Error: Unable to determine the type and value of the registry key.", OvalEnum::LEVEL_ERROR));
				item->AppendElement(new ItemEntity("type", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_ERROR));
				item->AppendElement(new ItemEntity("value", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_ERROR));
				
				break;
			}
	}
}
