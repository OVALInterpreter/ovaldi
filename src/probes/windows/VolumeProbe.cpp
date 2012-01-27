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

#include "VolumeProbe.h"

//****************************************************************************************//
//                              VolumeProbe Class                                         //
//****************************************************************************************//
VolumeProbe *VolumeProbe::instance = NULL;

VolumeProbe::VolumeProbe() {

}

VolumeProbe::~VolumeProbe() {
    instance = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

AbsProbe* VolumeProbe::Instance() {
    // Use lazy initialization for instance of the volume probe
    if ( instance == NULL ) {
        instance = new VolumeProbe();
    }

    return instance;
}

ItemVector* VolumeProbe::CollectItems ( Object *object ) {
    // Get the rootpath from the provided object
    ObjectEntity* rootpath = object->GetElementByName ( "rootpath" );
    ItemVector *collectedItems = new ItemVector();

	if ( rootpath->GetOperation() == OvalEnum::OPERATION_EQUALS ){
		StringVector volumes;
		//We are ignoring the flag for now
		/*OvalEnum::Flag flag =*/ rootpath->GetEntityValues(volumes);
		for(StringVector::iterator it = volumes.begin(); it != volumes.end(); it++){
			collectedItems->push_back(this->BuildVolumeObject((*it).c_str()));
		}
	}else{
		StringSet volumes;
		this->GetAllVolumes(volumes);
		ItemEntity* volumeItemEntity = new ItemEntity("rootpath","",OvalEnum::DATATYPE_STRING,true,OvalEnum::STATUS_EXISTS);
		for(StringSet::iterator it = volumes.begin(); it != volumes.end(); it++){
			volumeItemEntity->SetValue(*it);
			if ( rootpath->Analyze(volumeItemEntity) == OvalEnum::RESULT_TRUE ){
				collectedItems->push_back(this->BuildVolumeObject(*it));
			}
		}
		delete volumeItemEntity;
	}

    return collectedItems;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

Item* VolumeProbe::CreateItem() {
    Item* item = new Item ( 0 ,
                            "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows" ,
                            "win-sc" ,
                            "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows windows-system-characteristics-schema.xsd" ,
                            OvalEnum::STATUS_ERROR ,
                            "volume_item" );
    return item;
}

Item* VolumeProbe::BuildVolumeObject ( string rootPathStr ) {
    DWORD volumeSerialNumber;
    DWORD maximumComponentLength;
    DWORD fileSystemFlags;
    DWORD bufferSize = MAX_PATH + 1;
    char* volumeNameBuffer = ( char* ) malloc ( sizeof ( char* ) * ( bufferSize ) );
    char* fileSystemNameBuffer = ( char* ) malloc ( sizeof ( char* ) * ( bufferSize ) );

    if ( volumeNameBuffer != NULL && fileSystemNameBuffer != NULL ) {
		if ( ( GetVolumeInformationA ( rootPathStr.c_str() ,
                                      volumeNameBuffer ,
                                      bufferSize ,
                                      &volumeSerialNumber ,
                                      &maximumComponentLength ,
                                      &fileSystemFlags ,
                                      fileSystemNameBuffer ,
                                      bufferSize ) ) == 0 ) {
            if ( volumeNameBuffer != NULL ) {
                free ( volumeNameBuffer );
                volumeNameBuffer = NULL;
            }

            if ( fileSystemNameBuffer != NULL ) {
                free ( fileSystemNameBuffer );
                fileSystemNameBuffer = NULL;
            }

			DWORD error = GetLastError();
			Item* item = this->CreateItem();
			
			if ( error == ERROR_PATH_NOT_FOUND ){
				item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
				item->AppendElement(new ItemEntity ( "rootpath" , rootPathStr , OvalEnum::DATATYPE_STRING , true , OvalEnum::STATUS_DOES_NOT_EXIST));				
			}else{
				item->SetStatus(OvalEnum::STATUS_ERROR);
				item->AppendElement(new ItemEntity ( "rootpath" , rootPathStr , OvalEnum::DATATYPE_STRING , true , OvalEnum::STATUS_EXISTS ));
				item->AppendMessage(new OvalMessage("Error: GetVolumeInformation() was unable to retrieve all of the requested information regarding this volume. Microsoft System Error " + Common::ToString ( GetLastError() ) + " - " + WindowsCommon::GetErrorMessage ( GetLastError() ) ));
			}
			
			return item;

		} else {
            Item* item = this->CreateItem();
            bool fileCaseSensistiveSearch = false;
            bool fileCasePreservedNames = false;
            bool fileUnicodeOnDisk = false;
            bool filePersistentAcls = false;
            bool fileFileCompression = false;
            bool fileVolumeQuotas = false;
            bool fileSupportsSparseFiles = false;
            bool fileSupportsReparsePoints = false;
            bool fileSupportsRemoteStorage = false;
            bool fileVolumeIsCompressed = false;
            bool fileSupportsObjectIds = false;
            bool fileSupportsEncryption = false;
            bool fileNamedStreams = false;
            bool fileReadOnlyVolume = false;
			UINT driveType = 0;

            if ( ( fileSystemFlags & FILE_CASE_SENSITIVE_SEARCH ) == FILE_CASE_SENSITIVE_SEARCH ) {
                fileCaseSensistiveSearch = true;
            }

            if ( ( fileSystemFlags & FILE_CASE_PRESERVED_NAMES ) == FILE_CASE_PRESERVED_NAMES ) {
                fileCasePreservedNames = true;
            }

            if ( ( fileSystemFlags & FILE_UNICODE_ON_DISK ) == FILE_UNICODE_ON_DISK ) {
                fileUnicodeOnDisk = true;
            }

            if ( ( fileSystemFlags & FILE_PERSISTENT_ACLS ) == FILE_PERSISTENT_ACLS ) {
                filePersistentAcls = true;
            }

            if ( ( fileSystemFlags & FILE_FILE_COMPRESSION ) == FILE_FILE_COMPRESSION ) {
                fileFileCompression = true;
            }

            if ( ( fileSystemFlags & FILE_VOLUME_QUOTAS ) == FILE_VOLUME_QUOTAS ) {
                fileVolumeQuotas = true;
            }

            if ( ( fileSystemFlags & FILE_SUPPORTS_SPARSE_FILES ) == FILE_SUPPORTS_SPARSE_FILES ) {
                fileSupportsSparseFiles = true;
            }

            if ( ( fileSystemFlags & FILE_SUPPORTS_REPARSE_POINTS ) == FILE_SUPPORTS_REPARSE_POINTS ) {
                fileSupportsReparsePoints = true;
            }

            if ( ( fileSystemFlags & FILE_SUPPORTS_REMOTE_STORAGE ) == FILE_SUPPORTS_REMOTE_STORAGE ) {
                fileSupportsRemoteStorage = true;
            }

            if ( ( fileSystemFlags & FILE_VOLUME_IS_COMPRESSED ) == FILE_VOLUME_IS_COMPRESSED ) {
                fileVolumeIsCompressed = true;
            }

            if ( ( fileSystemFlags & FILE_SUPPORTS_OBJECT_IDS ) == FILE_SUPPORTS_OBJECT_IDS ) {
                fileSupportsObjectIds = true;
            }

            if ( ( fileSystemFlags & FILE_SUPPORTS_ENCRYPTION ) == FILE_SUPPORTS_ENCRYPTION ) {
                fileSupportsEncryption = true;
            }

            if ( ( fileSystemFlags & FILE_NAMED_STREAMS ) == FILE_NAMED_STREAMS ) {
                fileNamedStreams = true;
            }

            if ( ( fileSystemFlags & FILE_READ_ONLY_VOLUME ) == FILE_READ_ONLY_VOLUME ) {
                fileReadOnlyVolume = true;
            }
			
			driveType = GetDriveType(rootPathStr.c_str());

            item->SetStatus ( OvalEnum::STATUS_EXISTS );
			( rootPathStr.compare("") == 0 ) ? item->AppendElement ( new ItemEntity ( "rootpath" , "" , OvalEnum::DATATYPE_STRING , true , OvalEnum::STATUS_ERROR )  ) : item->AppendElement ( new ItemEntity ( "rootpath" , rootPathStr, OvalEnum::DATATYPE_STRING , true , OvalEnum::STATUS_EXISTS )  );
            ( fileSystemNameBuffer == NULL ) ? item->AppendElement ( new ItemEntity ( "file_system" , "" , OvalEnum::DATATYPE_STRING , false , OvalEnum::STATUS_ERROR )  ) : item->AppendElement ( new ItemEntity ( "file_system" , fileSystemNameBuffer , OvalEnum::DATATYPE_STRING , false , OvalEnum::STATUS_EXISTS )  );
            ( volumeNameBuffer == NULL ) ? item->AppendElement ( new ItemEntity ( "name" , "" , OvalEnum::DATATYPE_STRING , false , OvalEnum::STATUS_ERROR )  ) : item->AppendElement ( new ItemEntity ( "name" , volumeNameBuffer , OvalEnum::DATATYPE_STRING , false , OvalEnum::STATUS_EXISTS )  );
			item->AppendElement ( new ItemEntity ( "drive_type" , this->DriveTypeToString ( driveType ) , OvalEnum::DATATYPE_STRING , false , OvalEnum::STATUS_EXISTS ) );
            item->AppendElement ( new ItemEntity ( "volume_max_component_length" , Common::ToString ( maximumComponentLength ) , OvalEnum::DATATYPE_INTEGER , false , OvalEnum::STATUS_EXISTS ) );
            item->AppendElement ( new ItemEntity ( "serial_number" , Common::ToString ( volumeSerialNumber ) , OvalEnum::DATATYPE_INTEGER , false , OvalEnum::STATUS_EXISTS ) );
            item->AppendElement ( new ItemEntity ( "file_case_sensitive_search" , Common::ToString ( fileCaseSensistiveSearch ) , OvalEnum::DATATYPE_BOOLEAN , false , OvalEnum::STATUS_EXISTS ) );
            item->AppendElement ( new ItemEntity ( "file_case_preserved_names" , Common::ToString ( fileCasePreservedNames ) , OvalEnum::DATATYPE_BOOLEAN , false , OvalEnum::STATUS_EXISTS ) );
            item->AppendElement ( new ItemEntity ( "file_unicode_on_disk" , Common::ToString ( fileUnicodeOnDisk ) , OvalEnum::DATATYPE_BOOLEAN , false , OvalEnum::STATUS_EXISTS ) );
            item->AppendElement ( new ItemEntity ( "file_persistent_acls" , Common::ToString ( filePersistentAcls ) , OvalEnum::DATATYPE_BOOLEAN , false , OvalEnum::STATUS_EXISTS ) );
            item->AppendElement ( new ItemEntity ( "file_file_compression" , Common::ToString ( fileFileCompression ) , OvalEnum::DATATYPE_BOOLEAN , false , OvalEnum::STATUS_EXISTS ) );
            item->AppendElement ( new ItemEntity ( "file_volume_quotas" , Common::ToString ( fileVolumeQuotas ) , OvalEnum::DATATYPE_BOOLEAN , false , OvalEnum::STATUS_EXISTS ) );
            item->AppendElement ( new ItemEntity ( "file_supports_sparse_files" , Common::ToString ( fileSupportsSparseFiles ) , OvalEnum::DATATYPE_BOOLEAN , false , OvalEnum::STATUS_EXISTS ) );
            item->AppendElement ( new ItemEntity ( "file_supports_reparse_points" , Common::ToString ( fileSupportsReparsePoints ) , OvalEnum::DATATYPE_BOOLEAN , false , OvalEnum::STATUS_EXISTS ) );
            item->AppendElement ( new ItemEntity ( "file_supports_remote_storage" , Common::ToString ( fileSupportsRemoteStorage ) , OvalEnum::DATATYPE_BOOLEAN , false , OvalEnum::STATUS_EXISTS ) );
            item->AppendElement ( new ItemEntity ( "file_volume_is_compressed" , Common::ToString ( fileVolumeIsCompressed ) , OvalEnum::DATATYPE_BOOLEAN , false , OvalEnum::STATUS_EXISTS ) );
            item->AppendElement ( new ItemEntity ( "file_supports_object_ids" , Common::ToString ( fileSupportsObjectIds ) , OvalEnum::DATATYPE_BOOLEAN , false , OvalEnum::STATUS_EXISTS ) );
            item->AppendElement ( new ItemEntity ( "file_supports_encryption" , Common::ToString ( fileSupportsEncryption ) , OvalEnum::DATATYPE_BOOLEAN , false , OvalEnum::STATUS_EXISTS ) );
            item->AppendElement ( new ItemEntity ( "file_named_streams" , Common::ToString ( fileNamedStreams ) , OvalEnum::DATATYPE_BOOLEAN , false , OvalEnum::STATUS_EXISTS ) );
            item->AppendElement ( new ItemEntity ( "file_read_only_volume" , Common::ToString ( fileReadOnlyVolume ) , OvalEnum::DATATYPE_BOOLEAN , false , OvalEnum::STATUS_EXISTS ) );

            if ( volumeNameBuffer != NULL ) {
                free ( volumeNameBuffer );
                volumeNameBuffer = NULL;
            }

            if ( fileSystemNameBuffer != NULL ) {
                free ( fileSystemNameBuffer );
                fileSystemNameBuffer = NULL;
            }

            return item;
        }

    } else {
        if ( volumeNameBuffer != NULL ) {
            free ( volumeNameBuffer );
            volumeNameBuffer = NULL;
        }

        if ( fileSystemNameBuffer != NULL ) {
            free ( fileSystemNameBuffer );
            fileSystemNameBuffer = NULL;
        }

        return NULL;
    }
}

string VolumeProbe::GetPathName ( string volumeNameStr ) {
    DWORD  CharCount = MAX_PATH + 1;
    char* path = ( char* ) malloc ( sizeof ( char* ) * ( CharCount ) );
    string pathStr = "";

    if ( path ) {
		GetVolumePathNamesForVolumeNameA ( volumeNameStr.c_str() , path , CharCount , &CharCount );

        if (  GetLastError() == ERROR_INVALID_NAME ) {
            return pathStr;
        }

        pathStr = path;

        if ( path != NULL ) {
            free ( path );
            path = NULL;
        }
    }

    return pathStr;
}


void VolumeProbe::GetAllVolumes(StringSet &volumes) {
    bool isTrue;
    HANDLE handle;
    DWORD length = MAX_PATH + 1;
    char* volumeName = ( char* ) malloc ( sizeof ( char* ) * ( length ) );

    if ( volumeName != NULL ) {
        if ( ( handle = FindFirstVolumeA ( volumeName , length ) ) == INVALID_HANDLE_VALUE ) {
            throw ProbeException ( "Error: There were no volumes found on the system while using FindFirstVolume(). Microsoft System Error " + Common::ToString ( GetLastError() ) + " - " + WindowsCommon::GetErrorMessage ( GetLastError() ) );
        }

		volumes.insert(this->GetPathName(volumeName));
        isTrue = true;

        while ( isTrue ) {
            if ( FindNextVolumeA ( handle , volumeName , length ) == 0 ) {
                if ( GetLastError() != ERROR_NO_MORE_FILES ) {
              
	                if ( volumeName != NULL ) {
                        free ( volumeName );
                        volumeName = NULL;
                    }
					
					if ( FindVolumeClose ( handle ) == 0 ) {
						throw ProbeException ( "Error: FindVolumeClose() was unable to close the search handle opened by FindFirstVolume(). Microsoft System Error " + Common::ToString ( GetLastError() ) + " - " + WindowsCommon::GetErrorMessage ( GetLastError() ) );
					}

                    throw ProbeException ( "Error: There was an error retrieving the next volume while using FindNextVolume(). Microsoft System Error " + Common::ToString ( GetLastError() ) + " - " + WindowsCommon::GetErrorMessage ( GetLastError() ) );

                } else {
                    isTrue = false;
                }

            } else {
				volumes.insert(this->GetPathName(volumeName));
            }
        }

        if ( volumeName != NULL ) {
            free ( volumeName );
            volumeName = NULL;
        }

		if ( FindVolumeClose ( handle ) == 0 ) {
			throw ProbeException ( "Error: FindVolumeClose() was unable to close the search handle opened by FindFirstVolume(). Microsoft System Error " + Common::ToString ( GetLastError() ) + " - " + WindowsCommon::GetErrorMessage ( GetLastError() ) );
		}
    }
}

std::string VolumeProbe::DriveTypeToString(UINT driveType) {
	switch(driveType) {
		case (DRIVE_UNKNOWN):
			return "DRIVE_UNKNOWN";
			break;
		case (DRIVE_NO_ROOT_DIR):
			return "DRIVE_NO_ROOT_DIR";
			break;
		case (DRIVE_REMOVABLE):
			return "DRIVE_REMOVABLE";
			break;
		case (DRIVE_FIXED):
			return "DRIVE_FIXED";
			break;
		case (DRIVE_REMOTE):
			return "DRIVE_REMOTE";
			break;
		case (DRIVE_CDROM):
			return "DRIVE_CDROM";
			break;
		case (DRIVE_RAMDISK):
			return "DRIVE_RAMDISK";
			break;
		default:
			throw Exception("VolumeProbe::DriveTypeToString - Error unsupported driveType value.");
			break;
	}
}