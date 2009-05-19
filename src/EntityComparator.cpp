//
//
//****************************************************************************************//
// Copyright (c) 2002-2009, The MITRE Corporation
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

#include "EntityComparator.h"
//****************************************************************************************//
//								EntityComparator Class									  //	
//****************************************************************************************//
OvalEnum::ResultEnumeration EntityComparator::CompareBinary(OvalEnum::Operation op, string defValue, string scValue) {

    // Validate the inputs align with the notion of binary regex = "[0-9a-fA-F]"
    REGEX myRegex;
    if(!myRegex.IsMatch("[0-9a-fA-F]", defValue.c_str())) {
		throw Exception("Error: Invalid binary value on definition entity. " + defValue);
	}

    if(!myRegex.IsMatch("[0-9a-fA-F]", scValue.c_str())) {
		throw Exception("Error: Invalid binary value on system characteristics item entity. " + defValue);
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

OvalEnum::ResultEnumeration EntityComparator::CompareIosVersion(OvalEnum::Operation op, string defValue, string scValue) {
	OvalEnum::ResultEnumeration result = OvalEnum::RESULT_ERROR;

	// TODO need to implement
	
	throw Exception("Error: Comparing IOS versions has not been implemented.");
	
	return result;
}

OvalEnum::ResultEnumeration EntityComparator::CompareInteger(OvalEnum::Operation op, string defValue, string scValue) {
	
	OvalEnum::ResultEnumeration result = OvalEnum::RESULT_ERROR;
	
	int base = 0;

	long long defInt;
	long long scInt;

	char * e1 = NULL;
	char * e2 = NULL;
	
	// Convert the string defValue to a long long integer
	try {
		defInt = Common::StringToLongLong( ( char* ) defValue.c_str() , &e1 , base );
	}catch (string errorMessage){
		cout << errorMessage;	
	}

    // Convert the string scValue to a long long integer
	try {
		scInt = Common::StringToLongLong( ( char* ) scValue.c_str() , &e2 , base );
	}catch (string errorMessage){
		cout << errorMessage;	
	}

	if ( e1 != NULL ){
		*e1 = NULL;
	}

	if ( e2 != NULL ){
		*e2 = NULL;
	}

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
	
	if(versionStr.compare("") == 0) {
		throw Exception("Unable to parse version string. An empty string was provided.");
	}
	
	LongLongVector* tokens = new LongLongVector();

    long long tokenInt;
    char* endptr;
	int base = 10;
	
    // Get index of first delimiter
	size_t index = versionStr.find_first_not_of("0123456789"); 
	
	// Get index of last delimiter
	size_t lastindex = versionStr.find_last_not_of("0123456789");
	
	if(index == string::npos) {
        
		// No delmiter found and not an empty string.
		// Simply try to convert the versionStr to a long long integer and return it as the only token.
        try {
            tokenInt = Common::StringToLongLong( ( char* ) versionStr.c_str() , &endptr , base );
        }catch (string errorMessage){
            cout << errorMessage;	
        }

		if ( endptr != NULL ){
			*endptr = NULL;
		}

		tokens->push_back(tokenInt);

	} else {
	
		char delm = versionStr.at(index);
		char* delimiter = (char*)malloc(sizeof(char)*2);
		delimiter[0] = delm;
		delimiter[1] = '\0';

		char* theString = (char*)malloc(sizeof(char)*(versionStr.length()+1));
		theString = strcpy(theString, versionStr.c_str());
		char* token = strtok(theString, delimiter);
		
		
		if(token == NULL) {

			if(theString != NULL) {
				free(theString);
				theString = NULL;
			}
			if(tokens != NULL) {
				delete tokens;
				tokens = NULL;
			}
			if(delimiter != NULL) {
				free(delimiter);
				delimiter = NULL;
			}

			throw Exception("Error parsing version string. A delimiter was found, but no other components to the version were found. Input version string: \'" + versionStr + "\' delimiter detected: \'" + delm + "\'");
		} else {
			
			// If index is zero that means the version starts with a delimiter which is an invalid version value
			if ( index == 0 ){

				if(theString != NULL) {
					free(theString);
					theString = NULL;
				}
				if(tokens != NULL) {
					delete tokens;
					tokens = NULL;
				}
				if(delimiter != NULL) {
					free(delimiter);
					delimiter = NULL;
				}

				throw Exception("Error parsing version string. Version string: \'" + versionStr + "\' begins with a delimiter. Please consult the version datatype definition.");
			}

			// If index of last delimiter is equal to the length of the version string - 1 that means the version ends with a delimiter which is an invalid version value
			if ( lastindex == (versionStr.length() - 1) ){
				
				if(theString != NULL) {
					free(theString);
					theString = NULL;
				}
				if(tokens != NULL) {
					delete tokens;
					tokens = NULL;
				}
				if(delimiter != NULL) {
					free(delimiter);
					delimiter = NULL;
				}
				
				throw Exception("Error parsing version string. Version string: \'" + versionStr + "\' ends with a delimiter. Please consult the version datatype definition.");
			}

			// Create temporary copy of the version string so that the original is not destroyed
			string versionTempStr = versionStr;

			// Flag to determine if the previous token was a non-int
			bool prevTokenNonInt = false;

			// Keeps track of the total length of all the characters seen that make up an int token plus a extra character after each int token seen to account for a delimiter character
			// Note by doing so when you see the last int token it will add on an extra delimiter character which means the total length of all characters seen that make
			// up a version number will be equivalent to the totalTokenLength - 1
			int totalTokenLength = 0;

			while(token != NULL) {
	
				//	Get the first token as a string
				string tokenStr = token;
	
				// Check that the token is an integer
				size_t nonInt = tokenStr.find_first_not_of("0123456789");
	
				// If a non-int then the delimiter is different than the previous one
				if(nonInt != string::npos) {

					// Get the index of the new delimiter
					index = versionTempStr.find_first_not_of("0123456789");

					// Get new delimiter character
					delm = versionTempStr.at(index);
					
					// Put the delimiter character in a null terminated string
					delimiter[0] = delm;
					delimiter[1] = '\0';

					// Copy the current version string into a C string
					theString = strcpy(theString,versionTempStr.c_str());
					
					// Get the next token
					token = strtok(theString,delimiter);
					
					// If previous token was a non-int that means there were consecutive delimiters which is invalid
					if ( prevTokenNonInt ){
			
						if(theString != NULL) {
							free(theString);
							theString = NULL;
						}
						if(tokens != NULL) {
							delete tokens;
							tokens = NULL;
						}
						if(delimiter != NULL) {
							free(delimiter);
							delimiter = NULL;
						}

						throw Exception("Error parsing version string. Multiple consecutive delimiters found in version string: \'" + versionStr + "\'. Please consult the version datatype definition.");
					}

					// Last token was a non-int set flag to true
					prevTokenNonInt = true;

				}else{

					// Found an int so add the length of the token to the totalTokenLength + 1 to account for the delimiter
					totalTokenLength = totalTokenLength + strlen(token) + 1;

					// Convert the token to a long long integer
					try {
                        tokenInt = Common::StringToLongLong( ( char* ) token , &endptr , base );
                    }catch (string errorMessage){
                        cout << errorMessage;	
                    }

					if ( endptr != NULL ){
						*endptr = NULL;
					}

					// Add it to the vector
					tokens->push_back(tokenInt);

					// Get the next token
					token = strtok(NULL, delimiter);
					
					// If token is not null update the temp string to reflect the remaining version data
					if ( token != NULL ){ 	
						versionTempStr = versionTempStr.substr(versionTempStr.find(token));					
					}

					// Last token was an int set flag to false
					prevTokenNonInt = false;

				}
			}

			// Check for multiple consecutive delimiters in a version string
			// Compare the length of the original version string to the totalTokenLength - 1 to remove the extra delimiter that was added when the last int token was found
			if ( strlen(versionStr.c_str()) != (totalTokenLength-1) ){
				
				if(theString != NULL) {
					free(theString);
					theString = NULL;
				}
				if(tokens != NULL) {
					delete tokens;
					tokens = NULL;
				}
				if(delimiter != NULL) {
					free(delimiter);
					delimiter = NULL;
				}
				
				throw Exception("Error parsing version string. Multiple consecutive delimiters found in version string: \'" + versionStr + "\'. Please consult the version datatype definition.");
			}

		}
	
		if(delimiter != NULL) {
			free(delimiter);
			delimiter = NULL;
		}

		if(theString != NULL) {
			free(theString);
			theString = NULL;
		}
	}
	
	return tokens;
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

