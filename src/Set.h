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

#ifndef SET_H
#define SET_H

#include <string>
#include <vector>
#include <iostream>
#include <stdlib.h>

#include "Common.h"
#include "Filter.h"
#include "Object.h"
#include "OvalEnum.h"
#include "VariableValue.h"

XERCES_CPP_NAMESPACE_USE
using namespace std;

// Forward declarations to allow compilation.
class Filter;
class AbsState;
typedef vector < AbsState*, allocator<AbsState*> > AbsStateVector;
class Set;
typedef vector < Set*, allocator<Set*> > SetVector;
// end forward declarations

/**
	This class represents an Set in an oval definition schema.
	Sets are used by set objects to construct complex sets of items on a sytem for analysis.
*/
class Set {

public:
	/** Parse the set element and populate this object with its data.*/
	Set(DOMElement* setElm);

	/** Initialize the set and set the simple set flag to true */
	Set();

	/** Clean up all the set elements. */
	~Set();

	/** Parse the provided Set object element. */
	void Parse(DOMElement* setElm);

	/** Return a vector of all variable values used for this set. */
	VariableValueVector* GetVariableValues();
	
	/** Return the filters field's value. */
	AbsStateVector* GetFilters();
	/** Set the filters field's value. */
	void SetFilters(AbsStateVector* filters);

	/** Get the isSimpleSet field's value. */
	bool GetIsSimpleSet();
	/** Set the isSimpleSet field's value. */
	void SetIsSimpleSet(bool isSimpleSet);

	/** Return the referenceOne field's value. */
	AbsObject* GetReferenceOne();
	/** Set the referenceOne field's value. */
	void SetReferenceOne(AbsObject* object);

	/** Return the referenceTwo field's value. */
	AbsObject* GetReferenceTwo();
	/** Set the referenceTwo field's value. */
	void SetReferenceTwo(AbsObject* object);

	/** Return the setOne field's value. */
	Set* GetSetOne();
	/** Set the setOne field's value. */
	void SetSetOne(Set* set);

	/** Return the setTwo field's value. */
	Set* GetSetTwo();
	/** Set the setTwo field's value. */
	void SetSetTwo(Set* set);
	
	/** Return the setOperator field's value. */
	OvalEnum::SetOperator GetSetOperator();
	/** Set the setOperator field's value. */
	void SetSetOperator(OvalEnum::SetOperator setOperator);

	/** Add a filter to the end of the filters vector. */
	void AppendFilter(Filter* filter);
    
private:
	AbsStateVector filters;
	AbsObject* referenceOne;
	AbsObject* referenceTwo;
	Set* setOne;
	Set* setTwo;
	OvalEnum::SetOperator setOperator;
	bool isSimpleSet;
};

/** 
	This class represents an Exception that occured while processing a set.
*/
class SetException : public Exception {
	public:
		/** Set the error message and then set the severity to ERROR_FATAL. This is 
			done with the explicit call to the Exception class constructor that 
			takes a single string param.
		*/
		SetException(string errMsgIn = "", int severity = ERROR_FATAL, Exception *ex = NULL);

		~SetException();
};

#endif
