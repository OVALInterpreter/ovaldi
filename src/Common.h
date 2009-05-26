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

/**
  If XmlCommon.h is needed in a Windows environment, Common.h should be included
  instead in order to avoid conflicts with msxml.h
**/


#ifndef COMMON_H
#define COMMON_H

#ifdef WIN32
	#pragma warning(disable:4786)
	#include <aclapi.h>
	#include <windows.h>
	#include <lmerr.h>
#endif

#include "Exception.h"
#include "Log.h"
#include "REGEX.h"
#include "math.h"
#include "errno.h"

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <set>
#include <time.h>
#include <sstream>
#include <algorithm>
#include <utility>


#include "XmlCommon.h"
#include "DocumentManager.h"

using namespace std;

/**
	A vector for storing strings.
*/
typedef vector < string, allocator<string> > StringVector;

/**
    A set for storing unique strings.
*/
typedef set < string > StringSet;

/**
	A vector for storing integers.
*/
typedef vector < int, allocator<int> > IntVector;

/**
	A vector for storing long long integers.
*/
typedef vector < long long, allocator<long long> > LongLongVector;

/**	
	A pair for storing two related strings.
*/
typedef pair < string, string > StringPair;

/**	
	A vector for storing pairs of strings.
*/
typedef vector < StringPair*, allocator<StringPair*> > StringPairVector;

/**
	This class provides a set of common fuctions used through out the application.
	All functions are static.
*/
class Common {
	public:
		static string	GetDatafile();
		static bool		GetGenerateMD5();
		static string	GetXMLfile();
		static string	GetSchemaPath();
		static string	GetXMLfileMD5();
		static string	GetOutputFilename();
		static bool		GetUseProvidedData();
		static bool		GetUseVariableFile();
		static string	GetExternalVariableFile();
		static bool		GetVerifyXMLfile();
		static string	GetXSLFilename();
		static string	GetXSLOutputFilename();
		static bool     GetNoXsl();
		static string   GetDefinitionIdsString();
		static bool     GetLimitEvaluationToDefinitionIds();
		static bool     GetDoDefinitionSchematron();
		static string   GetDefinitionSchematronPath();
		static string   GetDefinitionIdsFile();

		static void		SetDataFile(string);
		static void		SetGenerateMD5(bool);
		static void		SetXMLfile(string);
		static void		SetSchemaPath(string);
		static void		SetXMLfileMD5(string);
		static void		SetOutputFilename(string);
		static void		SetUseProvidedData(bool);
		static void		SetUseVariableFile(bool);
		static void		SetExternalVariableFile(string);
		static void		SetVerifyXMLfile(bool);
		static void 	SetXSLFilename(string);
		static void		SetXSLOutputFilename(string);
		static void     SetNoXsl(bool);
		static void     SetDefinitionIdsString(string definitionIdsString);
		static void     SetLimitEvaluationToDefinitionIds(bool set);
		static void     SetDoDefinitionSchematron(bool set);
		static void     SetDefinitionSchematronPath(string definitionSchematronPath);
		static void     SetDefinitionIdsFile(string definitionIdsFile);

		static StringVector* ParseDefinitionIdsFile();
		static StringVector* ParseDefinitionIdsString();
		/** Pad the provided string with spaces so that it is the desired length. */
		static string	PadString(string, unsigned int);
		/** Pad the provided string with the specified char so that it is the desired length. */
		static string	PadStringWithChar(string, char, unsigned int);
		/**
		 *  This function takes a string and searches for all oldChrs.  If one is found,
	     *  it is replaced with a newChr.  It is only intended to work with a single char 
	     *  at a time. No multiple char strings allowed
	     */
		static string	SwitchChar(string stringIn, string oldChr, string newChr);
		/** Return true if the specified file exists. */
		static bool     FileExists(const char* filename);
		/** Return the int as a string. */
		static string	ToString(int);
		/** Return the unsigned int as a string. */
		static string	ToString(unsigned int);
		/** Return the long as a string. */
		static string	ToString(long);
		/** Return the long long as a string. */
		static string   ToString(long long);
		/** Return the unsigned long as a string. */
		static string	ToString(unsigned long);
		/** Return a the bool as a string. */
		static string	ToString(bool);
		/** Return a the char as a string. */
		static string	ToString(char);
		/** Return a double as a string. */
		static string	ToString(double);

		/** Converts a string into a string of all uppercase characters.
		 *	@param s The string that you would like to convert into all uppercase characters.
		 *	@return A string of uppercase characters.
		 */
		static string ToUpper(string s);
		/** Converts a string into a long long integer. At some point, we might want to consider placing the error checking code in a separate method to allow more flexibility with the StringToLongLong() method. 
		 *	@param numstr Pointer to the character string that you would like to convert into a long long integer.
		 *	@param endptr Points to the first character in the character string that cannot be converted.
	     *	@param base The base to use during the conversion process.
		 *	@return The long long integer value of the character string. If the value is outside the limit of LLONG_MAX or LLONG_MIN, the function will return LLONG_MAX if the value is positive, and LLONG_MIN if the value is negative. If the conversion was unsuccessful, the return value will be 0, and the errno variable will be set to the corresponding error code. If errno is equal to EINVAL, it indicates that the specified base value was invalid. If errno is equal to ERANGE, it indicates that the converted value exceeded the limit of LLONG_MIN or LLONGMAX. 
		 */
		static long long StringToLongLong(char* numstr , char** endptr , int base);

		/** 
		 *	Retrieve the date/time.  The final output will be in the format:
		 *	yyyy-mm-ddThh:mm:ss	2006-08-16T14:21:38
		 */
		static string	GetTimeStamp();

		static char fileSeperator;
		static string fileSeperatorStr;

		

	private:
		static string dataFile;
		static string outputFilename;
		static string startTime;
		static string externalVariablesFile;
		static string xmlfile;
		static string schemapath;
		static bool noXsl;
		static string xslFile;
		static string xslOutputFile;
		static string xmlfileMD5;
		static bool	generateMD5;
		static bool useProvidedData;
		static bool verifyXMLfile;
		static bool limitEvaluationToDefinitionIds;
		static string definitionIds;
		static bool doDefinitionSchematron;
		static string definitionSchematronPath;
		static string definitionIdsFile;

		/** format of a definition id. */
		static const string DEFINITION_ID;

		/** format required for a list of definitions ids */
		static const string DEFINITION_ID_LIST;
};

/** 
	This class represents an Exception that occured while running a function in the Common class.
*/
class CommonException : public Exception {
	public:
		CommonException(string errMsgIn = "", int severity = ERROR_FATAL, Exception* ex = NULL);
		~CommonException();
};

#endif
