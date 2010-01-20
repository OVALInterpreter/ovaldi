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

#include "AbsState.h"
#include "FilterEntity.h"

XERCES_CPP_NAMESPACE_USE

class FilterEntity;
class Item;
/**
	This class represents a filter in a set object of the oval definition schema.
	A Filter is a state that is used by a set object to filter out items from a set
	when creating a set of objects to analyze.
*/
class Filter : public AbsState {

public:
	~Filter();

	/** Get a Filter object for the specified state id.
		Manages the cache of filters.
	*/
	static Filter* GetFilter(std::string statieId);

	/** Analyze the specified Item return the boolean result for the Item. */
	bool Analyze(Item* item);	

	/** Parse the provided state from a definition file into an Filter object. */
	void Parse(DOMElement* stateElm);

	/** Delete all items in the cache. */
	static void ClearCache();

private:

	/** Create a Filter object setting only the operator and version properties. */
	Filter(OvalEnum::Operator myOperator = OvalEnum::OPERATOR_AND, int version = 1);

	/**
		Parse the state element with the specified id into a Filter object.
		@param id a string that hold the id of a state in an oval definition file to be parsed.
	*/
	Filter(std::string id);

	/** Create a complete Filter object setting all properties. */
	Filter(std::string id, std::string name, std::string xmlns, OvalEnum::Operator myOperator = OvalEnum::OPERATOR_AND, int version = 1);

	/** Cache the specified filter. */
	static void Cache(Filter* filter);

	/** Search the cache of Filters for the specified filter. 
		@return Returns a filter object with the specified id or NULL if not found
	*/
	static Filter* SearchCache(std::string id);

	static AbsStateMap processedFiltersMap;
};

#endif
