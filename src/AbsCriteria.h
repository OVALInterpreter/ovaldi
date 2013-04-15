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

#ifndef ABSCRITERIA_H
#define ABSCRITERIA_H

#include <string>
#include <iostream>
#include <stdlib.h>

#include "Common.h"
#include "OvalEnum.h"


XERCES_CPP_NAMESPACE_USE

class AbsCriteria;

/**
	This class represents the the commonalities among all types of criteria in the oval definitions schema.
	The oval definitions schema defines several different types of criteria. Each criteria shares a 
	common set of attributes and the need for a common set of operations. This class encapsulates
	all the common functionality.
*/
class AbsCriteria {

public :

	/**
	 * Possible values for a criteria, criterion, or extend_definition's
	 * applicability_check attribute.  APPLICABILITY_CHECK_UNKNOWN is
	 * required as a possible value because the attribute is optional,
	 * and the definition referenced by an extend_definition element may
	 * not be available.  So you wouldn't be able to determine the actual
	 * value of the attribute.  Actually, assuming the document is valid,
	 * it would sometimes be deducible from the applicability_check's of
	 * other criteria/on in the definition, but not always.
	 */
	enum ApplicabilityCheck {
		APPLICABILITY_CHECK_FALSE,
		APPLICABILITY_CHECK_TRUE,
		APPLICABILITY_CHECK_UNKNOWN
	};

	/** Create a complete AbsCriteria object. 
		All paremters are initialized with default values. 
		Default values are: negate = false; result = error
	*/
	AbsCriteria(bool negate = false, ApplicabilityCheck appCheck = APPLICABILITY_CHECK_UNKNOWN,
		OvalEnum::ResultEnumeration result = OvalEnum::RESULT_ERROR);

	/** Empty destructor. */
	virtual ~AbsCriteria();

	/** Abstract method to write a concrete criteria to an oval results document. */
	virtual void Write(DOMElement* parent) = 0;
	/** Abstract method to parse a concrete criteria from an oval definitions document. */
	virtual void Parse(DOMElement* criteriaElm) = 0;
	/** Abstract method that evaluates a concrete criteria and returns the result. */
	virtual OvalEnum::ResultEnumeration Analyze() = 0;

	/** Abstract method that evaluates a concrete criteria to not evaluated returns the result. */
	virtual OvalEnum::ResultEnumeration NotEvaluated() = 0;



	/** Get the result of the criteria. */
	OvalEnum::ResultEnumeration GetResult();
	/** Set the result of the criteria. */
	void SetResult(OvalEnum::ResultEnumeration result);

	/** Get the negate attribute for the criteria. */
	bool GetNegate();
	/** Set the negate attribute for the criteria. */
	void SetNegate(bool negate);

	void SetApplicabilityCheck(ApplicabilityCheck appCheck);
	ApplicabilityCheck GetApplicabilityCheck();

private:
	/** A flag used to indicate wheter or not that result of this criteria should be negated. */
	bool negate;
	/** The result of this criteria after it has been analyzed. */
	OvalEnum::ResultEnumeration result;
	/**
	 * Whether this criteria/criterion is considered an (or part of an)
	 * applicability check.
	 */
	ApplicabilityCheck applicabilityCheck;
};

/**	
	A vector for storing AbsCriteria objects. 
	Stores only pointers to the objects. 
*/
typedef std::vector < AbsCriteria* > AbsCriteriaVector;

#endif
