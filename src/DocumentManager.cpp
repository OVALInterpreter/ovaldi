//
// $Id: DocumentManager.cpp 4600 2008-01-03 16:49:12Z bakerj $
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


#include "DocumentManager.h"

//****************************************************************************************//
//								DocumentManager Class									  //
//****************************************************************************************//
XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* DocumentManager::definitionDoc = NULL;
XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* DocumentManager::systemCharacterisitcsDoc = NULL;
XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* DocumentManager::resultDoc = NULL;
XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* DocumentManager::externalVariableDoc = NULL;
XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* DocumentManager::evaluationIdDoc = NULL;

// ***************************************************************************************	//
//								Public members												//
// ***************************************************************************************	//
XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* DocumentManager::GetDefinitionDocument() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	return the definition document
	//
	// -----------------------------------------------------------------------

	return DocumentManager::definitionDoc;
}

XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* DocumentManager::GetResultDocument() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	return the resultDoc document
	//
	// -----------------------------------------------------------------------

	return DocumentManager::resultDoc;
}

XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* DocumentManager::GetSystemCharacterisitcsDocument() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	return the systemCharacterisitcsDoc document
	//
	// -----------------------------------------------------------------------

	return DocumentManager::systemCharacterisitcsDoc;
}

XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* DocumentManager::GetExternalVariableDocument() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	return the externalVariableDoc document
	//	If the document has not yet been parsed parse it
	//
	// -----------------------------------------------------------------------

	if(DocumentManager::externalVariableDoc == NULL) {
		try {
			XmlProcessor *processor = XmlProcessor::Instance();
			DocumentManager::externalVariableDoc = processor->ParseFile(Common::GetExternalVariableFile());
		} catch (Exception ex) {
			Log::Debug("Error while parsing external variable file: " + Common::GetExternalVariableFile() + " " + ex.GetErrorMessage());
			throw ex;
		} catch (...) {
			Log::Debug("An unknown error occured while parsing external variable file: " + Common::GetExternalVariableFile());
			throw Exception("An unknown error occured while parsing external variable file: " + Common::GetExternalVariableFile());
		}
	}

	return DocumentManager::externalVariableDoc;
}

XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* DocumentManager::GetEvaluationIdDocument() {

	if(DocumentManager::evaluationIdDoc == NULL) {
		try {
			XmlProcessor *processor = XmlProcessor::Instance();
			DocumentManager::evaluationIdDoc = processor->ParseFile(Common::GetDefinitionIdsFile());
		} catch (Exception ex) {
			Log::Debug("Error while parsing evaluation id file: " + Common::GetDefinitionIdsFile() + " " + ex.GetErrorMessage());
			throw ex;
		} catch (...) {
			Log::Debug("An unknown error occured while parsing evaluation id file: " + Common::GetDefinitionIdsFile());
			throw Exception("An unknown error occured while parsing evaluation id file: " + Common::GetDefinitionIdsFile());
		}
	}

	return DocumentManager::evaluationIdDoc;
}

void DocumentManager::SetSystemCharacterisitcsDocument(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* sc) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	set the systemCharacterisitcsDoc document
	//
	// -----------------------------------------------------------------------

	DocumentManager::systemCharacterisitcsDoc = sc;
}

void DocumentManager::SetResultDocument(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* r) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	set the resultDoc document
	//
	// -----------------------------------------------------------------------

	DocumentManager::resultDoc = r;
}

void DocumentManager::SetDefinitionDocument(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* d) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	set the definitionDoc document
	//
	// -----------------------------------------------------------------------

	DocumentManager::definitionDoc = d;
}

void DocumentManager::SetExternalVariableDocument(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* d) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	set the externalVariableDoc document
	//
	// -----------------------------------------------------------------------

	DocumentManager::externalVariableDoc = d;
}
