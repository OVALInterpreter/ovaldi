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

#include "RestrictionType.h"

//****************************************************************************************//
//									RestrictionType Class								  //	
//****************************************************************************************//

RestrictionType::RestrictionType(OvalEnum::Datatype datatype, string value) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Create a complete RestrictionType
	//
	// -----------------------------------------------------------------------
	this->datatype = datatype;
	this->value = value;
}

RestrictionType::RestrictionType() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	// -----------------------------------------------------------------------

}

RestrictionType::~RestrictionType() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	// -----------------------------------------------------------------------

}

// ***************************************************************************************	//
//								 Public members												//
// ***************************************************************************************	//
void RestrictionType::Parse(DOMElement* restrictionElm) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Parse the provided RestrictionType element into a RestrictionType
	//
	// -----------------------------------------------------------------------

	this->SetOperation(OvalEnum::ToOperation(XmlCommon::GetAttributeByName(restrictionElm, "operation")));
	this->SetValue(XmlCommon::GetDataNodeValue(restrictionElm));
}

void RestrictionType::SetValue(string value) {
	this->value = value;
}

string RestrictionType::GetValue() {
	return this->value;
}

void RestrictionType::SetOperation(OvalEnum::Operation operation) {
	this->operation = operation;
}
OvalEnum::Operation RestrictionType::GetOperation() {
	return this->operation;
}

bool RestrictionType::ValidateValue(OvalEnum::Datatype datatype, string externalValue) {

	OvalEnum::ResultEnumeration result = OvalEnum::RESULT_ERROR;

	// do comparison based on datatype
	if(datatype == OvalEnum::DATATYPE_BINARY) {
		result = EntityComparator::CompareBinary(this->GetOperation(), this->GetValue(), externalValue);
	} else if(datatype == OvalEnum::DATATYPE_BOOLEAN) {
		result = EntityComparator::CompareBoolean(this->GetOperation(), this->GetValue(), externalValue);
	} else if(datatype == OvalEnum::DATATYPE_EVR_STRING) {
		result = EntityComparator::CompareEvrString(this->GetOperation(), this->GetValue(), externalValue);
	} else if(datatype == OvalEnum::DATATYPE_FLOAT) {
		result = EntityComparator::CompareFloat(this->GetOperation(), this->GetValue(), externalValue);
	} else if(datatype == OvalEnum::DATATYPE_INTEGER) {
		result = EntityComparator::CompareInteger(this->GetOperation(), this->GetValue(), externalValue);
	} else if(datatype == OvalEnum::DATATYPE_IOS_VERSION) {
		result = EntityComparator::CompareIosVersion(this->GetOperation(), this->GetValue(), externalValue);
	} else if(datatype == OvalEnum::DATATYPE_STRING) {
		result = EntityComparator::CompareString(this->GetOperation(), this->GetValue(), externalValue);
	} else if(datatype == OvalEnum::DATATYPE_VERSION) {
		result = EntityComparator::CompareVersion(this->GetOperation(), this->GetValue(), externalValue);
	}
    
	// convert the result to a boolean
	bool isValid = false;
	if(result == OvalEnum::RESULT_TRUE) {
		isValid = true;
	} else if(result == OvalEnum::RESULT_FALSE) {
		isValid = false;	
	} else {
		throw Exception("RestrictionType::ValidateValue method unable to convert result value to a boolean. Found result: " + OvalEnum::ResultToString(result));
	}

	return isValid;
}
