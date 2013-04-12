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

#include <fstream>

//	required xerces	includes
#include <xercesc/dom/DOMImplementationRegistry.hpp>
#include <xercesc/dom/DOMException.hpp>

// for dom Writer
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMLSSerializer.hpp>
#include <xercesc/dom/DOMLSOutput.hpp>
#include <xercesc/dom/DOMLocator.hpp>
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/util/XMLUni.hpp>

// for entity resolver
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/framework/Wrapper4InputSource.hpp>

#include "XmlCommon.h"
#include "Common.h"
#include "XmlProcessor.h"
#include "Log.h"

using namespace std;
using namespace xercesc;

//****************************************************************************************//
//			DataDirResolver Class                                   					  //	
//****************************************************************************************//

DOMLSInput* DataDirResolver::resolveResource(const XMLCh *const /*resourceType*/,
											const XMLCh *const /*namespaceUri*/,
											const XMLCh *const /*publicId*/,
											const XMLCh *const systemId,
											const XMLCh *const /*baseURI*/) {
	string path = "";
	size_t last;
	string schemapath = Common::GetSchemaPath();

	if(schemapath.empty()) {
        schemapath = Common::fileSeperatorStr;
	}

    //remove double quotes
	while((last = schemapath.find_last_of("\"")) != string::npos) {
		schemapath.erase(last, 1);  
	}

    if(schemapath[schemapath.length()-1] != Common::fileSeperator) {
		schemapath += Common::fileSeperatorStr;
	}

	string systemIDFilename = XmlCommon::ToString(systemId);
	last = systemIDFilename.find_last_of("/\\");
	path = schemapath + systemIDFilename.substr(last+1);
	XMLCh* pathValue = XMLString::transcode(path.c_str());
	LocalFileInputSource* lfis = new LocalFileInputSource(pathValue);
	//Free memory allocated by XMLString::transcode(char*)
	XMLString::release(&pathValue);
	return new Wrapper4InputSource (lfis);	
}

//****************************************************************************************//
//																						  //	
//****************************************************************************************//

//****************************************************************************************//
//								XmlProcessor Class										  //	
//****************************************************************************************//
XmlProcessor* XmlProcessor::instance = NULL;
XmlProcessor* XmlProcessor::Instance() {

	if(XmlProcessor::instance == NULL) {
		XmlProcessor::instance = new XmlProcessor();
	}

	return XmlProcessor::instance;
}

XmlProcessor::XmlProcessor() : parserWithCallerAdoption(NULL), parser(NULL) {

	string schemaLocationPath = Common::BuildFilePath(
		Common::GetSchemaPath(),
		SCHEMALOCATION_CONFIG_FILENAME);

	ifstream in(schemaLocationPath.c_str());
	string schemaLocation;
	if (in) {
		string line;

		// read the whole thing in directly as a schemaLocation value.
		while (getline(in, line))
			schemaLocation += line + "\n";
	} else {
		Log::Message("Could not open schemaLocation config file: " + 
			schemaLocationPath + ".  Validation will rely on the "
			"schemaLocation attribute of instance documents.");
	}

    try  {

		parser = makeParser(schemaLocation);
		parserWithCallerAdoption = makeParser(schemaLocation);
		// add one extra feature on this parser to prevent it from
		// taking ownership of its documents.
		parserWithCallerAdoption->getDomConfig()->setParameter(XMLUni::fgXercesUserAdoptsDOMDocument, true);

    } catch (const XMLException& toCatch) {
        string errMsg = "Error:  An error occured durring initialization of the xml utilities:\n";
        errMsg.append(XmlCommon::ToString(toCatch.getMessage()));
		errMsg.append("\n");

		if (parser) parser->release();
		if (parserWithCallerAdoption) parserWithCallerAdoption->release();

		throw XmlProcessorException(errMsg);
    }	
}

XmlProcessor::~XmlProcessor() {

    XmlProcessor::instance = NULL;

	if(parser != NULL){
		parser->release();
    }

	if (parserWithCallerAdoption != NULL){
		parserWithCallerAdoption->release();
    }
}

DOMLSParser *XmlProcessor::makeParser(const string &schemaLocation) {
    // Instantiate the DOM parser.
	static const XMLCh gLS[] = { chLatin_L, chLatin_S, chNull };
	DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(gLS);

	DOMLSParser *parser = ((DOMImplementationLS*)impl)->createLSParser(DOMImplementationLS::MODE_SYNCHRONOUS, XMLUni::fgDOMXMLSchemaType);

	///////////////////////////////////////////////////////
	//	Set features on the builder
	///////////////////////////////////////////////////////
	DOMConfiguration *domCfg = parser->getDomConfig();

	domCfg->setParameter(XMLUni::fgDOMComments, false); // Discard Comment nodes in the document. 
	domCfg->setParameter(XMLUni::fgDOMDatatypeNormalization, true); // Let the validation process do its datatype normalization that is defined in the used schema language.  
	domCfg->setParameter(XMLUni::fgDOMNamespaces, true); //  Perform Namespace processing
	domCfg->setParameter(XMLUni::fgDOMValidate, true); // Report all validation errors.  
	domCfg->setParameter(XMLUni::fgXercesSchema, true); //  Enable the parser's schema support.
	domCfg->setParameter(XMLUni::fgXercesSchemaFullChecking, true); //  Enable full schema constraint checking, including checking which may be time-consuming or memory intensive. Currently, particle unique attribution constraint checking and particle derivation restriction checking are controlled by this option.  
	domCfg->setParameter(XMLUni::fgXercesValidationErrorAsFatal, true); //  The parser will treat validation error as fatal and will exit  
	domCfg->setParameter(XMLUni::fgXercesDOMHasPSVIInfo, true); // Enable storing of PSVI information in element and attribute nodes.

	///////////////////////////////////////////////////////
//****************************************************************************************//
//			The following code was added to handle air-gap operation					  //	
//****************************************************************************************//
	/* Look for XML schemas in local directory instead of Internet */
	domCfg->setParameter(XMLUni::fgDOMResourceResolver, &resolver);
//****************************************************************************************//
//			End of air-gap code															  //	
//****************************************************************************************//

	///////////////////////////////////////////////////////
	//	Add an Error Handler
	///////////////////////////////////////////////////////
	domCfg->setParameter(XMLUni::fgDOMErrorHandler, &errHandler);

	// Fix a schema location if possible, so instance documents don't
	// have to set the schemaLocation attribute.  And if they do, this
	// will actually cause it to be ignored.  So this is a hard
	// overriding of the value in instance documents.
	if (!schemaLocation.empty()) {
		XMLCh *schemaLocationCstr = XMLString::transcode(schemaLocation.c_str());
		domCfg->setParameter(XMLUni::fgXercesSchemaExternalSchemaLocation, schemaLocationCstr);
		XMLString::release(&schemaLocationCstr);
	}

	return parser;
}

DOMDocument* XmlProcessor::ParseFile(string filePathIn, bool callerAdopts) {
	
    DOMDocument *resultDocument = NULL;

    try  {
		errHandler.resetErrors();
		if (callerAdopts)
			resultDocument = parserWithCallerAdoption->parseURI(filePathIn.c_str());
		else
			resultDocument = parser->parseURI(filePathIn.c_str());

    } catch (const XMLException& toCatch) {
		string error = "Error while parsing xml file:";
		error.append(filePathIn);
		error.append("\n\tMessage: \n\t");
		error.append(XmlCommon::ToString(toCatch.getMessage()));
		throw XmlProcessorException(error);

    } catch (const DOMException& toCatch) {
		string error = "Error while parsing xml file:";
		error.append(filePathIn);
		error.append("\n\tMessage: \n\t");
		error.append(XmlCommon::ToString(toCatch.msg));
		throw XmlProcessorException(error);

	} catch (...) {
        string error = "Error while parsing xml file:";
		error.append(filePathIn);
		error.append("\n\tMessage: \n\tUnknown message");
		throw XmlProcessorException(error);
    }

	if(errHandler.getSawErrors()) {
		string error = "Error while parsing xml file:";
		error.append(errHandler.getErrorMessages());
		throw XmlProcessorException(error);
	}

	return resultDocument;
}

DOMDocument* XmlProcessor::CreateDOMDocument(string root) {

	XMLCh *xmlRoot = XMLString::transcode(root.c_str());
	XMLCh* core = XMLString::transcode ("Core");
	DOMImplementation* impl =  DOMImplementationRegistry::getDOMImplementation(core);
	DOMDocument* doc = impl->createDocument(0, xmlRoot, 0);
	//Free memory allocated by XMLString::transcode(char*)
	XMLString::release(&xmlRoot);
	XMLString::release(&core);
	return(doc);
}

DOMDocument* XmlProcessor::CreateDOMDocumentNS(string namespaceURI, string qualifiedName) {

	XMLCh *uri = XMLString::transcode(namespaceURI.c_str());
	XMLCh *name = XMLString::transcode(qualifiedName.c_str());
	XMLCh *core = XMLString::transcode ("Core");
	DOMImplementation* impl =  DOMImplementationRegistry::getDOMImplementation(core);
	DOMDocument* doc = impl->createDocument(uri, name, NULL);
	//Free memory allocated by XMLString::transcode(char*)
	XMLString::release(&uri);
	XMLString::release(&name);
	XMLString::release(&core);
	return(doc);
}

void XmlProcessor::WriteDOMDocument(DOMDocument* doc,  string filePath, bool writeToFile) {

	DOMLSOutput *out = NULL;
	XMLFormatTarget *myFormTarget = NULL;
	DOMLSSerializer *theSerializer = NULL;

	try
	{
		// get a serializer, an instance of DOMWriter
		static const XMLCh gLS[] = { chLatin_L, chLatin_S, chNull };
		DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(gLS);
		theSerializer = ((DOMImplementationLS*)impl)->createLSSerializer();
		DOMConfiguration *domCfg = theSerializer->getDomConfig();

		// set feature if the serializer supports the feature/mode
		if (domCfg->canSetParameter(XMLUni::fgDOMWRTSplitCdataSections, true))
			domCfg->setParameter(XMLUni::fgDOMWRTSplitCdataSections, true);

		if (domCfg->canSetParameter(XMLUni::fgDOMWRTDiscardDefaultContent, true))
			domCfg->setParameter(XMLUni::fgDOMWRTDiscardDefaultContent, true);

		if (domCfg->canSetParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true))
			domCfg->setParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true);

		if (domCfg->canSetParameter(XMLUni::fgDOMWRTBOM, false))
			domCfg->setParameter(XMLUni::fgDOMWRTBOM, false);

		//
		// Plug in a format target to receive the resultant
		// XML stream from the serializer.
		//
		// StdOutFormatTarget prints the resultant XML stream
		// to stdout once it receives any thing from the serializer.
		//
		if (writeToFile)
			myFormTarget = new LocalFileFormatTarget(filePath.c_str());
		else
			myFormTarget = new StdOutFormatTarget();

		DOMLSOutput *out = impl->createLSOutput();
		out->setByteStream(myFormTarget);

		//
		// do the serialization through DOMWriter::writeNode();
		//
		theSerializer->write(doc, out);

		out->release();
		theSerializer->release();
		delete myFormTarget;
	}
	catch(...)
	{
		string error;
		if(writeToFile)
		{
			error.append("Error while writing Document to XML file: ");
			error.append(filePath);
		}else
		{
			error.append("Error while writing Document to screen");
		}

		if (myFormTarget) delete myFormTarget;
		if (out) out->release();
		if (theSerializer) theSerializer->release();

		throw XmlProcessorException(error);
	}
}

//****************************************************************************************//
//								XmlProcessorException Class								  //	
//****************************************************************************************//
XmlProcessorException::XmlProcessorException(string errMsgIn, int severity) : Exception(errMsgIn, severity) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the error message and then set the severity to ERROR_FATAL. This is 
	//	done with the explicit call to the Exception class constructor that 
	//	takes a single string param.
	//
	// -----------------------------------------------------------------------

}

XmlProcessorException::~XmlProcessorException() {

}

//****************************************************************************************//
//								XmlProcessorErrorHandler Class							  //	
//****************************************************************************************//
XmlProcessorErrorHandler::XmlProcessorErrorHandler() : fSawErrors(false) {
	errorMessages = "";
}

XmlProcessorErrorHandler::~XmlProcessorErrorHandler() { }

// ---------------------------------------------------------------------------
//  XmlProcessorErrorHandler: Overrides of the DOM ErrorHandler interface
// ---------------------------------------------------------------------------
bool XmlProcessorErrorHandler::handleError(const DOMError& domError) {

    fSawErrors = true;
    if (domError.getSeverity() == DOMError::DOM_SEVERITY_WARNING)
        errorMessages.append("\n\tSeverity: Warning");
    else if (domError.getSeverity() == DOMError::DOM_SEVERITY_ERROR)
        errorMessages.append("\n\tSeverity: Error");
    else
        errorMessages.append("\n\tSeverity: Fatal Error");

	string msg =  XmlCommon::ToString(domError.getMessage());
	string file = XmlCommon::ToString(domError.getLocation()->getURI());
	XMLFileLoc line = domError.getLocation()->getLineNumber();
	XMLFileLoc at = domError.getLocation()->getColumnNumber();

	errorMessages.append("\n\tMessage: " + msg);
	errorMessages.append("\n\tFile: " + file);
	errorMessages.append("\n\tLine " + Common::ToString(line));
	errorMessages.append("\n\tAt char " + Common::ToString(at));
	
    return true;
}

void XmlProcessorErrorHandler::resetErrors() {
    fSawErrors = false;
	errorMessages.clear();
}

bool XmlProcessorErrorHandler::getSawErrors() const {
    return fSawErrors;
}

string XmlProcessorErrorHandler::getErrorMessages() const {
    return errorMessages;
}

