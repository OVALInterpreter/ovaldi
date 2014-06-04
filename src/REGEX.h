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

#ifndef REGEX_H
#define REGEX_H

//#include "Exception.h"
#include "Common.h"

//	required regex include
#include <pcre.h> 

#include <sstream>
#include <string>
#include <cstring>
#include <vector>

typedef std::vector < std::string > StringVector;

/**	The MAXMATCHES constant should be used by any search method to ensure that
	endless/excessive matching searching doesn't occure. With out some sort of 
	maximum it is possible to match so many items that the system would run out 
	of memory.
*/
#define MAXMATCHES 5000

/**
	This class provides pattern matching support to the application.
	inaddition to pattern mathcin support several related methods are also provided.
	This class uses the pcre library found at www.pcre.org for pattern matching. 
*/
class REGEX {
public:

	/** Simple constructor to initialize the matchCount member variable. */
	REGEX();

	~REGEX();

	/** 
		This function takes a string and searches for all regular expression characters. 
		If one is found and it is not already escaped it is escaped with a '\' The regular
		expression chars are stored in a string. The following chars need to be escaped:
		^ $ \ . [ ] ( ) * + ? |
	*/
	std::string EscapeRegexChars(std::string);

	/**
		This function takes a string and searches for the first regular	expression character that is not escaped. 
		If one is found its location is returned. If none are found -1 is returned. Only 
		regular expression chars that are not escaped are considered. The following are 
		considered regular expression chars if they are not escaped:
		^ $ \ . [ ] ( ) * + ? |
	*/
	int FindFirstRegexChar(const std::string stringIn);

	/**
		Return both the constant portion of a string and the remaining pattern. 
		If no constant portion is found set constOut to "" If the entire string
		is constant set patternOut to "". The input delimiter is used to ensure
		that constant strings are treated as a unit. 

		Loop through the provided pattern breaking it down by removing constant
		pieces from the start of the pattern. Build the constant string out of
		the pieces. Each piece is determined by looking for the next occurance
		of the specified delimiter. As the constatn string is built the 
		delimiter is added back in to the string.
	
		If an error occures an exception is thrown
	*/
	void GetConstantPortion(std::string patternIn, char delimIn, std::string *patternOut, std::string *constOut);
	
	/**	Return true if the searchString matches the specified pattern.
	*/
	bool IsMatch(const char *patternIn, const char *searchStringIn);

	/**	Return true if the searchString matches the specified pattern including the set of matched substrings.
		If the input regex identifies any subexpressions the matching substrings for those subexpressions
		are pushed onto the substrings input parameter.
	*/
	bool GetMatchingSubstrings(const char *patternIn, const char *searchStringIn, StringVector* substrings);

	/**
	 * Whereas GetMatchingSubstrings(const char*,const char*,StringVector*) returns details on the first
	 * match found, this method collects information on all matches found.  'matches' is a vector of vectors
	 * which will be modified to contain the matching results.  Each element of the outer vector is a match
	 * of the pattern.  Each element of the inner vector contains the bit of text which matched overall (as
	 * the first vector element) and matches for all captured substrings (in subsequent elements).
	 */
	void GetAllMatchingSubstrings(const std::string& pattern, const std::string& searchString, std::vector<StringVector> &matches, int matchOptions=0);

	/** 
		This function takes a string and searches for all the double '\'s. 
		Each double '\' //	is converted to a single '\'
	*/
	std::string	RemoveExtraSlashes(std::string);

	/** Set the match count back to zero */
	void	Reset();

	/**
	 * Define some constants for matching.  These
	 * are currently set to their PCRE equivalents.
	 * We define our own to insulate ourselves from
	 * the underlying regex implementation.
	 */
	enum MatchOptions {
		SINGLELINE = PCRE_DOTALL,
		MULTILINE = PCRE_MULTILINE,
		IGNORE_CASE = PCRE_CASELESS
	};

private:
	/**
		Return true if the specified pattern is constant. 
		If the string is of length = 0 return true.
	*/
	bool IsConstant(std::string);

	int matchCount;
};

/** 
	This class represents an Exception that occured while processing a regex.
*/
class REGEXException : public Exception {
public:
	/** 
		Set the error message and the severity to the specified values. 
		This is done with the explicit call to the Exception class constructor that 
		takes a string msg and an int severity param.
	*/
	REGEXException(std::string errMsgIn = "", int severity = ERROR_FATAL);

	~REGEXException();
};

#endif
