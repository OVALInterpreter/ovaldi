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


#include "VariableFactory.h"

//****************************************************************************************//
//								VariableFactory Class									  //
//****************************************************************************************//
DOMElement* VariableFactory::variablesElm = NULL;
// ***************************************************************************************	//
//								Public members												//
// ***************************************************************************************	//
AbsVariable* VariableFactory::GetVariable(string varId) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return a AbsVariable object for the specified id. 
	//	- check to see if the variable has already been processed.
	//	- determine what type of variable to process based on its element name
	//	- call the appropriate variable constructor
	//	- cache any newly parsed variable
	//
	// -----------------------------------------------------------------------
	
	AbsVariable* var = AbsVariable::SearchCache(varId);

	// check cache of processed vars first
	if(var == NULL) {
		// get the variables element in the definition document
		DOMElement* variablesElm = XmlCommon::FindElementNS(DocumentManager::GetDefinitionDocument(), "variables");

		// get the specific variable for this varId
		DOMElement* varElm = XmlCommon::FindElementByAttribute(variablesElm, "id", varId);

		if(varElm == NULL) {
			Log::Fatal("VariableFactory::GetVariable() - Could not find variable: " + varId + " Schema validation reqires that all referenced variables exist in the oval-definition document.");
			throw Exception("VariableFactory::GetVariable() - Could not find variable: " + varId);				 
		}

		// get the element name
		string elmName = XmlCommon::GetElementName(varElm);
		if(elmName.compare("local_variable") == 0) {
			var = new LocalVariable();
		} else if(elmName.compare("constant_variable") == 0) {
			var = new ConstantVariable();
		} else if(elmName.compare("external_variable") == 0) {
			var = new ExternalVariable();
		}

		var->SetId(varId);
		AbsVariable::Cache(var);
		try {
			var->Parse(varElm);
		} catch(Exception ex) {
			Log::Info("VariableFactory::GetVariable() - Error while parsing variable: " + varId + " " + ex.GetErrorMessage());
			throw ex;
		} catch(...) {
			Log::Info("VariableFactory::GetVariable() - Error while parsing variable: " + varId + "  Unknown Error");
			throw Exception("VariableFactory::GetVariable() - Error while parsing variable: " + varId + " Unknown Error");
		}

		// look for errors on the variable 
		if(var->GetFlag() != OvalEnum::FLAG_COMPLETE) {
			throw VariableFactoryException(var, "Error while computing variable value. \n" + var->ListMessages());
		}

	} else {
		// look for errors on the cached variable 
		if(var->GetFlag() != OvalEnum::FLAG_COMPLETE) {
			throw VariableFactoryException(var, "Error while computing variable value. \n" + var->ListMessages());
		}
	}

	return var;
}

//****************************************************************************************//
//							VariableFactoryException Class								  //	
//****************************************************************************************//
VariableFactoryException::VariableFactoryException(AbsVariable* var, string errMsgIn, int severity, Exception* ex) : Exception(errMsgIn, severity, ex) {

	this->SetVariable(var);
}

VariableFactoryException::~VariableFactoryException() {

}

void VariableFactoryException::SetVariable(AbsVariable* var) {
	this->var = var;
}

AbsVariable* VariableFactoryException::GetVariable() {
	return this->var;
}
