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

#include "REGEX.h"

using namespace std;

REGEX::REGEX() {
	this->matchCount = 0;
}

REGEX::~REGEX() {

}

string REGEX::EscapeRegexChars(string stringIn) {
	
	string fixedString = stringIn;

	size_t pos = fixedString.find_first_of(Common::REGEX_CHARS, 0);
	while (pos != string::npos) {
		fixedString.insert(pos, "\\");
		pos = fixedString.find_first_of(Common::REGEX_CHARS, pos+2);			
	}

	return fixedString;	
}

int REGEX::FindFirstRegexChar(const string stringIn) {
	
	for (string::size_type i = 0; i < stringIn.size(); ++i) {
		if (stringIn[i] == '\\') {
			// treat "\" at the end of a string as a literal char, not a regex
			// char.
			if (i < stringIn.size() - 1) {
				// if the char following '\' is a regex char, ignore it.
				// Otherwise, it's some sort of character class, and we
				// found our regex char.
				if (Common::IsRegexChar(stringIn[i+1]))
					++i;
				else
					return i;
			}
		} else if (Common::IsRegexChar(stringIn[i]))
			return i;
	}

	return -1;
}

void REGEX::GetConstantPortion(string patternIn, char delimIn, string *patternOut, string *constOut) {
	
	size_t nextDelim = string::npos;
	(*patternOut) = patternIn;
	(*constOut) = "";
	string tmpStr;
	bool rmCarrot = false;
	bool rmDollar = false;

	// If the delim char is a regex char, it will appear in the path
	// regex escaped.
	string escapedDelim(1, delimIn);
	if (Common::IsRegexChar(delimIn))
		escapedDelim = "\\"+escapedDelim;
	int delimLen = escapedDelim.length();

	//	Check if the pattern starts with a carrot (^)
	if((*patternOut).at(0) == '^')
	{
		(*patternOut) = (*patternOut).substr(1, (*patternOut).length()-1);
		rmCarrot = true;
	}

	//	Check if the pattern ends with a dollar ($)
	if((*patternOut).at((*patternOut).length()-1) == '$')
	{
		(*patternOut) = (*patternOut).substr(0, (*patternOut).length()-1);
		rmDollar = true;
	}

	while((nextDelim = (*patternOut).find(escapedDelim)) != string::npos)
	{
		//	Get the next substring
		tmpStr = (*patternOut).substr(0, nextDelim+delimLen);

		//	Make sure the substring is constant
		if(IsConstant(tmpStr))
		{
			//	Add the string to the constant string
			(*constOut)  = (*constOut)  + tmpStr;

			// Remove the string from the pattern
			(*patternOut) = (*patternOut).substr(nextDelim+delimLen);
			
		}else{
			
			// If not constant finished processing
			break;
		}
	}

	// Finally check the last piece of the pattern
	if (IsConstant((*patternOut)))
	{
		(*constOut)  = (*constOut)  + (*patternOut);
		(*patternOut) = "";
	}

	//	Add the $ to the end of the pattern if it was removed 
	//	and there is a pattern ramaining
	if(rmDollar && (*patternOut).length() != 0)
	{
		(*patternOut) = (*patternOut) + "$";
	}

	//	Add the carrot to the beginning of the pattern (if removed 
	//	or a constant portion was found) and there is still a pattern left
	if((rmCarrot || (*constOut).length() != 0) && (*patternOut).length() > 0)
	{
		(*patternOut) = "^" + (*patternOut);
	}
}

bool REGEX::IsConstant(string pattern) {

	size_t regexChar = string::npos;
	bool constant = true;

	//	If length is 0 return true
	if(pattern.length() == 0)
		return true;

	regexChar = FindFirstRegexChar(pattern);

	if( regexChar != string::npos )
	{
		//	Try removing a leading ^ if it is not followed by a regex char
		if (regexChar == 0 && pattern.at(regexChar) == '^')
		{
			//	Remove the ^
			string tmpPattern = pattern.substr(1, pattern.length()-1);
			
			//	Retest with recursive call
			constant = IsConstant(tmpPattern);

		}else {

			constant = false;
		}
	}

	return constant;
}

bool REGEX::IsMatch(const char *patternIn, const char *searchStringIn) {
	bool		result				= false;
	pcre		*compiledPattern;
	const char	*error;
	int			erroffset = -1;

	//	Test the match count
	if(this->matchCount >= MAXMATCHES)
	{
		string errMsg = "Warning: The specified pattern has matched more than the supported number of items.";
		errMsg.append("\nPattern: ");
		errMsg.append(patternIn);
		throw REGEXException(errMsg, ERROR_WARN);	
	}
		
	//	Compile the pattern
	compiledPattern = pcre_compile(	patternIn,			// the pattern					
									0,					// default options				
									&error,				// for error message			
									&erroffset,			// for error offset				
									NULL);				// use default character tables	

	//	Check for compile errors
	if(compiledPattern == NULL)
	{
		string errMsg = "Error: Failed to compile the specified regular expression pattern.\n\tPattern: ";
		errMsg.append(patternIn);
		errMsg.append("\n\tOffset: ");
		
		ostringstream erroffsetStr;
		erroffsetStr << erroffset;
		
		errMsg.append(erroffsetStr.str());
		errMsg.append("\n\tMessage: ");
		errMsg.append(error);
		throw REGEXException(errMsg);
	}
 

	//	Match a pattern
	int rc;
	int ovector[60];
	for(int i = 0; i < 60; i++){
		ovector[i] = -1;
	}
	rc = pcre_exec(	compiledPattern,		// result of pcre_compile()			
					NULL,					// we didn't study the pattern		
					searchStringIn,			// the subject string				
					strlen(searchStringIn),	// the length of the subject string	
					0,						// start at offset 0 in the subject	
					0,						// default options					
					ovector,				// vector of integers for substring information	
					60);					// number of elements in the vector	

	//	Test the return value of the pattern match 
	//	and increment the match count if a match was found
	if(rc == 0) {
		result = false;
	} else if (rc < 0) {
		// and some sort of erro we will not report for now.
		result = false;
	} else if (rc > 0) {
		result = true;
		this->matchCount++;
	}

	pcre_free(compiledPattern);

	return(result);
}

bool REGEX::GetMatchingSubstrings(const char *patternIn, const char *searchStringIn, StringVector* substrings) {

	bool		result				= false;
	pcre		*compiledPattern;
	const char	*error;
	int			erroffset = -1;

	//	Test the match count
	if(this->matchCount >= MAXMATCHES) {
		string errMsg = "Warning: The specified pattern has matched more than the supported number of items.";
		errMsg.append("\nPattern: ");
		errMsg.append(patternIn);
		throw REGEXException(errMsg, ERROR_WARN);	
	}
		
	//	Compile the pattern
	compiledPattern = pcre_compile(	patternIn,			// the pattern					
									0,					// default options				
									&error,				// for error message			
									&erroffset,			// for error offset				
									NULL);				// use default character tables	

	//	Check for compile errors
	if(compiledPattern == NULL) {

		string errMsg = "Error: Failed to compile the specified regular expression pattern.\n\tPattern: ";
		errMsg.append(patternIn);
		errMsg.append("\n\tOffset: ");
		
		ostringstream erroffsetStr;
		erroffsetStr << erroffset;
		
		errMsg.append(erroffsetStr.str());
		errMsg.append("\n\tMessage: ");
		errMsg.append(error);
		throw REGEXException(errMsg);
	}
 

	//	Match a pattern
	int rc;
	int ovector[60];
	for(int i = 0; i < 60; i++) {
		ovector[i] = -1;
	}
	rc = pcre_exec(	compiledPattern,		// result of pcre_compile()			
					NULL,					// we didn't study the pattern		
					searchStringIn,			// the subject string				
					strlen(searchStringIn),	// the length of the subject string	
					0,						// start at offset 0 in the subject	
					0,						// default options					
					ovector,				// vector of integers for substring information	
					60);					// number of elements in the vector	

	//	Test the return value of the pattern match 
	//	and increment the match count if a match was found
	if(rc == 0) {
		result = false;
	} else if (rc == -1) {
		result = false;
	} else if (rc < -1) {

		pcre_free(compiledPattern);

		// An error occured
		string errMsg = "Error: PCRE returned error code (" + Common::ToString(rc);
		errMsg.append(") While evaluating the following regex: ");
		errMsg.append(patternIn);
		errMsg.append(" against this string: ");
		errMsg.append(searchStringIn);
		throw REGEXException(errMsg);

	} else if (rc > 0) {
		// The string did match
		result = true;
		this->matchCount++;

		// next extract any matching substrings
		if(rc > 1) {

			const char **stringlist;
			int res = pcre_get_substring_list(searchStringIn, ovector, rc, &stringlist);

			if (res == PCRE_ERROR_NOMEMORY) {
				string error = "get substring list failed: unable to get memory for the result set.";
				pcre_free(compiledPattern);
				throw REGEXException(error);
			} else {
				int i = 0;
				for (i = 0; i < rc; i++) {
					string str = "";
					str.append(stringlist[i]);
					if(i > 0)
						substrings->push_back(str);
				}
				
				if (stringlist[i] != NULL) {
					pcre_free_substring_list(stringlist);
					pcre_free(compiledPattern);
					string error = "string list not terminated by NULL";
					throw REGEXException(error);
				}

				pcre_free_substring_list(stringlist);				
			}
		}
	}

	pcre_free(compiledPattern);
	return(result);
}

void REGEX::GetAllMatchingSubstrings(const string& pattern, const string& searchString, vector<StringVector> &matches, int matchOptions) {
	pcre *re;
	const char* error;
	int errOffset;
	static const int MAX_CAPTURED_VALUES = 100; //includes the overall match
	static const int MIN_CAPTURED_VALUES = 10; //includes the overall match

	re = pcre_compile(
		pattern.c_str(),  /* the pattern */
		matchOptions,     /* our given options */
		&error,           /* for error message */
		&errOffset,       /* for error offset */
		NULL);            /* use default character tables */

	//	Check for compile errors
	if(re == NULL) {
		string errMsg = "Error: Failed to compile the specified regular expression pattern.";
		errMsg += "\n\tPattern: " + pattern;
		errMsg += "\n\tOffset: " + Common::ToString(errOffset);
		errMsg += "\n\tMessage: " + string(error);
		throw REGEXException(errMsg);
	}

	// Try to be semi-intelligent about choosing the size of the "ovector".
	// Use the capture count as a guide.  I think the returned value only
	// counts parenthesized subexpressions and does not include the overall
	// match (so I need to include space for 1 extra value).
	int numCaptures;
	int errCode = pcre_fullinfo(re, NULL, PCRE_INFO_CAPTURECOUNT, &numCaptures);

	if (errCode < 0) {
		pcre_free(re);
		throw REGEXException(string("Pattern analysis failed!  Error code = ") + Common::ToString(errCode));
	}

	// safety check: don't let ovecSize be too large or too small
	if (numCaptures > MAX_CAPTURED_VALUES-1)
		numCaptures = MAX_CAPTURED_VALUES-1;
	if (numCaptures < MIN_CAPTURED_VALUES-1)
		numCaptures = MIN_CAPTURED_VALUES-1;
	int ovecSize = (numCaptures+1)*3;
    int *ovector = new int[ovecSize];

	int matchCount;
	int matchOffset = 0;
	StringVector match;
	do {
		memset(ovector, 0, ovecSize*sizeof(int));

		matchCount = pcre_exec(
			re,
			NULL,
			searchString.c_str(),
			searchString.size(),
			matchOffset,
			0,
			ovector,
			ovecSize);

		if (matchCount > 0) {
			match.clear();
			for (int i=0; i<matchCount; i++)
				if (ovector[i*2] > -1)
					match.push_back(searchString.substr(ovector[i*2], ovector[i*2+1]-ovector[i*2]));
				else
					match.push_back("");

			matches.push_back(match);

			// Make sure we add at least one to the offset, so we don't
			// loop infinitely matching in the same place.  This does not
			// perfectly mimic Perl, but it's simple and prevents infinite
			// loops...
			matchOffset = ovector[1] == matchOffset ? ovector[1]+1 : ovector[1];

		} else if (matchCount == 0) {
			pcre_free(re);
			delete[] ovector;
			throw REGEXException(string("Regex match error: too many captured values! (> ")+Common::ToString(ovecSize/3)+")");

		} else if (matchCount != PCRE_ERROR_NOMATCH) {
			pcre_free(re);
			delete[] ovector;

			string errMsg = "Error: PCRE returned error code (" + Common::ToString(matchCount);
			errMsg += ") While evaluating the following regex: ";
			errMsg += pattern;
			// I am not gonna quote the search string in the error message because it
			// could be very large.
			throw REGEXException(errMsg);
		}

	} while(matchCount > 0 && matchOffset < (int)searchString.size());

	pcre_free(re);
	delete[] ovector;
}

string REGEX::RemoveExtraSlashes(string strIn) {
	// This code works on the "constant portion" of a regex.
	// It *assumes* that the value passed in is a valid regex
	// with all regex metacharacters already escaped.
	//
	// It finds all escaping backslashes and removes them.  (We
	// don't remove all backslashes because a backslash may be
	// escaping another backslash that should be left in.)  The
	// result is a plain non-regex string.
	size_t pos = strIn.find('\\');
	while (pos != string::npos && pos < strIn.size() - 1) {
		strIn.erase(pos, 1);
		if (pos < strIn.size() - 1)
			pos = strIn.find('\\', pos + 1);
	}

	return strIn;	
}

void REGEX::Reset() {

	this->matchCount = 0;

}

//****************************************************************************************//
//								REGEXException Class									  //	
//****************************************************************************************//
REGEXException::REGEXException(string errMsgIn, int severity) : Exception(errMsgIn, severity) {

}

REGEXException::~REGEXException() {

}
