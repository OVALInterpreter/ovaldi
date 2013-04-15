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

#include "ObjectCollector.h"

using namespace std;

//****************************************************************************************//
//								DataCollector Class										  //	
//****************************************************************************************//

ObjectCollector::ObjectCollector() : AbsObjectCollector() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Initialize the instance of the oval object collector
	// -----------------------------------------------------------------------
	
	AbsObjectCollector::instance = this;
}

ObjectCollector::~ObjectCollector() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Nothing for now
	// -----------------------------------------------------------------------
	
	ProbeFactory::Shutdown();
}

// ***************************************************************************************	//
//								Public members												//
// ***************************************************************************************	//
bool ObjectCollector::IsApplicable(AbsObject* object) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return true if the specified object is applicable to this object 
	//	collector. All objects defined in the linux, unix, or independent schemas
	//	are applicable
	// -----------------------------------------------------------------------

	bool isApplicable = false;

	if(object->GetXmlns().rfind("macos") != string::npos) {
		isApplicable = true;
	} else if(object->GetXmlns().rfind("unix") != string::npos) {
		isApplicable = true;
	} else if(object->GetXmlns().rfind("independent") != string::npos) {
		isApplicable = true;
	}

	return isApplicable;
}

bool ObjectCollector::IsSupported(AbsObject* object) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return true if the specified object is supported by this object 
	//	collector. 
	// -----------------------------------------------------------------------

	bool isSupported = false;

	if(ProbeFactory::GetProbe(object->GetName()) != NULL) {
		isSupported = true;
	}

	return isSupported;
}

AbsProbe* ObjectCollector::GetProbe(Object* object) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return true if the specified object is supported to this object 
	//	collector
	// -----------------------------------------------------------------------

	return ProbeFactory::GetProbe(object->GetName());
}
