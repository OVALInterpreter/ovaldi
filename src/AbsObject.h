//
// $Id: AbsObject.h 4579 2008-01-02 17:39:07Z bakerj $
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

#ifndef ABSOBJECT_H
#define ABSOBJECT_H

//	required xerces includes
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMAttr.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/parsers/AbstractDOMParser.hpp>

//	other includes
#include <string>
#include <vector>
#include <iostream>
#include <stdlib.h>

#include "Exception.h"
#include "VariableValue.h"

XERCES_CPP_NAMESPACE_USE
using namespace std;

/**
	This class represents an AbsObject in an oval definition file.
	The oval definition schema defined two types of objects, a set object and an object.
*/
class AbsObject {
public:
	virtual ~AbsObject();

	virtual void Parse(DOMElement*) = 0;
	virtual VariableValueVector* GetVariableValues() = 0;
	
	string GetId();
	void SetId(string id);

	string GetComment();
	void SetComment(string comment);

	string GetName();
	void SetName(string name);

	string GetXmlns();
	void SetXmlns(string xmlns);

	int GetVersion();
	void SetVersion(int version);

	string GetType();

protected:
	AbsObject(string id = "", string comment = "", string xmlns = "", string name = "", int version = 1);
	string type;

private:

	string id;
	string comment;
	string name;
	int version;
	string xmlns;

};

/**
	A vector for storing AbsObject objects.
	All objects are stored by reference.
*/
typedef vector < AbsObject*, allocator<AbsObject*> > AbsObjectVector;

/** 
	This class represents an Exception that occured while processing an AbsObject.
*/
class AbsObjectException : public Exception {
	public:
		AbsObjectException(string errMsgIn = "", int severity = ERROR_FATAL, Exception *ex = NULL);
		~AbsObjectException();
};

#endif
