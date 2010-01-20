//
//
//****************************************************************************************//
// Copyright (c) 2002-2010, The MITRE Corporation
// All rights reserved.
//
// Redistribution and use in source	and	binary forms, with or without modification,	are
// permitted provided that the following conditions	are	met:
//
//	   * Redistributions of	source code	must retain	the	above copyright	notice,	this list
//		 of	conditions and the following disclaimer.
//	   * Redistributions in	binary form	must reproduce the above copyright notice, this	
//		 list of conditions	and	the	following disclaimer in	the	documentation and/or other
//		 materials provided	with the distribution.
//	   * Neither the name of The MITRE Corporation nor the names of	its	contributors may be
//		 used to endorse or	promote	products derived from this software	without	specific 
//		 prior written permission.
//
// THIS	SOFTWARE IS	PROVIDED BY	THE	COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A	PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
// SHALL THE COPYRIGHT OWNER OR	CONTRIBUTORS BE	LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL,	EXEMPLARY, OR CONSEQUENTIAL	DAMAGES	(INCLUDING,	BUT	NOT	LIMITED	TO,	PROCUREMENT
// OF SUBSTITUTE GOODS OR SERVICES;	LOSS OF	USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY	OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
// TORT	(INCLUDING NEGLIGENCE OR OTHERWISE)	ARISING	IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
// EVEN	IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//****************************************************************************************//

#ifndef	XMLPROCESSOR_H
#define	XMLPROCESSOR_H

#ifdef WIN32
#pragma	warning(disable:4786)
#endif

#include <string>
#include "Common.h" 

//	required xerces	includes
#include <xercesc/dom/DOMImplementationRegistry.hpp>
#include <xercesc/dom/DOMBuilder.hpp>
#include <xercesc/dom/DOMException.hpp>
#include <xercesc/dom/DOMErrorHandler.hpp>
#include <xercesc/dom/DOMError.hpp>
#include <xercesc/dom/DOMLocator.hpp>
#include <xercesc/dom/DOMNamedNodeMap.hpp>

// for dom Writer
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMWriter.hpp>
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>

// for entity resolver
#include <xercesc/dom/DOMEntityResolver.hpp>
#include <xercesc/dom/DOMInputSource.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/framework/Wrapper4InputSource.hpp>


XERCES_CPP_NAMESPACE_USE


/** 
	This class extends the default DOMEntityResolver and implments the resolve entity method 
	to support 
*/
class DataDirResolver : public DOMEntityResolver {
public:
	/**
     *     
	*/
	DOMInputSource *resolveEntity (const XMLCh *const publicId, const XMLCh *const systemId, const XMLCh *const baseURI);
};

/**
	This class uses xerces to parse, create and write XML documents.
	The XmlProcessor is a singleton. To get and instance of this class call the static Instance method.
*/
class XmlProcessor {
public:

	/** Clean up after the DOMBuilder. */
	~XmlProcessor();

	/** Static instance method is implemented to keep this calss as a singlton. 
		This method should be used to get an instance of this class.
	*/
	static XmlProcessor* Instance();
	
	/** Create a new DOMDocument with the specified root element. */ 
	XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument*	CreateDOMDocument(std::string root);
	/** Create a new DOMDocument with the specified qualifiedName and default namespace. */ 
	XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument*	CreateDOMDocumentNS(std::string namespaceURI, std::string qualifiedName);
	/** Parse the specified file and return a DOMDocument. 
		'filePathIn' should be the complete path to the desired file.
		When validating and xml file the schema must be in the same directory as the file.  
	*/
	XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument*	ParseFile(std::string);
	/** Write the DOMDocument to the specified XML file.
		filePath is the filename and path to the file that will be written
	*/
	void WriteDOMDocument(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc, std::string filePath,	bool writeToFile=true);

private:
	/** Init the XmlProcessor
		Throws an exception if there is an error.
	*/
	XmlProcessor();

	static XmlProcessor* instance;
	
	DOMBuilder *parser;
};

/** 
	This class represents an Exception that occured while running the XmlProcessor.
*/
class XmlProcessorException	: public Exception {
public:
	XmlProcessorException(std::string errMsgIn =	"",	int	severity = ERROR_FATAL);
	~XmlProcessorException();
};


/**
	Simple error handler deriviative to	install	on parser.
*/
class XmlProcessorErrorHandler : public	DOMErrorHandler {
public:
	XmlProcessorErrorHandler();
	~XmlProcessorErrorHandler();

	bool getSawErrors()	const;
	std::string getErrorMessages() const;

	bool handleError(const DOMError& domError);
	void resetErrors();

private:

	/** Unimplemented constructor */
	XmlProcessorErrorHandler(const XmlProcessorErrorHandler&);
	/** Unimplemented operators */
	void operator=(const XmlProcessorErrorHandler&);

	/**
		This is	set	if we get any errors, and is queryable via a getter
		method.	Its	used by	the	parser check if	there are errors.
	*/
	bool	fSawErrors;
	/**
		This string	is used	to store all error messages	that are generated 
	*/
	std::string	errorMessages;
};

#endif
