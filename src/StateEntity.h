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

#ifndef STATEENTITY_H
#define STATEENTITY_H

#include "AbsEntity.h" 
#include "VariableFactory.h"

XERCES_CPP_NAMESPACE_USE

/**
	This class represents an entity in the State as definted in the oval definition schema. 
	StateEntities have an entityCheck attribute in additional to all attributes defined in the 
	AbsEntity class.
*/
class StateEntity : public AbsEntity {
public:
	/**
		Copy constructor. Creates a new StateEntity based on the specified StateEntity. 
		@param orig a StateEntity* to be copied
	*/
	StateEntity(StateEntity* orig);
	/** Create a complete StateEntity object. */
	StateEntity(std::string name = "", std::string value = "", OvalEnum::Datatype datatype = OvalEnum::DATATYPE_STRING, OvalEnum::Operation operation = OvalEnum::OPERATION_EQUALS, AbsVariable* varRef = NULL, OvalEnum::Check entityCheck = OvalEnum::CHECK_ALL, OvalEnum::Check varCheck = OvalEnum::CHECK_ALL, bool nil = false);

	/** StateEntity destructor. */
	~StateEntity();

	/**
	 *  Return true if the specified entity is equal to the current entity. Otherwise return false.
 	 *  @param entity an AbsEntity* to compare against.
	 *  @return The result of the comparison.
	 */
	bool Equals(AbsEntity* entity);

	/** Return true if the specified entity value exists in the entity value vector.
	 *  @param entityValueVector the entity value vector for which you want to see if the specified entity value exists.
	 *	@param entityValue the entity value whose existence you would like to check in the specified entity value vector.
	 * 	@return A boolean value indicating whether or not the specified entity value exists in the specified entity values vector.
	 */
	bool ValueExistsInStateEntity(AbsEntityValueVector entityValueVector, AbsEntityValue* entityValue);

	/** Parse the XML representation of a StateEntity.
	 *	@param entitiyElm a DOMElement* that represents the xml version of an entity.
	 *	@return Void.
	 */
	void Parse(DOMElement* entitiyElm);

	/** Return the entity check value.
 	 *  @return A value from the OvalEnum::Check enumeration representing the entity check value of the state entity.
	 */
	OvalEnum::Check GetEntityCheck();

	/** Set the entity check value.
	 *  @param check A OvalEnum::Check value representing the entity check value of the state entity.
	 *  @return Void.
	 */
	void SetEntityCheck(OvalEnum::Check check);

private:

	OvalEnum::Check entityCheck;  /*!< a flag that defines how to compare this enetity against multiple corresponding entities in an Item.  */
};

#endif
