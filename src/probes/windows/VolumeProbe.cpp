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

#include "VolumeProbe.h"

//****************************************************************************************//
//                              VolumeProbe Class                                         //
//****************************************************************************************//
VolumeProbe *VolumeProbe::instance = NULL;

VolumeProbe::VolumeProbe() {
    volumes = NULL;
}

VolumeProbe::~VolumeProbe() {
    instance = NULL;
    VolumeProbe::DeleteVolumes();
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

    // Check datatypes - only allow the string datatype
    if ( rootpath->GetDatatype() != OvalEnum::DATATYPE_STRING ) {
        throw ProbeException ( "Error: invalid data type specified on rootpath. Found: " + OvalEnum::DatatypeToString ( rootpath->GetDatatype() ) );
    }

    // Check operation - only allow equals, not equals, and pattern match
    if ( rootpath->GetOperation() != OvalEnum::OPERATION_EQUALS && rootpath->GetOperation() != OvalEnum::OPERATION_PATTERN_MATCH && rootpath->GetOperation() != OvalEnum::OPERATION_NOT_EQUAL ) {
        throw ProbeException ( "Error: invalid operation specified on rootpath. Found: " + OvalEnum::OperationToString ( rootpath->GetOperation() ) );
    }

    // Use lazy initialization to gather all the shared resources on the system
    if ( volumes == NULL ) {
        VolumeProbe::GetAllVolumes();
    }

    ItemVector *collectedItems = new ItemVector();

    if ( rootpath->GetVarRef() == NULL ) {
        if ( rootpath->GetOperation() == OvalEnum::OPERATION_EQUALS ) {
            Item* aVolume = VolumeProbe::GetVolume ( rootpath->GetValue() );

            if ( aVolume != NULL ) {
                Item* temp = new Item ( *aVolume );
                collectedItems->push_back ( temp );

            } else {
                aVolume = this->CreateItem();
                aVolume->SetStatus ( OvalEnum::STATUS_DOES_NOT_EXIST );
                aVolume->AppendElement ( new ItemEntity ( "rootpath" , rootpath->GetValue() , OvalEnum::DATATYPE_STRING , true , OvalEnum::STATUS_DOES_NOT_EXIST ) );
                collectedItems->push_back ( aVolume );
            }

        } else {
            ItemVector::iterator iterator1;
            ItemEntityVector::iterator iterator2;

            // Loop through all volumes if they are a regex match on netname create item an return it
            for ( iterator1 = VolumeProbe::volumes->begin() ; iterator1 != VolumeProbe::volumes->end() ; iterator1++ ) {
                ItemEntityVector* ivec = ( *iterator1 )->GetElementsByName ( "rootpath" );

                for ( iterator2 = ivec->begin() ; iterator2 != ivec->end() ; iterator2++ ) {
                    if ( rootpath->GetOperation() == OvalEnum::OPERATION_NOT_EQUAL ) {
                        if ( rootpath->GetValue().compare ( ( *iterator2 )->GetValue() ) != 0 ) {
                            Item* temp = new Item ( **iterator1 );
                            collectedItems->push_back ( temp );
                        }

                    } else {
                        if ( this->myMatcher->IsMatch ( rootpath->GetValue().c_str() , ( *iterator2 )->GetValue().c_str() ) ) {
                            Item* temp = new Item ( **iterator1 );
                            collectedItems->push_back ( temp );
                        }
                    }
                }
            }
        }

    } else {
        // Loop through all of the volumes on the system
        // Only keep the volumes that match operation, value, and var check
        ItemVector::iterator it1;
        ItemEntityVector::iterator it2;

        for ( it1 = VolumeProbe::volumes->begin() ; it1 != VolumeProbe::volumes->end() ; it1++ ) {
            ItemEntityVector* ivec = ( *it1 )->GetElementsByName ( "rootpath" );

            for ( it2 = ivec->begin() ; it2 != ivec->end() ; it2++ ) {
                if ( rootpath->Analyze ( *it2 ) == OvalEnum::RESULT_TRUE ) {
                    Item* temp = new Item ( **it1 );
                    collectedItems->push_back ( temp );
                }
            }
        }
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

Item* VolumeProbe::GetVolume ( string rootPathStr ) {
    ItemVector::iterator iterator1;
    ItemEntityVector::iterator iterator2;

    for ( iterator1 = VolumeProbe::volumes->begin() ; iterator1 != VolumeProbe::volumes->end() ; iterator1++ ) {
        ItemEntityVector* ivec = ( *iterator1 )->GetElementsByName ( "rootpath" );

        for ( iterator2 = ivec->begin() ; iterator2 != ivec->end() ; iterator2++ ) {
            if ( ( *iterator2 )->GetValue().compare ( rootPathStr ) == 0 ) {
                return ( *iterator1 );
            }
        }
    }

    return NULL;
}

Item* VolumeProbe::BuildVolumeObject ( LPTSTR rootPathStr ) {
    DWORD volumeSerialNumber;
    DWORD maximumComponentLength;
    DWORD fileSystemFlags;
    DWORD bufferSize = MAX_PATH + 1;
    LPTSTR volumeNameBuffer = ( LPTSTR ) malloc ( sizeof ( _TCHAR ) * ( bufferSize ) );
    LPTSTR fileSystemNameBuffer = ( LPTSTR ) malloc ( sizeof ( _TCHAR ) * ( bufferSize ) );

    if ( volumeNameBuffer != NULL && fileSystemNameBuffer != NULL ) {
        if ( ( GetVolumeInformation ( rootPathStr ,
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

            VolumeProbe::DeleteVolumes();
            throw ProbeException ( "Error: GetVolumeInformation() was unable to retrieve all of the requested information about a particular volume. Microsoft System Error " + Common::ToString ( GetLastError() ) + " - " + WindowsCommon::GetErrorMessage ( GetLastError() ) );

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

            item->SetStatus ( OvalEnum::STATUS_EXISTS );
            ( rootPathStr == NULL ) ? item->AppendElement ( new ItemEntity ( "rootpath" , "" , OvalEnum::DATATYPE_STRING , true , OvalEnum::STATUS_ERROR )  ) : item->AppendElement ( new ItemEntity ( "rootpath" , VolumeProbe::GetPathName ( rootPathStr ) , OvalEnum::DATATYPE_STRING , true , OvalEnum::STATUS_EXISTS )  );
            ( fileSystemNameBuffer == NULL ) ? item->AppendElement ( new ItemEntity ( "file_system" , "" , OvalEnum::DATATYPE_STRING , false , OvalEnum::STATUS_ERROR )  ) : item->AppendElement ( new ItemEntity ( "file_system" , fileSystemNameBuffer , OvalEnum::DATATYPE_STRING , false , OvalEnum::STATUS_EXISTS )  );
            ( volumeNameBuffer == NULL ) ? item->AppendElement ( new ItemEntity ( "name" , "" , OvalEnum::DATATYPE_STRING , false , OvalEnum::STATUS_ERROR )  ) : item->AppendElement ( new ItemEntity ( "name" , volumeNameBuffer , OvalEnum::DATATYPE_STRING , false , OvalEnum::STATUS_EXISTS )  );
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

string VolumeProbe::GetPathName ( LPTSTR volumeNameStr ) {
    DWORD  CharCount = MAX_PATH + 1;
    LPTSTR path = ( LPTSTR ) malloc ( sizeof ( _TCHAR ) * ( CharCount ) );
    string pathStr = "";

    if ( path ) {
        GetVolumePathNamesForVolumeName ( volumeNameStr , path , CharCount , &CharCount );

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


void VolumeProbe::GetAllVolumes() {
    Item * aVolume;
    bool isTrue;
    HANDLE handle;
    DWORD length = MAX_PATH + 1;
    LPTSTR volumeName = ( LPTSTR ) malloc ( sizeof ( _TCHAR ) * ( length ) );

    if ( volumeName != NULL ) {
        if ( ( handle = FindFirstVolume ( volumeName , length ) ) == INVALID_HANDLE_VALUE ) {
            throw ProbeException ( "Error: There were no volumes found on the system while using FindFirstVolume(). Microsoft System Error " + Common::ToString ( GetLastError() ) + " - " + WindowsCommon::GetErrorMessage ( GetLastError() ) );
        }

        VolumeProbe::volumes = new ItemVector();

        if ( ( aVolume = VolumeProbe::BuildVolumeObject ( volumeName ) ) != NULL ) {
            VolumeProbe::volumes->push_back ( aVolume );
        }

        isTrue = true;

        while ( isTrue ) {
            if ( FindNextVolume ( handle , volumeName , length ) == 0 ) {
                FindVolumeClose ( handle );

                if ( GetLastError() != ERROR_NO_MORE_FILES ) {
                    VolumeProbe::DeleteVolumes();

                    if ( volumeName != NULL ) {
                        free ( volumeName );
                        volumeName = NULL;
                    }

                    throw ProbeException ( "Error: There was an error retrieving the next volume while using FindNextVolume(). Microsoft System Error " + Common::ToString ( GetLastError() ) + " - " + WindowsCommon::GetErrorMessage ( GetLastError() ) );

                } else {
                    isTrue = false;
                }

            } else {
                if ( ( aVolume = VolumeProbe::BuildVolumeObject ( volumeName ) ) != NULL ) {
                    VolumeProbe::volumes->push_back ( aVolume );
                }
            }
        }

        if ( volumeName != NULL ) {
            free ( volumeName );
            volumeName = NULL;
        }

        if ( FindVolumeClose ( handle ) == 0 ) {
            VolumeProbe::DeleteVolumes();
            throw ProbeException ( "Error: FindVolumeClose() was unable to close the search handle opened by FindFirstVolume(). Microsoft System Error " + Common::ToString ( GetLastError() ) + " - " + WindowsCommon::GetErrorMessage ( GetLastError() ) );
        }
    }
}

void VolumeProbe::DeleteVolumes() {
    if ( VolumeProbe::volumes != NULL ) {
        ItemVector::iterator it;

        for ( it = VolumeProbe::volumes->begin() ; it != VolumeProbe::volumes->end() ; it++ ) {
            if ( ( *it ) != NULL ) {
                delete ( *it );
                ( *it ) = NULL;
            }
        }

        delete VolumeProbe::volumes;
        VolumeProbe::volumes = NULL;
    }

    return;
}