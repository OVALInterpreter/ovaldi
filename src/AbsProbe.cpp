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

#include "AbsProbe.h"

ItemVector AbsProbe::globalItemCache;
//****************************************************************************************//
//								AbsProbe Class											  //	
//****************************************************************************************//

AbsProbe::AbsProbe() {

	myMatcher = new REGEX();
}

AbsProbe::~AbsProbe() {

	delete myMatcher;

	this->itemCache.clear();
}

//****************************************************************************************//
//								Protected Memebers										  //	
//****************************************************************************************//
Item* AbsProbe::CacheItem(Item* newItem) {

	Item* tmp = NULL;
	// Loop through cache
	ItemVector::iterator itemIterator;
	for(itemIterator = this->itemCache.begin(); itemIterator != this->itemCache.end(); itemIterator++) {
		
		// Compare the new item to the cached item if they are the same
		// break and return the cached item.
		if(newItem->Equals((*itemIterator))) {
			tmp = newItem;
			newItem = (*itemIterator);
			break;
		} 
	}

	if(tmp != NULL) {
		delete tmp;
		tmp = NULL;
	} else {
		// to get here the newItem did not match the cached item
		// so cache it
		if(newItem->GetId() == 0) {
			newItem->SetId(Item::AssignId());
		}
		this->itemCache.push_back(newItem);
		AbsProbe::globalItemCache.push_back(newItem);
	}
	return newItem;
}   

ItemVector* AbsProbe::Run(Object* object) {

	// create a vector of items that match the specified object
	ItemVector* items = this->CollectItems(object);	
	this->DeleteItemEntities();
	items = this->CacheAllItems(items);

	return items;
}

ItemVector* AbsProbe::CacheAllItems(ItemVector* items) {

	ItemVector* cachedItems = new ItemVector();
	while(items->size() > 0) {
		Item* item = (*items)[items->size()-1];
	  	items->pop_back();

		Item* cachedItem = this->CacheItem(item);
		cachedItems->push_back(cachedItem);
	}
	delete items;
	items = NULL;
	return cachedItems;
}

void AbsProbe::ClearGlobalCache() {

	Item* item = NULL;
	while(AbsProbe::globalItemCache.size() != 0) {
	  	item = AbsProbe::globalItemCache[AbsProbe::globalItemCache.size()-1];
	  	AbsProbe::globalItemCache.pop_back();
	  	delete item;
	  	item = NULL;
	}
}

ItemEntity* AbsProbe::CreateItemEntity(ObjectEntity* obj) {

	ItemEntity* itemEntity = new ItemEntity();
	if(obj != NULL) {
		itemEntity->SetName(obj->GetName());
		itemEntity->SetStatus(OvalEnum::STATUS_EXISTS);
		itemEntity->SetDatatype(obj->GetDatatype());
	} 
	this->createdItemEntities.push_back(itemEntity);
	return itemEntity;
}

void AbsProbe::DeleteItemEntities() {

	ItemEntity* tmp = NULL;
	while(this->createdItemEntities.size() != 0) {
	  	tmp = this->createdItemEntities[this->createdItemEntities.size()-1];
	  	this->createdItemEntities.pop_back();
	  	delete tmp;
	  	tmp = NULL;
	}
}

bool AbsProbe::IsMatch(string pattern, string value, bool isRegex) {

	bool match = false;

	if(isRegex) {
		if(this->myMatcher->IsMatch(pattern.c_str(), value.c_str())) {
			match = true;
		}
	} else {	
		if(value.compare(pattern) != 0) {
			match = true;
		}
	}

	return match;
}

//****************************************************************************************//
//								ProbeException Class									  //	
//****************************************************************************************//
ProbeException::ProbeException(string errMsgIn, int severity, Exception* ex) : Exception(errMsgIn, severity, ex) {

}

ProbeException::~ProbeException() {

}
