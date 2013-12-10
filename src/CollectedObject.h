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

#ifndef COLLECTEDOBJECT_H
#define COLLECTEDOBJECT_H

#include <string>
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <map>
#include <utility>

#include "Common.h"
#include "AbsObject.h"
#include "OvalMessage.h"
#include "Item.h"
#include "VariableValue.h"
#include "AbsDataCollector.h"
#include "OvalEnum.h"

XERCES_CPP_NAMESPACE_USE


class CollectedObject;

/**
	A vector for storing CollectedObject objects. 
	Stores only pointers to the objects. 
*/
typedef std::vector < CollectedObject* > CollectedObjectVector;

/**	
	A pair for storing object ids and CollectedObjects together. 
	Stores only pointers to the CollectedObjects. 
*/
typedef std::pair <std::string, CollectedObject* > CollectedObjectPair;

/**	
	A map for storing CollectedObjectPairs. 
	Stores only pointers to the objects. 
*/
typedef std::map <std::string, CollectedObject* > CollectedObjectMap;

class Item;
typedef std::vector < Item* > ItemVector;

/**
	This class represents an object in a oval sytem characteristics schema.
	The oval system characteristics schema defiens a collected objects element which
	contains any number of object elements. These object elements reflects the 
	objects that an OVAL definition specfied for data collection. Each object that is
	collected for an OVAL definition has a corresponding object in the oval system charaeristitcs file.
*/
class CollectedObject {
public:
	~CollectedObject();

	static CollectedObject* CreateNotApplicable(AbsObject* absObject);
	static CollectedObject* CreateNotSupported(AbsObject* absObject);
	static CollectedObject* CreateError(std::string objectId, int version);
	static CollectedObject* CreateError(AbsObject *absObject);
	static CollectedObject* Create(AbsObject* absObject);

	static CollectedObject* GetCollectedObject(std::string objectId);

    /** Write all collected objects in the map of collected objects.
		After writing delete the object to free memory.
    */
	static void WriteCollectedObjects();

	void Write(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* scFile, DOMElement* collectObjectsElm);
	
	OvalEnum::Flag GetFlag();
	void SetFlag(OvalEnum::Flag flag);
	
	std::string GetId();
	void SetId(std::string id);

	std::string GetComment();
	void SetComment(std::string comment);

	OvalMessageVector* GetMessages();
	void SetMessages(OvalMessageVector* messages);

	ItemVector* GetReferences();
	void SetReferences(ItemVector* references);

	int GetVariableInstance();
	void SetVariableInstance(int variableInstance);

	VariableValueVector GetVariableValues() const
	{ return variableValues; }
	void SetVariableValues(const VariableValueVector &variableValues)
	{ this->variableValues = variableValues; }

	int GetVersion();
	void SetVersion(int version);

	/** Add a message to the end of the messages vector. */
	void AppendOvalMessage(OvalMessage* message);

	/** Creates references to the set of input items and computes the flag value of this object based on the set. */
	void AppendReferencesAndComputeFlag(ItemVector* references);

	void AppendVariableValue(const VariableValue &variableValue);
	void AppendVariableValues(const VariableValueVector &variableValues);

private:

	/** Creates references to the set of input items. */
	void AppendReferences(ItemVector* references);

	CollectedObject(std::string id = "", std::string comment = "", int version = 1, int variableInstance = 0, OvalEnum::Flag flag = OvalEnum::FLAG_ERROR);
	/** Ensure that references are only written once. */
	bool IsWritten(IntVector* itemIds, int itemId);
	
	static void Cache(CollectedObject* collectedObject);

	OvalEnum::Flag flag;
	std::string id;
	std::string comment;
	OvalMessageVector messages;
	ItemVector references;
	VariableValueVector variableValues;
	int version;
	int variableInstance;

	static CollectedObjectMap collectedObjectsMap;
};

/** 
	This class represents an Exception that occured while processing a CollectedObject.
*/
class CollectedObjectException : public Exception {
	public:
		/** 
			Set the error message and then set the severity to ERROR_FATAL. This is 
			done with the explicit call to the Exception class constructor that 
	 		takes a single string param.
		*/
		CollectedObjectException(std::string errMsgIn = "", int severity = ERROR_FATAL, Exception *ex = NULL);
		~CollectedObjectException();
};

#endif
