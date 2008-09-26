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
#include "XmlProcessor.h"

//****************************************************************************************//
//			DataDirResolver Class                                   					  //	
//****************************************************************************************//

DOMInputSource* DataDirResolver::resolveEntity (const XMLCh *const publicId, const XMLCh *const systemId, const XMLCh *const baseURI) {
	
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
		schemapath = schemapath+"\\";
	}

	string systemIDFilename = XmlCommon::ToString(systemId);
	last = systemIDFilename.find_last_of("/\\");
	path = schemapath + systemIDFilename.substr(last+1);

	return new Wrapper4InputSource (new LocalFileInputSource (XMLString::transcode (path.c_str())));	
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

XmlProcessor::XmlProcessor() {

    try  {
        XMLPlatformUtils::Initialize();
    } catch (const XMLException& toCatch) {
        string errMsg = "Error:  An error occured durring initialization of the xml utilities:\n";
        errMsg.append(XmlCommon::ToString(toCatch.getMessage()));
		errMsg.append("\n");

		throw XmlProcessorException(errMsg);
    }	
}

XmlProcessor::~XmlProcessor() {

	//  Delete the parser itself.  Must be done prior to calling Terminate, below.
	if(parser != NULL)
		delete parser; //parser->release();

	XMLPlatformUtils::Terminate();

}

XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* XmlProcessor::ParseFile(string filePathIn) {
	
    XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *resultDocument = NULL;

    // Instantiate the DOM parser.
    static const XMLCh gLS[] = { chLatin_L, chLatin_S, chNull };
    DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(gLS);
    parser = ((DOMImplementationLS*)impl)->createDOMBuilder(DOMImplementationLS::MODE_SYNCHRONOUS, 0);

	///////////////////////////////////////////////////////
    //	Set fetuares on the builder
	///////////////////////////////////////////////////////


	parser->setFeature(XMLUni::fgDOMComments, false); // Discard Comment nodes in the document. 
    parser->setFeature(XMLUni::fgDOMDatatypeNormalization, true); // Let the validation process do its datatype normalization that is defined in the used schema language.  
	parser->setFeature(XMLUni::fgDOMNamespaces, true); //  Perform Namespace processing
	parser->setFeature(XMLUni::fgDOMValidation, true); // Report all validation errors.  
	parser->setFeature(XMLUni::fgXercesSchema, true); //  Enable the parser's schema support.
	parser->setFeature(XMLUni::fgXercesSchemaFullChecking, true); //  Enable full schema constraint checking, including checking which may be time-consuming or memory intensive. Currently, particle unique attribution constraint checking and particle derivation restriction checking are controlled by this option.  
	parser->setFeature(XMLUni::fgXercesValidationErrorAsFatal, true); //  The parser will treat validation error as fatal and will exit  
	parser->setFeature(XMLUni::fgXercesDOMHasPSVIInfo, true); // Enable storing of PSVI information in element and attribute nodes.

	///////////////////////////////////////////////////////
//****************************************************************************************//
//			The following code was added to handle air-gap operation					  //	
//****************************************************************************************//
	/* Look for XML schemas in local directory instead of Internet */
		DataDirResolver resolver;
		parser->setEntityResolver (&resolver);
//****************************************************************************************//
//			End of air-gap code															  //	
//****************************************************************************************//

	///////////////////////////////////////////////////////
    //	Add an Error Handler
	///////////////////////////////////////////////////////
	// Create a new DOMErrorHandler
	// and set it to the builder
	XmlProcessorErrorHandler *errHandler = new XmlProcessorErrorHandler();
	parser->setErrorHandler(errHandler);

    try  {
		// reset document pool
		parser->resetDocumentPool();
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

	if(errHandler->getSawErrors()) {
		string error = "Error while parsing xml file:";
		error.append(errHandler->getErrorMessages());
		throw XmlProcessorException(error);
	}

	return resultDocument;
}

XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* XmlProcessor::CreateDOMDocument(string root) {

	const XMLCh *xmlRoot = XMLString::transcode(root.c_str());

	DOMImplementation* impl =  DOMImplementationRegistry::getDOMImplementation(XMLString::transcode ("Core"));
	XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc = impl->createDocument(0, xmlRoot, 0);

	return(doc);
}

XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* XmlProcessor::CreateDOMDocumentNS(string namespaceURI, string qualifiedName) {

	const XMLCh *uri = XMLString::transcode(namespaceURI.c_str());
	const XMLCh *name = XMLString::transcode(qualifiedName.c_str());

	DOMImplementation* impl =  DOMImplementationRegistry::getDOMImplementation(XMLString::transcode ("Core"));
	XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc = impl->createDocument(uri, name, NULL);

	return(doc);
}

void XmlProcessor::WriteDOMDocument(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc,  string filePath, bool writeToFile) {

	try
	{
		// get a serializer, an instance of DOMWriter
		XMLCh tempStr[100];
		XMLString::transcode("LS", tempStr, 99);
		DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(tempStr);
		DOMWriter *theSerializer = ((DOMImplementationLS*)impl)->createDOMWriter();

		// set feature if the serializer supports the feature/mode
		if (theSerializer->canSetFeature(XMLUni::fgDOMWRTSplitCdataSections, true))
			theSerializer->setFeature(XMLUni::fgDOMWRTSplitCdataSections, true);

		if (theSerializer->canSetFeature(XMLUni::fgDOMWRTDiscardDefaultContent, true))
			theSerializer->setFeature(XMLUni::fgDOMWRTDiscardDefaultContent, true);

		if (theSerializer->canSetFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true))
			theSerializer->setFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true);

		if (theSerializer->canSetFeature(XMLUni::fgDOMWRTBOM, false))
			theSerializer->setFeature(XMLUni::fgDOMWRTBOM, false);

		//if (theSerializer->canSetFeature(XMLUni::fgDOMWRTDiscardDefaultContent, true))
		//	theSerializer->setFeature(XMLUni::fgDOMWRTBOM, true);

		//
		// Plug in a format target to receive the resultant
		// XML stream from the serializer.
		//
		// StdOutFormatTarget prints the resultant XML stream
		// to stdout once it receives any thing from the serializer.
		//
		XMLFormatTarget *myFormTarget;
		if (writeToFile)
			myFormTarget = new LocalFileFormatTarget(filePath.c_str());
		else
			myFormTarget = new StdOutFormatTarget();

		//
		// do the serialization through DOMWriter::writeNode();
		//
		theSerializer->writeNode(myFormTarget, *doc);

		delete theSerializer;
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
	long line = domError.getLocation()->getLineNumber();
	long at = domError.getLocation()->getColumnNumber();

	errorMessages.append("\n\tMessage: " + msg);
	errorMessages.append("\n\tFile: " + file);
	errorMessages.append("\n\tLine " + Common::ToString(line));
	errorMessages.append("\n\tAt char " + Common::ToString(at));
	
    return true;
}

void XmlProcessorErrorHandler::resetErrors() {
    fSawErrors = false;
}

bool XmlProcessorErrorHandler::getSawErrors() const {
    return fSawErrors;
}

string XmlProcessorErrorHandler::getErrorMessages() const {
    return errorMessages;
}

