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

#ifndef ABSSTATE_H
#define ABSSTATE_H

#include <string>
#include <vector>
#include <map>
#include <utility>
#include <xercesc/dom/DOMElement.hpp>

#include "OvalEnum.h"
#include "VariableValue.h"
#include "AbsEntity.h"
#include "Exception.h"

/**
	This class represents an State in an oval definition file.
	A state can be used as a filter in data collection or as a state in analysis.
*/
class AbsState {

public:
	AbsState(OvalEnum::Operator myOperator = OvalEnum::OPERATOR_AND, int version = 1);
	AbsState(std::string id, std::string name, std::string xmlns, OvalEnum::Operator myOperator = OvalEnum::OPERATOR_AND, int version = 1);
	virtual ~AbsState();

	virtual void Parse(xercesc::DOMElement* stateElm) = 0;

	VariableValueVector GetVariableValues();

	AbsEntityVector* GetElements();
	void SetElements(AbsEntityVector* elements);

	std::string GetId();
	void SetId(std::string id);

	std::string GetName();
	void SetName(std::string name);

	OvalEnum::Operator GetOperator();
	void SetOperator(OvalEnum::Operator ovalOperator);

	std::string GetXmlns();
	void SetXmlns(std::string xmlns);

	int GetVersion();
	void SetVersion(int version);

	void AppendElement(AbsEntity* absEntity);
    
private:
	AbsEntityVector elements;
	std::string id;
	std::string name;
	OvalEnum::Operator myOperator;
	int version;
	std::string xmlns;
};

/**	
	A pair for storing  state ids and AbsState objects together. 
	Stores only pointers to the objects. 
*/
typedef std::pair <std::string, AbsState* > AbsStatePair;

/**	
	A map for storing AbsState. 
	Stores only pointers to the objects. 
*/
typedef std::map <std::string, AbsState* > AbsStateMap;

/** 
	This class represents an Exception that occured while processing an AbsState.
*/
class AbsStateException : public Exception {
	public:
		AbsStateException(std::string errMsgIn = "", int severity = ERROR_FATAL, Exception *ex = NULL);
		~AbsStateException();
};

#endif
