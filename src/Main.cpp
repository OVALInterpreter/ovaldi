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

//	other includes
#include <time.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <xercesc/util/PlatformUtils.hpp>

#ifdef WIN32
	#include <comdef.h>
	#include "WindowsCommon.h"
#endif

#include "Digest.h"
#include "XmlProcessor.h"
#include "AbsDataCollector.h"
#include "Version.h"
#include "Analyzer.h"
#include "DocumentManager.h"
#include "DataCollector.h"
#include "XslCommon.h"
#include "XmlCommon.h"
#include "EntityComparator.h"
#include "OvalEnum.h"
#include "Directive.h"
#include "Log.h"
#include "Noncopyable.h"

#define EXIT_SUCCESS	0
#define	EXIT_FAILURE	1
#define BUFFER_SIZE 4096

using namespace std;
using namespace xercesc;

namespace {
	/** 
	 *  Processes the commandline arguments and enforces required arguments. 
	 *  There must be at least two arguments.  The program name and the xmlfile hash. (or
	 *  the -m flag signifing no hash is required)
	 */
	void ProcessCommandLine(int argc, char* argv[]);

	/** Prints out a list of option flags that can be used with this exe. */
	void Usage();

#ifdef WIN32
	/**
	 * Checks if we're a 32-bit process running under 64-bit
	 * windows, and prints a warning message if so.
	 */
	void CheckWow64();
#endif

	/**
	 * Runs schematron validation on the given file, via the given schematron XSL file.
	 * \return true if validation succeeded, false if validation failed.
	 */
	bool SchematronValidate(const string &fileToValidate, const string &schematronXSLFile);

	/**
	 * Enables exception-safe init/de-init of Xerces.
	 */
	class XercesInitializer : private Noncopyable {
	public:
		XercesInitializer() {
			XMLPlatformUtils::Initialize();
		}
		~XercesInitializer() {
			XMLPlatformUtils::Terminate();
		}
	};
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Main  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

int main(int argc, char* argv[]) {

///////////////////////		DEBUG	///////////////////////////////////////
#ifdef _DEBUG
	int startTime = 0;
	int endTime = 0;
	int parseStart = 0;
	int parseEnd = 0;
	int collectionStart = 0;
	int collectionEnd = 0;
	int analysisStart = 0;
	int analysisEnd = 0;
	startTime = GetTickCount();
#endif
///////////////////////		DEBUG	///////////////////////////////////////

	//////////////////////////////////////////////////////
	///////////////////  Print Header  ///////////////////
	//////////////////////////////////////////////////////

	// Get the current time measured in the number of seconds elapsed since 1/1/70.  Then
	// format this time so it is readable.
	time_t currentTime;
	time(&currentTime);
	char* timeBuffer = ctime(&currentTime);

	// Create header.
	string headerMessage = "";
	headerMessage.append("\n");
	headerMessage.append("----------------------------------------------------\n");
	headerMessage.append("OVAL Definition Interpreter\n");
    headerMessage.append("Version: " + Version::GetVersion() + " Build: " + Version::GetBuild() +"\n");
    headerMessage.append("Build date: " + Version::GetBuildDate() + "\n");
	headerMessage.append("Copyright (c) 2002-2014 - The MITRE Corporation\n");
	headerMessage.append("----------------------------------------------------\n");
	headerMessage.append("\nStart Time: ");
	headerMessage.append(timeBuffer);
	headerMessage.append("\n");

	// Send header to console.
	cout << headerMessage;

	#ifdef _DEBUG
		Log::SetLevel(Log::DEBUG);
		Log::SetToScreen(false);
	#else
		Log::SetLevel(Log::INFO);
		Log::SetToScreen(false);
	#endif

#ifdef WIN32
	// set security of COM connection to the default
	// can only be done once!
	HRESULT hres =  CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
	if (FAILED(hres)) {
		string errorMessage = _com_error(hres).ErrorMessage();
		Log::Fatal("Failed to initialize COM security: " + errorMessage);
		return EXIT_FAILURE;
	}
#endif

	//////////////////////////////////////////////////////
	/////////  Parse Command-line Options  ///////////////
	//////////////////////////////////////////////////////
	ProcessCommandLine(argc, argv);
	
	//////////////////////////////////////////////////////
	/////////  Initialize the Log System  ////////////////
	//////////////////////////////////////////////////////

	try {
		// init the log system - set the log level here
		Log::Init(Log::GetLevel(), Common::GetLogFileLocation(), Log::GetToScreen());	
	} catch (Exception ex) {
		cout << "*** Log initialization error: " << ex.GetErrorMessage() << "\n\n\n----------------------------------------------------" << endl;
		Usage();
		return 1;
	}

	// Now that the log has been initialized send the header to the log file.
	Log::UnalteredMessage(headerMessage);

#ifdef WIN32
	CheckWow64();
#endif

	//////////////////////////////////////////////////////
	//////////////////  Check MD5 Flag  //////////////////
	//////////////////////////////////////////////////////
	if(Common::GetGenerateMD5()) {

		// Open the xmlfile so we can pass it to the MD5 hash routine.  Make
		// sure we open it in binary mode.  If not, ctrl+m characters will be
		// stripped before computing the hash, resulting in the wrong hash
		// being produced.

		ifstream in(Common::GetXMLfile().c_str(), ios::in|ios::binary);
		if (!in) {
			cerr << endl << "** ERROR: Could not open file. The specified definition file does not exist.";
			Log::UnalteredMessage("\n** ERROR: Could not open file. The specified definition file does not exist.");
			exit( EXIT_FAILURE );
		}

		// Create the md5 hash.  This constructor creates a new md5 object,
		// updates the hash, finalizes the hash, and closes the FILE object.

		Digest digest;

		// Get the hash and print it to the screen.

		string hashBuf = digest.digest(in, Digest::MD5);
		cout << endl << hashBuf << endl;
		Log::UnalteredMessage(hashBuf);

		exit( EXIT_SUCCESS );
	}

#ifdef WIN32
	// Init WindowsCommon.  I want to wait until some of the startup
	// junk is complete, so we're not looking for windows domain controllers
	// and stuff every time someone just wants some commandline help...

	/**
	 * Protect WindowsCommon startup/shutdown in a guard, so we make sure
	 * its always shut down.
	 */
	class WindowsCommonSetupGuard {
	public:
		WindowsCommonSetupGuard() {
			try {
				WindowsCommon::init();
			} catch (Exception &e) {
				Log::Fatal("Initialization failure: " + e.GetErrorMessage());
				exit(EXIT_FAILURE);
			}
		}

		~WindowsCommonSetupGuard() {
			// just in case... let no exceptions out!
			// Maybe on uninit errors, I'll just let the
			// app continue; it's probably already shutting
			// down anyway...
			try {
				WindowsCommon::uninit();
			} catch (Exception &e) {
				Log::Fatal("Uninitialization failed: " +
					e.GetErrorMessage());
				//exit(EXIT_FAILURE);
			} catch (...) {
				Log::Fatal("Uninitialization failed!");
				//exit(EXIT_FAILURE);
			}
		}
	} windowsCommonSetupGuard;
#endif

	//////////////////////////////////////////////////////
	//////////////  Disable All Privileges  //////////////
	//////////////////////////////////////////////////////
	#ifdef WIN32
		if (WindowsCommon::DisableAllPrivileges() == false) {
			string errorMessage = "";

			errorMessage.append("\nERROR: Unable to disable all privileges.  The program ");
			errorMessage.append("will terminate.\n");

			cerr << errorMessage;
			Log::Fatal(errorMessage);

			exit( EXIT_FAILURE );
		}
	#endif

	//////////////////////////////////////////////////////
	///////////////  Verify oval.xml file - MD5  /////////
	//////////////////////////////////////////////////////
	string logMessage = "";

	if (Common::GetVerifyXMLfile() == true) {

		logMessage = " ** verifying the MD5 hash of '";
		logMessage.append(Common::GetXMLfile());
		logMessage.append("' file\n");

		cout << logMessage;
		Log::UnalteredMessage(logMessage);

		// Open the xmlfile so we can pass it to the MD5 hash routine.  Make sure we open
		// it in binary mode.  If not, ctrl+m characters will be stripped before computing
		// the hash, resulting in the wrong hash being produced.

		ifstream in(Common::GetXMLfile().c_str(), ios::in|ios::binary);
		if (!in) {
			string errorMessage = "";
			errorMessage.append("Unable to open the '");
 			errorMessage.append(Common::GetXMLfile());
			errorMessage.append("' file to verify it.  The program will terminate.\n");
			cerr << "\nERROR: " << errorMessage;
			Log::Fatal(errorMessage);

			exit( EXIT_FAILURE );
		}

		// Create the md5 hash.  This constructor creates a new md5 object, updates the
		// hash, finalizes the hash, and closes the FILE object.
		
		Digest digest;

		string hashBuf = digest.digest(in, Digest::MD5);

		// Compare (without regard to case) the MD5 hash we just created with the one
		// given by the user.  If the two do not match, then exit the application.  Make
		// sure we compare in both directions.  _strnicmp only checks that the first X
		// characters of string2 are the same as the first X characters of string1.
		// This means that without the second check, if the supplied datafile hash is only
		// the first character of the real hash, then the test will succeed.

		if (!Common::EqualsIgnoreCase(hashBuf, Common::GetXMLfileMD5()))
		{
			string errorMessage = "";

			errorMessage.append("The '");
			errorMessage.append(Common::GetXMLfile());
			errorMessage.append("' file is not match the provided MD5 hash.  The program will terminate.\n");

			cerr << "ERROR: " << errorMessage;
			Log::Fatal(errorMessage);

			exit( EXIT_FAILURE );
		}
	}

	// Important: gotta initialize Xerces to do any XML processing!
	XercesInitializer xercesInit;

	//////////////////////////////////////////////////////
	////////////  parse oval.xml file	//////////////////
	//////////////////////////////////////////////////////
	try {

		XmlProcessor *processor = XmlProcessor::Instance();
		
		//	Write output.log message
		logMessage = " ** parsing " + Common::GetXMLfile() + " file.\n";
		logMessage.append("    - validating xml schema.\n");
		cout << logMessage;
		Log::UnalteredMessage(logMessage);
		

		//	Parse the file
		// DEBUG
		#ifdef _DEBUG
			parseStart = GetTickCount();
		#endif
		DocumentManager::SetDefinitionDocument(processor->ParseFile(Common::GetXMLfile()));
		#ifdef _DEBUG
			parseEnd = GetTickCount();
		#endif

		//////////////////////////////////////////////////////
		///////  Check the version of the xml schema	//////
		//////////////////////////////////////////////////////
		// The interpreter is implemented to support a set major and minor version of the OVAL
		// Language and previous minor versions of that major version.
		// Make sure that the schema version of the xml file provided is less than or equal to the 
		// current major and minor version that the interpreter is built for.

		logMessage = " ** checking schema version\n";
				
		//	Get the schema version from the provided definitions.xml 
		DOMElement *schemaNode = XmlCommon::FindElementNS(DocumentManager::GetDefinitionDocument(), "schema_version");
		string strSchemaVersion = XmlCommon::GetDataNodeValue(schemaNode);
	    
		logMessage.append("     - Schema version - " + strSchemaVersion + "\n");		
		
		cout << logMessage;
		Log::UnalteredMessage(logMessage);

		OvalEnum::ResultEnumeration lteCurrentVersionResult = EntityComparator::CompareVersion(OvalEnum::OPERATION_LESS_THAN_OR_EQUAL, Version::GetVersion(), strSchemaVersion);
		OvalEnum::ResultEnumeration gteMinCompatVersionResult = EntityComparator::CompareVersion(OvalEnum::OPERATION_GREATER_THAN_OR_EQUAL, Version::GetMinumumCompatibleVersion(), strSchemaVersion);
		if(lteCurrentVersionResult != OvalEnum::RESULT_TRUE || gteMinCompatVersionResult != OvalEnum::RESULT_TRUE) {
			string errorMessage = "ERROR: Unsupported schema version found.\n";
			errorMessage.append("The schema version of the definitions file is: " + strSchemaVersion + " ");
			errorMessage.append("The OVAL Interpreter is compatible with versions ");
			errorMessage.append(Version::GetMinumumCompatibleVersion() + " through " + Version::GetVersion());
			errorMessage.append(".\n");

			cerr << errorMessage;
			Log::Fatal(errorMessage);
			exit( EXIT_FAILURE );
		}

		Directive::LoadDirectives();


		//////////////////////////////////////////////////////
		// Schematron validation							//
		//////////////////////////////////////////////////////
		
		if(Common::GetDoDefinitionSchematron()) {
			if (!SchematronValidate(Common::GetXMLfile(), Common::GetDefinitionSchematronPath()))
				exit(EXIT_FAILURE);
		} else {
			
			logMessage = " ** skipping Schematron validation\n";
			cout << logMessage;
			Log::UnalteredMessage(logMessage);
		}


		//////////////////////////////////////////////////////
		//  Get a data file									//
		//		- either run collector or parse input file	//
		//////////////////////////////////////////////////////
		//	Run the collector if desired
		if(!Common::GetUseProvidedData()) {

			//	Create a new data document
			logMessage = " ** creating a new OVAL System Characteristics file.\n";
			cout << logMessage;
			Log::UnalteredMessage(logMessage);

			DocumentManager::SetSystemCharacteristicsDocument(processor->CreateDOMDocumentNS("http://oval.mitre.org/XMLSchema/oval-system-characteristics-5", "oval_system_characteristics"));	       

			//	Initialize the data collector and then get and instance.
			DataCollector::Init();
			AbsDataCollector* dataCollector = AbsDataCollector::Instance();

			logMessage = " ** gathering data for the OVAL definitions.\n";
			cout << logMessage;
			Log::UnalteredMessage(logMessage);

			// DEBUG
			#ifdef _DEBUG
				collectionStart = GetTickCount();
			#endif

			dataCollector->Run();

			// DEBUG
			#ifdef _DEBUG
				collectionEnd = GetTickCount();
			#endif

			delete(dataCollector);

			// save the data model
			logMessage = " ** saving data model to " + Common::GetDatafile() +".\n";
			cout << logMessage;
			Log::UnalteredMessage(logMessage);
			processor->WriteDOMDocument(DocumentManager::GetSystemCharacteristicsDocument(), Common::GetDatafile());

			// Verify what we just wrote, if requested
			if (Common::GetDoSystemCharacteristicsSchematron()) {
				logMessage = " ** running XML-Schema validation on "+Common::GetDatafile()+"\n";
				cout << logMessage;
				Log::UnalteredMessage(logMessage);
				// create the DOM document and then immediately destroy it,
				// for the purposes of generating validation errors
				processor->ParseFile(Common::GetDatafile(), true)->release();
				if (!SchematronValidate(Common::GetDatafile(), Common::GetSystemCharacteristicsSchematronPath()))
					exit(EXIT_FAILURE);
			}

		//	Read in the data file
		} else {

			logMessage = " ** parsing " + Common::GetDatafile() + " for analysis.\n";
			logMessage.append("    - validating xml schema.\n");
			cout << logMessage;
			Log::UnalteredMessage(logMessage);

			//	Parse the data file
			DocumentManager::SetSystemCharacteristicsDocument(processor->ParseFile(Common::GetDatafile()));
		}
		
		//////////////////////////////////////////////////////
		///////////////		Run Analysis		//////////////
		//////////////////////////////////////////////////////

		// create a results document
		DocumentManager::SetResultDocument(processor->CreateDOMDocumentNS("http://oval.mitre.org/XMLSchema/oval-results-5", "oval_results"));

		//	Create the analyzer
		Analyzer* analyzer = new Analyzer();

		//	Output status
		logMessage = " ** running the OVAL Definition analysis.\n";
		cout << logMessage;
		Log::UnalteredMessage(logMessage);

		#ifdef _DEBUG
			analysisStart = GetTickCount();
		#endif
		
		//	run the analyzer
		if(Common::GetLimitEvaluationToDefinitionIds()){

            logMessage = " ** running the OVAL Definition analysis.\n";
		    cout << logMessage;
		    Log::UnalteredMessage(logMessage);

			string idFile = Common::GetDefinitionIdsFile();

			StringVector* ids = NULL;
			
			if(idFile.compare("") != 0) {
				ids = Common::ParseDefinitionIdsFile();	
			} else {
				ids = Common::ParseDefinitionIdsString();				
			}

            if(ids->size() == 0) {
                delete ids;
                string errorMessage = "The list of definition ids to evaluate was empty. Verify that the appropriate command line arguments were used.";
                cout << errorMessage << endl;
                Log::Info(errorMessage);

            } else {
			    analyzer->Run(ids);
                delete ids;
            }
			
		} else {
			analyzer->Run();
		}

		#ifdef _DEBUG
			analysisEnd = GetTickCount();
		#endif


		// Apply Directives
		logMessage = " ** applying directives to OVAL results.\n";
		cout << logMessage;
		Log::UnalteredMessage(logMessage);
		Directive::ApplyAll(DocumentManager::GetResultDocument());

		// print the results 
		analyzer->PrintResults();

		//	write the result document
		logMessage = " ** saving OVAL results to " + Common::GetOutputFilename() + ".\n";
		cout << logMessage;
		Log::UnalteredMessage(logMessage);
		processor->WriteDOMDocument(DocumentManager::GetResultDocument(), Common::GetOutputFilename());

		delete analyzer;

		if (Common::GetDoResultsSchematron()) {
			logMessage = " ** running XML-Schema validation on "+Common::GetOutputFilename()+"\n";
			cout << logMessage;
			Log::UnalteredMessage(logMessage);
			// create the DOM document and then immediately destroy it,
			// for the purposes of generating validation errors
			processor->ParseFile(Common::GetOutputFilename(), true)->release();
			if (!SchematronValidate(Common::GetOutputFilename(), Common::GetResultsSchematronPath()))
				exit(EXIT_FAILURE);
		}

		delete processor;

		// run the xsl
		if(!Common::GetNoXsl()) {
			logMessage = " ** running OVAL Results xsl: " + Common::GetXSLFilename() + ".\n";
			cout << logMessage;
			Log::UnalteredMessage(logMessage);
			XslCommon::ApplyXSL(Common::GetOutputFilename(), Common::GetXSLFilename(), Common::GetXSLOutputFilename());
		} else {
			logMessage = " ** skipping OVAL Results xsl\n";
			cout << logMessage;
			Log::UnalteredMessage(logMessage);
		}

	} catch(Exception ex) {
		cout << ex.GetErrorMessage() << endl;
		Log::Fatal(ex.GetErrorMessage());
		if (ex.GetSeverity() == ERROR_FATAL) {
			exit( EXIT_FAILURE );
		}
	}

	//////////////////////////////////////////////////////
	///////////////////  Print Footer  ///////////////////
	//////////////////////////////////////////////////////
	string footerMessage = "";
	footerMessage.append("\n");
	footerMessage.append("----------------------------------------------------\n");
	cout << footerMessage;
	Log::UnalteredMessage(footerMessage);

	// DEBUG
	#ifdef _DEBUG
		endTime = GetTickCount();
		cout << "Parse time: " << (parseEnd - parseStart)/1000.00 << endl;
		cout << "Collection time: " << collectionEnd - collectionStart << endl;
		cout << "Analysis time: " << analysisEnd - analysisStart << endl;
		cout << "Other time: " << (endTime - startTime) - ((analysisEnd - analysisStart) + (collectionEnd - collectionStart)) << endl;
		cout << "Run time: " << endTime - startTime << endl;
	#endif

	Log::Shutdown();

	return 0;

}

namespace {

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Functions  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
void ProcessCommandLine(int argc, char* argv[]) {

	// There must be at least two arguments.  The program name and the xmlfile hash. (or
	// the -m flag signifing no hash is required)
	//
	// Loop through each argument passed into this application.  With each one we need to
	// check to see if it is a valid option.  After checking the argument, decrement the
	// argc variable.  Therefore, with each loop, argc should get smaller and smaller until
	// it is eventually less than or equal to 1.  (NOTE: We have already checked argv[0]
	// which is why we stop when argc is less than or equal to 1)  This signifies that we
	// have run out of arguments to check.

	// these vars modify how -a (set schema path) works.  -a can also modify
	// other file paths, depending on whether the user explicitly
	// gives paths for them or not.
	bool definitionSchematronPathGiven = false;
	bool systemCharacteristicsSchematronPathGiven = false;
	bool resultsSchematronPathGiven = false;
	bool resultsXformPathGiven = false;

	try {
		while (argc > 1) {
			// Make sure that the switch control starts with a dash.
			if (argv[1][0] != '-') {
				if ((argc == 2) && (Common::GetVerifyXMLfile() == true)) {
					Common::SetXMLfileMD5(argv[1]);
					++argv;
					--argc;
					continue;
				} else {
					Usage();
					exit( EXIT_FAILURE );
				}
			}

			// Determine which option has been signalled.  Perform necessary steps.

			switch (argv[1][1]) {
				// **********  save the data  ********** //
				case 'd':

					if ((argc < 3) || (argv[2][0] == '-')) {
						Usage();
						exit( EXIT_FAILURE );
					} else {
						Common::SetDataFile(argv[2]);
						++argv;
						--argc;
					}

					break;	
					
				// **********  available options  ********** //
				case 'h':

					Usage();
					exit( EXIT_SUCCESS );

					break;

				// **********  use input data file  ********** //
				case 'i':

					if ((argc < 3) || (argv[2][0] == '-')) {
						Usage();
						exit( EXIT_FAILURE );
					} else {
						Common::SetDataFile(argv[2]);
						Common::SetUseProvidedData(true);
						if(!Common::FileExists(argv[2])) {
							string str = argv[2];
							throw CommonException("The specified data file does not exist! " + str);
						}
						++argv;
						--argc;
					}

					break;

				// **********  definition ids to evaluate  ********** //
				case 'e':

					if ((argc < 3) || (argv[2][0] == '-')) {
						Usage();
						exit( EXIT_FAILURE );
					} else {
						Common::SetDefinitionIdsString(argv[2]);
						Common::SetLimitEvaluationToDefinitionIds(true);
						++argv;
						--argc;
					}

					break;

				// **********  file containing definition ids to evaluate  ********** //
				case 'f':

					if ((argc < 3) || (argv[2][0] == '-')) {
						Usage();
						exit( EXIT_FAILURE );
					} else {
						Common::SetDefinitionIdsFile(argv[2]);
						Common::SetLimitEvaluationToDefinitionIds(true);
						++argv;
						--argc;
					}

					break;

				// **********  path to the oval definitions schematron xsl  ********** //
				case 'c':
					if ( ( argc > 2 && argv[2][0] == '-' ) || argc == 2 ) { //a path to the xsl was not specified - use default
							Common::SetDoDefinitionSchematron(true);
					}else{ //a path to the xsl was specified - use provided path
						Common::SetDefinitionSchematronPath(argv[2]);
						Common::SetDoDefinitionSchematron(true);
						definitionSchematronPathGiven = true;
						++argv;
						--argc;
					}

					break;

				// ********  don't compare xmlfile to MD5 hash  ******** //
				case 'm':
					
					Common::SetVerifyXMLfile(false);

					break;

				// **********  path to definitions.xml file  ********** //
				case 'o':

					if ((argc < 3) || (argv[2][0] == '-')) {
						Usage();
						exit( EXIT_FAILURE );
					} else {
						Common::SetXMLfile(argv[2]);
						++argv;
						--argc;
					}

					break;

				// **********  save results in XML file  ********** //
				case 'r':

					if ((argc < 3) || (argv[2][0] == '-')) {
						Usage();
						exit( EXIT_FAILURE );
					} else {
						Common::SetOutputFilename(argv[2]);
						++argv;
						--argc;
					}

					break;

				// **********  no xsl  ********** //
				case 's':

					Common::SetNoXsl(true);

					break;

				// **********  apply the specified xsl to the results XML file  ********** //
				case 't':

					if ((argc < 3) || (argv[2][0] == '-')) {
						Usage();
						exit( EXIT_FAILURE );
					} else {
						Common::SetXSLFilename(argv[2]);
						resultsXformPathGiven = true;
						++argv;
						--argc;
					}

					break;

				// **********  write specified xsl output to the specified file  ********** //
				case 'x':

					if ((argc < 3) || (argv[2][0] == '-')) {
						Usage();
						exit( EXIT_FAILURE );
					} else {
						Common::SetXSLOutputFilename(argv[2]);
						++argv;
						--argc;
					}

					break;
				
				// **********  write ovaldi.log to a specific location  ***************** //
				case 'y':
					
					if ((argc < 3) || (argv[2][0] == '-')) {
						Usage();
						exit( EXIT_FAILURE );
					} else {
						Common::SetLogFileLocation(argv[2]);
						++argv;
						--argc;
					}

					break;

				// **********  log level  ********** //
				case 'l':

					if ((argc < 3) || (argv[2][0] == '-')) {
						Usage();
						exit( EXIT_FAILURE );
					} else {
						Log::SetLevel(argv[2]);
						++argv;
						--argc;
					}					

					break;

				// **********  verbose mode  ********** //
				case 'p':

					Log::SetLevel(Log::DEBUG);
					Log::SetToScreen(true);

					break;

				// **********  location of external variable file  ********** //
				case 'v':

					if ((argc < 3) || (argv[2][0] == '-')) {
						Usage();
						exit( EXIT_FAILURE );
					} else {
						Common::SetExternalVariableFile(argv[2]);
						++argv;
						--argc;
					}

					break;

				// **********  location of directives config file  ********** //
				case 'g':

					if ((argc < 3) || (argv[2][0] == '-')) {
						Usage();
						exit( EXIT_FAILURE );
					} else {
						Common::SetDirectivesConfigFile(argv[2]);
						++argv;
						--argc;
					}

					break;

                // **********  path to directory containing OVAL schema  ********** //
			    case 'a':

				if ((argc < 3) || (argv[2][0] == '-')) {
					Usage();
					exit( EXIT_FAILURE );
				} else {
					Common::SetSchemaPath(argv[2]);

					++argv;
					--argc;
				}

				break;

				// **********  MD5 Utility  ********** //
				case 'z':

					Common::SetGenerateMD5(true);

					break;

				case 'j':
					if ( ( argc > 2 && argv[2][0] == '-' ) || argc == 2 ) { //a path to the xsl was not specified - use default
						Common::SetDoSystemCharacteristicsSchematron(true);
					} else{ //a path to the xsl was specified - use provided path
						Common::SetSystemCharacteristicsSchematronPath(argv[2]);
						Common::SetDoSystemCharacteristicsSchematron(true);
						systemCharacteristicsSchematronPathGiven = true;
						++argv;
						--argc;
					}

					break;

				case 'k':
					if ( ( argc > 2 && argv[2][0] == '-' ) || argc == 2 ) { //a path to the xsl was not specified - use default
						Common::SetDoResultsSchematron(true);
					} else{ //a path to the xsl was specified - use provided path
						Common::SetResultsSchematronPath(argv[2]);
						Common::SetDoResultsSchematron(true);
						resultsSchematronPathGiven = true;
						++argv;
						--argc;
					}

					break;

				// **********  Default  ********** //
				default:

					Usage();
					exit( EXIT_SUCCESS );
			}

			++argv;
			--argc;
		}

		// update other paths based on the current schema path,
		// for each one which the user hasn't explicitly given
		// an alternative path.  This may be a no-op if the user
		// also hasn't changed the schema path, but it won't
		// hurt.
		if (!definitionSchematronPathGiven)
			Common::SetDefinitionSchematronPath(
				Common::BuildFilePath(Common::GetSchemaPath(),
					DEFAULT_DEFINITION_SCHEMATRON_FILENAME));

		if (!systemCharacteristicsSchematronPathGiven)
			Common::SetSystemCharacteristicsSchematronPath(
				Common::BuildFilePath(Common::GetSchemaPath(),
					DEFAULT_SYSTEM_CHARACTERISTICS_SCHEMATRON_FILENAME));

		if (!resultsSchematronPathGiven)
			Common::SetResultsSchematronPath(
				Common::BuildFilePath(Common::GetSchemaPath(),
					DEFAULT_RESULTS_SCHEMATRON_FILENAME));

		if (!Common::GetNoXsl() && !resultsXformPathGiven)
			Common::SetXSLFilename(
				Common::BuildFilePath(Common::GetSchemaPath(),
					DEFAULT_RESULTS_XFORM_FILENAME));

	} catch(Exception ex) {
		cout << "*** Input error: " << ex.GetErrorMessage() << " Note that this message has not been written to the log file.\n\n\n----------------------------------------------------" << endl;
		Usage();
		exit( EXIT_FAILURE );
	}

	////////////////////////////////////////////////////////////
	// Check to make sure the MD5 hash was included if required.  
	// If not, we need to notify the user and exit.
	////////////////////////////////////////////////////////////
	if ((::Common::GetVerifyXMLfile() == true) && (::Common::GetXMLfileMD5().empty() == true)) {
		cerr << endl << "You must supply the MD5 hash for the xmlfile or use the -m ";
		cerr << "command to skip the MD5 check." << endl;
		Usage();
		exit( EXIT_FAILURE );
	}

	////////////////////////////////////////////////////////////
	// Prevent user from overwriting input files with output files.
	////////////////////////////////////////////////////////////

	StringVector inputs(0);
	StringVector outputs(0);
	
	inputs.push_back(Common::GetFullPath(Common::GetXMLfile()));
	inputs.push_back(Common::GetFullPath(Common::GetExternalVariableFile()));
	inputs.push_back(Common::GetFullPath(Common::GetXSLFilename()));
	inputs.push_back(Common::GetFullPath(Common::GetDirectivesConfigFile()));

	outputs.push_back(Common::GetFullPath(Common::GetOutputFilename()));
	outputs.push_back(Common::GetFullPath(Common::GetXSLOutputFilename()));
	outputs.push_back(Common::GetFullPath(Common::GetLogFileLocation()));

	if (Common::GetDoDefinitionSchematron()) {
		inputs.push_back(Common::GetFullPath(Common::GetDefinitionSchematronPath()));
	}

	if (Common::GetDoSystemCharacteristicsSchematron()) {
		inputs.push_back(Common::GetSystemCharacteristicsSchematronPath());
	}

	if (Common::GetDoResultsSchematron()) {
		inputs.push_back(Common::GetResultsSchematronPath());
	}

	if (Common::GetLimitEvaluationToDefinitionIds()) {
		inputs.push_back(Common::GetFullPath(Common::GetDefinitionIdsFile()));
	}

	// This variable can be either an input or an output depending on the command line flags used
	if (Common::GetUseProvidedData()) {
		inputs.push_back(Common::GetFullPath(Common::GetDatafile()));
	} else {
		outputs.push_back(Common::GetFullPath(Common::GetDatafile()));
	}

	for (unsigned int i = 0; i < inputs.size(); i++) {
		for (unsigned int j = 0; j < outputs.size(); j++) {
			if (inputs.at(i).compare(outputs.at(j)) == 0) {
				cerr << endl << "Output will overwrite input file " << inputs.at(i) << endl;
				Usage();
				exit( EXIT_FAILURE );
			}
		}
	}
}

void Usage() {

	#ifdef WIN32
	string defaultSchemaPath = "xml";
	#else
	string defaultSchemaPath = "/usr/share/ovaldi";
	#endif

	cout << endl;
	cout << "Command Line: ovaldi [options] MD5Hash" << endl;
	cout << endl;
	cout << "Options:" << endl;
	cout << "   -h           = show options available from command line." << endl;
	cout << "\n";
	
	cout << "Definition Evaluation Options:" << endl;
	cout << "   -o <string>  = path to the oval-definitions xml file. DEFAULT=\"definitions.xml\"" << endl;
	cout << "   -v <string>  = path to external variable values file. DEFAULT=\"external-variables.xml\"" << endl;
	cout << "   -e <string>  = evaluate the specified list of definitions. Supply definition ids as a comma separated list like: oval:com.example:def:123" << endl;
	cout << "   -f <string>  = path to a file containing a list of definitions to be evaluated. The file must comply with the evaluation-id schema." << endl;
	cout << "\n";
	
	cout << "Input Validation Options:" << endl;
	cout << "   -m           = do not verify the oval-definitions file with an MD5 hash." << endl;
	cout << "   -c <string>  = perform Schematron validation on the input OVAL Definitions. Path to an xsl may optionally be specified. DEFAULT=\"" << defaultSchemaPath<<Common::fileSeperator<<DEFAULT_DEFINITION_SCHEMATRON_FILENAME << '\"' << endl;
	cout << "\n";

	cout << "Data Collection Options:" << endl;
	cout << "   -a <string>  = path to the directory that contains the OVAL schema. DEFAULT=\"" << defaultSchemaPath << "\"" << endl;
	cout << "   -i <string>  = path to input System Characteristics file. Evaluation will be based on the contents of the file." << endl;
	cout << "\n";

	cout << "Result Output Options:" << endl;	
	cout << "   -d <string>  = save data to the specified XML file. DEFAULT=\"system-characteristics.xml\"" << endl;
	cout << "   -g <string>  = path to the oval directives configuration file. DEFAULT=\"directives.xml\"" << endl;
	cout << "   -r <string>  = save results to the specified XML file. DEFAULT=\"results.xml\"" << endl;
	cout << "   -s           = do not apply a stylesheet to the results xml." << endl;
	cout << "   -t <string>  = apply the specified xsl to the results xml. DEFAULT=\"" << defaultSchemaPath << Common::fileSeperatorStr << DEFAULT_RESULTS_XFORM_FILENAME<<'\"' << endl;
	cout << "   -x <string>  = output xsl transform results to the specified file. DEFAULT=\"results.html\"" << endl;
	cout << "   -j <string>  = perform schema/schematron validation on the output OVAL System Characteristics. Path to an xsl may optionally be specified. DEFAULT=\"" << defaultSchemaPath<<Common::fileSeperator<<DEFAULT_SYSTEM_CHARACTERISTICS_SCHEMATRON_FILENAME << '\"' << endl;
	cout << "   -k <string>  = perform schema/schematron validation on the output OVAL Results. Path to an xsl may optionally be specified. DEFAULT=\"" << defaultSchemaPath<<Common::fileSeperator<<DEFAULT_RESULTS_SCHEMATRON_FILENAME << '\"' << endl;
	cout << "\n";

	cout << "Other Options:" << endl;
	cout << "   -l <integer> = Log messages at the specified level. (DEBUG = 1, INFO = 2, MESSAGE = 3, FATAL = 4)" << endl;
	cout << "   -p           = print all information and error messages." << endl;
	cout << "   -y <string>  = save the ovaldi.log file to a specific location." << endl;
	cout << "   -z           = return md5 of current oval-definitions file." << endl;
	cout << endl;
}

#ifdef WIN32
	void CheckWow64() {
		if (WindowsCommon::IsWow64Process())
			Log::Info("Warning: you are running the interpreter as a 32-bit process on 64-bit Windows.  "
						"Be aware that you may get different results as compared to a 64-bit process.  "
						"We recommend running 64-bit builds on 64-bit systems.");
	}
#endif

	bool SchematronValidate(const string &fileToValidate, const string &schematronXSLFile) {
		string logMessage = " ** running Schematron validation on " + fileToValidate + "\n";
		cout << logMessage;
		Log::UnalteredMessage(logMessage);

		string result = XslCommon::ApplyXSL(fileToValidate, schematronXSLFile);
		// strip the xml declaration
		if(result.compare("") != 0) {
			size_t pos = result.rfind(">");
			if(pos != string::npos) {
				result = result.substr(pos+1, result.length()-pos);
			}
		}
		if(!result.empty()) {
			string errorMessage = "ERROR: Schematron validation failed with the following errors:\n";
			errorMessage.append(result);

			cerr << errorMessage;
			Log::Fatal(errorMessage);
			return false;
		} else {
			logMessage = "     - Schematron validation succeeded\n";
			cout << logMessage;
			Log::UnalteredMessage(logMessage);
			return true;
		}
	}
}
