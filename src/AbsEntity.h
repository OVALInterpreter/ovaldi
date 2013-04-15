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

#ifndef ABSENTITY_H
#define ABSENTITY_H

#include <string>
#include <iostream>
#include <stdlib.h>

#include "Common.h"
#include "OvalEnum.h"
#include "VariableValue.h"
#include "AbsVariable.h"
#include "ItemEntity.h"
#include "EntityComparator.h"
#include "AbsEntityValue.h"

XERCES_CPP_NAMESPACE_USE

/** 
	This class aligns roughly with the EntityBaseType as defined in the oval-definition-schema. 
*/
class AbsEntity {
public:
	/**
		This constructor has default values specified for all parameters. Default values are based on 
		the default values found in the oval schemas or reasonable defaults for this application.
		@param name a string representing the name of the entity.
		@param value a string representing the value of the entity.
		@param datatype an OvalEnum::Datatype representing the datatype attribute for the entity. 
		@param isObjectEntity a bool used to indicate whether or not the entity is part of an object specifier.
		@param operation an OvalEnum::Operation representing the operation attribute for the entity.
		@param varRef an AbsVariable* that represents the var_ref attribute for the entity. 
		@param varCheck an OvalEnum::Check that represents the var_check attribute for the entity.
		@param nil a bool that represents the xsi:nil attribute for the entity.
	*/
	AbsEntity(std::string name = "", std::string value = "", OvalEnum::Datatype datatype = OvalEnum::DATATYPE_STRING, OvalEnum::Operation operation = OvalEnum::OPERATION_EQUALS, 
		AbsVariable* varRef = NULL, OvalEnum::Check varCheck = OvalEnum::CHECK_ALL, bool nil = false);

	/**
		This destructor takes no special action. All dynamically allocated memory is freed by other classes.
	*/
	virtual ~AbsEntity();

	/**
		Parse the specified DOMElement* into an AbsEntity.
		@param entitiyElm a DOMElement* that represents the xml version of an entity.
	*/
	virtual void Parse(DOMElement* entitiyElm) = 0;

	/**
		Analyze the specified item entity based on the current AbsEntity's attributes and value.
		Ensures that any errors thrown durring entity analysis are caught and that an erro result is returned.
		NOTE: Does not account for a mis match in data type the data type from the OVAL Definition is 
		used. The datatype in the OVAL System Characteristics xml is ignored.
	
		@param scElement a ItemEntity* that represents an entity in an Item in the system characteristics file.
		@return The analysis result.
	*/
	OvalEnum::ResultEnumeration Analyze(ItemEntity* scElement);

    /**
	 * Gets values for the given entity.  Insulates me from having
	 * to care whether they were defined literally or via var_ref.
	 * The given StringVector is populated with the values.  This 
	 * may only be called on non-record entities.  Entities of 
	 * record type have two-dimensional values (e.g. like a matrix,
	 * or list of lists), which can't be captured with a plain vector.
	 * <p>
	 * If the given entity uses a var_ref, the return value will be
	 * the flag value of the referenced variable.  If the flag is not
	 * FLAG_COMPLETE, no values are added to the StringVector.
	 * </p><p>
	 * If the specified entity does not use a var_ref, FLAG_COMPLETE is
     * returned.
	 * </p>
     */
    OvalEnum::Flag GetEntityValues(StringVector &values);

    /**
	 * Gets values for the given entity.  Insulates me from having
	 * to care whether they were defined literally or via var_ref.
	 * The given StringVector is populated with the values.  This 
	 * may only be called on record entities.  Entities of 
	 * record type have two-dimensional values (e.g. like a matrix,
	 * or list of lists), which can't be captured with a plain vector.
	 * The given map will be filled out with the field names as map
	 * keys, and vectors of corresponding field values as the map
	 * values.
	 * <p>
	 * The returned value is a combination of flag values for all fields.
	 * For fields that don't have a var_ref, FLAG_COMPLETE is used.  The
	 * flags are combined via OvalEnum::CombineFlags().
	 * </p>
     */
	OvalEnum::Flag GetEntityValues(std::map<std::string, StringVector> &values);

	/** 
	 *	Return a vector of variable values that were used for this entity.
	 *	@return A VariableValueVector* that represents the variable values specified by the entity.
	 */
	VariableValueVector* GetVariableValues(); 

	/** Return the name field's value. 
	 *  @return A string representing the name of the entity.
	 */
	std::string GetName();

	/** Set the name field's value.
	 *  @param name A string representing the name of the entity.
	 *  @return Void.
	 */
	void SetName(std::string name);

	/** Return the string that is the value of the entity. Use this when the entity is a string-based value. 
	 *  @return A string representing the value of the entity.
	 */
	 std::string GetValue();

	/** Set the string value of the entity. Use this when the entity is string-based value.
	 *  @param value A string representing the value of the entity.
	 *  @return Void.
	 */
	void SetValue(std::string value);

	/** Return the values of the entity. Use this when the entity is not a string-based value.
	 *  @return A AbsEntityValueVector containing the values of the entity.
	 */
	AbsEntityValueVector GetValues();

	/** Set the values of the entity. Use this when the entity is not a string-based value.
	 *  @param value A AbsEntityValueVector representing the values of the entity.
	 *  @return Void.
	 */
	void SetValues(AbsEntityValueVector value);

	/** Return the datatype field's value.
	 *  @return A OvalEnum::Datatype value representing the datatype of the entity.
	 */
	OvalEnum::Datatype GetDatatype();

	/** Set the datatype field's value.
	 *  @param datatype An OvalEnum::Datatype enumeration value representing the value of the entity.
	 *  @return Void.
	 */
	void SetDatatype(OvalEnum::Datatype datatype);

	/** Return true if the xsi:nil is set to true.
	 *  @return A boolean value indicating whether or not the entity has a nil value.
	 */
	bool GetNil();

	/** Set the nil field's value.
	 *  @param nil The boolean value indicating whether or not the entity has a nil value.
	 *  @return Void.
	 */
	void SetNil(bool nil);

	/** Return the operation field's value.
	 *  @return A OvalEnum::Operation value representing the operation of the entity.
	 */
	OvalEnum::Operation GetOperation();

	/** Set the operation field's value.
	 *  @param operation An OvalEnum::Operation enumeration value indicating the operation of the entity.
	 *  @return Void.
	 */
	void SetOperation(OvalEnum::Operation operation);

	/** Return the varCheck field's value.
	 *  @return An OvalEnum::Check enumeration value representing the var check value of the entity.
	 */
	OvalEnum::Check GetVarCheck();
	
	/** Set the var check value.
	 *  @param check A OvalEnum::Check value representing the var check value of the entity.
	 *  @return Void.
	 */
	void SetVarCheck(OvalEnum::Check check);

	/** Return the referenced variable.
 	 *  @return An AbsVariable* which is the variable referenced by the entity's var ref attribute.
	 */
	AbsVariable* GetVarRef();

	/** Set the referenced variable of the entity.
	 *  @param varRef A AbsVariable value representing the variable referenced by the entity's var ref attribute.
	 *  @return Void.
	 */
	void SetVarRef(AbsVariable* varRef);

private:
	std::string name;
	AbsEntityValueVector value;
	OvalEnum::Datatype datatype;
	bool isObjectEntity;
	OvalEnum::Check varCheck;
	OvalEnum::Operation operation;
	AbsVariable* varRef;
	bool nil;
};

/**	
	A vector for storing AbsEntity objects. 
	Stores only pointers to the objects. 
*/
typedef std::vector < AbsEntity* > AbsEntityVector;

/** 
	This class represents an Exception that occured while processing an entity.
*/
class AbsEntityException : public Exception {
	public:
		AbsEntityException(std::string errMsgIn = "", int severity = ERROR_FATAL, Exception *ex = NULL);
		~AbsEntityException();
};

#endif
