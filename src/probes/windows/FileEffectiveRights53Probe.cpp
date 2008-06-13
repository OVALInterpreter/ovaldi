//
//
//****************************************************************************************//
// Copyright (c) 2002-2008, The MITRE Corporation
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
//								FileEffectiveRights53Probe Class							  //	
//****************************************************************************************//
FileEffectiveRights53Probe* FileEffectiveRights53Probe::instance = NULL;

FileEffectiveRights53Probe::FileEffectiveRights53Probe() {

}

FileEffectiveRights53Probe::~FileEffectiveRights53Probe() {

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

	ItemVector *collectedItems = new ItemVector();

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

	// support behaviors - init with defaults.
	bool includeGroupBehavior = true;
	bool resolveGroupBehavior = false;
	if(object->GetBehaviors()->size() != 0) {
		BehaviorVector* behaviors = object->GetBehaviors();
		BehaviorVector::iterator iterator;
		for(iterator = behaviors->begin(); iterator != behaviors->end(); iterator++) {
			Behavior* behavior = (*iterator);
			if(behavior->GetName().compare("include_group") == 0 && behavior->GetValue().compare("false") == 0) {
				includeGroupBehavior = false;
			} else if(behavior->GetName().compare("resolve_group") == 0 && behavior->GetValue().compare("true") == 0) {
				resolveGroupBehavior = true;
			} else {
				Log::Info("Unsupported behavior found when collecting " + object->GetId());
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

			if(fp->second.compare("") == 0) {

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
					StringSet* trusteeSIDs = this->GetTrusteeSIDs(fp->first, fp->second, trusteeSID, resolveGroupBehavior, includeGroupBehavior);
					if(!trusteeSIDs->empty()) {
						StringSet::iterator iterator;
						for(iterator = trusteeSIDs->begin(); iterator != trusteeSIDs->end(); iterator++) {
							try {
								Item* item = this->GetEffectiveRights(fp->first, fp->second, (*iterator));
								if(item != NULL) {
									collectedItems->push_back(item);
								}
							} catch (ProbeException ex) {
								Log::Debug(ex.GetErrorMessage());
							} catch (Exception ex) {
								Log::Debug("Unknown error while getting effective rights for file. Path: " + fp->first + " Name: " + fp->second + " " + ex.GetErrorMessage());
							}
						}
						delete trusteeSIDs;

					} else {

						StringVector* trusteeSIDs = NULL;
						if(this->ReportTrusteeSIDDoesNotExist(trusteeSID, trusteeSIDs)) {

							StringVector::iterator iterator;
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
					Log::Debug(ex.GetErrorMessage());
				} catch (...) {
					Log::Debug("Unknown error.");
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
						"fileeffectiverights53_item");

	return item;
}

StringSet* FileEffectiveRights53Probe::GetTrusteeSIDs(string path, string fileName, ObjectEntity* trusteeSID,  bool resolveGroupBehavior, bool includeGroupBehavior) {

	StringSet* trusteeSIDs = new StringSet();
	
	// load the set of all Trustee SIDs
	StringVector* allTrusteeSIDs = WindowsCommon::GetAllTrusteeSIDs();

	// does this trusteeSID use variables?
	if(trusteeSID->GetVarRef() == NULL) {
		
		// proceed based on operation
		if(trusteeSID->GetOperation() == OvalEnum::OPERATION_EQUALS) {
			
			// check that the trustee SID exists
			if(this->TrusteeSIDExists(trusteeSID->GetValue(), allTrusteeSIDs)) {
				trusteeSIDs->insert(trusteeSID->GetValue());
			}

		} else if(trusteeSID->GetOperation() == OvalEnum::OPERATION_NOT_EQUAL) {
			this->GetMatchingTrusteeSIDs(trusteeSID->GetValue(), allTrusteeSIDs, trusteeSIDs, false);
		} else if(trusteeSID->GetOperation() == OvalEnum::OPERATION_PATTERN_MATCH) {
			this->GetMatchingTrusteeSIDs(trusteeSID->GetValue(), allTrusteeSIDs, trusteeSIDs, true);
		}		

	} else {
	
		// loop through all trustee SIDs on the system
		// only keep those that match operation and value and var check
		StringVector::iterator it;
		ItemEntity* tmp = this->CreateItemEntity(trusteeSID);
		for(it = allTrusteeSIDs->begin(); it != allTrusteeSIDs->end(); it++) {
			tmp->SetValue((*it));
			if(trusteeSID->Analyze(tmp) == OvalEnum::RESULT_TRUE) {
				trusteeSIDs->insert((*it));
			}
		}
	}

	// using common code to get these do not delete they are cached.
	//delete allTrusteeSIDs;

	// apply the behaviors
	StringSet* trusteeSIDSet = new StringSet();
	if(!trusteeSIDs->empty()) {
		
		StringVector::iterator iterator;
		StringSet::iterator it;
		string domainStr = "";
		string sidStr = "";
		bool isGroup;

		for(it = trusteeSIDs->begin(); it != trusteeSIDs->end(); it++) {
			// is this a group
			isGroup = WindowsCommon::IsGroupSID((*it));

			if(isGroup && resolveGroupBehavior) {

				if(includeGroupBehavior) {
					trusteeSIDSet->insert((*it));
				}

				// get the group members and add them to the set
				StringVector* groupMembers = new StringVector();
				WindowsCommon::ExpandGroupBySID((*it), groupMembers);
				for(iterator = groupMembers->begin(); iterator != groupMembers->end(); iterator++) {
					trusteeSIDSet->insert((*iterator));
				}
				delete groupMembers;

			} else {
				trusteeSIDSet->insert((*it));
			}
		}
	}

	return trusteeSIDSet;
}

void FileEffectiveRights53Probe::GetMatchingTrusteeSIDs(string trusteeSIDPattern, StringVector* allTrusteeSIDs, StringSet* trusteeSIDs, bool isRegex) {

	StringVector::iterator iterator;
	for(iterator = allTrusteeSIDs->begin(); iterator != allTrusteeSIDs->end(); iterator++) {
		if(this->IsMatch(trusteeSIDPattern, (*iterator), isRegex)) {
			trusteeSIDs->insert((*iterator));
		}
	}
}

bool FileEffectiveRights53Probe::TrusteeSIDExists(string trusteeSID, StringVector* trusteeSIDs) {

	bool exists = false;

	StringVector::iterator iterator;
	for(iterator = trusteeSIDs->begin(); iterator != trusteeSIDs->end(); iterator++) {
		if(trusteeSID.compare((*iterator)) == 0) {
			exists = true;
		}
	}			

	return exists;
}

Item* FileEffectiveRights53Probe::GetEffectiveRights(string path, string fileName, string trusteeSID) {
	// -----------------------------------------------------------------------
	//
	//  ABSTRACT
	//
	//	Return a populated item for the specified trustees on the specified file.
	//
	//	- Call GetNamedSecurityInfo to get a DACL Security Descriptor for the file
	//	  http://msdn2.microsoft.com/en-us/library/aa446645.aspx
	//	- Use provided trustee name and call LsaLookupNames to get the sid
	//	  http://msdn2.microsoft.com/en-us/library/ms721797.aspx
	//	- Then call GetEffectiveRightsFromAcl with the dacl and the sid found in the earlier calls
	//	  http://msdn2.microsoft.com/en-us/library/aa446637.aspx
	// -----------------------------------------------------------------------
	

	Item* item = NULL;

	// build the path
	string filePath = path;
	if(fileName.compare("") != 0) {
		// Verify that the path that was passed into this function ends with a slash.  If
		// it doesn't, then add one.
		if (path[path.length()-1] != Common::fileSeperator)
			filePath.append(1, Common::fileSeperator);

		filePath.append(fileName);
	}

	string baseErrMsg = "Error unable to get effective rights for trustee: " + trusteeSID + " from dacl for file: " + filePath;

	DWORD res;
	PACL pdacl;
	PSECURITY_DESCRIPTOR pSD;

	res = GetNamedSecurityInfo(const_cast<char*>(filePath.c_str()),	// object name
							   SE_FILE_OBJECT,						// object type
							   DACL_SECURITY_INFORMATION |			// information type
							   PROTECTED_DACL_SECURITY_INFORMATION |
							   UNPROTECTED_DACL_SECURITY_INFORMATION, 			
							   NULL,								// owner SID
							   NULL,								// primary group SID
							   &pdacl,								// DACL
							   NULL,								// SACL
							   &pSD);								// Security Descriptor

	if (res != ERROR_SUCCESS) {
		if (res == ERROR_FILE_NOT_FOUND) {
			// should never get here. 
			// before calling this function the file should already have been checked for existence.
			throw ProbeException( baseErrMsg + " Unable locate the specified file."); 
		} else {
			throw ProbeException(baseErrMsg + " Unable to retrieve a copy of the security descriptor. System error message: " + WindowsCommon::GetErrorMessage(res)); 
		}
	} 

	// Check to see if a valid security descriptor was returned.  
    if ((IsValidSecurityDescriptor(pSD) == 0) || (IsValidAcl(pdacl) == 0)) {
		LocalFree(pSD);
		throw ProbeException(baseErrMsg + " Invalid data returned from call to GetNamedSecurityInfo().");

		//item->SetStatus(OvalEnum::STATUS_ERROR);
		//item->AppendMessage(new OvalMessage(baseErrMsg + " Invalid data returned from call to GetNamedSecurityInfo().", OvalEnum::LEVEL_ERROR));
		//Log::Debug(baseErrMsg + " Invalid data returned from call to GetNamedSecurityInfo().");
		//return item;
	}

	// the file exists so we can create the new item now.
	item = this->CreateItem();
	item->SetStatus(OvalEnum::STATUS_EXISTS);
	item->AppendElement(new ItemEntity("path", path, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
	item->AppendElement(new ItemEntity("filename", fileName, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
	item->AppendElement(new ItemEntity("trustee_sid", trusteeSID, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));


	// Get the sid for the trustee name
	PSID pSid = WindowsCommon::GetSIDForTrusteeSID(trusteeSID);

	// check the sid
	if(!IsValidSid(pSid)) {
		LocalFree(pSD);
		throw ProbeException(baseErrMsg + " Invalid sid found.");

		//item->SetStatus(OvalEnum::STATUS_ERROR);
		//item->AppendMessage(new OvalMessage(baseErrMsg + " Invalid sid found.", OvalEnum::LEVEL_ERROR));
		//Log::Debug(baseErrMsg + " Invalid sid found.");
		//return item;
	}

	// build the trustee structure
	TRUSTEE trustee = {0};
	BuildTrusteeWithSid(&trustee, pSid);

	// get the rights
	PACCESS_MASK accessRights = NULL;
	accessRights = reinterpret_cast<PACCESS_MASK>(::LocalAlloc(LPTR, sizeof(PACCESS_MASK) + sizeof(ACCESS_MASK)));
	if(accessRights == NULL) {
		throw ProbeException(baseErrMsg + " Out of memory! Unable to allocate memory for access rights.");

		//item->SetStatus(OvalEnum::STATUS_ERROR);
		//item->AppendMessage(new OvalMessage(baseErrMsg + " Out of memory! Unable to allocate memory for access rights.", OvalEnum::LEVEL_ERROR));
		//Log::Debug(baseErrMsg + " Out of memory! Unable to allocate memory for access rights.");
		//return item;
	}

	res = GetEffectiveRightsFromAcl(pdacl,
									&trustee,
									accessRights);
	if (res != ERROR_SUCCESS) {
		
		string errMsg = WindowsCommon::GetErrorMessage(res);		

		LocalFree(pSD);
		LocalFree(accessRights);
		
		//item->SetStatus(OvalEnum::STATUS_ERROR);
		//item->AppendMessage(new OvalMessage(baseErrMsg + " System error message: " + errMsg, OvalEnum::LEVEL_ERROR));
		//Log::Debug(baseErrMsg + " System error message: " + errMsg);
		//return item;

		throw ProbeException(baseErrMsg + " System error message: " + errMsg); 		
	} 
		
	// Convert access mask to binary.
	// http://msdn2.microsoft.com/en-us/library/aa374896.aspx
	char mask[33];
	ZeroMemory(&mask, 33);

	for (int j = 0; j < 32; j++) {
		if ((*accessRights) & (1 << j))
			mask[j] = '1';
		else
			mask[j] = '0';
	}

	// need to seperatly determine if the generic bit should be set.
	// the access mask that is returned never has the generic bits set. 
	// Those bits can be determined by rolling up the object specific access bits
	if((*accessRights) & FILE_GENERIC_READ)
		mask[31] = '1';
	if((*accessRights) & FILE_GENERIC_WRITE)
		mask[30] = '1';
	if((*accessRights) & FILE_GENERIC_EXECUTE)
		mask[29] = '1';
	if((*accessRights) & FILE_ALL_ACCESS)
		mask[28] = '1';

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

	LocalFree(pSD);
	LocalFree(accessRights);

	return item;
}

bool FileEffectiveRights53Probe::ReportTrusteeSIDDoesNotExist(ObjectEntity *trusteeSID, StringVector* trusteeSIDs) {
	
	bool result = false;
	
	if(trusteeSID->GetOperation() == OvalEnum::OPERATION_EQUALS && !trusteeSID->GetNil()) {		
		
		if(trusteeSID->GetVarRef() == NULL) {
			if(this->TrusteeSIDExists(trusteeSID->GetValue(), WindowsCommon::GetAllTrusteeSIDs())) {
				trusteeSIDs = new StringVector();
				trusteeSIDs->push_back(trusteeSID->GetValue());
				result = true;
			}
		} else {

			trusteeSIDs = new StringVector();
			VariableValueVector::iterator iterator;
			for(iterator = trusteeSID->GetVarRef()->GetValues()->begin(); iterator != trusteeSID->GetVarRef()->GetValues()->end(); iterator++) {
				if(this->TrusteeSIDExists((*iterator)->GetValue(), WindowsCommon::GetAllTrusteeSIDs())) {
					trusteeSIDs->push_back((*iterator)->GetValue());
					result = true;
				}
			}
		}
	}

	return  result;
}
