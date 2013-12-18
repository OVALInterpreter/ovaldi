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

#ifndef OBJECT_H
#define OBJECT_H

#include <map>
#include <utility>
#include <string>

#include "AbsObject.h"
#include "ObjectEntity.h"
#include "Behavior.h"
#include "Filter.h"

class Object;

/**	
	A pair for storing object ids and Object pointers together. 
	Stores only pointers to the objects. 
*/
typedef std::pair <std::string, Object* > ObjectPair;

/**	
	A map for storing ObjectPairs. 
	Stores only pointers to the objects. 
*/
typedef std::map <std::string, Object* > ObjectMap;

/**
	This class represents an Object in an oval definition file.	
*/
class Object : public AbsObject {
public:
    /** Create a complete object */
	Object(std::string id = "", std::string comment = "", std::string xmlns = "", std::string name = "", int version = 1);
	virtual ~Object();

    /** Parse the provided object element into an object. */
	virtual void Parse(xercesc::DOMElement* objectElm);

    /** Return a vector of variable values that were used for this object. */
	virtual VariableValueVector GetVariableValues();
	
    /** Set the behaviors field's value. */
	BehaviorVector* GetBehaviors();

    /** Return the behaviors field's value. */
	void SetBehaviors(BehaviorVector* behaviors);

    /** Return the elements field's value. */
	AbsEntityVector* GetElements();

    /** Set the elements field's value. */
	void SetElements(AbsEntityVector* elements);

    /** Return the element with the specified name. */
	ObjectEntity* GetElementByName(std::string elementName);

    /** Add an element to the end of the elements vector. */
	void AppendElement(ObjectEntity* objectEntity);

    /** Return TRUE if the input item matches the criteria specified by this object. */
    bool Analyze(Item* item);

	/**
	 * Returns the embedded filters.  The vector is owned by this object,
	 * do not delete it.
	 */
	FilterVector* GetFilters();

    /** Return an object for the specified object id.
		First the cache of Objects is checked. If the object is
		not found in the cache the object is looked up in the
		oval-definitions document and parsed. Once parsed 
		the new Object is added to the cache.

		If the Object is not found an exception is thrown. 
		
		NOTE: This method is not intended to be used during data collection.
	*/
	static Object* GetObjectById(std::string objectId);
    
    /** Delete all Objects in the cache. */
	static void ClearCache();

private:

    /** Cache the specified Object. */
	static void Cache(Object* object);

	/** Search the cache of Objects for the specified Object. 
		@return Returns the object with the specified id or NULL if not found.
	*/
	static Object* SearchCache(std::string id);

	AbsEntityVector elements;
	BehaviorVector behaviors;
	FilterVector filters;

    static ObjectMap objectCache;
};

#endif
