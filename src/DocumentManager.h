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
#ifndef DOCUMENTMANAGER_H
#define DOCUMENTMANAGER_H

#include <xercesc/dom/DOMDocument.hpp>

/**
	This class manages all documents in the application.
	Managing all documents in a single location provides a unform method for 
	fetching a document.
*/
class DocumentManager {
public:
	/** Return the definition document. */
	static xercesc::DOMDocument* GetDefinitionDocument();
	/** Return the systemCharacteristicsDoc document. */
	static xercesc::DOMDocument* GetSystemCharacteristicsDocument();
	/** Return the resultDoc document. */
	static xercesc::DOMDocument* GetResultDocument();
	/** Return the externalVariableDoc document.
	 *  If the document has not yet been parsed parse it.
	 */
	static xercesc::DOMDocument* GetExternalVariableDocument();

	/** Get the evaluation id docuement.
		If the evaluation id doc has not been parsed parse it.
		@return Returns a DOMDocument.
	*/
	static xercesc::DOMDocument* GetEvaluationIdDocument();

	/** Return the config document.
		If the config doc has not been parsed parse it.
		@return Returns a DOMDocument.
	*/
	static xercesc::DOMDocument* GetDirectivesConfigDocument();

	/** Set the definitionDoc document. */
	static void SetDefinitionDocument(xercesc::DOMDocument*);
	/** Set the systemCharacteristicsDoc document. */
	static void SetSystemCharacteristicsDocument(xercesc::DOMDocument*);
	/** Set the resultDoc document. */
	static void SetResultDocument(xercesc::DOMDocument*);
	/** Set the externalVariableDoc document. */
	static void SetExternalVariableDocument(xercesc::DOMDocument*);
	/** Set the configDoc document. */
	static void SetDirectivesConfigDocument(xercesc::DOMDocument*);

private:
	static xercesc::DOMDocument* systemCharacteristicsDoc;
	static xercesc::DOMDocument* definitionDoc;
	static xercesc::DOMDocument* resultDoc;
	static xercesc::DOMDocument* externalVariableDoc;
	static xercesc::DOMDocument* evaluationIdDoc;
	static xercesc::DOMDocument* directivesConfigDoc;
};

#endif
