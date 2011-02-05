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

#include "Exception.h"

using namespace std;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Exception  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
Exception::Exception(string msgIn, int severityIn, Exception* ex) {
	// -----------------------------------------------------------------------
	//  ABSTRACT
	//
	//  A simple constructor that sets the error message and the severity of
	//	this exception to the specified values
	//
	// -----------------------------------------------------------------------

	this->SetSeverity(severityIn);
	this->SetErrorMessage(msgIn);
	this->SetCause(ex);

	// this results in too much logging.
	// Log::Debug(msgIn);
}

Exception::~Exception() {
	// -----------------------------------------------------------------------
	//  ABSTRACT
	//
	//  Do nothing for now
	//
	// -----------------------------------------------------------------------
}

Exception* Exception::GetCause()
{
	// -----------------------------------------------------------------------
	//  ABSTRACT
	//
	//  Retrun the cause this exception
	//
	// -----------------------------------------------------------------------
	return this->cause;
}

string Exception::GetErrorMessage()
{
	// -----------------------------------------------------------------------
	//  ABSTRACT
	//
	//  Retrun the error message for this exception
	//
	// -----------------------------------------------------------------------
	return this->errorMessage;
}

int Exception::GetSeverity()
{
	// -----------------------------------------------------------------------
	//  ABSTRACT
	//
	//  Return the severity of this exception
	//
	// -----------------------------------------------------------------------
	return this->severity;
}

void Exception::SetCause(Exception* ex)
{
	// -----------------------------------------------------------------------
	//  ABSTRACT
	//
	//  Set the error message for this exception
	//
	// -----------------------------------------------------------------------
	this->cause = ex;
}

void Exception::SetErrorMessage(string errorMessageIn)
{
	// -----------------------------------------------------------------------
	//  ABSTRACT
	//
	//  Set the error message for this exception
	//
	// -----------------------------------------------------------------------
	this->errorMessage = errorMessageIn;
}

void Exception::SetSeverity(int severityIn)
{
	// -----------------------------------------------------------------------
	//  ABSTRACT
	//
	//  Set the severity of the exception
	//
	// -----------------------------------------------------------------------
	this->severity = severityIn;
}
