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

#include "AbsVariable.h"

using namespace std;

AbsVariableMap AbsVariable::processedVariableCache;

//****************************************************************************************//
//									AbsVariable Class									  //	
//****************************************************************************************//

// ***************************************************************************************	//
//								 Public members												//
// ***************************************************************************************	//

void AbsVariable::AppendMessages(StringVector* newMsgs) {

	StringVector::iterator iterator;
	for(iterator = newMsgs->begin(); iterator != newMsgs->end(); iterator++) {
		string str = "";
		str.append((*iterator));
		this->AppendMessage(str);
	}
}

string AbsVariable::ListMessages() {
	string messages = "";

	if(this->msgs->size() > 0) {
		StringVector::iterator iterator;
		for(iterator = this->msgs->begin(); iterator != this->msgs->end(); iterator++) {
			messages.append("\t- ");
			messages.append((*iterator));
			if(iterator != this->msgs->end()) {
				messages.append("\n");
			}
		}
	} else {
		messages.append("\t- No Messages found");
	}
	return messages;
}

AbsVariable* AbsVariable::SearchCache(string id) {

	AbsVariable* cachedAbsVariable = NULL;

	AbsVariableMap::iterator iterator = AbsVariable::processedVariableCache.find(id);
	if(iterator != AbsVariable::processedVariableCache.end()) {
		AbsVariablePair pair = (*iterator);
		cachedAbsVariable = pair.second;
	}

	return cachedAbsVariable;
}

void AbsVariable::ClearCache() {

	AbsVariable* var = NULL;
	
	if(!AbsVariable::processedVariableCache.empty()) {

		while(AbsVariable::processedVariableCache.size() != 0) {
			AbsVariableMap::iterator iterator = AbsVariable::processedVariableCache.begin();
			AbsVariablePair pair = (*iterator);
			var = pair.second;
			AbsVariable::processedVariableCache.erase(iterator);
	  		delete var;
	  		var = NULL;
		}

	}
}

void AbsVariable::Cache(AbsVariable* var) {

	string id = var->GetId();
	AbsVariable::processedVariableCache.insert(AbsVariablePair(id, var));
}
