//
// $Id: PossibleValueType.h 4579 2008-01-02 17:39:07Z bakerj $
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

#ifndef POSSIBLEVALUETYPE_H
#define POSSIBLEVALUETYPE_H

#include "Common.h"
#include "OvalEnum.h"
#include "EntityComparator.h"

using namespace std;
class PossibleValueType;

typedef vector < PossibleValueType*, allocator<PossibleValueType*> > PossibleValueTypeVector;

/**
	This class represents an the PossibleValueType related to external variables in the oval definitions schema.
*/
class PossibleValueType {
public:

	/** Create a PossibleValueType. */
	PossibleValueType();
	~PossibleValueType();

	/** Parses a valid PossibleValueType element as defined in the oval definitions schema. */
	void Parse(DOMElement* possibleElm);

	/** Ensure that the specified value matches the criteria specified by this possible_value element. */
	bool ValidateValue(OvalEnum::Datatype datatype, string externalValue);

	void SetHint(string hint);
	string GetHint();

	void SetValue(string value);
	string GetValue();

private:
	string hint;
	string value;
};

#endif
