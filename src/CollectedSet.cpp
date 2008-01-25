//
// $Id: CollectedSet.cpp 4579 2008-01-02 17:39:07Z bakerj $
//
//****************************************************************************************//
// Copyright (c) 2002-2008, The MITRE Corporation
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

#include "CollectedSet.h"

//****************************************************************************************//
//									CollectedSet Class									  //	
//****************************************************************************************//

CollectedSet::CollectedSet() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Initialize the collected set
	//
	// -----------------------------------------------------------------------

	this->SetFlag(OvalEnum::FLAG_ERROR);
}

CollectedSet::CollectedSet(ItemVector* items, OvalEnum::Flag flag, VariableValueVector* variableValues) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Initialize the simple set flag to true
	//
	// -----------------------------------------------------------------------

	this->SetFlag(OvalEnum::FLAG_ERROR);
	this->SetItems(items);
	this->AppendVariableValues(variableValues);
}

CollectedSet::~CollectedSet() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	do nothing for now
	//
	// -----------------------------------------------------------------------
}

// ***************************************************************************************	//
//								 Public members												//
// ***************************************************************************************	//
ItemVector* CollectedSet::GetItems() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the items field's value
	//
	// -----------------------------------------------------------------------

	return &this->items;
}

void CollectedSet::SetItems(ItemVector* items) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the items field's value
	//
	// -----------------------------------------------------------------------

	this->items = (*items);
}

OvalEnum::Flag CollectedSet::GetFlag() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the flag field's value
	//
	// -----------------------------------------------------------------------

	return this->flag;
}

void CollectedSet::SetFlag(OvalEnum::Flag flag) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the flag field's value
	//
	// -----------------------------------------------------------------------

	this->flag = flag;
}

VariableValueVector* CollectedSet::GetVariableValues() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the variableValues field's value
	//
	// -----------------------------------------------------------------------

	return &this->variableValues;
}

void CollectedSet::SetVariableValues(VariableValueVector* variableValues) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the variableValues field's value
	//
	// -----------------------------------------------------------------------

	this->variableValues = (*variableValues);
}

void CollectedSet::AppendVariableValue(VariableValue* variableValue) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Add a varaible value to the end of the variable values vector
	//
	// -----------------------------------------------------------------------

	this->variableValues.push_back(variableValue);
}

void CollectedSet::AppendVariableValues(VariableValueVector* vars) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Add a varaible value to the end of the variable values vector
	//
	// -----------------------------------------------------------------------

	VariableValueVector::iterator iterator;
	for(iterator = vars->begin(); iterator != vars->end(); iterator++) {
		VariableValue* var = (*iterator);
		this->variableValues.push_back(var);
	}
}

//****************************************************************************************//
//								CollectedSetException Class										  //	
//****************************************************************************************//
CollectedSetException::CollectedSetException(string errMsgIn, int severity, Exception* ex) : Exception(errMsgIn, severity, ex) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	CollectedSet the error message and then set the severity to ERROR_FATAL. This is 
	//	done with the explicit call to the Exception class constructor that 
	//	takes a single string param.
	//
	// -----------------------------------------------------------------------

}

CollectedSetException::~CollectedSetException() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Do nothing for now
	//
	// -----------------------------------------------------------------------

}

