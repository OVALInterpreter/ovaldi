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
string	Common::schemapath			  = "xml";
string	Common::outputFilename	      = "results.xml";
string	Common::externalVariablesFile = "external-variables.xml";
string	Common::xmlfileMD5			  = "";
string	Common::startTime			  = "";

bool    Common::noXsl                 = false;
string  Common::xslFile			      = "results_to_html.xsl";
string  Common::xslOutputFile		  = "results.html";

string Common::logFileLocation	      = "";

bool    Common::doDefinitionSchematron   = false;
string  Common::definitionSchematronPath = "oval-definitions-schematron.xsl";

bool    Common::generateMD5			  = false;
bool    Common::useProvidedData		  = false;
bool    Common::verifyXMLfile		  = true;

bool    Common::limitEvaluationToDefinitionIds = false;
string  Common::definitionIds                  = "";
string  Common::definitionIdsFile              = "";

const string Common::REGEX_CHARS = "^$\\.[](){}*+?|";

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

string Common::GetXSLFilename(){
	// if the the XSL filename is not specified use the default path and filename
	if ( Common::xslFile.compare("results_to_html.xsl") == 0 ){
		return Common::GetSchemaPath() + Common::fileSeperatorStr + Common::xslFile;
	}
	// otherwise use the specififed value
	else{
		return Common::xslFile;	
	}
}
string Common::GetXSLOutputFilename()
{
	return Common::xslOutputFile;	
}

string Common::GetLogFileLocation(){
	return Common::logFileLocation;
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
	
	// if the filepath to the XSL schematron file is not specified use the default path and filename
	if ( Common::definitionSchematronPath.compare("oval-definitions-schematron.xsl") == 0 ){
		return Common::GetSchemaPath() + Common::fileSeperatorStr + Common::definitionSchematronPath;
	}
	// otherwise use the specififed value
	else{
		return Common::definitionSchematronPath;
	}
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

void Common::SetLogFileLocation(string in){
	Common::logFileLocation = in;
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
	REGEX* regex = new REGEX();
	if(!regex->IsMatch(Common::DEFINITION_ID_LIST.c_str(), definitionIdsString.c_str())) {
		delete regex;
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

string Common::ToString(unsigned int num) {
	ostringstream result;
	result << num;

	return result.str();
}

string Common::ToString(long num) {
	ostringstream result;
	result << num;

	return result.str();
}

string Common::ToString(long long num) {
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

string Common::ToString(double num) {

	ostringstream result;
	result << num;

	return result.str();
}

string Common::ToUpper(string s) {

	string upperString;
	string::const_iterator iter;

	for(iter = s.begin(); iter != s.end(); iter++)
	{ 
		upperString += ::toupper(*iter);
	}

	return upperString;
}

string Common::ToLower(string s) {

	string lowerString;
	string::const_iterator iter;

	for(iter = s.begin(); iter != s.end(); iter++)
	{ 
		lowerString += ::tolower(*iter);
	}

	return lowerString;
}

long long Common::StringToLongLong( char* numstr , char** endptr , int base ){

	string numstrStr = numstr;
    
	long double place;
	long long value = 0;
	
	int tmp;
    int index = 0;
	bool negate = false;
	bool overflow = false;
    bool onlyws = true;
	

	place = strlen(numstr) - 1;
  
	// Check to see if the string integer value is negative
	if ( numstr[index] == '-' ){

		negate = true;
		place = place - 1;
		index = index + 1;
	
	}

	// Check the value of the base
	// If base is equal to 0 assume a base of 10
	if ( base == 0 ){

		// If the value of the string is not 0 determine if the base value should be set to 8 (octal) or 16 (hexadecimal)
		if ( numstr[index] == '0' && numstr[index+1] !='\0' ){

			onlyws = false;	

			if ( numstr[index+1] == 'X' || numstr[index+1] == 'x' ){

				base = 16;
				place = place - 2;
				index = index + 2;

			}else{

				base = 8;
				place = place - 1;
				index = index + 1;
	
			}

		}else{
		
			base = 10;

		}

	}else{

		// Otherwise make sure the base is between 2 and 36
		// Base is invalid indicate an error
		if ( ! ( ( base >= 2 ) && ( base <= 36 ) ) ){
		
			throw Exception( "Error: Invalid base value for integer \'" + numstrStr + "\'." );

		}else{

			if ( base == 8 || base == 16 ){
			
				if ( numstr[index] == '0' ){
	
					onlyws = false;	
				
					if ( numstr[index+1] == 'X' || numstr[index+1] == 'x' ){

						//base = 16;
						place = place - 2;
						index = index + 2;
	
					}else{
	
						//base = 8;
						place = place - 1;
						index = index + 1;
	
					}
	
				}

			}

		}

	}

	while ( numstr[index] != '\0' ){

		if ( isalpha( numstr[index] ) == 0 ){
	
			tmp = numstr[index] - '0';
	
		}else{
	
			tmp = toupper( numstr[index] ) - '0' - 7;
	
		}

		// If characters are whitespace characters
		if ( isspace( numstr[index] ) != 0 ){
	
			// Skip any leading whitespace characters however decrement the place holder
			if ( onlyws ){
	
				place = place - 1;
	
			}else{
				
				// If integers have been seen and whitespaces are seen set the endptr to the rest of the string if endptr is not null
				if ( endptr != NULL ){
				
					*endptr = numstr + index;

				}
				
                break;

			}

		}else if ( overflow ){
                         
			throw Exception( "Error: Integer overflow (value exceeds the boundaries for an integer of type long long which is from " + Common::ToString( LLONG_MIN ) + " to " + Common::ToString( LLONG_MAX ) + ") for integer \'" + numstrStr + "\'." );

		}else if ( ( tmp < 0 ) || ( tmp > ( base - 1 ) ) ){
			
			throw Exception( "Error: Invalid syntax for integer \'" + numstrStr + "\'." );

		}else{
	
			// The character is a digit so convert it to its numerical value and add to the total value
			long long newvalue;
			long long add;
			long double powval;

			onlyws = false;
			errno = 0;
			
			add = ( long long )( tmp * ( powval = pow( base , place ) ) );

			// pow() results in value overflow
			if ( ( ( powval == HUGE_VAL ) || ( powval == ( - HUGE_VAL ) ) ) && ( errno == ERANGE ) ) {
			
				throw Exception( "Error: Integer overflow (value exceeds the boundaries for an integer of type long long which is from " + Common::ToString( LLONG_MIN ) + " to " + Common::ToString( LLONG_MAX ) + ") for integer \'" + numstrStr + "\'." );

			}

			// pow() results in a domain error
			if ( errno == EDOM ){
	
				throw Exception( "Error: Unable to convert the string \'" + numstrStr + "\' into an integer of type long long." ); 

			}

			if ( add == LLONG_MIN || add == LLONG_MAX ){
			
				overflow = true;

			}

			if ( negate ){
			
				newvalue = LLONG_MIN + value;
	
				if ( newvalue > ( - add ) ){
				
					overflow = true;
				}
		
			}else{
			
				newvalue = LLONG_MAX - value;
	
				if ( newvalue < add ){
				
					overflow = true;

				}

			}

			value = value + add;
			place = place - 1;
	
		}
	
		// Increment the character index
		index = index + 1;

	}


	if ( overflow ){
	    
		throw Exception( "Error: Integer overflow (value exceeds the boundaries for an integer of type long long which is from " + Common::ToString( LLONG_MIN ) + " to " + Common::ToString( LLONG_MAX ) + ") for integer \'" + numstrStr + "\'." );

	}

	// If true negate the calculated value
	if ( negate ){
	
		value = value * ( - 1 );
	
	}

	if ( onlyws ){

        throw Exception( "Error: Unable to convert the string \'" + numstrStr + "\' into an integer of type long long." );     

	}else{
		
		if ( endptr != NULL ){

			*endptr = numstr + index;
           
            if ( **endptr != '\0' ){

                throw Exception( "Error: Invalid syntax for integer \'" + numstrStr + "\'." );
    
            }
	
		}

	}

	return value;

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
			filePath.append(filename.substr(1, filename.length()-2));
		}
    }

    return filePath;
}

StringPair* Common::SplitFilePath(const string filepath) {
	if(filepath.compare("") == 0 ){
        throw Exception("An empty-string filepath was specified when splitting a filepath.");
	}

	StringPair* fpComponents = NULL;
	size_t position = filepath.rfind(Common::fileSeperator);
	
	if ( position != string::npos ){
		fpComponents = new StringPair();
		fpComponents->first = filepath.substr(0,position);
		fpComponents->second = filepath.substr(position+1,filepath.length());
		// add a file separator at the end of the path.
		(fpComponents->first).append(1, Common::fileSeperator);
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

//****************************************************************************************//
//							CommonException Class										  //	
//****************************************************************************************//
CommonException::CommonException(string errMsgIn, int severity, Exception* ex) : Exception(errMsgIn, severity, ex) {

}

CommonException::~CommonException() {

}
