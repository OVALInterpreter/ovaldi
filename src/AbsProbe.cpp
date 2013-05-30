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

#include <algorithm>
#include <set>

#include "AbsProbe.h"

using namespace std;

namespace {
	// seems like the most typical std::less impl would not modify
	// its args, so it makes sense to declare them const.  In order
	// that std::less's first_argument_type and second_argument_type
	// typedefs don't lie about its arg types, that means you need to
	// specialize std::less for a const type, rather than use non-const
	// and just declare operator()(const T&, const T&) (i.e. add separate
	// const's in the formal parameter list).  But you also ought to
	// be able to use that comparator with a set with a non-const 
	// value_type, which means you can't just declare set<T> and use
	// the default comparator, because it would use std::less<T> rather
	// than std::less<const T>.  So you have to force a <const T>
	// comparator.  It's a weird situation... or I spose I could have
	// just not bothered with const at all, but shouldn't we try to
	// use const wherever it makes sense, to ensure const-correctness??
	// Or maybe I should have implemented operator<() for items and
	// used the default std::less template...?
	typedef set<Item*, std::less<const Item*> > ItemCache;
	ItemCache globalItemCache;
}

//****************************************************************************************//
//								AbsProbe Class											  //	
//****************************************************************************************//

AbsProbe::AbsProbe() {
	myMatcher = new REGEX();
}

AbsProbe::~AbsProbe() {

	delete myMatcher;
}

//****************************************************************************************//
//								Protected Memebers										  //	
//****************************************************************************************//
ItemVector* AbsProbe::Run(Object* object) {

	// create a vector of items that match the specified object
	ItemVector* items = this->CollectItems(object);	
	this->DeleteItemEntities();
	this->ApplyFilters(items, object->GetFilters());
	items = this->CacheAllItems(items);

	return items;
}

void AbsProbe::ApplyFilters(ItemVector* items, FilterVector* filters) {
	for(FilterVector::iterator filterIterator = filters->begin();
		filterIterator != filters->end(); ++filterIterator)
		items->erase(
			remove_if(items->begin(), items->end(), FilterFunctor(*filterIterator)),
			items->end());
}

ItemVector* AbsProbe::CacheAllItems(ItemVector* items) {

    ItemVector* cachedItems = new ItemVector();
    
	for(ItemVector::iterator itemIt = items->begin(); itemIt != items->end(); itemIt++) {
        Item* cacheCandidateItem = (*itemIt);

        pair<ItemCache::iterator, bool> ret = globalItemCache.insert(cacheCandidateItem);

        // if a new element was inserted ret.second will be true
        if (ret.second == true) {

            // need to id the item
            if(cacheCandidateItem->GetId() == 0) {
		    	cacheCandidateItem->SetId(Item::AssignId());
		    }
        } else {
            // new element was not inserted so delete the current candidate item
            delete cacheCandidateItem;
            cacheCandidateItem = NULL;
        }

        // add the corresponding item in the global cache to the output of cachedItems
        ItemCache::iterator cachedItemIt = ret.first;
        Item* retItem = *cachedItemIt;
        cachedItems->push_back(retItem);
	}

    items->clear();
	delete items;
	items = NULL;

	return cachedItems;
}

void AbsProbe::ClearGlobalCache() {

    for(ItemCache::iterator it = globalItemCache.begin(); it != globalItemCache.end(); it++) {
        Item* item = *it;
        delete item;
	  	item = NULL;
    }
    globalItemCache.clear();
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
