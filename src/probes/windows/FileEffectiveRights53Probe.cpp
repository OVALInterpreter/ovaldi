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

#include "FileEffectiveRights53Probe.h"


//****************************************************************************************//
//								FileEffectiveRights53Probe Class						  //	
//****************************************************************************************//
FileEffectiveRights53Probe* FileEffectiveRights53Probe::instance = NULL;

FileEffectiveRights53Probe::FileEffectiveRights53Probe() {

}

FileEffectiveRights53Probe::~FileEffectiveRights53Probe() {
	instance = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* FileEffectiveRights53Probe::Instance() {

	// Use lazy initialization
	if(instance == NULL) 
		instance = new FileEffectiveRights53Probe();

	return instance;	
}

ItemVector* FileEffectiveRights53Probe::CollectItems(Object* object) {
	// -----------------------------------------------------------------------
	//
	//  ABSTRACT
	//
	//	Get all the files on the system that match the pattern reusing the FileFinder.
	//	This probe operates a bit differently than the others. After locating matching
	//	files as the other file related probes do the trustee name is handled as follows:
	//
	//	if not using Variables 
	//	 - if operation == equals
	//		- call GetEffectiveRights method
	//
	//	 - operation == not equal || operation == pattern match
	//		- Get all SIDS on the system.
	//		- Get the set of matching SIDS
	//
	//	- if using variables
	//		- Get all SIDS on the system.
	//		- Get the set of matching SIDS
	//		- call GetEffectiveRights method
	//
	// -----------------------------------------------------------------------

	// get the path and file name
	ObjectEntity* path = object->GetElementByName("path");
	ObjectEntity* fileName = object->GetElementByName("filename");
    ObjectEntity* filePath = object->GetElementByName("filepath");

	ObjectEntity* trusteeSID = object->GetElementByName("trustee_sid");

	// check trustee datatypes - only allow string
	if(trusteeSID->GetDatatype() != OvalEnum::DATATYPE_STRING) {
		throw ProbeException("Error: invalid data type specified on trustee_name. Found: " + OvalEnum::DatatypeToString(trusteeSID->GetDatatype()));
	}

	// check trustee operation - only allow  equals, not equals and pattern match
	if(trusteeSID->GetOperation() != OvalEnum::OPERATION_EQUALS 
		&& trusteeSID->GetOperation() != OvalEnum::OPERATION_PATTERN_MATCH 
		&& trusteeSID->GetOperation() != OvalEnum::OPERATION_NOT_EQUAL) {
		throw ProbeException("Error: invalid operation specified on trustee_sid. Found: " + OvalEnum::OperationToString(trusteeSID->GetOperation()));
	}

	ItemVector *collectedItems = new ItemVector();

	// support behaviors - init with defaults.
	bool includeGroupBehavior = true;
	bool resolveGroupBehavior = false;
	if(object->GetBehaviors()->size() != 0) {
		BehaviorVector* behaviors = object->GetBehaviors();
		BehaviorVector::iterator iterator;
		for(iterator = behaviors->begin(); iterator != behaviors->end(); iterator++) {
			Behavior* behavior = (*iterator);
            if(behavior->GetName().compare("include_group") == 0)  {
                if(behavior->GetValue().compare("false") == 0) {
				    includeGroupBehavior = false;
                }
            } else if(behavior->GetName().compare("resolve_group") == 0) {
                if(behavior->GetValue().compare("true") == 0) {
				    resolveGroupBehavior = true;
                }
                Log::Info("Deprecated behavior found when collecting " + object->GetId() + " Found behavior: " + behavior->GetName() + " = " + behavior->GetValue());
            } else if(behavior->GetName().compare("max_depth") == 0 || behavior->GetName().compare("recurse_direction") == 0) {
                // skip these they are supported in the file finder class.

			} else {
                Log::Info("Unsupported behavior found when collecting " + object->GetId() + " Found behavior: " + behavior->GetName() + " = " + behavior->GetValue());
			}
		}		
	}

	FileFinder fileFinder;
	StringPairVector* filePaths = NULL;
	
	if ( WindowsCommon::EnablePrivilege(SE_BACKUP_NAME) == 0 ){
		Log::Message("Error: Unable to enable SE_BACKUP_NAME privilege.");
	}
	
	if(filePath != NULL){
		if ( (object->GetBehaviors())->size() > 0 ){
			throw ProbeException("Error: Behaviors do not apply to the filepath entity and cannot be used.");
		}
		filePaths = fileFinder.SearchFiles(filePath);	
	}else{
		filePaths = fileFinder.SearchFiles(path, fileName, object->GetBehaviors());
	}

	if ( WindowsCommon::DisableAllPrivileges() == 0 ){
		Log::Message("Error: Unable to disable all privileges.");
	}

	if(filePaths != NULL && filePaths->size() > 0) {
		// Loop through all file paths
		StringPairVector::iterator iterator;
		for(iterator = filePaths->begin(); iterator != filePaths->end(); iterator++) {

			StringPair* fp = (*iterator);

			// if there is no file name and the fileName obj entity is not set to nil
			if(fp->second.compare("") == 0 && !fileName->GetNil()) {

				Item* item = NULL;

				// check if the code should report that the filename does not exist.
				StringVector fileNames;
				if(fileFinder.ReportFileNameDoesNotExist(fp->first, fileName, &fileNames)) {
					StringVector::iterator iterator;
					for(iterator = fileNames.begin(); iterator != fileNames.end(); iterator++) {

						item = this->CreateItem();
						item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
						item->AppendElement(new ItemEntity("filepath", Common::BuildFilePath(fp->first, *iterator), OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_DOES_NOT_EXIST));
						item->AppendElement(new ItemEntity("path", fp->first, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
						item->AppendElement(new ItemEntity("filename", (*iterator), OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_DOES_NOT_EXIST, fileName->GetNil()));
						collectedItems->push_back(item);
					}
					
				} else {

					item = this->CreateItem();
					item->SetStatus(OvalEnum::STATUS_EXISTS);
					item->AppendElement(new ItemEntity("path", fp->first, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
					collectedItems->push_back(item);

				}

			} else {

				try {

					//
					// The file exists so lets get the sids to then examine effective rights
					//
					string filePathStr = Common::BuildFilePath((const string)fp->first, (const string)fp->second);
                    StringSet* trusteeSIDs = this->GetTrusteesForWindowsObject(SE_FILE_OBJECT, filePathStr, trusteeSID, true, resolveGroupBehavior, includeGroupBehavior);
					if(!trusteeSIDs->empty()) {
						StringSet::iterator iterator;
						for(iterator = trusteeSIDs->begin(); iterator != trusteeSIDs->end(); iterator++) {
							try {
								Item* item = this->GetEffectiveRights(fp->first, fp->second, (*iterator));
								if(item != NULL) {
									if (fileName->GetNil()) {
										ItemEntityVector* fileNameVector = item->GetElementsByName("filename");
										if (fileNameVector->size() > 0) {
											fileNameVector->at(0)->SetNil(true);
										}
									}
									collectedItems->push_back(item);
								}
							} catch (ProbeException ex) {
								Log::Message("ProbeException caught when collecting: " + object->GetId() + " " +  ex.GetErrorMessage());
							} catch (Exception ex) {
								Log::Message("Exception caught when collecting: " + object->GetId() + " " +  ex.GetErrorMessage());
							}
						}
						delete trusteeSIDs;

					} else {

						Log::Debug("No matching SIDs found when getting effective rights for object: " + object->GetId());

						StringSet* trusteeSIDs = new StringSet();
						if(this->ReportTrusteeDoesNotExist(trusteeSID, trusteeSIDs, true)) {

							StringSet::iterator iterator;
							for(iterator = trusteeSIDs->begin(); iterator != trusteeSIDs->end(); iterator++) {

								Item* item = this->CreateItem();
								item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
								item->AppendElement(new ItemEntity("filepath", filePathStr, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
								item->AppendElement(new ItemEntity("path", fp->first, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
								item->AppendElement(new ItemEntity("filename", fp->second, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS, (fileName != NULL && fileName->GetNil())));
								item->AppendElement(new ItemEntity("trustee_sid", (*iterator), OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_DOES_NOT_EXIST));
								collectedItems->push_back(item);
							}
						}
					}

				} catch (ProbeException ex) {
					Log::Message("ProbeException caught when collecting: " + object->GetId() + " " +  ex.GetErrorMessage());
				} catch (Exception ex) {
					Log::Message("Exception caught when collecting: " + object->GetId() + " " +  ex.GetErrorMessage());
				} catch (...) {
					Log::Message("Unknown error when collecting " + object->GetId());
				}
			}

			delete fp;
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
					item->AppendElement(new ItemEntity("filepath", (*iterator), OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_DOES_NOT_EXIST));
					item->AppendElement(new ItemEntity("path", fpComponents->first, OvalEnum::DATATYPE_STRING, true, (fileFinder.ReportPathDoesNotExist(pathStatus,&statusValues))?OvalEnum::STATUS_DOES_NOT_EXIST:OvalEnum::STATUS_EXISTS));
					item->AppendElement(new ItemEntity("filename", fpComponents->second, OvalEnum::DATATYPE_STRING, true, (fileFinder.ReportFileNameDoesNotExist(fpComponents->first,fileNameStatus,&statusValues))?OvalEnum::STATUS_DOES_NOT_EXIST:OvalEnum::STATUS_EXISTS));
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
					item->AppendElement(new ItemEntity("path", (*iterator), OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_DOES_NOT_EXIST));
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
Item* FileEffectiveRights53Probe::CreateItem() {

	Item* item = new Item(0, 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows", 
						"win-sc", 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows windows-system-characteristics-schema.xsd", 
						OvalEnum::STATUS_ERROR, 
						"fileeffectiverights_item");

	return item;
}

Item* FileEffectiveRights53Probe::GetEffectiveRights(string path, string fileName, string trusteeSID) {
	
	Log::Debug("Collecting effective rights for: " + path + " filename: " + fileName + " trustee_sid: " + trusteeSID);

	Item* item = NULL;
	PSID pSid = NULL;
	PACCESS_MASK pAccessRights = NULL;

	// build the path
    string filePath = Common::BuildFilePath((const string)path, (const string)fileName);

	string baseErrMsg = "Error unable to get effective rights for: " + path + " filename: " + fileName + " trustee_sid: " + trusteeSID;


	try {

		// verify that the file exists.
		if(GetFileAttributes(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
			string systemErrMsg = WindowsCommon::GetErrorMessage(GetLastError());
			throw ProbeException(baseErrMsg + " File " + filePath + " doesn't exist. " + systemErrMsg);
		}


		// Get the sid for the trustee name
		pSid = WindowsCommon::GetSIDForTrusteeSID(trusteeSID);

		
		// the file exists and the trustee_sid looks valid so we can create the new item now.
		Log::Debug("Creating item to hold file effective rights for: " + path + " filename: " + fileName + " trustee_sid: " + trusteeSID);
		item = this->CreateItem();
		item->SetStatus(OvalEnum::STATUS_EXISTS);
		item->AppendElement(new ItemEntity("filepath", filePath, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
		item->AppendElement(new ItemEntity("path", path, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
		item->AppendElement(new ItemEntity("filename", fileName, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
		item->AppendElement(new ItemEntity("trustee_sid", trusteeSID, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));


		// build structure to hold the rights
		pAccessRights = reinterpret_cast<PACCESS_MASK>(::LocalAlloc(LPTR, sizeof(PACCESS_MASK) + sizeof(ACCESS_MASK)));
		if(pAccessRights == NULL) {
			throw ProbeException(baseErrMsg + " Out of memory! Unable to allocate memory for access rights.");
		}

		// get the rights
		Log::Debug("Getting rights mask for file: " + path + " filename: " + fileName + " trustee_sid: " + trusteeSID);
		WindowsCommon::GetEffectiveRightsForWindowsObject(SE_FILE_OBJECT, pSid, &filePath, pAccessRights);
	
        if((*pAccessRights) & DELETE)
            item->AppendElement(new ItemEntity("standard_delete", "1", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));
        else 
            item->AppendElement(new ItemEntity("standard_delete", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));

		if((*pAccessRights) & READ_CONTROL)
            item->AppendElement(new ItemEntity("standard_read_control", "1", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));
        else 
            item->AppendElement(new ItemEntity("standard_read_control", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));

		if((*pAccessRights) & WRITE_DAC)
            item->AppendElement(new ItemEntity("standard_write_dac", "1", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));
        else 
            item->AppendElement(new ItemEntity("standard_write_dac", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));

		if((*pAccessRights) & WRITE_OWNER)
            item->AppendElement(new ItemEntity("standard_write_owner", "1", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));
        else 
            item->AppendElement(new ItemEntity("standard_write_owner", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));

		if((*pAccessRights) & SYNCHRONIZE)
            item->AppendElement(new ItemEntity("standard_synchronize", "1", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));
        else 
            item->AppendElement(new ItemEntity("standard_synchronize", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));
		
        if((*pAccessRights) & ACCESS_SYSTEM_SECURITY)
            item->AppendElement(new ItemEntity("access_system_security", "1", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));
        else 
            item->AppendElement(new ItemEntity("access_system_security", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));
		
        if( ( (*pAccessRights) & FILE_GENERIC_READ ) == FILE_GENERIC_READ )
            item->AppendElement(new ItemEntity("generic_read", "1", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));
        else 
            item->AppendElement(new ItemEntity("generic_read", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));

		if( ( (*pAccessRights) & FILE_GENERIC_WRITE ) == FILE_GENERIC_WRITE )
            item->AppendElement(new ItemEntity("generic_write", "1", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));
        else 
            item->AppendElement(new ItemEntity("generic_write", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));

		if( ( (*pAccessRights) & FILE_GENERIC_EXECUTE ) == FILE_GENERIC_EXECUTE )
            item->AppendElement(new ItemEntity("generic_execute", "1", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));
        else 
            item->AppendElement(new ItemEntity("generic_execute", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));

		if((*pAccessRights) & FILE_READ_DATA)
            item->AppendElement(new ItemEntity("generic_all", "1", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));
        else 
            item->AppendElement(new ItemEntity("generic_all", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));
		
        if((*pAccessRights) & FILE_READ_DATA)
            item->AppendElement(new ItemEntity("file_read_data", "1", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));
        else
            item->AppendElement(new ItemEntity("file_read_data", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));

		if((*pAccessRights) & FILE_WRITE_DATA)
            item->AppendElement(new ItemEntity("file_write_data", "1", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));
        else 
            item->AppendElement(new ItemEntity("file_write_data", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));

		if((*pAccessRights) & FILE_APPEND_DATA)
            item->AppendElement(new ItemEntity("file_append_data", "1", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));
        else
            item->AppendElement(new ItemEntity("file_append_data", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));
		
        if((*pAccessRights) & FILE_READ_EA)
            item->AppendElement(new ItemEntity("file_read_ea", "1", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));
        else 
            item->AppendElement(new ItemEntity("file_read_ea", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));

		if((*pAccessRights) & FILE_WRITE_EA)
            item->AppendElement(new ItemEntity("file_write_ea", "1", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));
        else 
            item->AppendElement(new ItemEntity("file_write_ea", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));

        if((*pAccessRights) & FILE_EXECUTE)
            item->AppendElement(new ItemEntity("file_execute", "1", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));
        else 
            item->AppendElement(new ItemEntity("file_execute", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));

		if((*pAccessRights) & FILE_DELETE_CHILD)
            item->AppendElement(new ItemEntity("file_delete_child", "1", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));
        else 
            item->AppendElement(new ItemEntity("file_delete_child", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));

		if((*pAccessRights) & FILE_READ_ATTRIBUTES)
            item->AppendElement(new ItemEntity("file_read_attributes", "1", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));
        else 
            item->AppendElement(new ItemEntity("file_read_attributes", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));

		if((*pAccessRights) & FILE_WRITE_ATTRIBUTES)
            item->AppendElement(new ItemEntity("file_write_attributes", "1", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));
        else 
            item->AppendElement(new ItemEntity("file_write_attributes", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));

	} catch(Exception ex) {

		if(item != NULL) {

			item->SetStatus(OvalEnum::STATUS_ERROR);
			item->AppendMessage(new OvalMessage(ex.GetErrorMessage(), OvalEnum::LEVEL_ERROR));

		} else {

			if(pAccessRights != NULL) {
				LocalFree(pAccessRights);
				pAccessRights = NULL;
			}
			
			if(pSid != NULL) {
				LocalFree(pSid);
				pSid = NULL;
			}
			throw ex;
		} 
	} 

	if(pAccessRights != NULL) {
		LocalFree(pAccessRights);
		pAccessRights = NULL;
	}
	
	if(pSid != NULL) {
		LocalFree(pSid);
		pSid = NULL;
	}

	return item;
}