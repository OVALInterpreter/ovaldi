//
// $Id: DocumentManager.h 4602 2008-01-03 17:29:35Z bakerj $
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
#ifndef DOCUMENTMANAGER_H
#define DOCUMENTMANAGER_H

//	required xerces includes
//#include <xercesc/dom/DOM.hpp>
//#include <xercesc/dom/DOMAttr.hpp>
//#include <xercesc/dom/DOMDocument.hpp>
//#include <xercesc/dom/DOMNodeList.hpp>
//#include <xercesc/util/XMLString.hpp>
//#include <xercesc/util/PlatformUtils.hpp>
//#include <xercesc/parsers/AbstractDOMParser.hpp>

#include "XmlProcessor.h"
#include "Common.h"

XERCES_CPP_NAMESPACE_USE
using namespace std;

/**
	This class manages all documents in the application.
	Managing all documents in a single location provides a unform method for 
	fetching a document.
*/
class DocumentManager {
public:
	static XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* GetDefinitionDocument();
	static XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* GetSystemCharacterisitcsDocument();
	static XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* GetResultDocument();
	static XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* GetExternalVariableDocument();

	/** Get the evaluation id docuement.
		If the evaluation id doc has not been parsed parse it.
		@return Returns a DOMDocument.
	*/
	static XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* GetEvaluationIdDocument();

	static void SetDefinitionDocument(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument*);
	static void SetSystemCharacterisitcsDocument(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument*);
	static void SetResultDocument(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument*);
	static void SetExternalVariableDocument(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument*);

private:
	static XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* systemCharacterisitcsDoc;
	static XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* definitionDoc;
	static XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* resultDoc;
	static XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* externalVariableDoc;
	static XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* evaluationIdDoc;
};

#endif
