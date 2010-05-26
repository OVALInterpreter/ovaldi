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

#ifndef FILTER_H
#define FILTER_H

#include <vector>
#include <xercesc/dom/DOMElement.hpp>
#include "Item.h"
#include "State.h"
#include "VariableValue.h"

/**
	This class represents a filter in a set object of the oval definition schema.
	A Filter refers to a state that is used by a set object to filter out items from a set
	when creating a set of items to analyze.
*/
class Filter {

public:
	Filter(xercesc::DOMElement *filterElm);


	/**
	 * Returns the selected action for this filter.
	 * \return true if the selected action for this filter is "exclude",
	 * or false otherwise.
	 */
	bool IsExcluding(){return excluding;}

	/**
	 * Sets the action for this filter.  See IsExcluding().
	 */
	void SetExcluding(bool excluding)
	{ this->excluding = excluding; }

	/**
	 * Test the specified Item and return a boolean result.
	 * The return value reflects whether the item "passed" the filter.  E.g.
	 * if the item satisfied the state and the filter action is "include", then
	 * true is returned.  Or if the item did not satisfy the state and the filter
	 * Action is "exclude", then true is returned.  Otherwise false is returned.
	 */
	bool DoFilter(Item* item);	

	/**
	 * Get variable values used in the referenced state.
	 */
	VariableValueVector *GetVariableValues()
	{ return state->GetVariableValues(); }

private:

	/** Parse the provided filter from a definition file into a Filter object. */
	void Parse(xercesc::DOMElement* filterElm);

	/**
	 * A filter may have one of two values for its "action" attribute: "include" or
	 * "exclude".  This field represents the selected action for this filter.
	 */
	bool excluding;

	/**
	 * This filter's associated state.
	 */
	State *state;
};

typedef std::vector<Filter*> FilterVector;

#endif
