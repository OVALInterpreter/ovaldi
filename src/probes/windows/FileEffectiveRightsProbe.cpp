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

#include "FileEffectiveRightsProbe.h"


//****************************************************************************************//
//								FileEffectiveRightsProbe Class							  //	
//****************************************************************************************//
FileEffectiveRightsProbe* FileEffectiveRightsProbe::instance = NULL;

FileEffectiveRightsProbe::FileEffectiveRightsProbe() {

}

FileEffectiveRightsProbe::~FileEffectiveRightsProbe() {
  instance = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* FileEffectiveRightsProbe::Instance() {

	// Use lazy initialization
	if(instance == NULL) 
		instance = new FileEffectiveRightsProbe();

	return instance;	
}

ItemVector* FileEffectiveRightsProbe::CollectItems(Object* object) {
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
	//		- Get all trustee names on the system. Reuse the logic in SidProbe::GetAllTrusteeNames() 
	//		- Get the set of matching trustee names
	//
	//	- if using variables
	//		- Get all trustee names on the system. Reuse the logic in SidProbe::GetAllTrusteeNames() 
	//		- Get the set of matching trustee names
	//		- call GetEffectiveRights method
	//
	// -----------------------------------------------------------------------

	// get the path and file name
	ObjectEntity* path = object->GetElementByName("path");
	ObjectEntity* fileName = object->GetElementByName("filename");
	ObjectEntity* trusteeName = object->GetElementByName("trustee_name");

	// check trustee datatypes - only allow string
	if(trusteeName->GetDatatype() != OvalEnum::DATATYPE_STRING) {
		throw ProbeException("Error: invalid data type specified on trustee_name. Found: " + OvalEnum::DatatypeToString(trusteeName->GetDatatype()));
	}

	// check trustee operation - only allow  equals, not equals and pattern match
	if(trusteeName->GetOperation() != OvalEnum::OPERATION_EQUALS 
		&& trusteeName->GetOperation() != OvalEnum::OPERATION_PATTERN_MATCH 
		&& trusteeName->GetOperation() != OvalEnum::OPERATION_NOT_EQUAL) {
		throw ProbeException("Error: invalid operation specified on trustee_name. Found: " + OvalEnum::OperationToString(trusteeName->GetOperation()));
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
					// The file exists so lets get the trustees to then examine effective rights
					//
					StringSet* trusteeNames = this->GetTrusteeNamesForFile(fp, trusteeName, resolveGroupBehavior, includeGroupBehavior);
					if(!trusteeNames->empty()) {
						StringSet::iterator iterator;
						for(iterator = trusteeNames->begin(); iterator != trusteeNames->end(); iterator++) {
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
						delete trusteeNames;

					} else {

						Log::Debug("No matching trustees found when getting effective rights for object: " + object->GetId());

						StringSet* trusteeNames = new StringSet();
						if(this->ReportTrusteeNameDoesNotExist(trusteeName, trusteeNames)) {

							StringSet::iterator iterator;
							for(iterator = trusteeNames->begin(); iterator != trusteeNames->end(); iterator++) {

								Item* item = this->CreateItem();
								item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
								item->AppendElement(new ItemEntity("path", fp->first, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
								item->AppendElement(new ItemEntity("filename", fp->second, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
								item->AppendElement(new ItemEntity("trustee_name", (*iterator), OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_DOES_NOT_EXIST));
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
Item* FileEffectiveRightsProbe::CreateItem() {

	Item* item = new Item(0, 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows", 
						"win-sc", 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows windows-system-characteristics-schema.xsd", 
						OvalEnum::STATUS_ERROR, 
						"fileeffectiverights_item");

	return item;
}

StringSet* FileEffectiveRightsProbe::GetTrusteeNamesForFile(StringPair* fp, ObjectEntity* trusteeName,  bool resolveGroupBehavior, bool includeGroupBehavior) {

	PACL pdacl = NULL;
    PSID ownerSid;
    PSID primaryGroupSid;
	PSECURITY_DESCRIPTOR pSD = NULL;

    StringSet* allTrusteeNames = new StringSet();
    StringSet* workingTrusteeNames = new StringSet();
    StringSet* resultingTrusteeNames = new StringSet();

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
		WindowsCommon::GetTrusteeNamesFromPACL(pdacl, allTrusteeNames);

        // insert the owner and primary group sids
        allTrusteeNames->insert(WindowsCommon::GetFormattedTrusteeName(ownerSid));
        allTrusteeNames->insert(WindowsCommon::GetFormattedTrusteeName(primaryGroupSid));

        Log::Debug("Found " + Common::ToString(allTrusteeNames->size()) + " trustee names when searching for all trustee names on the security descriptor of: " + filePath);

		// does this trusteeName use variables?
		if(trusteeName->GetVarRef() == NULL) {
			
			// proceed based on operation
			if(trusteeName->GetOperation() == OvalEnum::OPERATION_EQUALS) {
				
				// check that the trustee Name exists
                if(WindowsCommon::TrusteeNameExists(trusteeName->GetValue())) {
					workingTrusteeNames->insert(trusteeName->GetValue());
				}

			} else if(trusteeName->GetOperation() == OvalEnum::OPERATION_NOT_EQUAL) {
				this->GetMatchingTrusteeNames(trusteeName->GetValue(), allTrusteeNames, workingTrusteeNames, false);
			} else if(trusteeName->GetOperation() == OvalEnum::OPERATION_PATTERN_MATCH) {
				this->GetMatchingTrusteeNames(trusteeName->GetValue(), allTrusteeNames, workingTrusteeNames, true);
			}
		} else {

            if(trusteeName->GetOperation() == OvalEnum::OPERATION_EQUALS) {
                // in the case of equals simply loop through all the 
			    // variable values and add them to the set of all names
			    // if they exist on the system
			    VariableValueVector::iterator iterator;
			    for(iterator = trusteeName->GetVarRef()->GetValues()->begin(); iterator != trusteeName->GetVarRef()->GetValues()->end(); iterator++) {
    				
				    if(WindowsCommon::TrusteeNameExists((*iterator)->GetValue())) {
					    allTrusteeNames->insert((*iterator)->GetValue());
				    }
			    }
            }
		
			// loop through all trustee Names
			// only keep those that match operation and value and var check
			StringSet::iterator it;
			ItemEntity* tmp = this->CreateItemEntity(trusteeName);
			for(it = allTrusteeNames->begin(); it != allTrusteeNames->end(); it++) {
				tmp->SetValue((*it));
				if(trusteeName->Analyze(tmp) == OvalEnum::RESULT_TRUE) {
					workingTrusteeNames->insert((*it));
				}
			}
		}

		delete allTrusteeNames;
        allTrusteeNames = NULL;

		// apply the behaviors
		for(StringSet::iterator it = workingTrusteeNames->begin(); it != workingTrusteeNames->end(); it++) {
			// is this a group
            bool isGroup = WindowsCommon::IsGroup((*it));

			if(isGroup && resolveGroupBehavior) {

				if(includeGroupBehavior) {
					resultingTrusteeNames->insert((*it));
				}

				// get the group members and add them to the set
				StringSet* groupMembers = new StringSet();
                WindowsCommon::ExpandGroup((*it), groupMembers, includeGroupBehavior, resolveGroupBehavior);
				for(StringSet::iterator iterator = groupMembers->begin(); iterator != groupMembers->end(); iterator++) {
					resultingTrusteeNames->insert((*iterator));
				}
				delete groupMembers;

			} else {
				resultingTrusteeNames->insert((*it));
			}
		}
   
	} catch(...) {

		if(pSD != NULL) {
			LocalFree(pSD);
		}

        if(allTrusteeNames != NULL) {
			delete allTrusteeNames;
            allTrusteeNames = NULL;
		}

        if(workingTrusteeNames != NULL) {
			delete workingTrusteeNames;
            workingTrusteeNames = NULL;
		}

		if(resultingTrusteeNames != NULL) {
			delete resultingTrusteeNames;
            resultingTrusteeNames = NULL;
		}

        throw;
    }

	if(pSD != NULL) {
		LocalFree(pSD);
	}

    if(workingTrusteeNames != NULL) {
		delete workingTrusteeNames;
        workingTrusteeNames = NULL;
	}

    Log::Debug("Found " + Common::ToString(resultingTrusteeNames->size()) + " matching trustee names after applying behaviors on the security descriptor of: " + filePath);

	return resultingTrusteeNames;
}

void FileEffectiveRightsProbe::GetMatchingTrusteeNames(string trusteeNamePattern, StringSet* allTrusteeNames, StringSet* trusteeNames, bool isRegex) {
	StringSet::iterator iterator;
	for(iterator = allTrusteeNames->begin(); iterator != allTrusteeNames->end(); iterator++) {
		if(this->IsMatch(trusteeNamePattern, (*iterator), isRegex)) {
			trusteeNames->insert((*iterator));
		}
	}
}

Item* FileEffectiveRightsProbe::GetEffectiveRights(string path, string fileName, string trusteeName) {
	
	Item* item = NULL;
	PSID pSid = NULL;
	PACCESS_MASK pAccessRights = NULL;
	
	// build the path
	string filePath = Common::BuildFilePath((const string)path, (const string)fileName);

	string baseErrMsg = "Error unable to get effective rights for trustee: " + trusteeName + " from dacl for file: " + filePath;

	try {

		// verify that the file exists.
		if(GetFileAttributes(filePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
			string systemErrMsg = WindowsCommon::GetErrorMessage(GetLastError());
			throw ProbeException(baseErrMsg + " File " + filePath + " doesn't exist. " + systemErrMsg);
		}

		// Get the sid for the trustee name
		pSid = WindowsCommon::GetSIDForTrusteeName(trusteeName);
		

		// the file exists and trustee name seems good so we can create the new item now.
		item = this->CreateItem();
		item->SetStatus(OvalEnum::STATUS_EXISTS);
		item->AppendElement(new ItemEntity("path", path, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
		item->AppendElement(new ItemEntity("filename", fileName, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
		item->AppendElement(new ItemEntity("trustee_name", trusteeName, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));


		// build structure to hold the rights
		pAccessRights = reinterpret_cast<PACCESS_MASK>(::LocalAlloc(LPTR, sizeof(PACCESS_MASK) + sizeof(ACCESS_MASK)));
		if(pAccessRights == NULL) {	
			throw ProbeException(baseErrMsg + " Out of memory! Unable to allocate memory for access rights.");
		}

		// get the rights
		Log::Debug("Getting rights mask for file: " + path + " filename: " + fileName + " trustee_name: " + trusteeName);
		WindowsCommon::GetEffectiveRightsForFile(pSid, &filePath, pAccessRights);

			
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
		
        if((*pAccessRights) & FILE_GENERIC_READ)
            item->AppendElement(new ItemEntity("generic_read", "1", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));
        else 
            item->AppendElement(new ItemEntity("generic_read", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));

		if((*pAccessRights) & FILE_GENERIC_WRITE)
            item->AppendElement(new ItemEntity("generic_write", "1", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));
        else 
            item->AppendElement(new ItemEntity("generic_write", "0", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_EXISTS));

		if((*pAccessRights) & FILE_GENERIC_EXECUTE)
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

bool FileEffectiveRightsProbe::ReportTrusteeNameDoesNotExist(ObjectEntity *trusteeName, StringSet* trusteeNames) {
	
	bool result = false;
	
	if(trusteeName->GetOperation() == OvalEnum::OPERATION_EQUALS && !trusteeName->GetNil()) {		
		
		if(trusteeName->GetVarRef() == NULL) {
            if(!WindowsCommon::TrusteeNameExists(trusteeName->GetValue())) {
				trusteeNames->insert(trusteeName->GetValue());
				result = true;
			}
		} else {

			VariableValueVector::iterator iterator;
			for(iterator = trusteeName->GetVarRef()->GetValues()->begin(); iterator != trusteeName->GetVarRef()->GetValues()->end(); iterator++) {
                if(!WindowsCommon::TrusteeNameExists((*iterator)->GetValue())) {
					trusteeNames->insert((*iterator)->GetValue());
					result = true;
				}
			}
		}
	}

	return  result;
}
