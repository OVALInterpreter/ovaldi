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

#ifndef ANALYZER_H
#define ANALYZER_H

// other includes
#include "Common.h"
#include "Definition.h"
#include "DocumentManager.h"
#include "Version.h"
#include "Item.h"

using namespace std;

/**
	The Analyzer class is the starting point for the oval analysis.
*/
class Analyzer {
public:
	Analyzer();

	~Analyzer();

	/** Evaluate all OVAL Definitions.
		This Run method runs through all the OVAL Definitions in the specified
	    OVAL Definitions file and evaluates them.
	*/
	void Run();
	/** Evaluate the set of OVAL Definitions.
		This Run method runs through set of definitionIds provided. The method evaluates
		wach specified definition. All other definitions are reported as not evaluated in the
		oval-results.xml
		If a sepcified definition id is not found in the OVAL Definitions document an error is
		reported, but evaluation will continue until the all definitions have been processed.
	*/
	void Run(StringVector* definitionIds);

	/** Return a ptr to the tests element in the results doc. **/
	static DOMElement* GetResultsSystemTestsElm();
	/** Return a ptr to the definitions element in the results document. **/
	static DOMElement* GetResultsSystemDefinitionsElm();
	/** Return a ptr to the system element in the results document. **/
	static DOMElement* GetResultsSystemElm();
	/** Return a ptr to the results element in the results document. **/
	static DOMElement* GetResultsElm();

	/** Print the results of the analysis. */
	void PrintResults();

	/** Append a true result. **/
	static void AppendTrueResult(StringPair* pair);
	/** Append a false result. **/
	static void AppendFalseResult(StringPair* pair);
	/** Append an error result. **/
	static void AppendErrorResult(StringPair* pair);
	/** Append an unknown result. **/
	static void AppendUnknownResult(StringPair* pair);
	/** Append a not applicable result. **/
	static void AppendNotApplicableResult(StringPair* pair);
	/** Append a not evaluated result. **/
	static void AppendNotEvaluatedResult(StringPair* pair);

private:

	/** Format a pair of definition id and result value as a string for display. */
	string ResultPairToStr(StringPair* pair);

	/** Initialize the results document adding the basic structure to it. */
	void InitResultsDocument();

	/** Finialize the results document copying the sc and definitions files into their appropriate locations. */
	void FinializeResultsDocument();

	static DOMElement* definitionsElm;
	static DOMElement* testsElm;
	static DOMElement* resultsSystemElm;
	static DOMElement* resultsElm;

	static StringPairVector trueResults;
	static StringPairVector falseResults;
	static StringPairVector unknownResults;
	static StringPairVector errorResults;
	static StringPairVector notEvaluatedResults;
	static StringPairVector notApplicableResults;

};

/** 
	This class represents an Exception that occured while running the analyzer.
*/
class AnalyzerException : public Exception {
	public:
		AnalyzerException(string errMsgIn = "", int severity = ERROR_FATAL, Exception* ex = NULL);
		~AnalyzerException();
};

#endif
