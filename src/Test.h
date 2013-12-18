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

#ifndef TEST_H
#define TEST_H

#include <xercesc/dom/DOMElement.hpp>

#include "TestedItem.h"
#include "VariableValue.h"
#include "Object.h"
#include "OvalMessage.h"

class Test;

/**	
	A pair for storing test ids and Test objects together. 
	Stores only pointers to the objects. 
*/
typedef std::pair <std::string, Test* > TestPair;

/**	
	A map for storing TestPairs. 
	Stores only pointers to the objects. 
*/
typedef std::map <std::string, Test* > TestMap;

/**
	This class represents a test in OVAL.
	The class provides a method for parsing a test in an oval definition as well as methods
	for analyzing that test and then writing the test result to an oval results document.
*/
class Test {

public:

	/** Distroy the Test object.
	    Deletes each TestedItem.
	*/
	~Test();

	/** Write the Test element to the result document if it has not already been written.
		calls testedObject->Write() 
		calls testedVariable->Write() for each tested var.
	*/
	void Write(xercesc::DOMElement* parent);


	/** Evaluate the test and return the result. 
	    Make sure not previously analyzed.
	  	Get the collected object from the sc file by its id
	        - Parse all references in the colelcted object into Items
	  		- check flag on collected object against check_existence attribute on test
	        - If the test uses a state and the check_existence result is true 
	            - Get the state by its id and parse it.
	            - Analyze all items referenced by the collected object to determine individual results.
	            - Then use the check_state to determine the overall result for the test.
	 	 	- copy variable values from collected object to test as tested_variables
	 	 		this is just a matter of passing the variable_value element in the colelcted obj
	  	 		to the testedVariable->Parse() method.
	        - save and return the result
	 */
	OvalEnum::ResultEnumeration Analyze();

	/** Set the test to not evaluated. */
	OvalEnum::ResultEnumeration NotEvaluated();

	/** Determine the result of evaluating the check_existence attribute. */
	OvalEnum::ResultEnumeration EvaluateCheckExistence();

	/** Determine the result of evaluating the check attribute. */
	OvalEnum::ResultEnumeration EvaluateCheckState();

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
	/** Set the result field's value. */
	void SetResult(OvalEnum::ResultEnumeration result);
    
	/** Return the id field's value. */
	std::string GetId();
	/** Set the id field's value. */
	void SetId(std::string id);

	/** Return the objectId field's value. */
	std::string GetObjectId();
	/** Set the objectId field's value. */
	void SetObjectId(std::string objectId);

    /** Return the object that is referenced by this test. */
    Object* GetReferencedObject();

	/** Return the stateId field's value. */
	StringSet* GetStateIds();
	/** Set the stateIds field's value. */
	void SetStateIds(StringSet stateIds);
    /** Add the stateId to the list of state ids. */
	void AppendStateId(std::string stateId);
    /** return true if the test has a state reference. */
    bool HasStateReference();

	/** Return the name field's value. */
	std::string GetName();
	/** Set the name field's value. */
	void SetName(std::string name);

	/** Return the written field's value. */
	bool GetWritten();
	/** Set the written field's value. */
	void SetWritten(bool written);

	/** Return the analyzed field's value. */
	bool GetAnalyzed();
	/** Set the analyzed field's value. */
	void SetAnalyzed(bool analyzed);

	/** Return the check field's value. */
	OvalEnum::Check GetCheck();
	/** Set the check field's value. */
	void SetCheck(OvalEnum::Check check);

	/** Return the checkExistence field's value. */
	OvalEnum::Existence GetCheckExistence();
	/** Set the checkExistence field's value. */
	void SetCheckExistence(OvalEnum::Existence checkExistence);

    /** Return the stateOperator field's value. */
	OvalEnum::Operator GetStateOperator();
	/** Set the stateOperator field's value. */
	void SetStateOperator(OvalEnum::Operator stateOperator);

	/** Return the testedItems field's value. */
	TestedItemVector* GetTestedItems();
	/** Set the testedItems field's value. */
	void SetTestedItems(TestedItemVector* testedItems);
	/** Add the specified TestedItem to the set of tested Items. */
	void AppendTestedItem(TestedItem* testedItem);

	/** Return the testedVariables field's value **/
	VariableValueVector GetTestedVariables() const
	{ return testedVariables; }

	/** Set the testedVariables field's value **/
	void SetTestedVariables(const VariableValueVector &testedVariables)
	{ this->testedVariables = testedVariables; }

	/** Add the specified TestedVariable to the set of tested variables **/
	void AppendTestedVariable(const VariableValue &testedVariable);

    /** mark all the tested items as not evaluated. **/
    void MarkTestedItemsNotEvaluated();

	/** Delete all items in the cache. **/
	static void ClearCache();
	/** Cache the specified Test. */
	static void Cache(Test* test);

	/** Return a test object for the specified test id.
		First the cache of Tests is checked. If the test is
		not found in the cache the test is looked up in the
		oval-definitions document and parsed. Once parsed the new Test
		object is added to the cache.

		If the test is not found an exception is thrown. 
	*/
	static Test* GetTestById(std::string testId);

	/** Return the test's message vector. */
	OvalMessageVector* GetMessages();
	/** Add the specified message to the test's vector of messages **/
	void AppendMessage(OvalMessage* message);


private:

	/** Create a complete Test object **/
	Test();

	/** Parse the Test element into a Test object. 
		The resulting object is cached.
	*/
	void Parse(xercesc::DOMElement* testElm);

	/** Search the cache of Tests for the specified Test. 
	    Return NULL if not found 
	*/
	static Test* SearchCache(std::string id);

	int version;
	int variableInstance;
	OvalEnum::ResultEnumeration result;
	std::string id;
	bool written;
	bool analyzed;
	OvalEnum::Check check;
	OvalEnum::Existence checkExistence;
	OvalEnum::Check checkState;
    OvalEnum::Operator stateOperator;
	TestedItemVector testedItems;
	VariableValueVector testedVariables;
	std::string objectId;
	StringSet stateIds;
	std::string name;
	OvalMessageVector testMessages;

	static TestMap processedTestsMap;
};

#endif
