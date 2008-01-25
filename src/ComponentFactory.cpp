//
// $Id: ComponentFactory.cpp 4579 2008-01-02 17:39:07Z bakerj $
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

#include "ComponentFactory.h"

// ***************************************************************************************	//
//								 Public members												//
// ***************************************************************************************	//
AbsComponent* ComponentFactory::GetComponent(DOMElement* componentElm) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return the appropriate component based on the specifed component element
	// -----------------------------------------------------------------------

	AbsComponent* absComponent = NULL;

	// determine if this is a set object or a simple object
	string elmName = XmlCommon::GetElementName(componentElm);
	if(elmName.compare("variable_component")  == 0) {
		absComponent = new VariableComponent();
		absComponent->Parse(componentElm);
	} else if(elmName.compare("literal_component")  == 0) {
		absComponent = new LiteralComponent();
		absComponent->Parse(componentElm);
	} else if(elmName.compare("object_component")  == 0) {
		absComponent = new ObjectComponent();
		absComponent->Parse(componentElm);
	} else if(elmName.compare("substring")  == 0) {
		absComponent = new SubstringFunction();
		absComponent->Parse(componentElm);
	} else if(elmName.compare("concat")  == 0) {
		absComponent = new ConcatFunction();
		absComponent->Parse(componentElm);
	} else {
		throw Exception("Error: unsupported function: " + elmName);
	}

	return absComponent;
}

