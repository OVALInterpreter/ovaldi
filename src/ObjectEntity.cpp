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

#include "ObjectEntity.h"

using namespace std;

//****************************************************************************************//
//								ObjectEntity Class										  //	
//****************************************************************************************//
ObjectEntity::ObjectEntity(ObjectEntity* orig) : AbsEntity() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Copy constructor.
	//	Create a complete ObjectEntity object based on the specified obj.
	//
	// -----------------------------------------------------------------------

	this->SetDatatype(orig->GetDatatype());
	this->SetIsObjectEntity(true);
	this->SetName(orig->GetName());
	this->SetNil(orig->GetNil());
	this->SetOperation(orig->GetOperation());
	this->SetValue(orig->GetValue());
	this->SetVarCheck(orig->GetVarCheck());
	this->SetVarRef(orig->GetVarRef());
}

ObjectEntity::ObjectEntity(string name, string value, OvalEnum::Datatype datatype, OvalEnum::Operation operation, AbsVariable* varRef, OvalEnum::Check varCheck, bool nil)
									: AbsEntity(name, value, datatype, true, operation, varRef, varCheck, nil) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Create a complete ObjectEntity object
	//
	// -----------------------------------------------------------------------
}

ObjectEntity::~ObjectEntity() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Do nothing for now
	//
	// -----------------------------------------------------------------------
}

// ***************************************************************************************	//
//								 Public members												//
// ***************************************************************************************	//
bool ObjectEntity::Equals(AbsEntity* entity) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return true if this ObjectEntity is equal to the provided ObjectEntity
	// -----------------------------------------------------------------------
	ObjectEntity* objectEntity = (ObjectEntity*)entity;
	bool isEqual = false;

	if(this->GetDatatype() == objectEntity->GetDatatype()) {
		if(this->GetOperation() == objectEntity->GetOperation()) {
			if(this->GetName().compare(objectEntity->GetName()) == 0) {
				if(this->GetValue().compare(objectEntity->GetValue()) == 0) {
					if(this->GetVarCheck() == objectEntity->GetVarCheck()) {
						isEqual = true;
					}
				}
			}
		}
	}

	return isEqual;
}

void ObjectEntity::Parse(DOMElement* ObjectEntityElm) {

	this->SetName(XmlCommon::GetElementName(ObjectEntityElm));
	this->SetValue(XmlCommon::GetDataNodeValue(ObjectEntityElm));
	this->SetOperation(OvalEnum::ToOperation(XmlCommon::GetAttributeByName(ObjectEntityElm, "operation")));
	this->SetDatatype(OvalEnum::ToDatatype(XmlCommon::GetAttributeByName(ObjectEntityElm, "datatype")));
	this->SetVarCheck(OvalEnum::ToCheck(XmlCommon::GetAttributeByName(ObjectEntityElm, "var_check")));

	// to support version 5.3 it is best to just look for the deprected check = none exist 
	// and report it in the log
	if(this->GetVarCheck() == OvalEnum::CHECK_NONE_EXIST) {
		this->SetVarCheck(OvalEnum::CHECK_NONE_SATISFY);
		Log::Info("DEPRECATED var_check value: The \'none exist\' CheckEnumeration value has been deprecated and will be removed with the next major version of the language. The OVAL Interpreter has mapped this value to \'none satisfy\'");
	}

	// get the nill attribute
	string nilAttr = XmlCommon::GetAttributeByName(ObjectEntityElm, "xsi:nil");
	if(nilAttr.compare("") == 0 || nilAttr.compare("false") == 0) {
		this->SetNil(false);
	} else {
		this->SetNil(true);
	}

	// get variable value if needed
	string varRefStr = XmlCommon::GetAttributeByName(ObjectEntityElm, "var_ref");
	if(varRefStr.compare("") != 0) {
		//Log::Debug("ObjectEntity::Parse() - Found var_ref on object entity var id: " + varRefStr);
		AbsVariable* var = VariableFactory::GetVariable(varRefStr);
		this->SetValue("");
		this->SetVarRef(var);
	} else {
		this->SetVarRef(NULL);
	}
}
