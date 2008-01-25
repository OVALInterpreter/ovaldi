//
// $Id: RegistryProbe.cpp 4666 2008-01-23 14:03:59Z bakerj $
//
//****************************************************************************************//
// Copyright (c) 2002-2008, The MITRE Corporation
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

#include "RegistryProbe.h"

//****************************************************************************************//
//								RegistryProbe Class										  //	
//****************************************************************************************//
RegistryProbe* RegistryProbe::instance = NULL;

RegistryProbe::RegistryProbe() : AbsProbe() {

}

RegistryProbe::~RegistryProbe() {

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


	//	If in verbose logging mode write out a record for each match found 
	//	even if a later refinement excludes that match. For example, if
	//	a key pattern match is specified of .* all keys will match. Now a name
	//	of 'foo' under the key has been specified. In verbose mode a record for 
	//	all keys that matched will be printed and any key that doesn't have 
	//	a name of 'foo under it will have a message stating that the name was 
	//	not found. If not in verbose mode only keys that have a matching name 
	//	are printed.


	ItemVector *collectedItems = new ItemVector();

	// get the hive, key, and name from the provided object
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

	// TODO - determine how to support behaviors.
	if(object->GetBehaviors()->size() != 0) {
		throw ProbeException("Error: (RegistryProbe) Behaviors are not supported."); 
	}

	// get all the hives
	ItemEntityVector* hives = this->GetHives(hive);
	ItemEntityVector::iterator hiveIt;
	for(hiveIt = hives->begin(); hiveIt != hives->end(); hiveIt++) {
		
		// get all keys for the hive if the key is not nil
		if(key->GetNil()) {
			// get the registry item.
			Item* item = this->GetRegistryKey((*hiveIt), NULL, NULL);
			if(item != NULL) {
				collectedItems->push_back(item);
			}
		} else {

			ItemEntityVector* keys = this->GetKeys(key, (*hiveIt));
			ItemEntityVector::iterator keyIt;
			for(keyIt = keys->begin(); keyIt != keys->end(); keyIt++) {

				// get all names for the hive and key if the name is not nil
				if(name->GetNil()) {
						// get the registry item.
						Item* item = this->GetRegistryKey((*hiveIt), (*keyIt), NULL);
						if(item != NULL) {
							collectedItems->push_back(item);
						}
				} else {
					ItemEntityVector* names = this->GetNames(name, (*hiveIt), (*keyIt));
					ItemEntityVector::iterator nameIt;
					for(nameIt = names->begin(); nameIt != names->end(); nameIt++) {

						// get the registry item.
						Item* item = this->GetRegistryKey((*hiveIt), (*keyIt), (*nameIt));
						if(item != NULL) {
							collectedItems->push_back(item);
						}
					}
					delete names;
				}
			}
			delete keys;
		}
	}
	delete hives;

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

void RegistryProbe::GetMatchingHives(string pattern, ItemEntityVector* hives, bool isRegex) {

	try {
		if(this->IsMatch(pattern, "HKEY_CLASSES_ROOT", isRegex)) {
			ItemEntity* entity = this->CreateItemEntity(NULL);
			entity->SetName("hive");
			entity->SetValue("HKEY_CLASSES_ROOT");
			hives->push_back(entity);
		}

		if(this->IsMatch(pattern, "HKEY_CURRENT_CONFIG", isRegex)) {
			ItemEntity* entity = this->CreateItemEntity(NULL);
			entity->SetName("hive");
			entity->SetValue("HKEY_CURRENT_CONFIG");
			hives->push_back(entity);
		}

		if(this->IsMatch(pattern, "HKEY_CURRENT_USER", isRegex)) {
			ItemEntity* entity = this->CreateItemEntity(NULL);
			entity->SetName("hive");
			entity->SetValue("HKEY_CURRENT_USER");
			hives->push_back(entity);
		}

		if(this->IsMatch(pattern, "HKEY_LOCAL_MACHINE", isRegex)) {
			ItemEntity* entity = this->CreateItemEntity(NULL);
			entity->SetName("hive");
			entity->SetValue("HKEY_LOCAL_MACHINE");
			hives->push_back(entity);
		}

		if(this->IsMatch(pattern, "HKEY_USERS", isRegex)) {
			ItemEntity* entity = this->CreateItemEntity(NULL);
			entity->SetName("hive");
			entity->SetValue("HKEY_USERS");
			hives->push_back(entity);
		}
		
	} catch(REGEXException ex) {
		if(ex.GetSeverity() == ERROR_WARN) {
			string pcreMsg = "";
			pcreMsg.append("Registry Keys Probe Warning - while searching for matching hives:\n");
			pcreMsg.append("-----------------------------\n");
			pcreMsg.append(ex.GetErrorMessage());
			Log::Debug(pcreMsg);
		}
		
		throw;
	}
}

void RegistryProbe::GetMatchingKeys(string hiveIn, string keyIn, string pattern, ItemEntityVector* keys, bool isRegex) {

	HKEY hkey;								//	pointer to the key that will be opened
	LONG res;								//	result from enumerating the subkeys
	LONG openRes;							//	result from opening the key
	LPTSTR lpName = (LPTSTR)malloc(1024);	//	buffer to store the subkey name
	DWORD dwName = 1024;					//	number of chars in the subkey
	DWORD dwIndex = 0;						//	index of subkey to enumerate
	FILETIME ftLastWriteTime;				//	time the cur subkey was last written to
	string workingKey = "";					//	The name of the keyIn and the subkey concatenated
	string errorMessage = "";				//	

	/////////////////////////////////////////////////////////////////
	//	Open the specified key
	/////////////////////////////////////////////////////////////////
	HKEY rootKey = GetRootKey(hiveIn);
	if(rootKey == NULL) {
		errorMessage.append("(RegistryProbe) The registry hive '");
		errorMessage.append(hiveIn);
		errorMessage.append("' does not exist.");
		throw ProbeException(errorMessage);
	}
			

	openRes = RegOpenKeyEx(	rootKey,		// handle to open hive
							keyIn.c_str(),	// subkey name
							0,				// reserved
							KEY_READ,		// security access mask
							&hkey);			// pointer to open key

	/////////////////////////////////////////////////////////////////
	//	Check attempt to open key
	/////////////////////////////////////////////////////////////////
	if (openRes == ERROR_SUCCESS) {

		for (dwIndex = 0, res = ERROR_SUCCESS; res == ERROR_SUCCESS; dwIndex++) {
			//	Get the working key as a string
			workingKey = keyIn;
			if(workingKey.compare("") != 0) {

				if(workingKey.at(workingKey.length()-1) != '\\') {
					workingKey.append("\\");
				}
			}

			//	Reset the buffer and the buffer size
			dwName = 1024;
			ZeroMemory(lpName, dwName);
						
			res = RegEnumKeyEx(	hkey,				// handle to key to enumerate
								dwIndex,			// subkey index
								lpName,				// subkey name
								&dwName,			// size of subkey buffer
								NULL,				// reserved
								NULL,				// class string buffer
								NULL,				// size of class string buffer
								&ftLastWriteTime);	// last write time

			//	Check results
			if(res == ERROR_SUCCESS) {

				//	Add the subkey to the working key
				workingKey.append(lpName);

				//	Make recursive call
				this->GetMatchingKeys(hiveIn, workingKey, pattern, keys);

				//	If a match add the new key to the keys vector
				if(this->IsMatch(pattern, workingKey, isRegex)) {
					ItemEntity* entity = this->CreateItemEntity(NULL);
					entity->SetName("key");
					entity->SetValue(workingKey);
					keys->push_back(entity);
				}
			}
		}
	}
	
	RegCloseKey(hkey);	
}

void RegistryProbe::GetMatchingNames(string hiveIn, string keyIn, string pattern, ItemEntityVector* names, bool isRegex) {

	HKEY hkey;								//	pointer to the key that will be opened
	LONG res;								//	result from enumerating the subkeys
	LONG openRes;							//	result from opening the key
	LPTSTR lpName = (LPTSTR)malloc(1024);	//	buffer to store the subkey name
	DWORD dwName = 1024;					//	number of chars in the subkey
	DWORD dwIndex = 0;						//	index of subkey to enumerate
	string name = "";						//	The name of the keyIn and the subkey concatenated
	string errorMessage = "";				//

	/////////////////////////////////////////////////////////////////
	//	Open the specified key
	/////////////////////////////////////////////////////////////////
	HKEY rootKey = GetRootKey(hiveIn);
	if(rootKey == NULL) {

		errorMessage.append("(RegistryProbe) The registry hive '");
		errorMessage.append(hiveIn);
		errorMessage.append("' does not exist.");
		throw ProbeException(errorMessage);
	}
			

	openRes = RegOpenKeyEx(	rootKey,		// handle to open hive
							keyIn.c_str(),	// subkey name
							0,				// reserved
							KEY_QUERY_VALUE,// security access mask
							&hkey);			// pointer to open key

	/////////////////////////////////////////////////////////////////
	//	Check attempt to open key
	/////////////////////////////////////////////////////////////////
	if (openRes == ERROR_SUCCESS) {

		try {
			myMatcher->Reset();
			for (dwIndex = 0, res = ERROR_SUCCESS; res == ERROR_SUCCESS; dwIndex++) {

				//	Reset the buffer and the buffer size
				dwName = 1024;
				ZeroMemory(lpName, dwName);
						
				res = RegEnumValue(	hkey,		// handle to key to query
									dwIndex,	// index of value to query
									lpName,		// value buffer
									&dwName,	// size of value buffer
									NULL,		// reserved
									NULL,		// type buffer
									NULL,		// data buffer
									NULL);		// size of data buffer

				//	Check results
				if(res == ERROR_SUCCESS) {

					//	Get the name
					name = "";
					name.append(lpName);

					//	If a match add the new name to the names vector
					if(this->IsMatch(pattern, name, isRegex)) {
						ItemEntity* entity = this->CreateItemEntity(NULL);
						entity->SetName("name");
						entity->SetValue(name);
						names->push_back(entity);
					}
				}
			}
		} catch(REGEXException ex) {
			if(ex.GetSeverity() == ERROR_WARN) {

				string pcreMsg = "";
				pcreMsg.append("Registry Keys Probe Warning - when searching for matching names:\n");
				pcreMsg.append("-----------------------------\n");
				pcreMsg.append(ex.GetErrorMessage());
				Log::Debug(pcreMsg);
			} else {
				throw;
			}
		}
	}
	
	RegCloseKey(hkey);	
}

Item* RegistryProbe::GetRegistryKey(ItemEntity* hive, ItemEntity* key, ItemEntity* name) {

    HKEY hkey;
	DWORD parse_depth = 0;
	LONG res;
	Item* item = NULL;

	// Check hive
	HKEY rootKey = this->GetRootKey(hive->GetValue());		
	if(rootKey == NULL) {
	
		item->AppendElement(new ItemEntity("hive", hive->GetValue(), OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_DOES_NOT_EXIST));
		item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
		return item;

	} else {

		item = this->CreateItem();
		item->AppendElement(new ItemEntity("hive", hive->GetValue(), OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));

		// check and get key if key is not nil
		if(key == NULL) {
			item->SetStatus(OvalEnum::STATUS_EXISTS);
		} else {
			res = RegOpenKeyEx(rootKey,					// handle to open hive
							key->GetValue().c_str(),	// subkey name
							0,							// reserved
							KEY_READ,					// security access mask
							&hkey);						// pointer to open key

			if (res != ERROR_SUCCESS) {
				if (res == ERROR_FILE_NOT_FOUND || res == ERROR_BAD_PATHNAME) {

					item->AppendElement(new ItemEntity("key", key->GetValue(), OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_DOES_NOT_EXIST));
					item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);

				} else if (res == ERROR_INVALID_HANDLE) {

					string errorMessage = "";
					errorMessage.append("(RegistryProbe) The handle for the registry key '");
					errorMessage.append(key->GetValue());
					errorMessage.append("' is not valid.");
							
					item->AppendMessage(new OvalMessage(errorMessage, OvalEnum::LEVEL_ERROR));
					item->AppendElement(new ItemEntity("key", key->GetValue(), OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_ERROR));
					item->SetStatus(OvalEnum::STATUS_ERROR);
					// I chose to make this an item returned with an error because at a minimum the 
					// hive was found. Note that the other option is to throw and exception which 
					// would result in an error on the collected object.

				} else {
					
					string systemErrMsg = WindowsCommon::GetErrorMessage(res);

					char errorCodeBuffer[20];
					_ltoa(res, errorCodeBuffer, 20);

					string errorMessage = "";
					errorMessage.append("(RegistryProbe) Unable to get values for registry key '");
					errorMessage.append(key->GetValue());
					errorMessage.append("'.  Error Code - ");
					errorMessage.append(errorCodeBuffer);
					errorMessage.append(" - " + systemErrMsg);
					
					item->AppendMessage(new OvalMessage(errorMessage, OvalEnum::LEVEL_ERROR));
					item->AppendElement(new ItemEntity("key", key->GetValue(), OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_ERROR));
					item->SetStatus(OvalEnum::STATUS_ERROR);
					// I chose to make this an item returned with an error because at a minimum the 
					// hive was found. Note that the other option is to throw and exception which 
					// would result in an error on the collected object. 
					
				}		

			} else {

				// add the key to the result item
				item->AppendElement(new ItemEntity("key", key->GetValue(), OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));

				// if name not nil get the name
				if(name == NULL) {
					item->SetStatus(OvalEnum::STATUS_EXISTS);
				} else {
				
					DWORD type = 0;
					DWORD valuelen = 0;

					// Determine how big the buffer must be to store the data.  By specifying NULL for the
					// buffer size parameter, the function returns the value ERROR_MORE_DATA, and stores
					// the required buffer size, in bytes, into valuelen.
					res = RegQueryValueEx(hkey,						// handle to key
										name->GetValue().c_str(),	// value name
										NULL,						// reserved
										NULL,						// type buffer
										NULL,						// data buffer
										&valuelen);					// size of data buffer

					// Allocate space for the buffer.
					LPBYTE value = (LPBYTE) malloc(valuelen);

					// Retrieve the type and value for the specified name associated with an open registry
					// key.
					res = RegQueryValueEx(hkey,						// handle to key
										name->GetValue().c_str(),	// value name
										NULL,						// reserved
										&type,						// type buffer
										value,						// data buffer
										&valuelen);					// size of data buffer

					if (res == ERROR_FILE_NOT_FOUND || res == ERROR_BAD_PATHNAME) {
						
						item->AppendElement(new ItemEntity("name", name->GetValue(), OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_DOES_NOT_EXIST));
						item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);

					} else if (res != ERROR_SUCCESS) {

						string systemErrMsg = WindowsCommon::GetErrorMessage(res);

						char errorCodeBuffer[20];
						_ltoa(res, errorCodeBuffer, 20);
						
						string errorMessage = "";
						errorMessage.append("Unable to get type and value for the specified name: '");
						errorMessage.append(name->GetValue());
						errorMessage.append("'.  Error Code - ");
						errorMessage.append(errorCodeBuffer);
						errorMessage.append(" - " + systemErrMsg);
						
						item->AppendMessage(new OvalMessage(errorMessage, OvalEnum::LEVEL_ERROR));
						item->AppendElement(new ItemEntity("name", name->GetValue(), OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
						item->AppendElement(new ItemEntity("type",  "", OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_ERROR));
						item->AppendElement(new ItemEntity("value",  "", OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_ERROR));
						item->SetStatus(OvalEnum::STATUS_ERROR);

					//	Only call RetrieveInfo() if res == ERROR_SUCCESS
					} else {

						// now add the name entity.
						item->AppendElement(new ItemEntity("name", name->GetValue(), OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
						item->SetStatus(OvalEnum::STATUS_EXISTS);
						// We now have all the info we need.
						this->RetrieveInfo(hive->GetValue(), key->GetValue(), name->GetValue(), type, value, valuelen, item);
					}

					free(value);
				}
				RegCloseKey(hkey);
			}
		} 
	}

	return item;
}

HKEY RegistryProbe::GetRootKey(string hiveIn) {

	if (hiveIn.compare("HKEY_LOCAL_MACHINE") == 0) {
        return HKEY_LOCAL_MACHINE;
    } else if (hiveIn.compare("HKEY_USERS") == 0) {
        return HKEY_USERS;
    } else if (hiveIn.compare("HKEY_CURRENT_USER") == 0) {
        return HKEY_CURRENT_USER;
    } else if (hiveIn.compare("HKEY_CURRENT_CONFIG") == 0) {
        return HKEY_CURRENT_CONFIG;
    } else if (hiveIn.compare("HKEY_CLASSES_ROOT") == 0) {
        return HKEY_CLASSES_ROOT;
    } else {
		return NULL;
    }
}

void RegistryProbe::KeyPatternMatch(ItemEntity* hive, string pattern, ItemEntityVector* keys, bool isRegex) {

	string patternOut= "";
	string constantPortionOut= "";
	this->myMatcher->GetConstantPortion(pattern, "\\", &patternOut, &constantPortionOut);
	// Remove extra slashes
	constantPortionOut = myMatcher->RemoveExtraSlashes(constantPortionOut);

	if(patternOut.compare("") != 0) {
		try {
			myMatcher->Reset();

			//	Call search function with
			//	the constant portion found as the key and
			//	the entire pattern as the pattern
			this->GetMatchingKeys(hive->GetValue(), constantPortionOut, pattern, keys, isRegex);

		} catch(REGEXException ex) {

			if(ex.GetSeverity() == ERROR_WARN) {
				string pcreMsg = "";
				pcreMsg.append("Registry Keys Probe Warning - while searching for matching keys:\n");
				pcreMsg.append("\t" + ex.GetErrorMessage());
				Log::Debug(pcreMsg);
			} else {
				throw;
			}
		}

	} else {

		//	There are no pattern matching chars treat this as a normal path 
		//	after removing the double '\'
		pattern = myMatcher->RemoveExtraSlashes(pattern);
		if(this->KeyExists(hive->GetValue(), pattern)) {
			ItemEntity* entity = this->CreateItemEntity(NULL);
			entity->SetName("key");
			entity->SetValue(pattern);
			keys->push_back(entity);
		}
	}
}

void RegistryProbe::RetrieveInfo(string hiveIn, string keyIn, string nameIn, 
									  DWORD typeIn, LPBYTE valueIn, DWORD valuelenIn, Item* item) {

	switch (typeIn) {

		case REG_BINARY:
				{
				item->AppendElement(new ItemEntity("type",  "reg_binary", OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));

				// The buffer must be three bytes long, two bytes for each hex charater in the
				// binary data, and one byte for the terminating NULL character.
				char binaryBuf[3];

				// Loop through each hex character.  Make sure the buffer is NULL terminated.
				// Also make sure 0 is in the form 00, and 1 is 01, etc.
				string value = "";
				for (DWORD x=0; x<valuelenIn; x++) {

					ZeroMemory(binaryBuf, sizeof(binaryBuf));
					_snprintf(binaryBuf, sizeof(binaryBuf)-1, "%x", valueIn[x]);
					binaryBuf[sizeof(binaryBuf)-1] = '\0';
					if (strlen(binaryBuf) == 1) 
						value.append("0");
					value.append(binaryBuf);

					// add a space only if not at the end of the string
					if(x < (valuelenIn - 1)) {
						value.append(" ");
					}
				}
				item->AppendElement(new ItemEntity("value",  value, OvalEnum::DATATYPE_BINARY, false, OvalEnum::STATUS_EXISTS));

				break;
			}

		case REG_DWORD:
			{
				item->AppendElement(new ItemEntity("type",  "reg_dword", OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));

				// The dwordBuf is 12 bytes since the max DWORD (2,147,483,647) is 10 characters
				// long.  Also add a byte for a possible negative sign and a byte for the
				// terminating NULL character.
				char dwordBuf[12];

				ZeroMemory(dwordBuf, sizeof(dwordBuf));
				_snprintf(dwordBuf, sizeof(dwordBuf)-1, "%d", *((DWORD *)valueIn));
				dwordBuf[sizeof(dwordBuf)-1] = '\0';
				item->AppendElement(new ItemEntity("value",  dwordBuf, OvalEnum::DATATYPE_INTEGER, false, OvalEnum::STATUS_EXISTS));

				break;
			}

		case REG_EXPAND_SZ:
			{
				item->AppendElement(new ItemEntity("type",  "reg_expand_sz", OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));

				char expandBuf[3];
				string strValue = "";
				for (DWORD x=0; x<(valuelenIn); x++) {

					ZeroMemory(expandBuf, sizeof(expandBuf));
					_snprintf(expandBuf, sizeof(expandBuf)-1, "%C", valueIn[x]);
					expandBuf[sizeof(expandBuf)-1] = '\0';
					strValue.append(expandBuf);
				}
				item->AppendElement(new ItemEntity("value",  strValue, OvalEnum::DATATYPE_INTEGER, false, OvalEnum::STATUS_EXISTS));

				break;
			}

		case REG_MULTI_SZ:
			{
				item->AppendElement(new ItemEntity("type",  "reg_multi_sz", OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));

				char multiszBuf[3];

				// Loop through each character.  Make sure the buffer is NULL terminated.
				// MULTISZ data is an array of null-terminated strings, terminated by two null
				// characters.  Therefore, the loop goes to (valuelenIn - 2) since we can skip
				// the last two characters.  This keeps an extra bar from beeing added to the
				// end of the valueString.  A terminating NULL charater and will be
				// automatically replaced during the append method.

				// NOTE: valuelenIn can be less than 2.  When this is the case, (valuelenIn-2)
				// becomes a very high DWORD.  This is because there are no negative numbers
				// for DWORDS.  Make sure we guard against this by setting valuelenIn = 2 so
				// (valuelenIn-2) = 0 and the loop never runs.

				if (valuelenIn < 2) 
					valuelenIn = 2;

				string strValue = "";
				bool addedValue = false;
				for (DWORD x=0; x<(valuelenIn-2); x++) {

					ZeroMemory(multiszBuf, sizeof(multiszBuf));
					_snprintf(multiszBuf, sizeof(multiszBuf)-1, "%C", valueIn[x]);
					multiszBuf[sizeof(multiszBuf)-1] = '\0';
					if (multiszBuf[0] == '\0') {
						item->AppendElement(new ItemEntity("value",  strValue, OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));
						strValue = "";
						addedValue = true;
					} else {
						strValue.append(multiszBuf);
					}
				}

				if(!addedValue) {
					item->AppendElement(new ItemEntity("value",  "", OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));
				}

				break;
			}

		case REG_SZ:
			{
				item->AppendElement(new ItemEntity("type",  "reg_sz", OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));

				char strBuf[3];
				string strValue = "";
				for (DWORD x=0; x<(valuelenIn); x++) {

					ZeroMemory(strBuf, sizeof(strBuf));
					_snprintf(strBuf, sizeof(strBuf)-1, "%C", valueIn[x]);
					strBuf[sizeof(strBuf)-1] = '\0';
					strValue.append(strBuf);
				}
				item->AppendElement(new ItemEntity("value",  strValue, OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));

				break;
			}

		default:
			{
				item->AppendMessage(new OvalMessage("Error: Unable to determine the type and value of the registry key.", OvalEnum::LEVEL_ERROR));
				item->AppendElement(new ItemEntity("type",  "", OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_ERROR));
				item->AppendElement(new ItemEntity("value",  "", OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_ERROR));
				
				break;
			}
	}
}

ItemEntityVector* RegistryProbe::GetHives(ObjectEntity* hive) {

	ItemEntityVector* hives = new ItemEntityVector();

	// does this hive use variables?
	if(hive->GetVarRef() == NULL) {
		
		// proceed based on operation
		if(hive->GetOperation() == OvalEnum::OPERATION_EQUALS) {
			ItemEntity* tmp = this->CreateItemEntity(hive);
			tmp->SetValue(hive->GetValue());
			hives->push_back(tmp);

		} else if(hive->GetOperation() == OvalEnum::OPERATION_NOT_EQUAL) {
			// turn the provided hive value into a negative pattern match
			// then get all that match the pattern
			this->GetMatchingHives(hive->GetValue(), hives, false);

		} else if(hive->GetOperation() == OvalEnum::OPERATION_PATTERN_MATCH) {
			this->GetMatchingHives(hive->GetValue(), hives);
		}		

	} else {

		// Get all hives
		ItemEntityVector allHives;
		this->GetMatchingHives(".*", &allHives);

		// loop through all hives on the system
		// only keep hives that match operation and value and var check
		ItemEntityVector::iterator iterator;
		for(iterator = allHives.begin(); iterator != allHives.end(); iterator++) {
			
			if(hive->Analyze((*iterator)) == OvalEnum::RESULT_TRUE) {
				hives->push_back((*iterator));
			}
		}
	}

	return hives;
}

ItemEntityVector* RegistryProbe::GetKeys(ObjectEntity* key, ItemEntity* hive) {

	ItemEntityVector* keys = new ItemEntityVector();

	// does this key use variables?
	if(key->GetVarRef() == NULL) {
		
		// proceed based on operation
		if(key->GetOperation() == OvalEnum::OPERATION_EQUALS) {
			ItemEntity* tmp = this->CreateItemEntity(key);
			tmp->SetValue(key->GetValue());
			keys->push_back(tmp);

		} else if(key->GetOperation() == OvalEnum::OPERATION_NOT_EQUAL) {
			// turn the provided key value into a negative pattern match
			// then get all that match the pattern
			this->GetMatchingKeys(hive->GetValue(), "", key->GetValue(), keys, false);

		} else if(key->GetOperation() == OvalEnum::OPERATION_PATTERN_MATCH) {
			this->KeyPatternMatch(hive, key->GetValue(), keys);
		}		

	} else {

		// Get all keys
		ItemEntityVector allKeys;

		if(key->GetOperation() == OvalEnum::OPERATION_EQUALS) {
			// in the case of equals simply loop through all the 
			// variable values and add them to the set of all keys
			// if they exist on the system
			VariableValueVector::iterator iterator;
			for(iterator = key->GetVarRef()->GetValues()->begin(); iterator != key->GetVarRef()->GetValues()->end(); iterator++) {
				
				if(this->KeyExists(hive->GetValue(),(*iterator)->GetValue())) {
					ItemEntity* tmp = this->CreateItemEntity(key);
					tmp->SetValue((*iterator)->GetValue());
					allKeys.push_back(tmp);
				}
			}

		} else {
            this->KeyPatternMatch(hive, ".*", &allKeys);
		}
	
		// loop through all keys on the system
		// only keep keys that match operation and value and var check
		ItemEntityVector::iterator it;
		for(it = allKeys.begin(); it != allKeys.end(); it++) {
			
			if(key->Analyze((*it)) == OvalEnum::RESULT_TRUE) {
				keys->push_back((*it));
			}
		}
	}

	return keys;
}

ItemEntityVector* RegistryProbe::GetNames(ObjectEntity* name, ItemEntity* hive, ItemEntity* key) {

	ItemEntityVector* names = new ItemEntityVector();

	// does this name use variables?
	if(name->GetVarRef() == NULL) {
		
		// proceed based on operation
		if(name->GetOperation() == OvalEnum::OPERATION_EQUALS) {
			ItemEntity* tmp = this->CreateItemEntity(name);
			tmp->SetValue(name->GetValue());
			names->push_back(tmp);

		} else if(name->GetOperation() == OvalEnum::OPERATION_NOT_EQUAL) {
			// turn the provided key value into a negative pattern match
			// then get all that match the pattern
			this->GetMatchingNames(hive->GetValue(), key->GetValue(), name->GetValue(), names, false);

		} else if(name->GetOperation() == OvalEnum::OPERATION_PATTERN_MATCH) {
			this->GetMatchingNames(hive->GetValue(), key->GetValue(), name->GetValue(), names);
		}		

	} else {

		// Get all keys
		ItemEntityVector allNames;

		if(name->GetOperation() == OvalEnum::OPERATION_EQUALS) {
			// in the case of equals simply loop through all the 
			// variable values and add them to the set of all names
			// if they exist on the system
			VariableValueVector::iterator iterator;
			for(iterator = name->GetVarRef()->GetValues()->begin(); iterator != name->GetVarRef()->GetValues()->end(); iterator++) {
				
				if(this->NameExists(hive->GetValue(), key->GetValue(), (*iterator)->GetValue())) {
					ItemEntity* tmp = this->CreateItemEntity(name);
					tmp->SetValue((*iterator)->GetValue());
					allNames.push_back(tmp);
				}
			}

		} else {
			this->GetMatchingNames(hive->GetValue(), key->GetValue(), ".*", &allNames);
		}
	
		// loop through all hives on the system
		// only keep hives that match operation and value and var check
		ItemEntityVector::iterator it;
		for(it = allNames.begin(); it != allNames.end(); it++) {
			
			if(name->Analyze((*it)) == OvalEnum::RESULT_TRUE) {
				names->push_back((*it));
			}
		}
	}

	return names;
}

bool RegistryProbe::KeyExists(string hive, string key) {

	bool exists = false;
	HKEY hkey;
	DWORD parse_depth = 0;
	LONG res;

	// Check hive
	HKEY rootKey = this->GetRootKey(hive);		
	if(rootKey != NULL) {
		res = RegOpenKeyEx(rootKey,					// handle to open hive
						key.c_str(),				// subkey name
						0,							// reserved
						KEY_READ,					// security access mask
						&hkey);						// pointer to open key

		if (res != ERROR_SUCCESS) {
			if (res == ERROR_FILE_NOT_FOUND) {
				exists = false;

			} else if (res == ERROR_INVALID_HANDLE) {

				string errorMessage = "";
				errorMessage.append("(RegistryProbe) The handle for the registry key '");
				errorMessage.append(key);
				errorMessage.append("' is not valid.");							
				throw ProbeException(errorMessage);

			} else {
				
				string systemErrMsg = WindowsCommon::GetErrorMessage(res);
				char errorCodeBuffer[20];
				_ltoa(res, errorCodeBuffer, 20);
				string errorMessage = "";
				errorMessage.append("Error: Unable to check existence of registry key '");
				errorMessage.append(key);
				errorMessage.append("'.  Error Code - ");
				errorMessage.append(errorCodeBuffer);
				errorMessage.append(" - " + systemErrMsg);
				throw ProbeException(errorMessage);
			}		
		} else {
			RegCloseKey(hkey);
			exists = true;				
		}			
	}

	return exists;
}

bool RegistryProbe::NameExists(string hive, string key, string name) {

	bool exists = false;
	HKEY hkey;
	DWORD parse_depth = 0;
	LONG res;

	// Check hive
	HKEY rootKey = this->GetRootKey(hive);		
	if(rootKey != NULL) {

			res = RegOpenKeyEx(rootKey,					// handle to open hive
							key.c_str(),				// subkey name
							0,							// reserved
							KEY_READ,					// security access mask
							&hkey);						// pointer to open key

			if (res != ERROR_SUCCESS) {
				if (res == ERROR_FILE_NOT_FOUND) {
				exists = false;

			} else if (res == ERROR_INVALID_HANDLE) {

				string errorMessage = "";
				errorMessage.append("(RegistryProbe) The handle for the registry key '");
				errorMessage.append(key);
				errorMessage.append("' is not valid.");							
				throw ProbeException(errorMessage);

			} else {
				
				string systemErrMsg = WindowsCommon::GetErrorMessage(res);
				char errorCodeBuffer[20];
				_ltoa(res, errorCodeBuffer, 20);
				string errorMessage = "";
				errorMessage.append("Error: Unable to check existence of registry key '");
				errorMessage.append(key);
				errorMessage.append("'.  Error Code - ");
				errorMessage.append(errorCodeBuffer);
				errorMessage.append(" - " + systemErrMsg);
				throw ProbeException(errorMessage);
			}	

		} else {
			
			DWORD type = 0;
			DWORD valuelen = 0;

			// Determine how big the buffer must be to store the data.  By specifying NULL for the
			// buffer size parameter, the function returns the value ERROR_MORE_DATA, and stores
			// the required buffer size, in bytes, into valuelen.
			res = RegQueryValueEx(hkey,						// handle to key
								name.c_str(),				// value name
								NULL,						// reserved
								NULL,						// type buffer
								NULL,						// data buffer
								&valuelen);					// size of data buffer

			// Allocate space for the buffer.
			LPBYTE value = (LPBYTE) malloc(valuelen);

			// Retrieve the type and value for the specified name associated with an open registry
			// key.
			res = RegQueryValueEx(hkey,						// handle to key
								name.c_str(),				// value name
								NULL,						// reserved
								&type,						// type buffer
								value,						// data buffer
								&valuelen);					// size of data buffer

			if (res == ERROR_FILE_NOT_FOUND) {

				exists = false;

			} else if (res != ERROR_SUCCESS) {

				string systemErrMsg = WindowsCommon::GetErrorMessage(res);
				char errorCodeBuffer[20];
				_ltoa(res, errorCodeBuffer, 20);
				string errorMessage = "";
				errorMessage.append("Unable to get values for the specified name: '");
				errorMessage.append(name);
				errorMessage.append("'.  Error Code - ");
				errorMessage.append(errorCodeBuffer);
				errorMessage.append(" - " + systemErrMsg);
				throw ProbeException(errorMessage);

			} else {
				exists = true;
			}

			free(value);
			RegCloseKey(hkey);
		}
	}

	return exists;
}
