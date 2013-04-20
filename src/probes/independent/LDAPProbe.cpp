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

#include <sstream>
#include <iomanip>

#ifdef WIN32
#  include <Windows.h> // defines symbols needed by <LM.h>
#  include <LM.h>
#  include <Dsgetdc.h>
#  include <Winldap.h> // defines symbols needed by <Winber.h>
#  include <Winber.h>
#endif

#include "REGEX.h"
#include "Log.h"

#include "LDAPProbe.h"

using namespace std;

//****************************************************************************************//
//								LDAPProbe Class										  //
//****************************************************************************************//
LDAPProbe* LDAPProbe::instance = NULL;
const string LDAPProbe::SCOPE_BASE = "BASE";
const string LDAPProbe::SCOPE_ONE = "ONE";
const string LDAPProbe::SCOPE_SUBTREE = "SUBTREE";
const string LDAPProbe::NAMING_CONTEXTS_ATTRIBUTE = "namingcontexts";
const string LDAPProbe::SUBSCHEMA_SUBENTRY_ATTRIBUTE = "subschemasubentry";
const string LDAPProbe::ATTRIBUTE_TYPES_ATTRIBUTE = "attributetypes";
const string LDAPProbe::OBJECT_CLASS_ATTRIBUTE = "objectclass";

LDAPProbe::LDAPProbe() {
	ldap = LDAPProbe::OpenConnection ( LDAPProbe::GetLDAPServerLocation() );
	types = NULL;
	types = LDAPProbe::GetAttributeValueTypes();
}

LDAPProbe::~LDAPProbe() {
	instance = NULL;
	LDAPProbe::DeleteAllTypes();
	LDAPProbe::CloseConnection ( ldap );
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* LDAPProbe::Instance() {

	if ( instance == NULL ) {
		instance = new LDAPProbe();
	}

	return instance;
}

ItemVector* LDAPProbe::CollectItems ( Object *object ) {

	ObjectEntity* suffixEntity = object->GetElementByName ( "suffix" );
	ObjectEntity* relativeDnEntity = object->GetElementByName ( "relative_dn" );
	ObjectEntity* attributeEntity = object->GetElementByName ( "attribute" );
	ItemVector* collectedItems = new ItemVector();
	ItemEntity* currentRelativeDn = new ItemEntity ( "relative_dn", "", OvalEnum::DATATYPE_STRING, (relativeDnEntity->GetNil()?OvalEnum::STATUS_NOT_COLLECTED : OvalEnum::STATUS_EXISTS), relativeDnEntity->GetNil() );
	int scopeBehavior = LDAP_SCOPE_BASE;

	for ( BehaviorVector::iterator it = ( object->GetBehaviors() )->begin() ; it != ( object->GetBehaviors() )->end() ; it++ ) {
		if ( ( ( *it )->GetName() ).compare ( "scope" ) == 0 ) {
			if ( ( ( *it )->GetValue() ).compare ( LDAPProbe::SCOPE_ONE ) == 0 ) {
				scopeBehavior = LDAP_SCOPE_ONELEVEL;
			}
			if ( ( ( *it )->GetValue() ).compare ( LDAPProbe::SCOPE_SUBTREE ) == 0 ) {
				scopeBehavior = LDAP_SCOPE_SUBTREE;
			}
		} else {
			Log::Info ( "Unsupported behavior found when collecting " + object->GetId() + " Found behavior: " + ( *it )->GetName() + " = " + ( *it )->GetValue() );
		}
	}

	StringSet* suffixes = this->GetSuffixes ( suffixEntity );
	for ( StringSet::iterator it1 = suffixes->begin() ; it1 != suffixes->end() ; it1++ ) {
		string suffixStr = *it1;
		StringSet* relativeDns = this->GetRelativeDns ( suffixStr, relativeDnEntity, scopeBehavior );
		for ( StringSet::iterator it2 = relativeDns->begin() ; it2 != relativeDns->end() ; it2++ ) {
			string relativeDnStr = *it2;
			string objectClassStr = this->GetObjectClass ( suffixStr, relativeDnStr );
			StringSet* attributes = this->GetAttributes ( suffixStr, relativeDnStr, attributeEntity );
			for ( StringSet::iterator iterator3 = attributes->begin() ; iterator3 != attributes->end() ; iterator3++ ) {
				string attributeStr = ( *iterator3 );
				Item* item = this->CreateItem();
				item->SetStatus ( OvalEnum::STATUS_EXISTS );
				item->AppendElement ( new ItemEntity ( "suffix", suffixStr, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );

				if ( ( relativeDnEntity->GetNil() || relativeDnStr.compare ( "" ) != 0 ) ) {
					item->AppendElement ( new ItemEntity ( "relative_dn", relativeDnStr, OvalEnum::DATATYPE_STRING, (relativeDnEntity->GetNil()?OvalEnum::STATUS_NOT_COLLECTED : OvalEnum::STATUS_EXISTS), relativeDnEntity->GetNil() ) );
				}
				if ( !attributeEntity->GetNil() ) {
					item->AppendElement ( new ItemEntity ( "attribute", attributeStr, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS, attributeEntity->GetNil() ) );
					item->AppendElement ( new ItemEntity ( "object_class", "", OvalEnum::DATATYPE_STRING, ( objectClassStr.compare ( "" ) ==0 ) ? OvalEnum::STATUS_DOES_NOT_EXIST:OvalEnum::STATUS_EXISTS ) );
					this->GetLdapItem ( suffixStr, relativeDnStr, attributeStr,item );
					if ( item != NULL ) {
						collectedItems->push_back ( new Item ( *item ) );
					}
				} else {
					item->AppendElement ( new ItemEntity ( "attribute", attributeStr, OvalEnum::DATATYPE_STRING, (attributeEntity->GetNil()?OvalEnum::STATUS_NOT_COLLECTED : OvalEnum::STATUS_EXISTS), attributeEntity->GetNil() ) );
					item->AppendElement ( new ItemEntity ( "object_class", "", OvalEnum::DATATYPE_STRING, ( objectClassStr.compare ( "" ) ==0 ) ? OvalEnum::STATUS_DOES_NOT_EXIST:OvalEnum::STATUS_EXISTS ) );
					collectedItems->push_back ( new Item ( *item ) );
				}
			}
			currentRelativeDn->SetValue ( relativeDnStr );
			// do not report attributes that do not exist if behaviors were used (i.e. the relativeDnStr value won't match original relativeDnEntity object entity)
			if ( relativeDnEntity->Analyze ( currentRelativeDn ) == OvalEnum::RESULT_TRUE ) {
				StringVector attributesDne;
				if ( this->ReportAttributeDoesNotExist ( suffixStr, relativeDnStr, attributeEntity, &attributesDne ) ) {
					for ( StringVector::iterator iterator = attributesDne.begin(); iterator != attributesDne.end(); iterator++ ) {
						Item* item = this->CreateItem();
						item->SetStatus ( OvalEnum::STATUS_DOES_NOT_EXIST );
						item->AppendElement ( new ItemEntity ( "suffix", suffixStr, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
						item->AppendElement ( new ItemEntity ( "relative_dn", relativeDnStr, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS, relativeDnEntity->GetNil() ) );
						item->AppendElement ( new ItemEntity ( "attribute", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST, attributeEntity->GetNil() ) );
						item->AppendElement ( new ItemEntity ( "object_class", "", OvalEnum::DATATYPE_STRING, ( objectClassStr.compare ( "" ) == 0 ) ? OvalEnum::STATUS_DOES_NOT_EXIST:OvalEnum::STATUS_EXISTS ) );
						collectedItems->push_back ( item );
					}
				}
			}
			delete attributes;
		}
		StringVector relativeDnsDne;
		if ( this->ReportRelativeDnDoesNotExist ( suffixStr, relativeDnEntity, &relativeDnsDne ) ) {
			for ( StringVector::iterator iterator = relativeDnsDne.begin(); iterator != relativeDnsDne.end(); iterator++ ) {
				Item* item = this->CreateItem();
				item->SetStatus ( OvalEnum::STATUS_DOES_NOT_EXIST );
				item->AppendElement ( new ItemEntity ( "suffix", suffixStr, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
				item->AppendElement ( new ItemEntity ( "relative_dn", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST, relativeDnEntity->GetNil() ) );
				collectedItems->push_back ( item );
			}
		}
		delete relativeDns;
	}
	StringVector suffixesDne;
	if ( this->ReportSuffixDoesNotExist ( suffixEntity, &suffixesDne ) ) {
		for ( StringVector::iterator iterator = suffixesDne.begin(); iterator != suffixesDne.end(); iterator++ ) {
			Item* item = this->CreateItem();
			item->SetStatus ( OvalEnum::STATUS_DOES_NOT_EXIST );
			item->AppendElement ( new ItemEntity ( "suffix", ( *iterator ), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST ) );
			collectedItems->push_back ( item );
		}
	}
	delete currentRelativeDn;
	delete suffixes;
	return collectedItems;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
Item* LDAPProbe::CreateItem() {

	Item* item = new Item ( 0,
	                        "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#independent",
	                        "ind-sc",
	                        "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#independent independent-system-characteristics-schema.xsd",
	                        OvalEnum::STATUS_ERROR,
	                        "ldap_item" );

	return item;
}

StringSet* LDAPProbe::GetSuffixes ( ObjectEntity* suffixEntity ) {
	StringSet* suffixes = new StringSet();
	StringSet* allSuffixes = NULL;
	if ( suffixEntity->GetOperation() == OvalEnum::OPERATION_EQUALS ) {
		StringVector values;
		allSuffixes = new StringSet();
		suffixEntity->GetEntityValues ( values );
		for ( StringVector::iterator it = values.begin() ; it != values.end() ; it++ ) {
			allSuffixes->insert ( *it );
		}
	} else {
		allSuffixes = this->GetAllSuffixes ( );
	}
	ItemEntity* tmp = this->CreateItemEntity ( suffixEntity );
	for ( StringSet::iterator it = allSuffixes->begin() ; it != allSuffixes->end() ; it++ ) {
		tmp->SetValue ( ( *it ) );
		if ( suffixEntity->Analyze ( tmp ) == OvalEnum::RESULT_TRUE ) {
			if ( this->SuffixExists ( *it ) ) {
				suffixes->insert ( ( *it ) );
			}
		}
	}
	delete allSuffixes;
	return suffixes;
}

StringSet* LDAPProbe::GetAllSuffixes ( ) {
	StringSet* suffixes = new StringSet();
	StringVector* allSuffixes = LDAPProbe::GetValues ( "", "", LDAPProbe::NAMING_CONTEXTS_ATTRIBUTE );
	for ( StringVector::iterator it = allSuffixes->begin() ; it != allSuffixes->end() ; it++ ) {
		StringSet* dn = LDAPProbe::GetAllDistinguishedNames ( *it, "", LDAP_SCOPE_BASE );
		for ( StringSet::iterator it1 = dn->begin() ; it1 != dn->end() ; it1++ ) {
			suffixes->insert ( *it1 );
		}
		delete dn;
	}
	delete allSuffixes;
	return suffixes;
}

bool LDAPProbe::SuffixExists ( string suffixStr ) {
	return LDAPProbe::ObjectExists ( suffixStr, "" );
}

StringSet* LDAPProbe::GetRelativeDns ( string suffixStr , ObjectEntity* relativeDnEntity, int scopeBehavior ) {
	StringSet* relativeDns = new StringSet();
	if ( relativeDnEntity->GetNil() ) {
		relativeDns->insert ( "" );
	} else {
		StringSet* allRelativeDns = NULL;
		if ( relativeDnEntity->GetOperation() == OvalEnum::OPERATION_EQUALS ) {
			StringVector values;
			allRelativeDns = new StringSet();
			relativeDnEntity->GetEntityValues ( values );
			for ( StringVector::iterator it = values.begin() ; it != values.end() ; it++ ) {
				allRelativeDns->insert ( *it );
			}
		} else {
			allRelativeDns = this->GetAllDistinguishedNames ( suffixStr, "", LDAP_SCOPE_SUBTREE );
		}
		ItemEntity* tmp = this->CreateItemEntity ( relativeDnEntity );
		string relativeDistinguishedNameStr = "";
		for ( StringSet::iterator it = allRelativeDns->begin() ; it != allRelativeDns->end() ; it++ ) {
			relativeDistinguishedNameStr = LDAPProbe::RemoveDnBase ( suffixStr, *it );
			tmp->SetValue ( ( relativeDistinguishedNameStr ) );
			if ( relativeDnEntity->Analyze ( tmp ) == OvalEnum::RESULT_TRUE ) {
				if ( this->RelativeDnExists ( suffixStr, relativeDistinguishedNameStr ) ) {
					relativeDns->insert ( relativeDistinguishedNameStr );
				}
			}
		}
		delete allRelativeDns;
	}
	StringSet* behaviorRelativeDns = new StringSet();
	for ( StringSet::iterator it1 = relativeDns->begin() ; it1 != relativeDns->end() ; it1++ ) {
		StringSet* allDns = LDAPProbe::GetAllDistinguishedNames ( suffixStr, *it1, scopeBehavior );
		for ( StringSet::iterator it2 = allDns->begin() ; it2 != allDns->end() ; it2++ ) {
			string rdn = LDAPProbe::RemoveDnBase ( suffixStr, *it2 );
			behaviorRelativeDns->insert ( rdn );
		}
		delete allDns;
	}
	delete relativeDns;
	return behaviorRelativeDns;
}

bool LDAPProbe::RelativeDnExists ( string suffixStr, string relativeDnStr ) {
	bool exists = false;
	if ( LDAPProbe::ObjectExists ( suffixStr, relativeDnStr ) ) {
		exists = true;
	}
	return exists;
}


StringSet* LDAPProbe::GetAttributes ( string suffixStr, string relativeDnStr, ObjectEntity* attributeEntity ) {
	StringSet* attributes = new StringSet();
	if ( attributeEntity->GetNil() ) {
		attributes->insert ( "" );
	} else {
		StringSet* allAttributes = NULL;
		if ( attributeEntity->GetOperation() == OvalEnum::OPERATION_EQUALS ) {
			StringVector values;
			allAttributes = new StringSet();
			attributeEntity->GetEntityValues ( values );
			for ( StringVector::iterator it = values.begin() ; it != values.end() ; it++ ) {
				allAttributes->insert ( *it );
			}
		} else {
			allAttributes = this->GetAllAttributes ( suffixStr, relativeDnStr );
		}
		ItemEntity* tmp = this->CreateItemEntity ( attributeEntity );
		for ( StringSet::iterator it = allAttributes->begin() ; it != allAttributes->end() ; it++ ) {
			tmp->SetValue ( ( *it ) );
			if ( attributeEntity->Analyze ( tmp ) == OvalEnum::RESULT_TRUE ) {
				if ( this->AttributeExists ( suffixStr, relativeDnStr, *it ) ) {
					attributes->insert ( ( *it ) );
				}
			}
		}
		delete allAttributes;
	}

	return attributes;
}

bool LDAPProbe::AttributeExists ( string suffixStr, string relativeDnStr, string attributeStr ) {
	bool exists = false;
	StringSet* allAttributes = LDAPProbe::GetAllAttributes ( suffixStr, relativeDnStr );
	if ( allAttributes->find ( attributeStr ) != allAttributes->end() ) {
		exists = true;
	}
	delete allAttributes;
	return exists;
}

string LDAPProbe::BuildDistinguishedName ( string suffixStr, string relativeDnStr ) {
	string distinguishedNameStr = relativeDnStr;
	if ( relativeDnStr.compare ( "" ) != 0 ) {
		distinguishedNameStr.append ( "," );
	}
	distinguishedNameStr.append ( suffixStr );
	return distinguishedNameStr;
}

LDAP* LDAPProbe::OpenConnection ( string hostnameStr ) {
	LDAP* ldap = NULL;
	int version = 3;
	ULONG retval = 0;

	#ifdef WIN32
	if ( ( ldap = ldap_init ( ( char* ) hostnameStr.c_str(), 389 ) ) == NULL ) {
		throw ProbeException ( "Error: The LDAP session could not be initialized." );
	}
	#endif

	#if defined (LINUX) || defined (DARWIN)
	string hostnameUriStr = "ldap://";
	hostnameUriStr.append ( hostnameStr );
	hostnameUriStr.append ( ":389/" );
	if ( ( retval = ldap_initialize ( &ldap, hostnameUriStr.c_str() ) ) != LDAP_SUCCESS ) {
		throw ProbeException ( "Error: The LDAP session could not be initialized."+LDAPProbe::GetErrorMessage ( retval ) );
	}
	#endif

	if ( ( retval = ldap_set_option ( ldap, LDAP_OPT_PROTOCOL_VERSION, ( void* ) &version ) ) != LDAP_SUCCESS ) {
		throw ProbeException ( "Error: The LDAP protocol version could not be set. "+LDAPProbe::GetErrorMessage ( retval ) );
	}

	#ifdef WIN32
	if ( ( retval = ldap_bind_s ( ldap, NULL, NULL, LDAP_AUTH_SIMPLE ) ) != LDAP_SUCCESS ) {
		throw ProbeException ( "Error: The LDAP bind operation could not be completed successfully. "+LDAPProbe::GetErrorMessage ( retval ) );
	}
	#endif

	#if defined (LINUX) || defined (DARWIN)
	berval credentials;
	credentials.bv_val = NULL;
	credentials.bv_len = 0;
	if ( ( retval = ldap_sasl_bind_s ( ldap, "", LDAP_SASL_SIMPLE, &credentials, NULL, NULL, NULL ) ) != LDAP_SUCCESS ) {
		throw ProbeException ( "Error: The LDAP bind operation could not be completed successfully. "+LDAPProbe::GetErrorMessage ( retval ) );
	}
	#endif

	return ldap;
}

void LDAPProbe::CloseConnection ( LDAP* ldap ) {
	#ifdef WIN32
	if ( ( ldap != NULL ) && ( ldap_unbind_s ( ldap ) != LDAP_SUCCESS ) ) {
		throw ProbeException ( "Error: The LDAP unbind operation could not be completed successfully." );
	}
	#endif

	#if defined (LINUX) || defined (DARWIN)
	if ( ( ldap != NULL ) && ( ldap_unbind_ext_s ( ldap, NULL, NULL ) != LDAP_SUCCESS ) ) {
		throw ProbeException ( "Error: The LDAP unbind operation could not be completed successfully." );
	}
	#endif
	
	ldap = NULL;
}

string LDAPProbe::RemoveDnBase ( string suffixStr, string distinguishedNameStr ) {
	size_t pos = distinguishedNameStr.rfind ( suffixStr );
	if ( pos > 0 ) {
		return distinguishedNameStr.substr ( 0, pos-1 );
	} else {
		return "";
	}
}

string LDAPProbe::GetObjectClass ( string suffixStr, string relativeDnStr ) {
	string objectClassStr = "";
	StringVector* objectClassVector = LDAPProbe::GetValues ( suffixStr, relativeDnStr, LDAPProbe::OBJECT_CLASS_ATTRIBUTE );
	for ( StringVector::iterator it = objectClassVector->begin(); it != objectClassVector->end() ; it++ ) {
		objectClassStr.append ( *it );
		objectClassStr.append ( ";" );
	}
	delete objectClassVector;
	return objectClassStr;
}


string LDAPProbe::GetLDAPServerLocation() {
	
	#ifdef WIN32

	DOMAIN_CONTROLLER_INFO* DomainControllerInfo = NULL;
	DWORD dReturn = 0L;
	ULONG dcFlags;
	dcFlags = DS_IP_REQUIRED | DS_ONLY_LDAP_NEEDED | DS_RETURN_DNS_NAME;
	dReturn = DsGetDcName ( NULL, NULL, NULL, NULL, dcFlags, &DomainControllerInfo );

	if ( dReturn == ERROR_SUCCESS ) {
		string ldapServerLocationStr = DomainControllerInfo->DomainName;
		if ( NetApiBufferFree ( DomainControllerInfo ) != NERR_Success ) {
			throw ProbeException ( "Error: NetApiBufferFree() was unable to free the memory allocated for the DOMAIN_CONTROLLER_INFO structure." );
		}
		return ldapServerLocationStr;
	} else {
		throw ProbeException ( "Error: DsGetDcName() was unable to retrieve the name of the specified domain controller." );
	}

	#elif defined (LINUX) || defined (DARWIN)

	string pathStr = "/etc/";
	string fileNameStr = "ldap.conf";
	string bufferStr;
	string fileContentsStr;
	REGEX regex;
	ifstream infile;
	string filePathStr = Common::BuildFilePath ( pathStr, fileNameStr );
	infile.open ( filePathStr.c_str() );
	if ( infile.is_open() ) {
		string hostStr = "host";
		while ( !infile.eof() ) {
			getline ( infile, bufferStr );
			vector<StringVector> substrings;
			regex.GetAllMatchingSubstrings ( "\\S+", bufferStr, substrings );
			if ( substrings.size() > 0 ) {
				if ( hostStr.compare ( substrings[0][0] ) == 0 ) {
					for ( unsigned int i = 1 ; i < substrings.size() ; i++ ) {
						return substrings[i][0];
					}
				}
			}
		}
		infile.close();
	} else {
		throw ProbeException ( string ( "Couldn't open file: " ) + filePathStr );
	}

	return "";

	#else

	return "";
	
	#endif
}

StringSet* LDAPProbe::GetAllAttributes ( string suffixStr, string relativeDnStr ) {
	ULONG retval = 0;
	StringSet* attributes = new StringSet();
	LDAPMessage* result = NULL;
	string distinguishedName = LDAPProbe::BuildDistinguishedName ( suffixStr, relativeDnStr );
	if ( ( ( retval = ldap_search_ext_s ( ldap, ( char* ) distinguishedName.c_str(), LDAP_SCOPE_BASE, NULL, NULL, 1, NULL, NULL, NULL, 1, &result ) ) != LDAP_SUCCESS ) || ( result == NULL ) ) {
		Log::Message ( "Error: An error occurred while searching for all of the attributes for the distinguished name '"+distinguishedName+"'. "+LDAPProbe::GetErrorMessage ( retval ) );
	} else {
		LDAPMessage* entry = NULL;
		BerElement* attrPtr = NULL;
		char* attribute;
		if ( ( entry = ldap_first_entry ( ldap, result ) ) != NULL ) {
			do {
				if ( ( attribute = ldap_first_attribute ( ldap, entry, &attrPtr ) ) != NULL ) {
					do {
						attributes->insert ( attribute );
					} while ( ( attribute = ldap_next_attribute ( ldap, entry, attrPtr ) ) != NULL );
				}
				ber_free ( attrPtr,0 );
				attrPtr = NULL;
				ldap_memfree ( attribute );
				attribute = NULL;
			} while ( ( entry = ldap_next_entry ( ldap, entry ) ) != NULL );
		}
	}
	if ( result != NULL ) {
		ldap_msgfree ( result );
		result = NULL;
	}
	return attributes;
}

StringSet* LDAPProbe::GetAllDistinguishedNames ( string suffixStr, string relativeDnStr, int scopeBehavior ) {
	StringSet* distinguishedNames = new StringSet();
	ULONG retval = 0;
	LDAPMessage* result = NULL;
	LDAPMessage* entry = NULL;
	string distinguishedName = LDAPProbe::BuildDistinguishedName ( suffixStr, relativeDnStr );
	if ( ( ( retval=ldap_search_ext_s ( ldap, ( char* ) distinguishedName.c_str(), scopeBehavior, NULL, NULL, 0, NULL, NULL, NULL, 100000, &result ) ) != LDAP_SUCCESS ) || ( result == NULL ) ) {
		Log::Message ( "Error: An error occurred while searching for all of the distinguished names under the base distinguished name '"+distinguishedName+"'. "+LDAPProbe::GetErrorMessage ( retval ) );
	}
	if ( ( entry = ldap_first_entry ( ldap, result ) ) != NULL ) {
		do {
			distinguishedNames->insert ( ldap_get_dn ( ldap, entry ) );
		} while ( ( entry = ldap_next_entry ( ldap, entry ) ) != NULL );
	}
	if ( result != NULL ) {
		ldap_msgfree ( result );
		result = NULL;
	}
	return distinguishedNames;
}

bool LDAPProbe::ObjectExists ( string suffixStr, string relativeDnStr ) {
	bool exists = false;
	LDAPMessage* result = NULL;
	LDAPMessage* entry = NULL;
	unsigned long retval = 0;
	string distinguishedName = LDAPProbe::BuildDistinguishedName ( suffixStr, relativeDnStr );
	if ( ( ( retval=ldap_search_ext_s ( ldap, ( char* ) distinguishedName.c_str(), LDAP_SCOPE_BASE, NULL, NULL, 0, NULL, NULL, NULL, 1, &result ) ) != LDAP_SUCCESS ) || ( result == NULL ) ) {
		Log::Message ( "Error: An error occurred while determining if the entry '"+distinguishedName+"'. "+LDAPProbe::GetErrorMessage ( retval ) );
	} else {
		if ( ( entry = ldap_first_entry ( ldap, result ) ) != NULL ) {
			exists = true;
		}
	}
	if ( result != NULL ) {
		ldap_msgfree ( result );
		result = NULL;
	}
	return exists;
}

void LDAPProbe::GetLdapItem ( string suffixStr, string relativeDnStr, string attributeStr, Item* item ) {
	string type = "";
	StringVector* schemas = LDAPProbe::GetValues ( suffixStr, relativeDnStr, LDAPProbe::SUBSCHEMA_SUBENTRY_ATTRIBUTE );
	for ( StringVector::iterator schema = schemas->begin() ; schema != schemas->end() ; schema++ ) {
		TypeMapMap::iterator itmm;
		if ( ( itmm = types->find ( *schema ) ) != types->end() ) {
			TypeMap::iterator it;
			if ( ( it = ( itmm->second )->find ( attributeStr ) ) != ( itmm->second )->end() ) {
				type = it->second;
				break;
			}
		}
	}
	delete schemas;
	if ( type.compare ( "" ) != 0 ) {
		bool isBinary = (type.compare("LDAPTYPE_BINARY")==0)?true:false;
		item->AppendElement ( new ItemEntity ( "ldaptype", type, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
		StringVector* values = LDAPProbe::GetValues ( suffixStr, relativeDnStr, attributeStr, isBinary);
		if ( values->empty() ) {
			item->AppendElement ( new ItemEntity ( "value", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST ) );
		} else {
			for ( StringVector::iterator value = values->begin(); value != values->end(); value++ ) {
				item->AppendElement ( new ItemEntity ( "value", *value, (isBinary)?OvalEnum::DATATYPE_BINARY:OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
			}
		}
		delete values;
	} else {
		item->AppendElement ( new ItemEntity ( "ldaptype", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST ) );
		item->AppendElement ( new ItemEntity ( "value", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_NOT_COLLECTED ) );
		item->AppendMessage ( new OvalMessage ( "The data type of the attribute is unknown.  Therefore, the attribute's values were not collected." ) );
	}
}

TypeMapMap* LDAPProbe::GetAttributeValueTypes() {
	REGEX regex;
	string typeValue = "";
	string name = "NAME";
	string syntax = "SYNTAX";
	TypeMapMap* types = new TypeMapMap();
	StringVector* schemas = LDAPProbe::GetValues ( "", "", LDAPProbe::SUBSCHEMA_SUBENTRY_ATTRIBUTE );
	StringVector* nameValues = new StringVector();

	for ( StringVector::iterator it1 = schemas->begin() ; it1 != schemas->end() ; it1++ ) {
		TypeMap* tmap = new TypeMap();
		StringVector* attributeTypes = LDAPProbe::GetValues ( *it1, "", LDAPProbe::ATTRIBUTE_TYPES_ATTRIBUTE );
		for ( StringVector::iterator it2 = attributeTypes->begin() ; it2 != attributeTypes->end() ; it2++ ) {
			string attributeTypeStr = *it2;
			vector<StringVector> substrings;
			regex.GetAllMatchingSubstrings ( "\\S+", attributeTypeStr, substrings );
			for ( unsigned int i = 0 ; i < substrings.size() ; i++ ) {
				if ( name.compare ( substrings[i][0] ) == 0 ) {
					string nameValue = substrings[i+1][0];
					if ( nameValue.compare ( "(" ) == 0 ) {
						unsigned int j = i+1;
						while ( ( substrings[++j][0] ).compare ( ")" ) != 0 && j < substrings.size() ) {
							nameValues->push_back ( LDAPProbe::RemoveQuotes ( substrings[j][0] ) );
						}
					} else {
						nameValues->push_back ( LDAPProbe::RemoveQuotes ( substrings[i+1][0] ) );
					}
				} else if ( syntax.compare ( substrings[i][0] ) == 0 ) {
					typeValue = substrings[i+1][0];
				}
			}
			for ( StringVector::iterator it = nameValues->begin() ; it != nameValues->end() ; it++ ) {
				tmap->insert ( make_pair ( *it,LDAPProbe::GetDataType ( typeValue ) ) );
				tmap->insert ( make_pair ( Common::ToLower ( *it ), LDAPProbe::GetDataType ( typeValue ) ) );
			}
			nameValues->clear();
		}
		types->insert ( make_pair ( *it1,tmap ) );
	}
	delete schemas;
	delete nameValues;
	return types;
}

string LDAPProbe::GetDataType ( string oid ) {
	if ( oid.compare ( "1.3.6.1.4.1.1466.115.121.1.3" ) == 0 ) {
		return "LDAPTYPE_ATTRIBUTE_TYPE_DESCRIP_STRING";
	} else if ( oid.compare ( "1.3.6.1.4.1.1466.115.121.1.12" ) == 0 ) {
		return "LDAPTYPE_DN_STRING";
	} else if ( oid.compare ( "1.3.6.1.4.1.1466.115.121.1.6" ) == 0 ) {
		return "LDAPTYPE_BIT_STRING";
	} else if ( oid.compare ( "1.3.6.1.4.1.1466.115.121.1.44" ) == 0 ) {
		return "LDAPTYPE_PRINTABLE_STRING";
	} else if ( oid.compare ( "1.3.6.1.4.1.1466.115.121.1.36" ) == 0 ) {
		return "LDAPTYPE_NUMERIC_STRING";
	} else if ( oid.compare ( "1.3.6.1.4.1.1466.115.121.1.7" ) == 0 ) {
		return "LDAPTYPE_BOOLEAN";
	} else if ( oid.compare ( "1.3.6.1.4.1.1466.115.121.1.27" ) == 0 ) {
		return "LDAPTYPE_INTEGER";
	} else if ( oid.compare ( "1.3.6.1.4.1.1466.115.121.1.53" ) == 0 ) {
		return "LDAPTYPE_UTC_TIME";
	} else if ( oid.compare ( "1.3.6.1.4.1.1466.115.121.1.24" ) == 0 ) {
		return "LDAPTYPE_GENERALIZED_TIME";
	} else if ( oid.compare ( "1.3.6.1.4.1.1466.115.121.1.15" ) == 0 ) {
		return "LDAPTYPE_DIRECTORY_STRING";
	} else if ( oid.compare ( "1.3.6.1.4.1.1466.115.121.1.37" ) == 0 ) {
		return "LDAPTYPE_OBJECT_CLASS_DESCRIP_STRING";
	} else if ( oid.compare( "1.3.6.1.4.1.1466.115.121.1.5" ) == 0 ) {
		return "LDAPTYPE_BINARY";
	}else {
		return "";
	}
}

StringVector* LDAPProbe::GetValues ( string suffixStr, string relativeDnStr, string attributeStr, bool type ) {
	StringVector* valuesVector = new StringVector();
	LDAPMessage* entry = NULL;
	BerElement* attrPtr = NULL;
	LDAPMessage* result = NULL;
	char* attribute = NULL;
	char* attributeArray[2] ;
	berval** values = NULL;
	unsigned long count = 0;
	string distinguishedName = LDAPProbe::BuildDistinguishedName ( suffixStr, relativeDnStr );
	attributeArray[0] = ( char* ) attributeStr.c_str();
	attributeArray[1] = ( char* ) NULL;
	ULONG retval = 0;
	if ( ( ( retval = ldap_search_ext_s ( ldap, ( char* ) distinguishedName.c_str(), LDAP_SCOPE_BASE, NULL, attributeArray, 0, NULL, NULL, NULL, 1, &result ) ) != LDAP_SUCCESS ) || ( result == NULL ) ) {
		Log::Message ( "Error: An error occurred while retrieving the values(s) for the attribute '"+attributeStr+"' of the entry '"+distinguishedName+"'. "+LDAPProbe::GetErrorMessage ( retval ) );
	}
	if ( ( entry = ldap_first_entry ( ldap, result ) ) != NULL ) {
		do {
			if ( ( attribute = ldap_first_attribute ( ldap, entry, &attrPtr ) ) != NULL ) {
				do {
					if ( ( ( values = ldap_get_values_len ( ldap, entry, attribute ) ) != NULL ) && ( ( *values ) != NULL ) && ( count = ldap_count_values_len ( values ) ) > 0 ) {
						string value = "";
						for ( unsigned int i = 0; i < count; i++ ) {
							if ( type ){
								value = this->ConvertToBinary(values[i]->bv_val,values[i]->bv_len);
							}else{
								value = values[i]->bv_val;
							}
							valuesVector->push_back ( value );
							value.clear();
						}
					}

					ldap_value_free_len ( values );
					ldap_memfree ( attribute );
					attribute = NULL;
				} while ( ( attribute = ldap_next_attribute ( ldap, entry, attrPtr ) ) != NULL );
			}
			ber_free ( attrPtr,0 );
			attrPtr = NULL;
		} while ( ( entry = ldap_next_entry ( ldap, entry ) ) != NULL );
		if ( result != NULL ) {
			ldap_msgfree ( result );
			result = NULL;
		}
	}
	return valuesVector;
}

string LDAPProbe::ConvertToBinary(char* data, unsigned long length){
	ostringstream oss;
	oss << hex << setfill('0');
	for (unsigned long i = 0 ; i < length ; i++ )
		oss << setw(2) << (int)data[i];
	return oss.str();
}

string LDAPProbe::RemoveQuotes ( string str ) {
	string s = str;
	size_t pos = 0;
	if ( ( pos = s.find_first_of ( "\"'" ) ) != string::npos ) {
		s = s.substr ( pos+1,s.length() );
	}
	if ( ( pos = s.find_last_of ( "\"'" ) ) != string::npos ) {
		s = s.substr ( 0,pos );
	}
	return s;
}

bool LDAPProbe::ReportSuffixDoesNotExist ( ObjectEntity *suffixEntity, StringVector* suffixesDne ) {
	bool result = false;
	if ( suffixEntity->GetOperation() == OvalEnum::OPERATION_EQUALS ) {
		if ( suffixEntity->GetVarRef() == NULL ) {
			if ( !this->SuffixExists ( suffixEntity->GetValue() ) ) {
				suffixesDne->push_back ( suffixEntity->GetValue() );
				result = true;
			}
		} else {
			VariableValueVector::iterator iterator;
			for ( iterator = suffixEntity->GetVarRef()->GetValues()->begin(); iterator != suffixEntity->GetVarRef()->GetValues()->end(); iterator++ ) {
				if ( !this->SuffixExists ( ( *iterator )->GetValue() ) ) {
					suffixesDne->push_back ( ( *iterator )->GetValue() );
					result = true;
				}
			}
		}
	}
	return  result;
}

bool LDAPProbe::ReportRelativeDnDoesNotExist ( string suffixStr, ObjectEntity *relativeDnEntity, StringVector *relativeDnsDne ) {
	bool result = false;
	if ( relativeDnEntity->GetOperation() == OvalEnum::OPERATION_EQUALS && !relativeDnEntity->GetNil() ) {
		if ( relativeDnEntity->GetVarRef() == NULL ) {
			if ( !this->RelativeDnExists ( suffixStr, relativeDnEntity->GetValue() ) ) {
				relativeDnsDne->push_back ( relativeDnEntity->GetValue() );
				result = true;
			}
		} else {
			VariableValueVector::iterator iterator;
			for ( iterator = relativeDnEntity->GetVarRef()->GetValues()->begin(); iterator != relativeDnEntity->GetVarRef()->GetValues()->end(); iterator++ ) {
				if ( !this->RelativeDnExists ( suffixStr, ( *iterator )->GetValue() ) ) {
					relativeDnsDne->push_back ( ( *iterator )->GetValue() );
					result = true;
				}
			}
		}
	}
	return  result;
}

bool LDAPProbe::ReportAttributeDoesNotExist ( string suffixStr, string relativeDnStr, ObjectEntity *attributeEntity, StringVector *attributesDne ) {
	bool result = false;
	if ( attributeEntity->GetOperation() == OvalEnum::OPERATION_EQUALS && !attributeEntity->GetNil() ) {
		if ( attributeEntity->GetVarRef() == NULL ) {
			if ( !this->AttributeExists ( suffixStr, relativeDnStr, attributeEntity->GetValue() ) ) {
				attributesDne->push_back ( attributeEntity->GetValue() );
				result = true;
			}
		} else {
			VariableValueVector::iterator iterator;
			for ( iterator = attributeEntity->GetVarRef()->GetValues()->begin(); iterator != attributeEntity->GetVarRef()->GetValues()->end(); iterator++ ) {
				if ( !this->AttributeExists ( suffixStr, relativeDnStr, ( *iterator )->GetValue() ) ) {
					attributesDne->push_back ( ( *iterator )->GetValue() );
					result = true;
				}
			}
		}
	}
	return  result;
}

string LDAPProbe::GetErrorMessage ( ULONG error ) {
	return ldap_err2string ( error );
}

void LDAPProbe::DeleteAllTypes() {
	for ( TypeMapMap::iterator typeIt = LDAPProbe::types->begin() ; typeIt != LDAPProbe::types->end() ; typeIt++ ) {
		( typeIt->second )->clear();
		delete typeIt->second;
	}
	delete LDAPProbe::types;
	LDAPProbe::types = NULL;
}
