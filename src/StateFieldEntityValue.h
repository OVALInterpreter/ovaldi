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

#ifndef STATEFIELDENTITYVALUE_H
#define STATEFIELDENTITYVALUE_H

#include "AbsEntityValue.h"
#include "AbsVariable.h"
#include "VariableFactory.h"
XERCES_CPP_NAMESPACE_USE
using namespace std;

/**
	This class represents a field entity in an state entity that has a datatype of record as defined in the oval system characteristics schema.
*/
class StateFieldEntityValue : public AbsEntityValue {
public:

	/** Create a complete StateFieldEntityValue object. */
	StateFieldEntityValue(string name = "", string value = "", OvalEnum::Datatype datatype = OvalEnum::DATATYPE_STRING, OvalEnum::Operation operation = OvalEnum::OPERATION_EQUALS, AbsVariable* varRef = NULL, OvalEnum::Check entityCheck = OvalEnum::CHECK_ALL, OvalEnum::Check varCheck = OvalEnum::CHECK_ALL);
	
    /** StateFieldEntityValue copy constructor. */
    StateFieldEntityValue(const StateFieldEntityValue& StateFieldEntityValue);

    /** StateFieldEntityValue destructor. */
    ~StateFieldEntityValue();

	/** Return the name field's value. */
	string GetName();

	/** Set the name field's value. */
	void SetName(string name);

	/** Get the datatype field's value. */
	OvalEnum::Datatype GetDatatype();

	/** Set the datatype field's value. */
	void SetDatatype(OvalEnum::Datatype datatype);

	/** Determine if the specified ItemFieldEntityValue is equal to this ItemFieldEntityValue.
		@param entityValue the AbsEntityValue that you would like to see if the current ItemFieldEntityValue is equal to.
		@return a boolean value indicating whether or not the two ItemFieldEntityValues are equal.
	*/
	virtual bool Equals(AbsEntityValue* entityValue);

	/** Return the value of the operation attribute. */
	OvalEnum::Operation GetOperation();

	/** Set the value of the operation attribute. */
	void SetOperation(OvalEnum::Operation operation);

	/** Return the status field's value. */
	OvalEnum::SCStatus GetStatus();

	/** Set the scStatus field's value. */
	void SetStatus(OvalEnum::SCStatus status);

	/** Return the varCheck field's value */
	OvalEnum::Check GetEntityCheck();

	/** Set the varCheck field's value. */
	void SetEntityCheck(OvalEnum::Check check);
	
	/** Return the varCheck field's value */
	OvalEnum::Check GetVarCheck();

	/** Set the varCheck field's value. */
	void SetVarCheck(OvalEnum::Check check);

	/** Return the varRef field's value */
	AbsVariable* GetVarRef();
	
	/** Set the varRef field's value. */
	void SetVarRef(AbsVariable* varRef);

	/** Parse the specified entity to retrieve its field entity value.
	 *  @param entityElm A pointer to a DOMElement from which the field entity value should be retrieved.  This value should then be used to initialize the ItemFieldEntityValue. 
	 *  @return Void.
	 */
	virtual void Parse(DOMElement* entityElm);
	
	/** Analyze an ItemFieldEntityValue against this StateFieldEntityValue.
		@param scField the ItemFieldEntityValue which to analyze against this StateFieldEntityValue.
		@return A OvalEnum::ResultEnumeration indicating the results of the analysis.
	*/
	OvalEnum::ResultEnumeration Analyze(ItemFieldEntityValue* scField);

private:

	string name;
	OvalEnum::Datatype datatype;
	OvalEnum::Operation operation;
	AbsVariable* varRef;
	OvalEnum::Check varCheck;
	OvalEnum::Check entityCheck;
};

/**	
	A vector for storing EntityValue objects. 
	Stores only pointers to the objects. 
*/
typedef vector < StateFieldEntityValue*, allocator<StateFieldEntityValue*> > StateFieldEntityValueVector;

#endif

