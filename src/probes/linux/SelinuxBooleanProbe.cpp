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

#include <algorithm>
#include <iterator>
#include <memory>
#include <string>
#include <selinux/selinux.h>

#include <VectorPtrGuard.h>
#include <OvalEnum.h>
#include "SelinuxBooleanProbe.h"

using namespace std;

namespace {
	auto_ptr<Item> CreateItem();

	/**
	 * Adds items to the given item vector for the given selinux boolean names.
	 */
	void GetBooleansByName(const StringVector &names, 
						   ObjectEntity *nameObjEntity, ItemVector *items);

	/**
	 * Searches all selinux boolean names, adding items for those which match
	 * the given name entity.
	 */
	void SearchAllBooleans(ObjectEntity *nameObjEntity, ItemVector *items);
	
	/**
	 * Given an item which already contains a name object entity for the given
	 * selinux boolean name, complete the item by adding the rest of the
	 * entities.
	 */
	void CompleteItem(Item *item, const string &boolName);
}

//****************************************************************************************//
//								SelinuxBoolean Class							  //	
//****************************************************************************************//
SelinuxBooleanProbe *SelinuxBooleanProbe::instance = NULL;

SelinuxBooleanProbe::SelinuxBooleanProbe() {
	security_load_booleans(NULL);
}

SelinuxBooleanProbe::~SelinuxBooleanProbe() {
	instance = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* SelinuxBooleanProbe::Instance() {
	// Use lazy initialization
	if(instance == NULL) 
		instance = new SelinuxBooleanProbe();
	
	return instance;
}

ItemVector* SelinuxBooleanProbe::CollectItems(Object* object) {

	VectorPtrGuard<Item> collectedItems(new ItemVector());

	ObjectEntity *nameObjEntity = object->GetElementByName("name");

	if (nameObjEntity->GetOperation() == OvalEnum::OPERATION_EQUALS) {
		StringVector names;
		/* OvalEnum::Flag flag =*/ nameObjEntity->GetEntityValues(names);
		GetBooleansByName(names, nameObjEntity, collectedItems.get());
	} else
		SearchAllBooleans(nameObjEntity, collectedItems.get());

	return collectedItems.release();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

/** This is never called */
Item* SelinuxBooleanProbe::CreateItem() {
	return NULL;
}

namespace {

	auto_ptr<Item> CreateItem() {
		// -----------------------------------------------------------------------
		//
		//  ABSTRACT
		//
		//  Return a new Item created for storing selinux boolean information
		//
		// -----------------------------------------------------------------------

		auto_ptr<Item> item(new Item(0, 
									 "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#linux", 
									 "linux-sc", 
									 "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#linux linux-system-characteristics-schema.xsd", 
									 OvalEnum::STATUS_ERROR, 
									 "selinuxboolean_item"));

		return item;
	}

	void GetBooleansByName(const StringVector &names, 
						   ObjectEntity *nameObjEntity, ItemVector *items) {

		for (StringVector::const_iterator nameIter = names.begin();
			 nameIter != names.end();
			 ++nameIter) {

			auto_ptr<ItemEntity> nameItemEntity(
				new ItemEntity("name", *nameIter, OvalEnum::DATATYPE_STRING, 
							   true));
			
			if (nameObjEntity->Analyze(nameItemEntity.get()) != 
				OvalEnum::RESULT_TRUE)
				continue;
			
			auto_ptr<Item> item = ::CreateItem();
			item->SetStatus(OvalEnum::STATUS_EXISTS);
			item->AppendElement(nameItemEntity.release());
			CompleteItem(item.get(), *nameIter);
			items->push_back(item.release());
		}
	}

	void SearchAllBooleans(ObjectEntity *nameObjEntity, ItemVector *items) {
		char **boolNames;
		int numNames;
		
		// the function is declared to return int, but there is no documentation
		// describing how to interpret the return value.  So I am ignoring it.
		/* int res =*/ security_get_boolean_names(&boolNames, &numNames);

		// We can reuse GetBooleansByName to process the names.  So add all the
		// names to a vector.
		StringVector nameVec;

		copy(&boolNames[0], &boolNames[numNames], back_inserter(nameVec));

		GetBooleansByName(nameVec, nameObjEntity, items);
	}

	void CompleteItem(Item *item, const string &boolName) {
		bool curr, pending;
		
		curr = security_get_boolean_active(boolName.c_str());
		pending = security_get_boolean_pending(boolName.c_str());

		item->AppendElement(new ItemEntity("current_status", 
										   Common::ToString(curr), 
										   OvalEnum::DATATYPE_BOOLEAN));
		item->AppendElement(new ItemEntity("pending_status", 
										   Common::ToString(pending),
										   OvalEnum::DATATYPE_BOOLEAN));		
	}
}
