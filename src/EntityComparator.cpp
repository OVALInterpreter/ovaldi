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

#include <cassert>
#include <memory>
#include <typeinfo>
#include <ostream>

#ifdef WIN32
// stupid windows doesn't have stdint.h.... sigh
#  include <BaseTsd.h> // for UINT32
#  define uint32_t UINT32
#  include <winsock2.h>
#else
#  include <stdint.h>
#  include <netinet/in.h>
#  include <arpa/inet.h>
#endif

#include "REGEX.h"
#include "EntityComparator.h"
#include "StateOrObjectFieldEntityValue.h"
#include "ItemFieldEntityValue.h"

using namespace std;

namespace {
	/**
	 * Represents the ipv4 address of a host or network.  A prefix size
	 * can be specified in CIDR-notation, or a netmask in CIDR-like notation.
	 * A host address has a prefix size of 32.  Bits outside the prefix are
	 * ignored.  Addresses in in_addr structs passed in and out of methods of
	 * this class are assumed to be in host byte order.  I think it makes the 
	 * bit-twiddling code clearer.
	 * <p>
	 * One might think this class could be useful in other areas of the code.
	 * I hope someone factors (or allows me to factor) it out, if necessary.
	 */
	class Ipv4Address {

	public:
		Ipv4Address(const string &addrStr) {
			setFromString(addrStr);

			// simple check that might catch netmask/prefixSize
			// desync error.  But might not.
			assert(
				(prefixSize==0 && netmask.s_addr==0) ||
				(prefixSize>0 && (netmask.s_addr & (1U<<(32-prefixSize))))
			);
		}

		/**
		 * Two ipv4 addresses are equal if their prefix bits are equal
		 * (so they must have the same netmask).  Address bits outside the
		 * prefix are ignored.
		 */
		bool operator==(const Ipv4Address &other) const {
			return getInAddr().s_addr == other.getInAddr().s_addr &&
				getNetmask().s_addr == other.getNetmask().s_addr;
		}

		/**
		 * Two ipv4 addresses are equal if their prefix bits are equal
		 * (so they must have the same netmask).  Address bits outside the
		 * prefix are ignored.
		 */
		bool operator!=(const Ipv4Address &other) const {
			return !(*this == other);
		}

		/**
		 * Returns the number of bits in the prefix.  (== number
		 * of 1 bits in the netmask.)
		 */
		size_t getPrefixSize() const {
			return prefixSize;
		}

		/**
		 * Gets a prefix netmask, i.e. an ipv4 address such that
		 * some contiguous sequence of high-order bits are all 1,
		 * and all remaining bits are 0.
		 */
		in_addr getNetmask() const {
			return netmask;
		}

		/**
		 * Gets an in_addr structure for this address.
		 */
		in_addr getInAddr() const {
			return addr;
		}

		/**
		 * Returns true if \p otherAddr is a prefix of or equal to this
		 * address.
		 */
		bool subsetOf(const Ipv4Address &otherAddr) const;

		/**
		 * Returns true if this address is a prefix of or equal to
		 * otherAddr.
		 */
		bool supersetOf(const Ipv4Address &otherAddr) const {
			return otherAddr.subsetOf(*this);
		}

	private:

		void setFromString(const string &addrStr);

		in_addr addr;

		// the following two of course must be kept in sync!
		// It's faster to keep both than repeatedly compute one
		// from the other.
		in_addr netmask;
		size_t prefixSize;
	};

	in_addr dottedQuadToInAddr(const string &addrStr);

	ostream &operator<<(ostream &out, const Ipv4Address &addr);
}

//****************************************************************************************//
//								EntityComparator Class									  //	
//****************************************************************************************//
OvalEnum::ResultEnumeration EntityComparator::CompareBinary(OvalEnum::Operation op, string defValue, string scValue) {

    // Validate the inputs align with the notion of binary regex = "[0-9a-fA-F]"
    REGEX myRegex;
    if(!myRegex.IsMatch("^[0-9a-fA-F]+$", defValue.c_str())) {
		throw Exception("Error: Invalid binary value on definition entity. " + defValue);
	}

    if(!myRegex.IsMatch("^[0-9a-fA-F]+$", scValue.c_str())) {
		throw Exception("Error: Invalid binary value on system characteristics item entity. " + scValue);
	}

    string tmpdefValue = Common::ToUpper(defValue);
	string tmpscValue = Common::ToUpper(scValue);

	OvalEnum::ResultEnumeration result = OvalEnum::RESULT_ERROR;

	if(op == OvalEnum::OPERATION_EQUALS) {
		if(tmpdefValue.compare(tmpscValue) == 0) {
			result = OvalEnum::RESULT_TRUE;
		} else {
			result = OvalEnum::RESULT_FALSE;
		}
	} else if(op == OvalEnum::OPERATION_NOT_EQUAL) {
		if(tmpdefValue.compare(tmpscValue) != 0) {
			result = OvalEnum::RESULT_TRUE;
		} else {
			result = OvalEnum::RESULT_FALSE;			
		}
	} else {
		throw Exception("Error: Invalid operation. Operation: " + OvalEnum::OperationToString(op));
	}

	return result;
}

OvalEnum::ResultEnumeration EntityComparator::CompareBoolean(OvalEnum::Operation op, string defValue, string scValue) {
	
	OvalEnum::ResultEnumeration result = OvalEnum::RESULT_ERROR;

	bool defBoolValue = false;
	bool scBoolValue = false;
	
	if(defValue.compare("true") == 0 || defValue.compare("1") == 0) {
		defBoolValue = true;
	} else if(defValue.compare("false") == 0 || defValue.compare("0") == 0) {
		defBoolValue = false;
	} else {
		throw Exception("Error: Invalid boolean value on definition entity. " + defValue);
	}

	if(scValue.compare("true") == 0 || scValue.compare("1") == 0) {
		scBoolValue = true;
	} else if(scValue.compare("false") == 0 || scValue.compare("0") == 0) {
		scBoolValue = false;
	} else {
		throw Exception("Error: Invalid boolean value on system characteristics item entity. " + scValue);
	}

	if(op == OvalEnum::OPERATION_EQUALS) {
		if(defBoolValue == scBoolValue) {
			result = OvalEnum::RESULT_TRUE;
		} else {
			result = OvalEnum::RESULT_FALSE;
		}
	}

	else if(op == OvalEnum::OPERATION_NOT_EQUAL){
		if(defBoolValue != scBoolValue){
			result = OvalEnum::RESULT_TRUE;
		} else{
			result = OvalEnum::RESULT_FALSE;
		}
	}
	
	else {
		throw Exception("Error: Invalid operation. Operation: " + OvalEnum::OperationToString(op));
	} 

	return result;
}

OvalEnum::ResultEnumeration EntityComparator::CompareEvrString(OvalEnum::Operation op, string defValue, string scValue) {
	OvalEnum::ResultEnumeration result = OvalEnum::RESULT_ERROR;
	//EPOCH:VERSION-RELEASE, represented as found here: http://www.rpm.org/wiki/PackagerDocs/Dependencies#RequiringPackages
	// Validate the inputs align with the notion of an EVR string regex = "^(\d+):([^-]+)-([^-]+)$"
    REGEX myRegex;
    if(!myRegex.IsMatch("^(\\d+):([^-]+)-([^-]+)$", defValue.c_str())) {
		throw Exception("Error: Invalid EVR string value on definition entity. " + defValue);
	}

    if(!myRegex.IsMatch("^(\\d+):([^-]+)-([^-]+)$", scValue.c_str())) {
		throw Exception("Error: Invalid EVR string value on system characteristics item entity. " + scValue);
	}

	string installedEpochStr = EntityComparator::GetEpochFromEVR(scValue);
	string installedVersionStr = EntityComparator::GetVersionFromEVR(scValue);
	string installedReleaseStr = EntityComparator::GetReleaseFromEVR(scValue);
	string defEpochStr = EntityComparator::GetEpochFromEVR(defValue);
	string defVersionStr = EntityComparator::GetVersionFromEVR(defValue);
	string defReleaseStr = EntityComparator::GetReleaseFromEVR(defValue);

	int sense = 1; // default to later

	int defEpochInt = atoi(defEpochStr.c_str());
	int installedEpochInt = atoi(installedEpochStr.c_str());

	if(defEpochInt == installedEpochInt) {

		sense = rpmvercmp(installedVersionStr.c_str(),defVersionStr.c_str());
		if (sense == 0) {
			sense = rpmvercmp(installedReleaseStr.c_str(),defReleaseStr.c_str());
		}
	} else {
		
		sense = (installedEpochInt > defEpochInt)? 1 : -1;

	}

	// convert sense value to a result based on operator
	if(op == OvalEnum::OPERATION_EQUALS) {
		if(sense == 0) {
			result = OvalEnum::RESULT_TRUE;
		} else {
			result = OvalEnum::RESULT_FALSE;
		}
	} else if(op == OvalEnum::OPERATION_NOT_EQUAL) {
		if(sense == 0) {
			result = OvalEnum::RESULT_FALSE;
		} else {
			result = OvalEnum::RESULT_TRUE;
		}
	} else if(op == OvalEnum::OPERATION_LESS_THAN) {
		if(sense == -1) {
			result = OvalEnum::RESULT_TRUE;
		} else {
			result = OvalEnum::RESULT_FALSE;
		}
	} else if(op == OvalEnum::OPERATION_LESS_THAN_OR_EQUAL) {		
		if(sense == -1 || sense == 0) {
			result = OvalEnum::RESULT_TRUE;
		} else {
			result = OvalEnum::RESULT_FALSE;
		}
	} else if(op == OvalEnum::OPERATION_GREATER_THAN) {
		if(sense == 1) {
			result = OvalEnum::RESULT_TRUE;
		} else {
			result = OvalEnum::RESULT_FALSE;
		}
	} else if(op == OvalEnum::OPERATION_GREATER_THAN_OR_EQUAL) {
		if(sense == 1 || sense == 0) {
			result = OvalEnum::RESULT_TRUE;
		} else {
			result = OvalEnum::RESULT_FALSE;
		}
	} else {
		throw Exception("Error: Invalid operation. Operation: " + OvalEnum::OperationToString(op));
	} 

	return result;
}

/* compare alpha and numeric segments of two versions */
/* return 1: a is newer than b */
/*        0: a and b are the same version */
/*       -1: b is newer than a */
int EntityComparator::rpmvercmp(const char * a, const char * b) {
    char oldch1, oldch2;
    char * str1, * str2;
    char * one, * two;
    int rc;
    int isnum;

    /* easy comparison to see if versions are identical */
    if (!strcmp(a, b)) return 0;

// converted to malloc to run on windows.
//    str1 = alloca(strlen(a) + 1);
//    str2 = alloca(strlen(b) + 1);
	  str1 = (char*)malloc(strlen(a)+1);
	  str2 = (char*)malloc(strlen(b)+1);

    strcpy(str1, a);
    strcpy(str2, b);

    one = str1;
    two = str2;

    /* loop through each version segment of str1 and str2 and compare them */
    while (*one && *two) {
        while (*one && !EntityComparator::xisalnum(*one)) one++;
        while (*two && !EntityComparator::xisalnum(*two)) two++;

        str1 = one;
        str2 = two;

        /* grab first completely alpha or completely numeric segment */
        /* leave one and two pointing to the start of the alpha or numeric */
        /* segment and walk str1 and str2 to end of segment */
        if (EntityComparator::xisdigit(*str1)) {
            while (*str1 && EntityComparator::xisdigit(*str1)) str1++;
            while (*str2 && EntityComparator::xisdigit(*str2)) str2++;
            isnum = 1;
        } else {
            while (*str1 && EntityComparator::xisalpha(*str1)) str1++;
            while (*str2 && EntityComparator::xisalpha(*str2)) str2++;
            isnum = 0;
        }

        /* save character at the end of the alpha or numeric segment */
        /* so that they can be restored after the comparison */
        oldch1 = *str1;
        *str1 = '\0';
        oldch2 = *str2;
        *str2 = '\0';

        /* take care of the case where the two version segments are */
        /* different types: one numeric, the other alpha (i.e. empty) */
        if (one == str1) return -1;     /* arbitrary */
        /* XXX See patch #60884 (and details) from bugzilla #50977. */
        if (two == str2) return (isnum ? 1 : -1);

        if (isnum) {
            /* this used to be done by converting the digit segments */
            /* to ints using atoi() - it's changed because long  */
            /* digit segments can overflow an int - this should fix that. */

            /* throw away any leading zeros - it's a number, right? */
            while (*one == '0') one++;
            while (*two == '0') two++;

            /* whichever number has more digits wins */
            if (strlen(one) > strlen(two)) return 1;
            if (strlen(two) > strlen(one)) return -1;
        }

        /* strcmp will return which one is greater - even if the two */
        /* segments are alpha or if they are numeric.  don't return  */
        /* if they are equal because there might be more segments to */
        /* compare */
        rc = strcmp(one, two);
        if (rc) return (rc < 1 ? -1 : 1);

        /* restore character that was replaced by null above */
        *str1 = oldch1;
        one = str1;
        *str2 = oldch2;
        two = str2;
    }

    /* this catches the case where all numeric and alpha segments have */
    /* compared identically but the segment sepparating characters were */
    /* different */
    if ((!*one) && (!*two)) return 0;

    /* whichever version still has characters left over wins */
    if (!*one) return -1; else return 1;
}

int EntityComparator::xisalnum(int c) {
    return (EntityComparator::xisalpha(c) || EntityComparator::xisdigit(c));
}
int EntityComparator::xisdigit(int c)  {
    return (c >= '0' && c <= '9');
}
int EntityComparator::xisalpha(int c) {
    return (EntityComparator::xislower(c) || EntityComparator::xisupper(c));
}
int EntityComparator::xislower(int c) {
	return (c >= 'a' && c <= 'z');
}
int EntityComparator::xisupper(int c) {
	return (c >= 'A' && c <= 'Z');
}

OvalEnum::ResultEnumeration EntityComparator::CompareFloat(OvalEnum::Operation op, string defValue, string scValue) {
	// Validate the inputs align with the float regex = "^[-+]?\d*\.?\d+([eE][-+]?\d+)?$"
    REGEX myRegex;
    if(!myRegex.IsMatch("^[-+]?\\d*\\.?\\d+([eE][-+]?\\d+)?$", defValue.c_str())) {
		throw Exception("Error: Invalid float value on definition entity. " + defValue);
	}

    if(!myRegex.IsMatch("^[-+]?\\d*\\.?\\d+([eE][-+]?\\d+)?$", scValue.c_str())) {
		throw Exception("Error: Invalid float value on system characteristics item entity. " + scValue);
	}

	OvalEnum::ResultEnumeration result = OvalEnum::RESULT_ERROR;

	// convert the strings to doubles
	double defDouble = atof(defValue.c_str());
	double scDouble = atof(scValue.c_str());

	if(op == OvalEnum::OPERATION_EQUALS) {
		if(scDouble == defDouble) {
			result = OvalEnum::RESULT_TRUE;
		} else {
			result = OvalEnum::RESULT_FALSE;
		}
	} else if(op == OvalEnum::OPERATION_NOT_EQUAL) {
		if(scDouble != defDouble) {
			result = OvalEnum::RESULT_TRUE;
		} else {
			result = OvalEnum::RESULT_FALSE;
		}
	} else if(op == OvalEnum::OPERATION_LESS_THAN) {
		
		if(scDouble < defDouble) {
			result = OvalEnum::RESULT_TRUE;
		} else {
			result = OvalEnum::RESULT_FALSE;
		}
	} else if(op == OvalEnum::OPERATION_LESS_THAN_OR_EQUAL) {
		
		if(scDouble <= defDouble) {
			result = OvalEnum::RESULT_TRUE;
		} else {
			result = OvalEnum::RESULT_FALSE;
		}
	} else if(op == OvalEnum::OPERATION_GREATER_THAN) {
		
		if(scDouble > defDouble) {
			result = OvalEnum::RESULT_TRUE;
		} else {
			result = OvalEnum::RESULT_FALSE;
		}
	} else if(op == OvalEnum::OPERATION_GREATER_THAN_OR_EQUAL) {
		
		if(scDouble >= defDouble) {
			result = OvalEnum::RESULT_TRUE;
		} else {
			result = OvalEnum::RESULT_FALSE;
		}
	} else {
		throw Exception("Error: Invalid operation. Operation: " + OvalEnum::OperationToString(op));
	} 

	return result;
}

OvalEnum::ResultEnumeration EntityComparator::CompareIosVersion(OvalEnum::Operation /*op*/, string defValue, string scValue) {
	// TODO need to implement
	
	throw Exception("Error: Comparing IOS versions has not been implemented.");
}

OvalEnum::ResultEnumeration EntityComparator::CompareInteger(OvalEnum::Operation op, string defValue, string scValue) {
	OvalEnum::ResultEnumeration result = OvalEnum::RESULT_ERROR;
	// Validate the inputs align with the integer regex = "^[-+]?\d+$"
    REGEX myRegex;
    if(!myRegex.IsMatch("^[-+]?\\d+$", defValue.c_str())) {
		throw Exception("Error: Invalid integer value on definition entity. " + defValue);
	}

    if(!myRegex.IsMatch("^[-+]?\\d+$", scValue.c_str())) {
		throw Exception("Error: Invalid integer value on system characteristics item entity. " + scValue);
	}
	try {
		//int base = 0;
		long long defInt = 0;
		long long scInt = 0;
		//char * e1 = NULL;
		//char * e2 = NULL;

		// Convert the string defValue to a long long intege
		if (!Common::FromString(defValue, &defInt))
			throw Exception("Error: Invalid integer value on definition entity. " + defValue);
//		defInt = Common::StringToLongLong( ( char* ) defValue.c_str() , &e1 , base );
	
		// Convert the string scValue to a long long integer
		if (!Common::FromString(scValue, &scInt))
			throw Exception("Error: Invalid integer value on system characteristics item entity. " + scValue);
		//scInt = Common::StringToLongLong( ( char* ) scValue.c_str() , &e2 , base );
		/*
		if ( e1 != NULL ){
			*e1 = (char) NULL;
		}

		if ( e2 != NULL ){
			*e2 = (char) NULL;
		}
		*/
		if(op == OvalEnum::OPERATION_EQUALS) {
			if(scInt == defInt) {
				result = OvalEnum::RESULT_TRUE;
			} else {
				result = OvalEnum::RESULT_FALSE;
			}
		} else if(op == OvalEnum::OPERATION_NOT_EQUAL) {
			if(scInt != defInt) {
				result = OvalEnum::RESULT_TRUE;
			} else {
				result = OvalEnum::RESULT_FALSE;
			}
		} else if(op == OvalEnum::OPERATION_LESS_THAN) {
			
			if(scInt < defInt) {
				result = OvalEnum::RESULT_TRUE;
			} else {
				result = OvalEnum::RESULT_FALSE;
			}
		} else if(op == OvalEnum::OPERATION_LESS_THAN_OR_EQUAL) {
			
			if(scInt <= defInt) {
				result = OvalEnum::RESULT_TRUE;
			} else {
				result = OvalEnum::RESULT_FALSE;
			}
		} else if(op == OvalEnum::OPERATION_GREATER_THAN) {
			
			if(scInt > defInt) {
				result = OvalEnum::RESULT_TRUE;
			} else {
				result = OvalEnum::RESULT_FALSE;
			}
		} else if(op == OvalEnum::OPERATION_GREATER_THAN_OR_EQUAL) {
			
			if(scInt >= defInt) {
				result = OvalEnum::RESULT_TRUE;
			} else {
				result = OvalEnum::RESULT_FALSE;
			}
		} else if(op == OvalEnum::OPERATION_BITWISE_AND) {
			if ( ( defInt & scInt ) == defInt ){
				result = OvalEnum::RESULT_TRUE;
			}else{
				result = OvalEnum::RESULT_FALSE;
			}	
		} else if(op == OvalEnum::OPERATION_BITWISE_OR) {		
			if ( ( defInt | scInt ) == defInt ){
				result = OvalEnum::RESULT_TRUE;
			}else{
				result = OvalEnum::RESULT_FALSE;
			}
		} else {
			throw Exception("Error: Invalid operation. Operation: " + OvalEnum::OperationToString(op));
		} 
	}catch (string errorMessage){
		cout << errorMessage;	
	}

	return result;
}

OvalEnum::ResultEnumeration EntityComparator::CompareRecord(OvalEnum::Operation op, AbsEntityValueVector defValue, AbsEntityValueVector scValue) {
	OvalEnum::ResultEnumeration result = OvalEnum::RESULT_ERROR;
	// Equals is the only operation allowed on a entity of type record.
	if ( op == OvalEnum::OPERATION_EQUALS ){
		IntVector recordResults;
		IntVector fieldEntityResults;
		for(AbsEntityValueVector::iterator defIt = defValue.begin(); defIt != defValue.end(); defIt++){
			StateOrObjectFieldEntityValue* sfev = dynamic_cast<StateOrObjectFieldEntityValue*>(*defIt);
			if (!sfev) throw Exception("Don't know how to handle entity value of type: " + string(typeid(*defIt).name()));
			IntVector fieldResults;
			
			for(AbsEntityValueVector::iterator scIt = scValue.begin(); scIt != scValue.end(); scIt++){
				ItemFieldEntityValue* ifev = (ItemFieldEntityValue*)(*scIt);
				if ( sfev->GetName().compare(ifev->GetName()) == 0 ){
					fieldResults.push_back(sfev->Analyze(ifev));
				}
			}
			// If a field in the object/state entity is not present in the ItemEntity report an error for the field
			if ( fieldResults.size() == 0 ) {
				fieldResults.push_back(OvalEnum::RESULT_ERROR);
				Log::Debug("Warning: encountered state record field named \""+
					sfev->GetName()+"\" which doesn't match any fields in the item's record.");
			}
			OvalEnum::ResultEnumeration fieldFinalResult = OvalEnum::CombineResultsByCheck(&fieldResults, sfev->GetEntityCheck());
			recordResults.push_back(fieldFinalResult);
		}
		// Results of a record is the result of ANDing all of the fields together
		result = OvalEnum::CombineResultsByCheck(&recordResults,OvalEnum::CHECK_ALL);
	}else{
		throw Exception("Error: Invalid operation. Operation: "+OvalEnum::OperationToString(op)+". Only 'equals' is allowed for the 'record' datatype.");
	}

	return result;
}

OvalEnum::ResultEnumeration EntityComparator::CompareString(OvalEnum::Operation op, string defValue, string scValue) {
	OvalEnum::ResultEnumeration result = OvalEnum::RESULT_ERROR;

	if(op == OvalEnum::OPERATION_EQUALS) {
		if(defValue.compare(scValue) == 0) {
			result = OvalEnum::RESULT_TRUE;
		} else {
			result = OvalEnum::RESULT_FALSE;
		}
	} else if(op == OvalEnum::OPERATION_CASE_INSENSITIVE_EQUALS) {

		string upperDefValue = Common::ToUpper(defValue);
		string upperSCValue = Common::ToUpper(scValue);

		if(upperDefValue.compare(upperSCValue) == 0) {
			result = OvalEnum::RESULT_TRUE;
		} else {
			result = OvalEnum::RESULT_FALSE;
		}

	} else if(op == OvalEnum::OPERATION_NOT_EQUAL) {
		if(defValue.compare(scValue) != 0) {
			result = OvalEnum::RESULT_TRUE;
		} else {
			result = OvalEnum::RESULT_FALSE;			
		}
	} else if(op == OvalEnum::OPERATION_CASE_INSENSITIVE_NOT_EQUAL) {

		string upperDefValue = Common::ToUpper(defValue);
		string upperSCValue = Common::ToUpper(scValue);

		if(upperDefValue.compare(upperSCValue) != 0) {
			result = OvalEnum::RESULT_TRUE;
		} else {
			result = OvalEnum::RESULT_FALSE;
		}

	} else if(op == OvalEnum::OPERATION_PATTERN_MATCH) {
		REGEX myRegex;
		if(myRegex.IsMatch(defValue.c_str(), scValue.c_str())) {
			result = OvalEnum::RESULT_TRUE;
		} else {			
			result = OvalEnum::RESULT_FALSE;
		}
	} else {
		throw Exception("Error: Invalid operation. Operation: " + OvalEnum::OperationToString(op));
	}

	return result;
}

OvalEnum::ResultEnumeration EntityComparator::CompareVersion(OvalEnum::Operation op, string defValue, string scValue) {
	OvalEnum::ResultEnumeration result = OvalEnum::RESULT_ERROR;
	//ex: 1@2@31#5.3
	//ex: 1.5.3
	// Validate the inputs align with the version regex = "^\d+([\D]\d+)*?$"
    REGEX myRegex;
    if(!myRegex.IsMatch("^\\d+([\\D]\\d+)*?$", defValue.c_str())) {
		throw Exception("Error: Invalid version value on definition entity. " + defValue);
	}

    if(!myRegex.IsMatch("^\\d+([\\D]\\d+)*?$", scValue.c_str())) {
		throw Exception("Error: Invalid version value on system characteristics item entity. " + scValue);
	}
	// Invalid ops first
	if(op == OvalEnum::OPERATION_PATTERN_MATCH || op == OvalEnum::OPERATION_BITWISE_AND || op == OvalEnum::OPERATION_BITWISE_OR) {
		throw Exception("Error: Invalid operation. Operation: " + OvalEnum::OperationToString(op));	
	}

	// Create two vectors of the version components. 
    LongLongVector* defValues = EntityComparator::ParseVersionStr(defValue);
	LongLongVector* scValues = EntityComparator::ParseVersionStr(scValue);

	// pad the vectors such that they are the same length
	int lengthDiff = defValues->size() - scValues->size();
	while(lengthDiff != 0) {
		if(lengthDiff > 0) {
			scValues->push_back(0);
		} else if(lengthDiff < 0) {
			defValues->push_back(0);
		}

		lengthDiff = defValues->size() - scValues->size();
	}

	//	Loop through the vector of definition version values.
	for(unsigned int i = 0; i < defValues->size(); i++) {
		long long def = defValues->at(i);
		long long sc = scValues->at(i); 
		bool isLastValue = false;
		if(defValues->size() == (i+1)) {
			isLastValue = true;
		}

		if(op == OvalEnum::OPERATION_EQUALS) {
			if(def == sc) {
				result = OvalEnum::RESULT_TRUE;
			} else {
				result = OvalEnum::RESULT_FALSE;
				break;
			}
		} else if(op == OvalEnum::OPERATION_NOT_EQUAL) {
			if(def != sc) {
				result = OvalEnum::RESULT_TRUE;
				break;
			} else if(def == sc) {
				if(!isLastValue) {
					continue;
				} else {
					result = OvalEnum::RESULT_FALSE;
					break;
				}
			}
		} else if(op == OvalEnum::OPERATION_GREATER_THAN) {
			if(def > sc) {
				result = OvalEnum::RESULT_FALSE;
				break;
			} else if(def == sc) {
				if(!isLastValue) {
                    continue;
				} else {
					result = OvalEnum::RESULT_FALSE;
					break;
				}
			} else {
				result = OvalEnum::RESULT_TRUE;
				break;
			}
		} else if(op == OvalEnum::OPERATION_GREATER_THAN_OR_EQUAL) {
			if(def > sc) {
				result = OvalEnum::RESULT_FALSE;
				break;
			} else if(def < sc) {
				result = OvalEnum::RESULT_TRUE;
				break;
			} else if(def == sc) {
				result = OvalEnum::RESULT_TRUE;
				continue;
			}
		} else if(op == OvalEnum::OPERATION_LESS_THAN) {
			if(def < sc) {
				result = OvalEnum::RESULT_FALSE;
				break;
			} else if(def == sc) {
				if(!isLastValue) {
                    continue;
				} else {
					result = OvalEnum::RESULT_FALSE;
					break;
				}
			} else {
				result = OvalEnum::RESULT_TRUE;
				break;
			}
		} else if(op == OvalEnum::OPERATION_LESS_THAN_OR_EQUAL) {
			if(def < sc) {
				result = OvalEnum::RESULT_FALSE;
				break;
			} else if(def > sc) {
				result = OvalEnum::RESULT_TRUE;
				break;
			} else if(def == sc) {
				result = OvalEnum::RESULT_TRUE;
				continue;
			}			
		}
	}

	// delete the vectors of integers
	delete defValues;
	delete scValues;
	
	return result;
}

LongLongVector* EntityComparator::ParseVersionStr(string versionStr) {
	auto_ptr<LongLongVector> tokens(new LongLongVector());
	REGEX re;
	vector<StringVector> matches;
	re.GetAllMatchingSubstrings("\\d+", versionStr, matches);
	// convert the string matches to long longs
	for (vector<StringVector>::iterator iter = matches.begin();
		iter != matches.end(); ++iter) {
		long long val;
		// should always succeed since the string matched regex \d+
		Common::FromString((*iter)[0], &val);
		tokens->push_back(val);
	}

	return tokens.release();
}

string EntityComparator::GetEpochFromEVR(string evrStr) {
	string epoch = evrStr.substr(0, evrStr.find(":"));
	return epoch;
}

string EntityComparator::GetVersionFromEVR(string evrStr) {
	unsigned int colon = evrStr.find(":");
	unsigned int hyphen = evrStr.find("-");
	int count = (hyphen) - (colon+1);
	string version = evrStr.substr(colon +1, count);
	return version;
}
string EntityComparator::GetReleaseFromEVR(string evrStr) {
	unsigned int hyphen = evrStr.find("-");
	int count = evrStr.length() - (hyphen+1);
	string release = evrStr.substr(hyphen+1, count);
	return release;
}

OvalEnum::ResultEnumeration EntityComparator::CompareIpv4Address(OvalEnum::Operation op, string defValue, string scValue) {

	OvalEnum::ResultEnumeration result = OvalEnum::RESULT_ERROR;
	Ipv4Address defAddr(defValue), scAddr(scValue);

	switch (op) {
	case OvalEnum::OPERATION_EQUALS:
		result = scAddr == defAddr ? 
			OvalEnum::RESULT_TRUE : OvalEnum::RESULT_FALSE;
		break;

	case OvalEnum::OPERATION_NOT_EQUAL:
		result = scAddr == defAddr ? 
			OvalEnum::RESULT_FALSE : OvalEnum::RESULT_TRUE;
		break;

	case OvalEnum::OPERATION_GREATER_THAN:
		result = scAddr.getNetmask().s_addr != defAddr.getNetmask().s_addr ?
			OvalEnum::RESULT_ERROR :
				scAddr.getInAddr().s_addr > defAddr.getInAddr().s_addr ?
					OvalEnum::RESULT_TRUE : OvalEnum::RESULT_FALSE;
		break;

	case OvalEnum::OPERATION_GREATER_THAN_OR_EQUAL:
		result = scAddr.getNetmask().s_addr != defAddr.getNetmask().s_addr ?
			OvalEnum::RESULT_ERROR :
				scAddr.getInAddr().s_addr >= defAddr.getInAddr().s_addr ?
					OvalEnum::RESULT_TRUE : OvalEnum::RESULT_FALSE;
		break;

	case OvalEnum::OPERATION_LESS_THAN:
		result = scAddr.getNetmask().s_addr != defAddr.getNetmask().s_addr ?
			OvalEnum::RESULT_ERROR :
				scAddr.getInAddr().s_addr < defAddr.getInAddr().s_addr ?
					OvalEnum::RESULT_TRUE : OvalEnum::RESULT_FALSE;
		break;

	case OvalEnum::OPERATION_LESS_THAN_OR_EQUAL:
		result = scAddr.getNetmask().s_addr != defAddr.getNetmask().s_addr ?
			OvalEnum::RESULT_ERROR :
				scAddr.getInAddr().s_addr <= defAddr.getInAddr().s_addr ?
					OvalEnum::RESULT_TRUE : OvalEnum::RESULT_FALSE;
		break;

	case OvalEnum::OPERATION_SUBSET_OF:
		result = scAddr.subsetOf(defAddr) ? 
			OvalEnum::RESULT_TRUE : OvalEnum::RESULT_FALSE;
		break;

	case OvalEnum::OPERATION_SUPERSET_OF:
		result = scAddr.supersetOf(defAddr) ? 
			OvalEnum::RESULT_TRUE : OvalEnum::RESULT_FALSE;
		break;

	default:
		throw Exception("Error: Invalid operation. Operation: " + OvalEnum::OperationToString(op));
	}

	return result;
}

namespace {

	bool Ipv4Address::subsetOf(const Ipv4Address &otherAddr) const {
		if (getPrefixSize() < otherAddr.getPrefixSize())
			return false;
		// here, otherAddr must have an equal or shorter prefix
		in_addr otherNetmask = otherAddr.getNetmask();
		return (otherAddr.getInAddr().s_addr & otherNetmask.s_addr) ==
			(getInAddr().s_addr & otherNetmask.s_addr);
	}

	void Ipv4Address::setFromString(const string &addrStr) {
		REGEX re;
		string netmaskPart, ipPart;

		size_t slashOffset = addrStr.find('/');
		if (slashOffset == string::npos) 
			ipPart = addrStr;
		else {
			if (slashOffset < addrStr.size()-1)
				netmaskPart = addrStr.substr(slashOffset+1);
			ipPart = addrStr.substr(0, slashOffset);
		}

		// ---- IP part
		this->addr = dottedQuadToInAddr(ipPart);

		// ---- netmask/prefix part
		if (netmaskPart.empty()) {
			// if no '/' was found, interpret as a host address
			// (prefixSize==32)
			this->netmask.s_addr = 0xFFFFFFFFU;
			this->prefixSize = 32;

		} else if (re.IsMatch("^\\d+$", netmaskPart.c_str())) {
			if (!Common::FromString(netmaskPart, &this->prefixSize))
				throw Exception("Invalid ipv4_address syntax: "+addrStr);
			if (this->prefixSize > 32)
				throw Exception("Invalid ipv4_address syntax (CIDR prefix size out of range): "+addrStr);

			// I found out the hard way that bit-shifting by greater than
			// or equal to 8*sizeof(T) bits is undefined... :-P  So gotta always
			// special-case 32-bit shifts!
			this->netmask.s_addr = this->prefixSize==0 ? 0 : (0xFFFFFFFFU << (32 - this->prefixSize));

		} else if (re.IsMatch("^\\d+\\.\\d+\\.\\d+\\.\\d+$", netmaskPart.c_str())) {
			this->netmask = dottedQuadToInAddr(netmaskPart);

			// make sure the value consists all 1 bits on the left and
			// all 0 bits on the right.  So no "01" bit sequences.
			// The loop does double-duty, also finding the first 1 bit
			// so we can compute the prefix size.
			int idxOfFirst1 = -1;
			for (int i=0; i<31; ++i) {
				if (this->netmask.s_addr & (1U << i)) {
					if (!(this->netmask.s_addr & (1U << (i+1))))
						throw Exception("Invalid ipv4_address syntax (netmask doesn't represent a prefix): "+addrStr);
					else if (idxOfFirst1 == -1)
						idxOfFirst1 = i;
				}
			}

			// the above loop only looped up to bit 30 (indexed
			// starting at 0).  So if we didn't find a 1 bit, we check 
			// bit 31 here.  If the netmask is 0.0.0.0, the prefix
			// size is also set to 0.
			if (idxOfFirst1 == -1) {
				if (this->netmask.s_addr & 0x80000000U)
					this->prefixSize = 1;
				else
					this->prefixSize = 0;
			} else
				this->prefixSize = 32 - idxOfFirst1;

		} else
			throw Exception("Invalid ipv4_address syntax (bad CIDR prefix size or netmask):"+addrStr);

		// Strip off the low bits, since we're ignoring them.
		// This will simplify other code, which can assume that
		// the lower order bits are always 0.
		this->addr.s_addr &= this->netmask.s_addr;
	}

	in_addr dottedQuadToInAddr(const string &addrStr) {
		REGEX re;
		if (!re.IsMatch("^\\d+\\.\\d+\\.\\d+\\.\\d+$", addrStr.c_str()))
			throw Exception("Invalid ipv4 address syntax: "+addrStr);

		// can't use inet_addr(), since it would interpret
		// 0-prefixed values as octal, 0x as hex, etc.  But
		// the oval spec doesn't allow non-digits in an octet,
		// and doesn't ascribe any special meaning to leading zeros.
		vector<StringVector> matches;
		re.GetAllMatchingSubstrings("\\d+", addrStr, matches);
		if (matches.size() != 4)
			throw Exception("Invalid ipv4 address syntax (must be exactly 4 octets): "+addrStr);

		uint32_t octet[4];
		for (int i=0; i<4; ++i) {
			if (!Common::FromString(matches[i][0], &octet[i]))
				throw Exception("Couldn't parse octet in "+addrStr+" ("+matches[i][0]+')');
			if (octet[i] > 255) 
				throw Exception("Octet out of range in "+addrStr);
		}

		in_addr result;
		result.s_addr =
			(octet[0]<<24) |
			(octet[1]<<16) |
			(octet[2]<<8) |
			octet[3];

		return result;
	}

	ostream &operator<<(ostream &out, const Ipv4Address &addr) {
		// inet_ntoa is gonna expect network byte order, so I gotta
		// flip the bytes.
		in_addr tmp;
		tmp.s_addr = htonl(addr.getInAddr().s_addr);
		out << inet_ntoa(tmp);
		if (addr.getPrefixSize() < 32)
			out << '/' << addr.getPrefixSize();
		return out;
	}
}
