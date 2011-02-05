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

#ifndef RESTRICTIONTYPE_H
#define RESTRICTIONTYPE_H

#include "Common.h"
#include "OvalEnum.h"
#include "EntityComparator.h"


/**
	This class represents the RestrictionType related to external variables in the oval definition schema.
*/
class RestrictionType {
public:

	RestrictionType(OvalEnum::Datatype datatype, std::string value);
	RestrictionType();
	~RestrictionType();

	/** Parses a valid RestrictionType element as defined in the oval definitions schema. */
	void Parse(DOMElement* restrictionTypeElm);

	/** Ensure that the specified value matches the criteria specified by this restriction. */
	bool ValidateValue(OvalEnum::Datatype datatype, std::string externalValue);

	void SetValue(std::string value);
	std::string GetValue();

	void SetOperation(OvalEnum::Operation operation);
	OvalEnum::Operation GetOperation();

private:

	std::string value;
	OvalEnum::Datatype datatype;		
	OvalEnum::Operation operation;		
};

typedef std::vector < RestrictionType* > RestrictionTypeVector;
#endif
