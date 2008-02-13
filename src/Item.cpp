//
// $Id: Item.cpp 4597 2008-01-03 16:45:30Z bakerj $
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

#include "Item.h"

int Item::idCounter = 1;
ItemMap Item::processedItemsMap;

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
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the elements field's value
	//
	// -----------------------------------------------------------------------

	return &this->elements;
}

void Item::SetElements(ItemEntityVector* elements) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the elements field's value
	//
	// -----------------------------------------------------------------------

	this->elements = (*elements);
}

int Item::GetId() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the id field's value
	//
	// -----------------------------------------------------------------------

	return this->id;
}

void Item::SetId(int id) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the id field's value
	//
	// -----------------------------------------------------------------------

	this->id = id;
}

void Item::SetIsWritten(bool isWritten) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the isWritten field's value
	//
	// -----------------------------------------------------------------------

	this->isWritten = isWritten;
}

bool Item::GetIsWritten() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Get the isWritten field's value
	//
	// -----------------------------------------------------------------------

	return this->isWritten;
}


OvalMessageVector* Item::GetMessages() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Get the messages field's value
	//
	// -----------------------------------------------------------------------
	return &this->messages;
}
void Item::SetMessages(OvalMessageVector* messages) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the messages field's value
	//
	// -----------------------------------------------------------------------
	
	this->messages = (*messages);
}

string Item::GetName() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the name field's value
	//
	// -----------------------------------------------------------------------

	return this->name;
}

void Item::SetName(string name) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the name field's value
	//
	// -----------------------------------------------------------------------

	this->name = name;
}

OvalEnum::SCStatus Item::GetStatus() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the status field's value
	//
	// -----------------------------------------------------------------------

	return this->status;
}

void Item::SetStatus(OvalEnum::SCStatus status) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the scStatus field's value
	//
	// -----------------------------------------------------------------------

	this->status = status;
}

string Item::GetXmlns() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the xmlns field's value
	//
	// -----------------------------------------------------------------------

	return this->xmlns;
}

void Item::SetXmlns(string xmlns) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the xmlns field's value
	//
	// -----------------------------------------------------------------------

	this->xmlns = xmlns;
}

string Item::GetXmlnsAlias() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the xmlnsAlias field's value
	//
	// -----------------------------------------------------------------------

	return this->xmlnsAlias;
}

void Item::SetXmlnsAlias(string xmlnsAlias) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the xmlnsAlias field's value
	//
	// -----------------------------------------------------------------------

	this->xmlnsAlias = xmlnsAlias;
}

string Item::GetSchemaLocation() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the schemaLocation field's value
	//
	// -----------------------------------------------------------------------

	return this->schemaLocation;
}

void Item::SetSchemaLocation(string schemaLocation) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the schemaLocation field's value
	//
	// -----------------------------------------------------------------------

	this->schemaLocation = schemaLocation;
}

void Item::AppendElement(ItemEntity* itemEntity) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Add an element to the end of the elements vector
	//
	// -----------------------------------------------------------------------

	this->elements.push_back(itemEntity);
}

void Item::AppendMessage(OvalMessage* msg) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Add an message to the end of the messages vector
	//
	// -----------------------------------------------------------------------

	this->messages.push_back(msg);
}

bool Item::Equals(Item* item) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return true if this item is equal to the specified item.
	//
	//	NOTE: id and status are not compared
	//	NOTE: Assumes that an object element will only ever occure once.
	//	NOTE: Looks at only object elements
	// -----------------------------------------------------------------------
	bool isEqual = false;

	try {

		// compare name
		if(this->GetName().compare(item->GetName()) == 0) {
			// compare xmlns
			if(this->GetXmlns().compare(item->GetXmlns()) == 0) {
				// get the object elements for each item
				ItemEntityVector* myObjElms = this->GetObjectElements();
				ItemEntityVector* itemObjElms = item->GetObjectElements();
				
				// both the same number of object elements keep checking otherwise not equal
				if(myObjElms->size() == itemObjElms->size()) {

					// compare all object elements
					unsigned int myElementCount = myObjElms->size();
					unsigned int i = 0;
					bool finished = false;
					while(i < myElementCount && !finished) {
						ItemEntity* myElement = (ItemEntity*)myObjElms->at(i++);
						
						string elmName = myElement->GetName();
						// loop over itemObjElms to find the element with the same name
						ItemEntityVector::iterator it;
						for(it = itemObjElms->begin(); it != itemObjElms->end(); it++) {
							if((*it)->GetName().compare(elmName) == 0) {
								if(myElement->Equals((*it))) {
									isEqual = true;
									finished = false;
									break;
								} else {
									isEqual = false;
									finished = true;
									break;
								}
							}
						}
					}

				} else {
					isEqual = false;
				}

				delete myObjElms;
				delete itemObjElms;
			} 
		}
	} catch (Exception ex) {
		throw Exception("Error: An error occured with comparing two Items", ERROR_FATAL, &ex);
	} catch(...) {
		throw Exception("Error: An unknown error occured with comparing two Items", ERROR_FATAL);
	}

	return isEqual;
}

ItemEntityVector* Item::GetObjectElements() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return all object elements
	// -----------------------------------------------------------------------

	ItemEntityVector* objElms = new ItemEntityVector();

	ItemEntityVector::iterator iterator;
	for(iterator = this->GetElements()->begin(); iterator != this->GetElements()->end(); iterator++) {
		if((*iterator)->GetIsObjectEntity()) {
			objElms->push_back((*iterator));   
		}
	}

	return objElms;
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

void Item::Parse(DOMElement* scItemElm) {

	this->SetName(XmlCommon::GetElementName(scItemElm));
	this->SetId(atoi((XmlCommon::GetAttributeByName(scItemElm, "id")).c_str()));
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

void Item::Write(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* scFile, DOMElement* itemsElm) {

	if(!this->GetIsWritten()) {
		this->SetIsWritten(true);
		//this->SetId(Item::AssignId());

		// add the namespace and schema location to the sc document
		XmlCommon::AddXmlns(DocumentManager::GetSystemCharacterisitcsDocument(), this->GetXmlns(), this->GetXmlnsAlias());
		XmlCommon::AddSchemaLocation(DocumentManager::GetSystemCharacterisitcsDocument(), this->GetSchemaLocation());

		// Create a new item element	
		DOMElement *newItemElem = scFile->createElement(XMLString::transcode(this->GetName().c_str()));
		itemsElm->appendChild(newItemElem);

		// Add the attributes
		// handling defaults in the schema
		XmlCommon::AddAttribute(newItemElem, "id", Common::ToString(this->GetId()));	

		string strStatus = OvalEnum::SCStatusToString(this->GetStatus());
		if(strStatus.compare("exists") != 0)
			XmlCommon::AddAttribute(newItemElem, "status", strStatus);

		XmlCommon::AddAttribute(newItemElem, "xmlns", this->GetXmlns());	

		// Add any messges
		if(this->GetMessages()->size() > 0) {
			string msgStr = "";
			OvalMessageVector::iterator messageIterator;
			for(messageIterator = this->GetMessages()->begin(); messageIterator != this->GetMessages()->end(); messageIterator++) {
				OvalMessage* message = (*messageIterator);
				//message->Write(scFile, newItemElem, "oval-sc");
				msgStr.append(message->ToString() + "\n");
			}
				XmlCommon::AddChildElement(scFile, newItemElem, "oval-sc:message", msgStr);
		}

		// Call the write method for each element
		ItemEntityVector::iterator elementIterator;
		for(elementIterator = this->GetElements()->begin(); elementIterator != this->GetElements()->end(); elementIterator++) {
			ItemEntity* element = (ItemEntity*)(*elementIterator);
			element->Write(scFile, newItemElem);
		}
	}
}

Item* Item::GetItemById(string itemId) {

	Item* item = NULL;
	
	// Search the cache
	item = Item::SearchCache(atoi(itemId.c_str()));

	// if not found try to parse it.
	if(item == NULL) {

		DOMElement* systemDataElm = XmlCommon::FindElement(DocumentManager::GetSystemCharacterisitcsDocument(), "system_data");
		DOMElement* itemElm = XmlCommon::FindElementByAttribute(systemDataElm, "id", itemId);

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

//****************************************************************************************//
//								ItemException Class										  //	
//****************************************************************************************//
ItemException::ItemException(string errMsgIn, int severity, Exception* ex) : Exception(errMsgIn, severity, ex) {

}

ItemException::~ItemException() {

}
