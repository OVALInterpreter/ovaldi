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

#include <memory>
#include <AutoCloser.h>
#include <PrivilegeGuard.h>
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
				Log::Info("Deprecated behavior found when collecting " + object->GetId() + " Found behavior: " + behavior->GetName() + " = " + behavior->GetValue());
            } else if ( behavior->GetName().compare ( "resolve_group" ) == 0 ) {
                if ( behavior->GetValue().compare ( "true" ) == 0 ) {
                    resolveGroupBehavior = true;
                }

                Log::Info ( "Deprecated behavior found when collecting " + object->GetId() + ". Found behavior: " + behavior->GetName() + " = " + behavior->GetValue() );

            } else if ( behavior->GetName() == "max_depth" || behavior->GetName() == "recurse_direction" || behavior->GetName() == "windows_view" ) {
                // Skip these they are supported in the file finder class.
            } else {
                Log::Info ( "Unsupported behavior found when collecting " + object->GetId() + ". Found behavior: " + behavior->GetName() + " = " + behavior->GetValue() );
            }
        }
    }

	FileFinder fileFinder(WindowsCommon::behavior2view(object->GetBehaviors()));
	StringPairVector* filePaths = NULL;
	
	{
		PrivilegeGuard pg(SE_BACKUP_NAME, false);
	
		if(filePath != NULL){
			filePaths = fileFinder.SearchFiles(filePath);	
		}else{
			filePaths = fileFinder.SearchFiles(path, fileName, object->GetBehaviors());
		}
	}

	if(filePaths != NULL && filePaths->size() > 0) {
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
						item->AppendElement(new ItemEntity("filepath", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));
                        item->AppendElement ( new ItemEntity ( "path", fp->first, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
                        item->AppendElement ( new ItemEntity ( "filename", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST, fileName->GetNil() ) );
						item->AppendElement(new ItemEntity("windows_view",
							(fileFinder.GetView() == BIT_32 ? "32_bit" : "64_bit")));
                        collectedItems->push_back ( item );
                    }

                } else {
                    item = this->CreateItem();
                    item->SetStatus ( OvalEnum::STATUS_EXISTS );
                    item->AppendElement ( new ItemEntity ( "path", fp->first, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
					item->AppendElement(new ItemEntity("windows_view",
						(fileFinder.GetView() == BIT_32 ? "32_bit" : "64_bit")));
                    collectedItems->push_back ( item );
                }

            } else {
                try {
                    // The file exists so lets get the trustees and then examine their audited permissions.
					string filePathStr = Common::BuildFilePath(fp->first, fp->second);
					HANDLE fileHandle;

					// The SE_SECURITY_NAME privilege is needed to read the SACL
					// (for ACCESS_SYSTEM_SECURITY access, specified below.)
					{
						PrivilegeGuard pg(SE_SECURITY_NAME);

						fileHandle = fileFinder.GetFileHandle(filePathStr,
							READ_CONTROL|ACCESS_SYSTEM_SECURITY, 
							fileName && fileName->GetNil());

						pg.disable();
					}

					if (!fileHandle || fileHandle == INVALID_HANDLE_VALUE) {
						throw ProbeException("Error: unable to get trustees "
							"for file: Error opening file: " + 
							WindowsCommon::GetErrorMessage(GetLastError()));
					}

					// need the guard, cause I know at least GetTrusteesForWindowsObject()
					// throws, so I feel safer with it.
					AutoCloser<HANDLE, BOOL(WINAPI&)(HANDLE)> handleGuard(fileHandle, 
						CloseHandle, "file "+filePathStr);

					StringSet trusteeSIDs = GetTrusteesForWindowsObject(
						SE_FILE_OBJECT, fileHandle, trusteeSID, true, 
						resolveGroupBehavior, includeGroupBehavior);

                    if ( !trusteeSIDs.empty() ) {
                        for ( StringSet::iterator iterator = trusteeSIDs.begin(); iterator != trusteeSIDs.end(); iterator++ ) {
                            try {
                                Item* item = this->GetAuditedPermissions ( fileHandle, fp->first, fp->second, ( *iterator ) );

                                if ( item != NULL ) {
									if (fileName->GetNil()) {
										auto_ptr<ItemEntityVector> fileNameVector(item->GetElementsByName("filename"));
										if (fileNameVector->size() > 0) {
											fileNameVector->at(0)->SetNil(true);
											fileNameVector->at(0)->SetStatus(OvalEnum::STATUS_NOT_COLLECTED);
										}
									}
									item->AppendElement(new ItemEntity("windows_view",
										(fileFinder.GetView() == BIT_32 ? "32_bit" : "64_bit")));
                                    collectedItems->push_back ( item );
                                }

                            } catch ( ProbeException ex ) {
                                Log::Message ( "ProbeException caught when collecting: " + object->GetId() + " " +  ex.GetErrorMessage() );

                            } catch ( Exception ex ) {
                                Log::Message ( "Exception caught when collecting: " + object->GetId() + " " +  ex.GetErrorMessage() );
                            }
                        }
                    } else {
                        Log::Debug ( "No matching trustees found when getting audited permissions for object: " + object->GetId() );
                        StringSet* trusteeSIDs = new StringSet();

                        if ( this->ReportTrusteeDoesNotExist ( trusteeSID, trusteeSIDs, true ) ) {
                            for ( StringSet::iterator iterator = trusteeSIDs->begin(); iterator != trusteeSIDs->end(); iterator++ ) {
                                Item* item = this->CreateItem();
                                item->SetStatus ( OvalEnum::STATUS_DOES_NOT_EXIST );
								item->AppendElement(new ItemEntity("filepath", filePathStr, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
                                item->AppendElement ( new ItemEntity ( "path", fp->first, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
								item->AppendElement (new ItemEntity ("filename", fp->second, OvalEnum::DATATYPE_STRING, ((fileName->GetNil())?OvalEnum::STATUS_NOT_COLLECTED : OvalEnum::STATUS_EXISTS), fileName->GetNil() ) );
                                item->AppendElement ( new ItemEntity ( "trustee_sid", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST ) );
								item->AppendElement(new ItemEntity("windows_view",
									(fileFinder.GetView() == BIT_32 ? "32_bit" : "64_bit")));
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
        if ( filePath != NULL ){
			StringVector fpaths;
			if (fileFinder.ReportFilePathDoesNotExist(filePath,&fpaths)){
				StringVector statusValues;
				Item* item = NULL;
				StringPair* fpComponents = NULL;

				// build path ObjectEntity to pass to ReportPathDoesNotExist to retrieve the status of the path value
				ObjectEntity* pathStatus = new ObjectEntity("path","",OvalEnum::DATATYPE_STRING,OvalEnum::OPERATION_EQUALS,NULL,OvalEnum::CHECK_ALL,false);
				// build filename ObjectEntity to pass to ReportFileNameDoesNotExist to retrieve the status of the filename value
				ObjectEntity* fileNameStatus = new ObjectEntity("filename","",OvalEnum::DATATYPE_STRING,OvalEnum::OPERATION_EQUALS,NULL,OvalEnum::CHECK_ALL,false);
				
				for(StringVector::iterator iterator = fpaths.begin(); iterator != fpaths.end(); iterator++) {
					item = this->CreateItem();
					item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
					fpComponents = Common::SplitFilePath(*iterator);
					pathStatus->SetValue(fpComponents->first);
					fileNameStatus->SetValue(fpComponents->second);
					item->AppendElement(new ItemEntity("filepath", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));
					item->AppendElement(new ItemEntity("path", "", OvalEnum::DATATYPE_STRING, (fileFinder.ReportPathDoesNotExist(pathStatus,&statusValues))?OvalEnum::STATUS_DOES_NOT_EXIST:OvalEnum::STATUS_EXISTS));
					item->AppendElement(new ItemEntity("filename", "", OvalEnum::DATATYPE_STRING, (fileFinder.ReportFileNameDoesNotExist(fpComponents->first,fileNameStatus,&statusValues))?OvalEnum::STATUS_DOES_NOT_EXIST:OvalEnum::STATUS_EXISTS));
					item->AppendElement(new ItemEntity("windows_view",
						(fileFinder.GetView() == BIT_32 ? "32_bit" : "64_bit")));
					collectedItems->push_back(item);
					
					if ( fpComponents != NULL ){
						delete fpComponents;
						fpComponents = NULL;
					}
				}

				if ( pathStatus != NULL ){
					delete pathStatus;
					pathStatus = NULL;
				}
				if ( fileNameStatus != NULL ){
					delete fileNameStatus;
					fileNameStatus = NULL;
				}
			}
		}else{
			StringVector paths;
			if(fileFinder.ReportPathDoesNotExist(path, &paths)) {
				Item* item = NULL;
				StringVector::iterator iterator;
				for(iterator = paths.begin(); iterator != paths.end(); iterator++) {
					item = this->CreateItem();
					item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
					item->AppendElement(new ItemEntity("path", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));
					item->AppendElement(new ItemEntity("windows_view",
						(fileFinder.GetView() == BIT_32 ? "32_bit" : "64_bit")));
					collectedItems->push_back(item);
				}
			}
		}
    }

    if ( filePaths != NULL ){
		delete filePaths;
		filePaths = NULL;
	}
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

Item* FileAuditedPermissions53Probe::GetAuditedPermissions ( HANDLE fileHandle, string path, string fileName, string trusteeSID ) {
    Item* item = NULL;
    PSID pSid = NULL;
    PACCESS_MASK pSuccessfulAuditedRights = NULL;
    PACCESS_MASK pFailedAuditRights = NULL;
    // Build the path.
    string filePath = Common::BuildFilePath ( path, fileName );
    string baseErrMsg = "Error unable to get audited permissions for trustee: " + trusteeSID + " from dacl for file: " + filePath;

    try {
        // Get the sid for the trustee name.
        pSid = WindowsCommon::GetSIDForTrusteeSID ( trusteeSID );
        // The file exists and trustee name seems good so we can create the new item now.
        item = this->CreateItem();
        item->SetStatus ( OvalEnum::STATUS_EXISTS );
		item->AppendElement(new ItemEntity("filepath", filePath, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
        item->AppendElement ( new ItemEntity ( "path", path, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "filename", fileName, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "trustee_sid", trusteeSID, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
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
        WindowsCommon::GetAuditedPermissionsForWindowsObject ( SE_FILE_OBJECT, pSid, fileHandle, pSuccessfulAuditedRights, pFailedAuditRights );
        item->AppendElement ( new ItemEntity ( "standard_delete", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedRights ) & DELETE ), ( ( *pFailedAuditRights ) & DELETE ) ), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "standard_read_control", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedRights ) & READ_CONTROL ), ( ( *pFailedAuditRights ) & READ_CONTROL ) ), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "standard_write_dac", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedRights ) & WRITE_DAC ), ( ( *pFailedAuditRights ) & WRITE_DAC ) ), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "standard_write_owner", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedRights ) & WRITE_OWNER ), ( ( *pFailedAuditRights ) & WRITE_OWNER ) ), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "standard_synchronize", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedRights ) & SYNCHRONIZE ), ( ( *pFailedAuditRights ) & SYNCHRONIZE ) ), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "access_system_security", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedRights ) & ACCESS_SYSTEM_SECURITY ), ( ( *pFailedAuditRights ) & ACCESS_SYSTEM_SECURITY ) ), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "generic_read", ConvertPermissionsToStringValue ( ( ( ( *pSuccessfulAuditedRights ) & FILE_GENERIC_READ ) == FILE_GENERIC_READ ), ( ( ( *pFailedAuditRights ) & FILE_GENERIC_READ ) == FILE_GENERIC_READ ) ), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "generic_write", ConvertPermissionsToStringValue ( ( ( ( *pSuccessfulAuditedRights ) & FILE_GENERIC_WRITE ) == FILE_GENERIC_WRITE ), ( ( ( *pFailedAuditRights ) & FILE_GENERIC_WRITE ) == FILE_GENERIC_WRITE ) ), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "generic_execute", ConvertPermissionsToStringValue ( ( ( ( *pSuccessfulAuditedRights ) & FILE_GENERIC_EXECUTE ) == FILE_GENERIC_EXECUTE ), ( ( ( *pFailedAuditRights ) & FILE_GENERIC_EXECUTE ) == FILE_GENERIC_EXECUTE ) ), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "generic_all", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedRights ) & FILE_READ_DATA ), ( ( *pFailedAuditRights ) & FILE_READ_DATA ) ), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "file_read_data", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedRights ) & FILE_READ_DATA ), ( ( *pFailedAuditRights ) & FILE_READ_DATA ) ), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "file_write_data", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedRights ) & FILE_WRITE_DATA ), ( ( *pFailedAuditRights ) & FILE_WRITE_DATA ) ), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "file_append_data", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedRights ) & FILE_APPEND_DATA ), ( ( *pFailedAuditRights ) & FILE_APPEND_DATA ) ), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "file_read_ea", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedRights ) & FILE_READ_EA ), ( ( *pFailedAuditRights ) & FILE_READ_EA ) ), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "file_write_ea", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedRights ) & FILE_WRITE_EA ), ( ( *pFailedAuditRights ) & FILE_WRITE_EA ) ), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "file_execute", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedRights ) & FILE_EXECUTE ), ( ( *pFailedAuditRights ) & FILE_EXECUTE ) ), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "file_delete_child", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedRights ) & FILE_DELETE_CHILD ), ( ( *pFailedAuditRights ) & FILE_DELETE_CHILD ) ), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "file_read_attributes", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedRights ) & FILE_READ_ATTRIBUTES ), ( ( *pFailedAuditRights ) & FILE_READ_ATTRIBUTES ) ), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "file_write_attributes", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedRights ) & FILE_WRITE_ATTRIBUTES ), ( ( *pFailedAuditRights ) & FILE_WRITE_ATTRIBUTES ) ), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );

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