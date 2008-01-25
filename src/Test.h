//
// $Id: Test.h 4579 2008-01-02 17:39:07Z bakerj $
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

#ifndef TEST_H
#define TEST_H

#include <vector>
//#include "Analyzer.h"
#include "TestedItem.h"
#include "VariableValue.h"
#include "Log.h"
#include "State.h"


XERCES_CPP_NAMESPACE_USE
using namespace std;

class Test;

/**	
	A pair for storing test ids and Test objects together. 
	Stores only pointers to the objects. 
*/
typedef pair <string, Test* > TestPair;

/**	
	A map for storing TestPairs. 
	Stores only pointers to the objects. 
*/
typedef map <string, Test* > TestMap;

/**
	This class represents a test oval.
	The class provides a method for parsing a test in an oval definition as well as methods
	for analyzing that test and then writing the test result to an oval results document.
*/
class Test {

public:
	/** Create a complete Test object **/
	Test();
	~Test();

	void Write(DOMElement* parent);
	void Parse(DOMElement* testElm);

	/** Evaluate the test and return the result. 
	 *  Make sure not previously analyzed.
	 *	Get the collected object from the sc file by its id
	 *      - Parse all references in the colelcted object into Items
	 *		- check flag on collected object against check_existence attribute on test
	 *      - If the test uses a state and the check_existence result is true 
	 *          - Get the state by its id and parse it.
	 *          - Analyze all items referenced by the collected object to determine individual results.
	 *          - Then use the check_state to determine the overall result for the test.
	 *		- copy variable values from collected object to test as tested_variables
	 *			this is just a matter of passing the variable_value element in the colelcted obj
	 * 			to the testedVaraible->Parse() method.
	 *      - save and return the result
	 */
	OvalEnum::ResultEnumeration Analyze();

	/** Set the test to not evaluated. */
	OvalEnum::ResultEnumeration NotEvaluated();

	/** Determine the result of evaluating the check_existence attribute. */
	OvalEnum::ResultEnumeration EvaluateCheckExistence();

	/** Determine the result of evaluating the check attribute. */
	OvalEnum::ResultEnumeration EvaluateCheckState();

	int GetVersion();
	void SetVersion(int version);
	
	int GetVariableInstance();
	void SetVariableInstance(int variableInstance);
    
	OvalEnum::ResultEnumeration GetResult();
	void SetResult(OvalEnum::ResultEnumeration result);
    
	string GetId();
	void SetId(string id);

	string GetObjectId();
	void SetObjectId(string objectId);

	string GetStateId();
	void SetStateId(string stateId);

	bool GetWritten();
	void SetWritten(bool written);

	bool GetAnalyzed();
	void SetAnalyzed(bool analyzed);

	/** Return the check field's value. */
	OvalEnum::Check GetCheck();
	/** Set the check field's value. */
	void SetCheck(OvalEnum::Check check);

	/** Return the checkExistence field's value. */
	OvalEnum::Existence GetCheckExistence();
	/** Set the checkExistence field's value. */
	void SetCheckExistence(OvalEnum::Existence checkExistence);

	/** Return the testedItems field's value. */
	TestedItemVector* GetTestedItems();
	/** Set the testedItems field's value. */
	void SetTestedItems(TestedItemVector* testedItems);
	/** Add the specifed TestedItem to the set of tested Items. */
	void AppendTestedItem(TestedItem* testedItem);

	/** Return the testedVariables field's value **/
	VariableValueVector* GetTestedVariables();

	/** Set the testedVariables field's value **/
	void SetTestedVariables(VariableValueVector* testedVariables);
	/** Add the specified TestedVariable to the set of tested variables **/
	void AppendTestedVariable(VariableValue* testedVariable);

	/** Search the cache of Tests for the specifed Test. 
	    Return NULL if not found 
	*/
	static Test* SearchCache(string id);
	/** Delete all items in the cache. **/
	static void ClearCache();

private:
	int version;
	int variableInstance;
	OvalEnum::ResultEnumeration result;
	string id;
	bool written;
	bool analyzed;
	OvalEnum::Check check;
	OvalEnum::Existence checkExistence;
	OvalEnum::Check checkState;
	TestedItemVector testedItems;
	VariableValueVector testedVariables;
	string objectId;
	string stateId;

	//static TestVector processedTests;
	static TestMap processedTestsMap;
};

#endif
