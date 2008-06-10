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

#include "Object.h"

//****************************************************************************************//
//									Object Class										  //	
//****************************************************************************************//

Object::Object(string id, string comment, string xmlns, string name, int version) : AbsObject (id, comment, xmlns, name, version) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Create a complete object
	//
	// -----------------------------------------------------------------------

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
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the behaviors field's value
	//
	// -----------------------------------------------------------------------

	return &this->behaviors;
}

void Object::SetBehaviors(BehaviorVector* behaviors) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the behaviors field's value
	//
	// -----------------------------------------------------------------------

	this->behaviors = (*behaviors);
}

AbsEntityVector* Object::GetElements() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the elements field's value
	//
	// -----------------------------------------------------------------------

	return &this->elements;
}

void Object::SetElements(AbsEntityVector* elements) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the elements field's value
	//
	// -----------------------------------------------------------------------

	this->elements = (*elements);
}

void Object::AppendElement(ObjectEntity* objectEntity) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Add an element to the end of the elements vector
	//
	// -----------------------------------------------------------------------

	this->elements.push_back(objectEntity);
}

ObjectEntity* Object::GetElementByName(string elementName) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the set of elements with the specified name
	//
	// -----------------------------------------------------------------------

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
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return a vector of variable values that were used for this object.
	//
	// -----------------------------------------------------------------------

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
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Parse the provided object element int an object
	//
	// -----------------------------------------------------------------------

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
