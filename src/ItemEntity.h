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

#ifndef ITEMENTITY_H
#define ITEMENTITY_H

#include <string>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMDocument.hpp>

#include "OvalEnum.h"
#include "AbsEntityValue.h"

/**
	This class represents an entity in an Item as defined in the oval system characteristics schema.
*/
class ItemEntity {
public:

	/** Create a complete ItemEntity object. */
	ItemEntity(std::string name = "", std::string value = "", OvalEnum::Datatype datatype = OvalEnum::DATATYPE_STRING, OvalEnum::SCStatus status = OvalEnum::STATUS_EXISTS, bool isNil = false);

	/**
	 * Create a complete ItemEntity object.  This ItemEntity takes ownership of
	 * the given entity values, so don't delete them yourself.
	 */
	ItemEntity(std::string name, AbsEntityValueVector value, OvalEnum::Datatype datatype = OvalEnum::DATATYPE_RECORD, OvalEnum::SCStatus status = OvalEnum::STATUS_EXISTS, bool isNil = false);

    /** ItemEntity copy constructor. */
    ItemEntity(const ItemEntity& itemEntity);

    /** ItemEntity destructor. */
    ~ItemEntity();

	/** Write this ItemEntity to the sc file.
	    Inserts this ItemEntity as the last child of the specified
		itemElm.
	*/
	void Write(xercesc::DOMDocument* scFile, xercesc::DOMElement* itemElm, const std::string &ns);

	/** Parse the provided entity element */
	void Parse(xercesc::DOMElement* entityElm);

	/** Return the status field's value. */
	OvalEnum::SCStatus GetStatus() const;

	/** Set the status of the ItemEntity.
	 *  @param status A string value representing the status of the ItemEntity.
	 *  @return Void.
	 */
	void SetStatus(OvalEnum::SCStatus status);

	/** Return the name value of the ItemEntity.
	 *  @return A string representing the name value of the ItemEntity.
	 */
	std::string GetName() const;

	/** Set the name of the ItemEntity.
	 *  @param name A string value representing the name of the ItemEntity.
	 *  @return Void.
	 */
	void SetName(std::string name);

	/** Return the value of the ItemEntity.
	 *  @return A string representing the value of the ItemEntity.
	 */
	std::string GetValue() const;

	/** Set the value of the ItemEntity.
	 *  @param value A string representation of the value of the ItemEntity.
	 *  @return Void.
	 */
	void SetValue(std::string value);

	/** Return the values of the ItemEntity.
  	 *  @return A AbsEntityValueVector containing the values of the ItemEntity.
	 */
	AbsEntityValueVector GetValues() const;

	/** Set the values of the ItemEntity.
	 *  @param values A AbsEntityValueVector that contains the values for the ItemEntity.
	 *  @return Void.
	 */
	void SetValues(AbsEntityValueVector values);

	/**
	 * Get the number of "values" in this item entity.  This is 1
	 * for all non-record item entities, and the number of record
	 * fields for all record item entities.
	 */
	size_t GetNumValues() const { return value.size(); }

	/** Return the datatype of the ItemEntity.
 	 *  @return A value from the OvalEnum::Datatype enumeration representing the datatype of the ItemEntity.
	 */
	OvalEnum::Datatype GetDatatype() const;

	/** Set the datatype of the ItemEntity.
	 *  @param datatype A value from the OvalEnum::Datatype enumeration representing the datatype of the ItemEntity.
	 *  @return Void.
	 */
	void SetDatatype(OvalEnum::Datatype datatype);

	/** Set the nil field's value.
	 *  @param nil The boolean value indicating whether or not the entity has a nil value.
	 *  @return void
	 */
	void SetNil(bool isNil);

	/** Return true if the xsi:nil is set to true.
	 *  @return A boolean value indicating whether or not the entity has a nil value.
	 */
	bool GetNil() const;

private:
	OvalEnum::SCStatus scStatus;
	std::string name;
	AbsEntityValueVector value;
	OvalEnum::Datatype datatype;
	bool nil;
};

/**	
	A vector for storing ItemEntity objects. 
	Stores only pointers to the objects. 
*/
typedef std::vector < ItemEntity* > ItemEntityVector;

#endif
