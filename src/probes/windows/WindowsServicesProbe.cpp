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

#include "WindowsServicesProbe.h"
#include <PrivilegeGuard.h>

//****************************************************************************************//
//                              WindowsServicesProbe Class                                //
//****************************************************************************************//
WindowsServicesProbe* WindowsServicesProbe::instance = NULL;

WindowsServicesProbe::WindowsServicesProbe() {
	services = NULL;

    SC_HANDLE serviceHandle = NULL;
    serviceHandle = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
    if ( serviceHandle == NULL ) {
        throw ProbeException ( "Error: The function OpenSCManager() was unable to retrieve the handle for the service control manager database. Microsoft System Error " + Common::ToString ( GetLastError() ) + ") - " + WindowsCommon::GetErrorMessage ( GetLastError() ) );
    }

	this->serviceMgr.reset(
		new AutoCloser<SC_HANDLE, BOOL(WINAPI&)(SC_HANDLE)>(
			serviceHandle, CloseServiceHandle, "service control manager"));

	services = GetAllServices();
}

WindowsServicesProbe::~WindowsServicesProbe() {
    instance = NULL;
	if (services)
		delete services;
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
            && serviceNameEntity->GetOperation() != OvalEnum::OPERATION_NOT_EQUAL ) {
        throw ProbeException ( "Error: invalid operation specified on service_name. Found: " + OvalEnum::OperationToString ( serviceNameEntity->GetOperation() ) );
    }

    ItemVector* collectedItems = new ItemVector();

	if ( serviceNameEntity->GetOperation() == OvalEnum::OPERATION_EQUALS ){
		StringVector theServices;
		serviceNameEntity->GetEntityValues(theServices);
		for(StringVector::iterator it = theServices.begin(); it != theServices.end(); it++){
			Item * theItem = this->GetService(*it);
			if(theItem != NULL){
				collectedItems->push_back(theItem);
			}
		}
	}else if( serviceNameEntity->GetOperation() == OvalEnum::OPERATION_PATTERN_MATCH || OvalEnum::OPERATION_NOT_EQUAL){
			StringSet* allServices = WindowsServicesProbe::GetServices ( serviceNameEntity );
			for ( StringSet::iterator it = allServices->begin(); it != allServices->end(); it++ ) {
				Item * theItem = this->GetService(*it);
				if(theItem != NULL){
					collectedItems->push_back(theItem);
				}
			}
	}

    return collectedItems;
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
    StringSet* theServices = NULL;

    // Does this ObjectEntity use variables?
    if ( serviceNameEntity->GetVarRef() == NULL ) {
        // Proceed based on operation
        if ( serviceNameEntity->GetOperation() == OvalEnum::OPERATION_EQUALS ) {
            theServices = new StringSet();

            // If the service exists add it to the list
            if ( this->ServiceExists ( serviceNameEntity->GetValue() ) ) {
                theServices->insert ( serviceNameEntity->GetValue() );
            }

        } else if ( serviceNameEntity->GetOperation() == OvalEnum::OPERATION_NOT_EQUAL ) {
            theServices = this->GetMatchingServices ( serviceNameEntity->GetValue() , false);
        } else if ( serviceNameEntity->GetOperation() == OvalEnum::OPERATION_PATTERN_MATCH ) {
            theServices = this->GetMatchingServices ( serviceNameEntity->GetValue() , true );
        } 
		//else if( serviceNameEntity->GetOperation() == OPERATION_CASE_INSENSITIVE_EQUALS,){
		//	theServices = this->GetMatchingServices ( serviceNameEntity->GetValue() , true );
		//}

    } else {
        theServices = new StringSet();
        // Get all services
        StringSet* allServices; // = new StringSet();

        if ( serviceNameEntity->GetOperation() == OvalEnum::OPERATION_EQUALS ) {
            // In the case of equals simply loop through all the
            // variable values and add them to the set of all services
            // if they exist on the system
            VariableValueVector::iterator iterator;

            for ( iterator = serviceNameEntity->GetVarRef()->GetValues()->begin() ; iterator != serviceNameEntity->GetVarRef()->GetValues()->end() ; iterator++ ) {
                if ( this->ServiceExists ( ( *iterator )->GetValue() ) ) {
                    allServices->insert ( ( *iterator )->GetValue() );
                }
            }

        } else {
            allServices = this->GetMatchingServices ( ".*" , true );
        }

        // Loop through all services on the system
        // only keep services that match operation and value and var check
        ItemEntity* tmp = this->CreateItemEntity ( serviceNameEntity );
        StringSet::iterator it;

        for ( it = allServices->begin() ; it != allServices->end() ; it++ ) {
            tmp->SetValue ( ( *it ) );

            if ( serviceNameEntity->Analyze ( tmp ) == OvalEnum::RESULT_TRUE ) {
                theServices->insert ( ( *it ) );
            }
        }
    }

    return theServices;
}

StringSet* WindowsServicesProbe::GetMatchingServices ( string patternStr , bool isRegex ) {
    StringSet* matchingServices = new StringSet();

	if ( services == NULL ) {
		services = GetAllServices();
    }

    for ( StringSet::iterator iterator = services->begin() ; iterator != services->end() ; iterator++ ) {
        if ( this->IsMatch ( patternStr , *iterator , isRegex ) ) {
            matchingServices->insert ( *iterator );
        }
    }

    return matchingServices;
}


StringSet* WindowsServicesProbe::GetAllServices() {
    StringSet* allServices = new StringSet();
	ENUM_SERVICE_STATUS service;
	DWORD dwBytesNeeded = 0;
    DWORD dwServicesReturned = 0;
    DWORD dwResumedHandle = 0;
    DWORD dwServiceType = SERVICE_WIN32 | SERVICE_DRIVER;
	
    // Query services
    BOOL retVal = EnumServicesStatus(serviceMgr->get(), dwServiceType, SERVICE_STATE_ALL, &service, sizeof(ENUM_SERVICE_STATUS), &dwBytesNeeded, &dwServicesReturned, &dwResumedHandle);
 
	if (!retVal) {
		// Need big buffer
		if (ERROR_MORE_DATA == GetLastError()) {
			// Set the buffer
			DWORD dwBytes = sizeof(ENUM_SERVICE_STATUS) + dwBytesNeeded;
			ENUM_SERVICE_STATUS* pServices = NULL;
			pServices = new ENUM_SERVICE_STATUS [dwBytes];
			// Now query again for services
			if(! EnumServicesStatus(serviceMgr->get(), SERVICE_WIN32 | SERVICE_DRIVER, SERVICE_STATE_ALL, pServices, dwBytes, &dwBytesNeeded, &dwServicesReturned, &dwResumedHandle)){
				delete [] pServices;
				pServices = NULL; 

				delete allServices;
				allServices = NULL;

				throw ProbeException ( "ERROR: The function EnumServicesStatusEx() could not enumerate the services on the system. Microsoft System Error " + Common::ToString ( GetLastError() ) + ") - " + WindowsCommon::GetErrorMessage ( GetLastError() ) );
			}

			// now traverse each service to get information
			for (unsigned iIndex = 0; iIndex < dwServicesReturned; iIndex++) {
				std::string serviceName = (pServices + iIndex)->lpServiceName;
				 allServices->insert(serviceName);
			}
 
			delete [] pServices;
			pServices = NULL;

		}else{

			delete allServices;
			allServices = NULL;

			throw ProbeException ( "ERROR: The function EnumServicesStatus() could not enumerate the services on the system. Microsoft System Error " + Common::ToString ( GetLastError() ) + ") - " + WindowsCommon::GetErrorMessage ( GetLastError() ) );
		}
	}

    return allServices;
}

bool WindowsServicesProbe::ServiceExists ( string serviceNameStr ) {
    if ( WindowsServicesProbe::services->find ( serviceNameStr ) != WindowsServicesProbe::services->end() ) {
        return true;
    }

    return false;
}

Item* WindowsServicesProbe::GetService ( string serviceName ) {
	Item* item = NULL;
    SC_HANDLE schService;
	SC_HANDLE schService2;
	SERVICE_STATUS_PROCESS ssStatus;
    LPQUERY_SERVICE_CONFIG lpsc; 
    LPSERVICE_DESCRIPTION lpsd;
    DWORD dwBytesNeeded, cbBufSize, dwError; 
	std::string serviceType = "";
	std::string startType = "";

	#ifdef WIN32
	
	// Giving ourselves this privilege seems to gain us access
	// to a lot more processes.
	PrivilegeGuard pg(SE_DEBUG_NAME, false);

	#endif

    // open service
    schService = OpenService(serviceMgr->get(), serviceName.c_str(), SERVICE_QUERY_CONFIG); 
 
    if (schService == NULL){  
		 std::string logMsg = "ERROR: The function OpenService() could not access a service on the system.";
		 Log::Info(logMsg);
		 return NULL;
    }

    // Get the configuration information.
    if( !QueryServiceConfig(schService, NULL, 0, &dwBytesNeeded)){
        dwError = GetLastError();
        if( ERROR_INSUFFICIENT_BUFFER == dwError ){
            cbBufSize = dwBytesNeeded;
            lpsc = (LPQUERY_SERVICE_CONFIG) LocalAlloc(LMEM_FIXED, cbBufSize);

			if(lpsc == NULL){
				std::string logMsg = "ERROR: The function LocalAlloc() failed.";
				Log::Info(logMsg);
				CloseServiceHandle(schService); 
				return NULL;
			}

        }else{
			std::string logMsg = "ERROR: The function QueryServiceConfig() failed.";
			Log::Info(logMsg);
			return NULL;
		}
    }
  
    if( !QueryServiceConfig(schService, lpsc, cbBufSize, &dwBytesNeeded) ) {
		std::string logMsg = "ERROR: The function QueryServiceConfig() failed.";
		Log::Info(logMsg);
		return NULL;
	}

    if( !QueryServiceConfig2( schService, SERVICE_CONFIG_DESCRIPTION, NULL, 0, &dwBytesNeeded)){
        dwError = GetLastError();
        if( ERROR_INSUFFICIENT_BUFFER == dwError ){
            cbBufSize = dwBytesNeeded;
            lpsd = (LPSERVICE_DESCRIPTION) LocalAlloc(LMEM_FIXED, cbBufSize);

			if(lpsd == NULL){
				std::string logMsg = "ERROR: The function LocalAlloc() failed.";
				Log::Info(logMsg);
				CloseServiceHandle(schService); 
				return NULL;
			}

		}else{
			std::string logMsg = "ERROR: The function QueryServiceConfig2() failed.";
			Log::Info(logMsg);
			return NULL;
		}
    }
 
    if (! QueryServiceConfig2( schService, SERVICE_CONFIG_DESCRIPTION, (LPBYTE) lpsd, cbBufSize,  &dwBytesNeeded) ) {
		std::string logMsg = "ERROR: The function QueryServiceConfig2() failed.";
		Log::Info(logMsg);
		return NULL;
	 }

	item = this->CreateItem();
	item->SetStatus(OvalEnum::STATUS_EXISTS);
	
	item->AppendElement(new ItemEntity("service_name", serviceName, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
	
	if(lpsc->lpBinaryPathName != NULL && strlen(lpsc->lpBinaryPathName) > 0){
		item->AppendElement(new ItemEntity("path", Common::ToString(lpsc->lpBinaryPathName), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
	}else{
		item->AppendElement(new ItemEntity("path", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));	
	}

	serviceType = WindowsServicesProbe::ServiceTypeToString(lpsc->dwServiceType);
	if(serviceType.length() > 0){
		item->AppendElement(new ItemEntity("service_type", serviceType, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
	}else{
		item->AppendElement(new ItemEntity("service_type", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));
	}

	startType = WindowsServicesProbe::StartTypeToString(lpsc->dwStartType);
	if(startType.length() > 0){
		item->AppendElement(new ItemEntity("start_type", startType, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
	}else{
		item->AppendElement(new ItemEntity("start_type", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));
	}

	if(lpsc->lpDisplayName != NULL && strlen(lpsc->lpDisplayName) > 0){
		item->AppendElement(new ItemEntity("display_name", Common::ToString(lpsc->lpDisplayName), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
	}else{
		item->AppendElement(new ItemEntity("display_name", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));
	}

	if(lpsc->lpServiceStartName != NULL && strlen(lpsc->lpServiceStartName) > 0){
		item->AppendElement(new ItemEntity("start_name", Common::ToString(lpsc->lpServiceStartName), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
	}else{
		item->AppendElement(new ItemEntity("start_name", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));
	}

	if(lpsd->lpDescription != NULL && strlen(lpsd->lpDescription) > 0){
		item->AppendElement(new ItemEntity("description", Common::ToString(lpsd->lpDescription), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
	}else{
		item->AppendElement(new ItemEntity("description", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));
	}
	
	if(lpsc->lpDependencies != NULL && strlen(lpsc->lpDependencies) > 0){
		item->AppendElement(new ItemEntity("dependencies", Common::ToString(lpsc->lpDependencies), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
	}else{
		item->AppendElement(new ItemEntity("dependencies", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));
	}

    LocalFree(lpsc); 
    LocalFree(lpsd);
	CloseServiceHandle(schService); 

	// open service for status query
    schService2 = OpenService(serviceMgr->get(), serviceName.c_str(), SERVICE_QUERY_STATUS); 
    if (schService2 == NULL){  
		 std::string logMsg = "ERROR: The function OpenService() could not access a service on the system.";
		 Log::Info(logMsg);
		 return NULL;
    }

	if (!QueryServiceStatusEx( 
				schService2,                     // handle to service 
				SC_STATUS_PROCESS_INFO,         // information level
				(LPBYTE) &ssStatus,             // address of structure
				sizeof(SERVICE_STATUS_PROCESS), // size of structure
				&dwBytesNeeded ) )              // size needed if buffer is too small
		{
			std::string logMsg = "ERROR: The function QueryServiceStatusEx() failed.";
			Log::Info(logMsg);
	}

	if(ssStatus.dwProcessId > 0){
		item->AppendElement(new ItemEntity("pid", Common::ToString(ssStatus.dwProcessId), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
	}else{
		item->AppendElement(new ItemEntity("pid", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));
	}

	string controlStr =  "";
	controlStr = WindowsServicesProbe::ControlToString(ssStatus.dwControlsAccepted);

	if(controlStr.length() > 0){
		item->AppendElement(new ItemEntity("controls_accepted", controlStr, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
	}else{
		item->AppendElement(new ItemEntity("controls_accepted", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));
	}

	string flagStr =  "";
	flagStr = WindowsServicesProbe::ServiceFlagToString(ssStatus.dwServiceFlags);

	if(flagStr.length() > 0){
		item->AppendElement(new ItemEntity("service_flag", flagStr, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
	}else{
		item->AppendElement(new ItemEntity("service_flag", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));
	}

	CloseServiceHandle(schService2); 

	return item;
}


string WindowsServicesProbe::ServiceTypeToString(DWORD type){
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Convert the ServiceType value to a string
	//
	// -----------------------------------------------------------------------
	string typeStr = "";

	switch(type) {
		case (WindowsServicesProbe::SERVICE_KERNEL_DRIVER_TYPE):
			typeStr = "Kernel Driver";
			break;
		case (WindowsServicesProbe::SERVICE_FILE_SYSTEM_DRIVER_TYPE):
			typeStr = "File System Driver";
			break;
		case (WindowsServicesProbe::SERVICE_WIN32_OWN_PROCESS_TYPE):
			typeStr = "Win32 Own Process";
			break;
		case (WindowsServicesProbe::SERVICE_WIN32_SHARE_PROCESS_TYPE):
			typeStr = "Win32 Share Process";
			break;
		case(WindowsServicesProbe::SERVICE_INTERACTIVE_OWN_PROCESS_TYPE):
			typeStr = "Interactive Process";
			break;
		case(WindowsServicesProbe::SERVICE_INTERACTIVE_SHARE_PROCESS_TYPE):
			typeStr = "Interactive Process";
			break;
		default:
			std::string logMsg = "WindowsServicesProbe::ServiceTypeToString - Error unsupported service type value.";
			Log::Info(logMsg);
			break;
	}

	return typeStr;
}


string WindowsServicesProbe::StartTypeToString(DWORD type){
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Convert the StartType value to a string
	//
	// -----------------------------------------------------------------------
	string typeStr = "";

	switch(type) {
		case (WindowsServicesProbe::SERVICE_BOOT_START_TYPE):
			typeStr = "Boot";
			break;
		case (WindowsServicesProbe::SERVICE_SYSTEM_START_TYPE):
			typeStr = "System";
			break;
		case (WindowsServicesProbe::SERVICE_AUTO_START_TYPE):
			typeStr = "Automatic";
			break;
		case (WindowsServicesProbe::SERVICE_DEMAND_START_TYPE):
			typeStr = "Manual";
			break;
		case(WindowsServicesProbe::SERVICE_DISABLED_TYPE):
			typeStr = "Disabled";
			break;
		default:
			std::string logMsg = "WindowsServicesProbe::StartTypeToString - Error unsupported service start type value.";
			Log::Info(logMsg);
			break;
	}

	return typeStr;
}

string WindowsServicesProbe::CurrentStateToString(DWORD type){
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Convert the StartType value to a string
	//
	// -----------------------------------------------------------------------
	string typeStr = "";

	switch(type) {
		case (WindowsServicesProbe::SERVICE_STOPPED_STATE):
			typeStr = "Stopped";
			break;
		case (WindowsServicesProbe::SERVICE_START_PENDING_STATE):
			typeStr = "Start Pending";
			break;
		case (WindowsServicesProbe::SERVICE_STOP_PENDING_STATE):
			typeStr = "Stop Pending";
			break;
		case (WindowsServicesProbe::SERVICE_RUNNING_STATE):
			typeStr = "Running";
			break;
		case(WindowsServicesProbe::SERVICE_CONTINUE_PENDING_STATE):
			typeStr = "Continue Pending";
			break;
		case(WindowsServicesProbe::SERVICE_PAUSE_PENDING_STATE):
			typeStr = "Pause Pending";
			break;
		case(WindowsServicesProbe::SERVICE_PAUSED_STATE):
			typeStr = "Paused";
			break;
		default:
			std::string logMsg = "WindowsServicesProbe::CurrentStateToString - Error unsupported service start type value.";
			Log::Info(logMsg);
			break;
	}

	return typeStr;
}

string WindowsServicesProbe::ControlToString(DWORD type){
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Convert the StartType value to a string
	//
	// -----------------------------------------------------------------------
	string typeStr = "";

	switch(type) {
		case (WindowsServicesProbe::SERVICE_CONTROL_ACCEPT_STOP):
			typeStr = "Stop";
			break;
		case (WindowsServicesProbe::SERVICE_CONTROL_ACCEPT_PAUSE_CONTINUE):
			typeStr = "Continue";
			break;
		case (WindowsServicesProbe::SERVICE_CONTROL_ACCEPT_SHUTDOWN):
			typeStr = "Shutdown";
			break;
		case (WindowsServicesProbe::SERVICE_CONTROL_ACCEPT_PARAMCHANGE):
			typeStr = "Param Change";
			break;
		case(WindowsServicesProbe::SERVICE_CONTROL_ACCEPT_NETBINDCHANGE):
			typeStr = "Net Bind Change";
			break;
		case(WindowsServicesProbe::SERVICE_CONTROL_ACCEPT_HARDWAREPROFILECHANGE):
			typeStr = "Hardware Profile Change";
			break;
		case(WindowsServicesProbe::SERVICE_CONTROL_ACCEPT_POWEREVENT):
			typeStr = "Power Event";
			break;
		case(WindowsServicesProbe::SERVICE_CONTROL_ACCEPT_SESSIONCHANGE):
			typeStr = "Session Change";
			break;
		case(WindowsServicesProbe::SERVICE_CONTROL_ACCEPT_PRESHUTDOWN):
			typeStr = "Pre Shutdown";
			break;
		case(WindowsServicesProbe::SERVICE_CONTROL_ACCEPT_TIMECHANGE):
			typeStr = "Time Change";
			break;
		case(WindowsServicesProbe::SERVICE__CONTROLACCEPT_TRIGGEREVENT):
			typeStr = "Trigger Event";
			break;
		default:
			std::string logMsg = "WindowsServicesProbe::ControlStateToString - Error unsupported service start type value.";
			Log::Info(logMsg);
			break;
	}

	return typeStr;
}

string WindowsServicesProbe::ServiceFlagToString(DWORD type){
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Convert the Service Flag value to a string
	//
	// -----------------------------------------------------------------------
	string typeStr = "";

	switch(type) {
		case (WindowsServicesProbe::SERVICE_NOT_IN_SYSTEM_PROCESS_FLAG):
			typeStr = "Non-System Process";
			break;
		case (WindowsServicesProbe::SERVICE_RUNS_IN_SYSTEM_PROCESS_FLAG):
			typeStr = "System Process";
			break;
		
		default:
			std::string logMsg = "WindowsServicesProbe::ServiceFlagToString - Error unsupported service flag value.";
			Log::Info(logMsg);
			break;
	}

	return typeStr;
}
