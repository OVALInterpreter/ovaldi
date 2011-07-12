//
//
//****************************************************************************************//
// Copyright (c) 2002-2011, The MITRE Corporation
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

//	required xerces	includes
#include <xercesc/dom/DOMBuilder.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMErrorHandler.hpp>
#include <xercesc/dom/DOMError.hpp>

// for entity resolver
#include <xercesc/dom/DOMEntityResolver.hpp>
#include <xercesc/dom/DOMInputSource.hpp>

#include "Exception.h"

/** 
	This class extends the default DOMEntityResolver and implments the resolve entity method 
	to support 
*/
class DataDirResolver : public xercesc::DOMEntityResolver {
public:
	/**
     *     
	*/
	xercesc::DOMInputSource *resolveEntity (const XMLCh *const publicId, const XMLCh *const systemId, const XMLCh *const baseURI);
};

/**
	Simple error handler deriviative to	install	on parser.
*/
class XmlProcessorErrorHandler : public	xercesc::DOMErrorHandler {
public:
	XmlProcessorErrorHandler();
	~XmlProcessorErrorHandler();

	bool getSawErrors()	const;
	std::string getErrorMessages() const;

	bool handleError(const xercesc::DOMError& domError);
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
	xercesc::DOMDocument*	CreateDOMDocument(std::string root);
	/** Create a new DOMDocument with the specified qualifiedName and default namespace. */ 
	xercesc::DOMDocument*	CreateDOMDocumentNS(std::string namespaceURI, std::string qualifiedName);
	/** 
	 * Parse the specified file and return a DOMDocument.  When validating an 
	 * xml file, the schema must be in the same directory as the file.  
	 * \param 'filePathIn' the complete path to the desired file.
	 * \param callerAdopts set to true if you will release the document yourself.
	 *   Otherwise, the parser owns it and will delete it when the parser is released.
	 * \return the DOM document
	 */
	xercesc::DOMDocument*	ParseFile(std::string filePathIn, bool callerAdopts = false);
	/** Write the DOMDocument to the specified XML file.
		filePath is the filename and path to the file that will be written
	*/
	void WriteDOMDocument(xercesc::DOMDocument* doc, std::string filePath,	bool writeToFile=true);

private:
	/** Init the XmlProcessor
		Throws an exception if there is an error.
	*/
	XmlProcessor();

	/**
	 * Has the common code for creating an XML parser.
	 */
	xercesc::DOMBuilder *makeParser();

	static XmlProcessor* instance;

	/**
	 * This parser has user-adoption switched on, so it never
	 * owns the documents it builds.  Users must manually destroy
	 * those documents.
	 */
	xercesc::DOMBuilder *parserWithCallerAdoption;

	/**
	 * This parser doesn't have user-adoption switched on, so it
	 * owns the documents it builds and will destroy them
	 * automatically.  Two different parsers are used, because
	 * from looking at xerces' source, it looked like switching the
	 * user adoption feature on and off in the same parser won't
	 * work.  There is a flag, where once it's switched, it didn't
	 * appear to ever be switched off.  So to make sure this isn't
	 * leaking memory, I have created separate parsers.
	 */
	xercesc::DOMBuilder *parser;

	/** The entity resolver for both parsers. */
	DataDirResolver resolver;
	/** The error handler for both parsers. */
	XmlProcessorErrorHandler errHandler;
};

/** 
	This class represents an Exception that occured while running the XmlProcessor.
*/
class XmlProcessorException	: public Exception {
public:
	XmlProcessorException(std::string errMsgIn =	"",	int	severity = ERROR_FATAL);
	~XmlProcessorException();
};



#endif
