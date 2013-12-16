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

#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMNodeList.hpp>

#include "XmlCommon.h"
#include "DocumentManager.h"
#include "VariableFactory.h"

#include "VariableProbe.h"

using namespace std;
using namespace xercesc;

//****************************************************************************************//
//								VariableProbe Class										  //	
//****************************************************************************************//
VariableProbe *VariableProbe::instance = NULL;

VariableProbe::VariableProbe() {
}

VariableProbe::~VariableProbe() {
  instance = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* VariableProbe::Instance() {

	// Use lazy initialization
	if(instance == NULL) 
		instance = new VariableProbe();

	return instance;	
}

ItemVector* VariableProbe::CollectItems(Object *object) {

	// get the varRef from the provided object
	ObjectEntity* varRef = object->GetElementByName("var_ref");

	// check datatypes - only allow string
	if(varRef->GetDatatype() != OvalEnum::DATATYPE_STRING) {
		throw ProbeException("Error: invalid data type specified on var_ref. Found: " + OvalEnum::DatatypeToString(varRef->GetDatatype()));
	}

	// check operation - only allow  equals, not equals and pattern match
	if(varRef->GetOperation() != OvalEnum::OPERATION_EQUALS 
		&& varRef->GetOperation() != OvalEnum::OPERATION_PATTERN_MATCH 
		&& varRef->GetOperation() != OvalEnum::OPERATION_NOT_EQUAL) {
		throw ProbeException("Error: invalid operation specified on var_ref. Found: " + OvalEnum::OperationToString(varRef->GetOperation()));
	}

	ItemVector *collectedItems = new ItemVector();

	if(varRef->GetVarRef() == NULL) {
		if(varRef->GetOperation() == OvalEnum::OPERATION_EQUALS) {
			
			// get an item for this var ref
			Item* item = this->GetItemForVarId(varRef->GetValue());
			if(item != NULL) {
				collectedItems->push_back(item);
			} 

		} else {

			//
			// handle operation of not equal or pattern match
			// 

			StringVector* varIds = this->GetVariableIds();

			// loop through all vars
			StringVector::iterator iterator;
			for(iterator = varIds->begin(); iterator != varIds->end(); iterator++) {

				if(varRef->GetOperation() == OvalEnum::OPERATION_NOT_EQUAL) {
					if(varRef->GetValue().compare((*iterator)) != 0) {
						
						Item* item = this->GetItemForVarId((*iterator));
						if(item != NULL) {
							collectedItems->push_back(item);
						}
					}
				} else {
					if(this->myMatcher->IsMatch(varRef->GetValue().c_str(), (*iterator).c_str())) {
						Item* item = this->GetItemForVarId((*iterator));
						if(item != NULL) {
							collectedItems->push_back(item);
						}
					}
				}
				
			}
			delete varIds;
		}
	} else {

		StringVector* varIds = this->GetVariableIds();
	
		// loop through all varIds in the definitions document
		// only keep varIds that match operation and value and var check
		StringVector::iterator it;
		for(it = varIds->begin(); it != varIds->end(); it++) {
			ItemEntity* tmp = new ItemEntity("var_ref", (*it), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS);
			if(varRef->Analyze(tmp) == OvalEnum::RESULT_TRUE) {
				Item* item = this->GetItemForVarId((*it));
				if(item != NULL) {
					collectedItems->push_back(item);
				}
			}
			delete tmp;
			tmp = NULL;
		}
		delete varIds;
	}

	return collectedItems;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
Item* VariableProbe::CreateItem() {

	Item* item = new Item(0, 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#independent", 
						"ind-sc", 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#independent independent-system-characteristics-schema.xsd", 
						OvalEnum::STATUS_ERROR, 
						"variable_item");

	return item;
}

StringVector* VariableProbe::GetVariableIds() {

	DOMElement* variablesElm = XmlCommon::FindElementNS(DocumentManager::GetDefinitionDocument(), "variables");

	StringVector* varIds = new StringVector();
		
	if(variablesElm != NULL) {

		// loop through all child elements
		DOMNodeList *vars = variablesElm->getChildNodes();
		unsigned int index = 0;
		while(index < vars->getLength()) {
			DOMNode *tmpNode = vars->item(index);

			//	only concerned with ELEMENT_NODEs
			if (tmpNode->getNodeType() == DOMNode::ELEMENT_NODE) {
				DOMElement *var = (DOMElement*)tmpNode;

				//	get the id
				string id = XmlCommon::GetAttributeByName(var, "id");
				varIds->push_back(id);
			}

			index ++;
		}

	} else {
		delete varIds;
		throw ProbeException("Error: Variable probe is unable to locate any variables in the current oval-definitions-docuemnt.");
	}

	return varIds;
}


Item* VariableProbe::GetItemForVarId(string varId) {

	// note that if the oval-definitions document is valid the variable id will always exist. 
	// So an item should always be returned

	Item* item = NULL;
	item = this->CreateItem();
	item->SetStatus(OvalEnum::STATUS_EXISTS);
	item->AppendElement(new ItemEntity("var_ref", varId, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
		
	AbsVariable* var = VariableFactory::GetVariable(varId);

	VariableValueVector* varValues = var->GetValues();

	// loop through all values
	VariableValueVector::iterator iterator;
	for(iterator = varValues->begin(); iterator != varValues->end(); iterator++) { 		
		string value = (*iterator)->GetValue();	
		item->AppendElement(new ItemEntity("value", value, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
	}

	return item;
}
