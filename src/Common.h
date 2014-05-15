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

/**
  If XmlCommon.h is needed in a Windows environment, Common.h should be included
  instead in order to avoid conflicts with msxml.h
**/


#ifndef COMMON_H
#define COMMON_H

#ifdef WIN32
	#include <Meta.h>
#endif

#include "Exception.h"

#include <string>
#include <vector>
#include <set>
#include <sstream>
#include <cctype>

#include "StdTypedefs.h"

// Default filenames for various input and output files
#define DEFAULT_DEFINITION_SCHEMATRON_FILENAME "oval-definitions-schematron.xsl"
#define DEFAULT_SYSTEM_CHARACTERISTICS_SCHEMATRON_FILENAME "oval-system-characteristics-schematron.xsl"
#define DEFAULT_RESULTS_SCHEMATRON_FILENAME "oval-results-schematron.xsl"
#define DEFAULT_RESULTS_XFORM_FILENAME "results_to_html.xsl"

// fixed filename for the schemaLocation data
#define SCHEMALOCATION_CONFIG_FILENAME "schemaLocation.conf"

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
		static bool		GetDoSystemCharacteristicsSchematron();
		static bool		GetDoResultsSchematron();
		static std::string   GetDefinitionSchematronPath();
		static std::string   GetSystemCharacteristicsSchematronPath();
		static std::string   GetResultsSchematronPath();
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
		static void		SetDoSystemCharacteristicsSchematron(bool set);
		static void		SetDoResultsSchematron(bool set);
		static void     SetDefinitionSchematronPath(std::string path);
		static void     SetSystemCharacteristicsSchematronPath(std::string path);
		static void     SetResultsSchematronPath(std::string path);
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
		static bool     FileExists(const std::string &filename);

		/**
		 * Converts \p val to a string.  T must be a type such that
		 * 'out << val' is legal, where out is an ostringstream and val is
		 * of type T const&.  Inspired by boost's lexical_cast function.
		 */
		template <typename T>
		static std::string ToString(const T& val) {
			std::ostringstream result;
			result << std::boolalpha << val;

			return result.str();
		}

#ifdef WIN32
		/**
		 * A custom condition for our sfinae for FromString(): we want
		 * a special overload for the unsigned integral types, except
		 * for character types (which are considered integral by
		 * std::numeric_limits).
		 */
		template<typename T>
		struct IsUnsignedNonChar {
			static const bool value =
				std::numeric_limits<T>::is_integer &&
				!std::numeric_limits<T>::is_signed &&
				!IsChar<T>::value;
		};

		/**
		 * "Casts" from a string to type T.  The result is placed at the address
		 * pointed to by \p to.  T must be a type such that 'in >> val'
		 * is legal, where in is an istringstream, and val is of type T&.  Inspired
		 * by boost's lexical_cast function.  There is a specialization of the
		 * template for T=bool, since you don't seem to be able to tell an istream
		 * to accept both 1 and "true" for true, and similar for false.  So that
		 * specialization doesn't use istreams at all.
		 *
		 * @return false on failure, true on success
		 */
		template<typename T>
		static typename DisableIf<Common::IsUnsignedNonChar<T>::value, bool>::type
		FromString(const std::string &str, T *to) {
			std::istringstream iss(str);
			iss >> *to;
			if (!iss)
				return false;
			return true;
		}

		/**
		 * A special overload for unsigned non-char types, to hack around
		 * MS's buggy conversion code... Their standard library impl doesn't
		 * handle conversion to unsigned integral types properly.  It will
		 * wrap negatives around to positive and not set the fail flag on
		 * the stream.  So we have to go to a lot of extra trouble.... :(
		 */
		template<typename T>
		static typename EnableIf<Common::IsUnsignedNonChar<T>::value, bool>::type
		FromString(const std::string &str, T *to) {

			// For unsigned integral types, check if the string begins
			// with '-' (possibly after whitespace).  If so, check for
			// -0 or equiv (which is convertible to 0).  If str starts
			// with '-' but doesn't appear to be -0, then we fail the
			// conversion.  Otherwise, we conclude str doesn't represent
			// a negative number, and we drop down to the standard
			// conversion code.

			if (str.empty())
				return false;

			// be robust to leading spaces...
			size_t i = 0;
			while (i < str.size() && std::isspace(str[i])) ++i;
			if (i >= str.size())
				return false; // it was all spaces!
	
			// strings that start with '-' for unsigned types
			// can only be converted if the value is "-0" or
			// equiv, so let's allow that.
			if (str[i] == '-') {
				++i;
				if (i >= str.size())
					return false; // str ended with '-'.

				if (str[i] != '0')
					return false; // require at least one '0'
				++i;

				// allow repeated 0's...
				while (i < str.size() && str[i] == '0') ++i;

				// methinks I should allow a trailing whitespace
				// char as well (ignoring whatever comes after)...
				if (i >= str.size() || isspace(str[i])) {
					*to = T(0);
					return true;
				}

				return false;
			}

			// Otherwise, we know str doesn't represent a negative
			// number, so the standard operator>>() ought to behave
			// correctly.
			std::istringstream iss(str);
			iss >> *to;
			if (!iss)
				return false;
			return true;
		}
#else
		template<typename T>
		static bool FromString(const std::string &str, T *to) {
			std::istringstream iss(str);
			iss >> *to;
			if (!iss)
				return false;
			return true;
		}
#endif
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

		/**
		 *	Split a valid filepath into its path and filename components when the filepath is a regular expression.
		 *	@param filepath a non-NULL, non-empty, string representing the regular expression of the filepath that you would like split.
		 *	@return a pointer to a StringPair that contains the path and filename components of the specified filepath.
         */
		static StringPair* SplitFilePathRegex(const std::string filepath);

		/** Removes whitespace at the start of the given string. */
		static void TrimStart(std::string& str);

		/** Removes whitespace at the end of the given string. */
		static void TrimEnd(std::string& str);

		/** Removes whitespace at the start and end of the given string. */
		static void TrimString(std::string& str);

		static bool IsRegexChar(char c);
		
		/** Resolves the canonical path for a given path */
		static std::string GetFullPath(std::string path);

		/** Returns whether the two strings are equal, ignoring case. */
		static bool EqualsIgnoreCase(const std::string &s1, const std::string &s2);

		/** Returns whether the two strings are equal, ignoring case. */
		static bool EqualsIgnoreCase(const std::wstring &s1, const std::wstring &s2);

		/**
		 * If the given path ends with a trailing path separator, this method
		 * in most cases returns the path with that separator stripped off.  The exception
		 * is if the path consists of only a separator char, e.g. unix "/", then it
		 * must be left on, since the empty string would have a totally different meaning.
		 */
		static std::string StripTrailingSeparators(const std::string &path);

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
		static bool doSystemCharacteristicsSchematron;
		static bool doResultsSchematron;
		static std::string definitionSchematronPath;
		static std::string resultsSchematronPath;
		static std::string systemCharacteristicsSchematronPath;
		static std::string definitionIdsFile;

		/** format of a definition id. */
		static const std::string DEFINITION_ID;

		/** format required for a list of definitions ids */
		static const std::string DEFINITION_ID_LIST;

};


template<>
bool Common::FromString<bool>(const std::string &str, bool *to);

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
