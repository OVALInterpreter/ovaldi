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

#ifndef ABSVARIABLE_H
#define ABSVARIABLE_H

//	other includes
#include <string>
#include <map>
#include <utility>

#include <xercesc/dom/DOMElement.hpp>

#include "StdTypedefs.h"
#include "Exception.h"
#include "OvalEnum.h"
#include "VariableValue.h"

class AbsVariable;

/**	
	A pair for storing variable ids and AbsVariable together. 
	Stores only pointers to the AbsVariable. 
*/
typedef std::pair <std::string, AbsVariable* > AbsVariablePair;

/**	
	A map for storing AbsVariable. 
	Stores only pointers to the objects. 
*/
typedef std::map <std::string, AbsVariable* > AbsVariableMap;

/**
	This class represents an AbsVariable in an oval definition schema.
*/
class AbsVariable {
public:
	/** delete the messages */
	virtual ~AbsVariable();

	virtual void Parse(xercesc::DOMElement*) = 0;

	virtual VariableValueVector* GetVariableValues() = 0;
	
	/** Return the id field's value. */
	std::string GetId();
	/** Set the id field's value. */
	void SetId(std::string id);
	
	/** Return the datatype field's value. */
	OvalEnum::Datatype GetDatatype();
	/** Set the datatype field's value. */
	void SetDatatype(OvalEnum::Datatype datatype);

	/** Return the flag field's value. */
	OvalEnum::Flag GetFlag();
	/** Set the flag field's value. */
	void SetFlag(OvalEnum::Flag flag);

	/** Return the name field's value. */
	std::string GetName();
	/** Set the name field's value. */
	void SetName(std::string name);

	/** Return the values field's value. */
	VariableValueVector* GetValues();
	/** Set the values field's value. */
	void SetValues(VariableValueVector* value);
	/** Add a value to the set of values associated with this variable. */
	void AppendVariableValue(VariableValue* value);

	/** Return the version field's value. */
	int GetVersion();
	/** Set the version field's value. */
	void SetVersion(int version);

	/** Return the msgs field's value. */
	StringVector* GetMessages();
	/** Set the msgs field's value. */
	void SetMessages(StringVector* msgs);
	/** Add a msg to the end of the msgs vector. */
	void AppendMessage(std::string msg);
	/** Add a newMsgs to the end of the msgs vector. */
	void AppendMessages(StringVector* newMsgs);
	/** Create a string listing of all messages. */
	std::string ListMessages();

	/** Search the cache of variables for the specified variable. 
		return NULL if not found.
	*/
	static AbsVariable* SearchCache(std::string id);
	/** delete all items in the cache. */
	static void ClearCache();
	/** cache the specified var
		TODO - do i need to add protection to this cache
	*/
	static void Cache(AbsVariable* var);

protected:
	AbsVariable(std::string id = "", std::string name = "", int version = 1, OvalEnum::Datatype datatype = OvalEnum::DATATYPE_STRING, StringVector* msgs = new StringVector());

private:

	std::string id;
	OvalEnum::Flag flag;
	std::string name;
	int version;
	OvalEnum::Datatype datatype;
	VariableValueVector values; 
	//bool error;  // i don't think this is used anywhere
	StringVector* msgs;

	static AbsVariableMap processedVariableCache;
};

#endif
