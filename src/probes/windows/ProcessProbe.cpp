//
//
//****************************************************************************************//
// Copyright (c) 2002-2014, The MITRE Corporation
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

#include <Windows.h>
#include <Psapi.h>
#include <tlhelp32.h>

#include "WindowsCommon.h"
#include "WMIUtil.h"

#include "ProcessProbe.h"

using namespace std;

//****************************************************************************************//
//                              ProcessProbe Class                                 //
//****************************************************************************************//
ProcessProbe* ProcessProbe::instance = NULL;

ProcessProbe::ProcessProbe() {
    ProcessProbe::processes = NULL;
    ProcessProbe::pathMap = NULL;
    ProcessProbe::commandLineMap = NULL;
}

ProcessProbe::~ProcessProbe() {
    ProcessProbe::instance = NULL;
    ProcessProbe::DeleteProcesses();
    ProcessProbe::DeletePathMap();
    ProcessProbe::DeleteCommandLineMap();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

AbsProbe* ProcessProbe::Instance() {
    // Use lazy initialization for instance of the process probe
    if ( instance == NULL ) {
        ProcessProbe::instance = new ProcessProbe();
    }

    return ProcessProbe::instance;
}

ItemVector* ProcessProbe::CollectItems ( Object* object ) {
    // Get the command_line object entity from the provided object
    ObjectEntity* commandLineEntity = object->GetElementByName ( "command_line" );

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

    if ( ProcessProbe::processes == NULL ) {
        ProcessProbe::GetAllProcesses();
    }

    if ( commandLineEntity->GetVarRef() == NULL ) {
        if ( commandLineEntity->GetOperation() == OvalEnum::OPERATION_EQUALS ) {
            Item* aProcess = ProcessProbe::GetProcess ( commandLineEntity->GetValue() );

            if ( aProcess != NULL ) {
                collectedItems->push_back ( new Item ( *aProcess ) );

            } else {
                aProcess = this->CreateItem();
                aProcess->SetStatus ( OvalEnum::STATUS_DOES_NOT_EXIST );
                aProcess->AppendElement ( new ItemEntity ( "command_line" , "" , OvalEnum::DATATYPE_STRING , OvalEnum::STATUS_DOES_NOT_EXIST ) );
                collectedItems->push_back ( aProcess );
            }

        } else {
            // Loop through all processes if they are a regex match on command line create item and return it
            for ( ItemVector::iterator it1 = ProcessProbe::processes->begin() ; it1 != ProcessProbe::processes->end() ; it1++ ) {
                ItemEntityVector* commandLineVector = ( *it1 )->GetElementsByName ( "command_line" );

                for ( ItemEntityVector::iterator it2 = commandLineVector->begin() ; it2 != commandLineVector->end() ; it2++ ) {
                    if ( commandLineEntity->GetOperation() == OvalEnum::OPERATION_NOT_EQUAL ) {
                        if ( commandLineEntity->GetValue().compare ( ( *it2 )->GetValue() ) != 0 ) {
                            collectedItems->push_back ( new Item ( **it1 ) );
                        }

                    } else {
                        if ( this->myMatcher->IsMatch ( commandLineEntity->GetValue().c_str() , ( *it2 )->GetValue().c_str() ) ) {
                            collectedItems->push_back ( new Item ( **it1 ) );
                        }
                    }
                }
            }
        }

    } else {
        // Loop through all processes on the system
        // Only keep the processes that match operation, value, and var check
        for ( ItemVector::iterator it1 = ProcessProbe::processes->begin() ; it1 != ProcessProbe::processes->end() ; it1++ ) {
            ItemEntityVector* commandLineVector = ( *it1 )->GetElementsByName ( "command_line" );

            for ( ItemEntityVector::iterator it2 = commandLineVector->begin() ; it2 != commandLineVector->end() ; it2++ ) {
                if ( commandLineEntity->Analyze ( *it2 ) == OvalEnum::RESULT_TRUE ) {
                    collectedItems->push_back ( new Item ( **it1 ) );
                }
            }
        }
    }

    return collectedItems;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

Item* ProcessProbe::CreateItem() {
    Item* item = new Item ( 0 ,
                            "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows" ,
                            "win-sc" ,
                            "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows windows-system-characteristics-schema.xsd" ,
                            OvalEnum::STATUS_ERROR ,
                            "process_item" );
    return item;
}

void ProcessProbe::GetAllProcesses() {
    PROCESSENTRY32 processEntry;
    ProcessProbe::processes = new ItemVector();
    HANDLE processHandle = CreateToolhelp32Snapshot ( TH32CS_SNAPPROCESS , 0 );

    if ( processHandle == INVALID_HANDLE_VALUE ) {
        throw ProbeException ( "Error: The function CreateToolhelp32Snapshot() was unable to take a snapshot of the processes on the system. Microsoft System Error " + Common::ToString ( GetLastError() ) + " - " + WindowsCommon::GetErrorMessage ( GetLastError() ) );
    }

    if ( !WindowsCommon::EnablePrivilege ( SE_DEBUG_NAME ) ) {
        throw ProbeException ( "Error: The SeDebugPrivilege could not be enabled. Microsoft System Error " + Common::ToString ( GetLastError() ) + " - " + WindowsCommon::GetErrorMessage ( GetLastError() ) );
    }

    ProcessProbe::GetLogicalPathToDevicePath();
    ProcessProbe::GetAllCommandLineValues();

	processEntry.dwSize = sizeof(PROCESSENTRY32);

    if ( Process32First ( processHandle, &processEntry ) ) {
        ProcessProbe::BuildProcessItem ( processEntry );

        while ( Process32Next ( processHandle, &processEntry ) ) {
            ProcessProbe::BuildProcessItem ( processEntry );
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

Item* ProcessProbe::GetProcess ( string commandLineStr ) {
    for ( ItemVector::iterator it1 = ProcessProbe::processes->begin() ; it1 != ProcessProbe::processes->end() ; it1++ ) {
        ItemEntityVector* commandLineVector = ( *it1 )->GetElementsByName ( "command_line" );

        for ( ItemEntityVector::iterator it2 = commandLineVector->begin() ; it2 != commandLineVector->end() ; it2++ ) {
            if ( ( *it2 )->GetValue().compare ( commandLineStr ) == 0 ) {
                return ( *it1 );
            }
        }
    }

    return NULL;
}

void ProcessProbe::BuildProcessItem ( PROCESSENTRY32 processEntry ) {
    HANDLE openProcessHandle = OpenProcess ( PROCESS_ALL_ACCESS, false, processEntry.th32ProcessID );

    if ( openProcessHandle != NULL ) {
        StringStringMultiMap::iterator it;
        string commandLineStr = "";
        string deviceProcessImageNameStr = "";
        LPTSTR deviceProcessImageName = ( LPTSTR ) malloc ( sizeof ( TCHAR ) * MAX_PATH );
        unsigned int position = 0;

        if ( ( it = ProcessProbe::commandLineMap->find ( Common::ToString ( processEntry.th32ProcessID ) ) ) != ProcessProbe::commandLineMap->end() ) {
            commandLineStr = it->second;
        }

        if ( GetProcessImageFileName ( openProcessHandle, deviceProcessImageName, MAX_PATH ) ) {
            deviceProcessImageNameStr = deviceProcessImageName;

            for ( StringStringMultiMap::iterator iterator = ProcessProbe::pathMap->begin(); iterator != ProcessProbe::pathMap->end(); iterator++ ) {
                if ( ( position = deviceProcessImageNameStr.find ( iterator->first ) ) != string::npos ) {
                    deviceProcessImageNameStr.replace ( position, iterator->first.length(), iterator->second );
                    deviceProcessImageNameStr = deviceProcessImageNameStr.substr ( 0, deviceProcessImageNameStr.rfind ( processEntry.szExeFile ) - 1 );
                }
            }

            Item * item = this->CreateItem();
            item->SetStatus ( OvalEnum::STATUS_EXISTS );
            ( commandLineStr.compare ( "" ) == 0 ) ? item->AppendElement ( new ItemEntity ( "command_line" , "" , OvalEnum::DATATYPE_STRING , OvalEnum::STATUS_DOES_NOT_EXIST ) ) : item->AppendElement ( new ItemEntity ( "command_line" , commandLineStr , OvalEnum::DATATYPE_STRING , OvalEnum::STATUS_EXISTS ) );
            item->AppendElement ( new ItemEntity ( "pid" , Common::ToString ( processEntry.th32ProcessID ) , OvalEnum::DATATYPE_INTEGER , OvalEnum::STATUS_EXISTS ) );
            item->AppendElement ( new ItemEntity ( "ppid" , Common::ToString ( processEntry.th32ParentProcessID ) , OvalEnum::DATATYPE_INTEGER , OvalEnum::STATUS_EXISTS ) );
            item->AppendElement ( new ItemEntity ( "priority" , Common::ToString ( processEntry.pcPriClassBase ) , OvalEnum::DATATYPE_STRING , OvalEnum::STATUS_EXISTS ) );
            item->AppendElement ( new ItemEntity ( "image_path" , processEntry.szExeFile , OvalEnum::DATATYPE_STRING , OvalEnum::STATUS_EXISTS ) );
            ( deviceProcessImageNameStr.compare ( "" ) == 0 ) ? item->AppendElement ( new ItemEntity ( "current_dir" , "" , OvalEnum::DATATYPE_STRING , OvalEnum::STATUS_DOES_NOT_EXIST ) ) : item->AppendElement ( new ItemEntity ( "current_dir" , deviceProcessImageNameStr , OvalEnum::DATATYPE_STRING , OvalEnum::STATUS_EXISTS ) );
            ProcessProbe::processes->push_back ( item );
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

void ProcessProbe::GetLogicalPathToDevicePath() {
    string deviceDriveStr;
    string logicalDriveStr;
    char logicalDrive[3];
    char deviceDrive[MAX_PATH];
    unsigned int drives = GetLogicalDrives();
    unsigned int charCount = 0;
    ProcessProbe::pathMap = new StringStringMultiMap();
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


void ProcessProbe::GetAllCommandLineValues() {
    vector<WMIItem> wmiItems = WMIUtil::GetPropertyValues ( "ROOT\\CIMV2", "select * from win32_process", "processid", "commandline" );
    ProcessProbe::commandLineMap = new StringStringMultiMap();

    for ( unsigned int i = 0; i < wmiItems.size(); i++ ) {
        ProcessProbe::commandLineMap->insert ( make_pair ( wmiItems[i].KeyName, wmiItems[i].PropertyValue ) );
    }
}

void ProcessProbe::DeleteCommandLineMap() {
    if ( ProcessProbe::commandLineMap != NULL ) {
        ProcessProbe::commandLineMap->clear();
        delete ProcessProbe::commandLineMap;
    }
}

void ProcessProbe::DeletePathMap() {
    if ( ProcessProbe::pathMap != NULL ) {
        ProcessProbe::pathMap->clear();
        delete ProcessProbe::pathMap;
    }
}

void ProcessProbe::DeleteProcesses() {
    if ( ProcessProbe::processes != NULL ) {
        ItemVector::iterator it;

        for ( it = ProcessProbe::processes->begin() ; it != ProcessProbe::processes->end() ; it++ ) {
            if ( ( *it ) != NULL ) {
                delete ( *it );
                ( *it ) = NULL;
            }
        }

        delete ProcessProbe::processes;
        ProcessProbe::processes = NULL;
    }

    return;
}