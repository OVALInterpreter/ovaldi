//
// $Id: ItemEntity.h 4579 2008-01-02 17:39:07Z bakerj $
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

#ifndef ITEMENTITY_H
#define ITEMENTITY_H

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMAttr.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/parsers/AbstractDOMParser.hpp>

#include <string>
#include <iostream>
#include <stdlib.h>

#include "Log.h"
#include "OvalEnum.h"
#include "XmlCommon.h"
#include "Exception.h"

XERCES_CPP_NAMESPACE_USE
using namespace std;

/**
	This class represents an entity in an Item as defined in the oval system characteristics schema.
*/
class ItemEntity {
public:

	/** Create a complete ItemEntity object. */
	ItemEntity(string name = "", string value = "", OvalEnum::Datatype datatype = OvalEnum::DATATYPE_STRING, bool isObjectEntity = false, OvalEnum::SCStatus status = OvalEnum::STATUS_EXISTS);
	~ItemEntity();

	/** Return true if this ItemEntity is equal to the provided ItemEntity
		Note: Status is not compared.
	*/
	bool Equals(ItemEntity* entity);

	/** Write this ItemEntity to the sc file.
	    Inserts this ItemEntity as the last child of the specified
		itemElm.
	*/
	void Write(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* scFile, DOMElement* itemElm);

	/** Parse the provided entity element */
	void Parse(DOMElement* entityElm);

	/** Return the status field's value. */
	OvalEnum::SCStatus GetStatus();

	/** Set the scStatus field's value. */
	void SetStatus(OvalEnum::SCStatus status);

	/** Return the name field's value. */
	string GetName();

	/** Set the name field's value. */
	void SetName(string name);

	/** Return the value field's value. */
	string GetValue();

	/** Set the value field's value. */
	void SetValue(string value);

	/** Get the datatype field's value. */
	OvalEnum::Datatype GetDatatype();

	/** Set the datatype field's value. */
	void SetDatatype(OvalEnum::Datatype datatype);

	/** Get the isObjectEntity field's value. */
	bool GetIsObjectEntity();

	/** Set the isObjectEntity field's value. */
	void SetIsObjectEntity(bool isObjectEntity);

private:
	OvalEnum::SCStatus scStatus;
	string name;
	string value;
	OvalEnum::Datatype datatype;
	bool isObjectEntity;
};

/**	
	A vector for storing ItemEntity objects. 
	Stores only pointers to the objects. 
*/
typedef vector < ItemEntity*, allocator<ItemEntity*> > ItemEntityVector;

/** 
	This class represents an Exception that occured while processing an ItemEntity.
*/
class ItemEntityException : public Exception {
	public:
		ItemEntityException(string errMsgIn = "", int severity = ERROR_FATAL, Exception *ex = NULL);
		~ItemEntityException();
};

#endif
