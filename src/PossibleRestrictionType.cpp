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

#include "PossibleRestrictionType.h"

using namespace std;
using namespace xercesc;

//****************************************************************************************//
//									PossibleType Class									  //	
//****************************************************************************************//

PossibleRestrictionType::PossibleRestrictionType() {

}

PossibleRestrictionType::~PossibleRestrictionType() {

}

// ***************************************************************************************	//
//								 Public members												//
// ***************************************************************************************	//
void PossibleRestrictionType::Parse(DOMElement* possibleRestrictionTypeElm) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Parse the provided PossibleRestrictionType element into a 
	//  PossibleRestrictionType object.
	//
	// -----------------------------------------------------------------------

	// get the hint
	this->SetHint(XmlCommon::GetAttributeByName(possibleRestrictionTypeElm, "hint"));

	// Get all the restriction elements' values
	DOMNodeList *possibleRestrictionTypeElmChildren = possibleRestrictionTypeElm->getChildNodes();
	unsigned int index = 0;
	while(index < possibleRestrictionTypeElmChildren->getLength()) {
		DOMNode *tmpNode = possibleRestrictionTypeElmChildren->item(index);
		if (tmpNode->getNodeType() == DOMNode::ELEMENT_NODE) {
			DOMElement *childElm = (DOMElement*)tmpNode;

			string elmName = XmlCommon::GetElementName(childElm);
			
			// parse each child element
			if(elmName.compare("restriction") == 0) {
				RestrictionType *restrictionType = new RestrictionType();
				restrictionType->Parse(childElm);
				this->AppendRestrictionType(restrictionType);
			}
		}
		index ++;
	}
}

void PossibleRestrictionType::SetHint(string hint) {
	this->hint = hint;
}

string PossibleRestrictionType::GetHint() {
	return this->hint;
}

RestrictionTypeVector* PossibleRestrictionType::GetRestrictionTypes() {
	return &this->restrictionTypes;
}

void PossibleRestrictionType::AppendRestrictionType(RestrictionType* rt) {
	this->restrictionTypes.push_back(rt);
}

bool PossibleRestrictionType::ValidateValue(OvalEnum::Datatype datatype, string externalValue) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Ensure that the specified value matches the criteria 
	//	specified by this possible_restriction element
	//	
	// ----------------------------------------------------------------------

	bool isValid = true;
	
	// loop through all restriction elements - if all are true return true 
	RestrictionTypeVector::iterator iterator;
	for(iterator = this->GetRestrictionTypes()->begin(); iterator != this->GetRestrictionTypes()->end(); iterator++) {
		isValid = (*iterator)->ValidateValue(datatype, externalValue);
		if(!isValid) {
			break;
		}
	}

	return isValid;
}
