//
// $Id: REGEX.cpp 4579 2008-01-02 17:39:07Z bakerj $
//
//****************************************************************************************//
// Copyright (c) 2002-2008, The MITRE Corporation
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

REGEX::REGEX() {
	this->matchCount = 0;
}

REGEX::~REGEX() {

}

string REGEX::EscapeRegexChars(string stringIn) {
	
	string regexChars ="^$\\.[](){}*+?";
	string fixedString = stringIn;
	string prevChar = "";

	unsigned int pos = fixedString.find_first_of(regexChars, 0);
	while (pos != string::npos)
	{
		//	ensure that the char is not already escaped
		prevChar = fixedString.at(pos-1);
		if(prevChar.compare("\\") != 0)
		{
			fixedString.insert(pos, "\\");
			pos = fixedString.find_first_of(regexChars, pos+2);
		}else
		{
			pos = fixedString.find_first_of(regexChars, pos+1);
		}
	}

	return fixedString;	
}

int REGEX::FindFirstRegexChar(const string stringIn) {
	
	string regexChars	= "^$\\.[](){}*+?";
	string prevChar		= "";
	string curChar		= "";
	string nextChar		= "";
	unsigned int pos	= string::npos;
	int slashCount		= 0;
	int prevIndex		= 0;

	pos = stringIn.find_first_of(regexChars, 0);

	//	Check that one is found
	if(pos == string::npos)
		return -1;

	while (pos != string::npos)
	{
		//	ensure that the char is not escaped
		prevIndex = pos-1;
		if(prevIndex == -1)
			prevChar = "";
		else
			prevChar = stringIn.at(prevIndex);

		curChar = stringIn.at(pos);
		nextChar = stringIn.at(pos+1);
		
		//	If a '\' check that the next next char is a '\'
		if (strncmp(curChar.c_str(), "\\", 1) == 0)
		{
			if(strncmp(nextChar.c_str(), "\\", 1) == 0)
			{
				pos = stringIn.find_first_of(regexChars, pos+2);

			}else
			{
				break;
			}
			
		}else
		{
			//	Get count of consecutive previous '\'s
			slashCount = 0;
			while(prevChar.compare("\\") == 0 && prevIndex > 0)
			{
				slashCount++;
				prevChar = stringIn.at(--prevIndex);
			}

			if(slashCount % 2 == 0)
				break;
			
			pos = stringIn.find_first_of(regexChars, pos+1);
		}
	}

	return pos;	
}

int REGEX::FindLastRegexChar(const string stringIn) {

	string regexChars	= "^$\\.[](){}*+?";
	string prevChar		= "";
	unsigned int pos	= string::npos;
	int slashCount		= 0;
	int prevIndex		= 0;

	pos = stringIn.find_last_of(regexChars, stringIn.length());

	// Check that at least one regex character is found.

	if (pos == string::npos) return -1;

	while (pos != string::npos)
	{
		// Ensure that the char in question is not escaped.

		prevIndex = pos-1;

		if ((prevIndex) == -1) prevChar = "";
		else prevChar = stringIn.at(prevIndex);

		if (strncmp(prevChar.c_str(), "\\", 1) == 0)
		{
			// We have to make sure the preceeding slash is not part of a double slash as
			// that would negate the escape.  Get count of consecutive previous '\'s.  If
			// it is an even number, then the regex character in question is not escaped.

			slashCount = 1;
			prevChar = stringIn.at(--prevIndex);

			while(prevChar.compare("\\") == 0 && prevIndex > 0)
			{
				slashCount++;
				prevChar = stringIn.at(--prevIndex);
			}

			if(slashCount % 2 == 0) break;
			
			pos = stringIn.find_last_of(regexChars, (pos - slashCount - 1));
		}
		else
		{
			break;
		}
	}

	return pos;	
}

void REGEX::GetConstantPortion(string patternIn, string delimIn, string *patternOut, string *constOut) {
	
	unsigned int nextDelim = string::npos;
	int delimLen = delimIn.length();
	(*patternOut) = patternIn;
	(*constOut) = "";
	string tmpStr;
	bool rmCarrot = false;
	bool rmDollar = false;

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

	while((nextDelim = (*patternOut).find(delimIn, 0)) != string::npos)
	{
		//	Get the next substring
		tmpStr = (*patternOut).substr(0, nextDelim+1+delimLen);

		//	Make sure the substring is constant
		if(IsConstant(tmpStr))
		{
			//	Add the string to the constant string
			(*constOut)  = (*constOut)  + tmpStr;

			// Remove the string from the pattern
			(*patternOut) = (*patternOut).substr(nextDelim+1+delimLen, (*patternOut).length()-nextDelim+1+delimLen);
			
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
	if(rmCarrot && (*patternOut).length() != 0)
	{
		(*patternOut) = (*patternOut) + "$";
	}

	//	Add the carrot tot he beginning of the pattern (if removed 
	//	or a constant portion was found) and there is still a pattern left
	if((rmCarrot || (*constOut).length() != 0) && (*patternOut).length() > 0)
	{
		(*patternOut) = "^" + (*patternOut);
	}
}

bool REGEX::IsConstant(string pattern) {

	int regexChar = string::npos;
	bool constant = true;
	
	regexChar = FindFirstRegexChar(pattern);
	
	//	If length is 0 return true
	if(pattern.length() == 0)
		return true;

	if(regexChar != -1)
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
		string errMsg = "Error: Failed to compile the specifed regular expression pattern.\n\tPattern: ";
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
	int ovector[30];
	for(int i = 0; i < 30; i++){
		ovector[i] = -1;
	}
	rc = pcre_exec(	compiledPattern,		// result of pcre_compile()			
					NULL,					// we didn't study the pattern		
					searchStringIn,			// the subject string				
					strlen(searchStringIn),	// the length of the subject string	
					0,						// start at offset 0 in the subject	
					0,						// default options					
					ovector,				// vector of integers for substring information	
					30);					// number of elements in the vector	

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

	//free(compiledPattern);

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

		string errMsg = "Error: Failed to compile the specifed regular expression pattern.\n\tPattern: ";
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
		
		// An error occured
		string errMsg = "Error: PCRE returned error code (" + rc;
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

			if (rc < 0) {
				string error = "get substring list failed " + rc;
				error.append(" unalbe to get memory for the result set.");
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
					string error = "string list not terminated by NULL";
					throw REGEXException(error);
				}

				pcre_free_substring_list(stringlist);				
			}
		}
	}

	//free(compiledPattern);

	return(result);
}

//string REGEX::StrToNegativePattern(string strIn) {
//	// -----------------------------------------------------------------------
//	//  ABSTRACT
//	//
//	//  Convert the specified string to a negative pattern match
//	//
//	// -----------------------------------------------------------------------
//
//	return ("!^" + strIn + "$");
//}

string REGEX::RemoveExtraSlashes(string strIn) {

	string doubleSlash ="\\\\";

	unsigned int pos = strIn.find(doubleSlash, 0);
	while (pos != string::npos)
	{
		strIn.erase(pos++, 1);
		pos = strIn.find(doubleSlash, pos);
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

