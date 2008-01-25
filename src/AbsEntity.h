//
// $Id: AbsEntity.h 4579 2008-01-02 17:39:07Z bakerj $
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

#ifndef ABSENTITY_H
#define ABSENTITY_H

//	required xerces includes
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
#include "VariableValue.h"
#include "AbsVariable.h"
#include "EntityComparator.h"
#include "ItemEntity.h"


XERCES_CPP_NAMESPACE_USE
using namespace std;

/** 
	This class represents an AbsEntity in the oval definition and system characterisitcs schemas.
	Since AbsEntities differ slightly between the definition and system characteristics schemas this class only
	encapsulates the commonalities. 
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
	AbsEntity(string name = "", string value = "", OvalEnum::Datatype datatype = OvalEnum::DATATYPE_STRING, bool isObjectEntity = false, OvalEnum::Operation operation = OvalEnum::OPERATION_EQUALS, 
		AbsVariable* varRef = NULL, OvalEnum::Check varCheck = OvalEnum::CHECK_ALL, bool nil = false);

	/**
		This destructor takes no special action. All dynamically allocated memory is freed by other classes.
	*/
	virtual ~AbsEntity();

	/**
		Return true if the specified entity is equal to the current entity. Otherwise return false.
		@param entity an AbsEntity* to compare against.
		@return The result of the comparison.
	*/
	virtual bool Equals(AbsEntity* entity) = 0;

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
		Return a vector of variable values that were used for this entity.
	*/
	VariableValueVector* GetVariableValues(); 

	string GetName();
	void SetName(string name);

	string GetValue();
	void SetValue(string value);

	OvalEnum::Datatype GetDatatype();
	void SetDatatype(OvalEnum::Datatype datatype);

	bool GetIsObjectEntity();
	void SetIsObjectEntity(bool isObjectAbsEntity);

	/** Return true if the xsi:nil is set to true. */
	bool GetNil();
	void SetNil(bool nil);

	OvalEnum::Operation GetOperation();
	void SetOperation(OvalEnum::Operation operation);

	OvalEnum::Check GetVarCheck();
	void SetVarCheck(OvalEnum::Check check);

	AbsVariable* GetVarRef();
	void SetVarRef(AbsVariable* varRef);

private:
	string name;
	string value;
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
typedef vector < AbsEntity*, allocator<AbsEntity*> > AbsEntityVector;

/** 
	This class represents an Exception that occured while processing an entity.
*/
class AbsEntityException : public Exception {
	public:
		AbsEntityException(string errMsgIn = "", int severity = ERROR_FATAL, Exception *ex = NULL);
		~AbsEntityException();
};

#endif
