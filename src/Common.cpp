//
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

#include "Common.h"

UniqueStringVector::UniqueStringVector(StringVector* strings) {
	// caller is responsible for providing a ptr to a StringVector
	// caller is responsible for cleaning up after the StringVector
	// is no longer needed.
	this->uniqueStrings = strings;
}

UniqueStringVector::~UniqueStringVector() {

}

StringVector* UniqueStringVector::GetUniqueStrings() {
	return this->uniqueStrings;
}

void UniqueStringVector::Append(string newString) {
	if(!this->Exists(newString)) {
		this->uniqueStrings->push_back(newString);
	}
}

bool UniqueStringVector::Exists(string newString) {
	bool exists = false;

	StringVector::iterator iterator;
	for(iterator = this->uniqueStrings->begin(); iterator != this->uniqueStrings->end(); iterator++) {
		if(newString.compare((*iterator)) == 0) {
			exists = true;
		}
	}			

	return exists;
}

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
string	Common::schemapath			  = "xml";
string	Common::outputFilename	      = "results.xml";
string	Common::externalVariablesFile = "external-variables.xml";
string	Common::xmlfileMD5			  = "";
string	Common::startTime			  = "";

bool    Common::noXsl                 = false;
string  Common::xslFile			      = "results_to_html.xsl";
string  Common::xslOutputFile		  = "results.html";


bool    Common::doDefinitionSchematron   = false;
string  Common::definitionSchematronPath = "oval-definitions-schematron.xsl";

bool    Common::generateMD5			  = false;
bool    Common::useProvidedData		  = false;
bool    Common::verifyXMLfile		  = true;

bool    Common::limitEvaluationToDefinitionIds = false;
string  Common::definitionIds                  = "";
string  Common::definitionIdsFile              = "";


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

bool Common::GetVerifyXMLfile()
{
	return verifyXMLfile;
}

string Common::GetXSLFilename()
{
    return Common::GetSchemaPath() + Common::fileSeperatorStr + Common::xslFile;	
}
string Common::GetXSLOutputFilename()
{
	return Common::xslOutputFile;	
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

string Common::GetDefinitionSchematronPath() {
	return Common::GetSchemaPath() + Common::fileSeperatorStr + Common::definitionSchematronPath;
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
	if(Common::FileExists(xmlfileIn.c_str())) {
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

	if(Common::FileExists(varFilenameIn.c_str())) {
		externalVariablesFile = varFilenameIn;
	} else {
		throw CommonException("The specified external variables file does not exist! " + varFilenameIn);
	}
}

void Common::SetVerifyXMLfile(bool verifyXMLfileIn)
{
	verifyXMLfile = verifyXMLfileIn;
}

void Common::SetXSLFilename(string in) {
	
	if(Common::FileExists(in.c_str())) {
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

void Common::SetDefinitionIdsString(string definitionIdsString) {
	Common::definitionIds = definitionIdsString;
}

void Common::SetDefinitionIdsFile(string definitionIdsFile) {

	if(Common::FileExists(definitionIdsFile.c_str())) {
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

void Common::SetDefinitionSchematronPath(string definitionSchematronPath) {
	
	if(Common::FileExists(definitionSchematronPath.c_str())) {
		Common::definitionSchematronPath = definitionSchematronPath;
	} else {
		throw CommonException("The specified definition schematron validation file does not exist! " + definitionSchematronPath);
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

StringVector* Common::ParseDefinitionIdsFile() {
		
	string definitinIdsFile = Common::GetDefinitionIdsFile();

	Log::Debug("Parsing definition id file for limited definition evaluation. \"" + definitinIdsFile + "\"");

	string logMessage = "     - reading input definition ids from " + definitinIdsFile + "\n";
	cout << logMessage;
	Log::UnalteredMessage(logMessage);

	StringVector* definitionIds = NULL;
	if(definitinIdsFile.compare("") == 0) {
		throw Exception("Error: An empty string was provided as the path to the definition ids file to parse.");
	} else {
		
		// parse the file to get the set of ids
		DOMElement* definitionsElm = XmlCommon::FindElementNS(DocumentManager::GetEvaluationIdDocument(), "evalutation-definition-ids");
		if(definitionsElm != NULL) {
			
			definitionIds = new StringVector();

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
	REGEX* regex = new REGEX();
	if(!regex->IsMatch(Common::DEFINITION_ID_LIST.c_str(), definitionIdsString.c_str())) {
		delete regex;
		throw Exception("Error: Invalid parameter format. Expected a comma seperated list of definition ids. No spaces are allowed.");
	}

	// break the comma seperated string into definition ids.
	StringVector* definitionIds = new StringVector();
	const char delm = ',';

	char* theString = (char*)malloc(sizeof(char)*(definitionIdsString.length()+1));
	theString = strcpy(theString, definitionIdsString.c_str());
	char* token = strtok(theString, &delm);		

	if(token == NULL) {
		if(theString != NULL) {
			free(theString);
		}
		delete regex;
		delete definitionIds;
		throw Exception("Error parsing definition id list. A delimiter was found, but no definition ids were found. Input version string: \'" + definitionIdsString + "\'");
	} else {

		while(token != NULL) {
			//	Get the first token as a string
			string tokenStr = token;
			
			// make sure it is a valid dafinition id
			if(!regex->IsMatch(Common::DEFINITION_ID.c_str(), definitionIdsString.c_str())) {
				if(theString != NULL) {
					free(theString);
				}	
				delete definitionIds;
				delete regex;
				throw Exception("Error: Invalid parameter format. Expected a comma seperated list of definition ids. No spaces are allowed. Found invalid definition id");
			}

			// add it to the vector
			definitionIds->push_back(tokenStr);

			//	Get the next token
			token = strtok(NULL, &delm);
		}
	}

	if(theString != NULL) {
		free(theString);
	}
	delete regex;

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
	char *format = "%Y-%m-%dT%H:%M:%S";
	strftime(tmpbuf, 128-1, format, todayTime);

	// Make sure the buffer is null terminated.
	tmpbuf[sizeof(tmpbuf)-1] = '\0';
	
	return (tmpbuf);
}

bool Common::FileExists(const char * filename) {

    if (FILE * file = fopen(filename, "r")) {
        fclose(file);
        return true;
    }

    return false;
}

string Common::ToString(int num) {
	ostringstream result;
	result << num;

	return result.str();
}

string Common::ToString(long num) {
	ostringstream result;
	result << num;

	return result.str();
}

string Common::ToString(bool b) {
	
	if(b) 
		return "true";
	else 
		return "false";
}

string Common::ToString(char c) {

	string str;
	str = c;
	return str;
}

string Common::ToString(unsigned long num) {

	ostringstream result;
	result << num;

	return result.str();
}

//****************************************************************************************//
//							CommonException Class										  //	
//****************************************************************************************//
CommonException::CommonException(string errMsgIn, int severity, Exception* ex) : Exception(errMsgIn, severity, ex) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the error message and then set the severity to ERROR_FATAL. This is 
	//	done with the explicit call to the Exception class constructor that 
	//	takes a single string param.
	//
	// -----------------------------------------------------------------------

}

CommonException::~CommonException() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Do nothing for now
	//
	// -----------------------------------------------------------------------

}
