//
//
//****************************************************************************************//
// Copyright (c) 2002-2013, The MITRE Corporation
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

#ifndef CRITERION_H
#define CRITERION_H

#include "Analyzer.h"
#include "Test.h"
#include "AbsCriteria.h"


XERCES_CPP_NAMESPACE_USE

class AbsCriteria;

/**
	This class represenets a criterion in an oval definition.
*/
class Criterion : public AbsCriteria {

public:
	/** Create a complete Criterion object. */
	Criterion(bool negate = false, ApplicabilityCheck appCheck = APPLICABILITY_CHECK_UNKNOWN,
		OvalEnum::ResultEnumeration result = OvalEnum::RESULT_ERROR, Test* testRef = NULL);
	/** Empty destructor. */
	~Criterion();

	/** Writes a criterion element
	    calls test->Write() on the test ref 
	*/
	void Write(DOMElement* parent);
	/** Parses Criterion elm to a Criterion obj
		calls test->Parse() on the Test Ref
	*/
	void Parse(DOMElement* CriterionElm);
	/** Determine the result for the Criteria.
		Calls test->Analyze()
		Applies negate attribute
	    Saves and return the result
	*/
	OvalEnum::ResultEnumeration Analyze();
	/** Process the test as Not Evaluated. */
	OvalEnum::ResultEnumeration NotEvaluated();

	/** Return the testRef field's value. */
	Test* GetTestRef();
	/** Set the testRef field's value. */
	void SetTestRef(Test* testRef);

private:
	Test* testRef;
};

#endif
