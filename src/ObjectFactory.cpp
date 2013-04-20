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

#include <memory>

#include "AbsObject.h"
#include "SetObject.h"
#include "Object.h"
#include "XmlCommon.h"
#include "DocumentManager.h"

#include "ObjectFactory.h"

using namespace std;
using namespace xercesc;

// ***************************************************************************************	//
//								 Public members												//
// ***************************************************************************************	//
AbsObject* ObjectFactory::GetObjectById(string objectId) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Locate the specified Object by its id in the definiion file.
	//	Parse the specified xml version of the object and return the resutling
	//	populated AbsObject
	// -----------------------------------------------------------------------

	auto_ptr<AbsObject> absObject;

	// get the specified object element
	DOMElement* objectsElm = XmlCommon::FindElementNS(DocumentManager::GetDefinitionDocument(), "objects");
	DOMElement* objectElm = XmlCommon::FindElementByAttribute(objectsElm, "id", objectId);

	// determine if this is a set object or a simple object
	DOMElement* setElm = XmlCommon::FindElementNS(objectElm, "set");
	if(setElm == NULL) {
		absObject.reset(new Object());
		absObject->Parse(objectElm);
	} else {
		absObject.reset(new SetObject());
		absObject->Parse(objectElm);
	}

	return absObject.release();
}

