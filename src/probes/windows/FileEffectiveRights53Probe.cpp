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
            } else if(behavior->GetName().compare("max_depth") == 0 || behavior->GetName().compare("recurse_direction") == 0) {
                // skip these they are supported in the file finder class.

			} else {
                Log::Info("Unsupported behavior found when collecting " + object->GetId() + " Found behavior: " + behavior->GetName() + " = " + behavior->GetValue());
			}
		}		
	}

	FileFinder fileFinder;
	StringPairVector* filePaths = fileFinder.SearchFiles(path, fileName, object->GetBehaviors());

	if(filePaths->size() > 0) {
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
						item->AppendElement(new ItemEntity("path", fp->first, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
						item->AppendElement(new ItemEntity("filename", (*iterator), OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_DOES_NOT_EXIST));
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
                    StringSet* trusteeSIDs = this->GetTrusteeSIDsForFile(fp, trusteeSID, resolveGroupBehavior, includeGroupBehavior);
					if(!trusteeSIDs->empty()) {
						StringSet::iterator iterator;
						for(iterator = trusteeSIDs->begin(); iterator != trusteeSIDs->end(); iterator++) {
							try {
								Item* item = this->GetEffectiveRights(fp->first, fp->second, (*iterator));
								if(item != NULL) {
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
						if(this->ReportTrusteeSIDDoesNotExist(trusteeSID, trusteeSIDs)) {

							StringSet::iterator iterator;
							for(iterator = trusteeSIDs->begin(); iterator != trusteeSIDs->end(); iterator++) {

								Item* item = this->CreateItem();
								item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
								item->AppendElement(new ItemEntity("path", fp->first, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
								item->AppendElement(new ItemEntity("filename", fp->second, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
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
		// if no filepaths check if the code should report that the path does not exist
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
	delete filePaths;

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

StringSet* FileEffectiveRights53Probe::GetTrusteeSIDsForFile(StringPair* fp, ObjectEntity* trusteeSID,  bool resolveGroupBehavior, bool includeGroupBehavior) {

	PACL pdacl = NULL;
    PSID ownerSid;
    PSID primaryGroupSid;
	PSECURITY_DESCRIPTOR pSD = NULL;
	StringSet* allTrusteeSIDs = new StringSet();
    StringSet* workingTrusteeSIDs = new StringSet();
    StringSet* resultingTrusteeSIDs = new StringSet();
    string filePath;
	
    try {

        // start by getting the sids from the dacl, owner, and primary group off the file's security descriptor

		// build the path
        filePath = Common::BuildFilePath((const string)fp->first, (const string)fp->second);

        string baseErrMsg = "Error unable to get trustees for file: " + filePath;

		DWORD res = GetNamedSecurityInfo(const_cast<char*>(filePath.c_str()), // object name
							SE_FILE_OBJECT,						// object type
							DACL_SECURITY_INFORMATION |			// information type
							GROUP_SECURITY_INFORMATION | 
							OWNER_SECURITY_INFORMATION, 			
							&ownerSid,							// owner SID
							&primaryGroupSid,   				// primary group SID
							&pdacl,								// DACL
							NULL,								// SACL
							&pSD);								// Security Descriptor

		if (res != ERROR_SUCCESS) {
		    if (res == ERROR_FILE_NOT_FOUND) {
			    // should never get here. 
			    // before calling this function the file should already have been checked for existence.
			    throw Exception( baseErrMsg + " Unable locate the specified file."); 
		    } else {
			    throw Exception( baseErrMsg + " Unable to retrieve a copy of the security descriptor. System error message: " + WindowsCommon::GetErrorMessage(res)); 
		    }
	    }

        // get sids from the dacl
		WindowsCommon::GetSidsFromPACL(pdacl, allTrusteeSIDs);

        // insert the owner and primary group sids
        allTrusteeSIDs->insert(WindowsCommon::ToString(ownerSid));
        allTrusteeSIDs->insert(WindowsCommon::ToString(primaryGroupSid));

        Log::Debug("Found " + Common::ToString(allTrusteeSIDs->size()) + " trustee SIDs when searching for all SIDs on the security descriptor of: " + filePath);


		// does this trusteeSID use variables?
		if(trusteeSID->GetVarRef() == NULL) {
			
			// proceed based on operation
			if(trusteeSID->GetOperation() == OvalEnum::OPERATION_EQUALS) {
				
				// check that the trustee SID exists
				if(this->TrusteeSIDExists(trusteeSID->GetValue(), allTrusteeSIDs)) {
					workingTrusteeSIDs->insert(trusteeSID->GetValue());
				}

			} else if(trusteeSID->GetOperation() == OvalEnum::OPERATION_NOT_EQUAL) {
				this->GetMatchingTrusteeSIDs(trusteeSID->GetValue(), allTrusteeSIDs, workingTrusteeSIDs, false);
			} else if(trusteeSID->GetOperation() == OvalEnum::OPERATION_PATTERN_MATCH) {
				this->GetMatchingTrusteeSIDs(trusteeSID->GetValue(), allTrusteeSIDs, workingTrusteeSIDs, true);
			}
		} else {
		
			// loop through all trustee SIDs on the system
			// only keep those that match operation and value and var check
			StringSet::iterator it;
			ItemEntity* tmp = this->CreateItemEntity(trusteeSID);
			for(it = allTrusteeSIDs->begin(); it != allTrusteeSIDs->end(); it++) {
				tmp->SetValue((*it));
				if(trusteeSID->Analyze(tmp) == OvalEnum::RESULT_TRUE) {
					workingTrusteeSIDs->insert((*it));
				}
			}
		}

		delete allTrusteeSIDs;
        allTrusteeSIDs = NULL;

		// apply the behaviors
		for(StringSet::iterator it = workingTrusteeSIDs->begin(); it != workingTrusteeSIDs->end(); it++) {
			// is this a group
			bool isGroup = WindowsCommon::IsGroupSID((*it));

			if(isGroup && resolveGroupBehavior) {

				if(includeGroupBehavior) {
					resultingTrusteeSIDs->insert((*it));
				}

				// get the group members and add them to the set
				StringSet* groupMembers = new StringSet();
				WindowsCommon::ExpandGroupBySID((*it), groupMembers, includeGroupBehavior, resolveGroupBehavior);
				for(StringSet::iterator iterator = groupMembers->begin(); iterator != groupMembers->end(); iterator++) {
					resultingTrusteeSIDs->insert((*iterator));
				}
				delete groupMembers;

			} else {
				resultingTrusteeSIDs->insert((*it));
			}
		}
   
	} catch(...) {

		if(pSD != NULL) {
			LocalFree(pSD);
		}

        if(allTrusteeSIDs != NULL) {
			delete allTrusteeSIDs;
            allTrusteeSIDs = NULL;
		}

        if(workingTrusteeSIDs != NULL) {
			delete workingTrusteeSIDs;
            workingTrusteeSIDs = NULL;
		}

		if(resultingTrusteeSIDs != NULL) {
			delete resultingTrusteeSIDs;
            resultingTrusteeSIDs = NULL;
		}

        throw;
    }

	if(pSD != NULL) {
		LocalFree(pSD);
	}

    if(workingTrusteeSIDs != NULL) {
		delete workingTrusteeSIDs;
        workingTrusteeSIDs = NULL;
	}

    Log::Debug("Found " + Common::ToString(resultingTrusteeSIDs->size()) + " matching SIDs after applying behaviors on the security descriptor of: " + filePath);

	return resultingTrusteeSIDs;
}

void FileEffectiveRights53Probe::GetMatchingTrusteeSIDs(string trusteeSIDPattern, StringSet* allTrusteeSIDs, StringSet* trusteeSIDs, bool isRegex) {

	StringSet::iterator iterator;
	for(iterator = allTrusteeSIDs->begin(); iterator != allTrusteeSIDs->end(); iterator++) {
		if(this->IsMatch(trusteeSIDPattern, (*iterator), isRegex)) {
			trusteeSIDs->insert((*iterator));
		}
	}
}

bool FileEffectiveRights53Probe::TrusteeSIDExists(string trusteeSID, StringSet* trusteeSIDs) {

	bool exists = false;

	StringSet::iterator iterator;
	for(iterator = trusteeSIDs->begin(); iterator != trusteeSIDs->end(); iterator++) {
		if(trusteeSID.compare((*iterator)) == 0) {
			exists = true;
		}
	}			

	return exists;
}

Item* FileEffectiveRights53Probe::GetEffectiveRights(string path, string fileName, string trusteeSID) {
	
	//  Note: The original logic present in FileEffectiveRights53Probe  
	//        utilized the API GetEffectiveRightsFromAcl.  This API was very 
	//        restrictive in terms of what users could call it and on what files.
	//        The Authz API set does not suffer from this restriction.

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

		
		// the file exists and teh trustee_sid looks valid so we can create the new item now.
		Log::Debug("Creating item to hold file effective rights for: " + path + " filename: " + fileName + " trustee_sid: " + trusteeSID);
		item = this->CreateItem();
		item->SetStatus(OvalEnum::STATUS_EXISTS);
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
		WindowsCommon::GetEffectiveRightsForFile(pSid, &filePath, pAccessRights);
			

		// Convert access mask to binary.
		// http://msdn2.microsoft.com/en-us/library/aa374896.aspx
		char mask[33];
		ZeroMemory(&mask, 33);

		for (int j = 0; j < 32; j++) {
			if ((*pAccessRights) & (1 << j))
				mask[j] = '1';
			else
				mask[j] = '0';
		}

		// need to seperatly determine if the generic bit should be set.
		// the access mask that is returned never has the generic bits set. 
		// Those bits can be determined by rolling up the object specific access bits
		//if((*pAccessRights) & FILE_GENERIC_READ)
		//	mask[31] = '1';
		//if((*pAccessRights) & FILE_GENERIC_WRITE)
		//	mask[30] = '1';
		//if((*pAccessRights) & FILE_GENERIC_EXECUTE)
		//	mask[29] = '1';
		//if((*pAccessRights) & FILE_ALL_ACCESS)
		//	mask[28] = '1';
		
		// read values in the access_mask
		item->AppendElement(new ItemEntity("standard_delete", Common::ToString(mask[16]), OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));
		item->AppendElement(new ItemEntity("standard_read_control", Common::ToString(mask[17]), OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));
		item->AppendElement(new ItemEntity("standard_write_dac", Common::ToString(mask[18]), OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));
		item->AppendElement(new ItemEntity("standard_write_owner", Common::ToString(mask[19]), OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));
		item->AppendElement(new ItemEntity("standard_synchronize", Common::ToString(mask[20]), OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));
		item->AppendElement(new ItemEntity("access_system_security", Common::ToString(mask[24]), OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));
		item->AppendElement(new ItemEntity("generic_read", Common::ToString(mask[31]), OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));
		item->AppendElement(new ItemEntity("generic_write", Common::ToString(mask[30]), OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));
		item->AppendElement(new ItemEntity("generic_execute", Common::ToString(mask[29]), OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));
		item->AppendElement(new ItemEntity("generic_all", Common::ToString(mask[28]), OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));
		item->AppendElement(new ItemEntity("file_read_data", Common::ToString(mask[0]), OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));
		item->AppendElement(new ItemEntity("file_write_data", Common::ToString(mask[1]), OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));
		item->AppendElement(new ItemEntity("file_append_data", Common::ToString(mask[2]), OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));
		item->AppendElement(new ItemEntity("file_read_ea", Common::ToString(mask[3]), OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));
		item->AppendElement(new ItemEntity("file_write_ea", Common::ToString(mask[4]), OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));
		item->AppendElement(new ItemEntity("file_execute", Common::ToString(mask[5]), OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));
		item->AppendElement(new ItemEntity("file_delete_child", Common::ToString(mask[6]), OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));
		item->AppendElement(new ItemEntity("file_read_attributes", Common::ToString(mask[7]), OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));
		item->AppendElement(new ItemEntity("file_write_attributes", Common::ToString(mask[8]), OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));

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

bool FileEffectiveRights53Probe::ReportTrusteeSIDDoesNotExist(ObjectEntity *trusteeSID, StringSet* trusteeSIDs) {
	
	bool result = false;
	
	if(trusteeSID->GetOperation() == OvalEnum::OPERATION_EQUALS && !trusteeSID->GetNil()) {		
		
		if(trusteeSID->GetVarRef() == NULL) {
			if(!this->TrusteeSIDExists(trusteeSID->GetValue(), WindowsCommon::GetAllTrusteeSIDs())) {		
				trusteeSIDs->insert(trusteeSID->GetValue());
				result = true;
			}
		} else {

			for(VariableValueVector::iterator iterator = trusteeSID->GetVarRef()->GetValues()->begin(); iterator != trusteeSID->GetVarRef()->GetValues()->end(); iterator++) {
				if(this->TrusteeSIDExists((*iterator)->GetValue(), WindowsCommon::GetAllTrusteeSIDs())) {
					trusteeSIDs->insert((*iterator)->GetValue());
					result = true;
				}
			}
		}
	}

	return  result;
}
