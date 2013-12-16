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
#include <xercesc/dom/DOMElement.hpp>

#include "Common.h"
#include "DocumentManager.h"
#include "XmlCommon.h"

#include "ObjectReader.h"

using namespace std;
using namespace xercesc;

//****************************************************************************************//
//								ObjectReader Class										  //	
//****************************************************************************************//
OvalEnum::Flag ObjectReader::GetCollectedObjectFlag(string objectId) {

	OvalEnum::Flag flag = OvalEnum::FLAG_ERROR;

	DOMElement* collectedObjectsElm = XmlCommon::FindElement(DocumentManager::GetSystemCharacteristicsDocument(), "collected_objects");
	
	if(collectedObjectsElm != NULL) {
        DOMElement* objectElm = XmlCommon::FindElement(collectedObjectsElm, "object", "id", objectId);
		if(objectElm != NULL) {
			
			flag = OvalEnum::ToFlag(XmlCommon::GetAttributeByName(objectElm, "flag"));
			
		} else {
			throw Exception("Error: The specified object was not found in the provided System Characteristics file.");
		}
	} else {
		throw Exception("Error: Unable to locate collected_object element in provided System Characteristics file.");
	}
	return flag;
}

ItemVector* ObjectReader::GetItemsForObject(string objectId) {

	DOMElement* collectedObjectsElm = XmlCommon::FindElement(DocumentManager::GetSystemCharacteristicsDocument(), "collected_objects");
	
	ItemVector* items = new ItemVector();
	if(collectedObjectsElm != NULL) {
        DOMElement* objectElm = XmlCommon::FindElement(collectedObjectsElm, "object", "id", objectId);
		if(objectElm != NULL) {
			// Check the flag value. acceptable values are "complete" && "incomplete"
			OvalEnum::Flag flag = OvalEnum::ToFlag(XmlCommon::GetAttributeByName(objectElm, "flag"));
			if(flag == OvalEnum::FLAG_COMPLETE || flag == OvalEnum::FLAG_INCOMPLETE) {

				// loop through all references and get the corresponding item
				DOMNodeList *objectChildren = objectElm->getChildNodes();
				unsigned int index = 0;
				while(index < objectChildren->getLength()) {
					DOMNode* tmpNode = objectChildren->item(index);

					//	only concerned with ELEMENT_NODEs
					if (tmpNode->getNodeType() == DOMNode::ELEMENT_NODE) {
						DOMElement* objectChild = (DOMElement*)tmpNode;

						//	get the name of the child
						string childName = XmlCommon::GetElementName(objectChild);
						if(childName.compare("reference") == 0) {
							string itemRef = XmlCommon::GetAttributeByName(objectChild, "item_ref");
							Item* item = Item::GetItemById(itemRef);
							items->push_back(item);
						}
					}
					index ++;
				}

			} else {
				delete items;
				throw Exception("Error: The flag attribute value must be \'complete\'. Found: " + OvalEnum::FlagToString(flag));
			}
		} else {
			delete items;
			throw Exception("Error: The specified object was not found in the provided System Characteristics file.");
		}
	} else {
		delete items;
		throw Exception("Error: Unable to locate collected_object element in provided System Characteristics file.");
	}
	return items;
}

VariableValueVector* ObjectReader::GetVariableValuesForObject(string objectId) {

	DOMElement* collectedObjectsElm = XmlCommon::FindElement(DocumentManager::GetSystemCharacteristicsDocument(), "collected_objects");
	
	VariableValueVector* values = new VariableValueVector();
	if(collectedObjectsElm != NULL) {
        DOMElement* objectElm = XmlCommon::FindElement(collectedObjectsElm, "object", "id", objectId);
		if(objectElm != NULL) {
			// Check the flag value. only acceptable value is "complete"
			OvalEnum::Flag flag = OvalEnum::ToFlag(XmlCommon::GetAttributeByName(objectElm, "flag"));
			if(flag == OvalEnum::FLAG_COMPLETE) {

				// loop through all variable values
				DOMNodeList *objectChildren = objectElm->getChildNodes();
				unsigned int index = 0;
				while(index < objectChildren->getLength()) {
					DOMNode* tmpNode = objectChildren->item(index);

					//	only concerned with ELEMENT_NODEs
					if (tmpNode->getNodeType() == DOMNode::ELEMENT_NODE) {
						DOMElement* objectChild = (DOMElement*)tmpNode;

						//	get the name of the child
						string childName = XmlCommon::GetElementName(objectChild);
						if(childName.compare("variable_value") == 0) {
							string varId = XmlCommon::GetAttributeByName(objectChild, "variable_id");
							string varValue = XmlCommon::GetDataNodeValue(objectChild);
							VariableValue* var = new VariableValue(varId, varValue);
							values->push_back(var);
						}
					}
					index ++;
				}

			} else {
				delete values;
				throw Exception("Error: The flag attribute value must be \'complete\'. Found: " + OvalEnum::FlagToString(flag));
			}
		} else {
			delete values;
			throw Exception("Error: The specified object was not found in the provided System Characteristics document.");
		}
	} else {
		delete values;
		throw Exception("Error: Unable to locate collected_object element in provided System Characteristics document.");
	}
	return values;
}

StringVector* ObjectReader::GetMessagesForObject(string objectId){
	DOMElement* collectedObjectsElm = XmlCommon::FindElement(DocumentManager::GetSystemCharacteristicsDocument(), "collected_objects");
	
	StringVector* messages = new StringVector();
	if(collectedObjectsElm != NULL) {
        DOMElement* objectElm = XmlCommon::FindElement(collectedObjectsElm, "object", "id", objectId);
		if(objectElm != NULL) {

			// loop through all references and get the corresponding messages
			DOMNodeList *objectChildren = objectElm->getChildNodes();
			unsigned int index = 0;
			while(index < objectChildren->getLength()) {
				DOMNode* tmpNode = objectChildren->item(index);

				//	only concerned with ELEMENT_NODEs
				if (tmpNode->getNodeType() == DOMNode::ELEMENT_NODE) {
					DOMElement* objectChild = (DOMElement*)tmpNode;

					//	get the name of the child
					string childName = XmlCommon::GetElementName(objectChild);
					if(childName.compare("message") == 0) {
						messages->push_back(XmlCommon::GetDataNodeValue(objectChild));								
					}
				}
				index ++;
			}

		} else {
			delete messages;
			throw Exception("Error: The specified object was not found in the provided System Characteristics file.");
		}
	} else {
		delete messages;
		throw Exception("Error: Unable to locate collected_object element in provided System Characteristics file.");
	}
	return messages;	

}
