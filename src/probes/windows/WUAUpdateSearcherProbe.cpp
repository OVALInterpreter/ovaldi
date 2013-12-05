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

#include "WUAUpdateSearcherProbe.h"

//****************************************************************************************//
//						WUAUpdateSearcherProbe Class									  //	
//****************************************************************************************//

WUAUpdateSearcherProbe* WUAUpdateSearcherProbe::instance = NULL;

WUAUpdateSearcherProbe::WUAUpdateSearcherProbe() {
	pIUpdateSession = NULL;
	pIUpdateServiceManager = NULL;
	pIUSearcher = NULL;
	pIUpdateCollection = NULL;
	pISearchResult = NULL;
}

WUAUpdateSearcherProbe::~WUAUpdateSearcherProbe() {
  instance = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* WUAUpdateSearcherProbe::Instance() {

	// Use lazy initialization
	if(instance == NULL) 
		instance = new WUAUpdateSearcherProbe();

	return instance;	
}

/*
void WUAUpdateSearcherProbe::DeleteInstance() {
	if(instance != NULL) {
		delete instance;
		instance = NULL;
	}
}
*/

ItemVector* WUAUpdateSearcherProbe::CollectItems(Object* object) {

	// get the search_criteria
    /** 
        The search_criteria entity specifies a search criteria to use when 
        generating a search result.  The string used for the search criteria 
        entity must match the custom search language for Search method of the
        IUpdateSearcher interface.  The string consists of criteria that are 
        evaluated to determine which updates to return.  The Search method performs
        a synchronous search for updates by using the current configured search 
        options.   For more information about possible search criteria, please see
        the Search method of the IUpdateSearcher interface.
    */
	ObjectEntity* search_criteria = object->GetElementByName("search_criteria");

    // check datatype and operation
	if(search_criteria->GetDatatype() != OvalEnum::DATATYPE_STRING) {
		throw ProbeException("Error: invalid data type specified on search_criteria. Found: " + OvalEnum::DatatypeToString(search_criteria->GetDatatype()));
	}

	if(search_criteria->GetOperation() != OvalEnum::OPERATION_EQUALS ) {
		throw ProbeException("Error: invalid operation specified on search_criteria. Found: " + OvalEnum::OperationToString(search_criteria->GetOperation()));
	}

    ItemVector* collectedItems = new ItemVector();

	// get all the search_criteria
    ItemEntityVector* searchCriteriaItems = this->GetSearchCriteria(search_criteria);
	ItemEntityVector::iterator searchCriteriaIt;
	for(searchCriteriaIt=searchCriteriaItems->begin(); searchCriteriaIt!=searchCriteriaItems->end(); searchCriteriaIt++) {
		
		// run the queries and push the results on to the 
        // collectedItems return vector.
		Item* item = this->DoWUASearch((*searchCriteriaIt));
		if(item != NULL) {
			collectedItems->push_back(item);
		}
	}
	delete searchCriteriaItems;

	return collectedItems;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

Item* WUAUpdateSearcherProbe::CreateItem() {

	Item* item = new Item(0, 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows", 
						"win-sc", 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows windows-system-characteristics-schema.xsd", 
						OvalEnum::STATUS_ERROR, 
						"wuaupdatesearcher_item");

	return item;
}

ItemEntityVector* WUAUpdateSearcherProbe::GetSearchCriteria(ObjectEntity* search_criteria) {

    ItemEntityVector* searchCriteriaItems = new ItemEntityVector();

	// does this search_criteria use variables?
	if(search_criteria->GetVarRef() == NULL) {
		
		ItemEntity* tmp = this->CreateItemEntity(search_criteria);
		tmp->SetValue(search_criteria->GetValue());
		searchCriteriaItems->push_back(tmp);

	} else {

		// retrieve all the variable values that match the supplied var_ref.
		VariableValueVector* vars = search_criteria->GetVariableValues();

		// loop through all values
		VariableValueVector::iterator iterator;
		for(iterator = vars->begin(); iterator != vars->end(); iterator++) {

			ItemEntity* tmp = this->CreateItemEntity(search_criteria);
			tmp->SetValue((*iterator)->GetValue());
			searchCriteriaItems->push_back(tmp);
		}
	}

	return searchCriteriaItems;
}

void WUAUpdateSearcherProbe::ShutdownWUASearcher() {

    if(NULL != pISearchResult) {
        pISearchResult->Release();
        pISearchResult = NULL;
    }
    
    if(NULL != pIUpdateCollection) {
        pIUpdateCollection->Release();
        pIUpdateCollection = NULL;
    }

    if(NULL != pIUpdateSession) {
        pIUpdateSession->Release();
        pIUpdateSession = NULL;
    }

    if(NULL != pIUpdateServiceManager) {
        pIUpdateServiceManager->Release();
        pIUpdateServiceManager = NULL;
    }

    if(NULL != pIUSearcher) {
        pIUSearcher->Release();
        pIUSearcher = NULL;
    }
}

Item* WUAUpdateSearcherProbe::DoWUASearch(ItemEntity* search_criteria) {

    Item* item = NULL;

    this->InitWUASearcher();

	try {

        HRESULT hres;		
		
        // do the search call.
        BSTR searchString = _bstr_t(search_criteria->GetValue().c_str());
		hres = pIUSearcher->Search(searchString, &pISearchResult);
		if(FAILED(hres)) {
			string errorMessage = _com_error(hres).ErrorMessage();
			throw ProbeException("(WUAUpdateSearcherProbe) An error occured when attempting to Search for updates. " + errorMessage, ERROR_FATAL);
		}


        //
        // passed connection initialization and search call so create the item
        //
        item = this->CreateItem();
        item->AppendElement(new ItemEntity("search_criteria", search_criteria->GetValue(), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));


        // examine the result code on the pISearchResult
        OperationResultCode resultCode;
        pISearchResult->get_ResultCode(&resultCode);
        if(resultCode == orcSucceededWithErrors) {

            string errorMessage = "The search operation is complete, but one or more errors occurred during the operation. The results might be incomplete.";
            item->AppendMessage(new OvalMessage(errorMessage, OvalEnum::LEVEL_WARNING));

        } else if(resultCode != orcSucceeded) {
            string errorMessage;

            if(resultCode == orcFailed) {
                errorMessage = "The search operation failed to complete.";
            } else {
                errorMessage = "An error occured during the search operation.";
            }
            
			throw ProbeException("(WUAUpdateSearcherProbe) Update search error.  " + errorMessage, ERROR_FATAL);
        } 

        // get the updates that were found.
		hres = pISearchResult->get_Updates(&pIUpdateCollection);
		if (FAILED(hres)) {
			string errorMessage = _com_error(hres).ErrorMessage();
			throw ProbeException("(WUAUpdateSearcherProbe) Failed to .  " + errorMessage, ERROR_FATAL);
		}

        // Check to see if any updates were found	
		long resultCount = 0;
		hres = pIUpdateCollection->get_Count(&resultCount);
		if (FAILED(hres)) {
			string errorMessage = _com_error(hres).ErrorMessage();
			throw ProbeException("(WUAUpdateSearcherProbe) Failed to get a count of search results. " + errorMessage, ERROR_FATAL);
		} 

        if(resultCount == 0) {

            item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);

        } else {

		    // get the enum to the set of updates	
		    LPUNKNOWN pIUnknown = NULL;
		    hres = pIUpdateCollection->get__NewEnum(&pIUnknown);
		    if (FAILED(hres)) {
			    string errorMessage = _com_error(hres).ErrorMessage();
			    throw ProbeException("(WUAUpdateSearcherProbe) Failed to get an enumerator to the search results. " + errorMessage, ERROR_FATAL);
		    }
    		
		    // cast the enumerator of the search results
		    IEnumVARIANT *pIEnumVARIANT;
		    hres = pIUnknown->QueryInterface(IID_IEnumVARIANT,(void**)&pIEnumVARIANT);
		    if (FAILED(hres)) {
			    string errorMessage = _com_error(hres).ErrorMessage();
			    throw ProbeException("(WUAUpdateSearcherProbe) Failed to cast the enumerator of the search results. " + errorMessage, ERROR_FATAL);
		    }

		    // examine all the searh results		    
		    ULONG celtFetched = 0;
		    HRESULT nextHRES = S_OK;
            VARIANT variant;
		    while(S_OK == nextHRES) {

			    nextHRES = pIEnumVARIANT->Next(1, &variant, &celtFetched);

			    if((0==celtFetched) || (S_FALSE==nextHRES)) {

                    // No more results to process so break
				    break;

			    } else {

				    // We have a result. Create an ItemEntity for it and add it to the item.                    
                    IDispatch *pDisp = NULL;
                    IUpdate* pUpdate;		

				    pDisp = V_DISPATCH(&variant);
				    pDisp->QueryInterface(IID_IUpdate, (void**)&pUpdate); 
				    pDisp->Release();

				    IUpdateIdentity * pIUpdateIdentity;
				    pUpdate->get_Identity(&pIUpdateIdentity);
				    BSTR updateID;
				    pIUpdateIdentity->get_UpdateID(&updateID);
				    std::string buffer = _bstr_t(updateID);
    					
                    item->SetStatus(OvalEnum::STATUS_EXISTS);
				    item->AppendElement(new ItemEntity("update_id", buffer, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));				
			    }		
		    }
        }

	} catch (ProbeException ex) {

        if(item != NULL)
            delete item;

        this->ShutdownWUASearcher();

		// re-throw the error so it can be caught higher up.
		throw ex;

    } catch (...) {
        
        if(item != NULL)
            delete item;

		this->ShutdownWUASearcher();

		// re-throw the error so it can be caught higher up.
		throw ProbeException("(WUAUpdateSearcherProbe) An unknown error occured while searching for updates.");
	}

	this->ShutdownWUASearcher();

	return item;
}

void WUAUpdateSearcherProbe::InitWUASearcher() {

	HRESULT hres;

	try {

		// create the Update Session			
		hres = CoCreateInstance(CLSID_UpdateSession, NULL, CLSCTX_INPROC_SERVER, IID_IUpdateSession, (LPVOID *)&pIUpdateSession);
		if (FAILED(hres)) {
			string errorMessage = _com_error(hres).ErrorMessage();
			throw ProbeException("(WUAUpdateSearcherProbe) Failed to create IUpdateSession object. " + errorMessage, ERROR_FATAL);
		}

		// create the update service manager IUpdateServiceManager		
		hres = CoCreateInstance(CLSID_UpdateServiceManager, NULL, CLSCTX_INPROC_SERVER, IID_IUpdateServiceManager, (LPVOID *)&pIUpdateServiceManager);
		if (FAILED(hres)) {
			string errorMessage = _com_error(hres).ErrorMessage();
			throw ProbeException("(WUAUpdateSearcherProbe) Failed to create IUpdateServiceManager object. " + errorMessage, ERROR_FATAL);
		}			
		
		// create an update searcher
		hres = pIUpdateSession->CreateUpdateSearcher(&pIUSearcher);
		if (FAILED(hres)) {
			string errorMessage = _com_error(hres).ErrorMessage();
			throw ProbeException("(WUAUpdateSearcherProbe) Failed to create the IUpdateSearcher object. " + errorMessage, ERROR_FATAL);
		}
		
		// include potentially superceeded updates. We want to make sure we are searching ALL updates
		hres = pIUSearcher->put_IncludePotentiallySupersededUpdates(VARIANT_TRUE);
		if (FAILED(hres)) {
			string errorMessage = _com_error(hres).ErrorMessage();
			throw ProbeException("(WUAUpdateSearcherProbe) Failed to set search scope to include potentially superseded updates. " + errorMessage, ERROR_FATAL);
		}		

		// use the systems default server for searching for updates
		hres = pIUSearcher->put_ServerSelection(ssDefault);
		if (FAILED(hres)) {
			string errorMessage = _com_error(hres).ErrorMessage();
			throw ProbeException("(WUAUpdateSearcherProbe) Failed to set the server to search for updates. " + errorMessage, ERROR_FATAL);
		}	

	} catch (ProbeException ex) {

		this->ShutdownWUASearcher();

		// re-throw the error so it can be caught higher up.
		throw ex;

	} catch (...) {

		this->ShutdownWUASearcher();

		// re-throw the error so it can be caught higher up.
		throw ProbeException("An unknown error occured while executing a windows update search.");
	}
}
