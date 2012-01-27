//
//
//****************************************************************************************//
// Copyright (c) 2002-2012, The MITRE Corporation
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

#include <ItemFieldEntityValue.h>

#include "WMI57Probe.h"

//****************************************************************************************//
//								WMI57Probe Class											  //	
//****************************************************************************************//

WMI57Probe* WMI57Probe::instance = NULL;

WMI57Probe::WMI57Probe() {

}

WMI57Probe::~WMI57Probe() {
  instance = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* WMI57Probe::Instance() {

	// Use lazy initialization
	if(instance == NULL) 
		instance = new WMI57Probe();

	return instance;	
}

ItemVector* WMI57Probe::CollectItems(Object* object) {

	// get the namespace and wql query
	ObjectEntity* wmi_namespace = object->GetElementByName("namespace");
	ObjectEntity* wmi_wql = object->GetElementByName("wql");

	// check datatypes - only allow string
	if(wmi_namespace->GetDatatype() != OvalEnum::DATATYPE_STRING) {
		throw ProbeException("Error: invalid data type specified on namespace. Found: " + OvalEnum::DatatypeToString(wmi_namespace->GetDatatype()));
	}
	if(wmi_wql->GetDatatype() != OvalEnum::DATATYPE_STRING) {
		throw ProbeException("Error: invalid data type specified on wql. Found: " + OvalEnum::DatatypeToString(wmi_wql->GetDatatype()));
	}

	// check operation - only allow equals
	if(wmi_namespace->GetOperation() != OvalEnum::OPERATION_EQUALS) {
		throw ProbeException("Error: invalid operation specified on namespace. Found: " + OvalEnum::OperationToString(wmi_namespace->GetOperation()));
	}
	if(wmi_wql->GetOperation() != OvalEnum::OPERATION_EQUALS) {
		throw ProbeException("Error: invalid operation specified on wql. Found: " + OvalEnum::OperationToString(wmi_wql->GetOperation()));
	}

	ItemVector* collectedItems = new ItemVector();
	StringVector fields;

	// get all the namespaces
	ItemEntityVector* namespaces = this->GetNamespaces(wmi_namespace);
	ItemEntityVector::iterator namespaceIt;
	for(namespaceIt=namespaces->begin(); namespaceIt!=namespaces->end(); namespaceIt++) {
		
			// get all the wql queries
			ItemEntityVector* wqls = this->GetWQLs(wmi_wql);
			ItemEntityVector::iterator wqlIt;
			for(wqlIt=wqls->begin(); wqlIt!=wqls->end(); wqlIt++) {

				// run the wmi queries and push the results on the collectedItems
				// return vector.
				Item* item = this->GetWMI((*namespaceIt), (*wqlIt));
				if(item != NULL) {
					collectedItems->push_back(item);
				}
			}
			delete wqls;
	}
	delete namespaces;

	return collectedItems;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

Item* WMI57Probe::CreateItem() {

	Item* item = new Item(0, 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows", 
						"win-sc", 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows windows-system-characteristics-schema.xsd", 
						OvalEnum::STATUS_ERROR, 
						"wmi57_item");

	return item;
}

ItemEntityVector* WMI57Probe::GetNamespaces(ObjectEntity* wmi_namespace) {

	ItemEntityVector* namespaces = new ItemEntityVector();

	// does the namespace entity use variables?
	if(wmi_namespace->GetVarRef() == NULL) {
		
		ItemEntity* tmp = this->CreateItemEntity(wmi_namespace);
		tmp->SetValue(wmi_namespace->GetValue());
		namespaces->push_back(tmp);

	} else {

		// retrieve all the variable values that match the supplied var_ref.
		VariableValueVector* vars = wmi_namespace->GetVariableValues();

		// we may need to add a check to see if the namespace exists here?

		// loop through all values
		VariableValueVector::iterator iterator;
		for(iterator = vars->begin(); iterator != vars->end(); iterator++) {

			ItemEntity* tmp = this->CreateItemEntity(wmi_namespace);
			tmp->SetValue((*iterator)->GetValue());
			namespaces->push_back(tmp);
		}
	}

	return namespaces;
}

ItemEntityVector* WMI57Probe::GetWQLs(ObjectEntity* wmi_wql) {

	ItemEntityVector* wqls = new ItemEntityVector();

	// does this key use variables?
	if(wmi_wql->GetVarRef() == NULL) {
		
		ItemEntity* tmp = this->CreateItemEntity(wmi_wql);
		tmp->SetValue(wmi_wql->GetValue());
		wqls->push_back(tmp);
	
	} else {

		// retrieve all the variable values that match the supplied var_ref.
		VariableValueVector* vars = wmi_wql->GetVariableValues();

		// loop through all values
		VariableValueVector::iterator iterator;
		for(iterator = vars->begin(); iterator != vars->end(); iterator++) {

			ItemEntity* tmp = this->CreateItemEntity(wmi_wql);
			tmp->SetValue((*iterator)->GetValue());
			wqls->push_back(tmp);
		}
	}

	return wqls;
}

Item* WMI57Probe::GetWMI(ItemEntity* wmi_namespace, ItemEntity* wmi_wql) {

	IWbemLocator *pLoc = NULL;
	IWbemServices *pSvc = NULL;
	IEnumWbemClassObject* pEnumerator = NULL;

	Item* item = NULL;

	try {
		HRESULT hres;

		// find the WMI Locator
		hres = CoCreateInstance(CLSID_WbemLocator, NULL, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID *) &pLoc);
		if (FAILED(hres)) {
			string errorMessage = _com_error(hres).ErrorMessage();
			throw ProbeException("(WMI57Probe) Failed to create IWbemLocator object.  " + errorMessage, ERROR_FATAL);
		}

		// Connect to the specified namespace with the current user.
		hres = pLoc->ConnectServer(_bstr_t(wmi_namespace->GetValue().c_str()), NULL, NULL, 0, NULL, 0, 0, &pSvc);
		if (FAILED(hres)) {
			string errorMessage = _com_error(hres).ErrorMessage();
			throw ProbeException("(WMI57Probe) Unable to connect to the '" + wmi_namespace->GetValue() + "' namespace.  " + errorMessage, ERROR_FATAL);
		}

		// At this point we are connected to WMI.  Now set the security levels
		// of the WMI connection.
		hres = CoSetProxyBlanket(pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
		if (FAILED(hres)) {
			string errorMessage = _com_error(hres).ErrorMessage();   
 			throw ProbeException("(WMI57Probe) Unable to set the WMI proxy blanket.  " + errorMessage, ERROR_FATAL);
		}
	
		// run the query.  The results will be stored in pEnumerator.

		hres = pSvc->ExecQuery(_bstr_t(L"WQL"), _bstr_t(wmi_wql->GetValue().c_str()), WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator);
		if (FAILED(hres)) {
			throw ProbeException("(WMI57Probe) Wmi query failed. ('" + wmi_wql->GetValue() + "')", ERROR_FATAL);
		}

		IWbemClassObject *pclsObj[1];
		ULONG uReturn = 0;
		HRESULT enumhRes = pEnumerator->Next(WBEM_INFINITE, 1, pclsObj, &uReturn);
		string errorMessage = _com_error(hres).ErrorMessage();
				
		// create item
		item = this->CreateItem();
		item->AppendElement(new ItemEntity("namespace", wmi_namespace->GetValue(), OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
		item->AppendElement(new ItemEntity("wql", wmi_wql->GetValue(), OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
		item->SetStatus(OvalEnum::STATUS_EXISTS);
	
		// iterate through each instance returned and create a result entity for it.
		do {
			// We have finished looping through the enumeration.  If no results
			// were returned, ie pEnumerator is empty, then we will break out of the
			// while loop. Must create an item .  This will cause the wmi
			// probe to return an empty item vector which will mean the collected
			// object in the sc file will have a does not exist flag.
			if((uReturn == 0) || (enumhRes == WBEM_S_FALSE)) {
				item->AppendElement(new ItemEntity("result", "", OvalEnum::DATATYPE_RECORD, false, OvalEnum::STATUS_DOES_NOT_EXIST));
				break;
			} else {
				// We have a result.  Create an ItemEntity for it and add it to the
				// item.

				if ((enumhRes == WBEM_E_INVALID_PARAMETER) ||
					(enumhRes == WBEM_E_OUT_OF_MEMORY) ||
					(enumhRes == WBEM_E_UNEXPECTED) ||
					(enumhRes == WBEM_E_TRANSPORT_FAILURE) ||
					(enumhRes == WBEM_S_TIMEDOUT)) {

					string errorMessage = "";
					errorMessage.append("(WMI57Probe) There was an error retrieving one of the results.");

					item->AppendMessage(new OvalMessage(errorMessage, OvalEnum::LEVEL_ERROR));
					item->AppendElement(new ItemEntity("result", "", OvalEnum::DATATYPE_RECORD, false, OvalEnum::STATUS_ERROR));

				} else {

					VARIANT vtProp;
					VariantInit(&vtProp);

					// Get the name of the property.  We need to parse the SELECT
					// statment to determine the name.
					StringVector* fieldNames = this->GetWqlFields(wmi_wql->GetValue(),WMI57Probe::SELECT);
					AbsEntityValueVector fieldEntityValues;
					
					for(StringVector::iterator it = fieldNames->begin(); it != fieldNames->end(); it++){
						// Once the name has been retrieved, use it to get the value
						// associated with it.
						string fieldName = *it;
						hres = pclsObj[0]->Get(bstr_t(fieldName.c_str()), 0, &vtProp, 0, 0);
						string errorMsg = "";

						if(hres == WBEM_E_NOT_FOUND) {
							errorMsg = "WBEM_E_NOT_FOUND for wql: " + wmi_wql->GetValue();
                        } else if(hres == WBEM_E_OUT_OF_MEMORY) {
							errorMsg = "WBEM_E_OUT_OF_MEMORY for wql: " + wmi_wql->GetValue();
						} else if(hres == WBEM_E_INVALID_PARAMETER) {
							errorMsg = "WBEM_E_INVALID_PARAMETER for wql: " + wmi_wql->GetValue();
						} else if(hres == WBEM_E_FAILED) {
							errorMsg = "WBEM_E_FAILED for wql: " + wmi_wql->GetValue();
						} else if(hres == WBEM_S_NO_ERROR) {

							string strFieldValue = "";
							//convert fieldName to all lowercase as defined in OVAL Language (see oval-sc:FieldType)
							fieldName = Common::ToLower(fieldName);
							// based on the type of data get the value of the field
							if ((V_VT(&vtProp) == VT_BSTR)) {

								char* szChar = NULL;
								size_t size = 0;
								if((size = wcstombs(0, vtProp.bstrVal, 0)) != -1) {
									szChar = new char[size + 1];
									szChar[size] = NULL;
									wcstombs(szChar, vtProp.bstrVal, size);
									strFieldValue = szChar;
									fieldEntityValues.push_back(new ItemFieldEntityValue(fieldName, strFieldValue, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));                                 
									delete szChar;
								} else {
									fieldEntityValues.push_back(new ItemFieldEntityValue(fieldName, strFieldValue, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_ERROR));
								}

							} else if ((V_VT(&vtProp) == VT_UINT)) {
								int value = V_INT(&vtProp);
								strFieldValue = Common::ToString(value);
								fieldEntityValues.push_back(new ItemFieldEntityValue(fieldName, strFieldValue, OvalEnum::DATATYPE_INTEGER, OvalEnum::STATUS_EXISTS));
							} else if ((V_VT(&vtProp) == VT_BOOL)) {
								bool value;								
								if ( V_BOOL(&vtProp) == VARIANT_TRUE ){
									value = true;
								}else{
									value = false;
								}
								strFieldValue = Common::ToString(value);
								fieldEntityValues.push_back(new ItemFieldEntityValue(fieldName, strFieldValue, OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS));
							} else if ((V_VT(&vtProp) == VT_DATE)) {
								errorMsg = "Unsupported datatype VT_DATE found.";
							} else if ((V_VT(&vtProp) == VT_DECIMAL)) {
								errorMsg = "Unsupported datatype VT_DECIMAL found.";
							} else if ((V_VT(&vtProp) == VT_FILETIME)) {
								errorMsg = "Unsupported datatype VT_FILETIME found.";
							} else if ((V_VT(&vtProp) == VT_INT)) {
								int value = V_INT(&vtProp);
								strFieldValue = Common::ToString(value);                                  
								fieldEntityValues.push_back(new ItemFieldEntityValue(fieldName, strFieldValue, OvalEnum::DATATYPE_INTEGER, OvalEnum::STATUS_EXISTS));
							} else if ((V_VT(&vtProp) == VT_I1)) {
								char value = V_I1(&vtProp);
								strFieldValue += value;
								fieldEntityValues.push_back(new ItemFieldEntityValue(fieldName, strFieldValue, OvalEnum::DATATYPE_INTEGER, OvalEnum::STATUS_EXISTS));
							} else if ((V_VT(&vtProp) == VT_I2)) {
								int value = V_I2(&vtProp);
								strFieldValue = Common::ToString(value);
								fieldEntityValues.push_back(new ItemFieldEntityValue(fieldName, strFieldValue, OvalEnum::DATATYPE_INTEGER, OvalEnum::STATUS_EXISTS));
							} else if ((V_VT(&vtProp) == VT_I4)) {
								long value = V_I4(&vtProp);
								strFieldValue = Common::ToString(value);
								fieldEntityValues.push_back(new ItemFieldEntityValue(fieldName, strFieldValue, OvalEnum::DATATYPE_INTEGER, OvalEnum::STATUS_EXISTS));
							} else if ((V_VT(&vtProp) == VT_I8)) {
								errorMsg = "Unsupported datatype VT_I8 found.";
							} else if ((V_VT(&vtProp) == VT_NULL)) {
								fieldEntityValues.push_back(new ItemFieldEntityValue(fieldName, strFieldValue, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
							} else {
								errorMsg = "Unsupported datatype found.";
							}

							if(errorMsg.compare("") != 0) {
								item->AppendMessage(new OvalMessage(errorMsg, OvalEnum::LEVEL_ERROR));
								item->SetStatus(OvalEnum::STATUS_ERROR);
								Log::Debug("WMI Probe error: " + errorMsg);
							}
						}
					}
					// If we found values add them. Otherwise, make sure to report the result doesn't exist
					if ( fieldEntityValues.size() ){
						item->AppendElement(new ItemEntity("result",fieldEntityValues,OvalEnum::DATATYPE_RECORD,false,OvalEnum::STATUS_EXISTS));
					}else{
						item->AppendElement(new ItemEntity("result","",OvalEnum::DATATYPE_RECORD,false,OvalEnum::STATUS_DOES_NOT_EXIST));					}
					}
				
				for (ULONG n=0; n<uReturn; n++) pclsObj[n]->Release();
			}
		}
		while ( ( enumhRes = pEnumerator->Next(WBEM_INFINITE, 1, pclsObj, &uReturn) ) == WBEM_S_NO_ERROR  );
	} catch (ProbeException ex) {
		// Make sure we clean up if there is an error, otherwise we will get an COM
		// security error when we try to run the wmi probe again.

		if (pEnumerator != NULL) pEnumerator->Release();
		if (pSvc != NULL) pSvc->Release();
		if (pLoc != NULL) pLoc->Release();

		// re-throw the error so it can be caught higher up.

		throw ex;
	} catch (...) {
		// Make sure we clean up if there is an error, otherwise we will get an COM
		// security error when we try to run the wmi probe again.

		if (pEnumerator != NULL) pEnumerator->Release();
		if (pSvc != NULL) pSvc->Release();
		if (pLoc != NULL) pLoc->Release();

		// re-throw the error so it can be caught higher up.

		throw ProbeException("An unknown error occured while executing a wql.");
	}

	// Clean-up
	if (pEnumerator != NULL) pEnumerator->Release();
	if (pSvc != NULL) pSvc->Release();
	if (pLoc != NULL) pLoc->Release();

	return item;
}

StringVector* WMI57Probe::GetWqlFields(string wqlIn, WQLFieldType wqlFieldType) {
	IWbemQuery *wqlQuery = NULL;
	HRESULT hResult = NULL;
	StringVector* fieldNames = new StringVector();

	if ( FAILED((hResult = CoCreateInstance(CLSID_WbemQuery,0,CLSCTX_INPROC_SERVER,IID_IWbemQuery,(LPVOID *)&wqlQuery)))) {
			string errorMessage = _com_error(hResult).ErrorMessage();
			throw ProbeException("(WMI57Probe) Failed to create IWbemQuery object.  " + errorMessage, ERROR_FATAL);
	}

	if ( (hResult = wqlQuery->Parse(L"WQL",WindowsCommon::StringToWide(wqlIn),0)) == WBEM_S_NO_ERROR ){
		SWbemRpnEncodedQuery* wqlAnalysis = NULL;
		if ( (hResult = wqlQuery->GetAnalysis(WMIQ_ANALYSIS_RPN_SEQUENCE,0,(LPVOID *)&wqlAnalysis)) == WBEM_S_NO_ERROR ){
			switch(wqlFieldType){
				case WMI57Probe::SELECT:{
					for(unsigned long i = 0; i < wqlAnalysis->m_uSelectListSize; i++){
						for(unsigned long j = 0; j < wqlAnalysis->m_ppSelectList[i]->m_uNameListSize; j++){
							string field = WindowsCommon::UnicodeToAsciiString((wchar_t*)wqlAnalysis->m_ppSelectList[i]->m_ppszNameList[j]);
							if ( field.compare("*") != 0 ){
								fieldNames->push_back(field);
							}else{
								fieldNames->clear();
								delete fieldNames;
								throw ProbeException("Found a field name of '*'.  WQL queries that use '*' are not permitted as defined in the windows-definitions-schema.");
							}
						}
					}
					break;
				}
				case WMI57Probe::WHERE:{ 
					//Not used right now, but, here in case we need it.
					break;
				}
				case WMI57Probe::FROM:{
					//Not used right now, but, here in case we need it.
					break;
				}
				default:{
					break;	
				}
			}
			
		}
		//Free allocated memory
		if ( (hResult = wqlQuery->FreeMemory(wqlAnalysis)) != WBEM_S_NO_ERROR){
			string errorMessage = _com_error(hResult).ErrorMessage();
			throw ProbeException("(WMI57Probe) Failed to free the memory that is returned to the call to GetAnalysis() by the parser. "+ errorMessage, ERROR_FATAL);
		}

		if ( (hResult = wqlQuery->Empty()) != WBEM_S_NO_ERROR){
			string errorMessage = _com_error(hResult).ErrorMessage();
			throw ProbeException("(WMI57Probe) Failed to free the memory that is held by the parser. "+ errorMessage, ERROR_FATAL);
		}
	}else{
		string errorMessage = _com_error(hResult).ErrorMessage();
		throw ProbeException("(WMI57Probe) Failed to parse the wql.  " + errorMessage, ERROR_FATAL);
	}
	
	return fieldNames;
}
