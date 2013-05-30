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

#include <cassert>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMNodeList.hpp>

#include "XmlCommon.h"
#include "Common.h"
#include "DocumentManager.h"
#include <StringEntityValue.h>
#include <ItemFieldEntityValue.h>

#include "Item.h"

using namespace std;
using namespace xercesc;

int Item::idCounter = 1;
ItemMap Item::processedItemsMap;

namespace {

	/**
	 * Defines a total order on entity values.  This function is an
	 * implementation detail of std::less<Item*> and assumes that
	 * \p left and \p right are of the same subtype (StringEntityValue*
	 * or ItemFieldEntityValue*).
	 *
	 * \return true if \p left is "less than" \p right, false otherwise.
	 */
	bool entityValueLessThan(const AbsEntityValue *left, const AbsEntityValue *right) {

		/*
		This is a simple string check for string entity values.
		For fields of records, we have the following structure:
			datatype, status, name, value
		the "mask" attr is not supported by ovaldi.  I put
		the quick things to check first and name/value last.
		I hope this speeds things up.
		*/

		// Afaik, these are the only two subclasses of AbsEntityValue which can
		// be used in an ItemEntity.
		const StringEntityValue *lsv, *rsv;
		const ItemFieldEntityValue *liv, *riv;

		lsv = dynamic_cast<const StringEntityValue*>(left);
		liv = dynamic_cast<const ItemFieldEntityValue*>(left);
		rsv = dynamic_cast<const StringEntityValue*>(right);
		riv = dynamic_cast<const ItemFieldEntityValue*>(right);

		assert((lsv&&rsv)||(liv&&riv));

		if (lsv) {
			// plain strings
			int comp = lsv->GetValue().compare(rsv->GetValue());
			if (comp < 0)
				return true;
			return false;
		} else {
			// record fields
			if (liv->GetDatatype() < riv->GetDatatype())
				return true;
			else if (liv->GetDatatype() > riv->GetDatatype())
				return false;

			if (liv->GetStatus() < riv->GetStatus())
				return true;
			else if (liv->GetStatus() > riv->GetStatus())
				return false;

			int comp = liv->GetName().compare(riv->GetName());
			if (comp < 0)
				return true;
			else if (comp > 0)
				return false;

			comp = liv->GetValue().compare(riv->GetValue());
			if (comp < 0)
				return true;
			return false;
		}
	}

	/**
	 * Defines a total order on item entities.
	 *
	 * \return true if \p left is "less than" \p right, false otherwise.
	 */
	bool itemEntityLessThan(const ItemEntity *left, const ItemEntity *right) {
			
		/*
		General ItemEntity structure:
			datatype, status, nil, name, value(s)
		the "mask" attr is not supported by ovaldi.  I put
		the quick things to check first and name/values last.
		I hope this speeds things up.

		Record-type item entities may have more than one
		value; non-records have no more than one.  In fact,
		I think records can also have zero values and non-records
		must have exactly one??  It is confusing...

		*/

		// first check overall "length".  The only variability
		// is in the values, so just check that.
		size_t fieldsLeft = left->GetNumValues();
		size_t fieldsRight = right->GetNumValues();
		if (fieldsLeft < fieldsRight)
			return true;
		else if (fieldsLeft > fieldsRight)
			return false;

		if (left->GetDatatype() < right->GetDatatype())
			return true;
		else if (left->GetDatatype() > right->GetDatatype())
			return false;

		if (left->GetStatus() < right->GetStatus())
			return true;
		else if (left->GetStatus() > right->GetStatus())
			return false;

		// treat bools like ints
		if (left->GetNil() < right->GetNil())
			return true;
		else if (left->GetNil() > right->GetNil())
			return false;

		int comp = left->GetName().compare(right->GetName());
		if (comp < 0)
			return true;
		else if (comp > 0)
			return false;

		// check values
		AbsEntityValueVector leftVals = left->GetValues();
		AbsEntityValueVector rightVals = right->GetValues();
		AbsEntityValueVector::iterator leftIter, rightIter;
		for (leftIter = leftVals.begin(), rightIter = rightVals.begin();
			leftIter != leftVals.end();
			++leftIter, ++rightIter) {

			if (entityValueLessThan(*leftIter, *rightIter))
				return true;
			else if (entityValueLessThan(*rightIter, *leftIter))
				return false;
		}

		return false;
	}
}

namespace std {

	bool less<const Item*>::operator()(const Item *left, const Item *right) {

		/*
		General Item structure:
			status, xml namespace, name, entity(s)
		I put the quick things to check first and name/entities last.
		I hope this speeds things up.
		*/

		// first check overall "length".  The only variability
		// is in the entities, so just check that.
		if (left->GetNumElements() < right->GetNumElements())
			return true;
		else if (left->GetNumElements() > right->GetNumElements())
			return false;

		if (left->GetStatus() < right->GetStatus())
			return true;
		else if (left->GetStatus() > right->GetStatus())
			return false;

		int comp = left->GetName().compare(right->GetName());
		if (comp < 0)
			return true;
		else if (comp > 0)
			return false;

		comp = left->GetXmlns().compare(right->GetXmlns());
		if (comp < 0)
			return true;
		else if (comp > 0)
			return false;

		// check entities
		const ItemEntityVector *leftEnts = left->GetElements();
		const ItemEntityVector *rightEnts = right->GetElements();
		ItemEntityVector::const_iterator leftIter, rightIter;
		for (leftIter = leftEnts->begin(), rightIter = rightEnts->begin();
			leftIter != leftEnts->end();
			++leftIter, ++rightIter) {

			if (itemEntityLessThan(*leftIter, *rightIter))
				return true;
			else if (itemEntityLessThan(*rightIter, *leftIter))
				return false;
		}

		return false;
	}

}

//****************************************************************************************//
//									Item Class											  //	
//****************************************************************************************//
Item::Item(int id , string xmlns, string xmlnsAlias, string schemaLocation, OvalEnum::SCStatus status, string name) {

	this->SetId(id);
	this->SetXmlns(xmlns);
	this->SetXmlnsAlias(xmlnsAlias);
	this->SetSchemaLocation(schemaLocation);
	this->SetName(name);
	this->SetStatus(status);
	this->SetIsWritten(false);
}

Item::Item(const Item& item){

    ItemEntityVector iev = item.elements;
    ItemEntityVector::iterator it1;

    for( it1 = iev.begin() ; it1 != iev.end() ; it1++ ){
        ItemEntity* ie = new ItemEntity(**it1);
        this->elements.push_back(ie);
    }
    
    OvalMessageVector mv = item.messages;
    OvalMessageVector::iterator it2;
    
    for( it2 = mv.begin() ; it2 != mv.end() ; it2++ ){
        OvalMessage* m = new OvalMessage(**it2);
        this->messages.push_back(m);
    }

    this->SetId(item.id);
    this->SetName(item.name);
    this->SetStatus(item.status);
    this->SetXmlns(item.xmlns);
    this->SetXmlnsAlias(item.xmlnsAlias);
    this->SetSchemaLocation(item.schemaLocation);
    this->SetIsWritten(item.isWritten);

}

Item::~Item() {
	
	this->DeleteElements();
}

// ***************************************************************************************	//
//								 Static Public members										//
// ***************************************************************************************	//
int Item::AssignId() {

	return Item::idCounter++;
}

// ***************************************************************************************	//
//								 Public members												//
// ***************************************************************************************	//

ItemEntityVector* Item::GetElements() {
	return &this->elements;
}

void Item::SetElements(ItemEntityVector* elements) {
	this->elements = (*elements);
}

int Item::GetId() {
	return this->id;
}

void Item::SetId(int id) {
	this->id = id;
}

void Item::SetIsWritten(bool isWritten) {
	this->isWritten = isWritten;
}

bool Item::GetIsWritten() {
	return this->isWritten;
}


OvalMessageVector* Item::GetMessages() {
	return &this->messages;
}
void Item::SetMessages(OvalMessageVector* messages) {
    this->messages = (*messages);
}

string Item::GetName() const {
	return this->name;
}

void Item::SetName(string name) {
	this->name = name;
}

OvalEnum::SCStatus Item::GetStatus() const {
	return this->status;
}

void Item::SetStatus(OvalEnum::SCStatus status) {
	this->status = status;
}

string Item::GetXmlns() const {
	return this->xmlns;
}

void Item::SetXmlns(string xmlns) {
	this->xmlns = xmlns;
}

string Item::GetXmlnsAlias() const {
	return this->xmlnsAlias;
}

void Item::SetXmlnsAlias(string xmlnsAlias) {
	this->xmlnsAlias = xmlnsAlias;
}

string Item::GetSchemaLocation() {
	return this->schemaLocation;
}

void Item::SetSchemaLocation(string schemaLocation) {
	this->schemaLocation = schemaLocation;
}

void Item::AppendElement(ItemEntity* itemEntity) {
	this->elements.push_back(itemEntity);
}

void Item::AppendMessage(OvalMessage* msg) {
	this->messages.push_back(msg);
}

ItemEntityVector* Item::GetElementsByName(string elementName) {

	ItemEntityVector* matchingElements = new ItemEntityVector();

	ItemEntityVector::iterator iterator;
	for(iterator = this->GetElements()->begin(); iterator != this->GetElements()->end(); iterator++) {
		ItemEntity* element = (ItemEntity*)(*iterator);
		if(element->GetName().compare(elementName) == 0) {
			matchingElements->push_back(element);   
		}
	}

	return matchingElements;
}

ItemEntity* Item::GetElementByName(string itemEntityNameStr) {
    
	ItemEntity *ie = NULL;
	ItemEntityVector::iterator iterator;
	for(iterator = this->GetElements()->begin(); iterator != this->GetElements()->end(); iterator++) {

		ItemEntity* element = *iterator;

		if(element->GetName() == itemEntityNameStr) {
			if (ie) {
				delete ie;
				throw Exception("Error: This Item has contains multiple ItemEntities with the name '"+itemEntityNameStr+"'.");
			} else
				ie = new ItemEntity(*element);
		}
	}
    
	if (ie) return ie;
	return new ItemEntity();
}

void Item::Parse(DOMElement* scItemElm) {

	int id = 0;
	this->SetName(XmlCommon::GetElementName(scItemElm));
	Common::FromString(XmlCommon::GetAttributeByName(scItemElm, "id"), &id);
	this->SetId(id);
	this->SetXmlns(XmlCommon::GetNamespace(scItemElm));
	this->SetStatus(OvalEnum::ToSCStatus(XmlCommon::GetAttributeByName(scItemElm, "status")));
	
	// loop over all elements
	DOMNodeList *itemChildren = scItemElm->getChildNodes();
	unsigned int index = 0;
	while(index < itemChildren->getLength()) {
		DOMNode* tmpNode = itemChildren->item(index);

		//	only concerned with ELEMENT_NODEs
		if (tmpNode->getNodeType() == DOMNode::ELEMENT_NODE) {
			DOMElement* itemChild = (DOMElement*)tmpNode;

			//	get the name of the child
			string childName = XmlCommon::GetElementName(itemChild);
			if(childName.compare("message") == 0) {
				OvalMessage* msg = new OvalMessage();
				msg->Parse(itemChild);
				this->AppendMessage(msg);
			} else {
                ItemEntity* itemEntity = new ItemEntity();
				itemEntity->Parse(itemChild);
				this->AppendElement(itemEntity);
			}
		}
		index ++;
	}	
}

void Item::Write(DOMDocument* scFile, DOMElement* itemsElm) {

	if(!this->GetIsWritten()) {
		this->SetIsWritten(true);
		//this->SetId(Item::AssignId());

		// add the namespace and schema location to the sc document
		DOMDocument *scDoc = DocumentManager::GetSystemCharacteristicsDocument();
		XmlCommon::AddXmlns(scDoc, this->GetXmlns(), this->GetXmlnsAlias());
		XmlCommon::AddSchemaLocation(scDoc, this->GetSchemaLocation());

		// Create a new item element	
		DOMElement *newItemElem = XmlCommon::CreateElementNS(scDoc, this->GetXmlns(), this->GetName());
		//Free memory allocated by XMLString::transcode(char*)
		itemsElm->appendChild(newItemElem);

		// Add the attributes
		// handling defaults in the schema
		XmlCommon::AddAttribute(newItemElem, "id", Common::ToString(this->GetId()));	

		string strStatus = OvalEnum::SCStatusToString(this->GetStatus());
		if(strStatus.compare("exists") != 0)
			XmlCommon::AddAttribute(newItemElem, "status", strStatus);

		// Add any messges.  As of v5.8, no more than 50 are allowed per item.
		if(this->GetMessages()->size() > 0) {
			OvalMessageVector::iterator messageIterator;
			int messageCounter;
			for(messageIterator = this->GetMessages()->begin(), messageCounter = 0;
				messageIterator != this->GetMessages()->end() && messageCounter < 50;
				messageIterator++, messageCounter++) {
				OvalMessage* message = (*messageIterator);
				message->Write(scFile, newItemElem, "oval-sc", XmlCommon::scNS);
			}
		}

		// Call the write method for each element
		ItemEntityVector::iterator elementIterator;
		for(elementIterator = this->GetElements()->begin(); elementIterator != this->GetElements()->end(); elementIterator++) {
			ItemEntity* element = (ItemEntity*)(*elementIterator);
			element->Write(scFile, newItemElem, this->GetXmlns());
		}
	}
}

Item* Item::GetItemById(string itemId) {

	Item* item = NULL;
	
	// Search the cache
	int id;
	if (!Common::FromString(itemId, &id))
		throw Exception("GetItemById: invalid item ID: "+itemId);
	
	item = Item::SearchCache(id);

	// if not found try to parse it.
	if(item == NULL) {

		DOMElement* systemDataElm = XmlCommon::FindElement(DocumentManager::GetSystemCharacteristicsDocument(), "system_data");
		DOMElement* itemElm = XmlCommon::FindElementByAttribute(systemDataElm, "id", itemId); // TODO - is there a faster search method to use???

		if(itemElm == NULL) {
			throw Exception("Unable to find specified item in system-characteristics document. Item id: " + itemId);
		}

		item = new Item();
		item->Parse(itemElm);
		Item::Cache(item);
	}
	
	return item;
}

// ***************************************************************************************	//
//								 Private members											//
// ***************************************************************************************	//
void Item::DeleteElements() {
	ItemEntity* currentElement = NULL;
	while(elements.size() != 0) {
        
	  	currentElement = elements[elements.size()-1];
	  	elements.pop_back();
	  	delete currentElement;
	  	currentElement = NULL;
	}
	OvalMessage* currentMsg = NULL;
	while(messages.size() != 0) {
        
	  	currentMsg = messages[messages.size()-1];
	  	messages.pop_back();
	  	delete currentMsg;
	  	currentMsg = NULL;
	}
}

Item* Item::SearchCache(int id) {

	Item* cachedItem = NULL;

	ItemMap::iterator iterator;
	iterator = Item::processedItemsMap.find(id);
	if(iterator != Item::processedItemsMap.end()) {
		cachedItem = iterator->second;
	} 

	return cachedItem;
}

void Item::ClearCache() {

	ItemMap::iterator iterator;
	for(iterator = Item::processedItemsMap.begin(); iterator != Item::processedItemsMap.end(); iterator++) {
		
		Item* item = iterator->second;
		delete item;
	}
	
	Item::processedItemsMap.clear();
}

void Item::Cache(Item* item) {

	Item::processedItemsMap.insert(ItemPair(item->GetId(), item));
}

