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

#ifndef DEFINITION_H
#define DEFINITION_H
#include <map>
#include <utility>
#include "AbsCriteria.h"
#include "Criteria.h"

XERCES_CPP_NAMESPACE_USE

class Criteria;

class Definition;

/**
	A vector for storing Definition objects. 
	Stores only pointers to the objects. 
*/
typedef std::vector < Definition* > DefinitionVector;

/**	
	A pair for storing definition ids and Definition objects together. 
	Stores only pointers to the objects. 
*/
typedef std::pair <std::string, Definition* > DefinitionPair;

/**	
	A map for storing DefinitionPairs. 
	Stores only pointers to the objects. 
*/
typedef std::map <std::string, Definition* > DefinitionMap;

/**
	This class represents an oval definition.
	The Definition class provides methods for parsing a definition in an
	oval definitions files, analyzing that definitions, and then writing that defition
	with its analysis result to an oval results file.
*/
class Definition {

public:

	/** Create a complete Definition object */
//	Definition();
	/** 
		Destroy the object. 
		Call delete on the criteria element.
	*/
	~Definition();

	/** 
		Writes a Definition element to the results document.
		Calls criteria->Write()
	*/
	void Write(DOMElement* parent);
	/**
		Parses a definition from a oval-definitions document.
		Parses Definition elm to a Definition obj
		Calls Criteria->Parse() on the Criteria elm
		The final parsed Definition object is stored in the
		cache of processed definitions.
	*/
	void Parse(DOMElement* DefinitionElm);

	/** 
		Analyze the definition and return the result.
		Make sure not previously analyzed.
		calls Criteria->Analyze()
		saves and returns the result.
	*/
	OvalEnum::ResultEnumeration Analyze();

	/** Set the result to NOT_EVALUATED. */
	OvalEnum::ResultEnumeration NotEvaluated();

	/** Return the version field's value. */
	int GetVersion();
	/** Set the version field's value. */
	void SetVersion(int version);
	
	/** Return the variableInstance field's value. */
	int GetVariableInstance();
	/** Set the variableInstance field's value. */
	void SetVariableInstance(int variableInstance);
    
	/** Return the result field's value. */
	OvalEnum::ResultEnumeration GetResult();
	/** Set the result field's value .*/
	void SetResult(OvalEnum::ResultEnumeration result);

	/** Return the criteria field's value. */
	Criteria* GetCriteria();
	/** Set the criteria field's value. */
	void SetCriteria(Criteria* criteria);
    
	/** Return the id field's value. */
	std::string GetId();
	/** Set the id field's value. */
	void SetId(std::string id);

	/** Return the written field's value. */
	bool GetWritten();
	/** Set the written field's value. */
	void SetWritten(bool written);

	/** Return the analyzed field's value. */
	bool GetAnalyzed();
	/** Set the analyzed field's value. */
	void SetAnalyzed(bool analyzed);

	/** 
		Search the cache of definitions for the specified definition. 
		@return Returns the cached Definition or NULL if not found.
	*/
	static Definition* SearchCache(std::string id);

	/** Clear the cache of processed definitions. */
	static void ClearCache();

	/** Return a definition object for the specified definition id.
		First the cache of Definitions is checked. If the definition is
		not found in the cache the definition is looked up in the
		oval-definitions document and parsed. Once parsed the new Definition
		object is added to the cache.

		If the definition is not found an exception is thrown. 
	*/
	static Definition* GetDefinitionById(std::string definitionId);

private:

	/** Create a complete Definition object */
	Definition();

	int version;
	int variableInstance;
	OvalEnum::ResultEnumeration result;
	Criteria* criteria;
	std::string id;
	bool written;
	bool analyzed;

	static DefinitionMap processedDefinitionsMap;
};


#endif
