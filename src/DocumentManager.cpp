//
//
//****************************************************************************************//
// Copyright (c) 2002-2013, The MITRE Corporation
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

#include <iostream>

#include "XmlProcessor.h"
#include "Common.h"
#include "Log.h"


#include "DocumentManager.h"

using namespace std;
using namespace xercesc;

//****************************************************************************************//
//								DocumentManager Class									  //
//****************************************************************************************//
DOMDocument* DocumentManager::definitionDoc = NULL;
DOMDocument* DocumentManager::systemCharacteristicsDoc = NULL;
DOMDocument* DocumentManager::resultDoc = NULL;
DOMDocument* DocumentManager::externalVariableDoc = NULL;
DOMDocument* DocumentManager::evaluationIdDoc = NULL;
DOMDocument* DocumentManager::directivesConfigDoc = NULL;

// ***************************************************************************************	//
//								Public members												//
// ***************************************************************************************	//
DOMDocument* DocumentManager::GetDefinitionDocument() {
	return DocumentManager::definitionDoc;
}

DOMDocument* DocumentManager::GetResultDocument() {
	return DocumentManager::resultDoc;
}

DOMDocument* DocumentManager::GetSystemCharacteristicsDocument() {
	return DocumentManager::systemCharacteristicsDoc;
}

DOMDocument* DocumentManager::GetExternalVariableDocument() {

	if(DocumentManager::externalVariableDoc == NULL) {
		string varFile = Common::GetExternalVariableFile();
		if(Common::FileExists(varFile)) {
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
		} else {
			Log::Debug("The external variable file does not exist! The definitions being evaluated rely on external variables for proper evaluation. Values for external variables must be specified in an external variables file. " + varFile);
			throw Exception("The external variable file does not exist! The definitions being evaluated rely on external variables for proper evaluation. Values for external variables must be specified in an external variables file. " + varFile);
		}
	}

	return DocumentManager::externalVariableDoc;
}

DOMDocument* DocumentManager::GetEvaluationIdDocument() {

	if(DocumentManager::evaluationIdDoc == NULL) {
		string idFile = Common::GetDefinitionIdsFile();
		if(Common::FileExists(idFile)) {
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
		} else {
			Log::Debug("The evaluation id file does not exist! " + idFile);
			throw Exception("The evaluation id file does not exist! " + idFile);
		}
	}

	return DocumentManager::evaluationIdDoc;
}

DOMDocument* DocumentManager::GetDirectivesConfigDocument() {

	if(DocumentManager::directivesConfigDoc == NULL) {
		string varFile = Common::GetDirectivesConfigFile();
		if(Common::FileExists(varFile)) {
			try {
				XmlProcessor *processor = XmlProcessor::Instance();
				DocumentManager::directivesConfigDoc = processor->ParseFile(Common::GetDirectivesConfigFile());
			} catch (Exception ex) {
				string msg = "Error while parsing directives file: " + Common::GetDirectivesConfigFile() + " " + ex.GetErrorMessage();
				cout << msg << endl;
				Log::Message(msg);
				throw ex;
			} catch (...) {
				string msg = "An unknown error occured while parsing directives file: " + Common::GetDirectivesConfigFile();
				cout << msg << endl;
				Log::Message(msg);
				throw Exception(msg);
			}
		} else {
			Log::Debug("The directives file does not exist! " + varFile);
			throw Exception("The directives file does not exist! " + varFile);
		}
	}

	return DocumentManager::directivesConfigDoc;
}

void DocumentManager::SetSystemCharacteristicsDocument(DOMDocument* sc) {
	DocumentManager::systemCharacteristicsDoc = sc;
}

void DocumentManager::SetResultDocument(DOMDocument* r) {
	DocumentManager::resultDoc = r;
}

void DocumentManager::SetDefinitionDocument(DOMDocument* d) {
	DocumentManager::definitionDoc = d;
}

void DocumentManager::SetExternalVariableDocument(DOMDocument* d) {
	DocumentManager::externalVariableDoc = d;
}

void DocumentManager::SetDirectivesConfigDocument(DOMDocument* d) {
	DocumentManager::directivesConfigDoc = d;
}
