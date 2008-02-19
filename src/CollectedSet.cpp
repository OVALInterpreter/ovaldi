//
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
	this->SetFlag(OvalEnum::FLAG_ERROR);
}

CollectedSet::~CollectedSet() {
}

// ***************************************************************************************	//
//								 Public members												//
// ***************************************************************************************	//
ItemVector* CollectedSet::GetItems() {
	return &this->items;
}

void CollectedSet::SetItems(ItemVector* items) {
	this->items = (*items);
}

OvalEnum::Flag CollectedSet::GetFlag() {
	return this->flag;
}

void CollectedSet::SetFlag(OvalEnum::Flag flag) {
	this->flag = flag;
}

VariableValueVector* CollectedSet::GetVariableValues() {
	return &this->variableValues;
}

void CollectedSet::SetVariableValues(VariableValueVector* variableValues) {
	this->variableValues = (*variableValues);
}

void CollectedSet::AppendVariableValue(VariableValue* variableValue) {
	this->variableValues.push_back(variableValue);
}

void CollectedSet::AppendVariableValues(VariableValueVector* vars) {
	VariableValueVector::iterator iterator;
	for(iterator = vars->begin(); iterator != vars->end(); iterator++) {
		VariableValue* var = (*iterator);
		this->variableValues.push_back(var);
	}
}

//****************************************************************************************//
//								CollectedSetException Class								  //	
//****************************************************************************************//
CollectedSetException::CollectedSetException(string errMsgIn, int severity, Exception* ex) : Exception(errMsgIn, severity, ex) {

}

CollectedSetException::~CollectedSetException() {

}
