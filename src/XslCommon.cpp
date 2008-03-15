//
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

#include "XslCommon.h"

//****************************************************************************************//
//										XslCommon Class									  //	
//****************************************************************************************//

void XslCommon::ApplyXSL(string xmlIn, string xslIn, string xmlOut) {

	// 2. Initialize Xalan and Xerces
	XalanInitialize();

	// 3. Create a Xalan transformer
	XalanHandle xalan = NULL;
	xalan = CreateXalanTransformer();

	// 4. Perform each transformation
	int theResult = 0;
	theResult = XalanTransformToFile(xmlIn.c_str(),xslIn.c_str(), xmlOut.c_str(), xalan);

	// 5. Shut down Xalan
	XalanTerminate(false);
}

string XslCommon::ApplyXSL(string xmlIn, string xslIn) {

	// 2. Initialize Xalan and Xerces
	XalanInitialize();

	// 3. Create a Xalan transformer
	XalanHandle xalan = NULL;
	xalan = CreateXalanTransformer();

	char* theData = NULL;

	// 4. Perform each transformation
	int theResult = 0;
	theResult = XalanTransformToData(xmlIn.c_str(),xslIn.c_str(), &theData, xalan);

	string result = "";
	result.append(theData);

	XalanFreeData(theData);

	// 5. Shut down Xalan
	XalanTerminate(false);

	return result;
}
