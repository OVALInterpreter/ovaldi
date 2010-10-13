//
//
//****************************************************************************************//
// Copyright (c) 2002-2010, The MITRE Corporation
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

#include "MetabaseProbe.h"

//****************************************************************************************//
//								MetabaseProbe Class									      //
//****************************************************************************************//
MetabaseProbe* MetabaseProbe::instance = NULL;
const char MetabaseProbe::MetabaseSeparator = '/';
const string MetabaseProbe::MetabaseSeparatorStr = "/";

MetabaseProbe::MetabaseProbe() {
	MetabaseProbe::InitializeIMSAdminBaseInterface();
}

MetabaseProbe::~MetabaseProbe() {
	instance = NULL;
	MetabaseProbe::CloseIMSAdminBaseInterface();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* MetabaseProbe::Instance() {
	if ( instance == NULL ) {
		instance = new MetabaseProbe();
	}

	return instance;
}

ItemVector* MetabaseProbe::CollectItems ( Object* object ) {
	ObjectEntity* keyEntity = object->GetElementByName ( "key" );
	ObjectEntity* idEntity = object->GetElementByName ( "id" );
	ItemVector* collectedItems = new ItemVector();
	StringSet* keys = this->GetKeys ( keyEntity );

	if ( keys->size() > 0 ) {
		for ( StringSet::iterator key = keys->begin() ; key != keys->end() ; key++ ) {
			if ( MetabaseProbe::KeyExists ( *key ) ) {
				if ( ! ( idEntity->GetNil() ) ) {
					StringSet* ids = this->GetIds ( *key, idEntity );

					if ( ids->size() > 0 ) {
						for ( StringSet::iterator id = ids->begin() ; id != ids->end() ; id++ ) {
							Item* item = NULL;
							item = this->GetMetabaseItem ( *key, *id );

							if ( item != NULL ) {
								collectedItems->push_back ( item );
							}
						}

						StringVector dneIds;
						if ( MetabaseProbe::ReportIdDoesNotExist ( *key, idEntity, &dneIds ) ) {
							Item* item = NULL;
							for ( StringVector::iterator iterator = dneIds.begin(); iterator != dneIds.end(); iterator++ ) {
								item = this->CreateItem();
								item->SetStatus ( OvalEnum::STATUS_DOES_NOT_EXIST );
								item->AppendElement ( new ItemEntity ( "key",*key, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS ) );
								item->AppendElement ( new ItemEntity ( "id", ( *iterator ), OvalEnum::DATATYPE_INTEGER, true, OvalEnum::STATUS_DOES_NOT_EXIST, idEntity->GetNil() ) );
								collectedItems->push_back ( item );
							}
						}

						delete ids;
					}
				} else {
					Item* item = NULL;
					item = this->CreateItem();
					item->SetStatus ( OvalEnum::STATUS_EXISTS );
					item->AppendElement ( new ItemEntity ( "key", *key, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS ) );
					item->AppendElement ( new ItemEntity ( "id", "", OvalEnum::DATATYPE_INTEGER, true, OvalEnum::STATUS_EXISTS, true ) );
					collectedItems->push_back ( item );
				}
			}
		}

		StringVector dneKeys;

		if ( MetabaseProbe::ReportKeyDoesNotExist ( keyEntity, &dneKeys ) ) {
			Item* item = NULL;
			StringVector::iterator iterator;

			for ( iterator = dneKeys.begin(); iterator != dneKeys.end(); iterator++ ) {
				item = this->CreateItem();
				item->SetStatus ( OvalEnum::STATUS_DOES_NOT_EXIST );
				item->AppendElement ( new ItemEntity ( "key", ( *iterator ), OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_DOES_NOT_EXIST ) );
				collectedItems->push_back ( item );
			}
		}
	}

	delete keys;
	return collectedItems;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
Item* MetabaseProbe::CreateItem() {

	Item* item = new Item ( 0,
	                        "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows",
	                        "win-sc",
	                        "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows windows-system-characteristics-schema.xsd",
	                        OvalEnum::STATUS_ERROR,
	                        "metabase_item" );

	return item;
}

void MetabaseProbe::InitializeIMSAdminBaseInterface() {
	HRESULT hResult = CoInitialize ( NULL );

	if ( hResult != S_OK ) {
		throw ProbeException ( "Error: The COM library could not be initialized properly." );
	}

	hResult = CoCreateInstance ( CLSID_MSAdminBase, NULL, CLSCTX_ALL, IID_IMSAdminBase, ( void** ) &metabase );

	if ( hResult != S_OK ) {
		throw ProbeException ( "Error: The IMSAdminBase object class could not be created successfully." );
	}
}

void MetabaseProbe::CloseIMSAdminBaseInterface() {
	CoUninitialize();
	metabase = NULL;
}

StringSet* MetabaseProbe::GetKeys ( ObjectEntity* keyEntity ) {
	StringSet* keys = new StringSet();

	if ( keyEntity->GetVarRef() == NULL && keyEntity->GetOperation() == OvalEnum::OPERATION_NOT_EQUAL ) {
		this->GetKeysForPattern ( MetabaseProbe::MetabaseSeparatorStr, keyEntity->GetValue(), keys, false );
	} else if ( keyEntity->GetVarRef() == NULL && keyEntity->GetOperation() == OvalEnum::OPERATION_PATTERN_MATCH ) {
		string patternOut = "";
		string constPortion = "";
		string startKey = MetabaseProbe::MetabaseSeparatorStr;

		if ( ! ( ( keyEntity->GetValue() ).empty() ) && ( keyEntity->GetValue() ) [0] == '^' ) {
			REGEX regex;
			regex.GetConstantPortion ( keyEntity->GetValue(), MetabaseProbe::MetabaseSeparator, &patternOut, &constPortion );
			constPortion = regex.RemoveExtraSlashes ( constPortion );
		}

		if ( constPortion.compare ( "" ) != 0 && patternOut.compare ( "" ) != 0 ) {
			if ( MetabaseProbe::KeyExists ( constPortion ) ) {
				this->GetKeysForPattern ( constPortion, keyEntity->GetValue(), keys, true );
			}
		} else if ( constPortion.compare ( "" ) == 0 ) {
			this->GetKeysForPattern ( MetabaseProbe::MetabaseSeparatorStr, keyEntity->GetValue(), keys, true );
		} else if ( patternOut.compare ( "" ) == 0 ) {
			if ( MetabaseProbe::KeyExists ( constPortion ) ) {
				keys->insert ( constPortion );
			}
		}
	} else {
		StringSet* allKeys = new StringSet();

		if ( keyEntity->GetOperation() == OvalEnum::OPERATION_EQUALS ) {
			StringVector values;
			keyEntity->GetEntityValues ( values );

			for ( StringVector::iterator it = values.begin() ; it != values.end() ; it++ ) {
				allKeys->insert ( *it );
			}
		} else {
			this->GetKeysForPattern ( MetabaseProbe::MetabaseSeparatorStr, ".*", allKeys, true );
		}

		ItemEntity* tmp = this->CreateItemEntity ( keyEntity );

		for ( StringSet::iterator it = allKeys->begin() ; it != allKeys->end() ; it++ ) {
			tmp->SetValue ( ( *it ) );

			if ( keyEntity->Analyze ( tmp ) == OvalEnum::RESULT_TRUE ) {
				keys->insert ( ( *it ) );
			}
		}
	}

	return keys;
}

bool MetabaseProbe::KeyExists ( string keyStr ) {
	HRESULT hResult;
	METADATA_HANDLE handle;
	bool exists = false;

	if ( ( hResult = metabase->OpenKey ( METADATA_MASTER_ROOT_HANDLE, WindowsCommon::StringToWide ( keyStr ), METADATA_PERMISSION_READ, 10000, &handle ) ) == S_OK ) {
		exists = true;
	} else if ( hResult == HRESULT_FROM_WIN32 ( ERROR_PATH_NOT_FOUND ) ) {
		// Do nothing since the key was not found.
	} else {
		if ( hResult == HRESULT_FROM_WIN32 ( ERROR_PATH_BUSY ) ) {
			Log::Message ( "Error: The key '"+keyStr+"' exists, however, it could not be accessed because it is currently in use.  As a result, the existence for this key is being set to false." );
		}

		if ( hResult == E_INVALIDARG ) {
			Log::Message ( "Error: The method IMSAdminBase->OpenKey() failed because of an invalid parameter for key '"+keyStr+"'." );
		}
	}

	metabase->CloseKey ( handle );
	return exists;
}

void MetabaseProbe::GetKeysForPattern ( string keyStr, string regexStr, StringSet *keys, bool isRegex ) {
	if ( ( ( keyStr.empty() == true ) || ( keyStr == "" ) ) ) {
		return;
	}

	if ( keyStr[keyStr.length()-1] != MetabaseProbe::MetabaseSeparator ) {
		keyStr.append ( 1, MetabaseProbe::MetabaseSeparator );
	}

	if ( this->IsMatch ( regexStr , keyStr , isRegex ) ) {
		keys->insert ( keyStr );
	}

	HRESULT hResult;
	LPWSTR name = ( LPWSTR ) malloc ( sizeof ( WCHAR ) * MAX_PATH );
	DWORD index = 0;

	while ( ( hResult=metabase->EnumKeys ( METADATA_MASTER_ROOT_HANDLE, WindowsCommon::StringToWide ( keyStr ), name, index ) ) == S_OK ) {
		string nameStr = WindowsCommon::UnicodeToAsciiString ( name );
		string newKeyStr = keyStr;
		newKeyStr.append ( nameStr );
		this->GetKeysForPattern ( newKeyStr, regexStr, keys, isRegex );
		++index;
	}

	if ( hResult == E_ACCESSDENIED ) {
		Log::Message ( "Error: Access to the key '"+keyStr+"' has been denied.  Please make sure that the interpreter is being run with Administrator privileges." );
	}

	if ( hResult == E_INVALIDARG ) {
		Log::Message ( "Error: The method IMSAdminBase->EnumKeys() failed because of an invalid parameter for key '"+keyStr+"'." );
	}

	if ( name != NULL ) {
		free ( name );
		name = NULL;
	}
}

StringSet* MetabaseProbe::GetIds ( string keyStr, ObjectEntity* idEntity ) {
	StringSet* ids = new StringSet();
	StringSet* allIds = NULL;

	if ( idEntity->GetOperation() == OvalEnum::OPERATION_EQUALS ) {
		StringVector values;
		allIds = new StringSet();
		idEntity->GetEntityValues ( values );

		for ( StringVector::iterator it = values.begin() ; it != values.end() ; it++ ) {
			allIds->insert ( *it );
		}
	} else {
		allIds = MetabaseProbe::GetAllIds ( keyStr );
	}

	ItemEntity* temp = this->CreateItemEntity ( idEntity );

	for ( StringSet::iterator it = allIds->begin() ; it != allIds->end() ; it++ ) {
		temp->SetValue ( *it );

		if ( idEntity->Analyze ( temp ) == OvalEnum::RESULT_TRUE ) {
			ids->insert ( *it );
		}
	}

	delete allIds;
	return ids;
}

bool MetabaseProbe::IdExists ( string keyStr,string idStr ) {
	bool exists = false;
	StringSet* ids = MetabaseProbe::GetAllIds ( keyStr );

	if ( ids->find ( idStr ) != ids->end() ) {
		exists = true;
	}

	delete ids;
	return exists;
}

StringSet* MetabaseProbe::GetAllIds ( string keyStr ) {
	HRESULT hResult;
	DWORD count = 0;
	DWORD size = 0;
	DWORD set = 0;
	DWORD required = 0;
	unsigned char* buffer = NULL;
	StringSet* ids = new StringSet();

	if ( ( hResult = metabase->GetAllData ( METADATA_MASTER_ROOT_HANDLE, WindowsCommon::StringToWide ( keyStr ), METADATA_NO_ATTRIBUTES, ALL_METADATA, ALL_METADATA, &count, &set, size, buffer, &required ) ) == HRESULT_FROM_WIN32 ( ERROR_INSUFFICIENT_BUFFER ) ) {
		size = required;
		required = 0;
		buffer = ( unsigned char* ) malloc ( size );

		if ( ( hResult = metabase->GetAllData ( METADATA_MASTER_ROOT_HANDLE, WindowsCommon::StringToWide ( keyStr ), METADATA_NO_ATTRIBUTES, ALL_METADATA, ALL_METADATA, &count, &set, size, buffer, &required ) ) == S_OK ) {
			METADATA_GETALL_RECORD* records = ( METADATA_GETALL_RECORD* ) buffer;

			for ( unsigned int i = 0 ; i < count ; i++ ) {
				ids->insert ( Common::ToString ( records[i].dwMDIdentifier ) );
			}
		} else {
			if ( hResult == HRESULT_FROM_WIN32 ( ERROR_INSUFFICIENT_BUFFER ) ) {
				Log::Message ( "Error: The method IMSAdminBase->GetAllData() failed because a insufficient buffer was provided for key '"+keyStr+"'." );
			}

			if ( hResult == E_ACCESSDENIED ) {
				Log::Message ( "Error: Access to the key '"+keyStr+"' has been denied.  Please make sure that the interpreter is being run with Administrator privileges." );
			}

			if ( hResult == E_INVALIDARG ) {
				Log::Message ( "Error: The method IMSAdminBase->GetAllData() failed because of an invalid parameter for key '"+keyStr+"'." );
			}

			if ( hResult == HRESULT_FROM_WIN32 ( ERROR_PATH_NOT_FOUND ) ) {
				Log::Message ( "Error: The key '"+keyStr+"' could not be found." );
			}
		}

		if ( buffer != NULL ) {
			free ( buffer );
			buffer = NULL;
		}
	} else {
		if ( hResult == E_ACCESSDENIED ) {
			Log::Message ( "Error: Access to the key '"+keyStr+"' has been denied.  Please make sure that the interpreter is being run with Administrator privileges." );
		}

		if ( hResult == E_INVALIDARG ) {
			Log::Message ( "Error: The method IMSAdminBase->GetAllData() failed because of an invalid parameter for key '"+keyStr+"'." );
		}

		if ( hResult == HRESULT_FROM_WIN32 ( ERROR_PATH_NOT_FOUND ) ) {
			Log::Message ( "Error: The key '"+keyStr+"' could not be found." );
		}
	}

	return ids;
}

Item* MetabaseProbe::GetMetabaseItem ( string keyStr, string idStr ) {
	HRESULT hResult;
	METADATA_RECORD record;
	Item* item = NULL;
	unsigned long size = 0;
	record.dwMDIdentifier = strtoul ( idStr.c_str(), NULL, 0 );
	record.dwMDAttributes = METADATA_NO_ATTRIBUTES;
	record.dwMDUserType = ALL_METADATA;
	record.dwMDDataType = ALL_METADATA;
	record.dwMDDataLen = 0;
	record.pbMDData = NULL;

	if ( ( hResult = metabase->GetData ( METADATA_MASTER_ROOT_HANDLE, WindowsCommon::StringToWide ( keyStr ), &record, &size ) ) == HRESULT_FROM_WIN32 ( ERROR_INSUFFICIENT_BUFFER ) ) {
		record.dwMDDataLen = size;
		record.pbMDData = ( unsigned char* ) malloc ( record.dwMDDataLen );

		if ( ( hResult = metabase->GetData ( METADATA_MASTER_ROOT_HANDLE, WindowsCommon::StringToWide ( keyStr ), &record, &size ) ) == S_OK ) {
			item = this->CreateItem();
			item->SetStatus ( OvalEnum::STATUS_EXISTS );
			item->AppendElement ( new ItemEntity ( "key", keyStr, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS ) );
			item->AppendElement ( new ItemEntity ( "id", idStr, OvalEnum::DATATYPE_INTEGER, true, OvalEnum::STATUS_EXISTS ) );
			item->AppendElement ( new ItemEntity ( "name", "", OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_NOT_COLLECTED ) );
			string userType = MetabaseProbe::GetUserType ( record.dwMDUserType );
			string dataType = MetabaseProbe::GetDataType ( record.dwMDDataType );
			item->AppendElement ( new ItemEntity ( "user_type", userType, OvalEnum::DATATYPE_STRING, false, ( userType.compare ( "" ) == 0 ) ? OvalEnum::STATUS_DOES_NOT_EXIST:OvalEnum::STATUS_EXISTS ) );
			item->AppendElement ( new ItemEntity ( "data_type", dataType, OvalEnum::DATATYPE_STRING, false, ( dataType.compare ( "" ) == 0 ) ? OvalEnum::STATUS_DOES_NOT_EXIST:OvalEnum::STATUS_EXISTS ) );
			StringVector* data = MetabaseProbe::GetDataValues ( record.pbMDData, record.dwMDDataType, record.dwMDDataLen );

			if ( data->size() > 0 ) {
				for ( StringVector::iterator it = data->begin() ; it != data->end() ; it++ ) {
					item->AppendElement ( new ItemEntity ( "data", *it, OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS ) );
				}
			} else {
				item->AppendElement ( new ItemEntity ( "data", "", OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_DOES_NOT_EXIST ) );
			}

			delete data;
		} else if ( hResult == MD_ERROR_DATA_NOT_FOUND ) {
			item = this->CreateItem();
			item->SetStatus ( OvalEnum::STATUS_DOES_NOT_EXIST );
			item->AppendElement ( new ItemEntity ( "key", keyStr, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS ) );
			item->AppendElement ( new ItemEntity ( "id", idStr, OvalEnum::DATATYPE_INTEGER, true, OvalEnum::STATUS_EXISTS ) );
			item->AppendElement ( new ItemEntity ( "name", "", OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_NOT_COLLECTED ) );
			item->AppendElement ( new ItemEntity ( "data", "", OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_DOES_NOT_EXIST ) );
		} else {
			if ( hResult == HRESULT_FROM_WIN32 ( ERROR_INSUFFICIENT_BUFFER ) ) {
				Log::Message ( "Error: The method IMSAdminBase->GetAllData() failed because a insufficient buffer was provided for key '"+keyStr+"'." );
			}

			if ( hResult == E_ACCESSDENIED ) {
				Log::Message ( "Error: Access to the key '"+keyStr+"' has been denied.  Please make sure that the interpreter is being run with Administrator privileges." );
			}

			if ( hResult == E_INVALIDARG ) {
				Log::Message ( "Error: The method IMSAdminBase->GetData() failed because of an invalid parameter for key '"+keyStr+"'." );
			}

			if ( hResult == HRESULT_FROM_WIN32 ( ERROR_PATH_NOT_FOUND ) ) {
				Log::Message ( "Error: The key '"+keyStr+"' could not be found." );
			}
		}

		if ( record.pbMDData != NULL ) {
			free ( record.pbMDData );
			record.pbMDData = NULL;
		}
	} else {
		if ( hResult == E_ACCESSDENIED ) {
			Log::Message ( "Error: Access to the key '"+keyStr+"' has been denied.  Please make sure that the interpreter is being run with Administrator privileges." );
		}

		if ( hResult == E_INVALIDARG ) {
			Log::Message ( "Error: The method IMSAdminBase->GetData() failed because of an invalid parameter for key '"+keyStr+"'." );
		}

		if ( hResult == HRESULT_FROM_WIN32 ( ERROR_PATH_NOT_FOUND ) ) {
			Log::Message ( "Error: The key '"+keyStr+"' could not be found." );
		}
	}

	return item;
}

string MetabaseProbe::GetUserType ( unsigned long type ) {
	switch ( type ) {
		case ASP_MD_UT_APP: {
				return "ASP_MD_UT_APP";
			}
		case IIS_MD_UT_FILE: {
				return "IIS_MD_UT_FILE";
			}
		case IIS_MD_UT_SERVER: {
				return "IIS_MD_UT_SERVER";
			}
		case IIS_MD_UT_WAM: {
				return "IIS_MD_UT_WAM";
			}
		default: {
				return "";
			}
	}
}

string MetabaseProbe::GetDataType ( unsigned long type ) {
	switch ( type ) {
		case BINARY_METADATA: {
				return "BINARY_METADATA";
			}
		case DWORD_METADATA: {
				return "DWORD_METADATA";
			}
		case EXPANDSZ_METADATA: {
				return "EXPANDSZ_METADATA";
			}
		case MULTISZ_METADATA: {
				return "MULTISZ_METADATA";
			}
		case STRING_METADATA: {
				return "STRING_METADATA";
			}
		default: {
				return "";
			}
	}
}

StringVector* MetabaseProbe::GetDataValues ( unsigned char* data, unsigned long type, unsigned long length ) {
	string value = "";
	StringVector* values = new StringVector();

	switch ( type ) {
		case BINARY_METADATA: {
				char buffer[3];

				for ( unsigned int i = 0 ; i < length; i++ ) {
					ZeroMemory ( buffer, 3 );
					_snprintf ( buffer, 2, "%02x", ( unsigned long * ) ( * ( data+i ) ) );
					value.append ( buffer );
				}

				values->push_back ( value );
				break;
			}
		case DWORD_METADATA: {
				char buffer[12];
				ZeroMemory ( buffer, sizeof ( buffer ) );
				_snprintf ( buffer, sizeof ( buffer )-1, "%u", * ( unsigned long * ) data );
				buffer[sizeof ( buffer )-1] = '\0';
				values->push_back ( buffer );
				break;
			}
		case EXPANDSZ_METADATA: {
				for ( unsigned int i = 0 ; i < length ; i++ ) {
					if ( ( * ( data+i ) ) != '\0' ) {
						value.push_back ( ( char ) ( * ( data+i ) ) );
					}
				}

				values->push_back ( value );
				break;
			}
		case MULTISZ_METADATA: {
				char buffer[3];

				for ( unsigned int i = 0 ; i < length-2; i=i+2 ) {
					ZeroMemory ( buffer, sizeof ( buffer ) );
					_snprintf ( buffer, sizeof ( buffer )-1, "%C", ( unsigned long * ) ( * ( data+i ) ) );
					buffer[sizeof ( buffer )-1] = '\0';

					if ( buffer[0] == '\0' ) {
						values->push_back ( value );
						value.clear();
					} else {
						value.append ( buffer );
					}
				}

				break;
			}
		case STRING_METADATA: {
				for ( unsigned int i = 0 ; i < length ; i++ ) {
					if ( ( * ( data+i ) ) != '\0' ) {
						value.push_back ( ( char ) ( * ( data+i ) ) );
					}
				}

				values->push_back ( value );
				break;
			}
		default: {
				break;
			}
	}

	return values;
}

bool MetabaseProbe::ReportKeyDoesNotExist ( ObjectEntity *keyEntity, StringVector* keysDne ) {
	bool result = false;

	if ( keyEntity->GetOperation() == OvalEnum::OPERATION_EQUALS ) {
		if ( keyEntity->GetVarRef() == NULL ) {
			if ( !this->KeyExists ( keyEntity->GetValue() ) ) {
				keysDne->push_back ( keyEntity->GetValue() );
				result = true;
			}
		} else {

			VariableValueVector::iterator iterator;

			for ( iterator = keyEntity->GetVarRef()->GetValues()->begin(); iterator != keyEntity->GetVarRef()->GetValues()->end(); iterator++ ) {
				if ( !this->KeyExists ( ( *iterator )->GetValue() ) ) {
					keysDne->push_back ( ( *iterator )->GetValue() );
					result = true;
				}
			}
		}
	}

	return  result;
}

bool MetabaseProbe::ReportIdDoesNotExist ( string keyStr, ObjectEntity *idEntity, StringVector* idsDne ) {
	bool result = false;

	if ( idEntity->GetOperation() == OvalEnum::OPERATION_EQUALS ) {
		if ( idEntity->GetVarRef() == NULL ) {
			if ( !this->IdExists ( keyStr, idEntity->GetValue() ) ) {
				idsDne->push_back ( idEntity->GetValue() );
				result = true;
			}
		} else {
			VariableValueVector::iterator iterator;

			for ( iterator = idEntity->GetVarRef()->GetValues()->begin(); iterator != idEntity->GetVarRef()->GetValues()->end(); iterator++ ) {
				if ( !this->IdExists ( keyStr, ( *iterator )->GetValue() ) ) {
					idsDne->push_back ( ( *iterator )->GetValue() );
					result = true;
				}
			}
		}
	}

	return  result;
}
