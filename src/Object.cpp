//
//
//****************************************************************************************//
// Copyright (c) 2002-2010, The MITRE Corporation
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

#include "Object.h"

using namespace std;

ObjectMap Object::objectCache;

//****************************************************************************************//
//									Object Class										  //	
//****************************************************************************************//

Object::Object(string id, string comment, string xmlns, string name, int version) : AbsObject (id, comment, xmlns, name, version) {

	this->type = "Object";
}

Object::~Object() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	clean up all the elements
	//
	// -----------------------------------------------------------------------
	
	AbsEntity* currentElement = NULL;
	while(elements.size() != 0) {
	  	currentElement = elements[elements.size()-1];
	  	elements.pop_back();
	  	delete currentElement;
	  	currentElement = NULL;
	}
}

// ***************************************************************************************	//
//								 Public members												//
// ***************************************************************************************	//
BehaviorVector* Object::GetBehaviors() {

	return &this->behaviors;
}

void Object::SetBehaviors(BehaviorVector* behaviors) {

	this->behaviors = (*behaviors);
}

AbsEntityVector* Object::GetElements() {

	return &this->elements;
}

void Object::SetElements(AbsEntityVector* elements) {

	this->elements = (*elements);
}

void Object::AppendElement(ObjectEntity* objectEntity) {

	this->elements.push_back(objectEntity);
}

ObjectEntity* Object::GetElementByName(string elementName) {

	ObjectEntity* matchingElm = NULL;

	AbsEntityVector::iterator iterator;
	for(iterator = this->GetElements()->begin(); iterator != this->GetElements()->end(); iterator++) {
		AbsEntity* element = (*iterator);
		if(element->GetName().compare(elementName) == 0) {
			matchingElm = (ObjectEntity*)element;   
		}
	}

	return matchingElm;
}

VariableValueVector* Object::GetVariableValues() {

	VariableValueVector* varValues = new VariableValueVector();

	// get the variable values used on each element
	AbsEntityVector::iterator iterator;
	for(iterator = this->GetElements()->begin(); iterator != this->GetElements()->end(); iterator++) {
		ObjectEntity* entity = (ObjectEntity*)(*iterator);
		VariableValueVector* values = entity->GetVariableValues();
		VariableValueVector::iterator varValueIt;
		for(varValueIt = values->begin(); varValueIt != values->end(); varValueIt ++) {
			VariableValue* var = (*varValueIt);
			varValues->push_back(var);
		}

		delete values;
		values = NULL;
	}

	return varValues;
}

void Object::Parse(DOMElement* objectElm) {

	this->SetName(XmlCommon::GetElementName(objectElm));
	this->SetId(XmlCommon::GetAttributeByName(objectElm, "id"));
	this->SetComment(XmlCommon::GetAttributeByName(objectElm, "comment"));
	this->SetXmlns(XmlCommon::GetNamespace(objectElm));
	string versionStr = XmlCommon::GetAttributeByName(objectElm, "version");
	int version;
	if(versionStr.compare("") == 0) {
		version = 1;
	} else {
		version = atoi(versionStr.c_str());
	}
	this->SetVersion(version);

	//Log::Debug("Object::Parse Parsing object id: " + this->GetId());

	// loop over all elements
	DOMNodeList *objectChildren = objectElm->getChildNodes();
	unsigned int index = 0;
	while(index < objectChildren->getLength()) {
		DOMNode *tmpNode = objectChildren->item(index++);

		//	only concerned with ELEMENT_NODEs
		if (tmpNode->getNodeType() == DOMNode::ELEMENT_NODE) {

			DOMElement *objectChild = (DOMElement*)tmpNode;

			//	get the name of the child
			string childName = XmlCommon::GetElementName(objectChild);
			if(childName.compare("notes") == 0)
				continue;

			if(childName.compare("behaviors") == 0) {
				this->SetBehaviors(Behavior::Parse(objectChild));
			} else {
                ObjectEntity* objectEntity = new ObjectEntity();
				objectEntity->Parse(objectChild);
				this->AppendElement(objectEntity);
			}
		}
	}
}

bool Object::Analyze(Item* item) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Analyze the specified Item return the Result value for the Item.
	//
	//	1 - create a vector of Item elements that match each element in the object.
	//	2 - pass the vector to the AbsEntity analyze method
	//	3 - build a vector of results for each element in the object.
	//	4 - combine the results to a single value by doing a logical and of the result values
    // -----------------------------------------------------------------------

	OvalEnum::ResultEnumeration overallResult = OvalEnum::RESULT_ERROR;

	// Check the status of the Item
	if(item->GetStatus() == OvalEnum::STATUS_ERROR) {
		overallResult = OvalEnum::RESULT_ERROR;
	} else if(item->GetStatus() == OvalEnum::STATUS_NOT_COLLECTED) {
		overallResult = OvalEnum::RESULT_ERROR;
	} else if(item->GetStatus() == OvalEnum::STATUS_DOES_NOT_EXIST) {
		overallResult = OvalEnum::RESULT_FALSE;
	} else {

		// check data before analysis
		if(this->GetElements()->size() == 0) {
			overallResult = OvalEnum::RESULT_TRUE;
		} else {

			overallResult = OvalEnum::RESULT_TRUE;

			// vector of result values before they are combined
			IntVector iResults;

			// Loop through all elements in the object
			AbsEntityVector::iterator iterator;
            for(iterator = this->GetElements()->begin(); iterator != this->GetElements()->end() && overallResult == OvalEnum::RESULT_TRUE; iterator++) {
				ObjectEntity* objectEntity = (ObjectEntity*)(*iterator);

				// locate matching elements in the item
				string objectElmName = objectEntity->GetName(); 
				ItemEntityVector* scElements = item->GetElementsByName(objectElmName);

                if(scElements->size() == 0)  {

                    // no matching elements found so not a matching item.
                    iResults.push_back(OvalEnum::RESULT_FALSE);

                } else if (scElements->size() == 1) {

                    // found one match now we need to analyze the element to see if it is a match based on datatype and operation.
                    ItemEntity* itemEntity = (ItemEntity*)scElements->at(0);    
                    OvalEnum::ResultEnumeration elmResult = objectEntity->Analyze(itemEntity);
                    iResults.push_back(elmResult);

                } else {

                    // an error if more than 1 match is found
                    iResults.push_back(OvalEnum::RESULT_ERROR);
                    string errMessage = "More than one match element was found for an object entity. Object Id: " + this->GetId();
					errMessage.append(" Item Id: " + Common::ToString(item->GetId()));
                    Log::Fatal(errMessage);
                }

                scElements->clear();
				delete scElements;
			}

			// compute the overall result
            overallResult = OvalEnum::CombineResultsByOperator(&iResults, OvalEnum::OPERATOR_AND);
		}
	}

	// want to convert the result to a boolean
	bool isMatch = false;
	if(overallResult == OvalEnum::RESULT_TRUE) {
		isMatch = true;
	} else if(overallResult == OvalEnum::RESULT_FALSE) {
		isMatch = false;	
	} else {
		throw AbsObjectException("Object::Analyze method unable to convert result value to a boolean. Found result: " + OvalEnum::ResultToString(overallResult));
	}
	return isMatch;
}

Object* Object::GetObjectById(string objectId) {

	Object* object = NULL;
	
	// Search the cache
	object = Object::SearchCache(objectId);

	// if not found try to parse it.
	if(object == NULL) {

        DOMElement* objectsElm = XmlCommon::FindElement(DocumentManager::GetDefinitionDocument(), "objects");
		DOMElement* objectElm = XmlCommon::FindElementByAttribute(objectsElm, "id", objectId);

		if(objectElm == NULL) {
			throw Exception("Unable to find specified object in oval-definitions document. Object id: " + objectId);
		}

		object = new Object();
		object->Parse(objectElm);
		Object::Cache(object);
	}
	
	return object;
}

// ***************************************************************************************	//
//								 Private members											//
// ***************************************************************************************	//

Object* Object::SearchCache(string id) {

	Object* cachedObject = NULL;

	ObjectMap::iterator iterator;
	iterator = Object::objectCache.find(id);
	if(iterator != Object::objectCache.end()) {
		cachedObject = iterator->second;
	} 

	return cachedObject;
}

void Object::ClearCache() {

	ObjectMap::iterator iterator;
	for(iterator = Object::objectCache.begin(); iterator != Object::objectCache.end(); iterator++) {
		
		Object* object = iterator->second;
		delete object;
	}
	
	Object::objectCache.clear();
}

void Object::Cache(Object* object) {

    Object::objectCache.insert(ObjectPair(object->GetId(), object));
}

