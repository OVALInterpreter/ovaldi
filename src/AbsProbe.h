//
//
//****************************************************************************************//
// Copyright (c) 2002-2012, The MITRE Corporation
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

#ifndef ABSPROBE_H
#define ABSPROBE_H

#include <iostream>
#include <string>
#include <vector>

#include "Log.h"
#include "Common.h"
#include "Exception.h"
#include "REGEX.h"
#include "Item.h"
#include "ItemEntity.h"
#include "Object.h"
#include "ObjectEntity.h"
#include "Filter.h"


class Object;
class ObjectEntity;
class Item;
typedef std::vector < Item* > ItemVector;

/**
	This class in the base for all probes. 
	The AbsProbe class has provides several methods used by all probes including a
	caching mechanism for storing the results of a data collection run and ensuring 
	that an item on a system is only ever represented once in a data collection 
	results.
*/
class AbsProbe {
public:

	/** create a new probe
	    instatiate a new REGEX object
	*/
	AbsProbe();

	/** destroy the probe.
	    Delete the regex object.
		clear the item cache
	*/
	virtual ~AbsProbe();	

	/**
		Run the probe and collect all items on the system that match the specified object.
		This method is the starting point for all probes. the Run method provides a set of common
		functionality to all probes and then calls the AbsProbe::CollectItems(Object* object) virtual 
		method for actually collecting system items.
		@param object the Object* that is used to guide data colelction.
		@return A vector of items found on the system.
	*/
	ItemVector* Run(Object* object);
	
	/**
		Clear the cache of all Items collected by all probes.
	*/
	static void ClearGlobalCache();

protected:	
	/**
		Collects all Items on the system that match the specified Object.
		@param object A Object* that specifies what information to collect on the system.
		@return A vector of Items found on the system.
	*/
	virtual ItemVector* CollectItems(Object* object) = 0;

    /**
		Create an Item for storing the type of data that the probe collects.
		Each probe has its own implementation of this function. Probes set the 
		name, xmlns alias, schema location, and xmlns for the new item.
		@return A new Item.
	*/
	virtual Item* CreateItem() = 0;

	/**
		Create a new ItemEntity based on the specifeid ObjectEntity.
		All ItemEntities created by this function are deleted after a single run of the probe is completed.
		@param obj an ObjectEntity* that is used as a guide for creating the new ItemEntity.
		@return The resulting cached ItemEntity*
	*/
	ItemEntity* CreateItemEntity(ObjectEntity* obj = NULL);

	/**
		Deletes all ItemEntities that were created by the AbsProbe::CreateItemEntity(ObjectEntity* obj) method.
	*/
	void DeleteItemEntities();

	/**
		Return true if the specified value matches the specified pattern.
		This function uses the isRegex parameter to determine how to apply the pattern.
		@param pattern a string that specifies the pattern to compare against.
		@param value a string that specified that value to compare against.
		@param isRegex a bool that dictates how to apply the pattern. If true apply the pattern as a regular expression otherwise use a string compare for not equals. 
		@return The result of the comparison betweent eh two string values.
	*/
	bool IsMatch(std::string pattern, std::string value, bool isRegex = true);


	/** All probes make use of the regular expression processor for pattern matching. */
	REGEX *myMatcher;
	
private:
	void ApplyFilters(ItemVector* items, FilterVector* filters);

	/** 
		Cache all items in the vector of items.
		Called by the AbsProbe::Run(Object* object) to ensure that all Items return by a probe
		are cached and unique. Duplicate Items are deleted and the existing item is returned.
		@param items the set of Items to cache.
		@return A set of unique items that have all been cached.
	*/
	ItemVector* CacheAllItems(ItemVector* items);
    
	/** 
		The list of all ItemEntities created by the probe. 
		These are deleted after each run of the probe.
	*/
	ItemEntityVector createdItemEntities;

	/** The cache of all Items created by all probes. */
    static StringKeyedItemMap globalItemCache;
};


/** 
	This class represents an Exception that occured while processing a probe.
*/
class ProbeException : public Exception {
	public:

		ProbeException(std::string errMsgIn = "", int severity = ERROR_FATAL, Exception* ex = NULL);
		~ProbeException();
};

#endif
