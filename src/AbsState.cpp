//
//
//****************************************************************************************//
// Copyright (c) 2002-2014, The MITRE Corporation
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

#include <algorithm>
#include <iterator>
#include "AbsState.h"

using namespace std;

//****************************************************************************************//
//									AbsState Class										  //	
//****************************************************************************************//
AbsState::AbsState(OvalEnum::Operator myOperator, int version) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Create a complete State object
	//
	// -----------------------------------------------------------------------

	this->SetId("");
	this->SetName("");
	this->SetXmlns("");
	this->SetVersion(version);
	this->SetOperator(myOperator);
}

AbsState::AbsState(string id, string name, string xmlns, OvalEnum::Operator myOperator, int version) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Create a complete State object
	//
	// -----------------------------------------------------------------------

	this->SetId(id);
	this->SetName(name);
	this->SetXmlns(xmlns);
	this->SetVersion(version);
	this->SetOperator(myOperator);
}

AbsState::~AbsState() {
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
AbsEntityVector* AbsState::GetElements() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the elements field's value
	//
	// -----------------------------------------------------------------------

	return &this->elements;
}

void AbsState::SetElements(AbsEntityVector* elements) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the elements field's value
	//
	// -----------------------------------------------------------------------

	this->elements = (*elements);
}

string AbsState::GetId() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the id field's value
	//
	// -----------------------------------------------------------------------

	return this->id;
}

void AbsState::SetId(string id) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the id field's value
	//
	// -----------------------------------------------------------------------

	this->id = id;
}

string AbsState::GetName() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the name field's value
	//
	// -----------------------------------------------------------------------

	return this->name;
}

void AbsState::SetName(string name) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the name field's value
	//
	// -----------------------------------------------------------------------

	this->name = name;
}

OvalEnum::Operator AbsState::GetOperator() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Get the myOperator field's value
	//
	// -----------------------------------------------------------------------

	return this->myOperator;
}

void AbsState::SetOperator(OvalEnum::Operator ovalOperator) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the myOperator field's value
	//
	// -----------------------------------------------------------------------

	this->myOperator = ovalOperator;
}
string AbsState::GetXmlns() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the xmlns field's value
	//
	// -----------------------------------------------------------------------

	return this->xmlns;
}

int AbsState::GetVersion() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the version field's value
	//
	// -----------------------------------------------------------------------

	return this->version;
}

void AbsState::SetVersion(int version) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the version field's value
	//
	// -----------------------------------------------------------------------

	this->version = version;
}

void AbsState::SetXmlns(string xmlns) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the xmlns field's value
	//
	// -----------------------------------------------------------------------

	this->xmlns = xmlns;
}

void AbsState::AppendElement(AbsEntity* absEntity) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Add an element to the end of the elements vector
	//
	// -----------------------------------------------------------------------

	this->elements.push_back(absEntity);
}

VariableValueVector AbsState::GetVariableValues() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return a vector of variable values that were used for this state
	//
	// -----------------------------------------------------------------------

	VariableValueVector varValues;

	AbsEntityVector::iterator iterator;
	for(iterator = this->GetElements()->begin(); iterator != this->GetElements()->end(); iterator++) {
		AbsEntity* entity = (AbsEntity*)(*iterator);
		if(entity->GetVarRef() != NULL) {
			VariableValueVector values = entity->GetVariableValues();
			copy(values.begin(), values.end(), back_inserter(varValues));
		}
	}
	return varValues;
}

//****************************************************************************************//
//								StateException Class									  //	
//****************************************************************************************//
AbsStateException::AbsStateException(string errMsgIn, int severity, Exception* ex) : Exception(errMsgIn, severity, ex) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the error message and then set the severity to ERROR_FATAL. This is 
	//	done with the explicit call to the Exception class constructor that 
	//	takes a single string param.
	//
	// -----------------------------------------------------------------------

}

AbsStateException::~AbsStateException() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Do nothing for now
	//
	// -----------------------------------------------------------------------

}
