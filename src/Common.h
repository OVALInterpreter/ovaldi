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
#include <functional>
#include <cctype>


#include "XmlCommon.h"
#include "DocumentManager.h"

/**
	A vector for storing strings.
*/
typedef std::vector < std::string > StringVector;

/**
    A set for storing unique strings.
*/
typedef std::set < std::string > StringSet;

/**
	A vector for storing integers.
*/
typedef std::vector < int > IntVector;

/**
	A vector for storing long long integers.
*/
typedef std::vector < long long > LongLongVector;

/**	
	A pair for storing two related strings.
*/
typedef std::pair < std::string, std::string > StringPair;

/**	
	A vector for storing pairs of strings.
*/
typedef std::vector < StringPair* > StringPairVector;

/**
	This class provides a set of common fuctions used through out the application.
	All functions are static.
*/
class Common {
	public:
		static std::string	GetDatafile();
		static bool		GetGenerateMD5();
		static std::string	GetXMLfile();
		static std::string	GetSchemaPath();
		static std::string	GetXMLfileMD5();
		static std::string	GetOutputFilename();
		static bool		GetUseProvidedData();
		static bool		GetUseVariableFile();
		static std::string	GetExternalVariableFile();
		static std::string	GetDirectivesConfigFile();
		static bool		GetVerifyXMLfile();
		static std::string	GetXSLFilename();
		static std::string	GetXSLOutputFilename();
		static std::string   GetLogFileLocation();
		static bool     GetNoXsl();
		static std::string   GetDefinitionIdsString();
		static bool     GetLimitEvaluationToDefinitionIds();
		static bool     GetDoDefinitionSchematron();
		static std::string   GetDefinitionSchematronPath();
		static std::string   GetDefinitionIdsFile();

		static void		SetDataFile(std::string);
		static void		SetGenerateMD5(bool);
		static void		SetXMLfile(std::string);
		static void		SetSchemaPath(std::string);
		static void		SetXMLfileMD5(std::string);
		static void		SetOutputFilename(std::string);
		static void		SetUseProvidedData(bool);
		static void		SetUseVariableFile(bool);
		static void		SetExternalVariableFile(std::string);
		static void		SetDirectivesConfigFile(std::string);
		static void		SetVerifyXMLfile(bool);
		static void 	SetXSLFilename(std::string);
		static void		SetXSLOutputFilename(std::string);
		static void		SetLogFileLocation(std::string);
		static void     SetNoXsl(bool);
		static void     SetDefinitionIdsString(std::string definitionIdsString);
		static void     SetLimitEvaluationToDefinitionIds(bool set);
		static void     SetDoDefinitionSchematron(bool set);
		static void     SetDefinitionSchematronPath(std::string definitionSchematronPath);
		static void     SetDefinitionIdsFile(std::string definitionIdsFile);

		static StringVector* ParseDefinitionIdsFile();
		static StringVector* ParseDefinitionIdsString();
		/** Pad the provided string with spaces so that it is the desired length. */
		static std::string	PadString(std::string, unsigned int);
		/** Pad the provided string with the specified char so that it is the desired length. */
		static std::string	PadStringWithChar(std::string, char, unsigned int);
		/**
		 *  This function takes a string and searches for all oldChrs.  If one is found,
	     *  it is replaced with a newChr.  It is only intended to work with a single char 
	     *  at a time. No multiple char strings allowed
	     */
		static std::string	SwitchChar(std::string stringIn, std::string oldChr, std::string newChr);
		/** Return true if the specified file exists. */
		static bool     FileExists(const char* filename);
		/** Return the int as a string. */
		static std::string	ToString(int);
		/** Return the unsigned int as a string. */
		static std::string	ToString(unsigned int);
		/** Return the long as a string. */
		static std::string	ToString(long);
		/** Return the long long as a string. */
		static std::string   ToString(long long);
		/** Return the unsigned long as a string. */
		static std::string	ToString(unsigned long);
		/** Return a the bool as a string. */
		static std::string	ToString(bool);
		/** Return a the char as a string. */
		static std::string	ToString(char);
		/** Return a double as a string. */
		static std::string	ToString(double);

		/** Converts a string into a string of all uppercase characters.
		 *	@param s The string that you would like to convert into all uppercase characters.
		 *	@return A string of uppercase characters.
		 */
		static std::string ToUpper(std::string s);

		/** Converts a string into a string of all lowercase characters.
		 *	@param s The string that you would like to convert into all lowercase characters.
		 *	@return A string of lowercase characters.
		 */
		static std::string ToLower(std::string s);
		
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
		static std::string	GetTimeStamp();

        /**
		 *	Build a valid filepath out of the input path and filename. 
		 *	If the input filename is empty or null the path is returned.
		 *	@param path non NULL string representing the path portion of the filepath
		 *	@param filename string representing the filename
        */
        static std::string BuildFilePath(const std::string path, const std::string filename);

		/**
		 *	Split a valid filepath into its path and filename components. 
		 *	@param filepath a non-NULL, non-empty, string representing the filepath that you would like split.
		 *	@return a pointer to a StringPair that contains the path and filename components of the specified filepath.
         */
		static StringPair* SplitFilePath(const std::string filepath);

		/** Removes whitespace at the start of the given string. */
		static void TrimStart(std::string& str);

		/** Removes whitespace at the end of the given string. */
		static void TrimEnd(std::string& str);

		/** Removes whitespace at the start and end of the given string. */
		static void TrimString(std::string& str);

		static bool IsRegexChar(char c);
		
		/** Resolves the canonical path for a given path */
		static std::string GetFullPath(std::string path);

		static char fileSeperator;
		static std::string fileSeperatorStr;

		/** All chars which are not interpreted literally in a regex. */
		static const std::string REGEX_CHARS;

		

	private:
		static std::string dataFile;
		static std::string outputFilename;
		static std::string startTime;
		static std::string externalVariablesFile;
		static std::string directivesConfigFile;
		static std::string xmlfile;
		static std::string schemapath;
		static bool noXsl;
		static std::string xslFile;
		static std::string xslOutputFile;
		static std::string logFileLocation;
		static std::string logFileName;
		static std::string xmlfileMD5;
		static bool	generateMD5;
		static bool useProvidedData;
		static bool verifyXMLfile;
		static bool limitEvaluationToDefinitionIds;
		static std::string definitionIds;
		static bool doDefinitionSchematron;
		static std::string definitionSchematronPath;
		static std::string definitionIdsFile;

		/** format of a definition id. */
		static const std::string DEFINITION_ID;

		/** format required for a list of definitions ids */
		static const std::string DEFINITION_ID_LIST;

};

/** 
	This class represents an Exception that occured while running a function in the Common class.
*/
class CommonException : public Exception {
	public:
        /**
            Set the error message and then set the severity to ERROR_FATAL. This is
            done with the explicit call to the Exception class constructor that
            takes a single string param.
        */
		CommonException(std::string errMsgIn = "", int severity = ERROR_FATAL, Exception* ex = NULL);
		~CommonException();
};

#endif
