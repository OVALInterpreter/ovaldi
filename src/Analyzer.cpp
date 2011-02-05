//
//
//****************************************************************************************//
// Copyright (c) 2002-2011, The MITRE Corporation
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

#include "Analyzer.h"

using namespace std;

DOMElement* Analyzer::definitionsElm = NULL;
DOMElement* Analyzer::testsElm = NULL;
DOMElement* Analyzer::resultsSystemElm = NULL;
DOMElement* Analyzer::resultsElm = NULL;

StringPairVector Analyzer::trueResults;
StringPairVector Analyzer::falseResults;
StringPairVector Analyzer::unknownResults;
StringPairVector Analyzer::errorResults;
StringPairVector Analyzer::notEvaluatedResults;
StringPairVector Analyzer::notApplicableResults;

//****************************************************************************************//
//								Analyzer Class											  //	
//****************************************************************************************//

Analyzer::Analyzer() {

}

Analyzer::~Analyzer() {
}

// ***************************************************************************************	//
//								Public members												//
// ***************************************************************************************	//
DOMElement* Analyzer::GetResultsElm() {	
	return Analyzer::resultsElm;
}

DOMElement* Analyzer::GetResultsSystemElm() {	

	if(Analyzer::resultsSystemElm == NULL) {
		DOMElement *elm = XmlCommon::AddChildElement(DocumentManager::GetResultDocument(), Analyzer::GetResultsElm(), "system");
		Analyzer::resultsSystemElm = elm;
	}
	return Analyzer::resultsSystemElm;
}

DOMElement* Analyzer::GetResultsSystemDefinitionsElm() {	

	if(Analyzer::definitionsElm == NULL) {
		DOMElement *elm = XmlCommon::AddChildElement(DocumentManager::GetResultDocument(), Analyzer::GetResultsSystemElm(), "definitions");
		Analyzer::definitionsElm = elm;
	}
	return Analyzer::definitionsElm;
}

DOMElement* Analyzer::GetResultsSystemTestsElm() {
	
	if(Analyzer::testsElm == NULL) {
		DOMElement *elm = XmlCommon::AddChildElement(DocumentManager::GetResultDocument(), Analyzer::GetResultsSystemElm(), "tests");
		Analyzer::testsElm = elm;
	}
	return Analyzer::testsElm;
}



void Analyzer::AppendTrueResult(StringPair* pair) {
	return Analyzer::trueResults.push_back(pair);
}

void Analyzer::AppendFalseResult(StringPair* pair) {
	return Analyzer::falseResults.push_back(pair);
}

void Analyzer::AppendErrorResult(StringPair* pair) {
	return Analyzer::errorResults.push_back(pair);
}

void Analyzer::AppendUnknownResult(StringPair* pair) {
	return Analyzer::unknownResults.push_back(pair);
}

void Analyzer::AppendNotApplicableResult(StringPair* pair) {
	return Analyzer::notApplicableResults.push_back(pair);
}

void Analyzer::AppendNotEvaluatedResult(StringPair* pair) {
	return Analyzer::notEvaluatedResults.push_back(pair);
}

void Analyzer::Run() {

	this->InitResultsDocument();

	// get the definitions element in the definitions file
	int prevIdLength = 1;
	int curIdLength = 1;
	DOMElement* definitionsElm = XmlCommon::FindElementNS(DocumentManager::GetDefinitionDocument(), "definitions");
	if(definitionsElm != NULL) {

		if(!Log::WriteToScreen())
			cout << "      Analyzing definition:  "; 

		DOMNodeList* definitionElms = definitionsElm->getChildNodes();
		unsigned int i = 0;
		while(i < definitionElms->getLength()) {
			DOMNode* tmpNode = definitionElms->item(i);
			if (tmpNode->getNodeType() == DOMNode::ELEMENT_NODE) {
				DOMElement *definitionElm = (DOMElement*)tmpNode;
				
				// get the definition id and check the cache
				string definitionId = XmlCommon::GetAttributeByName(definitionElm, "id");
				if(Definition::SearchCache(definitionId) == NULL) {

					Log::Debug("Analyzing definition: " + definitionId);
					
					if(!Log::WriteToScreen()) {
						curIdLength = definitionId.length();
						string blankSpaces = "";
						if(prevIdLength > curIdLength)
							blankSpaces = Common::PadStringWithChar(blankSpaces, ' ', prevIdLength-curIdLength);

						string backSpaces = "";
						backSpaces = Common::PadStringWithChar(backSpaces, '\b', prevIdLength);
						string endBackSpaces = "";
						endBackSpaces = Common::PadStringWithChar(endBackSpaces, '\b', blankSpaces.length());
						cout << backSpaces << definitionId << blankSpaces << endBackSpaces;
					}

					Definition* def = Definition::GetDefinitionById(definitionId);
					def->Analyze();
					def->Write(Analyzer::GetResultsSystemDefinitionsElm());					
					prevIdLength = definitionId.length();
				}
   			}
			i++;
		}

		if(!Log::WriteToScreen()) {
			string fin = " FINISHED ";
			int curLen = fin.length();
			string blankSpaces = "";
			if(prevIdLength > curLen)
				blankSpaces = Common::PadStringWithChar(blankSpaces, ' ', prevIdLength-curLen);
			string backSpaces = "";
			backSpaces = Common::PadStringWithChar(backSpaces, '\b', prevIdLength);
			cout << backSpaces << fin << blankSpaces << endl;
		}

		Definition::ClearCache();
		Test::ClearCache();
		Item::ClearCache();
		State::ClearCache();
		VariableValue::ClearCache();

		this->FinializeResultsDocument();

	} else {
		string logMessage = "\n    No definitions found in the input oval-definitions document! \n";
		cout << logMessage;
		Log::UnalteredMessage(logMessage);
	}
}

void Analyzer::Run(StringVector* definitionIds) {

	this->InitResultsDocument();

	// Get the definitions element
	DOMElement* definitionsElm = XmlCommon::FindElementNS(DocumentManager::GetDefinitionDocument(), "definitions");
	if(definitionsElm != NULL) {
		if(!Log::WriteToScreen())
			cout << "      Analyzing definition:  "; 

		int prevIdLength = 1;
		int curIdLength = 1;

		// evaluate each sppecified definition id
		StringVector::iterator iterator;
		for(iterator = definitionIds->begin(); iterator != definitionIds->end(); iterator++) {

			string definitionId = (*iterator);
			if(Definition::SearchCache(definitionId) == NULL) {
				// get the definition element by its id
				DOMElement *definitionElm = XmlCommon::FindElementByAttribute(definitionsElm, "id", definitionId);

				if(definitionElm != NULL) {

					Log::Debug("Analyzing definition: " + definitionId);
							
					if(!Log::WriteToScreen()) {
						curIdLength = definitionId.length();
						string blankSpaces = "";
						if(prevIdLength > curIdLength)
							blankSpaces = Common::PadStringWithChar(blankSpaces, ' ', prevIdLength-curIdLength);

						string backSpaces = "";
						backSpaces = Common::PadStringWithChar(backSpaces, '\b', prevIdLength);
						string endBackSpaces = "";
						endBackSpaces = Common::PadStringWithChar(endBackSpaces, '\b', blankSpaces.length());
						cout << backSpaces << definitionId << blankSpaces << endBackSpaces;
					}

					Definition* def = Definition::GetDefinitionById(definitionId);
					def->Analyze();
					def->Write(Analyzer::GetResultsSystemDefinitionsElm());					
					prevIdLength = definitionId.length();

				} else {

					// did not find the definition that was specified...
					Log::Info("Notice: " + definitionId + " was not found in " + Common::GetXMLfile());
				}
			}
		}

		DOMNodeList* definitionElms = definitionsElm->getChildNodes();
		unsigned int i = 0;
		while(i < definitionElms->getLength()) {
			DOMNode* tmpNode = definitionElms->item(i);
			if (tmpNode->getNodeType() == DOMNode::ELEMENT_NODE) {
				DOMElement *definitionElm = (DOMElement*)tmpNode;
				
				// get the dedfinition id and check the cache
				string definitionId = XmlCommon::GetAttributeByName(definitionElm, "id");
				if(Definition::SearchCache(definitionId) == NULL) {

					Log::Debug("Analyzing definition: " + definitionId);
						
					if(!Log::WriteToScreen()) {
						curIdLength = definitionId.length();
						string blankSpaces = "";
						if(prevIdLength > curIdLength)
							blankSpaces = Common::PadStringWithChar(blankSpaces, ' ', prevIdLength-curIdLength);

						string backSpaces = "";
						backSpaces = Common::PadStringWithChar(backSpaces, '\b', prevIdLength);
						string endBackSpaces = "";
						endBackSpaces = Common::PadStringWithChar(endBackSpaces, '\b', blankSpaces.length());
						cout << backSpaces << definitionId << blankSpaces << endBackSpaces;
					}

					Definition* def = Definition::GetDefinitionById(definitionId);
					def->NotEvaluated();
					def->Write(Analyzer::GetResultsSystemDefinitionsElm());
					prevIdLength = definitionId.length();					
				}
   			}
			i++;
		}

		if(!Log::WriteToScreen()) {
			string fin = " FINISHED ";
			int curLen = fin.length();
			string blankSpaces = "";
			if(prevIdLength > curLen)
				blankSpaces = Common::PadStringWithChar(blankSpaces, ' ', prevIdLength-curLen);
			string backSpaces = "";
			backSpaces = Common::PadStringWithChar(backSpaces, '\b', prevIdLength);
			cout << backSpaces << fin << blankSpaces << endl;
		}

		Definition::ClearCache();
		Test::ClearCache();
		Item::ClearCache();
		State::ClearCache();
		VariableValue::ClearCache();

		this->FinializeResultsDocument();

	} else {
		string logMessage = "\n    No definitions found in the input oval-definitions document! \n";
		cout << logMessage;
		Log::UnalteredMessage(logMessage);
	}
}

void Analyzer::PrintResults() {

	///////////////////////////////////////////////////////////////////////////
	//	Display a header indicating that the definitions are being processed
	///////////////////////////////////////////////////////////////////////////
	cout << " ** OVAL definition results.\n" << endl;
	Log::UnalteredMessage(" ** OVAL definition results.\n\n");

	///////////////////////////////////////////////////////////////////////////
	//	Print the vulnerable results
	///////////////////////////////////////////////////////////////////////////
	cout << "    OVAL Id                                 Result" << endl;
	cout << "    -------------------------------------------------------" << endl;
	Log::UnalteredMessage("    OVAL Id                                 Result\n");
	Log::UnalteredMessage("    -------------------------------------------------------\n");

	// print each result value 
	StringPairVector::iterator it;
	for (it = this->trueResults.begin(); it != this->trueResults.end(); it++) {
		string resultStr = this->ResultPairToStr((*it));
		cout << resultStr << endl;
		Log::UnalteredMessage(resultStr + "\n");
	}
	for (it = this->falseResults.begin(); it != this->falseResults.end(); it++) {
		string resultStr = this->ResultPairToStr((*it));
		cout << resultStr << endl;
		Log::UnalteredMessage(resultStr + "\n");
	}

	for (it = this->unknownResults.begin(); it != this->unknownResults.end(); it++) {
		string resultStr = this->ResultPairToStr((*it));
		cout << resultStr << endl;
		Log::UnalteredMessage(resultStr + "\n");
	}

	for (it = this->errorResults.begin(); it != this->errorResults.end(); it++) {
		string resultStr = this->ResultPairToStr((*it));
		cout << resultStr << endl;
		Log::UnalteredMessage(resultStr + "\n");
	}

	for (it = this->notEvaluatedResults.begin(); it != this->notEvaluatedResults.end(); it++) {
		string resultStr = this->ResultPairToStr((*it));
		cout << resultStr << endl;
		Log::UnalteredMessage(resultStr + "\n");
	}

	for (it = this->notApplicableResults.begin(); it != this->notApplicableResults.end(); it++) {
		string resultStr = this->ResultPairToStr((*it));
		cout << resultStr << endl;
		Log::UnalteredMessage(resultStr + "\n");
	}

	cout << "    -------------------------------------------------------\n" << endl;
	cout << "\n ** finished evaluating OVAL definitions.\n" << endl;	
	Log::UnalteredMessage("    -------------------------------------------------------\n\n");
	Log::UnalteredMessage("\n ** finished evaluating OVAL definitions.\n\n");

}

string Analyzer::ResultPairToStr(StringPair* pair) {

	string resultStr = "    ";
	resultStr.append(Common::PadString(pair->first, 40));
	resultStr.append(Common::PadString(pair->second, 15));

	return resultStr;
}

void Analyzer::InitResultsDocument() {

	// Add all the namespace information
	XmlCommon::AddXmlns(DocumentManager::GetResultDocument(), "http://oval.mitre.org/XMLSchema/oval-common-5", "oval");
	XmlCommon::AddXmlns(DocumentManager::GetResultDocument(), "http://oval.mitre.org/XMLSchema/oval-definitions-5", "oval-def");
	XmlCommon::AddXmlns(DocumentManager::GetResultDocument(), "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5", "oval-sc");
	XmlCommon::AddXmlns(DocumentManager::GetResultDocument(), "http://oval.mitre.org/XMLSchema/oval-results-5", "oval-res");
	XmlCommon::AddXmlns(DocumentManager::GetResultDocument(), "http://www.w3.org/2001/XMLSchema-instance", "xsi");

	// Add all the schema locations to start out with
	// others will be added as they appear
	XmlCommon::AddSchemaLocation(DocumentManager::GetResultDocument(), "http://oval.mitre.org/XMLSchema/oval-common-5 oval-common-schema.xsd");
	XmlCommon::AddSchemaLocation(DocumentManager::GetResultDocument(), "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5 oval-system-characteristics-schema.xsd");
	XmlCommon::AddSchemaLocation(DocumentManager::GetResultDocument(), "http://oval.mitre.org/XMLSchema/oval-definitions-5 oval-definitions-schema.xsd");
	XmlCommon::AddSchemaLocation(DocumentManager::GetResultDocument(), "http://oval.mitre.org/XMLSchema/oval-results-5 oval-results-schema.xsd");

	// add the generator element
	DOMElement *ovalResultsElm = DocumentManager::GetResultDocument()->getDocumentElement();
	DOMElement *generatorElm = XmlCommon::AddChildElement(DocumentManager::GetResultDocument(), ovalResultsElm, "generator");
	XmlCommon::AddChildElement(DocumentManager::GetResultDocument(), generatorElm, "oval:product_name", "OVAL Definition Interpreter");
	XmlCommon::AddChildElement(DocumentManager::GetResultDocument(), generatorElm, "oval:product_version", Version::GetVersion() + " Build: " + Version::GetBuild());
	XmlCommon::AddChildElement(DocumentManager::GetResultDocument(), generatorElm, "oval:schema_version", Version::GetSchemaVersion());
	XmlCommon::AddChildElement(DocumentManager::GetResultDocument(), generatorElm, "oval:timestamp", Common::GetTimeStamp());
	XmlCommon::AddChildElement(DocumentManager::GetResultDocument(), generatorElm, "vendor", Version::GetVendor());

	// add the directives
	DOMElement *directivesElm = XmlCommon::AddChildElement(DocumentManager::GetResultDocument(), ovalResultsElm, "directives");
	
	DOMElement* defTrueElm = XmlCommon::AddChildElement(DocumentManager::GetResultDocument(), directivesElm, "definition_true");
	XmlCommon::AddAttribute(defTrueElm, "reported", "true");
	XmlCommon::AddAttribute(defTrueElm, "content", "full");
	
	DOMElement* defFalseElm = XmlCommon::AddChildElement(DocumentManager::GetResultDocument(), directivesElm, "definition_false");
	XmlCommon::AddAttribute(defFalseElm, "reported", "true");
	XmlCommon::AddAttribute(defFalseElm, "content", "full");

	DOMElement* defUnknownElm = XmlCommon::AddChildElement(DocumentManager::GetResultDocument(), directivesElm, "definition_unknown");
	XmlCommon::AddAttribute(defUnknownElm, "reported", "true");
	XmlCommon::AddAttribute(defUnknownElm, "content", "full");

	DOMElement* defErrorElm = XmlCommon::AddChildElement(DocumentManager::GetResultDocument(), directivesElm, "definition_error");
	XmlCommon::AddAttribute(defErrorElm, "reported", "true");
	XmlCommon::AddAttribute(defErrorElm, "content", "full");

	DOMElement* defNotEvalElm = XmlCommon::AddChildElement(DocumentManager::GetResultDocument(), directivesElm, "definition_not_evaluated");
	XmlCommon::AddAttribute(defNotEvalElm, "reported", "true");
	XmlCommon::AddAttribute(defNotEvalElm, "content", "full");

	DOMElement* defNotAppElm = XmlCommon::AddChildElement(DocumentManager::GetResultDocument(), directivesElm, "definition_not_applicable");
	XmlCommon::AddAttribute(defNotAppElm, "reported", "true");
	XmlCommon::AddAttribute(defNotAppElm, "content", "full");

	// add the results element
	DOMElement *resultsElm = XmlCommon::AddChildElement(DocumentManager::GetResultDocument(), ovalResultsElm, "results");
	Analyzer::resultsElm = resultsElm;
}

void Analyzer::FinializeResultsDocument() {

	// add the oval_definitions element
	DOMElement *ovalResultsElm = DocumentManager::GetResultDocument()->getDocumentElement();
	DOMElement* definitionNode = (DOMElement*)DocumentManager::GetResultDocument()->importNode(DocumentManager::GetDefinitionDocument()->getDocumentElement(), true);
	ovalResultsElm->insertBefore(definitionNode, this->resultsElm);
	// need to clean up the attributes on the oval_definitiosn element.
	// copy all namespaces the document root
	// add all schema locations to the document root.
	// leave only the xmlns attribute on the element to set the default ns for all child elements.
	XmlCommon::CopyNamespaces(DocumentManager::GetDefinitionDocument(), DocumentManager::GetResultDocument());
	XmlCommon::CopySchemaLocation(DocumentManager::GetDefinitionDocument(), DocumentManager::GetResultDocument());
	XmlCommon::RemoveAttributes(definitionNode);


	// add the oval_system characteristics element
	DOMElement* scNode = (DOMElement*)DocumentManager::GetResultDocument()->importNode(DocumentManager::GetSystemCharacterisitcsDocument()->getDocumentElement(), true);
	this->resultsSystemElm->appendChild(scNode);
	// need to clean up the attributes on the oval_definitions element.
	// copy all namespaces the document root
	// add all schema locations to the document root.
	// leave only the xmlns attribute on the element to seet the default ns for all child elements.
	XmlCommon::CopyNamespaces(DocumentManager::GetSystemCharacterisitcsDocument(), DocumentManager::GetResultDocument());
	XmlCommon::CopySchemaLocation(DocumentManager::GetSystemCharacterisitcsDocument(), DocumentManager::GetResultDocument());
	XmlCommon::RemoveAttributes(scNode);

}

//****************************************************************************************//
//								AnalyzerException Class									  //	
//****************************************************************************************//
AnalyzerException::AnalyzerException(string errMsgIn, int severity, Exception* ex) : Exception(errMsgIn, severity, ex) {

}

AnalyzerException::~AnalyzerException() {

}
