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

#ifndef TESTEDITEM_H
#define TESTEDITEM_H

#include <xercesc/dom/DOMElement.hpp>

#include "Item.h"

/**
	This class representsa tested_item int eh oval results schema.
	The class pairs an Item tieh the result of its analysis in the context of a specific test.
*/
class TestedItem {

public:

	/** Create a Test object and initialize its result to OvalEnum::RESULT_NOT_EVALUATED. */
	TestedItem();
	~TestedItem();

	/** Write a TestedItem element as a child of the parent element. */
	void Write(xercesc::DOMElement* parent);
	
	/** Return the result field's value. */
	OvalEnum::ResultEnumeration GetResult();

	/** Set the result field's value. */
	void SetResult(OvalEnum::ResultEnumeration result);
    
	/** Return the item field's value. */
	Item* GetItem();

	/** Set the item field's value. */
	void SetItem(Item* item);

private:
	OvalEnum::ResultEnumeration result;
	Item* item;
};

/**	
	A vector for storing TestedItem objects. 
	Stores only pointers to the objects. 
*/
typedef std::vector < TestedItem* > TestedItemVector;

#endif
