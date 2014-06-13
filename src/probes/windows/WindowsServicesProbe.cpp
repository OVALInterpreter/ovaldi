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

#include <cassert>
#include <memory>

#include <EntityComparator.h>
#include <WindowsCommon.h>
#include <VectorPtrGuard.h>
#include <ArrayGuard.h>

#include "WindowsServicesProbe.h"

using namespace std;

// convenience macros since there's a lot of bit and enum checking
// in this code...
#define BIT2STRING(bit_)	if (bitmask & (bit_)) bitStrings.push_back(#bit_);
// this one is specifically designed for use inside switch
// statements.
#define ENUM2STRING(enum_)	case (enum_): *enumString = #enum_; break;

//****************************************************************************************//
//                              WindowsServicesProbe Class                                //
//****************************************************************************************//
WindowsServicesProbe* WindowsServicesProbe::instance = NULL;

WindowsServicesProbe::WindowsServicesProbe() : services(NULL), serviceMgr(NULL) {

    serviceMgr = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
    if ( serviceMgr == NULL ) {
        throw ProbeException("Couldn't open service control manager: " + 
			WindowsCommon::GetErrorMessage(GetLastError()));
    }

	services = GetAllServices();
}

WindowsServicesProbe::~WindowsServicesProbe() {
    instance = NULL;
	if (services)
		delete services;
	if (serviceMgr)
		CloseServiceHandle(serviceMgr);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

AbsProbe* WindowsServicesProbe::Instance() {
    // Use lazy initialization for instance of the WindowsServicesProbe
    if ( instance == NULL ) {
        WindowsServicesProbe::instance = new WindowsServicesProbe();
    }

    return WindowsServicesProbe::instance;
}

ItemVector* WindowsServicesProbe::CollectItems ( Object* object ) {

	ObjectEntity* serviceNameEntity = object->GetElementByName("service_name");
	
	if ( serviceNameEntity->GetDatatype() != OvalEnum::DATATYPE_STRING ) {
        throw ProbeException ( "Error: invalid data type specified on service_name. Found: " + OvalEnum::DatatypeToString ( serviceNameEntity->GetDatatype() ) );
    }

    if ( serviceNameEntity->GetOperation() != OvalEnum::OPERATION_EQUALS
            && serviceNameEntity->GetOperation() != OvalEnum::OPERATION_PATTERN_MATCH
            && serviceNameEntity->GetOperation() != OvalEnum::OPERATION_NOT_EQUAL 
			&& serviceNameEntity->GetOperation() != OvalEnum::OPERATION_CASE_INSENSITIVE_EQUALS
			&& serviceNameEntity->GetOperation() != OvalEnum::OPERATION_CASE_INSENSITIVE_NOT_EQUAL) {
        throw ProbeException ( "Error: invalid operation specified on service_name. Found: " + OvalEnum::OperationToString ( serviceNameEntity->GetOperation() ) );
    }

	VectorPtrGuard<Item> collectedItems(new ItemVector());

	auto_ptr<StringSet> allServices( WindowsServicesProbe::GetServices ( serviceNameEntity ));

	for ( StringSet::iterator it = allServices->begin(); it != allServices->end(); it++ ) {
		Item * theItem = this->GetService(*it);
		if(theItem != NULL){
			collectedItems->push_back(theItem);
		}
	}
	allServices->clear();

	return collectedItems.release();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

Item* WindowsServicesProbe::CreateItem() {
    Item* item = new Item ( 0 ,
                            "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows" ,
                            "win-sc" ,
                            "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows windows-system-characteristics-schema.xsd" ,
                            OvalEnum::STATUS_ERROR ,
                            "service_item" );
    return item;
}

StringSet* WindowsServicesProbe::GetServices ( ObjectEntity* serviceNameEntity ) {
    auto_ptr<StringSet> theServices(new StringSet());
	
    // Does this ObjectEntity use variables?
    if ( serviceNameEntity->GetVarRef() == NULL ) {
        // Proceed based on operation
        if ( serviceNameEntity->GetOperation() == OvalEnum::OPERATION_EQUALS ) {
          
            // If the service exists, add it to the list
            if ( this->ServiceExists ( serviceNameEntity->GetValue(), false ) ) {
                theServices->insert ( serviceNameEntity->GetValue() );
            }

        } else if( serviceNameEntity->GetOperation() == OvalEnum::OPERATION_CASE_INSENSITIVE_EQUALS){

			   // If the service exists, add it to the list
            if ( this->ServiceExists ( serviceNameEntity->GetValue(), true ) ) {
                theServices->insert ( serviceNameEntity->GetValue() );
            }
		}else if ( serviceNameEntity->GetOperation() == OvalEnum::OPERATION_NOT_EQUAL  || 
				   serviceNameEntity->GetOperation() == OvalEnum::OPERATION_CASE_INSENSITIVE_NOT_EQUAL || 
				   serviceNameEntity->GetOperation() == OvalEnum::OPERATION_PATTERN_MATCH) {
            theServices = this->GetMatchingServices ( serviceNameEntity);
        }

    } else {
        // Get all services
		auto_ptr<StringSet> allServices(new StringSet());
       
        if ( serviceNameEntity->GetOperation() == OvalEnum::OPERATION_EQUALS ) {
            // In the case of equals simply loop through all the
            // variable values and add them to the set of all services
            // if they exist on the system
			VariableValueVector vals = serviceNameEntity->GetVarRef()->GetValues();
            VariableValueVector::iterator iterator;
            for ( iterator = vals.begin() ; iterator != vals.end() ; iterator++ ) {
                if ( this->ServiceExists ( iterator->GetValue(), false ) ) {
                    allServices->insert ( iterator->GetValue() );
                }
            }

        } else if ( serviceNameEntity->GetOperation() == OvalEnum::OPERATION_CASE_INSENSITIVE_EQUALS ) {
			VariableValueVector vals = serviceNameEntity->GetVarRef()->GetValues();
			VariableValueVector::iterator iterator;

            for ( iterator = vals.begin() ; iterator != vals.end() ; iterator++ ) {
                if ( this->ServiceExists ( iterator->GetValue(), true ) ) {
                    allServices->insert ( iterator->GetValue() );
                }
            }
		} else {
            allServices = this->GetMatchingServices ( serviceNameEntity );
        }

        ItemEntity tmp("service_name");
        for (StringSet::iterator it = allServices->begin() ; it != allServices->end() ; it++) {
            tmp.SetValue(*it);

            if ( serviceNameEntity->Analyze (&tmp ) == OvalEnum::RESULT_TRUE ) {
                theServices->insert(*it);
            }
        }
    }
	
    return theServices.release();
}

auto_ptr<StringSet> WindowsServicesProbe::GetMatchingServices (ObjectEntity* serviceNameEntity ) {
   auto_ptr<StringSet> matchingServices(new StringSet());

	if(serviceNameEntity->GetOperation() == OvalEnum::OPERATION_CASE_INSENSITIVE_NOT_EQUAL){
		for ( StringSet::iterator iterator = services->begin() ; iterator != services->end() ; iterator++ ){
			if(EntityComparator::CompareString(OvalEnum::OPERATION_CASE_INSENSITIVE_NOT_EQUAL, serviceNameEntity->GetValue(), *iterator) == OvalEnum::RESULT_TRUE){
				matchingServices->insert ( *iterator );
			}
		}
	}else if(serviceNameEntity->GetOperation() == OvalEnum::OPERATION_NOT_EQUAL){
		for ( StringSet::iterator iterator = services->begin() ; iterator != services->end() ; iterator++ ){
			if(EntityComparator::CompareString(OvalEnum::OPERATION_NOT_EQUAL, serviceNameEntity->GetValue(), *iterator) == OvalEnum::RESULT_TRUE){
				matchingServices->insert ( *iterator );
			}
		}
	}
	else if(serviceNameEntity->GetOperation() == OvalEnum::OPERATION_PATTERN_MATCH){
		for ( StringSet::iterator iterator = services->begin() ; iterator != services->end() ; iterator++ ) {
			if(EntityComparator::CompareString(OvalEnum::OPERATION_PATTERN_MATCH, serviceNameEntity->GetValue(), *iterator) == OvalEnum::RESULT_TRUE){
				matchingServices->insert ( *iterator );
			}
		} 
	}

    return matchingServices;
}

StringSet* WindowsServicesProbe::GetAllServices() {
    auto_ptr<StringSet> allServices(new StringSet());
	DWORD dwBytesNeeded = 0;
    DWORD dwServicesReturned = 0;
    DWORD dwResumedHandle = 0;
	DWORD currBufferSize = 0;
	DWORD errorCode;
	ArrayGuard<char> services;
	
    // Query services
    BOOL retVal = EnumServicesStatus(serviceMgr,
		SERVICE_WIN32 | SERVICE_DRIVER,
		SERVICE_STATE_ALL,
		NULL,
		sizeof(ENUM_SERVICE_STATUS),
		&dwBytesNeeded,
		&dwServicesReturned,
		&dwResumedHandle);
	errorCode = GetLastError();

	// Need big buffer
	while (!retVal && (ERROR_MORE_DATA == errorCode)) {

		// Enlarge the buffer if necessary.  No need to shrink it.
		if (dwBytesNeeded > currBufferSize) {
			services.reset(new char[dwBytesNeeded]);
			currBufferSize = dwBytesNeeded;
		}

		retVal = EnumServicesStatus(serviceMgr,
			SERVICE_WIN32 | SERVICE_DRIVER,
			SERVICE_STATE_ALL,
			(LPENUM_SERVICE_STATUS)services.get(),
			currBufferSize,
			&dwBytesNeeded,
			&dwServicesReturned,
			&dwResumedHandle);
		errorCode = GetLastError();

		if(retVal || errorCode == ERROR_MORE_DATA) {
			// now traverse each service to get information
			LPENUM_SERVICE_STATUS svcsAsArray = (LPENUM_SERVICE_STATUS)services.get();
			for (unsigned iIndex = 0; iIndex < dwServicesReturned; iIndex++) {
				allServices->insert(svcsAsArray[iIndex].lpServiceName);
			}
		}
	}

	if (!retVal && errorCode)
		throw ProbeException("Couldn't enum services: EnumServicesStatus(): " + WindowsCommon::GetErrorMessage(errorCode));

    return allServices.release();
}

bool WindowsServicesProbe::ServiceExists ( string serviceNameStr, bool caseInsensitive ) {
	if(caseInsensitive){
		
		for ( StringSet::iterator it = WindowsServicesProbe::services->begin(); it != WindowsServicesProbe::services->end(); it++ ) {
			if(Common::EqualsIgnoreCase(serviceNameStr, *it)){
				return true;
			}
		}
			
	}else{

		if ( WindowsServicesProbe::services->find ( serviceNameStr ) != WindowsServicesProbe::services->end() ) {
		    return true;
		}
	}

    return false;
}

Item* WindowsServicesProbe::GetService ( string serviceName ) {
    DWORD dwBytesNeeded = 0;
	DWORD dwError = 0; 
	ArrayGuard<BYTE> byteBuffer;
	BOOL ret;

	auto_ptr<Item> item(this->CreateItem());
	item->SetStatus(OvalEnum::STATUS_EXISTS);

	VectorPtrGuard<ItemEntity> itemEntities(new vector<ItemEntity*>());

	// The following code throws the collection of entities
	// that will go into the item into the itemEntities vector,
	// rather than directly into the item.  This makes them
	// easier to use in the following code.
	//
	// It assigns a separate variable to also point to each 
	// element of the vector, which makes each entity easy to
	// access in a readable way (rather than just hard-coding
	// vector indices).
	//
	// All entities default to status=error, and as the
	// following syscalls succeed, will be switched to status=exists.
	//
	// The reason for this design is because the success/failure of
	// subsequent syscalls will affect non-contiguous sets of
	// entities.  E.g. GetServiceFoo() might yield info for entities
	// 1, 2, 5, and 7, so that particular subset gets success/failure
	// depending on whether the call succeeded.  So I need to be able
	// to pick and choose in a readable maintainable way particular
	// entities.  Defaulting to status=error means that syscall errors
	// require no work (other than maybe adding a message): the entities
	// are already set up correctly.  I only have to do work on 
	// successes (and any other non-error condition).

// factor out var naming to make it easier to change
#define IE_VAR_NAME(ieName_) ieName_ ## Ie
// this macro lets you specify the oval type
#define INIT_ITEM_ENTITY_TYPE(ieName_, type_) \
	ItemEntity *IE_VAR_NAME(ieName_); \
	itemEntities->push_back(IE_VAR_NAME(ieName_) = new ItemEntity(#ieName_, "", OvalEnum::DATATYPE_ ## type_, OvalEnum::STATUS_ERROR));
// a simpler macro which uses datatype=string, which is
// what most entities are.
#define INIT_ITEM_ENTITY(ieName_) INIT_ITEM_ENTITY_TYPE(ieName_, STRING)

// Need an iterator to the middle of the entity vector, when the
// time comes to insert entities into the middle.  This is necessary
// for those entities which must be repeated for multiple values.
// So this code must scan through the vector until it finds an 
// address which matches the corresponding entity variable.  The 
// entity variables for those entities will wind up pointing to one
// of possibly several entities with the same name, to be added later.
#define IE_ITER_NAME(ieName_) ieName_ ## Iter
#define ITEM_ENTITY_ITERATOR_FOR(ieName_) \
	vector<ItemEntity*>::iterator IE_ITER_NAME(ieName_) = \
		itemEntities.get()->begin(); \
	while (IE_ITER_NAME(ieName_) != itemEntities.get()->end() && \
			*IE_ITER_NAME(ieName_) != IE_VAR_NAME(ieName_)) \
		++IE_ITER_NAME(ieName_); \
	assert(IE_ITER_NAME(ieName_) != itemEntities.get()->end());

	INIT_ITEM_ENTITY(service_name)
	INIT_ITEM_ENTITY(display_name)
	INIT_ITEM_ENTITY(description)
	INIT_ITEM_ENTITY(service_type) // multiple allowed
	INIT_ITEM_ENTITY(start_type)
	INIT_ITEM_ENTITY(current_state)
	INIT_ITEM_ENTITY(controls_accepted) // multiple allowed
	INIT_ITEM_ENTITY(start_name)
	INIT_ITEM_ENTITY(path)
	INIT_ITEM_ENTITY_TYPE(pid, INTEGER)
	INIT_ITEM_ENTITY_TYPE(service_flag, BOOLEAN)
	INIT_ITEM_ENTITY(dependencies) // multiple allowed

	IE_VAR_NAME(service_name)->SetValue(serviceName);
	IE_VAR_NAME(service_name)->SetStatus(OvalEnum::STATUS_EXISTS);

    // open service
    SC_HANDLE schService = OpenService(serviceMgr, serviceName.c_str(), 
		SERVICE_QUERY_CONFIG|SERVICE_QUERY_STATUS); 
    AutoCloser<SC_HANDLE, BOOL(WINAPI&)(SC_HANDLE)> schServiceCloser(schService, CloseServiceHandle, "ServiceCloser");

    if (schService == NULL) {
		string logMsg = "Error accessing service: OpenService(): " +
			WindowsCommon::GetErrorMessage(GetLastError());
		Log::Info(logMsg);

		item->SetStatus(OvalEnum::STATUS_ERROR);
		item->AppendMessage(new OvalMessage(logMsg, OvalEnum::LEVEL_ERROR));
		item->SetElements(itemEntities.get());
		itemEntities->clear();
		return item.release();
    }

	dwBytesNeeded = 0;
	byteBuffer.reset();
	do {
		if (dwBytesNeeded)
			byteBuffer.reset(new BYTE[dwBytesNeeded]);
		ret = QueryServiceConfigA(schService, 
			(LPQUERY_SERVICE_CONFIGA)byteBuffer.get(), dwBytesNeeded,
			&dwBytesNeeded);
		dwError = GetLastError();
	} while (!ret && ERROR_INSUFFICIENT_BUFFER == dwError);

	if (!ret) {
		string logMsg = "Error querying service: QueryServiceConfig(): " +
			WindowsCommon::GetErrorMessage(dwError);
		Log::Info(logMsg);

		item->AppendMessage(new OvalMessage(logMsg, OvalEnum::LEVEL_ERROR));
	} else {

		LPQUERY_SERVICE_CONFIGA lpsc = (LPQUERY_SERVICE_CONFIGA)byteBuffer.get();
		if(lpsc->lpDisplayName) {
			IE_VAR_NAME(display_name)->SetValue(lpsc->lpDisplayName);
			IE_VAR_NAME(display_name)->SetStatus(OvalEnum::STATUS_EXISTS);
		} else {
			// not sure this is possible...
			IE_VAR_NAME(display_name)->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
		}

		vector<string> serviceType = WindowsServicesProbe::ServiceTypeToString(lpsc->dwServiceType);
		if (serviceType.empty())
			IE_VAR_NAME(service_type)->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
		else {
			vector<string>::iterator valIter = serviceType.begin();
			// the first val goes into the existing item entity; any 
			// remaining vals go into new item entities we insert
			// next to the existing one.
			IE_VAR_NAME(service_type)->SetValue(*valIter);
			IE_VAR_NAME(service_type)->SetStatus(OvalEnum::STATUS_EXISTS);
			++valIter;
			ITEM_ENTITY_ITERATOR_FOR(service_type)
			for(;valIter != serviceType.end(); ++valIter)
				IE_ITER_NAME(service_type) = itemEntities->insert(
					IE_ITER_NAME(service_type),
					new ItemEntity("service_type", *valIter));
		}

		string startType;
		if (WindowsServicesProbe::StartTypeToString(lpsc->dwStartType, &startType)) {
			IE_VAR_NAME(start_type)->SetValue(startType);
			IE_VAR_NAME(start_type)->SetStatus(OvalEnum::STATUS_EXISTS);
		} else {
			item->AppendMessage(new OvalMessage("Unrecognized start type: " + 
				Common::ToString(lpsc->dwStartType)));
		}

		if (lpsc->lpBinaryPathName) {
			IE_VAR_NAME(path)->SetValue(lpsc->lpBinaryPathName);
			IE_VAR_NAME(path)->SetStatus(OvalEnum::STATUS_EXISTS);
		} else {
			// not sure this is possible...
			IE_VAR_NAME(path)->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
		}

		if (lpsc->lpServiceStartName) {
			IE_VAR_NAME(start_name)->SetValue(lpsc->lpServiceStartName);
			IE_VAR_NAME(start_name)->SetStatus(OvalEnum::STATUS_EXISTS);
		} else {
			// not sure this is possible...
			IE_VAR_NAME(start_name)->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
		}

		vector<string> depArrayTokens;
		char *subVal = lpsc->lpDependencies;
		while (subVal && *subVal) {
			depArrayTokens.push_back(subVal);
			while (*subVal) ++subVal;
			++subVal;
		}

		if (depArrayTokens.empty()) {
			IE_VAR_NAME(dependencies)->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
		} else {
			vector<string>::iterator valIter = depArrayTokens.begin();
			IE_VAR_NAME(dependencies)->SetValue(*valIter);
			IE_VAR_NAME(dependencies)->SetStatus(OvalEnum::STATUS_EXISTS);
			++valIter;
			ITEM_ENTITY_ITERATOR_FOR(dependencies)
			for (; valIter != depArrayTokens.end(); ++valIter)
				IE_ITER_NAME(dependencies) = itemEntities->insert(
					IE_ITER_NAME(dependencies),
					new ItemEntity("dependencies", *valIter));
		}
	} // if (!ret){...} else {

	byteBuffer.reset();
	dwBytesNeeded = 0;
	do {
		if (dwBytesNeeded)
			byteBuffer.reset(new BYTE[dwBytesNeeded]);
		ret = QueryServiceConfig2(schService, SERVICE_CONFIG_DESCRIPTION, 
			byteBuffer.get(), dwBytesNeeded, &dwBytesNeeded);
		dwError = GetLastError();
	} while (!ret && ERROR_INSUFFICIENT_BUFFER == dwError);

	if (!ret) {
		// I saw ERROR_FILE_NOT_FOUND happen here... docs say these error
		// codes can also come from registry functions which 
		// QueryServiceConfig2() calls.  In fact, the file specified in 
		// lpsc->lpBinaryPathName didn't exist, and it must have tried to get
		// the description from the file.  You could treat it as 
		// status=does not exist, but maybe we should still flag an error in
		// that case?
		string logMsg = "Error querying service: QueryServiceConfig2(): " +
			WindowsCommon::GetErrorMessage(dwError);
		Log::Info(logMsg);

		item->AppendMessage(new OvalMessage(logMsg, OvalEnum::LEVEL_ERROR));
	} else {
		LPSERVICE_DESCRIPTION lpsd = (LPSERVICE_DESCRIPTION)byteBuffer.get();
		if (lpsd->lpDescription) {
			IE_VAR_NAME(description)->SetValue(lpsd->lpDescription);
			IE_VAR_NAME(description)->SetStatus(OvalEnum::STATUS_EXISTS);
		} else {
			// this one is possible
			IE_VAR_NAME(description)->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
		}
	}

	byteBuffer.reset();
	dwBytesNeeded = 0;
	do {
		if (dwBytesNeeded)
			byteBuffer.reset(new BYTE[dwBytesNeeded]);
		ret = QueryServiceStatusEx(schService, SC_STATUS_PROCESS_INFO, 
			byteBuffer.get(), dwBytesNeeded, &dwBytesNeeded);
		dwError = GetLastError();
	} while (!ret && ERROR_INSUFFICIENT_BUFFER == dwError);

	if (!ret) {
		string logMsg = "Error querying service: QueryServiceStatusEx(): " +
			WindowsCommon::GetErrorMessage(dwError);
		Log::Info(logMsg);

		item->AppendMessage(new OvalMessage(logMsg, OvalEnum::LEVEL_ERROR));
	} else {
		LPSERVICE_STATUS_PROCESS ssStatus = (LPSERVICE_STATUS_PROCESS)byteBuffer.get();

		string currState;
		if (CurrentStateToString(ssStatus->dwCurrentState, &currState)) {
			IE_VAR_NAME(current_state)->SetValue(currState);
			IE_VAR_NAME(current_state)->SetStatus(OvalEnum::STATUS_EXISTS);
		} else {
			item->AppendMessage(new OvalMessage("Unrecognized service state: " +
				Common::ToString(ssStatus->dwCurrentState), 
				OvalEnum::LEVEL_ERROR));
		}

		vector<string> controlType = WindowsServicesProbe::ControlToString(ssStatus->dwControlsAccepted);
		if (controlType.empty()) {
			IE_VAR_NAME(controls_accepted)->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
		} else {
			vector<string>::iterator valIter = controlType.begin();
			IE_VAR_NAME(controls_accepted)->SetValue(*valIter);
			IE_VAR_NAME(controls_accepted)->SetStatus(OvalEnum::STATUS_EXISTS);
			++valIter;
			ITEM_ENTITY_ITERATOR_FOR(controls_accepted)
			for(; valIter != controlType.end(); ++valIter)
				IE_ITER_NAME(controls_accepted) = itemEntities->insert(
					IE_ITER_NAME(controls_accepted),
					new ItemEntity("controls_accepted", *valIter));
		}

		IE_VAR_NAME(pid)->SetValue(Common::ToString(ssStatus->dwProcessId));
		IE_VAR_NAME(pid)->SetStatus(OvalEnum::STATUS_EXISTS);

		bool serviceFlag;
		if (ServiceFlagToBool(ssStatus->dwServiceFlags, &serviceFlag)) {
			IE_VAR_NAME(service_flag)->SetValue(serviceFlag ? "true":"false");
			IE_VAR_NAME(service_flag)->SetStatus(OvalEnum::STATUS_EXISTS);
		} else {
			item->AppendMessage(new OvalMessage("Unrecognized service flag: " +
				Common::ToString(ssStatus->dwServiceFlags),
				OvalEnum::LEVEL_ERROR));
		}
	}

	item->SetElements(itemEntities.get());
	// SetElements() call above causes the item to take ownership
	// of the vector contents, but not the vector itself...
	// so we'll clear out the contents from our guarded vector
	// so the guard cleans up the vector but not the contents.
	itemEntities->clear();

	return item.release();

#undef ITEM_ENTITY_ITERATOR_FOR
#undef IE_ITER_NAME
#undef INIT_ITEM_ENTITY
#undef INIT_ITEM_ENTITY_TYPE
#undef IE_VAR_NAME
}

vector<string> WindowsServicesProbe::ServiceTypeToString(DWORD bitmask) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Convert the ServiceType value to a vector of strings
	//
	// ----------------------------------------------------------------------- 

	vector<string> bitStrings;

	BIT2STRING(SERVICE_KERNEL_DRIVER)
	BIT2STRING(SERVICE_FILE_SYSTEM_DRIVER)
	BIT2STRING(SERVICE_WIN32_OWN_PROCESS)
	BIT2STRING(SERVICE_WIN32_SHARE_PROCESS)
	BIT2STRING(SERVICE_INTERACTIVE_PROCESS)

	return bitStrings;
}

bool WindowsServicesProbe::StartTypeToString(DWORD type, string *enumString){
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Convert the StartType value to a string
	//
	// -----------------------------------------------------------------------

	bool ok = true;

	switch(type) {
		ENUM2STRING(SERVICE_BOOT_START)
		ENUM2STRING(SERVICE_SYSTEM_START)
		ENUM2STRING(SERVICE_AUTO_START)
		ENUM2STRING(SERVICE_DEMAND_START)
		ENUM2STRING(SERVICE_DISABLED)

		default:
			ok = false;
			break;
	}

	return ok;
}

bool WindowsServicesProbe::CurrentStateToString(DWORD state, string *enumString){
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Convert the CurrentStateType value to a string
	//
	// -----------------------------------------------------------------------

	bool ok = true;

	switch(state) {

		ENUM2STRING(SERVICE_STOPPED)
		ENUM2STRING(SERVICE_START_PENDING)
		ENUM2STRING(SERVICE_STOP_PENDING)
		ENUM2STRING(SERVICE_RUNNING)
		ENUM2STRING(SERVICE_CONTINUE_PENDING)
		ENUM2STRING(SERVICE_PAUSE_PENDING)
		ENUM2STRING(SERVICE_PAUSED)

		default:
			ok = false;
			break;
	}

	return ok;
}

vector<string> WindowsServicesProbe::ControlToString(DWORD bitmask){
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Convert the ControlsAcceptedType value to a vector of strings
	//
	// -----------------------------------------------------------------------
	vector<string> bitStrings;

	BIT2STRING(SERVICE_ACCEPT_STOP)
	BIT2STRING(SERVICE_ACCEPT_PAUSE_CONTINUE)
	BIT2STRING(SERVICE_ACCEPT_SHUTDOWN)
	BIT2STRING(SERVICE_ACCEPT_PARAMCHANGE)
	BIT2STRING(SERVICE_ACCEPT_NETBINDCHANGE)
	BIT2STRING(SERVICE_ACCEPT_HARDWAREPROFILECHANGE)
	BIT2STRING(SERVICE_ACCEPT_POWEREVENT)
	BIT2STRING(SERVICE_ACCEPT_SESSIONCHANGE)
	BIT2STRING(SERVICE_ACCEPT_PRESHUTDOWN)
	BIT2STRING(SERVICE_ACCEPT_TIMECHANGE)
	BIT2STRING(SERVICE_ACCEPT_TRIGGEREVENT)

	return bitStrings;
}

bool WindowsServicesProbe::ServiceFlagToBool(DWORD type, bool *serviceFlag){
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Convert the Service Flag value to a bool
	//
	// -----------------------------------------------------------------------
	bool ok = true;

	switch(type) {
		case 0:
			*serviceFlag = false;
			break;
		case SERVICE_RUNS_IN_SYSTEM_PROCESS:
			*serviceFlag = true;
			break;
		
		default:
			ok = false;
			break;
	}

	return ok;
}

