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

#include "ExternalVariable.h"

using namespace std;

//****************************************************************************************//
//									ExternalVariable Class								  //	
//****************************************************************************************//

ExternalVariable::ExternalVariable(string id, string name, int version, OvalEnum::Datatype datatype, StringVector* msgs) : AbsVariable (id, name, version, datatype, msgs) {

}

ExternalVariable::~ExternalVariable() {

}

// ***************************************************************************************	//
//								 Public members												//
// ***************************************************************************************	//
void ExternalVariable::Parse(DOMElement* externalVariableElm) {

	this->SetId(XmlCommon::GetAttributeByName(externalVariableElm, "id"));
	this->SetDatatype(OvalEnum::ToDatatype(XmlCommon::GetAttributeByName(externalVariableElm, "datatype")));
	string versionStr = XmlCommon::GetAttributeByName(externalVariableElm, "version");
	int version = atoi(versionStr.c_str());
	this->SetVersion(version);

	// Get all the possible elements' values
	DOMNodeList *externalVariableElmChildren = externalVariableElm->getChildNodes();
	unsigned int index = 0;
	while(index < externalVariableElmChildren->getLength()) {
		DOMNode *tmpNode = externalVariableElmChildren->item(index);
		if (tmpNode->getNodeType() == DOMNode::ELEMENT_NODE) {
			DOMElement *childElm = (DOMElement*)tmpNode;
			string elmName = XmlCommon::GetElementName(childElm);
			
			// parse each possible element
			if(elmName.compare("possible_value") == 0) {
				PossibleValueType *possibleValueType = new PossibleValueType();
				possibleValueType->Parse(childElm);
				this->AppendPossibleValueType(possibleValueType);
			} else if (elmName.compare("possible_restriction") == 0) {
				PossibleRestrictionType *possibleRestrictionType = new PossibleRestrictionType();
				possibleRestrictionType->Parse(childElm);
				this->AppendPossibleRestrictionType(possibleRestrictionType);
			}
		}
		index ++;
	}

	// Finally call ComputeValue
	this->ComputeValue();
}

VariableValueVector* ExternalVariable::GetVariableValues() {

	VariableValueVector* values = new VariableValueVector();

	return values;
}

PossibleValueTypeVector* ExternalVariable::GetPossibleValueTypes() {
	return &this->possibleValueTypes;
}

void ExternalVariable::AppendPossibleValueType(PossibleValueType* pv) {
	this->possibleValueTypes.push_back(pv);
}

PossibleRestrictionTypeVector* ExternalVariable::GetPossibleRestrictionTypes() {
	return &this->possibleRestrictionTypes;
}

void ExternalVariable::AppendPossibleRestrictionType(PossibleRestrictionType* pr) {
	this->possibleRestrictionTypes.push_back(pr);
}

void ExternalVariable::ComputeValue() {

	// get the external variables file
	XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* externalVariableDoc = NULL;
	try {
		externalVariableDoc = DocumentManager::GetExternalVariableDocument();
	} catch(Exception ex) {
		this->SetFlag(OvalEnum::FLAG_ERROR);
		this->AppendMessage("Error parsing external variable file. " + ex.GetErrorMessage());
		return;
	} catch(...) {
		this->SetFlag(OvalEnum::FLAG_ERROR);
		this->AppendMessage("Unknown error parsing external variable file.");
        return;
	}

	// get the variable from the external variables file
	DOMElement* variableElm = XmlCommon::FindElementNS(externalVariableDoc, "variable", "id", this->GetId());
	if(variableElm == NULL) {
		this->SetFlag(OvalEnum::FLAG_ERROR);
		this->AppendMessage("Error unable to find external variable " + this->GetId() + " in external variable document.");
	} else {

		// validate the datatype and warn if not matching
		OvalEnum::Datatype externalDatatype = OvalEnum::ToDatatype(XmlCommon::GetAttributeByName(variableElm, "datatype"));
		if(this->GetDatatype() != externalDatatype) {
			string warningMessage = "Warning, the specified external variable (" + this->GetId() + ") in the supplied external variable document has the wrong datatype. The variable in the oval-definition document expected datatype=" + OvalEnum::DatatypeToString(this->GetDatatype()) + ". The external variable in the supplied variable document has datatype=" + OvalEnum::DatatypeToString(externalDatatype) + ".";
			this->AppendMessage(warningMessage);
			Log::Info(warningMessage);
		}


		// get each value from the variable element in the external variables document.
		DOMNodeList *variableElmChildren = variableElm->getChildNodes();
		unsigned int index = 0;
		while(index < variableElmChildren->getLength()) {
			DOMNode *tmpNode = variableElmChildren->item(index);
			if (tmpNode->getNodeType() == DOMNode::ELEMENT_NODE) {
				DOMElement *childElm = (DOMElement*)tmpNode;
				string elmName = XmlCommon::GetElementName(childElm);				
			
				// parse each value element
				if(elmName.compare("value") == 0) {
					string externalValue = XmlCommon::GetDataNodeValue(childElm);

					// validate the value
					if(this->ValidateValue(this->GetDatatype(), externalValue)) {

						// add the value to the set of values for this exteranl variable.
						VariableValue* varValue = new VariableValue(this->GetId(), externalValue);
						this->AppendVariableValue(varValue);

					} else {
						this->SetFlag(OvalEnum::FLAG_ERROR);
						this->AppendMessage("Error a value ('" + externalValue + "') of external variable " + this->GetId() + " does not match the possible values for the variable.");
						return;
					}
				}
			}
			index ++;
		}
		this->SetFlag(OvalEnum::FLAG_COMPLETE);
		
	}
}

bool ExternalVariable::ValidateValue(OvalEnum::Datatype datatype, string externalValue) {

	bool isValid = false;
	
	// loop through all possible_value elements - if any are true return true 
	PossibleValueTypeVector::iterator value;
	for(value = this->GetPossibleValueTypes()->begin(); value != this->GetPossibleValueTypes()->end(); value++) {
		isValid = (*value)->ValidateValue(datatype, externalValue);
		if(isValid) {
			break;
		}
	}

	if(!isValid) {
		// loop through all the possible_restriction elements - if any are true return true
		PossibleRestrictionTypeVector::iterator restriction;
		for(restriction = this->GetPossibleRestrictionTypes()->begin(); restriction != this->GetPossibleRestrictionTypes()->end(); restriction++) {
			isValid = (*restriction)->ValidateValue(datatype, externalValue);
			if(isValid) {
				break;
			}
		}
	}

	// if no possible_value or possible_restriction elements then the value is valid
	if(this->GetPossibleValueTypes()->size() == 0 && this->GetPossibleRestrictionTypes()->size() == 0 ) {
			isValid = true;
	}

	return isValid;
}

