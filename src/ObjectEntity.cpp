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

#include "ObjectEntity.h"
#include "StateOrObjectFieldEntityValue.h"

using namespace std;

//****************************************************************************************//
//								ObjectEntity Class										  //	
//****************************************************************************************//
ObjectEntity::ObjectEntity(ObjectEntity* orig) : AbsEntity() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Copy constructor.
	//	Create a complete ObjectEntity object based on the specified obj.
	//
	// -----------------------------------------------------------------------

	this->SetDatatype(orig->GetDatatype());
	this->SetName(orig->GetName());
	this->SetNil(orig->GetNil());
	this->SetOperation(orig->GetOperation());
	this->SetValue(orig->GetValue());
	this->SetVarCheck(orig->GetVarCheck());
	this->SetVarRef(orig->GetVarRef());
}

ObjectEntity::ObjectEntity(string name, string value, OvalEnum::Datatype datatype, OvalEnum::Operation operation, AbsVariable* varRef, OvalEnum::Check varCheck, bool nil)
									: AbsEntity(name, value, datatype, operation, varRef, varCheck, nil) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Create a complete ObjectEntity object
	//
	// -----------------------------------------------------------------------
}

ObjectEntity::~ObjectEntity() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Do nothing for now
	//
	// -----------------------------------------------------------------------
}

// ***************************************************************************************	//
//								 Public members												//
// ***************************************************************************************	//
void ObjectEntity::Parse(DOMElement* objectEntityElm) {

	this->SetName(XmlCommon::GetElementName(objectEntityElm));
	this->SetOperation(OvalEnum::ToOperation(XmlCommon::GetAttributeByName(objectEntityElm, "operation")));
	this->SetDatatype(OvalEnum::ToDatatype(XmlCommon::GetAttributeByName(objectEntityElm, "datatype")));
	this->SetVarCheck(OvalEnum::ToCheck(XmlCommon::GetAttributeByName(objectEntityElm, "var_check")));

	// The datatype is not 'record' so we can just grab the string value in the element.
	if ( this->GetDatatype() != OvalEnum::DATATYPE_RECORD ){
		this->SetValue(XmlCommon::GetDataNodeValue(objectEntityElm));
	}else{
		// The datatype is 'record' so we need to loop over all of the elements (which are the fields).
		DOMNodeList *objectEntityChildren = objectEntityElm->getChildNodes();
		unsigned int index = 0;
		AbsEntityValueVector parsedValues;
		while(index < objectEntityChildren->getLength()) {
			DOMNode* tmpNode = objectEntityChildren->item(index);

			if (tmpNode->getNodeType() == DOMNode::ELEMENT_NODE) {
				DOMElement* objectEntityChild = (DOMElement*)tmpNode;

				//	get the name of the child
				string childName = XmlCommon::GetElementName(objectEntityChild);
				if ( childName == "field" || childName == "oval-def:field"){
					StateOrObjectFieldEntityValue* fev = new StateOrObjectFieldEntityValue();
					fev->Parse(objectEntityChild);
					parsedValues.push_back(fev);
				}else{
					// shouldn't happen, if we validated against the xml schema...
					throw Exception("Encountered a non-field child element of "
						"an object entity with record type (" + childName + ")");
				}
			}
			index ++;
		}	
		this->SetValues(parsedValues);
	}

	// to support version 5.3 it is best to just look for the deprected check = none exist 
	// and report it in the log
	if(this->GetVarCheck() == OvalEnum::CHECK_NONE_EXIST) {
		this->SetVarCheck(OvalEnum::CHECK_NONE_SATISFY);
		Log::Info("DEPRECATED var_check value: The \'none exist\' CheckEnumeration value has been deprecated and will be removed with the next major version of the language. The OVAL Interpreter has mapped this value to \'none satisfy\'");
	}

	// get the nill attribute
	string nilAttr = XmlCommon::GetAttributeByName(objectEntityElm, "xsi:nil");
	if(nilAttr.compare("") == 0 || nilAttr.compare("false") == 0) {
		this->SetNil(false);
	} else {
		this->SetNil(true);
	}

	// get variable value if needed
	string varRefStr = XmlCommon::GetAttributeByName(objectEntityElm, "var_ref");
	if(varRefStr.compare("") != 0) {
		//Log::Debug("ObjectEntity::Parse() - Found var_ref on object entity var id: " + varRefStr);
		AbsVariable* var = VariableFactory::GetVariable(varRefStr);
		this->SetValue("");
		this->SetVarRef(var);
	} else {
		this->SetVarRef(NULL);
	}
}
