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

#ifndef ARITHMETICFUNCTION_H
#define ARITHMETICFUNCTION_H

#include "AbsFunctionComponent.h"
#include "ComponentFactory.h"

XERCES_CPP_NAMESPACE_USE

/**
	This class represents a ArithmeticFunction component in a local_variable in the 
    oval definition schema.

    The arithmetic function takes two or more integer or float components and performs a 
    basic mathmetical function on them.  The result of this function in a single integer
    or float.  Note that if both an integer and float components are used then the result
    is a float.
*/
class ArithmeticFunction : public AbsFunctionComponent {
public:

	/** Create a complete ArithmeticFunction object. */
    ArithmeticFunction(OvalEnum::ArithmeticOperation op = OvalEnum::ARITHMETIC_ADD);
	~ArithmeticFunction();

	/** Parse the begin element and its child component element. */
	void Parse(DOMElement* componentElm); 

	/** Compute and return the value. */
	ComponentValue* ComputeValue();

	/** Return the variable values used to compute this function's value. */
	VariableValueVector* GetVariableValues();

	/** Get the arithmetic_operation field's value. */
	OvalEnum::ArithmeticOperation GetArithmeticOperation();
	/** Set the arithmetic_operation field's value. */
	void SetArithmeticOperation(OvalEnum::ArithmeticOperation opIn);

private:
    /**
     * Does a cartesian cross combination (according to this function's operator)
     * of the given two ComponentValue's, and returns the resulting ComponentValue.
     * @param componentValue1 The first ComponentValue
     * @param componentValue1 The second ComponentValue
     * @return the combined ComponentValue
     * @throw Exception if the arithmetic operator is not supported
     */
    ComponentValue* CombineTwoComponentValues(ComponentValue* componentValue1, ComponentValue* componentValue2);

    OvalEnum::ArithmeticOperation arithmeticOperation;
};

#endif
