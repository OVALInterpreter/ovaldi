//
//
//****************************************************************************************//
// Copyright (c) 2002-2009, The MITRE Corporation
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

#include "AbsEntity.h"

//****************************************************************************************//
//								AbsEntity Class											  //	
//****************************************************************************************//
AbsEntity::AbsEntity(string name, string value, OvalEnum::Datatype datatype, bool isObjectEntity, OvalEnum::Operation operation, AbsVariable* varRef, OvalEnum::Check varCheck, bool nil) {

	this->SetName(name);
	this->SetValue(value);
	this->SetDatatype(datatype);
	this->SetIsObjectEntity(isObjectEntity);
	this->SetOperation(operation);
	this->SetVarCheck(varCheck);
	this->SetVarRef(NULL);
	this->SetNil(nil);
}

AbsEntity::~AbsEntity() {

}

// ***************************************************************************************	//
//								 Public members												//
// ***************************************************************************************	//

string AbsEntity::GetName() {

	return this->name;
}

void AbsEntity::SetName(string name) {

	this->name = name;
}

string AbsEntity::GetValue() {

	return this->value;
}

void AbsEntity::SetValue(string value) {

	this->value = value;
}

OvalEnum::Datatype AbsEntity::GetDatatype() {

	return this->datatype;
}

void AbsEntity::SetDatatype(OvalEnum::Datatype datatype) {

	this->datatype = datatype;
}

bool AbsEntity::GetIsObjectEntity() {

	return this->isObjectEntity;
}

void AbsEntity::SetIsObjectEntity(bool isObjectEntity) {

	this->isObjectEntity = isObjectEntity;
}


OvalEnum::Check AbsEntity::GetVarCheck() {

	return this->varCheck;
}

void AbsEntity::SetVarCheck(OvalEnum::Check varCheck) {

	this->varCheck = varCheck;
}

bool AbsEntity::GetNil() {

	return this->nil;
}

void AbsEntity::SetNil(bool nil) {

	this->nil = nil;
}

OvalEnum::Operation AbsEntity::GetOperation() {

	return this->operation;
}

void AbsEntity::SetOperation(OvalEnum::Operation operation) {

	this->operation = operation;
}

AbsVariable* AbsEntity::GetVarRef() {

	return this->varRef;
}

void AbsEntity::SetVarRef(AbsVariable* varRef) {

	this->varRef = varRef;
}

VariableValueVector* AbsEntity::GetVariableValues() {

	VariableValueVector* varValues = new VariableValueVector();

	if(this->GetVarRef() != NULL) {
		AbsVariable* varRef = this->GetVarRef();

		// get the values of the variable
		VariableValueVector* values = varRef->GetValues();
		VariableValueVector::iterator varValueIt;
		for(varValueIt = values->begin(); varValueIt != values->end(); varValueIt ++) {
			varValues->push_back((*varValueIt));
		}

		// get any variable values that were used for determinihng the value of this variable
		values = varRef->GetVariableValues();
		for(varValueIt = values->begin(); varValueIt != values->end(); varValueIt ++) {
			varValues->push_back((*varValueIt));
		}
		// BUG - These can not currently be deleted. 
		// The code is not consistant here. In places a new vector is returned
		// in others a reference to a vector that is managed by other code is returned.
		//delete values;
		//values = NULL;
	}

	return varValues;
}

OvalEnum::ResultEnumeration AbsEntity::Analyze(ItemEntity* scElement) {

	OvalEnum::ResultEnumeration result = OvalEnum::RESULT_ERROR;
	try {

		// check the status on the sc element
		if(scElement->GetStatus() == OvalEnum::STATUS_ERROR) {
			return OvalEnum::RESULT_ERROR;
		} else if(scElement->GetStatus() == OvalEnum::STATUS_NOT_COLLECTED) {
			return OvalEnum::RESULT_ERROR;
		} else if(scElement->GetStatus() == OvalEnum::STATUS_DOES_NOT_EXIST) {
			return OvalEnum::RESULT_FALSE;
		}

		// based on data type call the appropriate comparison method
		if(this->GetVarRef() == NULL) {

			if(this->GetDatatype() == OvalEnum::DATATYPE_BINARY) {
				result = EntityComparator::CompareBinary(this->GetOperation(), this->GetValue(), scElement->GetValue());
			} else if(this->GetDatatype() == OvalEnum::DATATYPE_BOOLEAN) {
				result = EntityComparator::CompareBoolean(this->GetOperation(), this->GetValue(), scElement->GetValue());
			} else if(this->GetDatatype() == OvalEnum::DATATYPE_EVR_STRING) {
				result = EntityComparator::CompareEvrString(this->GetOperation(), this->GetValue(), scElement->GetValue());
			} else if(this->GetDatatype() == OvalEnum::DATATYPE_FLOAT) {
				result = EntityComparator::CompareFloat(this->GetOperation(), this->GetValue(), scElement->GetValue());
			} else if(this->GetDatatype() == OvalEnum::DATATYPE_INTEGER) {
				result = EntityComparator::CompareInteger(this->GetOperation(), this->GetValue(), scElement->GetValue());
			} else if(this->GetDatatype() == OvalEnum::DATATYPE_IOS_VERSION) {
				result = EntityComparator::CompareIosVersion(this->GetOperation(), this->GetValue(), scElement->GetValue());
			} else if(this->GetDatatype() == OvalEnum::DATATYPE_STRING) {
				result = EntityComparator::CompareString(this->GetOperation(), this->GetValue(), scElement->GetValue());
			} else if(this->GetDatatype() == OvalEnum::DATATYPE_VERSION) {
				result = EntityComparator::CompareVersion(this->GetOperation(), this->GetValue(), scElement->GetValue());
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
						tmp = EntityComparator::CompareBinary(this->GetOperation(), (*iterator)->GetValue(), scElement->GetValue());
					} else if(this->GetDatatype() == OvalEnum::DATATYPE_BOOLEAN) {
						tmp = EntityComparator::CompareBoolean(this->GetOperation(), (*iterator)->GetValue(), scElement->GetValue());
					} else if(this->GetDatatype() == OvalEnum::DATATYPE_EVR_STRING) {
						tmp = EntityComparator::CompareEvrString(this->GetOperation(), (*iterator)->GetValue(), scElement->GetValue());
					} else if(this->GetDatatype() == OvalEnum::DATATYPE_FLOAT) {
						tmp = EntityComparator::CompareFloat(this->GetOperation(), (*iterator)->GetValue(), scElement->GetValue());
					} else if(this->GetDatatype() == OvalEnum::DATATYPE_INTEGER) {
						tmp = EntityComparator::CompareInteger(this->GetOperation(), (*iterator)->GetValue(), scElement->GetValue());
					} else if(this->GetDatatype() == OvalEnum::DATATYPE_IOS_VERSION) {
						tmp = EntityComparator::CompareIosVersion(this->GetOperation(), (*iterator)->GetValue(), scElement->GetValue());
					} else if(this->GetDatatype() == OvalEnum::DATATYPE_STRING) {
						tmp = EntityComparator::CompareString(this->GetOperation(), (*iterator)->GetValue(), scElement->GetValue());
					} else if(this->GetDatatype() == OvalEnum::DATATYPE_VERSION) {
						tmp = EntityComparator::CompareVersion(this->GetOperation(), (*iterator)->GetValue(), scElement->GetValue());
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

//****************************************************************************************//
//							AbsEntityException Class									  //	
//****************************************************************************************//
AbsEntityException::AbsEntityException(string errMsgIn, int severity, Exception* ex) : Exception(errMsgIn, severity, ex) {

}

AbsEntityException::~AbsEntityException() {

}

