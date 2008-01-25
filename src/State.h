//
// $Id: State.h 4579 2008-01-02 17:39:07Z bakerj $
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
	State(OvalEnum::Operator myOperator = OvalEnum::OPERATOR_AND, int version = 1);
	/**
		Parse the state element with the specified id into a State object.
		@param id a string that hold the id of a state in an oval definition file to be parsed.
	*/
	State(string id);
	State(string id, string name, string xmlns, OvalEnum::Operator myOperator = OvalEnum::OPERATOR_AND, int version = 1);
	~State();

	OvalEnum::ResultEnumeration Analyze(Item* item);
	void Parse(DOMElement* stateElm);

	static State* SearchCache(string id);
	static void ClearCache();
	static void Cache(State* state);

private:
	static AbsStateMap processedStatesMap;
};

#endif
