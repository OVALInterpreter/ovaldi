//
//
//****************************************************************************************//
// Copyright (c) 2002-2011, The MITRE Corporation
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

using namespace std;

// ***************************************************************************************	//
//								 Public members												//
// ***************************************************************************************	//
AbsComponent* ComponentFactory::GetComponent(DOMElement* componentElm) {

	AbsComponent* absComponent = NULL;

	// determine if this is a set object or a simple object
	string elmName = XmlCommon::GetElementName(componentElm);
	if(elmName.compare("variable_component")  == 0) {
		absComponent = new VariableComponent();
	} else if(elmName.compare("literal_component")  == 0) {
		absComponent = new LiteralComponent();
	} else if(elmName.compare("object_component")  == 0) {
		absComponent = new ObjectComponent();
	//
	// functions 
	//
	} else if(elmName.compare("substring")  == 0) {
		absComponent = new SubstringFunction();
	} else if(elmName.compare("concat")  == 0) {
		absComponent = new ConcatFunction();
	} else if(elmName.compare("escape_regex")  == 0) {
		absComponent = new EscapeRegexFunction();
	} else if(elmName.compare("begin")  == 0) {
        absComponent = new BeginFunction();
	} else if(elmName.compare("end")  == 0) {
        absComponent = new EndFunction();
	} else if(elmName.compare("split")  == 0) {
        absComponent = new SplitFunction();
	} else if(elmName.compare("time_difference")  == 0) {
        absComponent = new TimeDifferenceFunction();
    } else if(elmName.compare("regex_capture")  == 0) {
        absComponent = new RegexCaptureFunction();
    } else if(elmName.compare("arithmetic")  == 0) {
        absComponent = new ArithmeticFunction();
    } else {
		throw Exception("Error: unsupported function: " + elmName);
	}

    absComponent->Parse(componentElm);

	return absComponent;
}
