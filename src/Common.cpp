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

#include <time.h>
#include <algorithm>
#include <iostream>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMNodeList.hpp>

#include "Log.h"
#include "XmlCommon.h"
#include "DocumentManager.h"
#include "REGEX.h"

#ifdef WIN32
	#include <windows.h>
#endif

#include "Common.h"

using namespace std;

// constants
const string Common::DEFINITION_ID = "oval:[A-Za-z0-9_\\-\\.]+:def:[1-9][0-9]*";
const string Common::DEFINITION_ID_LIST = "oval:[A-Za-z0-9_\\-\\.]+:def:[1-9][0-9]*(,oval:[A-Za-z0-9_\\-\\.]+:def:[1-9][0-9]*)*";

#ifdef WIN32
	char Common::fileSeperator = '\\';
	string Common::fileSeperatorStr = "\\";
#else
	char Common::fileSeperator = '/';
	string Common::fileSeperatorStr = "/";
#endif

// Initialize static variables.
string	Common::dataFile			  = "system-characteristics.xml";
string	Common::xmlfile				  = "definitions.xml";
#ifdef WIN32
string	Common::schemapath			  = "xml";
#else
string	Common::schemapath			  = "/usr/share/ovaldi";
#endif
string	Common::outputFilename	      = "results.xml";
string	Common::externalVariablesFile = "external-variables.xml";
string	Common::directivesConfigFile  = "directives.xml";
string	Common::xmlfileMD5			  = "";
string	Common::startTime			  = "";

bool    Common::noXsl                 = false;
string  Common::xslFile			      = Common::BuildFilePath(Common::schemapath, DEFAULT_RESULTS_XFORM_FILENAME);
string  Common::xslOutputFile		  = "results.html";

string Common::logFileLocation	      = "";
string Common::logFileName            = "ovaldi.log";

bool    Common::doDefinitionSchematron   = false;
bool    Common::doSystemCharacteristicsSchematron   = false;
bool    Common::doResultsSchematron   = false;
string  Common::definitionSchematronPath = Common::BuildFilePath(Common::schemapath, DEFAULT_DEFINITION_SCHEMATRON_FILENAME);
string  Common::systemCharacteristicsSchematronPath = Common::BuildFilePath(Common::schemapath, DEFAULT_SYSTEM_CHARACTERISTICS_SCHEMATRON_FILENAME);
string  Common::resultsSchematronPath = Common::BuildFilePath(Common::schemapath, DEFAULT_RESULTS_SCHEMATRON_FILENAME);

bool    Common::generateMD5			  = false;
bool    Common::useProvidedData		  = false;
bool    Common::verifyXMLfile		  = true;

bool    Common::limitEvaluationToDefinitionIds = false;
string  Common::definitionIds                  = "";
string  Common::definitionIdsFile              = "";

const string Common::REGEX_CHARS = "^$\\.[](){}*+?|";

namespace {
	bool caseInsensitiveCmpChars(char c1, char c2) {
		return tolower(c1) == tolower(c2);
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Accessors  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

string Common::GetDatafile()
{
	return dataFile;	
}

bool Common::GetGenerateMD5()
{
	return generateMD5;	
}

string Common::GetXMLfile()
{
	return xmlfile;	
}

string Common::GetSchemaPath()
{
	return schemapath;	
}

string Common::GetXMLfileMD5()
{
	return xmlfileMD5;	
}

string Common::GetOutputFilename()
{
	return outputFilename;	
}

bool Common::GetUseProvidedData()
{
	return useProvidedData;	
}

string Common::GetExternalVariableFile()
{
	return externalVariablesFile;	
}

string Common::GetDirectivesConfigFile()
{
	return directivesConfigFile;	
}

bool Common::GetVerifyXMLfile()
{
	return verifyXMLfile;
}

string Common::GetXSLFilename(){
	return Common::xslFile;	
}
string Common::GetXSLOutputFilename()
{
	return Common::xslOutputFile;	
}

string Common::GetLogFileLocation(){
	return Common::logFileLocation + Common::logFileName;
}

bool Common::GetNoXsl() {
	return Common::noXsl;
}

string Common::GetDefinitionIdsString() {
	return Common::definitionIds;
}

string Common::GetDefinitionIdsFile() {
	return Common::definitionIdsFile;
}

bool Common::GetLimitEvaluationToDefinitionIds() {
	return Common::limitEvaluationToDefinitionIds;
}

bool Common::GetDoDefinitionSchematron() {
	return Common::doDefinitionSchematron;
}

bool Common::GetDoSystemCharacteristicsSchematron() {
	return Common::doSystemCharacteristicsSchematron;
}

bool Common::GetDoResultsSchematron() {
	return Common::doResultsSchematron;
}

string Common::GetDefinitionSchematronPath() {
	return Common::definitionSchematronPath;
}

string Common::GetSystemCharacteristicsSchematronPath() {
	return Common::systemCharacteristicsSchematronPath;
}

string Common::GetResultsSchematronPath() {
	return Common::resultsSchematronPath;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Mutators  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

void Common::SetDataFile(string fileIn) {
	dataFile = fileIn;	
}

void Common::SetGenerateMD5(bool genMD5In) {
	generateMD5 = genMD5In;
}

void Common::SetXMLfile(string xmlfileIn) {
	if(Common::FileExists(xmlfileIn)) {
		xmlfile = xmlfileIn;
	} else {
		throw CommonException("The specified definition file does not exist! " + xmlfileIn);
	}
}

void Common::SetSchemaPath(string schemapathIn)
{
	schemapath = schemapathIn;
}

void Common::SetXMLfileMD5(string xmlfileMD5In) {
	xmlfileMD5 = xmlfileMD5In;
}

void Common::SetOutputFilename(string outputFilenameIn)
{
	outputFilename = outputFilenameIn;
}

void Common::SetUseProvidedData(bool useDataIn) {
	useProvidedData = useDataIn;
}

void Common::SetExternalVariableFile(string varFilenameIn) {

	if(Common::FileExists(varFilenameIn)) {
		externalVariablesFile = varFilenameIn;
	} else {
		throw CommonException("The specified external variables file does not exist! " + varFilenameIn);
	}
}

void Common::SetDirectivesConfigFile(string varFilenameIn) {

	if(Common::FileExists(varFilenameIn)) {
		directivesConfigFile = varFilenameIn;
	} else {
		throw CommonException("The specified directives file does not exist! " + varFilenameIn);
	}
}

void Common::SetVerifyXMLfile(bool verifyXMLfileIn)
{
	verifyXMLfile = verifyXMLfileIn;
}

void Common::SetXSLFilename(string in) {
	
	if(Common::FileExists(in)) {
		Common::xslFile = in;
	} else {
		throw CommonException("The specified results xsl file does not exist! " + in);
	}
}

void Common::SetXSLOutputFilename(string in) {
	Common::xslOutputFile = in;
}
void Common::SetNoXsl(bool noXsl) {
	Common::noXsl = noXsl;
}

void Common::SetLogFileLocation(string in){
	if ( in.compare("") != 0 ){
		// if the path does not end with a file separator then add it
		if ( in.at(in.length() - 1) != Common::fileSeperator ){
			in.push_back(Common::fileSeperator);
		}
	}
	Common::logFileLocation = in;
}

void Common::SetDefinitionIdsString(string definitionIdsString) {
	Common::definitionIds = definitionIdsString;
}

void Common::SetDefinitionIdsFile(string definitionIdsFile) {

	if(Common::FileExists(definitionIdsFile)) {
		Common::definitionIdsFile = definitionIdsFile;
	} else {
		throw CommonException("The specified definition ids file does not exist! " + definitionIdsFile);
	}
}

void Common::SetLimitEvaluationToDefinitionIds(bool set) {
	Common::limitEvaluationToDefinitionIds = set;
}

void Common::SetDoDefinitionSchematron(bool set) {
	Common::doDefinitionSchematron = set;
}

void Common::SetDoSystemCharacteristicsSchematron(bool set) {
	Common::doSystemCharacteristicsSchematron = set;
}

void Common::SetDoResultsSchematron(bool set) {
	Common::doResultsSchematron = set;
}

void Common::SetDefinitionSchematronPath(string path) {

	if(Common::FileExists(path)) {
		Common::definitionSchematronPath = path;
	} else {
		throw CommonException("The specified definition schematron validation file does not exist! " + path);
	}
}

void Common::SetSystemCharacteristicsSchematronPath(string path) {

	if(Common::FileExists(path)) {
		Common::systemCharacteristicsSchematronPath = path;
	} else {
		throw CommonException("The specified system characteristics schematron validation file does not exist! " + path);
	}
}

void Common::SetResultsSchematronPath(string path) {

	if(Common::FileExists(path)) {
		Common::resultsSchematronPath = path;
	} else {
		throw CommonException("The specified results schematron validation file does not exist! " + path);
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

StringVector* Common::ParseDefinitionIdsFile() {
	using namespace xercesc;

	string definitinIdsFile = Common::GetDefinitionIdsFile();

	Log::Debug("Parsing definition id file for limited definition evaluation. \"" + definitinIdsFile + "\"");

	string logMessage = "     - reading input definition ids from " + definitinIdsFile + "\n";
	cout << logMessage;
	Log::UnalteredMessage(logMessage);

	StringVector* definitionIds = NULL;
	if(definitinIdsFile.compare("") == 0) {
		throw Exception("Error: An empty string was provided as the path to the definition ids file to parse.");
	} else {
		
        definitionIds = new StringVector();

		// parse the file to get the set of ids
		DOMElement* definitionsElm = XmlCommon::FindElementNS(DocumentManager::GetEvaluationIdDocument(), "evalutation-definition-ids");
		if(definitionsElm != NULL) {

			DOMNodeList* definitionElms = definitionsElm->getChildNodes();
			unsigned int i = 0;
			while(i < definitionElms->getLength()) {
				DOMNode* tmpNode = definitionElms->item(i);
				if (tmpNode->getNodeType() == DOMNode::ELEMENT_NODE) {
					DOMElement *definitionElm = (DOMElement*)tmpNode;
					
					// get the definition id and check the cache
					string definitionId = XmlCommon::GetDataNodeValue(definitionElm);
					definitionIds->push_back(definitionId);
				}
				i++;
			}
		}
	}

	return definitionIds;
}

StringVector* Common::ParseDefinitionIdsString() {
	string definitionIdsString = Common::GetDefinitionIdsString();

	Log::Debug("Parsing definition id string for limited definition evaluation. \"" + definitionIdsString + "\"");

	string logMessage = "     - parsing input definition ids to evaluate\n";
	cout << logMessage;
	Log::UnalteredMessage(logMessage);

	// validate the format of the string
	REGEX regex;
	if(!regex.IsMatch(Common::DEFINITION_ID_LIST.c_str(), definitionIdsString.c_str())) {
		throw Exception("Error: Invalid parameter format. Expected a comma separated list of definition ids. No spaces are allowed.");
	}

	// break the comma separated string into definition ids.
	StringVector* definitionIds = new StringVector();
	const char *delm = ",";

	char* theString = (char*)malloc(sizeof(char)*(definitionIdsString.length()+1));
	theString = strcpy(theString, definitionIdsString.c_str());
	char* token = strtok(theString, delm);		

	if(token == NULL) {
		if(theString != NULL) {
			free(theString);
		}
		delete definitionIds;
		throw Exception("Error parsing definition id list. A delimiter was found, but no definition ids were found. Input version string: \'" + definitionIdsString + "\'");
	} else {

		while(token != NULL) {
			//	Get the first token as a string
			string tokenStr = token;
			
			// make sure it is a valid dafinition id
			if(!regex.IsMatch(Common::DEFINITION_ID.c_str(), definitionIdsString.c_str())) {
				if(theString != NULL) {
					free(theString);
				}	
				delete definitionIds;
				throw Exception("Error: Invalid parameter format. Expected a comma separated list of definition ids. No spaces are allowed. Found invalid definition id");
			}

			// add it to the vector
			definitionIds->push_back(tokenStr);

			//	Get the next token
			token = strtok(NULL, delm);
		}
	}

	if(theString != NULL) {
		free(theString);
	}

	return definitionIds;
}

string Common::PadString(string strIn, unsigned int desiredLength) {
	while(strIn.length() < desiredLength) {
		strIn.append(" ");
	}

	return strIn;
}

string Common::PadStringWithChar(string strIn, char pad, unsigned int desiredLength) {

	while(strIn.length() < desiredLength) {
		strIn = strIn + pad;
	}

	return strIn;
}

string Common::SwitchChar(string fixedString, string oldChr, string newChr) {

	if(oldChr.length() != 1 || newChr.length() != 1)
		throw CommonException("Error: (SwitchChar) can only switch strings of length = 1.");

	size_t pos = fixedString.find(oldChr, 0);
	while (pos != string::npos)
	{
		fixedString.erase(pos, 1);
		fixedString.insert(pos, newChr);
		pos = fixedString.find(oldChr, pos+1);
	}

	return fixedString;
}

string Common::GetTimeStamp() {
	char tmpbuf[128];
	
	time_t tmpTime;
	struct tm *todayTime;
	
	// Get the time as a long integer, then convert it to local time.
	time(&tmpTime);
	todayTime = localtime(&tmpTime);
	
	// Build the time string.
	string format = "%Y-%m-%dT%H:%M:%S";
	strftime(tmpbuf, 128-1, format.c_str(), todayTime);

	// Make sure the buffer is null terminated.
	tmpbuf[sizeof(tmpbuf)-1] = '\0';
	
	return (tmpbuf);
}

bool Common::FileExists(const string &filename) {

    if (FILE * file = fopen(filename.c_str(), "r")) {
        fclose(file);
        return true;
    }

    return false;
}

string Common::ToUpper(string s) {

	string upperString;
	string::const_iterator iter;

	for(iter = s.begin(); iter != s.end(); iter++)
	{ 
		upperString += (char)std::toupper(*iter);
	}

	return upperString;
}

string Common::ToLower(string s) {

	string lowerString;
	string::const_iterator iter;

	for(iter = s.begin(); iter != s.end(); iter++)
	{ 
		lowerString += (char)std::tolower(*iter);
	}

	return lowerString;
}

string Common::BuildFilePath(const string path, const string filename) {

    if(path.compare("") == 0)
        throw Exception("An empty path was specified when building a file path.");

	string filePath = path;
    if(filename.compare("") != 0) {
        // Verify that the path that was passed into this function ends with a slash.  If
        // it doesn't, then add one.
        if (path[path.length()-1] != Common::fileSeperator)
	        filePath.append(1, Common::fileSeperator);

        if(filename[0] != Common::fileSeperator) {
			filePath.append(filename);
		} else {
			filePath.append(filename.substr(1));
		}
    }

    return filePath;
}

StringPair* Common::SplitFilePath(const string filepath) {
	if(filepath.empty() ){
        throw Exception("An empty-string filepath was specified when splitting a filepath.");
	}

	// filepaths that end in a sep char are not filepaths that refer to
	// a file.  They consist of only a path to a directory.
	// So return NULL in that case.
	if (filepath[filepath.size()-1] == Common::fileSeperator)
		return NULL;

#ifdef WIN32
	// Windows special case: check for "X:\".  If the filepath
	// has that format, it's entirely a path, without a filename
	// part.  If we don't do this, the dumb code below will
	// split it into "X:" and "".
	if (filepath.size() >= 3 &&
		isalpha(filepath[0]) &&
		filepath[1] == ':' &&
		filepath.find_first_not_of(Common::fileSeperator, 2) == string::npos)
		// find_first_not_of above handles multiple contiguous sep chars.
		// NULL return indicates there is no filename component.  It was
		// just a path.
		return NULL;
#endif

	StringPair* fpComponents = NULL;
	size_t position = filepath.rfind(Common::fileSeperator);
	
	if ( position != string::npos ){
		fpComponents = new StringPair();
		// run the path part thru the stripper.  I left the sep
		// char on the path part, because StripTrailingSeparators()
		// will intelligently handle windows paths that look like "X:\",
		// where the trailing separator should not in fact be stripped.
		fpComponents->first = Common::StripTrailingSeparators(
			filepath.substr(0, position + 1));
		fpComponents->second = filepath.substr(position + 1);
	}

	return fpComponents;
}

StringPair* Common::SplitFilePathRegex(const string filepath) {
	if(filepath.empty() ){
        throw Exception("An empty-string filepath was specified when splitting a filepath.");
	}

	StringPair* fpComponents = NULL;
	size_t position;
	string escapedDelim = IsRegexChar(Common::fileSeperator) ?
		"\\"+Common::fileSeperatorStr :
		Common::fileSeperatorStr;

	position = filepath.rfind(escapedDelim);

	if ( position != string::npos ){
		fpComponents = new StringPair();
		fpComponents->first = filepath.substr(0,position);
		fpComponents->second = filepath.substr(position+escapedDelim.size());
	}
    
	return fpComponents;
}

void Common::TrimStart(string &str) {
	// isspace is apparently overloaded, so the compiler doesn't know how to
	//   instantiate the ptr_fun template with isspace... So I gotta use this
	//   little	workaround to tell it which one to use.
	int (*myisspace)(int) = isspace;

	string::iterator iter = find_if(str.begin(), str.end(), not1(ptr_fun(myisspace)));
	str.replace(str.begin(), iter, "");
}

void Common::TrimEnd(string &str) {
	int (*myisspace)(int) = isspace;

	// there isn't a reverse find_if that I could find (ha)... so I just
	// use the same alg with reverse iterators.
	string::reverse_iterator riter = find_if(str.rbegin(), str.rend(), not1(ptr_fun(myisspace)));
	string::iterator iter = riter.base();
	str.replace(iter, str.end(), "");
}

void Common::TrimString(string &str) {
	Common::TrimStart(str);
	Common::TrimEnd(str);
}

bool Common::IsRegexChar(char c) {
	return Common::REGEX_CHARS.find(c) != string::npos;
}

/**
 * Return the full path for a given path or file name.
 *
 * Note: When this function is used in Linux, the file must exist.  If it does
 * not exist, the input path will be returned instead.
 *
 * @param path path to a file.  Example: ../../../../../../etc/passwd
 * @return Full path for the given file or relative path.  Example: /etc/passwd
 */
string Common::GetFullPath(string path) {
	char* buffer;
	string fullpath = path;

	#ifdef WIN32
	DWORD size = GetFullPathName(path.c_str(), 0, NULL, NULL);
	buffer = (char*)malloc(size);

	if (GetFullPathName(path.c_str(), size, buffer, NULL)) {
		// Since windows paths are case-insensitive, make the path lower case
		// so that string comparisons can be done easily.
		fullpath = Common::ToLower(buffer);
	}
	#else
	// The realpath function only works for files that actually exist.
	// Null will be returned for files that do not exist, resulting in
	// this function returning the input path.
	if ((buffer = realpath(path.c_str(), NULL))) {
		fullpath = buffer;
	}
	#endif

	free(buffer);
	return fullpath;
}

bool Common::EqualsIgnoreCase(const string &s1, const string &s2) {
	if (s1.size() != s2.size())
		return false;
	return equal(s1.begin(), s1.end(), s2.begin(), caseInsensitiveCmpChars);
}

string Common::StripTrailingSeparators(const string &path) {
	if (path.empty())
		return path;

#ifdef WIN32
	// Special case for windows: Check if it looks like
	// "X:\" where X is some drive letter, in which case
	// we must leave the slash on.
	if (path.size() >= 3 &&
		isalpha(path[0]) &&
		path[1]==':' &&
		path.find_first_not_of(Common::fileSeperator, 2) == string::npos)
		// substr in case there are multiple sep chars
		return path.substr(0,3);
#endif

	// look for non-separator chars.  If there are none,
	// leave the path as is.  (Actually, if the path consists
	// of multiple sep chars, return a single sep char, effectively
	// collapsing them down to one.)
	if (path.find_first_not_of(Common::fileSeperator) == string::npos)
		return Common::fileSeperatorStr;

	// ok there are some non-sep chars in the string.  If the
	// last char is a sep char, search for the last non-sep char
	// before it, and strip that suffix of sep chars off.
	if (path[path.size() - 1] == Common::fileSeperator) {
		// search backward to find the first non-sep char 
		// (which must exist, cause we already checked there
		// was at least one).
		size_t prevNonSepPos = path.find_last_not_of(Common::fileSeperator);
		return path.substr(0, prevNonSepPos + 1);
	}

	// doesn't end with a sep, so return the path unchanged.
	return path;
}

template<>
bool Common::FromString<bool>(const std::string &str, bool *to) {
	if (str=="1" || Common::EqualsIgnoreCase(str, "true")) {
		*to = true;
		return true;
	} else if (str=="0" || Common::EqualsIgnoreCase(str, "false")) {
		*to = false;
		return true;
	}

	return false;
}

//****************************************************************************************//
//							CommonException Class										  //	
//****************************************************************************************//
CommonException::CommonException(string errMsgIn, int severity, Exception* ex) : Exception(errMsgIn, severity, ex) {

}

CommonException::~CommonException() {

}
