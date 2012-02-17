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

#include "RegistryFinder.h"

char RegistryFinder::keySeparator = '\\';

//****************************************************************************************//
//					                   RegKey Class        	    						  //	
//****************************************************************************************//

RegKey::RegKey() {
	this->hive = "";
	this->key = "";
	this->name = "";
}

RegKey::RegKey ( string hive, string key, string name ) {
	this->hive = hive;
	this->key = key;
	this->name = name;
}

string RegKey::GetHive() {
	return hive;
}

string RegKey::GetKey() {
	return key;
}

string RegKey::GetName() {
	return name;
}

void RegKey::SetHive ( string hive ) {
	this->hive = hive;
}

void RegKey::SetKey ( string key ) {
	this->key = key;
}

void RegKey::SetName ( string name ) {
	this->name = name;
}

//****************************************************************************************//
//					                RegistryFinder Class	    						  //	
//****************************************************************************************//

// 4100 is "unreferenced formal parameter".
// As a 32-bit app, for now, the view is hard-coded
// to the 32-bit view, so the view params are not
// used, and causes the warning.  This can be removed
// if/when the 32-bit app detects its OS environment
// and can switch its view.
#ifndef _WIN64
#  pragma warning(push)
#  pragma warning(disable:4100)
#endif
RegistryFinder::RegistryFinder(BitnessView view) {
	registryMatcher = new REGEX();

	bitnessView =
#ifdef _WIN64
		view;
#else
		BIT_32;
#endif

	regKeyObjectType =
#ifdef _WIN64
		(bitnessView == BIT_64 ?
			SE_REGISTRY_KEY : SE_REGISTRY_WOW64_32KEY);
#else
		SE_REGISTRY_KEY;
#endif
}

RegistryFinder::RegistryFinder(const string &viewStr) {
	registryMatcher = new REGEX();

	bitnessView =
#ifdef _WIN64
		behavior2view(viewStr);
#else
		BIT_32;
#endif

	regKeyObjectType =
#ifdef _WIN64
		(bitnessView == BIT_64 ?
			SE_REGISTRY_KEY : SE_REGISTRY_WOW64_32KEY);
#else
		SE_REGISTRY_KEY;
#endif
}
#ifndef _WIN64
#  pragma warning(pop)
#endif

RegistryFinder::~RegistryFinder() {
	delete registryMatcher;
}

RegKeyVector* RegistryFinder::SearchRegistries ( ObjectEntity* hiveEntity, ObjectEntity* keyEntity, ObjectEntity* nameEntity, BehaviorVector* behaviors ) {

    RegKeyVector* regKeyVector = new RegKeyVector();
    StringSet* hives = this->GetHives ( hiveEntity );
	
    for ( StringSet::iterator it1 = hives->begin(); it1 != hives->end(); it1++ ) {
        StringSet* keys = this->GetKeys ( *it1, keyEntity, behaviors );

        for ( StringSet::iterator it2 = keys->begin(); it2 != keys->end(); it2++ ) {
            StringSet* names = this->GetNames ( *it1, *it2, nameEntity );

            for ( StringSet::iterator it3 = names->begin(); it3 != names->end(); it3++ ) {
                RegKey* regKey = new RegKey ( *it1, *it2, *it3 );
                regKeyVector->push_back ( regKey );
            }

            names->clear();
            delete names;
        }

        keys->clear();
        delete keys;
    }

    hives->clear();
    delete hives;
    return regKeyVector;
}

StringSet* RegistryFinder::GetHives ( ObjectEntity* hiveEntity ) {
    StringSet* hives = new StringSet();

    if ( hiveEntity->GetVarRef() == NULL ) {
        // proceed based on operation
        if ( hiveEntity->GetOperation() == OvalEnum::OPERATION_EQUALS ) {
            if ( this->HiveExists ( hiveEntity->GetValue() ) ) {
                hives->insert ( hiveEntity->GetValue() );
            }

        } else if ( hiveEntity->GetOperation() == OvalEnum::OPERATION_NOT_EQUAL ) {
            // turn the provided hive value into a negative pattern match
            // then get all hives that match the pattern
            this->FindHives ( hiveEntity->GetValue(), hives, false );

        } else if ( hiveEntity->GetOperation() == OvalEnum::OPERATION_PATTERN_MATCH ) {
            this->FindHives ( hiveEntity->GetValue(), hives );
        }

    } else {
        StringSet* allHives = new StringSet();

        if ( hiveEntity->GetOperation() == OvalEnum::OPERATION_EQUALS ) {
            // in the case of equals simply loop through all the
            // variable values and add them to the set of all hives
            // if they exist on the system
            for ( VariableValueVector::iterator iterator = hiveEntity->GetVarRef()->GetValues()->begin(); iterator != hiveEntity->GetVarRef()->GetValues()->end(); iterator++ ) {
                if ( this->HiveExists ( ( *iterator )->GetValue() ) ) {
                    hives->insert ( ( *iterator )->GetValue() );
                }
            }

        } else {
            // for not equals and pattern match fetch all hives that match
            // any of the variable values. Then analyze each hive found on
            // the system against the variable values
            // loop through all variable values and call FindHives()
            VariableValueVector* values = hiveEntity->GetVariableValues();

            for ( VariableValueVector::iterator iterator = values->begin(); iterator != values->end(); iterator++ ) {
                if ( hiveEntity->GetOperation() == OvalEnum::OPERATION_NOT_EQUAL ) {
                    this->FindHives ( ( *iterator )->GetValue(), allHives, false );

                } else {
                    this->FindHives ( ( *iterator )->GetValue(), allHives, true );
                }
            }
        }

        // only keep hives that match operation and value and var check
        ItemEntity* tmp = new ItemEntity ( "hive", "", OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS );

        for ( StringSet::iterator it = allHives->begin(); it != allHives->end(); it++ ) {
            tmp->SetValue ( ( *it ) );

            if ( hiveEntity->Analyze ( tmp ) == OvalEnum::RESULT_TRUE ) {
                hives->insert ( ( *it ) );
            }
        }

        delete tmp;
        delete allHives;
    }

    return hives;
}

StringSet* RegistryFinder::GetKeys ( string hiveStr, ObjectEntity* keyEntity, BehaviorVector* behaviors ) {
    StringSet* keys = new StringSet();

    if ( keyEntity->GetNil() ) {
        keys = new StringSet();
        keys->insert ( "" );
        return keys;
    }

    if ( keyEntity->GetVarRef() == NULL ) {
        // proceed based on operation
        if ( keyEntity->GetOperation() == OvalEnum::OPERATION_EQUALS ) {
            if ( this->KeyExists ( hiveStr, keyEntity->GetValue() ) ) {
                keys->insert ( keyEntity->GetValue() );
            }

        } else if ( keyEntity->GetOperation() == OvalEnum::OPERATION_NOT_EQUAL ) {
            // turn the provided key value into a negative pattern match
            // then get all keys that match the pattern
            this->FindKeys ( hiveStr, keyEntity->GetValue(), keys, false );

        } else if ( keyEntity->GetOperation() == OvalEnum::OPERATION_PATTERN_MATCH ) {
            this->FindKeys ( hiveStr, keyEntity->GetValue(), keys );
        }

    } else {
        StringSet* allKeys = new StringSet();

        if ( keyEntity->GetOperation() == OvalEnum::OPERATION_EQUALS ) {
            // in the case of equals simply loop through all the
            // variable values and add them to the set of all keys
            // if they exist on the system
            for ( VariableValueVector::iterator iterator = keyEntity->GetVarRef()->GetValues()->begin(); iterator != keyEntity->GetVarRef()->GetValues()->end(); iterator++ ) {
                if ( this->KeyExists ( hiveStr, ( *iterator )->GetValue() ) ) {
                    keys->insert ( ( *iterator )->GetValue() );
                }
            }

        } else {
            // for not equals and pattern match fetch all keys that match
            // any of the variable values. Then analyze each key found on
            // the system against the variable values
            // loop through all variable values and call FindKeys
            VariableValueVector* values = keyEntity->GetVariableValues();

            for ( VariableValueVector::iterator iterator = values->begin(); iterator != values->end(); iterator++ ) {
                if ( keyEntity->GetOperation() == OvalEnum::OPERATION_NOT_EQUAL ) {
                    this->FindKeys ( hiveStr, ( *iterator )->GetValue(), allKeys, false );

                } else {
                    this->FindKeys ( hiveStr, ( *iterator )->GetValue(), allKeys, true );
                }
            }
        }

        // only keep keys that match operation and value and var check
        ItemEntity* tmp = new ItemEntity ( "key", "", OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS );

        for ( StringSet::iterator it = allKeys->begin(); it != allKeys->end(); it++ ) {
            tmp->SetValue ( ( *it ) );

            if ( keyEntity->Analyze ( tmp ) == OvalEnum::RESULT_TRUE ) {
                keys->insert ( ( *it ) );
            }
        }

        delete tmp;
        allKeys->clear();
        delete allKeys;
    }

    // apply any behaviors and consolidate the results
    StringSet* behaviorKeys = this->ProcessKeyBehaviors ( hiveStr, keys, behaviors );

    for ( StringSet::iterator it = behaviorKeys->begin(); it != behaviorKeys->end(); it++ ) {
        keys->insert ( ( *it ) );
    }

    behaviorKeys->clear();
    delete behaviorKeys;
    return keys;
}

StringSet* RegistryFinder::GetNames ( string hiveStr, string keyStr, ObjectEntity* nameEntity ) {
    StringSet* names = new StringSet();

    if ( nameEntity->GetNil() ) {
        names = new StringSet();
        names->insert ( "" );
        return names;
    }

    if ( nameEntity->GetVarRef() == NULL ) {
        // proceed based on operation
        if ( nameEntity->GetOperation() == OvalEnum::OPERATION_EQUALS ) {
            if ( this->NameExists ( hiveStr, keyStr, nameEntity->GetValue() ) ) {
                names->insert ( nameEntity->GetValue() );
            }

        } else if ( nameEntity->GetOperation() == OvalEnum::OPERATION_NOT_EQUAL ) {
            // turn the provided name value into a negative pattern match
            // then get all names that match the pattern
            this->FindNames ( hiveStr, keyStr, nameEntity->GetValue(), names, false );

        } else if ( nameEntity->GetOperation() == OvalEnum::OPERATION_PATTERN_MATCH ) {
            this->FindNames ( hiveStr, keyStr, nameEntity->GetValue(), names );
        }

    } else {
        StringSet* allNames = new StringSet();

        if ( nameEntity->GetOperation() == OvalEnum::OPERATION_EQUALS ) {
            // in the case of equals simply loop through all the
            // variable values and add them to the set of all names
            // if they exist on the system
            for ( VariableValueVector::iterator iterator = nameEntity->GetVarRef()->GetValues()->begin(); iterator != nameEntity->GetVarRef()->GetValues()->end(); iterator++ ) {
                if ( this->NameExists ( hiveStr, keyStr, ( *iterator )->GetValue() ) ) {
                    names->insert ( ( *iterator )->GetValue() );
                }
            }

        } else {
            // for not equals and pattern match fetch all names that match
            // any of the variable values. Then analyze each name found on
            // the system against the variable values
            // loop through all variable values and call FindNames
            VariableValueVector* values = nameEntity->GetVariableValues();

            for ( VariableValueVector::iterator iterator = values->begin(); iterator != values->end(); iterator++ ) {
                if ( nameEntity->GetOperation() == OvalEnum::OPERATION_NOT_EQUAL ) {
                    this->FindNames ( hiveStr, keyStr, ( *iterator )->GetValue(), allNames, false );

                } else {
                    this->FindNames ( hiveStr, keyStr, ( *iterator )->GetValue(), allNames, true );
                }
            }
        }

        // only keep names that match operation and value and var check
        ItemEntity* tmp = new ItemEntity ( "hive", "", OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS );

        for ( StringSet::iterator it = allNames->begin(); it != allNames->end(); it++ ) {
            tmp->SetValue ( ( *it ) );

            if ( nameEntity->Analyze ( tmp ) == OvalEnum::RESULT_TRUE ) {
                names->insert ( ( *it ) );
            }
        }

        delete tmp;
        delete allNames;
    }

    return names;
}

StringSet* RegistryFinder::ReportHiveDoesNotExist ( ObjectEntity *hiveEntity ) {
    StringSet* hives = NULL;

    if ( hiveEntity->GetOperation() == OvalEnum::OPERATION_EQUALS ) {
        if ( hiveEntity->GetVarRef() == NULL ) {
            if ( !this->HiveExists ( hiveEntity->GetValue() ) ) {
                hives = new StringSet();
                hives->insert ( hiveEntity->GetValue() );
            }

        } else {
            for ( VariableValueVector::iterator iterator = hiveEntity->GetVarRef()->GetValues()->begin(); iterator != hiveEntity->GetVarRef()->GetValues()->end(); iterator++ ) {
                if ( !this->HiveExists ( ( *iterator )->GetValue() ) ) {
                    if ( hives == NULL ) {
                        hives = new StringSet();
                    }

                    hives->insert ( ( *iterator )->GetValue() );
                }
            }
        }
    }

    return hives;
}

StringSet* RegistryFinder::ReportKeyDoesNotExist ( string hiveStr, ObjectEntity *keyEntity ) {
    StringSet* keys = NULL;

    if ( keyEntity->GetOperation() == OvalEnum::OPERATION_EQUALS && !keyEntity->GetNil() ) {
        if ( keyEntity->GetVarRef() == NULL ) {
            if ( !this->KeyExists ( hiveStr, keyEntity->GetValue() ) ) {
                keys = new StringSet();
                keys->insert ( keyEntity->GetValue() );
            }

        } else {
            for ( VariableValueVector::iterator iterator = keyEntity->GetVarRef()->GetValues()->begin(); iterator != keyEntity->GetVarRef()->GetValues()->end(); iterator++ ) {
                if ( !this->KeyExists ( hiveStr, ( *iterator )->GetValue() ) ) {
                    if ( keys == NULL ) {
                        keys = new StringSet();
                    }

                    keys->insert ( ( *iterator )->GetValue() );
                }
            }
        }
    }

    return keys;
}

StringSet* RegistryFinder::ReportNameDoesNotExist ( string hiveStr, string keyStr, ObjectEntity *nameEntity ) {
    StringSet* names = NULL;

    if ( nameEntity->GetOperation() == OvalEnum::OPERATION_EQUALS && !nameEntity->GetNil() ) {
        if ( nameEntity->GetVarRef() == NULL ) {
            if ( !this->NameExists ( hiveStr, keyStr, nameEntity->GetValue() ) ) {
                names = new StringSet();
                names->insert ( nameEntity->GetValue() );
            }

        } else {
            for ( VariableValueVector::iterator iterator = nameEntity->GetVarRef()->GetValues()->begin(); iterator != nameEntity->GetVarRef()->GetValues()->end(); iterator++ ) {
                if ( !this->NameExists ( hiveStr, keyStr, ( *iterator )->GetValue() ) ) {
                    if ( names == NULL ) {
                        names = new StringSet();
                    }

                    names->insert ( ( *iterator )->GetValue() );
                }
            }
        }
    }

    return names;
}

LONG RegistryFinder::GetHKeyHandle ( HKEY *keyHandle, string hiveStr, string keyStr ) {
    HKEY hiveHandle;

	if (hiveStr.compare("HKEY_LOCAL_MACHINE") == 0) {
        hiveHandle = HKEY_LOCAL_MACHINE;
    } else if (hiveStr.compare("HKEY_USERS") == 0) {
        hiveHandle = HKEY_USERS;
    } else if (hiveStr.compare("HKEY_CURRENT_USER") == 0) {
        hiveHandle = HKEY_CURRENT_USER;
    } else if (hiveStr.compare("HKEY_CURRENT_CONFIG") == 0) {
        hiveHandle = HKEY_CURRENT_CONFIG;
    } else if (hiveStr.compare("HKEY_CLASSES_ROOT") == 0) {
        hiveHandle = HKEY_CLASSES_ROOT;
    } else {
		return NULL;
    }

	// short-circuit optimization so we don't have to bother with
	// doing a RegKeyOpenEx().  In my experiments, I found that
	// if you open a hive key (with subkey ""), you just get that same
	// hive key back again.  And you can RegCloseKey() that key without
	// error, but it won't actually close (as it shouldn't, since those
	// hive keys should always be available).  But it means we can
	// treat the hive keys uniformly with respect to other types of keys,
	// which makes for simpler code.  Btw, this is in conflict with the
	// MSDN docs, which say you only get the same handle back if you
	// opened hive HKEY_CLASSES_ROOT.  Otherwise, it says you get a
	// *new* handle.  In my experiments, I always got the same handle
	// back, i.e. they tested equal with the == operator, with any
	// hive.
    if (keyStr.empty()) {
        *keyHandle = hiveHandle;
		return ERROR_SUCCESS;
    }

	return GetHKeyHandle(keyHandle, hiveHandle, keyStr);
}

LONG RegistryFinder::GetHKeyHandle ( HKEY *keyHandle, HKEY superKey, string subKeyStr ) {

	REGSAM view =
#ifdef _WIN64
	(bitnessView == BIT_32 ? KEY_WOW64_32KEY : KEY_WOW64_64KEY);
#else
	// For now, don't OR any extra bits into the mask for 32-bit app.
	// Which means it's getting the 32-bit view.  The constructors
	// of this class also ensure that for now, when this app is built
	// as 32-bit, this will have bitnessView == BIT_32, so is consistent
	// with this behavior.  But the *_WOW64_* enumerators make no
	// sense on 32-bit windows (in case we're running on that platform),
	// since there is no WOW, so I don't want to use them.
	0;
#endif
    return RegOpenKeyExW ( superKey, WindowsCommon::StringToWide ( subKeyStr ),
		0, KEY_READ | view, keyHandle );
}

string RegistryFinder::BuildRegistryKey(const string hiveStr, const string keyStr) {

    if(hiveStr.compare("") == 0)
        throw RegistryFinderException("An empty hive was specified when building a registry key.");

	string registryKey = hiveStr;
    if(keyStr.compare("") != 0) {
        // Verify that the hive that was passed into this function ends with a slash.  If
        // it doesn't, then add one.
        if (hiveStr[hiveStr.length()-1] != RegistryFinder::keySeparator)
	        registryKey.append(1, RegistryFinder::keySeparator);

        if(keyStr[0] != RegistryFinder::keySeparator) {
			registryKey.append(keyStr);
		} else {
			registryKey.append(keyStr.substr(1, keyStr.length()-2));
		}
    }

    return registryKey;
}

string RegistryFinder::ConvertHiveForWindowsObjectName( string hiveStr ){
	if ( hiveStr.compare("HKEY_LOCAL_MACHINE") == 0 ){
		return "MACHINE";
	}else if ( hiveStr.compare("HKEY_USERS") == 0 ){
		return "USERS";
	}else if ( hiveStr.compare("HKEY_CURRENT_USER") == 0 ){	
		return "CURRENT_USER";
	}else if ( hiveStr.compare("HKEY_CURRENT_CONFIG") == 0 ){
		return "CURRENT_CONFIG";
	}else if ( hiveStr.compare("HKEY_CLASSES_ROOT") == 0 ){
		return "CLASSES_ROOT";		
	}else{
		return "";
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

bool RegistryFinder::HiveExists ( string hiveStr ) {
    HKEY keyHandle = NULL;

    if ( GetHKeyHandle ( &keyHandle, hiveStr) ) {
        return false;
    }

    if ( RegCloseKey ( keyHandle ) != ERROR_SUCCESS ) {
        throw RegistryFinderException ( "Error: RegCloseKey() was unable to close the handle. Microsoft System Error " + Common::ToString ( GetLastError() ) + ") - " + WindowsCommon::GetErrorMessage ( GetLastError() ) );
    }

    return true;
}

bool RegistryFinder::KeyExists ( string hiveStr, string keyStr ) {
    HKEY keyHandle = NULL;

    if ( GetHKeyHandle ( &keyHandle, hiveStr, keyStr ) ) {
        return false;
    }

    if ( RegCloseKey ( keyHandle ) != ERROR_SUCCESS ) {
        throw RegistryFinderException ( "Error: RegCloseKey() was unable to close the handle. Microsoft System Error " + Common::ToString ( GetLastError() ) + ") - " + WindowsCommon::GetErrorMessage ( GetLastError() ) );
    }

    return true;
}

bool RegistryFinder::NameExists ( string hiveStr, string keyStr, string nameStr ) {
    HKEY keyHandle; 

    if ( GetHKeyHandle ( &keyHandle, hiveStr, keyStr ) ) {
        return false;
    }

    if ( RegQueryValueExW ( keyHandle, WindowsCommon::StringToWide ( nameStr ), NULL, NULL, NULL, NULL ) != ERROR_SUCCESS ) {
        return false;
    }

    if ( RegCloseKey ( keyHandle ) != ERROR_SUCCESS ) {
        throw RegistryFinderException ( "Error: RegCloseKey() was unable to close the handle. Microsoft System Error " + Common::ToString ( GetLastError() ) + ") - " + WindowsCommon::GetErrorMessage ( GetLastError() ) );
    }

    return true;
}

void RegistryFinder::FindHives ( string patternStr, StringSet* hives, bool isRegex ) {
    StringSet* allHives = GetAllHives();

    for ( StringSet::iterator iterator = allHives->begin() ; iterator != allHives->end(); iterator++ ) {
        if ( this->IsMatch ( patternStr , *iterator , isRegex ) ) {
            hives->insert ( *iterator );
        }
    }
}

void RegistryFinder::FindKeys ( string hiveStr, string regexStr, StringSet* keys, bool isRegex ) {
    string keyNameStr = "";
    string patternOutStr = "";
    string constPortionStr = "";
    string keySeparatorStr = "";

	// This optimization only applies when the regex is anchored to
	// the beginning of keys. (regex has to start with '^')
	if (isRegex && !regexStr.empty() && regexStr[0] == '^') {	
		this->registryMatcher->GetConstantPortion ( regexStr, RegistryFinder::keySeparator, &patternOutStr, &constPortionStr );
		constPortionStr = this->registryMatcher->RemoveExtraSlashes ( constPortionStr );
	}
    if ( constPortionStr.compare ( "" ) != 0 && patternOutStr.compare ( "" ) != 0 ) {
        this->GetRegistriesForPattern ( hiveStr, constPortionStr, regexStr, keys, isRegex );

    } else if ( constPortionStr.compare ( "" ) == 0 ) {
		constPortionStr.append( 1, RegistryFinder::keySeparator);
		this->GetRegistriesForPattern ( hiveStr, constPortionStr, regexStr, keys, isRegex );

    } else if ( patternOutStr.compare ( "" ) == 0 ) {
        if ( this->KeyExists ( hiveStr, constPortionStr ) ) {
            keys->insert ( constPortionStr );
        }
    }
}

void RegistryFinder::FindNames ( string hiveStr, string keyStr, string patternStr, StringSet* names, bool isRegex ) {
    StringSet* allNames = GetAllNames ( hiveStr, keyStr );

    for ( StringSet::iterator iterator = allNames->begin() ; iterator != allNames->end(); iterator++ ) {
        if ( this->IsMatch ( patternStr , *iterator , isRegex ) ) {
            names->insert ( *iterator );
        }
    }
}

StringSet* RegistryFinder::GetAllHives() {
    StringSet* hives = new StringSet();
    hives->insert ( "HKEY_CLASSES_ROOT" );
    hives->insert ( "HKEY_CURRENT_CONFIG" );
    hives->insert ( "HKEY_CURRENT_USER" );
    hives->insert ( "HKEY_LOCAL_MACHINE" );
    hives->insert ( "HKEY_USERS" );
    return hives;
}

StringSet* RegistryFinder::GetAllSubKeys ( string hiveStr, string keyStr ) {
    StringSet* subkeys = new StringSet();
    LPWSTR name = ( LPWSTR ) malloc ( sizeof ( WCHAR ) * MAX_PATH );
    HKEY keyHandle;
    DWORD index = 0;
    DWORD size = MAX_PATH;

    if ( !GetHKeyHandle ( &keyHandle, hiveStr, keyStr ) ) {
        while ( RegEnumKeyExW ( keyHandle, index, name, &size, NULL, NULL, NULL, NULL ) == ERROR_SUCCESS ) {
            string nameStr = "";
            nameStr.append ( keyStr );
			if ( keyStr.compare ( "" ) != 0 ) nameStr.append ( 1, RegistryFinder::keySeparator );

            nameStr.append ( WindowsCommon::UnicodeToAsciiString ( name ) );

			if ( nameStr.at ( nameStr.length() - 1 ) == RegistryFinder::keySeparator ) {
                nameStr.erase ( nameStr.length() - 1, 1 );
            }

            subkeys->insert ( nameStr );
            size = MAX_PATH;
            ++index;
        }

        if ( RegCloseKey ( keyHandle ) != ERROR_SUCCESS ) {
            throw RegistryFinderException ( "Error: RegCloseKey() was unable to close the handle. Microsoft System Error " + Common::ToString ( GetLastError() ) + ") - " + WindowsCommon::GetErrorMessage ( GetLastError() ) );
        }
    }

    if ( name != NULL ) {
        free ( name );
        name = NULL;
    }

    return subkeys;
}

StringSet* RegistryFinder::GetAllNames ( string hiveStr, string keyStr ) {
    StringSet* names = new StringSet();
    LPWSTR name = ( LPWSTR ) malloc ( sizeof ( WCHAR ) * MAX_PATH );
    HKEY keyHandle;
    DWORD index = 0;
    DWORD size = MAX_PATH;
    string nameStr = "";

    if ( !GetHKeyHandle ( &keyHandle, hiveStr, keyStr ) ) {
        while ( RegEnumValueW ( keyHandle, index, name, &size, NULL, NULL, NULL, NULL ) == ERROR_SUCCESS ) {
            nameStr = WindowsCommon::UnicodeToAsciiString ( name );
            names->insert ( nameStr );
            size = MAX_PATH;
            ++index;
        }

        if ( RegCloseKey ( keyHandle ) != ERROR_SUCCESS ) {
            throw RegistryFinderException ( "Error: RegCloseKey() was unable to close the handle. Microsoft System Error " + Common::ToString ( GetLastError() ) + ") - " + WindowsCommon::GetErrorMessage ( GetLastError() ) );
        }
    }

    if ( name != NULL ) {
        free ( name );
        name = NULL;
    }

    return names;
}

StringSet* RegistryFinder::ProcessKeyBehaviors ( string hiveStr, StringSet* keys, BehaviorVector* behaviors ) {
    // Process the behaviors to identify any additional keys.
    // initialize these default values based on the defaults
    // set in the oval definitions schema
    string recurseDirection = Behavior::GetBehaviorValue ( behaviors, "recurse_direction" );

    if ( recurseDirection.compare ( "" ) == 0 ) {
        recurseDirection = "none";
    }

    string maxDepthStr = Behavior::GetBehaviorValue ( behaviors, "max_depth" );
    int maxDepth = -1;

    if ( maxDepthStr.compare ( "" ) != 0 ) {
        maxDepth = atoi ( maxDepthStr.c_str() );

        if ( maxDepth < -1 )
            maxDepth = -1;
    }

    // only need to address recurseDirection up & down if maxDepth is not 0
    StringSet* behaviorKeys = new StringSet();

    if ( recurseDirection.compare ( "up" ) == 0 && maxDepth != 0 ) {
        for ( StringSet::iterator iterator = keys->begin(); iterator != keys->end(); iterator++ ) {
            this->UpwardRegistryRecursion ( behaviorKeys, hiveStr, *iterator, maxDepth );
        }

    } else if ( recurseDirection.compare ( "down" ) == 0 && maxDepth != 0 ) {
        for ( StringSet::iterator iterator = keys->begin(); iterator != keys->end(); iterator++ ) {
            this->DownwardRegistryRecursion ( behaviorKeys, hiveStr, ( *iterator ), maxDepth );
        }
    }

    return behaviorKeys;
}

void RegistryFinder::GetRegistriesForPattern ( string hiveStr, string keyStr, string regexStr, StringSet *keys, bool isRegex ) {
    if ( ( ( keyStr.empty() == true ) || ( keyStr == "" ) ) ) {
        return;
    }

	if ( RegistryFinder::KeyExists(hiveStr,keyStr) && this->IsMatch( regexStr , keyStr , isRegex ) ){
		keys->insert( keyStr );
	}		

    // Verify that the key that was passed into this function ends with a slash.  If it doesn't, then add one.
    if ( keyStr[keyStr.length()-1] != RegistryFinder::keySeparator ) {
        keyStr.append ( 1, RegistryFinder::keySeparator );
    }

    // Verify that the key that was passed into this function does not begin with a slash.  If it does then remove it.
    if ( keyStr[0] == RegistryFinder::keySeparator ) {
        keyStr.erase ( 0, 1 );
    }

    LPWSTR name = ( LPWSTR ) malloc ( sizeof ( WCHAR ) * MAX_PATH );
    HKEY keyHandle;
    DWORD index = 0;
    DWORD size = MAX_PATH;

    if ( !GetHKeyHandle ( &keyHandle, hiveStr, keyStr ) ) {
        while ( RegEnumKeyExW ( keyHandle, index, name, &size, NULL, NULL, NULL, NULL ) == ERROR_SUCCESS ) {
            string nameStr = WindowsCommon::UnicodeToAsciiString ( name );
            string newKeyStr = keyStr;
            newKeyStr.append ( nameStr );
			this->GetRegistriesForPattern( hiveStr, newKeyStr , regexStr, keys , isRegex );
            size = MAX_PATH;
            ++index;
        }

        if ( RegCloseKey ( keyHandle ) != ERROR_SUCCESS ) {
            throw RegistryFinderException ( "Error: RegCloseKey() was unable to close the handle. Microsoft System Error " + Common::ToString ( GetLastError() ) + ") - " + WindowsCommon::GetErrorMessage ( GetLastError() ) );
        }
    }

    if ( name != NULL ) {
        free ( name );
        name = NULL;
    }
}

bool RegistryFinder::IsMatch ( string patternStr, string valueStr, bool isRegex ) {
    bool match = false;

    if ( isRegex ) {
        if ( this->registryMatcher->IsMatch ( patternStr.c_str(), valueStr.c_str() ) ) {
            match = true;
        }

    } else {
        if ( valueStr.compare ( patternStr ) != 0 ) {
            match = true;
        }
    }

    return match;
}

void RegistryFinder::DownwardRegistryRecursion ( StringSet* keys, string hiveStr, string keyStr, int maxDepth ) {
    if ( maxDepth == 0 ) {
        return;
    }

    if ( maxDepth < -1 ) {
        throw RegistryFinderException ( "Error invalid max_depth. max_depth must be -1 or more. Found: " + Common::ToString ( maxDepth ) );
    }

    StringSet* subkeys = this->GetAllSubKeys ( hiveStr, keyStr );

    for ( StringSet::iterator it = subkeys->begin(); it != subkeys->end(); it++ ) {
        keys->insert ( *it );

        if ( maxDepth == -1 ) {
            this->DownwardRegistryRecursion ( keys, hiveStr, *it, maxDepth );

        } else if ( maxDepth > 0 ) {
            this->DownwardRegistryRecursion ( keys, hiveStr, *it, maxDepth - 1);
        }
    }

    subkeys->clear();
    delete subkeys;
    return;
}

void RegistryFinder::UpwardRegistryRecursion ( StringSet* keys, string hiveStr, string keyStr, int maxDepth ) {
    if ( maxDepth == 0 ) {
        return;
    }

    if ( maxDepth < -1 ) {
		throw RegistryFinderException ( "Error invalid max_depth. max_depth must be -1 or more. Found: " + Common::ToString ( maxDepth ) );
	}

    if ( keyStr.compare ( "" ) == 0 ) {
        return;
    }

    if ( keyStr[keyStr.length()-1] == RegistryFinder::keySeparator ) {
        keyStr = keyStr.substr ( 0, ( keyStr.length() - 1 ) );
    }

    basic_string <char>::size_type index = keyStr.find_last_of ( RegistryFinder::keySeparator );

    if ( index == string::npos ) {
        if ( maxDepth > 0 || maxDepth == - 1 ) {
            keys->insert ( hiveStr );
        }

        return;
    }

    string parentKeyStr = keyStr.substr ( 0, index );
    keys->insert ( parentKeyStr );

    if ( maxDepth == -1 ) {
        this->UpwardRegistryRecursion ( keys, hiveStr, parentKeyStr, maxDepth );

    } else if ( maxDepth > 0 ) {
        this->UpwardRegistryRecursion ( keys, hiveStr, parentKeyStr, --maxDepth );
    }

    return;
}

RegistryFinder::BitnessView RegistryFinder::behavior2view(const string &viewStr) {
	if (viewStr == "32_bit")
		return BIT_32;
	if (viewStr == "64_bit")
		return BIT_64;
	throw RegistryFinderException("Unrecognized view value: "+viewStr);
}

RegistryFinder::BitnessView RegistryFinder::behavior2view(BehaviorVector *bv) {
	BitnessView schemaDefault = BIT_64;

	if (!bv)
		return schemaDefault;

	string viewStr = Behavior::GetBehaviorValue(bv, "windows_view");

	if (viewStr.empty())
		return schemaDefault;

	return behavior2view(viewStr);
}

//****************************************************************************************//
//					       RegistryFinderException Class								  //	
//****************************************************************************************//
RegistryFinderException::RegistryFinderException(string errMsgIn, int severity, Exception* ex) : Exception(errMsgIn, severity, ex) {

}

RegistryFinderException::~RegistryFinderException() {

}
