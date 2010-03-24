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

#include "FileProbe.h"

//****************************************************************************************//
//								FileProbe Class											  //	
//****************************************************************************************//
FileProbe* FileProbe::instance = NULL;

FileProbe::FileProbe() {

}

FileProbe::~FileProbe() {
  instance = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* FileProbe::Instance() {

	// Use lazy initialization
	if(instance == NULL) 
		instance = new FileProbe();

	return instance;	
}

ItemVector* FileProbe::CollectItems(Object* object) {

	ItemVector *collectedItems = new ItemVector();

	// get the path, filename, and filepath
	ObjectEntity* path = object->GetElementByName("path");
	ObjectEntity* fileName = object->GetElementByName("filename");
    ObjectEntity* filePath = object->GetElementByName("filepath");
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
			string filepath = Common::BuildFilePath(fp->first,fp->second);

			if(fp->second.compare("") == 0) {

				Item* item = NULL;

				// check if the code should report that the filename does not exist.
				StringVector fileNames;
				if(fileFinder.ReportFileNameDoesNotExist(fp->first, fileName, &fileNames)) {

					StringVector::iterator iterator;
					for(iterator = fileNames.begin(); iterator != fileNames.end(); iterator++) {

						item = this->CreateItem();
						item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
						item->AppendElement(new ItemEntity("filepath",Common::BuildFilePath(fp->first,*iterator),OvalEnum::DATATYPE_STRING,true,OvalEnum::STATUS_DOES_NOT_EXIST));
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
				Item* item = this->GetFileAttributes(fp->first, fp->second);
				if(item != NULL) {
					collectedItems->push_back(item);
				}
				item = NULL;
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
Item* FileProbe::CreateItem() {

	Item* item = new Item(0, 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows", 
						"win-sc", 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows windows-system-characteristics-schema.xsd", 
						OvalEnum::STATUS_ERROR, 
						"file_item");

	return item;
}

Item* FileProbe::GetFileAttributes(string path, string fileName) {

	HANDLE hFile;
	DWORD res;
	char buf[512];

	Item *item = NULL;

	string filePath = Common::BuildFilePath((const string)path, (const string)fileName);

	try {

		//////////////////////////////////////////////////////
		//////////////////////  Owner  ///////////////////////
		//////////////////////////////////////////////////////

		// Get the handle of the object.
		//
		// SMC-AUDIT: ISSUE: should probably verify that this is a regular file before opening,
		// instead of a virtual memory file!
		
		hFile = CreateFile(filePath.c_str(),			// file name
							GENERIC_READ,				// access mode
							FILE_SHARE_READ,			// share mode
							NULL,						// SD
							OPEN_EXISTING,				// how to create
							FILE_ATTRIBUTE_NORMAL,		// file attributes
							NULL);						// handle to template file

		if (hFile == INVALID_HANDLE_VALUE) {

			DWORD errorNum = GetLastError();
			string sysErrMsg = WindowsCommon::GetErrorMessage(errorNum);

			if(errorNum == ERROR_FILE_NOT_FOUND || errorNum == ERROR_PATH_NOT_FOUND) {
				
				string errorMessage = "";
				errorMessage.append("(FileProbe) Unable to open a handle to the file: '");
				errorMessage.append(filePath);
				errorMessage.append("'. This error should never occur since the file has already be confirmed to exist on the system. " + sysErrMsg);
				throw ProbeException(errorMessage);
				
			} else if(errorNum == ERROR_PATH_NOT_FOUND) {

				string errorMessage = "";
				errorMessage.append("(FileProbe) Unable to open a handle to the file: '");
				errorMessage.append(filePath);
				errorMessage.append("'. This error should never occur since the file has already be confirmed to exist on the system. " + sysErrMsg);
				throw ProbeException(errorMessage);

			} else {

				string errorMessage = "";
				errorMessage.append("(FileProbe) Unable to open a handle to the file: '");
				errorMessage.append(filePath);
				errorMessage.append("'.  " + sysErrMsg);
				throw ProbeException(errorMessage);
			}
		}

		// add the path and file name
		item = this->CreateItem();
		item->SetStatus(OvalEnum::STATUS_EXISTS);
		item->AppendElement(new ItemEntity("filepath", filePath, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
		item->AppendElement(new ItemEntity("path", path, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
		item->AppendElement(new ItemEntity("filename", fileName, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
		ItemEntity* owner = new ItemEntity("owner", "", OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_ERROR);
		item->AppendElement(owner);

		//////////////////////////////////////////////////////
		/////////////////////   Owner    /////////////////////
		//////////////////////////////////////////////////////
		try {

			PSID owner_sid;
			PSECURITY_DESCRIPTOR p_sd;

			// Get the owner SID of the file.
			res = GetSecurityInfo(hFile,						// handle to object
								  SE_FILE_OBJECT,				// object type
								  OWNER_SECURITY_INFORMATION,	// information type
								  &owner_sid,					// owner SID
								  NULL,							// primary group SID
								  NULL,							// DACL
								  NULL,							// SACL
								  &p_sd);						// SD

			if (res != ERROR_SUCCESS) {

				string errorMessage = "";
				errorMessage.append("(FileProbe) Unable to get the security descriptor for the file: '");
				errorMessage.append(filePath);
				errorMessage.append("'.");
				throw ProbeException(errorMessage, ERROR_NOTICE);
			}

			// First call to LookupAccountSid to get the buffer sizes.
			LPTSTR aname = NULL;
			LPTSTR dname = NULL;
			DWORD dwaname = 0;
			DWORD dwdname = 0;
			SID_NAME_USE eUse;

			res = LookupAccountSid(NULL,					// name of local or remote computer
								   owner_sid,				// security identifier
								   aname,					// account name buffer
								   (LPDWORD)&dwaname,		// size of account name buffer
								   dname,					// domain name
								   (LPDWORD)&dwdname,		// size of domain name buffer
								   &eUse);					// SID type

			// Reallocate memory for the buffers.
			aname = (LPTSTR) malloc(dwaname);
			if (aname == NULL) {

				string errorMessage = "";
				errorMessage.append("(FileProbe) Could not allocate space for the owner name for file: '");
				errorMessage.append(filePath);
				errorMessage.append("'.");
				throw ProbeException(errorMessage);
			}

			dname = (LPTSTR) malloc(dwdname);
			if (dname == NULL) {

				free(aname);

				string errorMessage = "";
				errorMessage.append("(FileProbe) Could not allocate space for the domain name for the file: '");
				errorMessage.append(filePath);
				errorMessage.append("'.");
				throw ProbeException(errorMessage);
			}

			// Second call to LookupAccountSid to get the account name.
			res = LookupAccountSid(NULL,					// name of local or remote computer
								   owner_sid,				// security identifier
								   aname,					// account name buffer
								   (LPDWORD)&dwaname,		// size of account name buffer
								   dname,					// domain name
								   (LPDWORD)&dwdname,		// size of domain name buffer
								   &eUse);					// SID type

			if (res == FALSE) {

				free(aname);
				free(dname);

				string errorMessage = "";
				errorMessage.append("(FileProbe) Unable to get the name of the account ");
				errorMessage.append("for this SID for the file: '");
				errorMessage.append(filePath);
				errorMessage.append("'.");
				throw ProbeException(errorMessage);
			}
			
			//	Set owner (use domain\username format)
			string ownerStr;
			ownerStr.append(dname);
			ownerStr.append("\\");
			ownerStr.append(aname);
			owner->SetValue(ownerStr);
			owner->SetStatus(OvalEnum::STATUS_EXISTS);
			free(aname);
			free(dname);

		} catch(ProbeException ex) {	
			item->AppendMessage(new OvalMessage("Message: " + ex.GetErrorMessage()));
			owner->SetValue("");
			owner->SetStatus(OvalEnum::STATUS_ERROR);

		} catch(...) {
			
			string errorMessage = "";
			errorMessage.append("(FileProbe) Unknown error attempting to get the owner of the file '");
			errorMessage.append(filePath);
			errorMessage.append("' ");
			item->AppendMessage(new OvalMessage(errorMessage));
			owner->SetValue("");
			owner->SetStatus(OvalEnum::STATUS_ERROR);
		}

		//////////////////////////////////////////////////////
		/////////////////////  FileSize  /////////////////////
		//////////////////////////////////////////////////////

		struct _stat statusBuffer;
		int result;
 
		// Get status information associated with the file.
		result = _stat(filePath.c_str(), &statusBuffer);
		if (result < 0) {
			string errorMessage = "";
			errorMessage.append("(FileProbe) Unable to get status information ");
			errorMessage.append("associated with the file: '");
			errorMessage.append(filePath);
			errorMessage.append("'.");
			
			item->AppendElement(new ItemEntity("size", "", OvalEnum::DATATYPE_INTEGER, false, OvalEnum::STATUS_ERROR));
			item->AppendMessage(new OvalMessage(errorMessage));
			
		} else {

			// Add file size.
			ZeroMemory(buf, sizeof(buf));
			_snprintf(buf, sizeof(buf)-1, "%ld", statusBuffer.st_size);
			buf[sizeof(buf)-1] = '\0';
			item->AppendElement(new ItemEntity("size", buf, OvalEnum::DATATYPE_INTEGER, false, OvalEnum::STATUS_EXISTS));
		}


		//////////////////////////////////////////////////////
		/////////////////////  File Times  ///////////////////
		//////////////////////////////////////////////////////
		FILETIME creationTime;
		FILETIME lastAccessTime;
		FILETIME writeTime;

		BOOL timeRes = GetFileTime(	hFile,
									&creationTime,   //c_time
									&lastAccessTime, //a_time
									&writeTime);     //m_time

		if(!timeRes) {

			ItemEntity* aTime = new ItemEntity("a_time",  "", OvalEnum::DATATYPE_INTEGER, false, OvalEnum::STATUS_ERROR);
			ItemEntity* cTime = new ItemEntity("c_time",  "", OvalEnum::DATATYPE_INTEGER, false, OvalEnum::STATUS_ERROR);
			ItemEntity* mTime = new ItemEntity("m_time",  "", OvalEnum::DATATYPE_INTEGER, false, OvalEnum::STATUS_ERROR);
			string lastError = WindowsCommon::GetErrorMessage(GetLastError());
			item->AppendMessage(new OvalMessage("Unable to file times for file. " + lastError, OvalEnum::LEVEL_ERROR));

		} else {
			
			//////////////////////////////////////////////////////
			/////////////////////  Accessed  /////////////////////
			//////////////////////////////////////////////////////
			ItemEntity* aTime = new ItemEntity("a_time", WindowsCommon::ToString(lastAccessTime), OvalEnum::DATATYPE_INTEGER, false, OvalEnum::STATUS_EXISTS);
			item->AppendElement(aTime);

			//////////////////////////////////////////////////////
			/////////////////////  Created  /////////////////////
			//////////////////////////////////////////////////////
			ItemEntity* cTime = new ItemEntity("c_time", WindowsCommon::ToString(creationTime), OvalEnum::DATATYPE_INTEGER, false, OvalEnum::STATUS_EXISTS);
			item->AppendElement(cTime);

			//////////////////////////////////////////////////////
			/////////////////////  Modified  /////////////////////
			//////////////////////////////////////////////////////
			ItemEntity* mTime = new ItemEntity("m_time", WindowsCommon::ToString(writeTime), OvalEnum::DATATYPE_INTEGER, false, OvalEnum::STATUS_EXISTS);
			item->AppendElement(mTime);

		}

		//////////////////////////////////////////////////////
		////////////////////  MSChecksum  ////////////////////
		//////////////////////////////////////////////////////

		DWORD headersum;  
		DWORD checksum;

		res = MapFileAndCheckSum(const_cast<char*>(filePath.c_str()), &headersum, &checksum);
		if (res != CHECKSUM_SUCCESS) {
			string errorMessage = "";
			errorMessage.append("(FileProbe) Unable to get ms_checksum information for the file: '");
			errorMessage.append(filePath);
			errorMessage.append("'");
			item->AppendElement(new ItemEntity("ms_checksum", "", OvalEnum::DATATYPE_INTEGER, false, OvalEnum::STATUS_ERROR));
			item->AppendMessage(new OvalMessage(errorMessage));

		} else {
			ZeroMemory(buf, sizeof(buf));
			_snprintf(buf, sizeof(buf)-1, "%d", checksum);
			buf[sizeof(buf)-1] = '\0';
			item->AppendElement(new ItemEntity("ms_checksum", buf, OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));
		}

		// initialize remaining version information entities...
		ItemEntity* version = new ItemEntity("version", "", OvalEnum::DATATYPE_VERSION, false, OvalEnum::STATUS_ERROR);
		item->AppendElement(version);
		ItemEntity* type = new ItemEntity("type", "", OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_ERROR);
		item->AppendElement(type);
		ItemEntity* devClass = new ItemEntity("development_class", "", OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_ERROR);
		item->AppendElement(devClass);
		ItemEntity* company = new ItemEntity("company", "", OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_ERROR);	
		item->AppendElement(company);
		ItemEntity* internalName = new ItemEntity("internal_name", "", OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_ERROR);
		item->AppendElement(internalName);
		ItemEntity* language = new ItemEntity("language", "", OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_ERROR);
		item->AppendElement(language);
		ItemEntity* originalFilename = new ItemEntity("original_filename", "", OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_ERROR);
		item->AppendElement(originalFilename);
		ItemEntity* productName = new ItemEntity("product_name", "", OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_ERROR);
		item->AppendElement(productName);
		ItemEntity* productVersion = new ItemEntity("product_version", "", OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_ERROR);
		item->AppendElement(productVersion);
		
		DWORD junk;
		DWORD versionsize;
		LPVOID versionbuf;

		// Get the required size of the version info buffer.
		versionsize = GetFileVersionInfoSize(filePath.c_str(), &junk);
		if (versionsize > 0) {

			versionbuf = (LPVOID)malloc(versionsize);
			if (GetFileVersionInfo(filePath.c_str(), 0, versionsize, versionbuf) == TRUE) {

				//////////////////////////////////////////////////////
				////////////////////    version    ///////////////////
				//////////////////////////////////////////////////////

				this->GetVersion(versionbuf, filePath, item, version);			

				//////////////////////////////////////////////////////
				///////////////  development_class  //////////////////
				//////////////////////////////////////////////////////

				this->GetDevelopementClass(versionbuf, filePath, item, devClass);

				//////////////////////////////////////////////////////
				///////////////       company       //////////////////
				//////////////////////////////////////////////////////

				this->GetCompany(versionbuf, filePath, item, company);

				//////////////////////////////////////////////////////
				///////////////    internal_name    //////////////////
				//////////////////////////////////////////////////////

				this->GetInternalName(versionbuf, filePath, item, internalName);

				//////////////////////////////////////////////////////
				///////////////       language      //////////////////
				//////////////////////////////////////////////////////

				this->GetLanguage(versionbuf, filePath, item, language);

				//////////////////////////////////////////////////////
				///////////////  original_filename  //////////////////
				//////////////////////////////////////////////////////

				this->GetOriginalFilename(versionbuf, filePath, item, originalFilename);

				//////////////////////////////////////////////////////
				///////////////     product_name    //////////////////
				//////////////////////////////////////////////////////

				this->GetProductName(versionbuf, filePath, item, productName);

				//////////////////////////////////////////////////////
				///////////////   product_version   //////////////////
				//////////////////////////////////////////////////////

				this->GetProductVersion(versionbuf, filePath, item, productVersion);

			} else {
				string errorMessage = "";
				errorMessage.append("(FileProbe) Unable to get version info for the file: '");
				errorMessage.append(filePath);
				errorMessage.append("'");
				item->AppendMessage(new OvalMessage(errorMessage));
			}

			free(versionbuf);

		} else {

			string errorMessage = "";
			errorMessage.append("(FileProbe) No version information available for the file: '");
			errorMessage.append(filePath);
			errorMessage.append("'");
			item->AppendMessage(new OvalMessage(errorMessage));
		}

		//////////////////////////////////////////////////////
		/////////////////////  FileType  /////////////////////
		//////////////////////////////////////////////////////

		this->GetType(hFile, filePath, item, type);

		//////////////////////////////////////////////////////
		//////////////////////////////////////////////////////

		//Close the file handle
		if ( CloseHandle(hFile) == 0 ){
			Log::Debug("Error: Unable to close the handle for "+filePath);
		}

	} catch(ProbeException ex) {	
	
		Log::Debug(ex.GetErrorMessage());

	} catch(...) {	
	
		string errMsg = "";
		errMsg.append("(FileProbe) Unknown error attempting to get file attribute information for the file: '");
		errMsg.append(filePath);
		errMsg.append("'");
		Log::Debug(errMsg);
		
	}

	return item;
}

void FileProbe::GetVersion(LPVOID versionbuf, string filePath, Item *item, ItemEntity* version) {

	VS_FIXEDFILEINFO* pFFI = NULL;
	UINT vdatalen;

	//	Get The major, minor, private and build numbers for the file
	if (VerQueryValue(versionbuf, "\\", (void**)&pFFI, &vdatalen) == FALSE) {

		string errorMessage = "";
		errorMessage.append("(FileProbe) Unable to get version information for the file '");
		errorMessage.append(filePath);
		errorMessage.append("'");
		item->AppendMessage(new OvalMessage(errorMessage));

	} else if (pFFI == NULL) {

		string errorMessage = "";
		errorMessage.append("(FileProbe) No version information available for the file: '");
		errorMessage.append(filePath);
		errorMessage.append("'");
		version->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
		item->AppendMessage(new OvalMessage(errorMessage));

	} else {

		char ver1[16];
		char ver2[16];
		char ver3[16];
		char ver4[16];

		// get the file version data 
		ZeroMemory(ver1, sizeof(ver1));
		ZeroMemory(ver2, sizeof(ver2));
		ZeroMemory(ver3, sizeof(ver3));
		ZeroMemory(ver4, sizeof(ver4));

		_snprintf(ver1, sizeof(ver1)-1, "%d", (HIWORD(pFFI->dwFileVersionMS)));
		_snprintf(ver2, sizeof(ver2)-1, "%d", (LOWORD(pFFI->dwFileVersionMS)));
		_snprintf(ver3, sizeof(ver3)-1, "%d", (HIWORD(pFFI->dwFileVersionLS)));
		_snprintf(ver4, sizeof(ver4)-1, "%d", (LOWORD(pFFI->dwFileVersionLS)));

		string versionStr = "";
		versionStr.append(ver1);
		versionStr.append(".");
		versionStr.append(ver2);
		versionStr.append(".");
		versionStr.append(ver3);
		versionStr.append(".");
		versionStr.append(ver4);
		version->SetValue(versionStr);
		version->SetStatus(OvalEnum::STATUS_EXISTS);
	}
}

void FileProbe::GetType(HANDLE hFile, string filePath, Item *item, ItemEntity* type) {

	DWORD res = GetFileType(hFile);

	BOOL gfaRes;
	WIN32_FILE_ATTRIBUTE_DATA lpFileInformation;

	switch (res) {

		case FILE_TYPE_DISK:

			gfaRes = GetFileAttributesEx(filePath.c_str(),				// file or directory name
										 GetFileExInfoStandard,			// attribute class
										 (LPVOID)&lpFileInformation);	// attribute information 

			if (lpFileInformation.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
				type->SetValue("FILE_ATTRIBUTE_DIRECTORY");
			else
					type->SetValue("FILE_TYPE_DISK");

			type->SetStatus(OvalEnum::STATUS_EXISTS);

			break;

		case FILE_TYPE_CHAR:

			type->SetValue("FILE_TYPE_CHAR");
			type->SetStatus(OvalEnum::STATUS_EXISTS);
			break;

		case FILE_TYPE_PIPE:

			type->SetValue("FILE_TYPE_PIPE");
			type->SetStatus(OvalEnum::STATUS_EXISTS);
			break;

		case FILE_TYPE_UNKNOWN:
			{
				string errorMessage = "";
				errorMessage.append("(FileProbe) No file type information available for the file: '");
				errorMessage.append(filePath);
				errorMessage.append("'");
				item->AppendMessage(new OvalMessage(errorMessage));
				type->SetStatus(OvalEnum::STATUS_ERROR);
			}
			
			break;

		default:
			{
				string errorMessage = "";
				errorMessage.append("(FileProbe) Unable to get file type information for the file: '");
				errorMessage.append(filePath);
				errorMessage.append("'");
				item->AppendMessage(new OvalMessage(errorMessage));
				type->SetStatus(OvalEnum::STATUS_ERROR);
			}
			
			break;
	}
}

void FileProbe::GetDevelopementClass(LPVOID versionbuf, string filePath, Item *item, ItemEntity* devClass) {

	//	Get the language-code page and construct the string for file version request
	UINT vdatalen;
	DWORD *lpTransArray;
	TCHAR szSubblock[80];
	TCHAR szSubblockHeader[25];
	int retVal = VerQueryValue(versionbuf,  
				  TEXT("\\VarFileInfo\\Translation"),
				  (LPVOID*)&lpTransArray,
				  &vdatalen);

	if(retVal != 0) {
		
		//	Convert the code page info into a zero-terminated
		//	string specifying which version-information value to retrieve
		_stprintf(szSubblockHeader, TEXT("\\StringFileInfo\\%04X%04X"), LOWORD(lpTransArray[0]), HIWORD(lpTransArray[0]));					
		_stprintf(szSubblock, TEXT("%s\\%s"), szSubblockHeader, TEXT("FileVersion"));

		//	Get the file's developement class
		LPTSTR lpszValue;
		retVal = VerQueryValue(versionbuf,
								szSubblock, 
								(LPVOID *)&lpszValue, 
								&vdatalen);
					  
		if(retVal != 0) {

			//	Check to see if the version string has a developement path string in it
			string verStr = lpszValue;
			REGEX verMatcher;
			if(verMatcher.IsMatch(".+\\([^\\)].+\\)", verStr.c_str())) {

				//	Parse the version string
				verStr = verStr.substr(verStr.find("(") + 1);
				devClass->SetValue(verStr.substr(0, verStr.find(".")));
				devClass->SetStatus(OvalEnum::STATUS_EXISTS);
				
			} else {
				
				devClass->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
			}

		} else if(vdatalen == 0) {

			string errorMessage = "";
			errorMessage.append("(FileProbe) Unable to get development_class. No value is available for the specified version-information name, \"szSubblock,\" for file: ");
			errorMessage.append(filePath);
			errorMessage.append("'");
			item->AppendMessage(new OvalMessage(errorMessage));
			devClass->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);

		} else if(retVal == 0) {

			string errorMessage = "";
			errorMessage.append("(FileProbe) Unable to get development_class. Either specified name, \"szSubblock,\" does not exist or the specified resource is not valid for file: ");
			errorMessage.append(filePath);
			errorMessage.append("'");
			item->AppendMessage(new OvalMessage(errorMessage));
			devClass->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
		}

	} else if(vdatalen == 0) {

		string errorMessage = "";
		errorMessage.append("(FileProbe) Unable to get development_class. No value is available for the specified version-information name, \"szSubblock,\" for file: ");
		errorMessage.append(filePath);
		errorMessage.append("'");
		item->AppendMessage(new OvalMessage(errorMessage));
		devClass->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);

	} else if(retVal == 0) {

		string errorMessage = "";
		errorMessage.append("(FileProbe) Unable to get development_class. Either specified name, \"szSubblock,\" does not exist or the specified resource is not valid for file: ");
		errorMessage.append(filePath);
		errorMessage.append("'");
		item->AppendMessage(new OvalMessage(errorMessage));
		devClass->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
	}
}

void FileProbe::GetCompany(LPVOID versionbuf, string filePath, Item *item, ItemEntity* company) {

	//	Get the language-code page and construct the string for file version request
	UINT vdatalen;
	DWORD *lpTransArray;
	TCHAR szSubblock[80];
	TCHAR szSubblockHeader[25];
	int retVal = VerQueryValue(versionbuf,  
				  TEXT("\\VarFileInfo\\Translation"),
				  (LPVOID*)&lpTransArray,
				  &vdatalen);

	if(retVal != 0) {
		
		//	Convert the code page info into a zero-terminated
		//	string specifying which version-information value to retrieve
		_stprintf(szSubblockHeader, TEXT("\\StringFileInfo\\%04X%04X"), LOWORD(lpTransArray[0]), HIWORD(lpTransArray[0]));					
		_stprintf(szSubblock, TEXT("%s\\%s"), szSubblockHeader, TEXT("CompanyName"));

		//	Get the file's developement class
		LPTSTR lpszValue;
		retVal = VerQueryValue(versionbuf,
								szSubblock, 
								(LPVOID *)&lpszValue, 
								&vdatalen);
					  
		if(retVal != 0) {

			string companyNameStr = lpszValue;
			if(companyNameStr.compare("") != 0) {
				company->SetValue(companyNameStr);
				company->SetStatus(OvalEnum::STATUS_EXISTS);
			} else {
				company->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
			}

		} else if(vdatalen == 0) {

			string errorMessage = "";
			errorMessage.append("(FileProbe) Unable to get company. No value is available for the specified version-information name, \"szSubblock,\" for file: ");
			errorMessage.append(filePath);
			errorMessage.append("'");
			item->AppendMessage(new OvalMessage(errorMessage));
			company->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);

		} else if(retVal == 0) {

			string errorMessage = "";
			errorMessage.append("(FileProbe) Unable to get company. Either specified name, \"szSubblock,\" does not exist or the specified resource is not valid for file: ");
			errorMessage.append(filePath);
			errorMessage.append("'");
			item->AppendMessage(new OvalMessage(errorMessage));
			company->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
		}

	} else if(vdatalen == 0) {

		string errorMessage = "";
		errorMessage.append("(FileProbe) Unable to get company. No value is available for the specified version-information name, \"szSubblock,\" for file: ");
		errorMessage.append(filePath);
		errorMessage.append("'");
		item->AppendMessage(new OvalMessage(errorMessage));
		company->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);

	} else if(retVal == 0) {

		string errorMessage = "";
		errorMessage.append("(FileProbe) Unable to get company. Either specified name, \"szSubblock,\" does not exist or the specified resource is not valid for file: ");
		errorMessage.append(filePath);
		errorMessage.append("'");
		item->AppendMessage(new OvalMessage(errorMessage));
		company->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
	}
}

void FileProbe::GetInternalName(LPVOID versionbuf, string filePath, Item *item, ItemEntity* internalName) {

	//	Get the language-code page and construct the string for file version request
	UINT vdatalen;
	DWORD *lpTransArray;
	TCHAR szSubblock[80];
	TCHAR szSubblockHeader[25];
	int retVal = VerQueryValue(versionbuf,  
				  TEXT("\\VarFileInfo\\Translation"),
				  (LPVOID*)&lpTransArray,
				  &vdatalen);

	if(retVal != 0) {
		
		//	Convert the code page info into a zero-terminated
		//	string specifying which version-information value to retrieve
		_stprintf(szSubblockHeader, TEXT("\\StringFileInfo\\%04X%04X"), LOWORD(lpTransArray[0]), HIWORD(lpTransArray[0]));					
		_stprintf(szSubblock, TEXT("%s\\%s"), szSubblockHeader, TEXT("InternalName"));

		//	Get the file's developement class
		LPTSTR lpszValue;
		retVal = VerQueryValue(versionbuf,
								szSubblock, 
								(LPVOID *)&lpszValue, 
								&vdatalen);
					  
		if(retVal != 0) {

			string internalNameStr = lpszValue;
			if(internalNameStr.compare("") != 0) {
				internalName->SetValue(internalNameStr);
				internalName->SetStatus(OvalEnum::STATUS_EXISTS);
			} else {
				internalName->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
			}

		} else if(vdatalen == 0) {

			string errorMessage = "";
			errorMessage.append("(FileProbe) Unable to get internal_name. No value is available for the specified version-information name, \"szSubblock,\" for file: ");
			errorMessage.append(filePath);
			errorMessage.append("'");
			item->AppendMessage(new OvalMessage(errorMessage));
			internalName->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);

		} else if(retVal == 0) {

			string errorMessage = "";
			errorMessage.append("(FileProbe) Unable to get internal_name. Either specified name, \"szSubblock,\" does not exist or the specified resource is not valid for file: ");
			errorMessage.append(filePath);
			errorMessage.append("'");
			item->AppendMessage(new OvalMessage(errorMessage));
			internalName->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
		}

	} else if(vdatalen == 0) {

		string errorMessage = "";
		errorMessage.append("(FileProbe) Unable to get internal_name. No value is available for the specified version-information name, \"szSubblock,\" for file: ");
		errorMessage.append(filePath);
		errorMessage.append("'");
		item->AppendMessage(new OvalMessage(errorMessage));
		internalName->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);

	} else if(retVal == 0) {

		string errorMessage = "";
		errorMessage.append("(FileProbe) Unable to get internal_name. Either specified name, \"szSubblock,\" does not exist or the specified resource is not valid for file: ");
		errorMessage.append(filePath);
		errorMessage.append("'");
		item->AppendMessage(new OvalMessage(errorMessage));
		internalName->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
	}
}

void FileProbe::GetLanguage(LPVOID versionbuf, string filePath, Item *item, ItemEntity* language) {

	language->SetStatus(OvalEnum::STATUS_NOT_COLLECTED);
}

void FileProbe::GetOriginalFilename(LPVOID versionbuf, string filePath, Item *item, ItemEntity* originalFilename) {

	//	Get the language-code page and construct the string for file version request
	UINT vdatalen;
	DWORD *lpTransArray;
	TCHAR szSubblock[80];
	TCHAR szSubblockHeader[25];
	int retVal = VerQueryValue(versionbuf,  
				  TEXT("\\VarFileInfo\\Translation"),
				  (LPVOID*)&lpTransArray,
				  &vdatalen);

	if(retVal != 0) {
		
		//	Convert the code page info into a zero-terminated
		//	string specifying which version-information value to retrieve
		_stprintf(szSubblockHeader, TEXT("\\StringFileInfo\\%04X%04X"), LOWORD(lpTransArray[0]), HIWORD(lpTransArray[0]));					
		_stprintf(szSubblock, TEXT("%s\\%s"), szSubblockHeader, TEXT("OriginalFilename"));

		//	Get the file's developement class
		LPTSTR lpszValue;
		retVal = VerQueryValue(versionbuf,
								szSubblock, 
								(LPVOID *)&lpszValue, 
								&vdatalen);
					  
		if(retVal != 0) {

			string originalFilenameStr = lpszValue;
			if(originalFilenameStr.compare("") != 0) {
				originalFilename->SetValue(originalFilenameStr);
				originalFilename->SetStatus(OvalEnum::STATUS_EXISTS);
			} else {
				originalFilename->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
			}

		} else if(vdatalen == 0) {

			string errorMessage = "";
			errorMessage.append("(FileProbe) Unable to get original_filename. No value is available for the specified version-information name, \"szSubblock,\" for file: ");
			errorMessage.append(filePath);
			errorMessage.append("'");
			item->AppendMessage(new OvalMessage(errorMessage));
			originalFilename->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);

		} else if(retVal == 0) {

			string errorMessage = "";
			errorMessage.append("(FileProbe) Unable to get original_filename. Either specified name, \"szSubblock,\" does not exist or the specified resource is not valid for file: ");
			errorMessage.append(filePath);
			errorMessage.append("'");
			item->AppendMessage(new OvalMessage(errorMessage));
			originalFilename->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
		}

	} else if(vdatalen == 0) {

		string errorMessage = "";
		errorMessage.append("(FileProbe) Unable to get original_filename. No value is available for the specified version-information name, \"szSubblock,\" for file: ");
		errorMessage.append(filePath);
		errorMessage.append("'");
		item->AppendMessage(new OvalMessage(errorMessage));
		originalFilename->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);

	} else if(retVal == 0) {

		string errorMessage = "";
		errorMessage.append("(FileProbe) Unable to get original_filename. Either specified name, \"szSubblock,\" does not exist or the specified resource is not valid for file: ");
		errorMessage.append(filePath);
		errorMessage.append("'");
		item->AppendMessage(new OvalMessage(errorMessage));
		originalFilename->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
	}
}

void FileProbe::GetProductName(LPVOID versionbuf, string filePath, Item *item, ItemEntity* productName) {

	//	Get the language-code page and construct the string for file version request
	UINT vdatalen;
	DWORD *lpTransArray;
	TCHAR szSubblock[80];
	TCHAR szSubblockHeader[25];
	int retVal = VerQueryValue(versionbuf,  
				  TEXT("\\VarFileInfo\\Translation"),
				  (LPVOID*)&lpTransArray,
				  &vdatalen);

	if(retVal != 0) {
		
		//	Convert the code page info into a zero-terminated
		//	string specifying which version-information value to retrieve
		_stprintf(szSubblockHeader, TEXT("\\StringFileInfo\\%04X%04X"), LOWORD(lpTransArray[0]), HIWORD(lpTransArray[0]));					
		_stprintf(szSubblock, TEXT("%s\\%s"), szSubblockHeader, TEXT("ProductName"));

		//	Get the file's developement class
		LPTSTR lpszValue;
		retVal = VerQueryValue(versionbuf,
								szSubblock, 
								(LPVOID *)&lpszValue, 
								&vdatalen);
					  
		if(retVal != 0) {

			string productNameStr = lpszValue;
			if(productNameStr.compare("") != 0) {
				productName->SetValue(productNameStr);
				productName->SetStatus(OvalEnum::STATUS_EXISTS);
			} else {
				productName->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
			}

		} else if(vdatalen == 0) {

			string errorMessage = "";
			errorMessage.append("(FileProbe) Unable to get product_name. No value is available for the specified version-information name, \"szSubblock,\" for file: ");
			errorMessage.append(filePath);
			errorMessage.append("'");
			item->AppendMessage(new OvalMessage(errorMessage));
			productName->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);

		} else if(retVal == 0) {

			string errorMessage = "";
			errorMessage.append("(FileProbe) Unable to get product_name. Either specified name, \"szSubblock,\" does not exist or the specified resource is not valid for file: ");
			errorMessage.append(filePath);
			errorMessage.append("'");
			item->AppendMessage(new OvalMessage(errorMessage));
			productName->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
		}

	} else if(vdatalen == 0) {

		string errorMessage = "";
		errorMessage.append("(FileProbe) Unable to get product_name. No value is available for the specified version-information name, \"szSubblock,\" for file: ");
		errorMessage.append(filePath);
		errorMessage.append("'");
		item->AppendMessage(new OvalMessage(errorMessage));
		productName->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);

	} else if(retVal == 0) {

		string errorMessage = "";
		errorMessage.append("(FileProbe) Unable to get product_name. Either specified name, \"szSubblock,\" does not exist or the specified resource is not valid for file: ");
		errorMessage.append(filePath);
		errorMessage.append("'");
		item->AppendMessage(new OvalMessage(errorMessage));
		productName->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
	}
}

void FileProbe::GetProductVersion(LPVOID versionbuf, string filePath, Item *item, ItemEntity* productVersion) {

	//	Get the language-code page and construct the string for file version request
	UINT vdatalen;
	DWORD *lpTransArray;
	TCHAR szSubblock[80];
	TCHAR szSubblockHeader[25];
	int retVal = VerQueryValue(versionbuf,  
				  TEXT("\\VarFileInfo\\Translation"),
				  (LPVOID*)&lpTransArray,
				  &vdatalen);

	if(retVal != 0) {
		
		//	Convert the code page info into a zero-terminated
		//	string specifying which version-information value to retrieve
		_stprintf(szSubblockHeader, TEXT("\\StringFileInfo\\%04X%04X"), LOWORD(lpTransArray[0]), HIWORD(lpTransArray[0]));					
		_stprintf(szSubblock, TEXT("%s\\%s"), szSubblockHeader, TEXT("ProductVersion"));

		//	Get the file's developement class
		LPTSTR lpszValue;
		retVal = VerQueryValue(versionbuf,
								szSubblock, 
								(LPVOID *)&lpszValue, 
								&vdatalen);
					  
		if(retVal != 0) {

			string productVersionStr = lpszValue;
			if(productVersionStr.compare("") != 0) {
				productVersion->SetValue(productVersionStr);
				productVersion->SetStatus(OvalEnum::STATUS_EXISTS);
			} else {
				productVersion->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
			}

		} else if(vdatalen == 0) {

			string errorMessage = "";
			errorMessage.append("(FileProbe) Unable to get product_version. No value is available for the specified version-information name, \"szSubblock,\" for file: ");
			errorMessage.append(filePath);
			errorMessage.append("'");
			item->AppendMessage(new OvalMessage(errorMessage));
			productVersion->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);

		} else if(retVal == 0) {

			string errorMessage = "";
			errorMessage.append("(FileProbe) Unable to get product_version. Either specified name, \"szSubblock,\" does not exist or the specified resource is not valid for file: ");
			errorMessage.append(filePath);
			errorMessage.append("'");
			item->AppendMessage(new OvalMessage(errorMessage));
			productVersion->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
		}

	} else if(vdatalen == 0) {

		string errorMessage = "";
		errorMessage.append("(FileProbe) Unable to get product_version. No value is available for the specified version-information name, \"szSubblock,\" for file: ");
		errorMessage.append(filePath);
		errorMessage.append("'");
		item->AppendMessage(new OvalMessage(errorMessage));
		productVersion->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);

	} else if(retVal == 0) {

		string errorMessage = "";
		errorMessage.append("(FileProbe) Unable to get product_version. Either specified name, \"szSubblock,\" does not exist or the specified resource is not valid for file: ");
		errorMessage.append(filePath);
		errorMessage.append("'");
		item->AppendMessage(new OvalMessage(errorMessage));
		productVersion->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
	}
}
