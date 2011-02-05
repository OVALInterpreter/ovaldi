//
//
//****************************************************************************************//
// Copyright (c) 2002-2011, The MITRE Corporation
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

#include "ComponentValue.h"

using namespace std;

//****************************************************************************************//
//								ComponentValue Class									  //	
//****************************************************************************************//
ComponentValue::ComponentValue(OvalEnum::Flag flag, StringVector* values, StringVector* msgs) {

	this->SetFlag(flag);
	this->SetValues(values);
	this->SetMessages(msgs);
}

ComponentValue::~ComponentValue() {

	delete values;
	values = NULL;
	delete msgs;
	msgs = NULL;

}

// ***************************************************************************************	//
//								 Public members												//
// ***************************************************************************************	//
OvalEnum::Flag ComponentValue::GetFlag() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the flag field's value
	//
	// -----------------------------------------------------------------------

	return this->flag;
}

void ComponentValue::SetFlag(OvalEnum::Flag flag) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the flag field's value
	//
	// -----------------------------------------------------------------------

	this->flag = flag;
}

StringVector* ComponentValue::GetValues() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the values field's value
	//
	// -----------------------------------------------------------------------

	return this->values;
}

void ComponentValue::SetValues(StringVector* values) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the values field's value
	//
	// -----------------------------------------------------------------------

	this->values = values;
}

void ComponentValue::AppendValue(string value) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Add a value to the end of the values vector
	//
	// -----------------------------------------------------------------------

	this->values->push_back(value);
}

StringVector* ComponentValue::GetMessages() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the msgs field's value
	//
	// -----------------------------------------------------------------------

	return this->msgs;
}

void ComponentValue::SetMessages(StringVector* msgs) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the msgs field's value
	//
	// -----------------------------------------------------------------------

	this->msgs = msgs;
}

void ComponentValue::AppendMessage(string msg) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Add a msg to the end of the msgs vector
	//
	// -----------------------------------------------------------------------

	this->msgs->push_back(msg);
}

void ComponentValue::AppendMessages(StringVector* newMsgs) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Add a newMsgs to the end of the msgs vector
	//
	// -----------------------------------------------------------------------

	StringVector::iterator iterator;
	for(iterator = newMsgs->begin(); iterator != newMsgs->end(); iterator++) {
		this->AppendMessage((*iterator));
	}
}


