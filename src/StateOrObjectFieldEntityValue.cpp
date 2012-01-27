//
//
//****************************************************************************************//
// Copyright (c) 2002-2012, The MITRE Corporation
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
#include "VariableFactory.h"

using namespace std;
using namespace xercesc;

//****************************************************************************************//
//								StateOrObjectFieldEntityValue Class								      //	
//****************************************************************************************//

StateOrObjectFieldEntityValue::StateOrObjectFieldEntityValue(string name, string value, OvalEnum::Datatype datatype, OvalEnum::Operation operation, AbsVariable* varRef, OvalEnum::Check entityCheck, OvalEnum::Check varCheck) : AbsEntityValue(value) {
	this->name = name;
	this->datatype = datatype;
	this->operation = operation;
	this->varRef = varRef;
	this->varCheck = varCheck;
	this->entityCheck = entityCheck;
}

StateOrObjectFieldEntityValue::StateOrObjectFieldEntityValue(const StateOrObjectFieldEntityValue& otherFieldEntity){
	this->name = otherFieldEntity.name;
	this->value = otherFieldEntity.value;
	this->datatype = otherFieldEntity.datatype;
	this->operation = otherFieldEntity.operation;
	this->varRef = otherFieldEntity.varRef;
	this->varCheck = otherFieldEntity.varCheck;
	this->entityCheck = otherFieldEntity.entityCheck;
}

StateOrObjectFieldEntityValue::~StateOrObjectFieldEntityValue() {

}

// ***************************************************************************************	//
//								 Public members												//
// ***************************************************************************************	//
bool StateOrObjectFieldEntityValue::Equals(AbsEntityValue* entityValue){
	StateOrObjectFieldEntityValue* fieldEntityValue = (StateOrObjectFieldEntityValue*)entityValue;
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

void StateOrObjectFieldEntityValue::Parse(DOMElement* objectEntityElm) {

	this->SetName(XmlCommon::GetAttributeByName(objectEntityElm,"name"));
	this->SetValue(XmlCommon::GetDataNodeValue(objectEntityElm));
	this->SetOperation(OvalEnum::ToOperation(XmlCommon::GetAttributeByName(objectEntityElm, "operation")));
	this->SetDatatype(OvalEnum::ToDatatype(XmlCommon::GetAttributeByName(objectEntityElm, "datatype")));
	this->SetEntityCheck(OvalEnum::ToCheck(XmlCommon::GetAttributeByName(objectEntityElm, "entity_check")));
	this->SetVarCheck(OvalEnum::ToCheck(XmlCommon::GetAttributeByName(objectEntityElm, "var_check")));

	// to support version 5.3 it is best to just look for the deprecated check = none exist 
	// and report it in the log
	if(this->GetVarCheck() == OvalEnum::CHECK_NONE_EXIST) {
		this->SetVarCheck(OvalEnum::CHECK_NONE_SATISFY);
		Log::Info("DEPRECATED var_check value: The \'none exist\' CheckEnumeration value has been deprecated and will be removed with the next major version of the language. The OVAL Interpreter has mapped this value to \'none satisfy\'");
	}

	// to support version 5.3 it is best to just look for the deprecated check = none exist 
	// and report it in the log
	if(this->GetEntityCheck() == OvalEnum::CHECK_NONE_EXIST) {
		this->SetEntityCheck(OvalEnum::CHECK_NONE_SATISFY);
		Log::Info("DEPRECATED entity_check value: The \'none exist\' CheckEnumeration value has been deprecated and will be removed with the next major version of the language. The OVAL Interpreter has mapped this value to \'none satisfy\'");
	}

	// get variable value if needed
	string varRefStr = XmlCommon::GetAttributeByName(objectEntityElm, "var_ref");
	if(varRefStr.compare("") != 0) {
		AbsVariable* var = VariableFactory::GetVariable(varRefStr);
		this->SetValue("");
		this->SetVarRef(var);
	} else {
		this->SetVarRef(NULL);
	}
}

string StateOrObjectFieldEntityValue::GetName() {

	return this->name;
}

void StateOrObjectFieldEntityValue::SetName(string name) {

	this->name = name;
}

OvalEnum::Datatype StateOrObjectFieldEntityValue::GetDatatype() {

	return this->datatype;
}

void StateOrObjectFieldEntityValue::SetDatatype(OvalEnum::Datatype datatype) {

	this->datatype = datatype;
}

OvalEnum::Operation StateOrObjectFieldEntityValue::GetOperation() {

	return this->operation;
}

void StateOrObjectFieldEntityValue::SetOperation(OvalEnum::Operation operation) {
	
	this->operation = operation;
}

OvalEnum::Check StateOrObjectFieldEntityValue::GetEntityCheck() {

	return this->entityCheck;
}

void StateOrObjectFieldEntityValue::SetEntityCheck(OvalEnum::Check entityCheck) {

	this->entityCheck = entityCheck;
}

AbsVariable* StateOrObjectFieldEntityValue::GetVarRef() {

	return this->varRef;
}

void StateOrObjectFieldEntityValue::SetVarRef(AbsVariable* varRef) {

	this->varRef = varRef;
}

OvalEnum::Check StateOrObjectFieldEntityValue::GetVarCheck() {

	return this->varCheck;
}

void StateOrObjectFieldEntityValue::SetVarCheck(OvalEnum::Check varCheck) {

	this->varCheck = varCheck;
}

OvalEnum::ResultEnumeration StateOrObjectFieldEntityValue::Analyze(ItemFieldEntityValue* scField){

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
			} else if(this->GetDatatype() == OvalEnum::DATATYPE_IPV4_ADDRESS) {
				result = EntityComparator::CompareIpv4Address(this->GetOperation(), this->GetValue(), scField->GetValue());
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
					} else if(this->GetDatatype() == OvalEnum::DATATYPE_IPV4_ADDRESS) {
						tmp = EntityComparator::CompareIpv4Address(this->GetOperation(), (*iterator)->GetValue(), scField->GetValue());
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
