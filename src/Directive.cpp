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
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMNodeList.hpp>

#include "DocumentManager.h"
#include "Log.h"
#include "XmlCommon.h"

#include "Directive.h"

using namespace std;
using namespace xercesc;

//
// Static members
//
map<OvalEnum::ClassEnumeration, DirectiveMap> Directive::directives;
bool Directive::includeSource = true;

Directive Directive::GetDirective(OvalEnum::ClassEnumeration classEnum, OvalEnum::ResultEnumeration key) {
	return *directives[classEnum][key];
}


void Directive::SetDirective(OvalEnum::ClassEnumeration classEnum, OvalEnum::ResultEnumeration key, Directive directive) {
	directives[classEnum][key] = &directive;
}

void Directive::LoadDirectives() {
	// Load the configuration XML.  If it can not be loaded, load defaults and return.
	DOMDocument* config;

    if(directives[OvalEnum::CLASS_DEFAULT][OvalEnum::RESULT_TRUE] != NULL)
        delete directives[OvalEnum::CLASS_DEFAULT][OvalEnum::RESULT_TRUE];
    if(directives[OvalEnum::CLASS_DEFAULT][OvalEnum::RESULT_FALSE] != NULL)
        delete directives[OvalEnum::CLASS_DEFAULT][OvalEnum::RESULT_FALSE];
    if(directives[OvalEnum::CLASS_DEFAULT][OvalEnum::RESULT_UNKNOWN] != NULL)
        delete directives[OvalEnum::CLASS_DEFAULT][OvalEnum::RESULT_UNKNOWN];
    if(directives[OvalEnum::CLASS_DEFAULT][OvalEnum::RESULT_ERROR] != NULL)
        delete directives[OvalEnum::CLASS_DEFAULT][OvalEnum::RESULT_ERROR];
    if(directives[OvalEnum::CLASS_DEFAULT][OvalEnum::RESULT_NOT_EVALUATED] != NULL)
        delete directives[OvalEnum::CLASS_DEFAULT][OvalEnum::RESULT_NOT_EVALUATED];
    if(directives[OvalEnum::CLASS_DEFAULT][OvalEnum::RESULT_NOT_APPLICABLE] != NULL)
        delete directives[OvalEnum::CLASS_DEFAULT][OvalEnum::RESULT_NOT_APPLICABLE];
    directives.clear();
	try {
		config = DocumentManager::GetDirectivesConfigDocument();
	} catch (Exception e) {
		// If it fails to load the configuration document, load default directives
		Log::Debug("Unable to load directives configuration file.  Using default directives.");
		directives[OvalEnum::CLASS_DEFAULT][OvalEnum::RESULT_TRUE] = new Directive(OvalEnum::RESULT_TRUE);
		directives[OvalEnum::CLASS_DEFAULT][OvalEnum::RESULT_FALSE] = new Directive(OvalEnum::RESULT_FALSE);
		directives[OvalEnum::CLASS_DEFAULT][OvalEnum::RESULT_UNKNOWN] = new Directive(OvalEnum::RESULT_UNKNOWN);
		directives[OvalEnum::CLASS_DEFAULT][OvalEnum::RESULT_ERROR] = new Directive(OvalEnum::RESULT_ERROR);
		directives[OvalEnum::CLASS_DEFAULT][OvalEnum::RESULT_NOT_EVALUATED] = new Directive(OvalEnum::RESULT_NOT_EVALUATED);
		directives[OvalEnum::CLASS_DEFAULT][OvalEnum::RESULT_NOT_APPLICABLE] = new Directive(OvalEnum::RESULT_NOT_APPLICABLE);
		return;
	}

	// Read the XML and create appropriate Directive objects.
	
	DOMElement* directivesElem = XmlCommon::FindElementNS(config, "oval_directives");
	DOMNodeList* directiveClassNodes = directivesElem->getChildNodes();
	for (unsigned int j = 0; j < directiveClassNodes->getLength(); j++) {
		DOMNode* classNode = directiveClassNodes->item(j);
		if (classNode->getNodeType() == DOMNode::ELEMENT_NODE) {
			DOMElement* classElem = (DOMElement*)classNode;

			// Find the type/class of the directive
			string elemName = XmlCommon::GetElementName(classElem);
			OvalEnum::ClassEnumeration defClass = OvalEnum::CLASS_DEFAULT;
			if (elemName.compare("class_directives") == 0) {
				defClass = OvalEnum::ToClass(XmlCommon::GetAttributeByName(classElem, "class"));
			} else if (elemName.compare("directives") == 0) {
				includeSource = (XmlCommon::GetAttributeByName(classElem, "include_source_definitions") != "false");
			} else {
				continue;
			}

			// Loop through the definition types for the current set of directives
			DOMNodeList* directiveNodes = classElem->getChildNodes();
			for (unsigned int i = 0; i < directiveNodes->getLength(); i++) {
				DOMNode* node = directiveNodes->item(i);
		
				if (node->getNodeType() == DOMNode::ELEMENT_NODE) {
					try {
						string nodeName = XmlCommon::GetElementName((DOMElement*)node);
						string strContent = XmlCommon::GetAttributeByName((DOMElement*)node, "content");
						string strReported = XmlCommon::GetAttributeByName((DOMElement*)node, "reported");
						OvalEnum::ResultEnumeration result = OvalEnum::ToResult(nodeName);
						OvalEnum::ResultContent content = OvalEnum::ToResultContent(strContent);
						bool reported = (strReported == "false") ? false : true;
						directives[defClass][result] = new Directive(result, reported, content);
					} catch (Exception e) {
						string msg = "Error reading directive from configuration file: " + e.GetErrorMessage();
						Log::Message(msg);
						cout << msg << endl;
						continue;
					}
				}
			}
		}
	}
}

void Directive::ApplyAll(DOMDocument* resultsDoc) {
	//
	// Update <directives> tag in results with correct settings
	//
	DOMElement* ovalResultsElem = XmlCommon::FindElement(resultsDoc, "oval_results");
	DOMElement* directivesElem = XmlCommon::FindElement(ovalResultsElem, "directives");
	DOMElement* ovalElem = XmlCommon::FindElement(ovalResultsElem, "oval_definitions");
	DOMNodeList* directiveNodes = directivesElem->getChildNodes();
	bool skip = includeSource;
	for (unsigned int i = 0; i < directiveNodes->getLength(); i++) {
		DOMNode* node = directiveNodes->item(i);

		if (node->getNodeType() == DOMNode::ELEMENT_NODE) {
			try {
				string nodeName = XmlCommon::GetElementName((DOMElement*)node);
				OvalEnum::ResultEnumeration result = OvalEnum::ToResult(nodeName);
				Directive directive = Directive::GetDirective(OvalEnum::CLASS_DEFAULT, result);
				XmlCommon::AddAttribute((DOMElement*)node, "content", OvalEnum::ResultContentToString(directive.GetResultContent()));
				XmlCommon::AddAttribute((DOMElement*)node, "reported", (directive.GetReported() ? "true" : "false"));

				// If all of the directives are set using the defaults (which is to include everything)
				if (skip && (!directive.GetReported() || directive.GetResultContent() != OvalEnum::RESULT_CONTENT_FULL)) {
					skip = false;
				}
			} catch (Exception e) {
				Log::Message("Failed to modify directives in results document: " + e.GetErrorMessage());
				continue;
			}
		}
	}

	// Insert any class_directives elements that override the default directives
	std::map<OvalEnum::ClassEnumeration, DirectiveMap>::iterator i;
	for (i = directives.begin(); i != directives.end(); i++) {
		OvalEnum::ClassEnumeration defClass = i->first;
		if (defClass == OvalEnum::CLASS_DEFAULT) {
			continue;
		}
		
		skip = false;
		DOMElement* classElem = XmlCommon::CreateElementNS(resultsDoc, XmlCommon::resNS, "class_directives");
		XmlCommon::AddAttribute(classElem, "class", OvalEnum::ClassToString(defClass));
		ovalResultsElem->insertBefore(classElem, ovalElem);
		for (DirectiveMap::iterator j = i->second.begin(); j != i->second.end(); j++) {
			string defType = OvalEnum::ResultToDirectiveString(j->first);
			DOMElement* defElem = XmlCommon::AddChildElementNS(resultsDoc, classElem, XmlCommon::resNS, defType);
			XmlCommon::AddAttribute(defElem, "content", OvalEnum::ResultContentToString(j->second->GetResultContent()));
			XmlCommon::AddAttribute(defElem, "reported", j->second->GetReported() ? "true" : "false");
		}
	}

	XmlCommon::AddAttribute(directivesElem, "include_source_definitions", includeSource ? "true" : "false");

	// If all of the directives are set to their default settings, skip
	// this process because it won't change anything.
	if (skip) {
		return;
	}

	// Recurse into several elements checking for references
	map<string,map<string, Directive::ElementType> > references;
	DOMElement* resultsElem = XmlCommon::FindElement(ovalResultsElem, "results");
	DOMElement* systemElem = XmlCommon::FindElement(resultsElem, "system");
	DOMElement* systemCharElem = XmlCommon::FindElement(systemElem, "oval_system_characteristics");
	Directive::BuildReferences(XmlCommon::FindElement(ovalElem, "variables"), &references, Directive::ELEMENT_VARIABLE, "id");
	Directive::BuildReferences(XmlCommon::FindElement(ovalElem, "objects"), &references, Directive::ELEMENT_OBJECT, "id");
	Directive::BuildReferences(XmlCommon::FindElement(ovalElem, "tests"), &references, Directive::ELEMENT_TEST, "id");
	Directive::BuildReferences(XmlCommon::FindElement(ovalElem, "states"), &references, Directive::ELEMENT_STATE, "id");
	Directive::BuildReferences(XmlCommon::FindElement(systemElem, "definitions"), &references, Directive::ELEMENT_DEFINITION, "definition_id");
	Directive::BuildReferences(XmlCommon::FindElement(systemElem, "tests"), &references, Directive::ELEMENT_TEST, "test_id");
	Directive::BuildReferences(XmlCommon::FindElement(systemCharElem, "collected_objects"), &references, Directive::ELEMENT_OBJECT, "id");
	//Directive::BuildReferences(XmlCommon::FindElement(systemCharElem, "system_data"), &references, Directive::ELEMENT_ITEM, "id");

	// Build a map of each definition and its class
	map<string, OvalEnum::ClassEnumeration> definitionClass;
	DOMElement* ovalDefinitionsElem = XmlCommon::FindElement(ovalElem, "definitions");
	DOMNodeList* ovalDefinitionNodes = ovalDefinitionsElem->getChildNodes();
	for (unsigned int i = 0; i < ovalDefinitionNodes->getLength(); i++) {
		DOMNode* node = ovalDefinitionNodes->item(i);

		if (node->getNodeType() == DOMNode::ELEMENT_NODE) {
			string defId = XmlCommon::GetAttributeByName((DOMElement*)node, "id");
			OvalEnum::ClassEnumeration defClass = OvalEnum::ToClass(XmlCommon::GetAttributeByName((DOMElement*)node, "class"));
			definitionClass[defId] = defClass;
		}
	}

	// Loop through all definitions and mark them (and any referenced definitions) with the
	// appropriate ResultContent value.
	map<string, OvalEnum::ResultContent> contentType;
	DOMElement* systemDefinitionsElem = XmlCommon::FindElement(systemElem, "definitions");
	DOMNodeList* definitionNodes = systemDefinitionsElem->getChildNodes();
	for (unsigned int i = 0; i < definitionNodes->getLength(); i++) {
		DOMNode* node = definitionNodes->item(i);

		if (node->getNodeType() == DOMNode::ELEMENT_NODE) {
			// Gather necessary information about this definition
			string defId = XmlCommon::GetAttributeByName((DOMElement*)node, "definition_id");
			string strResult = XmlCommon::GetAttributeByName((DOMElement*)node, "result");
			OvalEnum::ResultEnumeration result = OvalEnum::ToResult(strResult);

			Directive directive = Directive::GetDirective(OvalEnum::CLASS_DEFAULT, result);
			if (directives.find(definitionClass[defId]) != directives.end()) {
				directive = Directive::GetDirective(definitionClass[defId], result);
			}

			if (directive.GetReported()) {
				OvalEnum::ResultContent content = directive.GetResultContent();
				contentType[defId] = OvalEnum::CombineResultContent(contentType[defId], content);

				// Because thin results don't contain any referenced content, don't 
				// mark referenced definitions for thin results.
				if (content != OvalEnum::RESULT_CONTENT_THIN) {
					// Build a list of the elements that are related to this definition and update their
					// content types.
					Directive::WalkReferences(defId, references, &contentType, content);
				}
			}
		}
	}
	
	// Remove definitions that are not to be reported.  Remove criteria from thin definitions.
	// Use reverse loop because elements are being removed
	for (int i = definitionNodes->getLength() - 1; i >= 0 ; i--) {
		DOMNode* node = definitionNodes->item(i);

		if (node->getNodeType() == DOMNode::ELEMENT_NODE) {
			string defId = XmlCommon::GetAttributeByName((DOMElement*)node, "definition_id");
			if (contentType.find(defId) == contentType.end()) {
				systemDefinitionsElem->removeChild(node);
				node->release();
			} else if (contentType[defId] == OvalEnum::RESULT_CONTENT_THIN) {
				DOMNode* criteriaNode = XmlCommon::FindElement((DOMElement*)node, "criteria");
				node->removeChild(criteriaNode);
				criteriaNode->release();
			}
		}
	}
	

	// Delete all elements that are not desired in the final document
	// Clean up any elements that now no longer have children
	DOMElement* testsElem = XmlCommon::FindElement(systemElem, "tests");
	if (testsElem) {
		Directive::RemoveUnwanted(testsElem, "test_id", OvalEnum::RESULT_CONTENT_FULL, contentType);
		if (!XmlCommon::HasChildElements(testsElem)) {
			systemElem->removeChild(testsElem);
			testsElem->release();
		}
	}

	DOMElement* objectsElem = XmlCommon::FindElement(systemCharElem, "collected_objects");
	if (objectsElem) {
		Directive::RemoveUnwanted(objectsElem, "id", OvalEnum::RESULT_CONTENT_FULL, contentType);
		if (!XmlCommon::HasChildElements(objectsElem)) {
			systemCharElem->removeChild(objectsElem);
			objectsElem->release();
		}
	}

	DOMElement* dataElem = XmlCommon::FindElement(systemCharElem, "system_data");
	if (dataElem) {
		Directive::RemoveUnwanted(dataElem, "id", OvalEnum::RESULT_CONTENT_FULL, contentType);
		if (!XmlCommon::HasChildElements(dataElem)) {
			systemCharElem->removeChild(dataElem);
			dataElem->release();
		}
	}

	// Remove definitions if desired
	if (!includeSource) {
		ovalResultsElem->removeChild(ovalElem);
		ovalElem->release();
	}

	if (!XmlCommon::HasChildElements(systemDefinitionsElem)) {
		systemElem->removeChild(systemDefinitionsElem);
		systemDefinitionsElem->release();
	}
}

//
// Public members
//
Directive::Directive(OvalEnum::ResultEnumeration result, bool reported, OvalEnum::ResultContent content) {
	this->result = result;
	this->reported = reported;
	this->content = content;
}

OvalEnum::ResultEnumeration Directive::GetResult() {
	return this->result;
}

bool Directive::GetReported() {
	return this->reported;
}

OvalEnum::ResultContent Directive::GetResultContent() {
	return this->content;
}

void Directive::SetResult(OvalEnum::ResultEnumeration value) {
	this->result = value;
}

void Directive::SetReported(bool value) {
	this->reported = value;
}

void Directive::SetResultContent(OvalEnum::ResultContent value) {
	this->content = value;
}

//
// Private Members
//

void Directive::BuildReferences(DOMElement* parent, map<string,map<string, Directive::ElementType> >* references, Directive::ElementType type, string idAttr, string id) {
	if (parent == NULL) {
		return;
	}

	// Try to find the ID of this node if one was not provided.
	if (id == "")
		id = XmlCommon::GetAttributeByName(parent, idAttr);
	
	// Depending on the type of element that was passed in, determine the appropriate attributes to search
	map<string,Directive::ElementType> refnames;
	switch(type) {
		case Directive::ELEMENT_DEFINITION:
			refnames["definition_ref"] = Directive::ELEMENT_DEFINITION;
			refnames["test_ref"] = Directive::ELEMENT_TEST;
			break;
		case Directive::ELEMENT_TEST:
			refnames["state_ref"] = Directive::ELEMENT_STATE;
			refnames["object_ref"] = Directive::ELEMENT_OBJECT;
			break;
		case Directive::ELEMENT_OBJECT:
			refnames["item_ref"] = Directive::ELEMENT_ITEM;
			refnames["state_ref"] = Directive::ELEMENT_STATE;
			refnames["object_ref"] = Directive::ELEMENT_OBJECT;
			refnames["var_ref"] = Directive::ELEMENT_VARIABLE;
			//refnames["variable_id"] = Directive::ELEMENT_VARIABLE;
			break;
		case Directive::ELEMENT_VARIABLE:
			refnames["object_ref"] = Directive::ELEMENT_OBJECT;
			break;
		case Directive::ELEMENT_STATE:
			refnames["var_ref"] = Directive::ELEMENT_VARIABLE;
			break;
		case Directive::ELEMENT_ITEM:
			return;
	}

	// If an ID was found, try to find references.  If no ID was found, assume that this is a
	// container node and skip to searching the child elements
	if (id != "") {
		string ref;
		// Loop through and test the element for all reference types
		for (map<string,Directive::ElementType>::iterator it = refnames.begin(); it != refnames.end(); it++) {
			ref = XmlCommon::GetAttributeByName(parent, it->first);
			if (ref != "") {

				// Add the reference to the reference map
				(*references)[id][ref] = it->second;
			}
		}

		// Complex Object elements contain references in a data node rather than inside
		// attributes like every other element.
		if (type == Directive::ELEMENT_OBJECT) {
			if (XmlCommon::GetElementName(parent) == "object_reference") {
				string ref = XmlCommon::GetDataNodeValue(parent);
				if (ref != "") {
					(*references)[id][ref] = Directive::ELEMENT_OBJECT;
				}
			} else if (XmlCommon::GetElementName(parent) == "filter") {
				string ref = XmlCommon::GetDataNodeValue(parent);
				if (ref != "") {
					(*references)[id][ref] = Directive::ELEMENT_STATE;
				}
			}
		}
	}
	
	// Loop through any child nodes and try to find references
	DOMNodeList* list = parent->getChildNodes();
	for (unsigned int i = 0; i < list->getLength(); i++) {
		DOMNode* child = list->item(i);
		if (child->getNodeType() == DOMNode::ELEMENT_NODE) {
			Directive::BuildReferences((DOMElement*)child, references, type, idAttr, id);
		}
	}
}

void Directive::WalkReferences(string id, map<string,map<string, Directive::ElementType> > references, map<string, OvalEnum::ResultContent>* included, OvalEnum::ResultContent type) {
	map<string, Directive::ElementType>::iterator end = references[id].end();
	for (map<string, Directive::ElementType>::iterator it = references[id].begin(); it != end; it++) {
		string newId = it->first;
		// If this object was already included, don't include it again to avoid possability of 
		// an infinite loop
		OvalEnum::ResultContent currentValue = (*included)[newId];
		(*included)[newId] = OvalEnum::CombineResultContent(currentValue, type);
		
		// If the type has changed, recurse.  Do not recurse if the element is an "item" or "state"
		// because those elements never contain references
		if (it->second != Directive::ELEMENT_ITEM && it->second != Directive::ELEMENT_STATE && (*included)[newId] != currentValue) {
			Directive::WalkReferences(newId, references, included, type);
		}
	}
}

void Directive::RemoveUnwanted(DOMElement* parent, string idAttr, OvalEnum::ResultContent wantedTypes, map<string, OvalEnum::ResultContent> included) {
	// Loop through any child nodes and try to find references
	DOMNodeList* list = parent->getChildNodes();
	// Loop backwards because elements are being removed
	for (int i = list->getLength() - 1; i >= 0; i--) {
		DOMNode* child = list->item(i);
		if (child->getNodeType() == DOMNode::ELEMENT_NODE) {
			string id = XmlCommon::GetAttributeByName((DOMElement*)child, idAttr);
			map<string, OvalEnum::ResultContent>::iterator it = included.find(id);
			// Remove elements that are not included in the map
			// Remove elements that do not match one of the types in the wantedTypes flag
			if (it == included.end() || !(it->second & wantedTypes)) {
				parent->removeChild(child);
				child->release();
			}
		}
	}
}
