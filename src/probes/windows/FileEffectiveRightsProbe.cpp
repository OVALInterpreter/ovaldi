//
// $Id: FileEffectiveRightsProbe.cpp 4579 2008-01-02 17:39:07Z bakerj $
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

#include "FileEffectiveRightsProbe.h"

/**
	NOTE: At this time we are not certain how to obtain all effective rights for a file.
	The code in the FileEffectiveRightsProbe::GetEffectiveRights attempts to do this.
	If you have any idea how to fix this issue send email to oval@mitre.org.

	Thanks in advance for your comments.
*/


//****************************************************************************************//
//								FileEffectiveRightsProbe Class							  //	
//****************************************************************************************//
FileEffectiveRightsProbe* FileEffectiveRightsProbe::instance = NULL;

FileEffectiveRightsProbe::FileEffectiveRightsProbe() {

}

FileEffectiveRightsProbe::~FileEffectiveRightsProbe() {

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

	ItemVector *collectedItems = new ItemVector();

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

					StringVector* trusteeNames = this->GetTrusteeNames(fp->first, fp->second, trusteeName);
					if(trusteeNames->size() > 0) {
						StringVector::iterator iterator;
						for(iterator = trusteeNames->begin(); iterator != trusteeNames->end(); iterator++) {
							try {
								Item* item = this->GetEffectiveRights(fp->first, fp->second, (*iterator));
								if(item != NULL) {
									collectedItems->push_back(item);
								}
							} catch (Exception ex) {
								Log::Debug("Error while getting effective rights for file. Path: " + fp->first + " Name: " + fp->second + " " + ex.GetErrorMessage());
							}
						}
						delete trusteeNames;

					} else {

						StringVector* trusteeNames = NULL;
						if(this->ReportTrusteeNameDoesNotExist(trusteeName, trusteeNames)) {

							StringVector::iterator iterator;
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
Item* FileEffectiveRightsProbe::CreateItem() {

	Item* item = new Item(0, 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows", 
						"win-sc", 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows windows-system-characteristics-schema.xsd", 
						OvalEnum::STATUS_ERROR, 
						"fileeffectiverights_item");

	return item;
}

StringVector* FileEffectiveRightsProbe::GetTrusteeNames(string path, string fileName, ObjectEntity* trusteeName) {
	// -----------------------------------------------------------------------
	//  ABSTRACT
	//
	//	Return the set of trustee names that are specified by the 
	//  ObjectEntity* trusteeName parameter.
	// -----------------------------------------------------------------------

	StringVector* trusteeNames = new StringVector();
	
	// load the set of all Trustee names
	StringVector* allTrusteeNames = WindowsCommon::GetAllTrusteeNames();

	// does this trusteeName use variables?
	if(trusteeName->GetVarRef() == NULL) {
		
		// proceed based on operation
		if(trusteeName->GetOperation() == OvalEnum::OPERATION_EQUALS) {
			
			// check that the trusee name exists
			if(this->TrusteeNameExists(trusteeName->GetValue(), allTrusteeNames)) {
				trusteeNames->push_back(trusteeName->GetValue());
			}

		} else if(trusteeName->GetOperation() == OvalEnum::OPERATION_NOT_EQUAL) {
			this->GetMatchingTrusteeNames(trusteeName->GetValue(), allTrusteeNames, trusteeNames, false);
		} else if(trusteeName->GetOperation() == OvalEnum::OPERATION_PATTERN_MATCH) {
			this->GetMatchingTrusteeNames(trusteeName->GetValue(), allTrusteeNames, trusteeNames, true);
		}		

	} else {
	
		// loop through all trustee names on the system
		// only keep those that match operation and value and var check
		StringVector::iterator it;
		ItemEntity* tmp = this->CreateItemEntity(trusteeName);
		for(it = allTrusteeNames->begin(); it != allTrusteeNames->end(); it++) {
			tmp->SetValue((*it));
			if(trusteeName->Analyze(tmp) == OvalEnum::RESULT_TRUE) {
				trusteeNames->push_back((*it));
			}
		}
	}

	// using common code to get these do not delete they are cached.
	//delete allTrusteeNames;

	return trusteeNames;
}

void FileEffectiveRightsProbe::GetMatchingTrusteeNames(string trusteeNamePattern, StringVector* allTrusteeNames, StringVector* trusteeNames, bool isRegex) {
	// -----------------------------------------------------------------------
	//
	//  ABSTRACT
	//
	//	Select all Trustees for the specified file that match the specified
	//	trusteeNamePattern. matching is done via regex or != depending on the
	//	isRegex boolean flag. All matches are added to the trusteeNames vector.
	// -----------------------------------------------------------------------

	StringVector::iterator iterator;
	for(iterator = allTrusteeNames->begin(); iterator != allTrusteeNames->end(); iterator++) {
		if(this->IsMatch(trusteeNamePattern, (*iterator), isRegex)) {
			trusteeNames->push_back((*iterator));
		}
	}
}

bool FileEffectiveRightsProbe::TrusteeNameExists(string trusteeName, StringVector* trusteeNames) {
	// -----------------------------------------------------------------------
	//
	//  ABSTRACT
	//
	//	Return a true if the specified trustee name exists in the set of all names.  
	// -----------------------------------------------------------------------

	bool exists = false;

	StringVector::iterator iterator;
	for(iterator = trusteeNames->begin(); iterator != trusteeNames->end(); iterator++) {
		if(trusteeName.compare((*iterator)) == 0) {
			exists = true;
		}
	}			

	return exists;
}

Item* FileEffectiveRightsProbe::GetEffectiveRights(string path, string fileName, string trusteeName) {
	// -----------------------------------------------------------------------
	//
	//  ABSTRACT
	//
	//	Return a populated item for the specified trustess on the specified file.
	//
	//	- Use provided trustee name and call LsaLookupNames to get the sid
	//	  http://msdn.microsoft.com/library/default.asp?url=/library/en-us/secmgmt/security/lsalookupnames.asp
	//	- Then call GetNamedSecurityInfo to get a DACL Security Descriptor for the file
	//	  http://msdn.microsoft.com/library/default.asp?url=/library/en-us/secauthz/security/getnamedsecurityinfo.asp
	//	- Then call GetEffectiveRightsFromAcl with the dacl and the sid found in the earlier calls
	//	  http://msdn.microsoft.com/library/default.asp?url=/library/en-us/secauthz/security/geteffectiverightsfromacl.asp
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

	// elevate privs - this should not be needed and has no effect on the 
	//	rights obtained by the api calls below
	//TOKEN_PRIVILEGES tokPrivileges;
	//HANDLE hProcess = GetCurrentProcess();
	//HANDLE hToken = NULL;
	//LUID secNameLUID;
	//OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
	//LookupPrivilegeValue(NULL, SE_SECURITY_NAME, &secNameLUID);
	//tokPrivileges.PrivilegeCount = 1;
	//tokPrivileges.Privileges[0].Luid = secNameLUID;
	//tokPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	//BOOL  result = AdjustTokenPrivileges(hToken, FALSE, &tokPrivileges, NULL, NULL, NULL);
	//if(!result) {
	//	cout << "Unable to elevate privilages" << endl;
	//} else {
	//	cout << "elevated" << endl;
	//}
	//

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
			// should never get here. before calling this function the file should already have 
			// been checked for existence.
			throw ProbeException("Error unable locate " + filePath + " while getting trustte names."); 
		} else {
			throw ProbeException("Error unable to retrieve a copy of the security descriptor for " + filePath + " while getting trustee names."); 
		}
	} 

	// Check to see if a valid security descriptor were returned.  
    if ((IsValidSecurityDescriptor(pSD) == 0) || (IsValidAcl(pdacl) == 0)) {
		LocalFree(pSD);
		throw ProbeException("Error invalid data returned from call to GetNamedSecurityInfo(). Cannot get effective rights for file: " + filePath);
	}

	// Get the sid for the trustee name
	PSID pSid = WindowsCommon::GetSIDForTrusteeName(trusteeName);

	// check the sid
	if(!IsValidSid(pSid)) {
		LocalFree(pSD);
		throw ProbeException("Invalid sid for trustee: " +  trusteeName);
	}

	// build the trustee structure
	TRUSTEE trustee = {0};
	BuildTrusteeWithSid(&trustee, pSid);

	// get the rights
	PACCESS_MASK accessRights = NULL;
	accessRights = reinterpret_cast<PACCESS_MASK>(::LocalAlloc(LPTR, sizeof(PACCESS_MASK) + sizeof(ACCESS_MASK)));
	if(accessRights == NULL) {
		ProbeException("Out of memory");
	}

	res = GetEffectiveRightsFromAcl(pdacl,
									&trustee,
									accessRights);
	if (res != ERROR_SUCCESS) {
		LocalFree(pSD);
		throw ProbeException("Error unable to get effective rights for trustee: " + trusteeName + " from dacl for file: " + filePath); 
	} 
		
	// Convert access mask to binary.
	// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/secauthz/security/access_mask_format.asp
	int j;
	char mask[33];

	for (j = 0; j < 32; j++) {
		if ((*accessRights) & (1 << j))
			mask[31 - j] = '1';
		else
			mask[31 - j] = '0';
	}
	mask[32] = 0;

	item = this->CreateItem();
	item->SetStatus(OvalEnum::STATUS_EXISTS);
	item->AppendElement(new ItemEntity("path", path, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
	item->AppendElement(new ItemEntity("filename", fileName, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
	item->AppendElement(new ItemEntity("trustee_name", trusteeName, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));

	// removed for version 5.2 of the oval schema
	//string domainStr = "";
	//string sidStr = "";
	//bool isGroup = WindowsCommon::LookUpTrusteeName(&trusteeName, &sidStr, &domainStr);
	//item->AppendElement(new ItemEntity("trustee_domain", domainStr, OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));
	//item->AppendElement(new ItemEntity("trustee_sid", sidStr, OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));

	// read values in the access_mask
	// http://windowssdk.msdn.microsoft.com/en-us/library/ms717896.aspx
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

	return item;
}

bool FileEffectiveRightsProbe::ReportTrusteeNameDoesNotExist(ObjectEntity *trusteeName, StringVector* trusteeNames) {
	
	bool result = false;
	
	if(trusteeName->GetOperation() == OvalEnum::OPERATION_EQUALS && !trusteeName->GetNil()) {		
		
		if(trusteeName->GetVarRef() == NULL) {
			if(this->TrusteeNameExists(trusteeName->GetValue(), WindowsCommon::GetAllTrusteeNames())) {
				trusteeNames = new StringVector();
				trusteeNames->push_back(trusteeName->GetValue());
				result = true;
			}
		} else {

			trusteeNames = new StringVector();
			VariableValueVector::iterator iterator;
			for(iterator = trusteeName->GetVarRef()->GetValues()->begin(); iterator != trusteeName->GetVarRef()->GetValues()->end(); iterator++) {
				if(this->TrusteeNameExists((*iterator)->GetValue(), WindowsCommon::GetAllTrusteeNames())) {
					trusteeNames->push_back((*iterator)->GetValue());
					result = true;
				}
			}
		}
	}

	return  result;
}
