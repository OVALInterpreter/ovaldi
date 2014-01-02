//
//
//****************************************************************************************//
// Copyright (c) 2002-2014, The MITRE Corporation
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

#include "XmlCommon.h"

#include "Filter.h"

using namespace std;
using namespace xercesc;

//****************************************************************************************//
//									Filter Class										  //	
//****************************************************************************************//

Filter::Filter(DOMElement *filterElm) {
	this->Parse(filterElm);
}

// ***************************************************************************************	//
//								 Public members												//
// ***************************************************************************************	//
bool Filter::DoFilter(Item* item) {

	OvalEnum::ResultEnumeration result = this->state->Analyze(item);

	// for a filter really want to convert the result to a boolean
	if (result != OvalEnum::RESULT_TRUE && result != OvalEnum::RESULT_FALSE)
		throw AbsStateException("Filtering failed: analysis of an item did "
			"not result in a true/false result.  Found result: " +
			OvalEnum::ResultToString(result));

	return (result == OvalEnum::RESULT_TRUE && !this->IsExcluding()) ||
		(result == OvalEnum::RESULT_FALSE && this->IsExcluding());
}

void Filter::Parse(DOMElement* filterElm) {
	string stateId = XmlCommon::GetDataNodeValue(filterElm);
	string actionStr = XmlCommon::GetAttributeByName(filterElm, "action");
	this->SetExcluding(actionStr.empty() || actionStr == "exclude");
	this->state = State::GetStateById(stateId);
}

bool FilterFunctor::operator()(Item *item) {
	// the *_if (e.g. remove_if) algorithms need true
	// returned to indicate the item should be removed,
	// so we negate DoFilter's return value.
	return !this->filter->DoFilter(item);
}
