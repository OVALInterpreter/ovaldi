//
//
//****************************************************************************************//
// Copyright (c) 2002-2010, The MITRE Corporation
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
#include "Process58Probe.h"

//****************************************************************************************//
//                              Process58Probe Class                                 //
//****************************************************************************************//
Process58Probe* Process58Probe::instance = NULL;

Process58Probe::Process58Probe() {
    Process58Probe::processes = NULL;
    Process58Probe::pathMap = NULL;
    Process58Probe::commandLineMap = NULL;
}

Process58Probe::~Process58Probe() {
    Process58Probe::instance = NULL;
    Process58Probe::DeleteProcesses();
    Process58Probe::DeletePathMap();
    Process58Probe::DeleteCommandLineMap();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

AbsProbe* Process58Probe::Instance() {
    // Use lazy initialization for instance of the process probe
    if ( instance == NULL ) {
        Process58Probe::instance = new Process58Probe();
    }

    return Process58Probe::instance;
}

ItemVector* Process58Probe::CollectItems ( Object* object ) {
    // Get the command_line object entity from the provided object
    ObjectEntity* commandLineEntity = object->GetElementByName ( "command_line" );
	ObjectEntity* pidEntity = object->GetElementByName("pid");

    // Check datatypes - only allow the string datatype
    if ( commandLineEntity->GetDatatype() != OvalEnum::DATATYPE_STRING ) {
        throw ProbeException ( "Error: Invalid data type specified on command_line. Found: " + OvalEnum::DatatypeToString ( commandLineEntity->GetDatatype() ) );
    }

    // Check operation - only allow  equals, not equals and pattern match
    if ( commandLineEntity->GetOperation() != OvalEnum::OPERATION_EQUALS && commandLineEntity->GetOperation() != OvalEnum::OPERATION_PATTERN_MATCH && commandLineEntity->GetOperation() != OvalEnum::OPERATION_NOT_EQUAL ) {
        throw ProbeException ( "Error: Invalid operation specified on command_line. Found: " + OvalEnum::OperationToString ( commandLineEntity->GetOperation() ) );
    }

    // Use lazy initialization to gather all the processes on the system
    ItemVector* collectedItems = new ItemVector();

    if ( Process58Probe::processes == NULL ) {
        Process58Probe::GetAllProcesses();
    }

	for (ItemVector::iterator iter = this->processes->begin();
		iter != this->processes->end();
		++iter) {
		if (object->Analyze(*iter))
			collectedItems->push_back(new Item(**iter));
	}

    return collectedItems;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

Item* Process58Probe::CreateItem() {
    Item* item = new Item ( 0 ,
                            "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows" ,
                            "win-sc" ,
                            "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows windows-system-characteristics-schema.xsd" ,
                            OvalEnum::STATUS_ERROR ,
                            "process_item" );
    return item;
}

void Process58Probe::GetAllProcesses() {
    PROCESSENTRY32 processEntry;
    Process58Probe::processes = new ItemVector();
    HANDLE processHandle = CreateToolhelp32Snapshot ( TH32CS_SNAPPROCESS , 0 );

    if ( processHandle == INVALID_HANDLE_VALUE ) {
        throw ProbeException ( "Error: The function CreateToolhelp32Snapshot() was unable to take a snapshot of the processes on the system. Microsoft System Error " + Common::ToString ( GetLastError() ) + " - " + WindowsCommon::GetErrorMessage ( GetLastError() ) );
    }

    if ( !WindowsCommon::EnablePrivilege ( SE_DEBUG_NAME ) ) {
        throw ProbeException ( "Error: The SeDebugPrivilege could not be enabled. Microsoft System Error " + Common::ToString ( GetLastError() ) + " - " + WindowsCommon::GetErrorMessage ( GetLastError() ) );
    }

    Process58Probe::GetLogicalPathToDevicePath();
    Process58Probe::GetAllCommandLineValues();

	processEntry.dwSize = sizeof(PROCESSENTRY32);

    if ( Process32First ( processHandle, &processEntry ) ) {
        Process58Probe::BuildProcessItem ( processHandle, processEntry );

        while ( Process32Next ( processHandle, &processEntry ) ) {
            Process58Probe::BuildProcessItem ( processHandle, processEntry );
        }
    }

    if ( !WindowsCommon::DisableAllPrivileges() ) {
        throw ProbeException ( "Error: All of the privileges could not be disabled. Microsoft System Error " + Common::ToString ( GetLastError() ) + " - " + WindowsCommon::GetErrorMessage ( GetLastError() ) );
    }

    if ( !CloseHandle ( processHandle ) ) {
        throw ProbeException ( "Error: The handle opened by the CreateToolhelp32Snapshot() function call could not be closed with the CloseHandle() function call. Microsoft System Error " + Common::ToString ( GetLastError() ) + " - " + WindowsCommon::GetErrorMessage ( GetLastError() ) );
    }

    return;
}

Item* Process58Probe::GetProcess ( string commandLineStr ) {
    for ( ItemVector::iterator it1 = Process58Probe::processes->begin() ; it1 != Process58Probe::processes->end() ; it1++ ) {
        ItemEntityVector* commandLineVector = ( *it1 )->GetElementsByName ( "command_line" );

        for ( ItemEntityVector::iterator it2 = commandLineVector->begin() ; it2 != commandLineVector->end() ; it2++ ) {
            if ( ( *it2 )->GetValue().compare ( commandLineStr ) == 0 ) {
                return ( *it1 );
            }
        }
    }

    return NULL;
}

void Process58Probe::BuildProcessItem ( HANDLE processHandle, PROCESSENTRY32 processEntry ) {
    HANDLE openProcessHandle = OpenProcess ( PROCESS_ALL_ACCESS, false, processEntry.th32ProcessID );

    if ( openProcessHandle != NULL ) {
        StringStringMultiMap::iterator it;
        string commandLineStr = "";
        string deviceProcessImageNameStr = "";
        LPTSTR deviceProcessImageName = ( LPTSTR ) malloc ( sizeof ( TCHAR ) * MAX_PATH );
        int position = 0;

        if ( ( it = Process58Probe::commandLineMap->find ( Common::ToString ( processEntry.th32ProcessID ) ) ) != Process58Probe::commandLineMap->end() ) {
            commandLineStr = it->second;
        }

        if ( GetProcessImageFileName ( openProcessHandle, deviceProcessImageName, MAX_PATH ) ) {
            deviceProcessImageNameStr = deviceProcessImageName;

            for ( StringStringMultiMap::iterator iterator = Process58Probe::pathMap->begin(); iterator != Process58Probe::pathMap->end(); iterator++ ) {
                if ( ( position = deviceProcessImageNameStr.find ( iterator->first ) ) != string::npos ) {
                    deviceProcessImageNameStr.replace ( position, iterator->first.length(), iterator->second );
                    deviceProcessImageNameStr = deviceProcessImageNameStr.substr ( 0, deviceProcessImageNameStr.rfind ( processEntry.szExeFile ) - 1 );
                }
            }

            Item * item = this->CreateItem();
            item->SetStatus ( OvalEnum::STATUS_EXISTS );
            ( commandLineStr.compare ( "" ) == 0 ) ? item->AppendElement ( new ItemEntity ( "command_line" , commandLineStr , OvalEnum::DATATYPE_STRING , true , OvalEnum::STATUS_DOES_NOT_EXIST ) ) : item->AppendElement ( new ItemEntity ( "command_line" , commandLineStr , OvalEnum::DATATYPE_STRING , true , OvalEnum::STATUS_EXISTS ) );
            item->AppendElement ( new ItemEntity ( "pid" , Common::ToString ( processEntry.th32ProcessID ) , OvalEnum::DATATYPE_INTEGER , true , OvalEnum::STATUS_EXISTS ) );
            item->AppendElement ( new ItemEntity ( "ppid" , Common::ToString ( processEntry.th32ParentProcessID ) , OvalEnum::DATATYPE_INTEGER , false , OvalEnum::STATUS_EXISTS ) );
            item->AppendElement ( new ItemEntity ( "priority" , Common::ToString ( processEntry.pcPriClassBase ) , OvalEnum::DATATYPE_STRING , false , OvalEnum::STATUS_EXISTS ) );
            item->AppendElement ( new ItemEntity ( "image_path" , processEntry.szExeFile , OvalEnum::DATATYPE_STRING , false , OvalEnum::STATUS_EXISTS ) );
            ( deviceProcessImageNameStr.compare ( "" ) == 0 ) ? item->AppendElement ( new ItemEntity ( "current_dir" , deviceProcessImageNameStr , OvalEnum::DATATYPE_STRING , false , OvalEnum::STATUS_DOES_NOT_EXIST ) ) : item->AppendElement ( new ItemEntity ( "current_dir" , deviceProcessImageNameStr , OvalEnum::DATATYPE_STRING , false , OvalEnum::STATUS_EXISTS ) );
            Process58Probe::processes->push_back ( item );
		}

        if ( deviceProcessImageName != NULL ) {
            free ( deviceProcessImageName );
            deviceProcessImageName = NULL;
        }

        if ( !CloseHandle ( openProcessHandle ) ) {
            throw ProbeException ( "Error: The handle opened by the OpenProcess() function call could not be closed with the CloseHandle() function call. Microsoft System Error " + Common::ToString ( GetLastError() ) + " - " + WindowsCommon::GetErrorMessage ( GetLastError() ) );
        }
    }
}

void Process58Probe::GetLogicalPathToDevicePath() {
    string deviceDriveStr;
    string logicalDriveStr;
    char logicalDrive[3];
    char deviceDrive[MAX_PATH];
    unsigned int drives = GetLogicalDrives();
    unsigned int charCount = 0;
    Process58Probe::pathMap = new StringStringMultiMap();
    logicalDrive[1] = ':';
    logicalDrive[2] = '\0';

    while ( drives != 0 ) {
        if ( drives & 1 ) {
            logicalDrive[0] = ( char ) ( 65 + charCount );

            if ( QueryDosDevice ( logicalDrive, deviceDrive, MAX_PATH ) ) {
                deviceDriveStr = deviceDrive;
                logicalDriveStr = logicalDrive;
                pathMap->insert ( make_pair ( deviceDriveStr , logicalDriveStr ) );

            } else {
                throw ProbeException ( "Error: The QueryDosDevice() function call could not retrieve the device drive for the logical drive '" + logicalDriveStr + "'. Microsoft System Error " + Common::ToString ( GetLastError() ) + " - " + WindowsCommon::GetErrorMessage ( GetLastError() ) );
            }
        }

        drives = drives >> 1;
        ++charCount;
    }
}


void Process58Probe::GetAllCommandLineValues() {
    vector<WMIItem> wmiItems = WMIUtil::GetPropertyValues ( "ROOT\\CIMV2", "select * from win32_process", "processid", "commandline" );
    Process58Probe::commandLineMap = new StringStringMultiMap();

    for ( unsigned int i = 0; i < wmiItems.size(); i++ ) {
        Process58Probe::commandLineMap->insert ( make_pair ( wmiItems[i].KeyName, wmiItems[i].PropertyValue ) );
    }
}

void Process58Probe::DeleteCommandLineMap() {
    if ( Process58Probe::commandLineMap != NULL ) {
        Process58Probe::commandLineMap->clear();
        delete Process58Probe::commandLineMap;
    }
}

void Process58Probe::DeletePathMap() {
    if ( Process58Probe::pathMap != NULL ) {
        Process58Probe::pathMap->clear();
        delete Process58Probe::pathMap;
    }
}

void Process58Probe::DeleteProcesses() {
    if ( Process58Probe::processes != NULL ) {
        ItemVector::iterator it;

        for ( it = Process58Probe::processes->begin() ; it != Process58Probe::processes->end() ; it++ ) {
            if ( ( *it ) != NULL ) {
                delete ( *it );
                ( *it ) = NULL;
            }
        }

        delete Process58Probe::processes;
        Process58Probe::processes = NULL;
    }

    return;
}