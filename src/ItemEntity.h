//
//
//****************************************************************************************//
// Copyright (c) 2002-2010, The MITRE Corporation
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

#include <string>
#include <iostream>
#include <stdlib.h>

#include "Common.h"
#include "OvalEnum.h"

XERCES_CPP_NAMESPACE_USE

/**
	This class represents an entity in an Item as defined in the oval system characteristics schema.
*/
class ItemEntity {
public:

	/** Create a complete ItemEntity object. */
	ItemEntity(std::string name = "", std::string value = "", OvalEnum::Datatype datatype = OvalEnum::DATATYPE_STRING, bool isObjectEntity = false, OvalEnum::SCStatus status = OvalEnum::STATUS_EXISTS);
	
    /** ItemEntity copy constructor. */
    ItemEntity(const ItemEntity& itemEntity);

    /** ItemEntity destructor. */
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

    /** Create a unique string representation of the ItemEntity.
        A unique string can be created for an ItemEntity by concatenating the results of each of the following:
        <ul>
            <li>getting the ItemEntity name</li>
            <li>getting the ItemEntity value</li>
        </ul>
    */
    std::string UniqueString();

	/** Parse the provided entity element */
	void Parse(DOMElement* entityElm);

	/** Return the status field's value. */
	OvalEnum::SCStatus GetStatus();

	/** Set the scStatus field's value. */
	void SetStatus(OvalEnum::SCStatus status);

	/** Return the name field's value. */
	std::string GetName();

	/** Set the name field's value. */
	void SetName(std::string name);

	/** Return the value field's value. */
	std::string GetValue();

	/** Set the value field's value. */
	void SetValue(std::string value);

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
	std::string name;
	std::string value;
	OvalEnum::Datatype datatype;
	bool isObjectEntity;
};

/**	
	A vector for storing ItemEntity objects. 
	Stores only pointers to the objects. 
*/
typedef std::vector < ItemEntity* > ItemEntityVector;

#endif
