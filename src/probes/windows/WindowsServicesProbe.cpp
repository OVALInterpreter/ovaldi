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

#include <VectorPtrGuard.h>
#include <PrivilegeGuard.h>
#include "WindowsServicesProbe.h"

using namespace std;

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
		}else if ( serviceNameEntity->GetOperation() == OvalEnum::OPERATION_NOT_EQUAL ) {
            theServices = this->GetMatchingServices ( serviceNameEntity->GetValue() , false,  false);
        } else if ( serviceNameEntity->GetOperation() == OvalEnum::OPERATION_CASE_INSENSITIVE_NOT_EQUAL ) {
            theServices = this->GetMatchingServices ( serviceNameEntity->GetValue() , false, true);
        }else if ( serviceNameEntity->GetOperation() == OvalEnum::OPERATION_PATTERN_MATCH ) {
            theServices = this->GetMatchingServices ( serviceNameEntity->GetValue() , true, false );
        } 

    } else {
        // Get all services
		auto_ptr<StringSet> allServices(new StringSet());
       
        if ( serviceNameEntity->GetOperation() == OvalEnum::OPERATION_EQUALS ) {
            // In the case of equals simply loop through all the
            // variable values and add them to the set of all services
            // if they exist on the system
            VariableValueVector::iterator iterator;

            for ( iterator = serviceNameEntity->GetVarRef()->GetValues()->begin() ; iterator != serviceNameEntity->GetVarRef()->GetValues()->end() ; iterator++ ) {
                if ( this->ServiceExists ( ( *iterator )->GetValue(), false ) ) {
                    allServices->insert ( ( *iterator )->GetValue(), false );
                }
            }

        } else if ( serviceNameEntity->GetOperation() == OvalEnum::OPERATION_CASE_INSENSITIVE_EQUALS ) {
			VariableValueVector::iterator iterator;

            for ( iterator = serviceNameEntity->GetVarRef()->GetValues()->begin() ; iterator != serviceNameEntity->GetVarRef()->GetValues()->end() ; iterator++ ) {
                if ( this->ServiceExists ( ( *iterator )->GetValue(), true ) ) {
                    allServices->insert ( ( *iterator )->GetValue(), false );
                }
            }
		} else {
            allServices = this->GetMatchingServices ( ".*" , true, false );
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
	
    return theServices.release();
}

auto_ptr<StringSet> WindowsServicesProbe::GetMatchingServices ( string patternStr , bool isRegex, bool caseInsensitive ) {
   auto_ptr<StringSet> matchingServices(new StringSet());

	if(caseInsensitive){
		
		for ( StringSet::iterator iterator = services->begin() ; iterator != services->end() ; iterator++ ){
			if(! Common::EqualsIgnoreCase(*iterator,patternStr)){
				matchingServices->insert ( *iterator );
			}
		}

	}else{
		for ( StringSet::iterator iterator = services->begin() ; iterator != services->end() ; iterator++ ) {
		    if ( this->IsMatch ( patternStr , *iterator , isRegex ) ) {
		        matchingServices->insert ( *iterator );
		    }
		} 
	}

    return matchingServices;
}

StringSet* WindowsServicesProbe::GetAllServices() {
    StringSet* allServices = new StringSet();
	DWORD dwBytesNeeded = 0;
    DWORD dwServicesReturned = 0;
    DWORD dwResumedHandle = 0;
    DWORD dwServiceType = SERVICE_WIN32 | SERVICE_DRIVER;
	
    // Query services
    BOOL retVal = EnumServicesStatus(serviceMgr->get(), dwServiceType, SERVICE_STATE_ALL, NULL, sizeof(ENUM_SERVICE_STATUS), &dwBytesNeeded, &dwServicesReturned, &dwResumedHandle);
	DWORD errorCode = GetLastError();

	// Need big buffer
	while (!retVal && (ERROR_MORE_DATA == errorCode)) {

		// Set the buffer
		ENUM_SERVICE_STATUS* pServices = (ENUM_SERVICE_STATUS*)new char[dwBytesNeeded];

		retVal = EnumServicesStatus(serviceMgr->get(), SERVICE_WIN32 | SERVICE_DRIVER, SERVICE_STATE_ALL, pServices, dwBytesNeeded, &dwBytesNeeded, &dwServicesReturned, &dwResumedHandle);
		errorCode = GetLastError();

		// If the call failed for any other reason than more data
		if(!retVal && (ERROR_MORE_DATA != errorCode)){
			delete [] pServices;
			pServices = NULL; 

			delete allServices;
			allServices = NULL;

			throw ProbeException ( "ERROR: The function EnumServicesStatusEx() could not enumerate the services on the system. Microsoft System Error " + Common::ToString ( GetLastError() ) + ") - " + WindowsCommon::GetErrorMessage ( GetLastError() ) );
		}

		// now traverse each service to get information
		for (unsigned iIndex = 0; iIndex < dwServicesReturned; iIndex++) {
				string serviceName = (pServices + iIndex)->lpServiceName;
				allServices->insert(serviceName);
		}
 
		delete [] pServices;
		pServices = NULL;
	}

    return allServices;
}

bool WindowsServicesProbe::ServiceExists ( string serviceNameStr, bool caseInsensitive ) {
	if(caseInsensitive){
		
		for ( StringSet::iterator it = WindowsServicesProbe::services->begin(); it != WindowsServicesProbe::services->end(); it++ ) {

			string temp = (string)(*it);
			if(Common::EqualsIgnoreCase(serviceNameStr, temp)){
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
	Item* item = NULL;
    SC_HANDLE schService;
	SC_HANDLE schService2;
	SERVICE_STATUS_PROCESS ssStatus;
    LPQUERY_SERVICE_CONFIG lpsc = NULL; 
    LPSERVICE_DESCRIPTION lpsd = NULL;
    DWORD dwBytesNeeded = 0;
	DWORD cbBufSize = 0;
	DWORD dwError = 0; 
	vector<string> serviceType;
	vector<string> controlType;
	string startType;
	string path;
	string startName;

	// Giving ourselves this privilege seems to gain us access
	// to a lot more processes.
	PrivilegeGuard pg(SE_DEBUG_NAME, false);

    // open service
    schService = OpenService(serviceMgr->get(), serviceName.c_str(), SERVICE_QUERY_CONFIG); 
    AutoCloser<SC_HANDLE, BOOL(WINAPI&)(SC_HANDLE)> schServiceCloser(schService, CloseServiceHandle, "ServiceCloser");

    if (schService == NULL){  
		 string logMsg = "ERROR: The function OpenService() could not access a service on the system. Microsoft System Error " + Common::ToString (GetLastError()) + " - " + WindowsCommon::GetErrorMessage(GetLastError());
		 Log::Info(logMsg);
		 return NULL;
    }

    // Get the configuration information.
    if( !QueryServiceConfig(schService, NULL, 0, &dwBytesNeeded)){
        dwError = GetLastError();
        if( ERROR_INSUFFICIENT_BUFFER == dwError ){
            cbBufSize = dwBytesNeeded;
            lpsc = (LPQUERY_SERVICE_CONFIG) malloc(cbBufSize);

			if(lpsc == NULL){
				string logMsg = "ERROR: The function malloc() failed due to insufficient system memory.";
				Log::Info(logMsg);
				return NULL;
			}

        }else{
			string logMsg = "ERROR: The function QueryServiceConfig() failed.  Microsoft System Error " + Common::ToString ( GetLastError() ) + ") - " + WindowsCommon::GetErrorMessage ( GetLastError() );
			Log::Info(logMsg);
			return NULL;
		}
    }
  
    if( !QueryServiceConfig(schService, lpsc, cbBufSize, &dwBytesNeeded) ) {
		string logMsg = "ERROR: The function QueryServiceConfig() failed. Microsoft System Error " + Common::ToString ( GetLastError() ) + ") - " + WindowsCommon::GetErrorMessage ( GetLastError() );
		Log::Info(logMsg);
		free(lpsc);
		return NULL;
	}

    if( !QueryServiceConfig2( schService, SERVICE_CONFIG_DESCRIPTION, NULL, 0, &dwBytesNeeded)){
        dwError = GetLastError();
        if( ERROR_INSUFFICIENT_BUFFER == dwError ){
            cbBufSize = dwBytesNeeded;
            lpsd = (LPSERVICE_DESCRIPTION) malloc(cbBufSize);

			if(lpsd == NULL){
				string logMsg = "ERROR: The function malloc() failed due to insufficient system memory.";
				Log::Info(logMsg);
				free(lpsc);
				return NULL;
			}

		}else{
			string logMsg = "ERROR: The function QueryServiceConfig2() failed. Microsoft System Error " + Common::ToString ( GetLastError() ) + ") - " + WindowsCommon::GetErrorMessage ( GetLastError() );
			Log::Info(logMsg);
			free(lpsc);
			return NULL;
		}
    }
 
    if (! QueryServiceConfig2( schService, SERVICE_CONFIG_DESCRIPTION, (LPBYTE) lpsd, cbBufSize,  &dwBytesNeeded) ) {
		string logMsg = "ERROR: The function QueryServiceConfig2() failed. Microsoft System Error " + Common::ToString ( GetLastError() ) + ") - " + WindowsCommon::GetErrorMessage ( GetLastError() );
		Log::Info(logMsg);
		free(lpsc);
		free(lpsd);
		return NULL;
	 }

	item = this->CreateItem();
	item->SetStatus(OvalEnum::STATUS_EXISTS);
	
	item->AppendElement(new ItemEntity("service_name", serviceName, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
	
	if(lpsc->lpDisplayName != NULL && lpsc->lpDisplayName[0]){
		item->AppendElement(new ItemEntity("display_name", Common::ToString(lpsc->lpDisplayName), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
	}else{
		item->AppendElement(new ItemEntity("display_name", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_ERROR));
		string errorMessage = "The DisplayName could not be determined or was unrecognized.";
		item->AppendMessage(new OvalMessage(errorMessage));
	}

	if(lpsd->lpDescription != NULL && lpsd->lpDescription[0]){
		item->AppendElement(new ItemEntity("description", Common::ToString(lpsd->lpDescription), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
	}else{
		item->AppendElement(new ItemEntity("description", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));
	}

	serviceType = WindowsServicesProbe::ServiceTypeToString(lpsc->dwServiceType);
	if(!serviceType.empty()){

		for(vector<string>::iterator it = serviceType.begin(); it != serviceType.end(); ++it) {

			item->AppendElement(new ItemEntity("service_type", Common::ToString(*it), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
		}

	}else{
		item->AppendElement(new ItemEntity("service_type", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_ERROR));
		string errorMessage = "The ServiceType could not be determined or was unrecognized.";
		item->AppendMessage(new OvalMessage(errorMessage));
	}	

	startType = WindowsServicesProbe::StartTypeToString(lpsc->dwStartType);
	if(!startType.empty()){
		item->AppendElement(new ItemEntity("start_type", startType, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
	}else{
		item->AppendElement(new ItemEntity("start_type", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_ERROR));
		string errorMessage = "The StartType could not be determined or was unrecognized.";
		item->AppendMessage(new OvalMessage(errorMessage));
	}

	if(lpsc->lpBinaryPathName != NULL && lpsc->lpBinaryPathName[0]){
		path = Common::ToString(lpsc->lpBinaryPathName);
	}

	if(lpsc->lpServiceStartName != NULL && lpsc->lpServiceStartName[0]){
		startName = Common::ToString(lpsc->lpServiceStartName);
	}else{
		startName = "";
	}

	istringstream ss(lpsc->lpDependencies);
    istream_iterator<string> begin(ss), end;
    vector<string> depArrayTokens(begin, end); 

    free(lpsc); 
    free(lpsd);
	
	// open service for status query
    schService2 = OpenService(serviceMgr->get(), serviceName.c_str(), SERVICE_QUERY_STATUS); 
	AutoCloser<SC_HANDLE, BOOL(WINAPI&)(SC_HANDLE)> schService2Closer(schService2, CloseServiceHandle, "ServiceCloser");
    if (schService2 == NULL){  
		 string logMsg = "ERROR: The function OpenService() could not access a service on the system.  Microsoft System Error " + Common::ToString ( GetLastError() ) + ") - " + WindowsCommon::GetErrorMessage ( GetLastError() );
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
			string logMsg = "ERROR: The function QueryServiceStatusEx() failed. Microsoft System Error " + Common::ToString ( GetLastError() ) + ") - " + WindowsCommon::GetErrorMessage ( GetLastError() );
			Log::Info(logMsg);
			return NULL;
	}
	
	item->AppendElement(new ItemEntity("current_state", CurrentStateToString(ssStatus.dwCurrentState), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
	


	controlType = WindowsServicesProbe::ControlToString(ssStatus.dwControlsAccepted);
	if(!controlType.empty()){

		for(vector<string>::iterator it = controlType.begin(); it != controlType.end(); ++it) {

			item->AppendElement(new ItemEntity("controls_accepted", Common::ToString(*it), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
		}
	}else{
		item->AppendElement(new ItemEntity("controls_accepted", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));
	}

	// startName can be blank, so we don't check its length like we do with the other entities
	item->AppendElement(new ItemEntity("start_name", startName, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
	
	
	if(!path.empty()){
		item->AppendElement(new ItemEntity("path", path, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
	}else{
		item->AppendElement(new ItemEntity("path", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));	
	}

	item->AppendElement(new ItemEntity("pid", Common::ToString(ssStatus.dwProcessId), OvalEnum::DATATYPE_INTEGER, OvalEnum::STATUS_EXISTS));

	bool serviceFlag =  false;
	serviceFlag = WindowsServicesProbe::ServiceFlagToBool(ssStatus.dwServiceFlags);
	item->AppendElement(new ItemEntity("service_flag", Common::ToString(serviceFlag), OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS));

	if(!depArrayTokens.empty()){
		for(unsigned int i=0; i < depArrayTokens.size(); i++){
			item->AppendElement(new ItemEntity("dependencies", Common::ToString(depArrayTokens.at(i)), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
		}
	}else{
		item->AppendElement(new ItemEntity("dependencies", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));
	}

	return item;
}

vector<string> WindowsServicesProbe::ServiceTypeToString(DWORD type){
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Convert the ServiceType value to a vector of strings
	//
	// ----------------------------------------------------------------------- 

	vector<string> typeStrings;

	if(type & SERVICE_KERNEL_DRIVER){
			typeStrings.push_back("SERVICE_KERNEL_DRIVER");
	}

	if(type & SERVICE_FILE_SYSTEM_DRIVER){
		typeStrings.push_back("SERVICE_FILE_SYSTEM_DRIVER");
	}

	if(type & SERVICE_WIN32_OWN_PROCESS){
		typeStrings.push_back("SERVICE_WIN32_OWN_PROCESS");
	}

	if(type & SERVICE_WIN32_SHARE_PROCESS){
		typeStrings.push_back("SERVICE_WIN32_SHARE_PROCESS");
	}

	if(type & SERVICE_INTERACTIVE_PROCESS){
		typeStrings.push_back("SERVICE_INTERACTIVE_PROCESS");
	}

	return typeStrings;
}

string WindowsServicesProbe::StartTypeToString(DWORD type){
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Convert the StartType value to a string
	//
	// -----------------------------------------------------------------------
	string typeStr;

	switch(type) {
		case (SERVICE_BOOT_START):
			typeStr = "SERVICE_BOOT_START";
			break;
		case (SERVICE_SYSTEM_START):
			typeStr = "SERVICE_SYSTEM_START";
			break;
		case (SERVICE_AUTO_START):
			typeStr = "SERVICE_AUTO_START";
			break;
		case (SERVICE_DEMAND_START):
			typeStr = "SERVICE_DEMAND_START";
			break;
		case(SERVICE_DISABLED):
			typeStr = "SERVICE_DISABLED";
			break;
		default:
			string logMsg = "WindowsServicesProbe::StartTypeToString - Error unsupported service start type value.";
			Log::Info(logMsg);
			break;
	}

	return typeStr;
}

string WindowsServicesProbe::CurrentStateToString(DWORD type){
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Convert the CurrentStateType value to a string
	//
	// -----------------------------------------------------------------------
	string typeStr;

	switch(type) {
		case (SERVICE_STOPPED):
			typeStr = "SERVICE_STOPPED";
			break;
		case (SERVICE_START_PENDING):
			typeStr = "SERVICE_START_PENDING";
			break;
		case (SERVICE_STOP_PENDING):
			typeStr = "SERVICE_STOP_PENDING";
			break;
		case (SERVICE_RUNNING):
			typeStr = "SERVICE_RUNNING";
			break;
		case(SERVICE_CONTINUE_PENDING):
			typeStr = "SERVICE_CONTINUE_PENDING";
			break;
		case(SERVICE_PAUSE_PENDING):
			typeStr = "SERVICE_PAUSE_PENDING";
			break;
		case(SERVICE_PAUSED):
			typeStr = "SERVICE_PAUSED";
			break;
		default:
			string logMsg = "WindowsServicesProbe::CurrentStateToString - Error unsupported service start type value.";
			Log::Info(logMsg);
			break;
	}

	return typeStr;
}

vector<string> WindowsServicesProbe::ControlToString(DWORD type){
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Convert the ControlsAcceptedType value to a vector of strings
	//
	// -----------------------------------------------------------------------
	vector<string> typeStrings;


	if(type & SERVICE_ACCEPT_STOP) {
			typeStrings.push_back("SERVICE_ACCEPT_STOP");
	}

	if(type & SERVICE_ACCEPT_PAUSE_CONTINUE){
		typeStrings.push_back("SERVICE_ACCEPT_PAUSE_CONTINUE");
	}

	if(type & SERVICE_ACCEPT_SHUTDOWN){
		typeStrings.push_back("SERVICE_ACCEPT_SHUTDOWN");
	}

	if(type & SERVICE_ACCEPT_PARAMCHANGE){
		typeStrings.push_back("SERVICE_ACCEPT_PARAMCHANGE");
	}
	
	if(type & SERVICE_ACCEPT_NETBINDCHANGE){
		typeStrings.push_back("SERVICE_ACCEPT_NETBINDCHANGE");
	}

	if(type & SERVICE_ACCEPT_HARDWAREPROFILECHANGE){
		typeStrings.push_back("SERVICE_ACCEPT_HARDWAREPROFILECHANGE");
	}	
	
	if(type & SERVICE_ACCEPT_POWEREVENT){
		typeStrings.push_back("SERVICE_ACCEPT_POWEREVENT");
	}	
		
	if(type & SERVICE_ACCEPT_SESSIONCHANGE){
		typeStrings.push_back("SERVICE_ACCEPT_SESSIONCHANGE");
	}	

	if(type & SERVICE_ACCEPT_PRESHUTDOWN){
		typeStrings.push_back("SERVICE_ACCEPT_PRESHUTDOWN");
	}	
	
	if(type & SERVICE_ACCEPT_TIMECHANGE){
		typeStrings.push_back("SERVICE_ACCEPT_TIMECHANGE");
	}	
		
	if(type & SERVICE_ACCEPT_TRIGGEREVENT){
		typeStrings.push_back("SERVICE_ACCEPT_TRIGGEREVENT");
	}
		
	return typeStrings;
}

bool WindowsServicesProbe::ServiceFlagToBool(DWORD type){
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Convert the Service Flag value to a bool
	//
	// -----------------------------------------------------------------------
	bool serviceFlag = false;

	switch(type) {
		case (WindowsServicesProbe::SERVICE_NOT_IN_SYSTEM_PROCESS_FLAG):
			serviceFlag = false;
			break;
		case (WindowsServicesProbe::SERVICE_RUNS_IN_SYSTEM_PROCESS_FLAG):
			serviceFlag = true;
			break;
		
		default:
			string logMsg = "WindowsServicesProbe::ServiceFlagToString - Error unsupported service flag value.";
			Log::Info(logMsg);
			break;
	}

	return serviceFlag;
}

