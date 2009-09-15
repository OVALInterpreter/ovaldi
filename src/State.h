//
//
//****************************************************************************************//
// Copyright (c) 2002-2009, The MITRE Corporation
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

#ifndef STATE_H
#define STATE_H

#include "AbsState.h"
#include "StateEntity.h"

XERCES_CPP_NAMESPACE_USE
using namespace std;

class StateEntity;
class Item;
class State;

/**
	This class represents a state in the oval definition schema.
	States are used by tests for analysis purposes. The State 
	declares what an expected condition is for some item on a system.
*/
class State : public AbsState {

public:

	~State();

	/** Analyze the specified Item return the Result value for the Item.
	
		1 - create a vector of Item elements that match each element in the state.
		2 - pass the vector to the StateEntity analyze method
		3 - build a vector of results for each element in the state.
		4 - combine the results to a single value based on the states operator
	*/
	OvalEnum::ResultEnumeration Analyze(Item* item);

	/** Parse the provided state element from a oval definition file into a State object. */
	void Parse(DOMElement* stateElm);

	/** Sarch the cache of States for the specified State. 
		Return NULL if not found
	*/
	static State* SearchCache(string id);

	/** Delete all items in the cache. */
	static void ClearCache();

	/** Cache the specified state. */
	static void Cache(State* state);

	/** Return a state object for the specified state id.
		First the cache of States is checked. If the state is
		not found in the cache the state is looked up in the
		oval-definitions document and parsed. Once parsed the new State
		object is added to the cache.

		If the state is not found an exception is thrown. 
	*/
	static State* GetStateById(string stateId);

private:

	/** Create a new State object.
		Sets the operator and version
	*/
	State(OvalEnum::Operator myOperator = OvalEnum::OPERATOR_AND, int version = 1);
	

	static AbsStateMap processedStatesMap;
};

#endif
