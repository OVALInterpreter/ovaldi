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

#include "Definition.h"

DefinitionMap Definition::processedDefinitionsMap;

//****************************************************************************************//
//								Definition Class										  //	
//****************************************************************************************//
Definition::Definition() {

	this->SetId("");
	this->SetCriteria(NULL);
	this->SetResult(OvalEnum::RESULT_ERROR);
	this->SetVariableInstance(1);
	this->SetVersion(1);
	this->SetWritten(false);
	this->SetAnalyzed(false);
}

Definition::~Definition() {

	delete this->GetCriteria();
}


// ***************************************************************************************	//
//								 Static members												//
// ***************************************************************************************	//

Definition* Definition::GetDefinitionById(string definitionId) {
	
	Definition* definition = NULL;
	
	// Search the cache
	definition = Definition::SearchCache(definitionId);

	// if not found try to parse it.
	if(definition == NULL) {

		DOMElement* definitionsElm = XmlCommon::FindElementNS(DocumentManager::GetDefinitionDocument(), "definitions");
		DOMElement* definitionElm = XmlCommon::FindElementByAttribute(definitionsElm, "id", definitionId);

		if(definitionElm == NULL) {
			throw new Exception("Unable to find specified definition in oval-definition document. Definition id: " + definitionId);
		}
		definition = new Definition();
		definition->Parse(definitionElm);
	}
	
	return definition;
}

// ***************************************************************************************	//
//								 Public members												//
// ***************************************************************************************	//

Criteria* Definition::GetCriteria() {

	return this->criteria;
}

void Definition::SetCriteria(Criteria* criteria) {

	this->criteria = criteria;
}

string Definition::GetId() {

	return this->id;
}

void Definition::SetId(string id) {

	this->id = id;
}

OvalEnum::ResultEnumeration Definition::GetResult() {

	return this->result;
}

void Definition::SetResult(OvalEnum::ResultEnumeration result) {

	this->result = result;
}

int Definition::GetVariableInstance() {

	return this->variableInstance;
}

void Definition::SetVariableInstance(int variableInstance) {

	this->variableInstance = variableInstance;
}

int Definition::GetVersion() {

	return this->version;
}

void Definition::SetVersion(int version) {

	this->version = version;
}

bool Definition::GetWritten() {

	return this->written;
}

void Definition::SetWritten(bool written) {

	this->written = written;
}

bool Definition::GetAnalyzed() {

	return this->analyzed;
}

void Definition::SetAnalyzed(bool analyzed) {

	this->analyzed = analyzed;
}

Definition* Definition::SearchCache(string id) {

	Definition* cachedDef = NULL;

	DefinitionMap::iterator iterator;
	iterator = Definition::processedDefinitionsMap.find(id);
	if(iterator != Definition::processedDefinitionsMap.end()) {
		cachedDef = iterator->second;
	} 

	return cachedDef;
}

void Definition::ClearCache() {

	DefinitionMap::iterator iterator;
	for(iterator = Definition::processedDefinitionsMap.begin(); iterator != Definition::processedDefinitionsMap.end(); iterator++) {
		
		Definition* def = iterator->second;
		delete def;
	}
	
	Definition::processedDefinitionsMap.clear();
}

void Definition::Write(DOMElement* parentElm) {

	if(!this->GetWritten()) {
		this->SetWritten(true);

		// get the parent document
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* resultDoc = parentElm->getOwnerDocument();

		// create a new definition element
		DOMElement* definitionElm = XmlCommon::AddChildElement(resultDoc, parentElm, "definition");

		// add the attributes
		XmlCommon::AddAttribute(definitionElm, "definition_id", this->GetId());
		XmlCommon::AddAttribute(definitionElm, "version", Common::ToString(this->GetVersion()));
		XmlCommon::AddAttribute(definitionElm, "result", OvalEnum::ResultToString(this->GetResult()));

		if(this->GetVariableInstance() != 1) {
			XmlCommon::AddAttribute(definitionElm, "variable_instance", Common::ToString(this->GetVariableInstance()));
		}	

		// write the criteria
		if(this->GetCriteria() != NULL) {
			this->GetCriteria()->Write(definitionElm);
		}
	}
}

void Definition::Parse(DOMElement* definitionElm) {

	// get id
	string id = XmlCommon::GetAttributeByName(definitionElm, "id");

	// get the attributes
	this->SetId(XmlCommon::GetAttributeByName(definitionElm, "id"));
	string versionStr = XmlCommon::GetAttributeByName(definitionElm, "version");
	int versionInt = atoi(versionStr.c_str());
	this->SetVersion(versionInt);
	
	// parse the criteria
	DOMElement* criteriaElm = XmlCommon::FindElementNS(definitionElm, "criteria");
	
	if(criteriaElm != NULL) {
		this->SetCriteria(new Criteria());	
		this->GetCriteria()->Parse(criteriaElm);
	}

	Definition::processedDefinitionsMap.insert(DefinitionPair(this->GetId(), this));
}

OvalEnum::ResultEnumeration Definition::Analyze() {

	if(!this->GetAnalyzed()) {
		
		if(this->GetCriteria() == NULL) {

			this->SetResult(OvalEnum::RESULT_NOT_EVALUATED);
	
		} else {

			// analyze the definition and save the result
			OvalEnum::ResultEnumeration currentResult = this->GetCriteria()->Analyze();
			this->SetResult(currentResult);
		}
		this->SetAnalyzed(true);

		// make sure the results are stored only once
		StringPair* pair = new StringPair();
		pair->first = this->GetId();
		pair->second = OvalEnum::ResultToString(this->GetResult());
		if(result == OvalEnum::RESULT_TRUE) {
			Analyzer::AppendTrueResult(pair);
		} else if(result == OvalEnum::RESULT_FALSE) {
			Analyzer::AppendFalseResult(pair);
		} else if(result == OvalEnum::RESULT_UNKNOWN) {
			Analyzer::AppendUnknownResult(pair);
		} else if(result == OvalEnum::RESULT_NOT_APPLICABLE) {
			Analyzer::AppendNotApplicableResult(pair);
		} else if(result == OvalEnum::RESULT_NOT_EVALUATED) {
			Analyzer::AppendNotEvaluatedResult(pair);
		} else if(result == OvalEnum::RESULT_ERROR) {
			Analyzer::AppendErrorResult(pair);
		}
	}

	return this->GetResult();
}

OvalEnum::ResultEnumeration Definition::NotEvaluated() {

	if(!this->GetAnalyzed()) {

		// analyze the definition and save the result
		OvalEnum::ResultEnumeration currentResult = this->GetCriteria()->NotEvaluated();
		this->SetResult(currentResult);

		this->SetAnalyzed(true);

		// make sure the results are stored only once
		StringPair* pair = new StringPair();
		pair->first = this->GetId();
		pair->second = OvalEnum::ResultToString(currentResult);
		Analyzer::AppendNotEvaluatedResult(pair);
	}

	return this->GetResult();
}
