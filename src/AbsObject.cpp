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

#include "AbsObject.h"

using namespace std;

//****************************************************************************************//
//									AbsObject Class										  //	
//****************************************************************************************//

AbsObject::AbsObject(string id, string comment, string xmlns, string name, int version) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Create a complete Entity object
	//
	// -----------------------------------------------------------------------

	this->SetId(id);
	this->SetComment(comment);
	this->SetXmlns(xmlns);
	this->SetName(name);
	this->SetVersion(version);
}

AbsObject::~AbsObject() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	clean up all the elements
	//
	// -----------------------------------------------------------------------
}

// ***************************************************************************************	//
//								 Public members												//
// ***************************************************************************************	//

string AbsObject::GetId() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the id field's value
	//
	// -----------------------------------------------------------------------

	return this->id;
}

void AbsObject::SetId(string id) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the id field's value
	//
	// -----------------------------------------------------------------------

	this->id = id;
}

string AbsObject::GetComment() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the comment field's value
	//
	// -----------------------------------------------------------------------

	return this->comment;
}

void AbsObject::SetComment(string comment) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the comment field's value
	//
	// -----------------------------------------------------------------------

	this->comment = comment;
}

string AbsObject::GetName() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the name field's value
	//
	// -----------------------------------------------------------------------

	return this->name;
}

void AbsObject::SetName(string name) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the name field's value
	//
	// -----------------------------------------------------------------------

	this->name = name;
}

int AbsObject::GetVersion() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the version field's value
	//
	// -----------------------------------------------------------------------

	return this->version;
}

void AbsObject::SetVersion(int version) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the version field's value
	//
	// -----------------------------------------------------------------------

	this->version = version;
}

string AbsObject::GetXmlns() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the xmlns field's value
	//
	// -----------------------------------------------------------------------

	return this->xmlns;
}

string AbsObject::GetType() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the type field's value
	//
	// -----------------------------------------------------------------------

	return this->type;
}

void AbsObject::SetXmlns(string xmlns) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the xmlns field's value
	//
	// -----------------------------------------------------------------------

	this->xmlns = xmlns;
}

//****************************************************************************************//
//								AbsObjectException Class								  //	
//****************************************************************************************//
AbsObjectException::AbsObjectException(string errMsgIn, int severity, Exception* ex) : Exception(errMsgIn, severity, ex) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the error message and then set the severity to ERROR_FATAL. This is 
	//	done with the explicit call to the Exception class constructor that 
	//	takes a single string param.
	//
	// -----------------------------------------------------------------------

}

AbsObjectException::~AbsObjectException() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Do nothing for now
	//
	// -----------------------------------------------------------------------

}
