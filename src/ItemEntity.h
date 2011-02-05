//
//
//****************************************************************************************//
// Copyright (c) 2002-2011, The MITRE Corporation
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
#include "AbsEntityValue.h"
#include "StringEntityValue.h"
#include "ItemFieldEntityValue.h"
XERCES_CPP_NAMESPACE_USE

/**
	This class represents an entity in an Item as defined in the oval system characteristics schema.
*/
class ItemEntity {
public:

	/** Create a complete ItemEntity object. */
	ItemEntity(std::string name = "", std::string value = "", OvalEnum::Datatype datatype = OvalEnum::DATATYPE_STRING, bool isObjectEntity = false, OvalEnum::SCStatus status = OvalEnum::STATUS_EXISTS, bool isNil = false);

	/** Create a complete ItemEntity object. */
	ItemEntity(std::string name, AbsEntityValueVector value, OvalEnum::Datatype datatype = OvalEnum::DATATYPE_RECORD, bool isObjectEntity = false, OvalEnum::SCStatus status = OvalEnum::STATUS_EXISTS, bool isNil = false);

    /** ItemEntity copy constructor. */
    ItemEntity(const ItemEntity& itemEntity);

    /** ItemEntity destructor. */
    ~ItemEntity();

	/** Return true if this ItemEntity is equal to the provided ItemEntity. Note: Status is not compared.
		@param entity An ItemEntity* that you would like to compare to this ItemEntity.
		@return A boolean value indicating whether or not the two ItemEntities are equal.
	*/
	bool Equals(ItemEntity* entity);

	/** Return true if the specified entity value exists in the entity value vector.
		@param entityValueVector the entity value vector for which you want to see if the specified entity value exists.
		@param entityValue the entity value whose existence you would like to check in the specified entity value vector.
		@return A boolean value indicating whether or not the specified entity value exists in the specified entity values vector.
	*/
	bool ValueExistsInItemEntity(AbsEntityValueVector entityValueVector, AbsEntityValue* entityValue);

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

	/** Set the status of the ItemEntity.
	 *  @param status A string value representing the status of the ItemEntity.
	 *  @return Void.
	 */
	void SetStatus(OvalEnum::SCStatus status);

	/** Return the name value of the ItemEntity.
	 *  @return A string representing the name value of the ItemEntity.
	 */
	std::string GetName();

	/** Set the name of the ItemEntity.
	 *  @param name A string value representing the name of the ItemEntity.
	 *  @return Void.
	 */
	void SetName(std::string name);

	/** Return the value of the ItemEntity.
	 *  @return A string representing the value of the ItemEntity.
	 */
	std::string GetValue();

	/** Set the value of the ItemEntity.
	 *  @param value A string representation of the value of the ItemEntity.
	 *  @return Void.
	 */
	void SetValue(std::string value);

	/** Return the values of the ItemEntity.
  	 *  @return A AbsEntityValueVector containing the values of the ItemEntity.
	 */
	AbsEntityValueVector GetValues();

	/** Set the values of the ItemEntity.
	 *  @param values A AbsEntityValueVector that contains the values for the ItemEntity.
	 *  @return Void.
	 */
	void SetValues(AbsEntityValueVector values);

	/** Return the datatype of the ItemEntity.
 	 *  @return A value from the OvalEnum::Datatype enumeration representing the datatype of the ItemEntity.
	 */
	OvalEnum::Datatype GetDatatype();

	/** Set the datatype of the ItemEntity.
	 *  @param datatype A value from the OvalEnum::Datatype enumeration representing the datatype of the ItemEntity.
	 *  @return Void.
	 */
	void SetDatatype(OvalEnum::Datatype datatype);

	/** Return whether or not the ItemEntity is an object entity.
	 *  @return A boolean value indicating whether or not the ItemEntity is an object entity.
	 */
	bool GetIsObjectEntity();

	/** Set the boolean value indicating whether or not the ItemEntity is an object entity.
	 *  @param isObjectEntity A boolean value indicating whether or not the ItemEntity is an object entity.
	 *  @return Void.
	 */
	void SetIsObjectEntity(bool isObjectEntity);

	/** Set the nil field's value.
	 *  @param nil The boolean value indicating whether or not the entity has a nil value.
	 *  @return void
	 */
	void SetNil(bool isNil);

	/** Return true if the xsi:nil is set to true.
	 *  @return A boolean value indicating whether or not the entity has a nil value.
	 */
	bool GetNil();

private:
	OvalEnum::SCStatus scStatus;
	std::string name;
	AbsEntityValueVector value;
	OvalEnum::Datatype datatype;
	bool isObjectEntity;
	bool nil;
};

/**	
	A vector for storing ItemEntity objects. 
	Stores only pointers to the objects. 
*/
typedef std::vector < ItemEntity* > ItemEntityVector;

#endif
