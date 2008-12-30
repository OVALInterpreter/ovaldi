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

#ifndef ITEM_H
#define ITEM_H

#include <string>
#include <vector>
#include <map>
#include <utility>
#include <iostream>
#include <stdlib.h>

#include "Common.h"
#include "ItemEntity.h"
#include "OvalMessage.h"
#include "DocumentManager.h"


XERCES_CPP_NAMESPACE_USE
using namespace std;

class Item;

/**
	A vector for storing Item objects. 
	Stores only pointers to the objects. 
*/
typedef vector < Item*, allocator<Item*> > ItemVector;

/**	
	A pair for storing  item ids and Item objects together. 
	Stores only pointers to the objects. 
*/
typedef pair <int, Item* > ItemPair;

/**	
	A map for storing ItemPairs. 
	Stores only pointers to the objects. 
*/
typedef map <int, Item* > ItemMap;

/**
	This class represents an Item in a system characteristics document.

	Items are created and populated by probes during data collection. During
	data collection Items must be unique. The Item::Equals() method and the 
	caching methods provided in this class should be utilized to ensure uniqueness.
	This class also provides static methods for assigning Item ids.

	Items are also created during analysis by parsing them from a system-characteristics 
	document.
      
*/
class Item {
public:
	/** Create a complete Item object. */
	Item(int id = 0, string xmlns = "", string xmlnsAlias = "", string schemaLocation = "", OvalEnum::SCStatus status = OvalEnum::STATUS_EXISTS, string name = "");
	
	/** Clean up all the elements. */
	~Item();

	/** Assign a new Item Id. */
	static int AssignId();

	bool Equals(Item* Item);

	/** Return all elements with the specified name.
		@return Returns a vector of elements with a matching name. If no mathes are found the vector is empty. The caller should delete the returned vector but not its contents.
	*/
	ItemEntityVector* GetElementsByName(string elementName);

	/** Parse the provided item element from an sc file into an Item object. */
	void Parse(DOMElement* scItemElm);

	/** Write this item to a sc file. 
		Make sure the item has not already been written to the sc file.
	*/
	void Write(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* scFile, DOMElement* itemsElm);

	ItemEntityVector* GetElements();
	void SetElements(ItemEntityVector* elements);

	int GetId();
	void SetId(int id);

	void SetIsWritten(bool isWritten);
	bool GetIsWritten();

	OvalMessageVector* GetMessages();
	void SetMessages(OvalMessageVector* messages);

	string GetName();
	void SetName(string name);

	string GetSchemaLocation();
	void SetSchemaLocation(string schemaLocation);

	string GetXmlns();
	void SetXmlns(string xmlns);

	string GetXmlnsAlias();
	void SetXmlnsAlias(string xmlnsAlias);

	OvalEnum::SCStatus GetStatus();
	void SetStatus(OvalEnum::SCStatus status);

	void AppendElement(ItemEntity* itemEntity);
	void AppendMessage(OvalMessage* msg);

	ItemEntityVector* GetObjectElements();

	/** Delete all items in the cache. */
	static void ClearCache();

	/** Return an item object for the specified item id.
		First the cache of Items is checked. If the item is
		not found in the cache the item is looked up in the
		system-characteristics document and parsed. Once parsed 
		the new Item object is added to the cache.

		If the item is not found an exception is thrown. 

		
		NOTE: This method is not intended to be used during data collection.
	*/
	static Item* GetItemById(string itemId);

private:

	/** Delete all the elements in the item entity vector. */
	void DeleteElements();

	/** Cache the specified item. */
	static void Cache(Item* item);

	/** Search the cache of Items for the specifed Item. 
		@return Returns the item with the specified id or NULL if not found.
	*/
	static Item* SearchCache(int id);

	ItemEntityVector elements;
	int id;
	OvalMessageVector messages;
	string name;
	OvalEnum::SCStatus status;
	string xmlns;
	string xmlnsAlias;
	string schemaLocation;
	bool isWritten;

	static int idCounter;

	static ItemMap processedItemsMap;
};

/** 
	This class represents an Exception that occured while processing an item.
*/
class ItemException : public Exception {
	public:
		ItemException(string errMsgIn = "", int severity = ERROR_FATAL, Exception *ex = NULL);
		~ItemException();
};

#endif
