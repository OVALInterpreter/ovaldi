//
//
//****************************************************************************************//
// Copyright (c) 2002-2009, The MITRE Corporation
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


#ifndef WMIUTIL_H
#define WMIUTIL_H

// No need to include <comdefsp.h> which causes conflicts with the XERCES headers
#define _INC_COMDEFSP
#include <comdef.h>
#undef _INC_COMDEFSP

#include <Dsgetdc.h>
#include <Lm.h>
#include <Wbemidl.h>
#include <windows.h>

#include <string>
#include <vector>
#include <map>

#include "Exception.h"
#include "Log.h"
#include "WMIItem.h"

using namespace std;

/**
	This class is responsible for collecting information from WMI.

    This class is intended to be used when no other API based solution is available.
*/
class WMIUtil {

public:
	~WMIUtil();

	static std::vector<WMIItem> GetPropertyValues(std::string wmi_namespace, std::string wmi_query, std::string keyName, std::string propertyName);

private:
	static void Open(std::string wmi_namespace);
	static std::string GetStringFromVariant(VARIANT value);
	static void Close();
};

#endif
