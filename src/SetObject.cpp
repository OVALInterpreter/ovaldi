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

#include "Common.h"
#include "ObjectFactory.h"
#include "AbsObject.h"
#include "State.h"
#include "Object.h"
#include "XmlCommon.h"

#include "SetObject.h"

using namespace std;
using namespace xercesc;

//****************************************************************************************//
//									SetObject Class										  //	
//****************************************************************************************//
SetObject::SetObject(string id, string comment, string name, int version, string xmlns)  : AbsObject (id, comment, xmlns, name, version) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Create a complete SetObject
	//
	// -----------------------------------------------------------------------

	this->type = "SetObject";

}

SetObject::~SetObject() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	clean up all the elements
	//
	// -----------------------------------------------------------------------
	
	delete(this->set);
}

// ***************************************************************************************	//
//								 Public members												//
// ***************************************************************************************	//
Set* SetObject::GetSet() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the set field's value
	//
	// -----------------------------------------------------------------------

	return this->set;
}

void SetObject::SetSet(Set* set) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the set field's value
	//
	// -----------------------------------------------------------------------

	this->set = set;
}

VariableValueVector* SetObject::GetVariableValues() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return a vector of all variable values used in this SetObject
	//
	// -----------------------------------------------------------------------
	
	VariableValueVector* varValues = NULL;

	// This call is ok because the Set creates a new VariableValueVector 
	// and returns it to the caller 
	varValues = this->GetSet()->GetVariableValues();

	return varValues;
}

void SetObject::Parse(DOMElement* setObjectElm) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Parse the provided Set object element
	//
	// -----------------------------------------------------------------------

	this->SetName(XmlCommon::GetElementName(setObjectElm));
	this->SetId(XmlCommon::GetAttributeByName(setObjectElm, "id"));
	this->SetComment(XmlCommon::GetAttributeByName(setObjectElm, "comment"));
	this->SetXmlns(XmlCommon::GetNamespace(setObjectElm));
	string versionStr = XmlCommon::GetAttributeByName(setObjectElm, "version");
	int version = 0;
	if(versionStr.empty()) {
		version = 1;
	} else {
		Common::FromString(versionStr, &version);
	}
	this->SetVersion(version);

	// loop over all elements
	DOMNodeList *setObjectChildren = setObjectElm->getChildNodes();
	unsigned int index = 0;
	while(index < setObjectChildren->getLength()) {
		DOMNode *tmpNode = setObjectChildren->item(index++);

		//	only concerned with ELEMENT_NODEs
		if (tmpNode->getNodeType() == DOMNode::ELEMENT_NODE) {
			DOMElement *setObjectChild = (DOMElement*)tmpNode;

			//	get the name of the child
			string childName = XmlCommon::GetElementName(setObjectChild);
			if(childName.compare("notes") == 0) { // ignore notes
				continue;

			// look for set object
			} else if(childName.compare("set") == 0) {
				// create a new set object based on this element
				Set* newSetObj = new Set(setObjectChild);
				this->SetSet(newSetObj);
				break;
			} 
		}
	}
}
