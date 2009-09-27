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

#include "FileAuditedPermissions53Probe.h"

//****************************************************************************************//
//                              FileAuditedPermissions53Probe Class                       //
//****************************************************************************************//
FileAuditedPermissions53Probe* FileAuditedPermissions53Probe::instance = NULL;

FileAuditedPermissions53Probe::FileAuditedPermissions53Probe() {
}

FileAuditedPermissions53Probe::~FileAuditedPermissions53Probe() {
    instance = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* FileAuditedPermissions53Probe::Instance() {
    // Use lazy initialization.
    if ( instance == NULL )
        instance = new FileAuditedPermissions53Probe();

    return instance;
}

ItemVector* FileAuditedPermissions53Probe::CollectItems ( Object* object ) {
    // Get the path, file name, and file path.
    ObjectEntity* path = object->GetElementByName ( "path" );
    ObjectEntity* fileName = object->GetElementByName ( "filename" );
    ObjectEntity* filePath = object->GetElementByName ( "filepath" );

    if ( filePath != NULL )
        throw ProbeException ( "Error: The filepath entity is not currently supported." );

    // Get trustee SID.
    ObjectEntity* trusteeSID = object->GetElementByName ( "trustee_sid" );

    // Check trustee datatypes - only allow string.
    if ( trusteeSID->GetDatatype() != OvalEnum::DATATYPE_STRING ) {
        throw ProbeException ( "Error: Invalid data type specified on trustee_sid. Found: " + OvalEnum::DatatypeToString ( trusteeSID->GetDatatype() ) );
    }

    // Check trustee operation - only allow  equals, not equals and pattern match.
    if ( trusteeSID->GetOperation() != OvalEnum::OPERATION_EQUALS
            && trusteeSID->GetOperation() != OvalEnum::OPERATION_PATTERN_MATCH
            && trusteeSID->GetOperation() != OvalEnum::OPERATION_NOT_EQUAL ) {
        throw ProbeException ( "Error: Invalid operation specified on trustee_sid. Found: " + OvalEnum::OperationToString ( trusteeSID->GetOperation() ) );
    }

    ItemVector *collectedItems = new ItemVector();
    // Support behaviors - init with defaults.
    bool includeGroupBehavior = true;
    bool resolveGroupBehavior = false;

    if ( object->GetBehaviors()->size() != 0 ) {
        BehaviorVector* behaviors = object->GetBehaviors();

        for ( BehaviorVector::iterator iterator = behaviors->begin(); iterator != behaviors->end(); iterator++ ) {
            Behavior* behavior = ( *iterator );

            if ( behavior->GetName().compare ( "include_group" ) == 0 )  {
                if ( behavior->GetValue().compare ( "false" ) == 0 ) {
                    includeGroupBehavior = false;
                }

            } else if ( behavior->GetName().compare ( "resolve_group" ) == 0 ) {
                if ( behavior->GetValue().compare ( "true" ) == 0 ) {
                    resolveGroupBehavior = true;
                }

                Log::Info ( "Deprecated behavior found when collecting " + object->GetId() + ". Found behavior: " + behavior->GetName() + " = " + behavior->GetValue() );

            } else if ( behavior->GetName().compare ( "max_depth" ) == 0 || behavior->GetName().compare ( "recurse_direction" ) == 0 ) {
                // Skip these they are supported in the file finder class.
            } else {
                Log::Info ( "Unsupported behavior found when collecting " + object->GetId() + ". Found behavior: " + behavior->GetName() + " = " + behavior->GetValue() );
            }
        }
    }

    FileFinder fileFinder;
    StringPairVector* filePaths = fileFinder.SearchFiles ( path, fileName, object->GetBehaviors() );

    if ( filePaths->size() > 0 ) {
        // Loop through all file paths.
        for ( StringPairVector::iterator iterator = filePaths->begin(); iterator != filePaths->end(); iterator++ ) {
            StringPair* fp = ( *iterator );

            // If there is no file name and the fileName ObjectEntity is not set to nil.
            if ( fp->second.compare ( "" ) == 0 && !fileName->GetNil() ) {
                Item* item = NULL;
                // Check if the code should report that the filename does not exist.
                StringVector fileNames;

                if ( fileFinder.ReportFileNameDoesNotExist ( fp->first, fileName, &fileNames ) ) {
                    StringVector::iterator iterator;

                    for ( iterator = fileNames.begin(); iterator != fileNames.end(); iterator++ ) {
                        item = this->CreateItem();
                        item->SetStatus ( OvalEnum::STATUS_DOES_NOT_EXIST );
                        item->AppendElement ( new ItemEntity ( "path", fp->first, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS ) );
                        item->AppendElement ( new ItemEntity ( "filename", ( *iterator ), OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_DOES_NOT_EXIST ) );
                        collectedItems->push_back ( item );
                    }

                } else {
                    item = this->CreateItem();
                    item->SetStatus ( OvalEnum::STATUS_EXISTS );
                    item->AppendElement ( new ItemEntity ( "path", fp->first, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS ) );
                    collectedItems->push_back ( item );
                }

            } else {
                try {
                    // The file exists so lets get the trustees and then examine their audited permissions.
                    string filePathStr = Common::BuildFilePath ( ( const string ) fp->first, ( const string ) fp->second );
                    StringSet* trusteeSIDs = this->GetTrusteesForWindowsObject ( SE_FILE_OBJECT, filePathStr, trusteeSID, true, resolveGroupBehavior, includeGroupBehavior );

                    if ( !trusteeSIDs->empty() ) {
                        for ( StringSet::iterator iterator = trusteeSIDs->begin(); iterator != trusteeSIDs->end(); iterator++ ) {
                            try {
                                Item* item = this->GetAuditedPermissions ( fp->first, fp->second, ( *iterator ) );

                                if ( item != NULL ) {
                                    collectedItems->push_back ( item );
                                }

                            } catch ( ProbeException ex ) {
                                Log::Message ( "ProbeException caught when collecting: " + object->GetId() + " " +  ex.GetErrorMessage() );

                            } catch ( Exception ex ) {
                                Log::Message ( "Exception caught when collecting: " + object->GetId() + " " +  ex.GetErrorMessage() );
                            }
                        }

                        trusteeSIDs->clear();
                        delete trusteeSIDs;
                        trusteeSIDs = NULL;

                    } else {
                        Log::Debug ( "No matching trustees found when getting audited permissions for object: " + object->GetId() );
                        StringSet* trusteeSIDs = new StringSet();

                        if ( this->ReportTrusteeDoesNotExist ( trusteeSID, trusteeSIDs, true ) ) {
                            for ( StringSet::iterator iterator = trusteeSIDs->begin(); iterator != trusteeSIDs->end(); iterator++ ) {
                                Item* item = this->CreateItem();
                                item->SetStatus ( OvalEnum::STATUS_DOES_NOT_EXIST );
                                item->AppendElement ( new ItemEntity ( "path", fp->first, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS ) );
                                item->AppendElement ( new ItemEntity ( "filename", fp->second, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS ) );
                                item->AppendElement ( new ItemEntity ( "trustee_sid", ( *iterator ), OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_DOES_NOT_EXIST ) );
                                collectedItems->push_back ( item );
                            }
                        }

                        trusteeSIDs->clear();
                        delete trusteeSIDs;
                        trusteeSIDs = NULL;
                    }

                } catch ( ProbeException ex ) {
                    Log::Message ( "ProbeException caught when collecting: " + object->GetId() + " " +  ex.GetErrorMessage() );

                } catch ( Exception ex ) {
                    Log::Message ( "Exception caught when collecting: " + object->GetId() + " " +  ex.GetErrorMessage() );

                } catch ( ... ) {
                    Log::Message ( "Unknown error when collecting " + object->GetId() );
                }
            }

            delete fp;
            fp = NULL;
        }

    } else {
        // If there are no file paths check to see if the code should report that the path does not exist.
        StringVector paths;

        if ( fileFinder.ReportPathDoesNotExist ( path, &paths ) ) {
            Item* item = NULL;

            for ( StringVector::iterator iterator = paths.begin(); iterator != paths.end(); iterator++ ) {
                item = this->CreateItem();
                item->SetStatus ( OvalEnum::STATUS_DOES_NOT_EXIST );
                item->AppendElement ( new ItemEntity ( "path", ( *iterator ), OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_DOES_NOT_EXIST ) );
                collectedItems->push_back ( item );
            }
        }
    }

    filePaths->clear();
    delete filePaths;
    filePaths = NULL;
    return collectedItems;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
Item* FileAuditedPermissions53Probe::CreateItem() {
    Item* item = new Item ( 0,
                            "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows",
                            "win-sc",
                            "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows windows-system-characteristics-schema.xsd",
                            OvalEnum::STATUS_ERROR,
                            "fileauditedpermissions_item" );
    return item;
}

Item* FileAuditedPermissions53Probe::GetAuditedPermissions ( string path, string fileName, string trusteeSID ) {
    Item* item = NULL;
    PSID pSid = NULL;
    PACCESS_MASK pSuccessfulAuditedRights = NULL;
    PACCESS_MASK pFailedAuditRights = NULL;
    // Build the path.
    string filePath = Common::BuildFilePath ( ( const string ) path, ( const string ) fileName );
    string baseErrMsg = "Error unable to get audited permissions for trustee: " + trusteeSID + " from dacl for file: " + filePath;

    try {
        // Verify that the file exists.
        if ( GetFileAttributes ( filePath.c_str() ) == INVALID_FILE_ATTRIBUTES ) {
            string systemErrMsg = WindowsCommon::GetErrorMessage ( GetLastError() );
            throw ProbeException ( baseErrMsg + " File " + filePath + " doesn't exist. " + systemErrMsg );
        }

        // Get the sid for the trustee name.
        pSid = WindowsCommon::GetSIDForTrusteeSID ( trusteeSID );
        // The file exists and trustee name seems good so we can create the new item now.
        item = this->CreateItem();
        item->SetStatus ( OvalEnum::STATUS_EXISTS );
        item->AppendElement ( new ItemEntity ( "path", path, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "filename", fileName, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "trustee_sid", trusteeSID, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS ) );
        // Build structure to hold the successful audited rights.
        pSuccessfulAuditedRights = reinterpret_cast<PACCESS_MASK> ( ::LocalAlloc ( LPTR, sizeof ( PACCESS_MASK ) + sizeof ( ACCESS_MASK ) ) );

        if ( pSuccessfulAuditedRights == NULL ) {
            if ( pSid != NULL ) {
                LocalFree ( pSid );
                pSid = NULL;
            }

            throw ProbeException ( baseErrMsg + " Out of memory! Unable to allocate memory for successful audited rights." );
        }

        // Build structure to hold the failed audit rights.
        pFailedAuditRights = reinterpret_cast<PACCESS_MASK> ( ::LocalAlloc ( LPTR, sizeof ( PACCESS_MASK ) + sizeof ( ACCESS_MASK ) ) );

        if ( pFailedAuditRights == NULL ) {
            if ( pSid != NULL ) {
                LocalFree ( pSid );
                pSid = NULL;
            }

            if ( pSuccessfulAuditedRights != NULL ) {
                LocalFree ( pSuccessfulAuditedRights );
                pSuccessfulAuditedRights = NULL;
            }

            throw ProbeException ( baseErrMsg + " Out of memory! Unable to allocate memory for failed audit rights." );
        }

        // Get the audited rights.
        Log::Debug ( "Getting audited permissions masks for file: " + path + " filename: " + fileName + " trustee_sid: " + trusteeSID );
        WindowsCommon::GetAuditedPermissionsForWindowsObject ( SE_FILE_OBJECT, pSid, &filePath, pSuccessfulAuditedRights, pFailedAuditRights );
        item->AppendElement ( new ItemEntity ( "standard_delete", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedRights ) & DELETE ), ( ( *pFailedAuditRights ) & DELETE ) ), OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "standard_read_control", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedRights ) & READ_CONTROL ), ( ( *pFailedAuditRights ) & READ_CONTROL ) ), OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "standard_write_dac", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedRights ) & WRITE_DAC ), ( ( *pFailedAuditRights ) & WRITE_DAC ) ), OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "standard_write_owner", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedRights ) & WRITE_OWNER ), ( ( *pFailedAuditRights ) & WRITE_OWNER ) ), OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "standard_synchronize", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedRights ) & SYNCHRONIZE ), ( ( *pFailedAuditRights ) & SYNCHRONIZE ) ), OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "access_system_security", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedRights ) & ACCESS_SYSTEM_SECURITY ), ( ( *pFailedAuditRights ) & ACCESS_SYSTEM_SECURITY ) ), OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "generic_read", ConvertPermissionsToStringValue ( ( ( ( *pSuccessfulAuditedRights ) & FILE_GENERIC_READ ) == FILE_GENERIC_READ ), ( ( ( *pFailedAuditRights ) & FILE_GENERIC_READ ) == FILE_GENERIC_READ ) ), OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "generic_write", ConvertPermissionsToStringValue ( ( ( ( *pSuccessfulAuditedRights ) & FILE_GENERIC_WRITE ) == FILE_GENERIC_WRITE ), ( ( ( *pFailedAuditRights ) & FILE_GENERIC_WRITE ) == FILE_GENERIC_WRITE ) ), OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "generic_execute", ConvertPermissionsToStringValue ( ( ( ( *pSuccessfulAuditedRights ) & FILE_GENERIC_EXECUTE ) == FILE_GENERIC_EXECUTE ), ( ( ( *pFailedAuditRights ) & FILE_GENERIC_EXECUTE ) == FILE_GENERIC_EXECUTE ) ), OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "generic_all", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedRights ) & FILE_READ_DATA ), ( ( *pFailedAuditRights ) & FILE_READ_DATA ) ), OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "file_read_data", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedRights ) & FILE_READ_DATA ), ( ( *pFailedAuditRights ) & FILE_READ_DATA ) ), OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "file_write_data", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedRights ) & FILE_WRITE_DATA ), ( ( *pFailedAuditRights ) & FILE_WRITE_DATA ) ), OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "file_append_data", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedRights ) & FILE_APPEND_DATA ), ( ( *pFailedAuditRights ) & FILE_APPEND_DATA ) ), OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "file_read_ea", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedRights ) & FILE_READ_EA ), ( ( *pFailedAuditRights ) & FILE_READ_EA ) ), OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "file_write_ea", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedRights ) & FILE_WRITE_EA ), ( ( *pFailedAuditRights ) & FILE_WRITE_EA ) ), OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "file_execute", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedRights ) & FILE_EXECUTE ), ( ( *pFailedAuditRights ) & FILE_EXECUTE ) ), OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "file_delete_child", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedRights ) & FILE_DELETE_CHILD ), ( ( *pFailedAuditRights ) & FILE_DELETE_CHILD ) ), OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "file_read_attributes", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedRights ) & FILE_READ_ATTRIBUTES ), ( ( *pFailedAuditRights ) & FILE_READ_ATTRIBUTES ) ), OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "file_write_attributes", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedRights ) & FILE_WRITE_ATTRIBUTES ), ( ( *pFailedAuditRights ) & FILE_WRITE_ATTRIBUTES ) ), OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS ) );

    } catch ( Exception ex ) {
        if ( item != NULL ) {
            item->SetStatus ( OvalEnum::STATUS_ERROR );
            item->AppendMessage ( new OvalMessage ( ex.GetErrorMessage(), OvalEnum::LEVEL_ERROR ) );

        } else {
            if ( pSuccessfulAuditedRights != NULL ) {
                LocalFree ( pSuccessfulAuditedRights );
                pSuccessfulAuditedRights = NULL;
            }

            if ( pFailedAuditRights != NULL ) {
                LocalFree ( pFailedAuditRights );
                pFailedAuditRights = NULL;
            }

            if ( pSid != NULL ) {
                LocalFree ( pSid );
                pSid = NULL;
            }

            throw ex;
        }
    }

    if ( pSuccessfulAuditedRights != NULL ) {
        LocalFree ( pSuccessfulAuditedRights );
        pSuccessfulAuditedRights = NULL;
    }

    if ( pFailedAuditRights != NULL ) {
        LocalFree ( pFailedAuditRights );
        pFailedAuditRights = NULL;
    }

    if ( pSid != NULL ) {
        LocalFree ( pSid );
        pSid = NULL;
    }

    return item;
}

string FileAuditedPermissions53Probe::ConvertPermissionsToStringValue ( ACCESS_MASK success , ACCESS_MASK failure ) {
    if ( success && failure ) return "AUDIT_SUCCESS_FAILURE";

    else if ( success && !failure ) return "AUDIT_SUCCESS";

    else if ( !success && failure ) return "AUDIT_FAILURE";

    else if ( !success && !failure ) return "AUDIT_NONE";

    else return "";
}