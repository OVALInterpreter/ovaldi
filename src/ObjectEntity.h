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

#ifndef OBJECTENTITY_H
#define OBJECTENTITY_H

#include "AbsEntity.h"
#include "VariableFactory.h"

XERCES_CPP_NAMESPACE_USE
using namespace std;

class ObjectEntity;

/**
	This class represents an entity in an ObjectEntity as defined in the oval definition schema. 
	All new instances of this class are created with the AbsEntity::isObjectEntity flag set to true.
*/
class ObjectEntity : public AbsEntity {
public:
	/**
		Copy constructor. Creates a new ObjectEntity based on the specified ObjectEntity. 
		@param orig a ObjectEntity* to be copied
	*/
	ObjectEntity(ObjectEntity* orig);

	/** Create a complete ObjectEntity object. */
	ObjectEntity(string name = "", string value = "", OvalEnum::Datatype datatype = OvalEnum::DATATYPE_STRING, OvalEnum::Operation operation = OvalEnum::OPERATION_EQUALS, AbsVariable* varRef = NULL, OvalEnum::Check varCheck = OvalEnum::CHECK_ALL, bool nil = false);
	
	/** ObjectEntity destructor. */
	~ObjectEntity();

	/**
	 *  Return true if the specified entity is equal to the current entity. Otherwise return false.
	 *	@param entity an AbsEntity* to compare against.
	 *	@return The result of the comparison.
	 */
	bool Equals(AbsEntity* entity);

	/** Parse the XML representation of a ObjectEntity.
	 *	@param entitiyElm a DOMElement* that represents the xml version of an entity.
	 *	@return Void.
	 */
	void Parse(DOMElement* entitiyElm);
};

#endif
