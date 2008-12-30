//
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

#ifndef ABSOBJECTCOLLECTOR_H
#define ABSOBJECTCOLLECTOR_H


//	other includes
#include <string>
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <typeinfo>

#include "Common.h"
#include "OvalEnum.h"
#include "Filter.h"
#include "Object.h"
#include "SetObject.h"
#include "Item.h"
#include "CollectedObject.h"
#include "CollectedSet.h"
#include "AbsProbe.h"

//	namespaces
using namespace std;

// forward declarations
class AbsProbe;
class SetObject;
class Object;
class Set;
class CollectedObject;
class CollectedSet;
class Filter;
class AbsState;
typedef vector < AbsState*, allocator<AbsState*> > AbsStateVector;


struct probe_comparator {
  bool operator() ( AbsProbe * p1, AbsProbe * p2 ) {
    return p1 < p2;
  }
};


typedef set<AbsProbe *, probe_comparator> AbsProbeSet;


/**
	This class acts a base class for all platform specific object collectors.
*/
class AbsObjectCollector {
public:
	virtual ~AbsObjectCollector();

	/** Return the protected static instance variable. This calss is a singleton. */
	static AbsObjectCollector* Instance();

	/** 
	    Collect the object with the specified id.
	    Return a collected object for the specified object id.
	    Check the cache of all collected objects to make sure the specified
	    object has not already been processed. 
	    TODO: Clarify this comment...
	    This interpreter will never create a collected object based on an
	    object other than a set object that is flagged as incomplete.
	*/
	CollectedObject* Run(string objectId);

protected:
	AbsObjectCollector();
	static AbsObjectCollector* instance;

	/** 
		Return true if the input object is applicable on the host being evaluated.
		Return true if the specified object is applicable to this object 
		collector. All objects defined in the windows and independent schemas 
		are applicable
	*/
	virtual bool IsApplicable(AbsObject* object) = 0;

	/** Return true if the specified object is supported by this object collector. */
	virtual bool IsSupported(AbsObject* object) = 0;

	/** Call the ProbeFactory and get a probe capable of collecting the input object.
		If an appropriate probe is not found NULL is returned.
	*/
	virtual AbsProbe* GetProbe(Object* object) = 0;
	
	/** Based on the type of object call the appropriate process method. */
	CollectedObject* Process(AbsObject* absObject);

	/**
		Process the input set object and return the resulting CollectedObject.
		The following actions are taken:
		 - Copy the variable values from the specified object.
		 - Make call to process set to start the recursive processing of the set.
		 - Set the references based on the results of processing the set 
		 - Set the flag based on the results of processing the set
		 - Copy the variable values used to create the collected set

	*/
	CollectedObject* ProcessSetObject(SetObject* setObject);

	/** 
		Process the input object and return the resulting CollectedObject.
		The following actions are taken:
		 - make sure the object is supported and applicable
		 - copy any variable values from the object
		 - Get the probe for the object
		 - Run the probe with the object
		 - Set the matching collected items for the collected object
		 - Set the flag value for the collected object.
	*/
	CollectedObject* ProcessObject(Object* object);

	/** 
		Process the input set and return the resulting CollectedSet.
		Either recursivley process each child set or Process each object reference. 
	*/
	CollectedSet* ProcessSet(Set* set);	
	
	/** Based the input set operator combine the 2 input flag values and return the resulting flag value. */
	OvalEnum::Flag CombineFlagBySetOperator(OvalEnum::SetOperator setOp, OvalEnum::Flag set1Flag, OvalEnum::Flag set2Flag);

	/**
	    Return a true if the specified item exists in the specified set
	    Comparing items based on their ids assumes that Item ids are only
	    assigned to unique items. This is ensured when probes return Items.
	*/
	bool ExistsInSet(ItemVector* itemSet, Item* item);

	/** Return a single set that contains all unique items in both sets. */
	CollectedSet* Union(CollectedSet* collectedSet1, CollectedSet* collectedSet2);

	/** 
		Create a set of items that exist only in one of the specified sets. 
		From the schema documentation....
		The intersection of two sets in OVAL results in a set that contains 
		everything that belongs to both sets in the collection, but nothing else. 
		If A and B are sets, then the intersection of A and B contains all 
		the elements of A that also belong to B, but no other elements.
	*/
	CollectedSet* Intersection(CollectedSet* collectedSet1, CollectedSet* collectedSet2);

	/** Create a set of unique items in set 1 but not in set 2, a relative complement. */
	CollectedSet* Compelement(CollectedSet* collectedSet1, CollectedSet* collectedSet2);

	/** 
	    Apply the input Filters to the vector of items.
	    For Each Filter provided Loop through all the Items in the current set
	    of items. Only add items to the result set of items if they do NOT 
	    match all filters
	*/
	ItemVector* ApplyFilters(ItemVector* items, AbsStateVector* filters);
	
	/** Copy all items in the src vector to the dest vector.	*/
	void CopyItems(ItemVector* dest, ItemVector* src);
};

/** 
	This class represents an Exception that occured while runing the object collector.
*/
class AbsObjectCollectorException : public Exception {
	public:
		AbsObjectCollectorException(string errMsgIn = "", int severity = ERROR_FATAL, Exception *ex = NULL);
		~AbsObjectCollectorException();
};

#endif
