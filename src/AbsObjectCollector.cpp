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
#include "AbsObjectCollector.h"

using namespace std;

//****************************************************************************************//
//							AbsObjectCollector Class									  //	
//****************************************************************************************//
AbsObjectCollector* AbsObjectCollector::instance = NULL;

AbsObjectCollector::AbsObjectCollector() {
	
}

AbsObjectCollector::~AbsObjectCollector() {

}

AbsObjectCollector* AbsObjectCollector::Instance() { 

	return AbsObjectCollector::instance;
}
// ***************************************************************************************	//
//								Public members												//
// ***************************************************************************************	//
CollectedObject* AbsObjectCollector::Run(string objectId) {
	
	// check the cache
	CollectedObject* collectedObject = CollectedObject::GetCollectedObject(objectId);

	// Check to make sure this object has not already been collected.
	if(collectedObject == NULL) {

		//Log::Debug("AbsObjectCollector::Run processing object id: " + objectId);				

		// get the specified object element
		DOMElement* objectsElm = XmlCommon::FindElementNS(DocumentManager::GetDefinitionDocument(), "objects");
		DOMElement* objectElm = XmlCommon::FindElementByAttribute(objectsElm, "id", objectId);
		string versionStr = XmlCommon::GetAttributeByName(objectElm, "version");

		int version;
		if(versionStr.empty()) {
			version = 1;
		} else {
			version = atoi(versionStr.c_str());
		}

		// Parse this object
		AbsObject* absObject = NULL;
		try {
			absObject = ObjectFactory::GetObjectById(objectId);
		} catch (VariableFactoryException ex) {
			collectedObject = CollectedObject::CreateError(objectId, version);
			collectedObject->SetFlag(ex.GetVariable()->GetFlag());
			
			// add all the messages reported with the AbsVariable
			StringVector::iterator iterator;
			for(iterator = ex.GetVariable()->GetMessages()->begin(); iterator != ex.GetVariable()->GetMessages()->end(); iterator++) {
				collectedObject->AppendOvalMessage(new OvalMessage((*iterator)));
			}
			
		} catch(Exception ex) {
			Log::Info("AbsObjectCollector::Run() - Error while parsing object: " + objectId + " " + ex.GetErrorMessage());
			collectedObject = CollectedObject::CreateError(objectId, version);
			collectedObject->AppendOvalMessage(new OvalMessage("AbsObjectCollector::Run() - Error while parsing object: " + objectId + " " + ex.GetErrorMessage()));
		} catch(...) {
			Log::Info("AbsObjectCollector::Run() - Error while parsing object: " + objectId + " Unkown error.");
			collectedObject = CollectedObject::CreateError(objectId, version);
			collectedObject->AppendOvalMessage(new OvalMessage("AbsObjectCollector::Run() - Error while parsing object: " + objectId + " Unkown error."));
		}


		//
		// This second check to see if collectedObject is NULL is here
		// to allow the code to catch errors in loading the object and report 
		// those errors as a collected object in the sc file
		//
		if(collectedObject == NULL) {
	
			try {

				// Process the object
				collectedObject = this->Process(absObject);

			} catch(Exception ex) {
				if(collectedObject == NULL) 
					collectedObject = CollectedObject::CreateError(absObject);

				collectedObject->AppendOvalMessage(new OvalMessage(ex.GetErrorMessage(), OvalEnum::LEVEL_FATAL));
				collectedObject->SetFlag(OvalEnum::FLAG_ERROR);
				if(absObject != NULL) {
					delete absObject;
					absObject = NULL;
				}
				Log::Debug("Error while collecting data for object: " + collectedObject->GetId() + " " + ex.GetErrorMessage());
			} catch(...) {
				if(collectedObject == NULL) 
					collectedObject = CollectedObject::CreateError(absObject);

				collectedObject->AppendOvalMessage(new OvalMessage("An unknown error occured while collecting data." , OvalEnum::LEVEL_FATAL));
				if(absObject != NULL) {
					delete absObject;
					absObject = NULL;
				}
				Log::Debug("An unknown error occured while collecting data for object: " + collectedObject->GetId());
			} 				
		}
        if(absObject != NULL)
            delete absObject;
	}

	return collectedObject;
}

// ***************************************************************************************	//
//								Private members												//
// ***************************************************************************************	//
void AbsObjectCollector::ApplyFilters(ItemVector* items, FilterVector* filters) {
	for(FilterVector::iterator filterIterator = filters->begin();
		filterIterator != filters->end(); ++filterIterator)
		items->erase(
			remove_if(items->begin(), items->end(), FilterFunctor(*filterIterator)),
			items->end());
}

bool AbsObjectCollector::ExistsInSet(ItemVector* itemSet, Item* item) {

	bool exists = false;
	
	for(ItemVector::iterator iterator = itemSet->begin(); iterator != itemSet->end(); iterator++) {
		if(item->GetId() == (*iterator)->GetId()) {
			exists = true;
			break;
		}
	}	

	return exists;
}
CollectedSet* AbsObjectCollector::Union(CollectedSet* collectedSet1, CollectedSet* collectedSet2) {
	
	ItemVector* resultItems = new ItemVector();

	ItemVector* itemSet1 = collectedSet1->GetItems();
	ItemVector* itemSet2 = collectedSet2->GetItems();

	ItemVector::iterator iterator;
	for(iterator = itemSet1->begin(); iterator != itemSet1->end(); iterator++) {
		if(!this->ExistsInSet(resultItems, (*iterator))) {
			resultItems->push_back((*iterator));
		}
	}

	for(iterator = itemSet2->begin(); iterator != itemSet2->end(); iterator++) {
		if(!this->ExistsInSet(resultItems, (*iterator))) {
			resultItems->push_back((*iterator));
		}
	}
	
	CollectedSet* result = new CollectedSet();
	result->SetItems(resultItems);

	return result;
}

CollectedSet* AbsObjectCollector::Intersection(CollectedSet* collectedSet1, CollectedSet* collectedSet2) {

	ItemVector* resultItems = new ItemVector();

	ItemVector* itemSet1 = collectedSet1->GetItems();
	ItemVector* itemSet2 = collectedSet2->GetItems();

	// Add the items from set 1 that exist in set 2
	for(ItemVector::iterator iterator = itemSet1->begin(); iterator != itemSet1->end(); iterator++) {
		if(this->ExistsInSet(itemSet2, (*iterator))) {
			resultItems->push_back((*iterator));
		}
	}

	CollectedSet* result = new CollectedSet();
	result->SetItems(resultItems);

	return result;
}

CollectedSet* AbsObjectCollector::Compelement(CollectedSet* collectedSet1, CollectedSet* collectedSet2) {

	ItemVector* resultItems = new ItemVector();

	ItemVector* itemSet1 = collectedSet1->GetItems();
	ItemVector* itemSet2 = collectedSet2->GetItems();

	ItemVector::iterator iterator;
	for(iterator = itemSet1->begin(); iterator != itemSet1->end(); iterator++) {
		if(!this->ExistsInSet(itemSet2, (*iterator))) {
			resultItems->push_back((*iterator));
		}
	}

	CollectedSet* result = new CollectedSet();
	result->SetItems(resultItems);

	return result;
}

CollectedObject* AbsObjectCollector::Process(AbsObject* absObject) {

	// Based on the type of object call the appropriate process method.
	CollectedObject* collectedObject = NULL;
	if(typeid(*absObject) == typeid(Object)) {
		collectedObject = this->ProcessObject((Object*)absObject);
	} else {
		collectedObject = this->ProcessSetObject((SetObject*)absObject);
	}

	return collectedObject;
}

CollectedObject* AbsObjectCollector::ProcessSetObject(SetObject* setObject) {

	CollectedSet* collectedSet = this->ProcessSet(setObject->GetSet());
	CollectedObject* collectedObject = CollectedObject::Create(setObject);	
	collectedObject->SetReferences(collectedSet->GetItems());
	collectedObject->SetFlag(collectedSet->GetFlag());
	collectedObject->AppendVariableValues(collectedSet->GetVariableValues());
    delete collectedSet;
	return collectedObject;
}

CollectedSet* AbsObjectCollector::ProcessSet(Set* set) {

	CollectedSet* collectedSet1 = NULL;
	CollectedSet* collectedSet2 = NULL;

	if(!set->GetIsSimpleSet()) {		

		// make recursive call for each child set.
		if(set->GetSetOne() != NULL) {
			collectedSet1 = this->ProcessSet(set->GetSetOne());
		}

		if(set->GetSetTwo() != NULL) {
			collectedSet2 = this->ProcessSet(set->GetSetTwo());
		}

	} else {

		// Collect each referenced object using any supplied filters
		if(set->GetReferenceOne() != NULL) {
			collectedSet1 = new CollectedSet();
			CollectedObject* refOneCollectedObj = this->Run(set->GetReferenceOne()->GetId());
			ItemVector* itemSet1 = refOneCollectedObj->GetReferences();
			ItemVector filteredItems(*itemSet1);
			this->ApplyFilters(&filteredItems, set->GetFilters());
			VariableValueVector* set1Vars = refOneCollectedObj->GetVariableValues();
			collectedSet1->AppendVariableValues(set1Vars);
			collectedSet1->SetFlag(refOneCollectedObj->GetFlag());
			collectedSet1->SetItems(&filteredItems);
		}

		if(set->GetReferenceTwo() != NULL) {
			collectedSet2 = new CollectedSet();
			CollectedObject* refTwoCollectedObj = this->Run(set->GetReferenceTwo()->GetId());
			ItemVector* itemSet2 = refTwoCollectedObj->GetReferences();
			ItemVector filteredItems(*itemSet2);
			this->ApplyFilters(&filteredItems, set->GetFilters());
			VariableValueVector* set2Vars = refTwoCollectedObj->GetVariableValues();
			collectedSet2->AppendVariableValues(set2Vars);
			collectedSet2->SetFlag(refTwoCollectedObj->GetFlag());
			collectedSet2->SetItems(&filteredItems);
		}
	}

	// combine all sets by operator
	CollectedSet* combinedCollectedSet = NULL;
	if(collectedSet2 != NULL) {
		if(set->GetSetOperator() == OvalEnum::SET_OPERATOR_UNION) {
			combinedCollectedSet = this->Union(collectedSet1, collectedSet2);
		} else if(set->GetSetOperator() == OvalEnum::SET_OPERATOR_COMPLEMENT) {
			combinedCollectedSet = this->Compelement(collectedSet1, collectedSet2);
		} else if(set->GetSetOperator() == OvalEnum::SET_OPERATOR_INTERSECTION) {
			combinedCollectedSet = this->Intersection(collectedSet1, collectedSet2);
		}

		// determine flag value
		OvalEnum::Flag resultFlag = this->CombineFlagBySetOperator(set->GetSetOperator(), collectedSet1->GetFlag(), collectedSet2->GetFlag());
		combinedCollectedSet->SetFlag(resultFlag);

		// maintain variable values used.
		combinedCollectedSet->AppendVariableValues(collectedSet1->GetVariableValues());
		combinedCollectedSet->AppendVariableValues(collectedSet2->GetVariableValues());

	} else {
		combinedCollectedSet = collectedSet1;
	}

    /**
        If the reult flag is OvalEnum::FLAG_COMPLETE it is possible that through the set operations
        no matching items were found. If this occurs then we need to set the result flag to 
        OvalEnum::FLAG_DOES_NOT_EXIST.
    */
    if(combinedCollectedSet->GetFlag() == OvalEnum::FLAG_COMPLETE && combinedCollectedSet->GetItems()->size() == 0)
        combinedCollectedSet->SetFlag(OvalEnum::FLAG_DOES_NOT_EXIST);

	return combinedCollectedSet;
}

CollectedObject* AbsObjectCollector::ProcessObject(Object* object) {

	CollectedObject* collectedObject = NULL;

	// Handle objects that are not applicable 
	if(this->IsApplicable(object) == false) {
		collectedObject = CollectedObject::CreateNotApplicable(object);
		collectedObject->SetVariableValues(object->GetVariableValues());
	} else {

		// handle objects that are not supported
		if(this->IsSupported(object) == false) {
			collectedObject = CollectedObject::CreateNotSupported(object);
			collectedObject->SetVariableValues(object->GetVariableValues());
		} else {

			ItemVector* items = NULL;
			AbsProbe* probe = this->GetProbe(object);
			if(probe != NULL) {
				items = probe->Run(object);

				// only create collected object if the probe succeeds
				collectedObject = CollectedObject::Create(object);
				collectedObject->AppendVariableValues(object->GetVariableValues());
				collectedObject->AppendReferencesAndComputeFlag(items);
			} else {
				
				// because we first check if the object is supported the code should never get here.
				throw AbsObjectCollectorException("Error: Unable to locate a probe for the specified object.", ERROR_FATAL);
			}
		}
	}

	return collectedObject;
}

OvalEnum::Flag AbsObjectCollector::CombineFlagBySetOperator(OvalEnum::SetOperator setOp, OvalEnum::Flag set1Flag, OvalEnum::Flag set2Flag) {

	OvalEnum::Flag result = OvalEnum::FLAG_ERROR;

	if(setOp == OvalEnum::SET_OPERATOR_COMPLEMENT) {

		if(set1Flag == OvalEnum::FLAG_ERROR) {
			
			result = OvalEnum::FLAG_ERROR;

		} else if(set1Flag == OvalEnum::FLAG_COMPLETE) {

			if(set2Flag == OvalEnum::FLAG_NOT_APPLICABLE || set2Flag == OvalEnum::FLAG_ERROR) {
				result = OvalEnum::FLAG_ERROR;
			} else if(set2Flag == OvalEnum::FLAG_COMPLETE || set2Flag == OvalEnum::FLAG_DOES_NOT_EXIST) {
				result = OvalEnum::FLAG_COMPLETE;
			} else if(set2Flag == OvalEnum::FLAG_INCOMPLETE) {
				// i differ here i think this is error drew thinks this is incomplete
				result = OvalEnum::FLAG_ERROR;
			} else if(set2Flag == OvalEnum::FLAG_NOT_COLLECTED) {
				// i differ here i think this is error drew thinks this is not collected
				result = OvalEnum::FLAG_ERROR;
			}

		} else if(set1Flag == OvalEnum::FLAG_INCOMPLETE) {

			if(set2Flag == OvalEnum::FLAG_NOT_APPLICABLE || set2Flag == OvalEnum::FLAG_ERROR) {
				result = OvalEnum::FLAG_ERROR;
			} else if(set2Flag == OvalEnum::FLAG_DOES_NOT_EXIST) {
				result = OvalEnum::FLAG_INCOMPLETE;
			} else if(set2Flag == OvalEnum::FLAG_NOT_COLLECTED) {
				// i differ here i think this is error drew thinks this is not collected
				result = OvalEnum::FLAG_ERROR;
			} else if(set2Flag == OvalEnum::FLAG_INCOMPLETE) {
				// i differ here i think this is error drew thinks this is incomplete
				result = OvalEnum::FLAG_ERROR;
			} else if(set2Flag == OvalEnum::FLAG_COMPLETE) {
				result = OvalEnum::FLAG_INCOMPLETE;
			}

		} else if(set1Flag == OvalEnum::FLAG_DOES_NOT_EXIST) {

			if(set2Flag != OvalEnum::FLAG_NOT_APPLICABLE) {
				result = OvalEnum::FLAG_DOES_NOT_EXIST;	
			} else {
				result = OvalEnum::FLAG_ERROR;
			}

		} else if(set1Flag == OvalEnum::FLAG_NOT_COLLECTED) {
			
			if(set2Flag != OvalEnum::FLAG_NOT_APPLICABLE && set2Flag != OvalEnum::FLAG_ERROR) {
				result = OvalEnum::FLAG_NOT_COLLECTED;	
			} else {
				result = OvalEnum::FLAG_ERROR;
			}

		} else if(set1Flag == OvalEnum::FLAG_NOT_APPLICABLE) {
			result = OvalEnum::FLAG_ERROR;
		}

	} else if(setOp == OvalEnum::SET_OPERATOR_INTERSECTION) {

		if(set1Flag == OvalEnum::FLAG_ERROR) {
			result = OvalEnum::FLAG_ERROR;
		} else if(set1Flag == OvalEnum::FLAG_COMPLETE) {

			if(set2Flag == OvalEnum::FLAG_ERROR) {
				result = OvalEnum::FLAG_ERROR;
			} else if(set2Flag == OvalEnum::FLAG_DOES_NOT_EXIST) {
				result = OvalEnum::FLAG_DOES_NOT_EXIST;
			} else if(set2Flag == OvalEnum::FLAG_COMPLETE || set2Flag == OvalEnum::FLAG_NOT_APPLICABLE) {
				result = OvalEnum::FLAG_COMPLETE;
			} else if(set2Flag == OvalEnum::FLAG_INCOMPLETE) {
				result = OvalEnum::FLAG_INCOMPLETE;
			} else if(set2Flag == OvalEnum::FLAG_NOT_COLLECTED) {
				result = OvalEnum::FLAG_NOT_COLLECTED;
			}

		} else if(set1Flag == OvalEnum::FLAG_INCOMPLETE) {
			
			if(set2Flag == OvalEnum::FLAG_ERROR) {
				result = OvalEnum::FLAG_ERROR;
			} else if(set2Flag == OvalEnum::FLAG_DOES_NOT_EXIST) {
				result = OvalEnum::FLAG_DOES_NOT_EXIST;
			} else if(set2Flag == OvalEnum::FLAG_NOT_APPLICABLE) {
				result = OvalEnum::FLAG_NOT_APPLICABLE;
			} else if(set2Flag == OvalEnum::FLAG_NOT_COLLECTED || set2Flag == OvalEnum::FLAG_COMPLETE || set2Flag == OvalEnum::FLAG_INCOMPLETE) {
				result = OvalEnum::FLAG_INCOMPLETE;
			}

		} else if(set1Flag == OvalEnum::FLAG_DOES_NOT_EXIST) {
			result = OvalEnum::FLAG_DOES_NOT_EXIST;
		} else if(set1Flag == OvalEnum::FLAG_NOT_COLLECTED) {
			
			if(set2Flag == OvalEnum::FLAG_ERROR) {
				result = OvalEnum::FLAG_ERROR;
			} else if(set2Flag == OvalEnum::FLAG_DOES_NOT_EXIST) {
				result = OvalEnum::FLAG_DOES_NOT_EXIST;
			} else if(set2Flag == OvalEnum::FLAG_COMPLETE || set2Flag == OvalEnum::FLAG_INCOMPLETE || set2Flag == OvalEnum::FLAG_NOT_APPLICABLE || set2Flag == OvalEnum::FLAG_NOT_COLLECTED) {
				result = OvalEnum::FLAG_NOT_COLLECTED;
			}

		} else if(set1Flag == OvalEnum::FLAG_NOT_APPLICABLE) {

			if(set2Flag == OvalEnum::FLAG_ERROR) {
				result = OvalEnum::FLAG_ERROR;
			} else if(set2Flag == OvalEnum::FLAG_DOES_NOT_EXIST) {
				result = OvalEnum::FLAG_DOES_NOT_EXIST;
			} else if(set2Flag == OvalEnum::FLAG_COMPLETE) {
				result = OvalEnum::FLAG_COMPLETE;
			} else if(set2Flag == OvalEnum::FLAG_INCOMPLETE) {
				result = OvalEnum::FLAG_INCOMPLETE;
			} else if(set2Flag == OvalEnum::FLAG_NOT_APPLICABLE) {
				result = OvalEnum::FLAG_NOT_APPLICABLE;
			} else if(set2Flag == OvalEnum::FLAG_NOT_COLLECTED) {
				result = OvalEnum::FLAG_NOT_COLLECTED;
			}
		}

	} else if(setOp == OvalEnum::SET_OPERATOR_UNION) {
		
		if(set1Flag == OvalEnum::FLAG_ERROR) {

			result = OvalEnum::FLAG_ERROR;

		} else if(set1Flag == OvalEnum::FLAG_COMPLETE) {

			if(set2Flag == OvalEnum::FLAG_ERROR) {
				result = OvalEnum::FLAG_ERROR;	
			} else if(set2Flag == OvalEnum::FLAG_COMPLETE || set2Flag == OvalEnum::FLAG_DOES_NOT_EXIST || set2Flag == OvalEnum::FLAG_NOT_APPLICABLE) {
				result = OvalEnum::FLAG_COMPLETE;
			} else if(set2Flag == OvalEnum::FLAG_INCOMPLETE || set2Flag == OvalEnum::FLAG_NOT_COLLECTED) {
				result = OvalEnum::FLAG_INCOMPLETE;
			}

		} else if(set1Flag == OvalEnum::FLAG_INCOMPLETE) {
			
			if(set2Flag != OvalEnum::FLAG_ERROR) {
				result = OvalEnum::FLAG_INCOMPLETE;	
			} else {
				result = OvalEnum::FLAG_ERROR;
			}

		} else if(set1Flag == OvalEnum::FLAG_DOES_NOT_EXIST) {
			
			if(set2Flag == OvalEnum::FLAG_ERROR) {
				result = OvalEnum::FLAG_ERROR;	
			} else if(set2Flag == OvalEnum::FLAG_COMPLETE) {
				result = OvalEnum::FLAG_COMPLETE;
			} else if(set2Flag == OvalEnum::FLAG_INCOMPLETE || set2Flag == OvalEnum::FLAG_NOT_COLLECTED) {
				result = OvalEnum::FLAG_INCOMPLETE;
			} else if(set2Flag == OvalEnum::FLAG_NOT_APPLICABLE || set2Flag == OvalEnum::FLAG_DOES_NOT_EXIST) {
				result = OvalEnum::FLAG_DOES_NOT_EXIST;
			}

		} else if(set1Flag == OvalEnum::FLAG_NOT_COLLECTED) {
			
			if(set2Flag == OvalEnum::FLAG_ERROR) {
				result = OvalEnum::FLAG_ERROR;	
			} else if(set2Flag == OvalEnum::FLAG_COMPLETE || set2Flag == OvalEnum::FLAG_INCOMPLETE || set2Flag == OvalEnum::FLAG_DOES_NOT_EXIST) {
				result = OvalEnum::FLAG_INCOMPLETE;
			} else if(set2Flag == OvalEnum::FLAG_NOT_APPLICABLE || set2Flag == OvalEnum::FLAG_NOT_COLLECTED) {
				result = OvalEnum::FLAG_NOT_COLLECTED;
			}

		} else if(set1Flag == OvalEnum::FLAG_NOT_APPLICABLE) {

			if(set2Flag == OvalEnum::FLAG_ERROR) {
				result = OvalEnum::FLAG_ERROR;	
			} else if(set2Flag == OvalEnum::FLAG_COMPLETE) {
				result = OvalEnum::FLAG_COMPLETE;
			} else if(set2Flag == OvalEnum::FLAG_INCOMPLETE) {
				result = OvalEnum::FLAG_INCOMPLETE;
			} else if(set2Flag == OvalEnum::FLAG_NOT_APPLICABLE) {
				result = OvalEnum::FLAG_NOT_APPLICABLE;
			} else if(set2Flag == OvalEnum::FLAG_DOES_NOT_EXIST) {
				result = OvalEnum::FLAG_DOES_NOT_EXIST;
			} else if(set2Flag == OvalEnum::FLAG_NOT_COLLECTED) {
				result = OvalEnum::FLAG_NOT_COLLECTED;
			}

		}

	}

	return result;
}

//****************************************************************************************//
//						AbsObjectCollectorException Class								  //	
//****************************************************************************************//
AbsObjectCollectorException::AbsObjectCollectorException(string errMsgIn, int severity, Exception* ex) : Exception(errMsgIn, severity, ex) {

}

AbsObjectCollectorException::~AbsObjectCollectorException() {

}
