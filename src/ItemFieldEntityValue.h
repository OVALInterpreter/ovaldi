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

#ifndef ITEMFIELDENTITYVALUE_H
#define ITEMFIELDENTITYVALUE_H

#include "AbsEntityValue.h"

XERCES_CPP_NAMESPACE_USE

/**
	This class represents a field entity in an item entity that has a datatype of record as defined in the OVAL systems characteristics schema.
*/
class ItemFieldEntityValue : public AbsEntityValue {
public:

	/** Create a complete ItemFieldEntityValue object.
	 *  @param name A string that represents the name attribute of the field entity. 
	 *  @param value A string that represents the value of the field entity.
	 *  @param datatype A OvalEnum::Datatype that specifies the datatype of the value of the field entity.
	 *  @param status A OvalEnum::SCStatus that specifies the status of the value of the field entity.
	 */
	ItemFieldEntityValue(std::string name = "", std::string value = "", OvalEnum::Datatype datatype = OvalEnum::DATATYPE_STRING, OvalEnum::SCStatus status = OvalEnum::STATUS_EXISTS);
	
    /** ItemFieldEntityValue copy constructor. 
	 *  @param itemFieldEntityValue The ItemFieldEntityValue that you would like to copy into this ItemFieldEntityValue.
	 */
    ItemFieldEntityValue(const ItemFieldEntityValue& itemFieldEntityValue);

    /** ItemFieldEntityValue destructor. */
    ~ItemFieldEntityValue();

	/** Return the name value of the field.
	 *  @return A string representing the name value of the field.
	 */
	std::string GetName();

	/** Set the name field's value.
	 *  @param name A string value representing the field's name.
	 *  @return Void.
	 */
	void SetName(std::string name);

	/** Return the datatype of the field.
 	 *  @return A value from the OvalEnum::Datatype enumeration representing the datatype of the field.
	 */
	OvalEnum::Datatype GetDatatype();

	/** Set the datatype of the field.
	 *  @param datatype A OvalEnum::Datatype enumeration value representing the datatype of the field.
	 *  @return Void.
	 */
	void SetDatatype(OvalEnum::Datatype datatype);

	/** Return the status of the ItemFieldEntityValue.
	 *  @return A OvalEnum::SCStatus value representing the status of the ItemFieldEntityValue.
	 */
	OvalEnum::SCStatus GetStatus();

	/** Set the status of the ItemFieldEntityValue.
	    @param status The status of the ItemFieldEntityValue.  
		@return Void.
	 */
	void SetStatus(OvalEnum::SCStatus status);
	
	/** Determine if the specified AbsEntityValue is equal to this ItemFieldEntityValue.
		@param entityValue the AbsEntityValue that you would like to see if the current ItemFieldEntityValue is equal to.
		@return a boolean value indicating whether or not the two ItemFieldEntityValues are equal.
	*/
	virtual bool Equals(AbsEntityValue* entityValue);

	/** Write this ItemFieldEntityValue as the value of the specified entity in the specified systems-characteristics file. 
	 *  @param scFile A pointer to a DOMDocument that specifies the system-characteristics file where the data should be written to.
	 *  @param itemEntityElm A pointer to a DOMDocument that specifies the entity for which the ItemFieldEntityValue should be written to.
	 *  @return Void.
	 */
	void Write(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* scFile, DOMElement* itemEntityElm);
	
	/** Parse the specified entity to retrieve its field entity value.
	 *  @param entityElm A pointer to a DOMElement from which the field entity value should be retrieved.  This value should then be used to initialize the ItemFieldEntityValue. 
	 *  @return Void.
	 */
	void Parse(DOMElement* entityElm);

private:
	std::string name;
	OvalEnum::Datatype datatype;
	OvalEnum::SCStatus scStatus;
};

/**	
	A vector for storing ItemFieldEntityValue objects. 
	Stores only pointers to the objects. 
*/
typedef std::vector < ItemFieldEntityValue* > ItemFieldEntityValueVector;

#endif

