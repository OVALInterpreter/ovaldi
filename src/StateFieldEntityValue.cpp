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

#include "StateFieldEntityValue.h"

using namespace std;

//****************************************************************************************//
//								StateFieldEntityValue Class								      //	
//****************************************************************************************//

StateFieldEntityValue::StateFieldEntityValue(string name, string value, OvalEnum::Datatype datatype, OvalEnum::Operation operation, AbsVariable* varRef, OvalEnum::Check entityCheck, OvalEnum::Check varCheck) : AbsEntityValue(value) {
	this->name = name;
	this->datatype = datatype;
	this->operation = operation;
	this->varRef = varRef;
	this->varCheck = varCheck;
	this->entityCheck = entityCheck;
}

StateFieldEntityValue::StateFieldEntityValue(const StateFieldEntityValue& stateFieldEntity){
	this->name = stateFieldEntity.name;
	this->value = stateFieldEntity.value;
	this->datatype = stateFieldEntity.datatype;
	this->operation = stateFieldEntity.operation;
	this->varRef = stateFieldEntity.varRef;
	this->varCheck = stateFieldEntity.varCheck;
	this->entityCheck = stateFieldEntity.entityCheck;
}

StateFieldEntityValue::~StateFieldEntityValue() {

}

// ***************************************************************************************	//
//								 Public members												//
// ***************************************************************************************	//
bool StateFieldEntityValue::Equals(AbsEntityValue* entityValue){
	StateFieldEntityValue* fieldEntityValue = (StateFieldEntityValue*)entityValue;
	bool isEqual = false;

	if(this->GetDatatype() == fieldEntityValue->GetDatatype()) {
		if(this->GetOperation() == fieldEntityValue->GetOperation()) {
			if(this->GetName().compare(fieldEntityValue->GetName()) == 0) {
				if(this->GetValue().compare(fieldEntityValue->GetValue()) == 0) {
					if(this->GetEntityCheck() == fieldEntityValue->GetEntityCheck()) {
						if(this->GetVarCheck() == fieldEntityValue->GetVarCheck()) {
							isEqual = true;
						}
					}
				}
			}
		}
	}
	return isEqual;
}

void StateFieldEntityValue::Parse(DOMElement* stateEntityElm) {

	this->SetName(XmlCommon::GetAttributeByName(stateEntityElm,"name"));
	this->SetValue(XmlCommon::GetDataNodeValue(stateEntityElm));
	this->SetOperation(OvalEnum::ToOperation(XmlCommon::GetAttributeByName(stateEntityElm, "operation")));
	this->SetDatatype(OvalEnum::ToDatatype(XmlCommon::GetAttributeByName(stateEntityElm, "datatype")));
	this->SetEntityCheck(OvalEnum::ToCheck(XmlCommon::GetAttributeByName(stateEntityElm, "entity_check")));
	this->SetVarCheck(OvalEnum::ToCheck(XmlCommon::GetAttributeByName(stateEntityElm, "var_check")));

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

string StateFieldEntityValue::GetName() {

	return this->name;
}

void StateFieldEntityValue::SetName(string name) {

	this->name = name;
}

OvalEnum::Datatype StateFieldEntityValue::GetDatatype() {

	return this->datatype;
}

void StateFieldEntityValue::SetDatatype(OvalEnum::Datatype datatype) {

	this->datatype = datatype;
}

OvalEnum::Operation StateFieldEntityValue::GetOperation() {

	return this->operation;
}

void StateFieldEntityValue::SetOperation(OvalEnum::Operation operation) {
	
	this->operation = operation;
}

OvalEnum::Check StateFieldEntityValue::GetEntityCheck() {

	return this->entityCheck;
}

void StateFieldEntityValue::SetEntityCheck(OvalEnum::Check entityCheck) {

	this->entityCheck = entityCheck;
}

AbsVariable* StateFieldEntityValue::GetVarRef() {

	return this->varRef;
}

void StateFieldEntityValue::SetVarRef(AbsVariable* varRef) {

	this->varRef = varRef;
}

OvalEnum::Check StateFieldEntityValue::GetVarCheck() {

	return this->varCheck;
}

void StateFieldEntityValue::SetVarCheck(OvalEnum::Check varCheck) {

	this->varCheck = varCheck;
}

OvalEnum::ResultEnumeration StateFieldEntityValue::Analyze(ItemFieldEntityValue* scField){

	OvalEnum::ResultEnumeration result = OvalEnum::RESULT_ERROR;
	
	try {
		
		// check the status on the sc element
		if(scField->GetStatus() == OvalEnum::STATUS_ERROR) {
			return OvalEnum::RESULT_ERROR;
		} else if(scField->GetStatus() == OvalEnum::STATUS_NOT_COLLECTED) {
			return OvalEnum::RESULT_ERROR;
		} else if(scField->GetStatus() == OvalEnum::STATUS_DOES_NOT_EXIST) {
			return OvalEnum::RESULT_FALSE;
		}
		
		// based on data type call the appropriate comparison method
		if(this->GetVarRef() == NULL) {

			if(this->GetDatatype() == OvalEnum::DATATYPE_BINARY) {
				result = EntityComparator::CompareBinary(this->GetOperation(), this->GetValue(), scField->GetValue());
			} else if(this->GetDatatype() == OvalEnum::DATATYPE_BOOLEAN) {
				result = EntityComparator::CompareBoolean(this->GetOperation(), this->GetValue(), scField->GetValue());
			} else if(this->GetDatatype() == OvalEnum::DATATYPE_EVR_STRING) {
				result = EntityComparator::CompareEvrString(this->GetOperation(), this->GetValue(), scField->GetValue());
			} else if(this->GetDatatype() == OvalEnum::DATATYPE_FLOAT) {
				result = EntityComparator::CompareFloat(this->GetOperation(), this->GetValue(), scField->GetValue());
			} else if(this->GetDatatype() == OvalEnum::DATATYPE_INTEGER) {
				result = EntityComparator::CompareInteger(this->GetOperation(), this->GetValue(), scField->GetValue());
			} else if(this->GetDatatype() == OvalEnum::DATATYPE_IOS_VERSION) {
				result = EntityComparator::CompareIosVersion(this->GetOperation(), this->GetValue(), scField->GetValue());
			} else if(this->GetDatatype() == OvalEnum::DATATYPE_STRING) {
				result = EntityComparator::CompareString(this->GetOperation(), this->GetValue(), scField->GetValue());
			} else if(this->GetDatatype() == OvalEnum::DATATYPE_VERSION) {
				result = EntityComparator::CompareVersion(this->GetOperation(), this->GetValue(), scField->GetValue());
			}

		} else {

			// loop over each variable value
			IntVector results;
			// check the flag on the variable
			if(this->GetVarRef()->GetFlag() == OvalEnum::FLAG_COMPLETE) { 
				VariableValueVector* values = this->GetVarRef()->GetValues();
				for(VariableValueVector::iterator iterator = values->begin(); iterator != values->end(); iterator++) {
					OvalEnum::ResultEnumeration tmp = OvalEnum::RESULT_ERROR;
					if(this->GetDatatype() == OvalEnum::DATATYPE_BINARY) {
						tmp = EntityComparator::CompareBinary(this->GetOperation(), (*iterator)->GetValue(), scField->GetValue());
					} else if(this->GetDatatype() == OvalEnum::DATATYPE_BOOLEAN) {
						tmp = EntityComparator::CompareBoolean(this->GetOperation(), (*iterator)->GetValue(), scField->GetValue());
					} else if(this->GetDatatype() == OvalEnum::DATATYPE_EVR_STRING) {
						tmp = EntityComparator::CompareEvrString(this->GetOperation(), (*iterator)->GetValue(), scField->GetValue());
					} else if(this->GetDatatype() == OvalEnum::DATATYPE_FLOAT) {
						tmp = EntityComparator::CompareFloat(this->GetOperation(), (*iterator)->GetValue(), scField->GetValue());
					} else if(this->GetDatatype() == OvalEnum::DATATYPE_INTEGER) {
						tmp = EntityComparator::CompareInteger(this->GetOperation(), (*iterator)->GetValue(), scField->GetValue());
					} else if(this->GetDatatype() == OvalEnum::DATATYPE_IOS_VERSION) {
						tmp = EntityComparator::CompareIosVersion(this->GetOperation(), (*iterator)->GetValue(), scField->GetValue());
					} else if(this->GetDatatype() == OvalEnum::DATATYPE_STRING) {
						tmp = EntityComparator::CompareString(this->GetOperation(), (*iterator)->GetValue(), scField->GetValue());
					} else if(this->GetDatatype() == OvalEnum::DATATYPE_VERSION) {
						tmp = EntityComparator::CompareVersion(this->GetOperation(), (*iterator)->GetValue(), scField->GetValue());
					}

					results.push_back(tmp);
				}
				result = OvalEnum::CombineResultsByCheck(&results, this->GetVarCheck());
			} else {
				
				result = OvalEnum::RESULT_ERROR;
			}
		}

	} catch (Exception ex) {
		result = OvalEnum::RESULT_ERROR;
		Log::Debug("An error occured while Analyzing an entity. " + ex.GetErrorMessage());
	} catch (...) {
		result = OvalEnum::RESULT_ERROR;
		Log::Debug("An unexpected error occured while Analyzing an entity.");
	}

	return result;
}
