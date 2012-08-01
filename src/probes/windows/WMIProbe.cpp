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

#include "WMIProbe.h"

//****************************************************************************************//
//								WMIProbe Class											  //	
//****************************************************************************************//

WMIProbe* WMIProbe::instance = NULL;

WMIProbe::WMIProbe() {

}

WMIProbe::~WMIProbe() {
  instance = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* WMIProbe::Instance() {

	// Use lazy initialization
	if(instance == NULL) 
		instance = new WMIProbe();

	return instance;	
}

ItemVector* WMIProbe::CollectItems(Object* object) {

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

Item* WMIProbe::CreateItem() {

	Item* item = new Item(0, 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows", 
						"win-sc", 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows windows-system-characteristics-schema.xsd", 
						OvalEnum::STATUS_ERROR, 
						"wmi_item");

	return item;
}

ItemEntityVector* WMIProbe::GetNamespaces(ObjectEntity* wmi_namespace) {

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

ItemEntityVector* WMIProbe::GetWQLs(ObjectEntity* wmi_wql) {

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

Item* WMIProbe::GetWMI(ItemEntity* wmi_namespace, ItemEntity* wmi_wql) {

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
			throw ProbeException("(WMIProbe) Failed to create IWbemLocator object.  " + errorMessage, ERROR_FATAL);
		}

		// Connect to the specified namespace with the current user.
		hres = pLoc->ConnectServer(_bstr_t(wmi_namespace->GetValue().c_str()), NULL, NULL, 0, NULL, 0, 0, &pSvc);
		if (FAILED(hres)) {
			string errorMessage = _com_error(hres).ErrorMessage();
			throw ProbeException("(WMIProbe) Unable to connect to the '" + wmi_namespace->GetValue() + "' namespace.  " + errorMessage, ERROR_FATAL);
		}

		// At this point we are connected to WMI.  Now set the security levels
		// of the WMI connection.
		hres = CoSetProxyBlanket(pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
		if (FAILED(hres)) {
			string errorMessage = _com_error(hres).ErrorMessage();   
 			throw ProbeException("(WMIProbe) Unable to set the WMI proxy blanket.  " + errorMessage, ERROR_FATAL);
		}

		// run the query.  The results will be stored in pEnumerator.
		hres = pSvc->ExecQuery(_bstr_t(L"WQL"), _bstr_t(wmi_wql->GetValue().c_str()), WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator);
		if (FAILED(hres)) {
			throw ProbeException("(WMIProbe) Wmi query failed. ('" + wmi_wql->GetValue() + "')", ERROR_FATAL);
		}

		IWbemClassObject *pclsObj[1];
		ULONG uReturn = 0;
		HRESULT enumhRes = pEnumerator->Next(WBEM_INFINITE, 1, pclsObj, &uReturn);

		// iterate through each instance returned
		do {
			// We have finished looping through the enumeration.  If no results
			// were returned, ie pEnumerator is empty, then we will break out of the
			// while loop. Must create an item .  This will cause the wmi
			// probe to return an empty item vector which will mean the collected
			// object in the sc file will have a does not exist flag.

			if(item == NULL){
				item = this->CreateItem();
				item->AppendElement(new ItemEntity("namespace", wmi_namespace->GetValue(), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
				item->AppendElement(new ItemEntity("wql", wmi_wql->GetValue(), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
				item->SetStatus(OvalEnum::STATUS_EXISTS);
			}

			if((uReturn == 0) || (enumhRes == WBEM_S_FALSE)) {
				item->AppendElement(new ItemEntity("result", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));
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
					errorMessage.append("(WMIProbe) There was an error retrieving one of the results.");

					item->AppendMessage(new OvalMessage(errorMessage, OvalEnum::LEVEL_ERROR));
					item->AppendElement(new ItemEntity("result", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_ERROR));

				} else {

					VARIANT vtProp;
					VariantInit(&vtProp);

					// Get the name of the property.  We need to parse the SELECT
					// statment to determine the name.
					
					string fieldName = "unknown error";
					if (this->GetWqlField(wmi_wql->GetValue(), &fieldName) == true) {
						// Once the name has been retrieved, use it to get the value
						// associated with it.

						// get the data associated with the name
						//hres = pclsObj[0]->Get(bstr_t(fieldName.c_str()), 0, &vtProp, pvtType, 0);
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

							// based on the type of data get the value of the field
							if ((V_VT(&vtProp) == VT_BSTR)) {

								char* szChar = NULL;
								size_t size = 0;
								if((size = wcstombs(0, vtProp.bstrVal, 0)) != -1) {
									szChar = new char[size + 1];
									szChar[size] = NULL;
									wcstombs(szChar, vtProp.bstrVal, size);
									strFieldValue = szChar;
									item->AppendElement(new ItemEntity("result", strFieldValue, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
                                    delete szChar;
								} else {
									item->AppendElement(new ItemEntity("result", strFieldValue, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_ERROR));
								}

							} else if ((V_VT(&vtProp) == VT_UINT)) {
								int value = V_INT(&vtProp);
								strFieldValue = Common::ToString(value);
								item->AppendElement(new ItemEntity("result", strFieldValue, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
							} else if ((V_VT(&vtProp) == VT_BOOL)) {
								bool value;								
								if ( V_BOOL(&vtProp) == VARIANT_TRUE ){
									value = true;
								}else{
									value = false;
								}
								strFieldValue = Common::ToString(value);
								item->AppendElement(new ItemEntity("result", strFieldValue, OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS));
							} else if ((V_VT(&vtProp) == VT_DATE)) {
								errorMsg = "Unsupported datatype VT_DATE found.";
							} else if ((V_VT(&vtProp) == VT_DECIMAL)) {
								errorMsg = "Unsupported datatype VT_DECIMAL found.";
							} else if ((V_VT(&vtProp) == VT_FILETIME)) {
								errorMsg = "Unsupported datatype VT_DATE found.";
							} else if ((V_VT(&vtProp) == VT_INT)) {
								int value = V_INT(&vtProp);
								strFieldValue = Common::ToString(value);
								item->AppendElement(new ItemEntity("result", strFieldValue, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
							} else if ((V_VT(&vtProp) == VT_I1)) {
								char value = V_I1(&vtProp);
								strFieldValue += value;
								item->AppendElement(new ItemEntity("result", strFieldValue, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
							} else if ((V_VT(&vtProp) == VT_I2)) {
								int value = V_I2(&vtProp);
								strFieldValue = Common::ToString(value);
								item->AppendElement(new ItemEntity("result", strFieldValue, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
							} else if ((V_VT(&vtProp) == VT_I4)) {
								long value = V_I4(&vtProp);
								strFieldValue = Common::ToString(value);
								item->AppendElement(new ItemEntity("result", strFieldValue, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
							} else if ((V_VT(&vtProp) == VT_I8)) {
								errorMsg = "Unsupported datatype VT_I8 found.";
							} else if ((V_VT(&vtProp) == VT_NULL)) {
								item->AppendElement(new ItemEntity("result", strFieldValue, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
							} else {
								errorMsg = "Unsupported datatype found.";
							}

							if(errorMsg.compare("") != 0) {
								item->AppendMessage(new OvalMessage(errorMsg, OvalEnum::LEVEL_ERROR));
								item->SetStatus(OvalEnum::STATUS_ERROR);
								Log::Debug("WMI Probe error: " + errorMsg);
							}
						}
					} else {
						
						// record error.  Should never get here as an invalid WQL statement
						// shouldn't run.
						string errorMessage = "";
						errorMessage.append("(WMIProbe) A result exists, but couldn't obtain the ");
						errorMessage.append("name of the property needed to get the result.  ");
						errorMessage.append("ERROR MESSAGE - " + fieldName);

						item->AppendMessage(new OvalMessage(errorMessage, OvalEnum::LEVEL_ERROR));
						item->AppendElement(new ItemEntity("result", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_ERROR));
					}

					VariantClear(&vtProp);
					 
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

	// clean-up
	if (pEnumerator != NULL) pEnumerator->Release();
	if (pSvc != NULL) pSvc->Release();
	if (pLoc != NULL) pLoc->Release();

	return item;
}

bool WMIProbe::GetWqlField(string wqlIn, string* fieldName) {
	int endSelect, startFrom;

	// Create a copy of the WQL string in upper case.  This way we can find the opening
	// SELECT and FROM words which are not case sensitive.

	string wqlUpperCase = wqlIn;
	for(unsigned int i=0;i<wqlIn.length();i++) {
        wqlUpperCase[i] = (char)std::toupper(wqlIn[i]);
	}

	// Find the opening SELECT statement

	endSelect = wqlUpperCase.find("SELECT ",0);
	if (endSelect == -1) {
		*fieldName = "While searching for the fieldname, couldn't find the opening SELECT.";
		return false;
	}
	endSelect = endSelect + 7;

	// Find the FROM statement.

	startFrom = wqlUpperCase.find(" FROM", endSelect);
	if (startFrom == -1) {
		*fieldName = "While searching for the fieldname, couldn't find the FROM statement.";
		return false;
	}

	*fieldName = wqlIn.substr(endSelect, startFrom-endSelect);

	// Removing any ' ' from the WQL
	string newFieldname;
	string::const_iterator iter = fieldName->begin();
	while(iter != fieldName->end())
	{
		if(*iter != ' ') {
			newFieldname = newFieldname + *iter;
		}

		iter++;
	}
	
	*fieldName = newFieldname;

	// make sure the field name is not *
	if((*fieldName).compare("*") == 0) {
		throw ProbeException("Found field name of '*'. Only a single named field is allowed");
	}

	return true;
}
