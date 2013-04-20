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

#include <windows.h>
#include <comdef.h>
#include <Dsgetdc.h>
#include <Lm.h>
#include <Wbemidl.h>

#include "Common.h"
#include "Exception.h"
#include "Log.h"

#include "WMIUtil.h"

using namespace std;

namespace {
	string GetStringFromVariant(VARIANT value);
}

static IEnumWbemClassObject* pEnumerator = NULL;
static IWbemLocator *pLoc = NULL;
static IWbemServices *pSvc = NULL;

void WMIUtil::Open(std::string wmi_namespace) {

	HRESULT hres;

	// find the WMI Locator
	hres = CoCreateInstance(CLSID_WbemLocator, NULL, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID *) &pLoc);
	if (FAILED(hres)) {
		string errorMessage = _com_error(hres).ErrorMessage();
		throw Exception("(WMIUtil) Failed to create IWbemLocator object.  " + errorMessage, ERROR_FATAL);
	}

	// Connect to the specified namespace with the current user.
	hres = pLoc->ConnectServer(_bstr_t(wmi_namespace.c_str()), NULL, NULL, 0, NULL, 0, 0, &pSvc);
	if (FAILED(hres)) {
		string errorMessage = _com_error(hres).ErrorMessage();
		throw Exception("(WMIUtil) Unable to connect to the '" + wmi_namespace + "' namespace.  " + errorMessage, ERROR_FATAL);
	}

	// At this point we are connected to WMI.  Now set the security levels
	// of the WMI connection.
	hres = CoSetProxyBlanket(pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
	if (FAILED(hres)) {
		string errorMessage = _com_error(hres).ErrorMessage();   
		throw Exception("(WMIUtil) Unable to set the WMI proxy blanket.  " + errorMessage, ERROR_FATAL);
	}
}

// Input: map of key/property names
// Returns: map of key/property names with value tacked onto end of property name after semicolon
std::vector<WMIItem> WMIUtil::GetPropertyValues(std::string wmi_namespace, std::string wmi_query,std::string keyName, std::string propertyName) {

	IWbemClassObject *pclsObj[1];
	pclsObj[0] = NULL;
	std::vector<WMIItem> results;

	try {
		ULONG uReturn = 0;
		HRESULT hres;	
		HRESULT enumhRes;

		Open(wmi_namespace);

		// Run the query.  The results will be stored in pEnumerator.
		hres = pSvc->ExecQuery(_bstr_t(L"WQL"), _bstr_t(wmi_query.c_str()), WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator);
		if (FAILED(hres)) {
			throw Exception("(WMIUtil) Wmi query failed. ('" + wmi_query + "')", ERROR_FATAL);
		}

		while (((enumhRes = pEnumerator->Next(WBEM_INFINITE, 1, pclsObj, &uReturn)) == WBEM_S_NO_ERROR) && (uReturn > 0)) {
			WMIItem wmiItem;
			VARIANT keyname;

			// Get the keyname and determine if this is an object we should be concerned with.
			HRESULT result = pclsObj[0]->Get(bstr_t(keyName.c_str()), 0, &keyname, NULL, NULL);

			if(SUCCEEDED(result)) {	
				wmiItem.KeyName = GetStringFromVariant(keyname);

				VariantClear(&keyname);

				VARIANT propertyValue;

				// Now get the property
				HRESULT result = pclsObj[0]->Get(bstr_t(propertyName.c_str()), 0, &propertyValue, NULL, NULL);

				if(SUCCEEDED(result)) {
					wmiItem.PropertyName = propertyName;
					wmiItem.PropertyValue = GetStringFromVariant(propertyValue);

					VariantClear(&propertyValue);

					results.push_back(wmiItem);
				} else {
					// Ignore the fact can't retrieve the propertyname - TODO: OK?
				}					
			} else {
				// Ignore the fact this object doesn't have a keyname property - TODO: OK?
			}

			pclsObj[0]->Release();
			pclsObj[0] = NULL;
		}

	} catch(...) {
		if(pclsObj[0] != NULL) {
			pclsObj[0]->Release();
			pclsObj[0] = NULL;
		}

		Close();

		throw;
	}

	if(pclsObj[0] != NULL) {
		pclsObj[0]->Release();
		pclsObj[0] = NULL;
	}

	Close();

	return results;
}

void WMIUtil::Close() {

	if(pEnumerator != NULL) {
		pEnumerator->Release();
	}

	if(pLoc != NULL) {
		pLoc->Release();
	}

	if(pSvc != NULL) {
		pSvc->Release();
	}
}

namespace {

	string GetStringFromVariant(VARIANT value) {

		string stringValue = "";

		if ((V_VT(&value) == VT_BSTR)) {
			char* szChar = NULL;
			size_t size = 0;
			if((size = wcstombs(0, value.bstrVal, 0)) != -1) 
			{
				szChar = new char[size + 1];
				szChar[size] = NULL;
				wcstombs(szChar, value.bstrVal, size);
				stringValue = szChar;		
						
				delete(szChar);
			} else {
				// TODO - currently logging the error, but should an exception be thrown?
				Log::Debug("WMIUtil::GetStringFromVariant() - Error converting value to string.");
			}
		} else if ((V_VT(&value) == VT_UINT)) {
			unsigned int intValue = V_INT(&value);
			stringValue = Common::ToString(intValue);
		} else if ((V_VT(&value) == VT_BOOL)) {
			BOOL boolValue = V_BOOL(&value);
			if(boolValue == FALSE) {
				stringValue = "0";
			} else {
				stringValue = "1";
			}
		} else if ((V_VT(&value) == VT_DATE)) {
			//TODO - need to format VT_DATE
		} else if ((V_VT(&value) == VT_DECIMAL)) {
			//TODO - need to format VT_DECIMAL
		} else if ((V_VT(&value) == VT_FILETIME)) {
			//TODO - need to format VT_FILETIME
		} else if ((V_VT(&value) == VT_INT)) {
			int intValue = V_INT(&value);
			stringValue = Common::ToString(intValue);
		} else if ((V_VT(&value) == VT_I1)) {
			char charValue = V_I1(&value);
			stringValue += charValue;
		} else if ((V_VT(&value) == VT_I2)) {
			int intValue = V_I2(&value);
			stringValue = Common::ToString(intValue);
		} else if ((V_VT(&value) == VT_I4)) {
			char valBuf[20];
			long longValue = V_I4(&value);
			stringValue = _ltoa(longValue, valBuf, 10);
		} else if ((V_VT(&value) == VT_I8)) {
			// TODO - need to format VT_I8
		} else if ((V_VT(&value) == VT_NULL)) {
			// TODO - Not sure what to do with VT_NULL
		} else {
			// TODO Unsupported data type
		}

		return stringValue;
	}

}

