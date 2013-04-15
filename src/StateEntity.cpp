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

#include "StateOrObjectFieldEntityValue.h"
#include "StringEntityValue.h"

#include "StateEntity.h"

using namespace std;

//****************************************************************************************//
//								StateEntity Class						     			  //	
//****************************************************************************************//
StateEntity::StateEntity(StateEntity* orig) : AbsEntity() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Copy constructor.
	//	Create a complete StateEntity object based on the specified obj.
	//
	// -----------------------------------------------------------------------

	this->SetDatatype(orig->GetDatatype());
	this->SetEntityCheck(orig->GetEntityCheck());
	this->SetName(orig->GetName());
	this->SetNil(orig->GetNil());
	this->SetOperation(orig->GetOperation());
	this->SetValue(orig->GetValue());
	this->SetVarCheck(orig->GetVarCheck());
	this->SetVarRef(orig->GetVarRef());
}

StateEntity::StateEntity(string name, string value, OvalEnum::Datatype datatype, OvalEnum::Operation operation, AbsVariable* varRef, OvalEnum::Check entityCheck, OvalEnum::Check varCheck, bool nil)
									: AbsEntity(name, value, datatype, operation, varRef, varCheck, nil) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Create a complete StateEntity object
	//
	// -----------------------------------------------------------------------

	this->SetEntityCheck(entityCheck);
}

StateEntity::~StateEntity() {

}

// ***************************************************************************************	//
//								 Public members												//
// ***************************************************************************************	//
OvalEnum::Check StateEntity::GetEntityCheck() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the entityCheck field's value
	//
	// -----------------------------------------------------------------------

	return this->entityCheck;
}

void StateEntity::SetEntityCheck(OvalEnum::Check entityCheck) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the entityCheck field's value
	//
	// -----------------------------------------------------------------------

	this->entityCheck = entityCheck;
}

void StateEntity::Parse(DOMElement* stateEntityElm) {

	this->SetName(XmlCommon::GetElementName(stateEntityElm));
	this->SetOperation(OvalEnum::ToOperation(XmlCommon::GetAttributeByName(stateEntityElm, "operation")));
	this->SetDatatype(OvalEnum::ToDatatype(XmlCommon::GetAttributeByName(stateEntityElm, "datatype")));
	this->SetEntityCheck(OvalEnum::ToCheck(XmlCommon::GetAttributeByName(stateEntityElm, "entity_check")));
	this->SetVarCheck(OvalEnum::ToCheck(XmlCommon::GetAttributeByName(stateEntityElm, "var_check")));

	// The datatype is not 'record' so we can just grab the string value in the element.
	if ( this->GetDatatype() != OvalEnum::DATATYPE_RECORD ){
		this->SetValue(XmlCommon::GetDataNodeValue(stateEntityElm));
	}else{
		// The datatype is 'record' so we need to loop over all of the elements (which are the fields).
		DOMNodeList *stateChildren = stateEntityElm->getChildNodes();
		unsigned int index = 0;
		AbsEntityValueVector parsedValues;
		while(index < stateChildren->getLength()) {
			DOMNode* tmpNode = stateChildren->item(index);

			if (tmpNode->getNodeType() == DOMNode::ELEMENT_NODE) {
				DOMElement* stateChild = (DOMElement*)tmpNode;

				//	get the name of the child
				string childName = XmlCommon::GetElementName(stateChild);
				if ( childName.compare("field") == 0 || childName.compare("oval-def:field") == 0){
					StateOrObjectFieldEntityValue* fev = new StateOrObjectFieldEntityValue();
					fev->Parse(stateChild);
					parsedValues.push_back(fev);
				}else{
					StringEntityValue* sev = new StringEntityValue();
					sev->Parse(stateChild);
					parsedValues.push_back(sev);
				}
			}
			index ++;
		}	
		this->SetValues(parsedValues);
	}
	// to support version 5.3 it is best to just look for the deprected check = none exist 
	// and report it in the log
	if(this->GetVarCheck() == OvalEnum::CHECK_NONE_EXIST) {
		this->SetVarCheck(OvalEnum::CHECK_NONE_SATISFY);
		Log::Info("DEPRECATED var_check value: The \'none exist\' CheckEnumeration value has been deprecated and will be removed with the next major version of the language. The OVAL Interpreter has mapped this value to \'none satisfy\'");
	}

	// to support version 5.3 it is best to just look for the deprected check = none exist 
	// and report it in the log
	if(this->GetEntityCheck() == OvalEnum::CHECK_NONE_EXIST) {
		this->SetEntityCheck(OvalEnum::CHECK_NONE_SATISFY);
		Log::Info("DEPRECATED entity_check value: The \'none exist\' CheckEnumeration value has been deprecated and will be removed with the next major version of the language. The OVAL Interpreter has mapped this value to \'none satisfy\'");
	}

	// get the nill attribute
	string nilAttr = XmlCommon::GetAttributeByName(stateEntityElm, "nil");
	if(nilAttr.compare("") == 0 || nilAttr.compare("false") == 0) {
		this->SetNil(false);
	} else {
		this->SetNil(true);
	}

	// get variable value if needed
	string varRefStr = XmlCommon::GetAttributeByName(stateEntityElm, "var_ref");
	if(varRefStr.compare("") != 0) {
		AbsVariable* var = VariableFactory::GetVariable(varRefStr);
		this->SetValue("");
		this->SetVarRef(var);
	} else {
		this->SetVarRef(NULL);
	}
}

