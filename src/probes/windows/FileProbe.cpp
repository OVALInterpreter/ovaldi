//
//
//****************************************************************************************//
// Copyright (c) 2002-2013, The MITRE Corporation
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

#include <aclapi.h>
#include <imagehlp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <tchar.h>

#include <AutoCloser.h>
#include <PrivilegeGuard.h>
#include <FsRedirectionGuard.h>
#include <iomanip>
#include <sstream>

#include <WindowsCommon.h>

#include "FileProbe.h"

using namespace std;

namespace {
	/**
	 * This code was originally written to be (sort of) both narrow and wide-char capable,
	 * so I am preserving that by using basic_string<TCHAR>...
	 */
	void GetFromVerInfo(LPVOID versionbuf, string filePath, Item *item,
		ItemEntity *entity, const basic_string<TCHAR> &fieldName);
}

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
		// Loop through all file paths
		StringPairVector::iterator iterator;
		for(iterator = filePaths->begin(); iterator != filePaths->end(); iterator++) {

			StringPair* fp = (*iterator);
			string filepath = Common::BuildFilePath(fp->first,fp->second);

			if(fp->second.compare("") == 0) {

				Item* item = NULL;

				// check if the code should report that the filename does not exist.
				if(fileFinder.ReportFileNameDoesNotExist(fp->first, fileName)) {

					item = this->CreateItem();
					item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);					
					ItemEntity* filepath = new ItemEntity("filepath", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST);	
					item->AppendElement(filepath);
					ItemEntity* path = new ItemEntity("path", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS);	
					item->AppendElement(path);
					ItemEntity* filename = new ItemEntity("filename", "", OvalEnum::DATATYPE_STRING,OvalEnum::STATUS_EXISTS);	
					item->AppendElement(filename);
							
					item->AppendElement(new ItemEntity("windows_view",
						(fileFinder.GetView() == BIT_32 ? "32_bit" : "64_bit")));
					collectedItems->push_back(item);
					
				} else {

					item = this->CreateItem();
					item->SetStatus(OvalEnum::STATUS_EXISTS);
					item->AppendElement(new ItemEntity("path", fp->first, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
					if (fileName->GetNil()){
						item->AppendElement(new ItemEntity("filename", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_NOT_COLLECTED, true)); //GetNil is true
					}
					else
					{
						item->AppendElement(new ItemEntity("filename", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS, false)); //GetNil is false
					}
					item->AppendElement(new ItemEntity("windows_view",
						(fileFinder.GetView() == BIT_32 ? "32_bit" : "64_bit")));
					collectedItems->push_back(item);

				}

			} else {
				Item* item = this->GetFileAttributes(fp->first, fp->second, fileFinder);
				if(item != NULL) {
					item->AppendElement(new ItemEntity("windows_view",
						(fileFinder.GetView() == BIT_32 ? "32_bit" : "64_bit")));
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
				Item* item = NULL;
				StringPair* fpComponents = NULL;

				// build path ObjectEntity to pass to ReportPathDoesNotExist to retrieve the status of the path value
				ObjectEntity* pathStatus = new ObjectEntity("path","",OvalEnum::DATATYPE_STRING,OvalEnum::OPERATION_EQUALS,NULL,OvalEnum::CHECK_ALL,false);
				
				for(StringVector::iterator iterator = fpaths.begin(); iterator != fpaths.end(); iterator++) {
					item = this->CreateItem();
					item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
					fpComponents = Common::SplitFilePath(*iterator);
					pathStatus->SetValue(fpComponents->first);
					ItemEntity* filepath = new ItemEntity("filepath", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST);	
					item->AppendElement(filepath);
					bool pathDoesntExist = fileFinder.ReportPathDoesNotExist(pathStatus);
					ItemEntity *path;
					if (pathDoesntExist)
						path = new ItemEntity("path", "", 
							OvalEnum::DATATYPE_STRING, 
							OvalEnum::STATUS_DOES_NOT_EXIST);
					else
						path = new ItemEntity("path", fpComponents->first);	
					item->AppendElement(path);
					ItemEntity* filename = new ItemEntity("filename", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST);
					item->AppendElement(filename);
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
			}
		}else{
			if(fileFinder.ReportPathDoesNotExist(path)) {
				Item* item = NULL;
				item = this->CreateItem();
				item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
				ItemEntity* path = new ItemEntity("path", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST);	
				item->AppendElement(path);
				item->AppendElement(new ItemEntity("windows_view",
					(fileFinder.GetView() == BIT_32 ? "32_bit" : "64_bit")));
				collectedItems->push_back(item);
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

Item* FileProbe::GetFileAttributes(string path, string fileName, FileFinder &fileFinder) {

	HANDLE hFile;
	DWORD res;
	BOOL ok;

	Item *item = NULL;

	string filePath = Common::BuildFilePath(path, fileName);

	try {

		//////////////////////////////////////////////////////
		//////////////////////  Owner  ///////////////////////
		//////////////////////////////////////////////////////

		// Get the handle of the object.
		//
		// SMC-AUDIT: ISSUE: should probably verify that this is a regular file before opening,
		// instead of a virtual memory file!
		
		hFile = fileFinder.GetFileHandle(filePath, READ_CONTROL);
		AutoCloser<HANDLE, BOOL(WINAPI&)(HANDLE)> handleGuard(hFile, 
			CloseHandle, "file "+filePath);

		if (hFile == INVALID_HANDLE_VALUE) {

			DWORD errorNum = GetLastError();
			// not-found errors are ok; we're observing an active system, and files
			// can appear and disappear at any time.
			if(errorNum != ERROR_FILE_NOT_FOUND && errorNum != ERROR_PATH_NOT_FOUND) {
				string sysErrMsg = WindowsCommon::GetErrorMessage(errorNum);
				string errorMessage = "Unable to open a handle to the file: "+sysErrMsg;
				throw ProbeException(errorMessage);
			}
		}

		// add the path and file name
		item = this->CreateItem();
		item->SetStatus(OvalEnum::STATUS_EXISTS);
		item->AppendElement(new ItemEntity("filepath", filePath, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
		item->AppendElement(new ItemEntity("path", path, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
		item->AppendElement(new ItemEntity("filename", fileName, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
		ItemEntity* owner = new ItemEntity("owner", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_ERROR);
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
				errorMessage.append("(FileProbe) Unable to get the security descriptor: "
					+ WindowsCommon::GetErrorMessage(res));
				throw ProbeException(errorMessage, ERROR_NOTICE);
			}

			// First call to LookupAccountSid to get the buffer sizes.
			LPTSTR aname = NULL;
			LPTSTR dname = NULL;
			DWORD dwaname = 0;
			DWORD dwdname = 0;
			SID_NAME_USE eUse;

			ok = LookupAccountSid(NULL,					// name of local or remote computer
								   owner_sid,				// security identifier
								   aname,					// account name buffer
								   (LPDWORD)&dwaname,		// size of account name buffer
								   dname,					// domain name
								   (LPDWORD)&dwdname,		// size of domain name buffer
								   &eUse);					// SID type

			if (!ok && GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
				LocalFree(p_sd);
				string errorMessage = "LookupAccountSid: " +
					WindowsCommon::GetErrorMessage(GetLastError());
				throw ProbeException(errorMessage, ERROR_NOTICE);
			}

			// Reallocate memory for the buffers.
			aname = (LPTSTR) malloc(dwaname);
			if (aname == NULL) {
				LocalFree(p_sd);
				string errorMessage = "";
				errorMessage.append("(FileProbe) Could not allocate space for the owner name for file: '");
				errorMessage.append(filePath);
				errorMessage.append("'.");
				throw ProbeException(errorMessage);
			}

			dname = (LPTSTR) malloc(dwdname);
			if (dname == NULL) {
				LocalFree(p_sd);
				free(aname);

				string errorMessage = "";
				errorMessage.append("(FileProbe) Could not allocate space for the domain name for the file: '");
				errorMessage.append(filePath);
				errorMessage.append("'.");
				throw ProbeException(errorMessage);
			}

			// Second call to LookupAccountSid to get the account name.
			ok = LookupAccountSid(NULL,					// name of local or remote computer
								   owner_sid,				// security identifier
								   aname,					// account name buffer
								   (LPDWORD)&dwaname,		// size of account name buffer
								   dname,					// domain name
								   (LPDWORD)&dwdname,		// size of domain name buffer
								   &eUse);					// SID type

			if (!ok) {
				LocalFree(p_sd);
				free(aname);
				free(dname);

				string errorMessage = 
					"Unable to get the name of the account for this SID: " + 
					WindowsCommon::GetErrorMessage(GetLastError());
				throw ProbeException(errorMessage);
			}
			
			//	Set owner (use domain\username format)
			string ownerStr;
			ownerStr.append(dname);
			ownerStr.append("\\");
			ownerStr.append(aname);
			owner->SetValue(ownerStr);
			owner->SetStatus(OvalEnum::STATUS_EXISTS);
			LocalFree(p_sd);
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
		FS_REDIRECT_GUARD_BEGIN(fileFinder.GetView())
		result = _stat(filePath.c_str(), &statusBuffer);
		FS_REDIRECT_GUARD_END
		if (result < 0) {
			string errorMessage = "";
			errorMessage.append("(FileProbe) Unable to get status information ");
			errorMessage.append("associated with the file: '");
			errorMessage.append(filePath);
			errorMessage.append("'.");
			
			item->AppendElement(new ItemEntity("size", "", OvalEnum::DATATYPE_INTEGER, OvalEnum::STATUS_ERROR));
			item->AppendMessage(new OvalMessage(errorMessage));
			
		} else {
			item->AppendElement(new ItemEntity("size", Common::ToString(statusBuffer.st_size), OvalEnum::DATATYPE_INTEGER, OvalEnum::STATUS_EXISTS));
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

			ItemEntity* aTime = new ItemEntity("a_time",  "", OvalEnum::DATATYPE_INTEGER, OvalEnum::STATUS_ERROR);
			item->AppendElement(aTime);
            ItemEntity* cTime = new ItemEntity("c_time",  "", OvalEnum::DATATYPE_INTEGER, OvalEnum::STATUS_ERROR);
            item->AppendElement(cTime);
			ItemEntity* mTime = new ItemEntity("m_time",  "", OvalEnum::DATATYPE_INTEGER, OvalEnum::STATUS_ERROR);
            item->AppendElement(mTime);
			string lastError = WindowsCommon::GetErrorMessage(GetLastError());
			item->AppendMessage(new OvalMessage("Unable to file times for file. " + lastError, OvalEnum::LEVEL_ERROR));

		} else {
			
			//////////////////////////////////////////////////////
			/////////////////////  Accessed  /////////////////////
			//////////////////////////////////////////////////////
			ItemEntity* aTime = new ItemEntity("a_time", WindowsCommon::ToString(lastAccessTime), OvalEnum::DATATYPE_INTEGER, OvalEnum::STATUS_EXISTS);
			item->AppendElement(aTime);

			//////////////////////////////////////////////////////
			/////////////////////  Created  /////////////////////
			//////////////////////////////////////////////////////
			ItemEntity* cTime = new ItemEntity("c_time", WindowsCommon::ToString(creationTime), OvalEnum::DATATYPE_INTEGER, OvalEnum::STATUS_EXISTS);
			item->AppendElement(cTime);

			//////////////////////////////////////////////////////
			/////////////////////  Modified  /////////////////////
			//////////////////////////////////////////////////////
			ItemEntity* mTime = new ItemEntity("m_time", WindowsCommon::ToString(writeTime), OvalEnum::DATATYPE_INTEGER, OvalEnum::STATUS_EXISTS);
			item->AppendElement(mTime);

		}

		//////////////////////////////////////////////////////
		////////////////////  MSChecksum  ////////////////////
		//////////////////////////////////////////////////////

		DWORD headersum;  
		DWORD checksum;

		FS_REDIRECT_GUARD_BEGIN(fileFinder.GetView())
		res = MapFileAndCheckSum(const_cast<char*>(filePath.c_str()), &headersum, &checksum);
		FS_REDIRECT_GUARD_END
		if (res != CHECKSUM_SUCCESS) {
			string errorMessage = "";
			errorMessage.append("(FileProbe) Unable to get ms_checksum information for the file: '");
			errorMessage.append(filePath);
			errorMessage.append("'");
			item->AppendElement(new ItemEntity("ms_checksum", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_ERROR));
			item->AppendMessage(new OvalMessage(errorMessage));

		} else {
			item->AppendElement(new ItemEntity("ms_checksum", Common::ToString(checksum), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
		}

		// initialize remaining version information entities...
		ItemEntity* version = new ItemEntity("version", "", OvalEnum::DATATYPE_VERSION, OvalEnum::STATUS_DOES_NOT_EXIST);
		item->AppendElement(version);
		ItemEntity* type = new ItemEntity("type", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST);
		item->AppendElement(type);
		ItemEntity* devClass = new ItemEntity("development_class", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST);
		item->AppendElement(devClass);
		ItemEntity* company = new ItemEntity("company", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST);	
		item->AppendElement(company);
		ItemEntity* internalName = new ItemEntity("internal_name", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST);
		item->AppendElement(internalName);
		ItemEntity* language = new ItemEntity("language", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST);
		item->AppendElement(language);
		ItemEntity* originalFilename = new ItemEntity("original_filename", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST);
		item->AppendElement(originalFilename);
		ItemEntity* productName = new ItemEntity("product_name", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST);
		item->AppendElement(productName);
		ItemEntity* productVersion = new ItemEntity("product_version", "", OvalEnum::DATATYPE_VERSION, OvalEnum::STATUS_DOES_NOT_EXIST);
		item->AppendElement(productVersion);
		
		DWORD junk;
		DWORD versionsize;
		LPVOID versionbuf;

		// Get the required size of the version info buffer.
		FS_REDIRECT_GUARD_BEGIN(fileFinder.GetView())
		versionsize = GetFileVersionInfoSize(filePath.c_str(), &junk);
		FS_REDIRECT_GUARD_END
		if (versionsize > 0) {

			versionbuf = (LPVOID)malloc(versionsize);
			FS_REDIRECT_GUARD_BEGIN(fileFinder.GetView())
			ok = GetFileVersionInfo(filePath.c_str(), 0, versionsize, versionbuf);
			FS_REDIRECT_GUARD_END
			if (ok) {

				//////////////////////////////////////////////////////
				////////////////////    version    ///////////////////
				//////////////////////////////////////////////////////

				this->GetVersion(versionbuf, filePath, item, version);			

				//////////////////////////////////////////////////////
				///////////////  development_class  //////////////////
				//////////////////////////////////////////////////////

				GetFromVerInfo(versionbuf, filePath, item, devClass, "FileVersion");

				//////////////////////////////////////////////////////
				///////////////       company       //////////////////
				//////////////////////////////////////////////////////

				GetFromVerInfo(versionbuf, filePath, item, company, "CompanyName");

				//////////////////////////////////////////////////////
				///////////////    internal_name    //////////////////
				//////////////////////////////////////////////////////

				GetFromVerInfo(versionbuf, filePath, item, internalName, "InternalName");

				//////////////////////////////////////////////////////
				///////////////       language      //////////////////
				//////////////////////////////////////////////////////
				WORD* langInfo;        
				UINT cbLang;
				VerQueryValue(versionbuf, _T("\\VarFileInfo\\Translation"),(LPVOID*)&langInfo, &cbLang);  
				DWORD lcid = langInfo[0];
				LPTSTR szLang = ( LPTSTR ) malloc ( sizeof ( TCHAR ) * 100 );
				if(VerLanguageName(lcid, szLang, 100) != 0){
					language->SetValue(Common::ToString(szLang));
					language->SetStatus(OvalEnum::STATUS_EXISTS);
					
				}else{
					language->SetStatus(OvalEnum::STATUS_ERROR);
				}
				if ( szLang != NULL ) {
					free ( szLang );
					szLang = NULL;
				}

				//////////////////////////////////////////////////////
				///////////////  original_filename  //////////////////
				//////////////////////////////////////////////////////

				GetFromVerInfo(versionbuf, filePath, item, originalFilename, "OriginalFilename");

				//////////////////////////////////////////////////////
				///////////////     product_name    //////////////////
				//////////////////////////////////////////////////////

				GetFromVerInfo(versionbuf, filePath, item, productName, "ProductName");

				//////////////////////////////////////////////////////
				///////////////   product_version   //////////////////
				//////////////////////////////////////////////////////

				GetFromVerInfo(versionbuf, filePath, item, productVersion, "ProductVersion");


				// Post-processing for development_class:
				// for dev class, there is a parenthesized part at the end we need
				// to pull out and use.
				REGEX verMatcher;
				if(devClass->GetStatus() == OvalEnum::STATUS_EXISTS &&
					verMatcher.IsMatch(".+\\([^\\)].+\\)", devClass->GetValue().c_str())) {
					string verStr = devClass->GetValue();
					verStr = verStr.substr(verStr.find("(") + 1);
					devClass->SetValue(verStr.substr(0, verStr.find(".")));
				} else {
					devClass->SetValue("");
					devClass->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
				}

			} else {
				// we got a size from GetFileVersionInfoSize(), but then getting the
				// actual info failed.  This seems worthy of error status on these
				// entities, although MSDN docs are not clear about under which
				// conditions this can occur.
				version->SetStatus(OvalEnum::STATUS_ERROR);
				devClass->SetStatus(OvalEnum::STATUS_ERROR);
				company->SetStatus(OvalEnum::STATUS_ERROR);
				internalName->SetStatus(OvalEnum::STATUS_ERROR);
				language->SetStatus(OvalEnum::STATUS_ERROR);
				originalFilename->SetStatus(OvalEnum::STATUS_ERROR);
				productName->SetStatus(OvalEnum::STATUS_ERROR);
				productVersion->SetStatus(OvalEnum::STATUS_ERROR);

				string errorMessage = "(FileProbe) Unable to get version info for the file: '"
					+ filePath + "': " + WindowsCommon::GetErrorMessage(GetLastError());
				item->AppendMessage(new OvalMessage(errorMessage));
			}

			free(versionbuf);

		} else {

			string errorMessage = "(FileProbe) No version information available for the file: '"
				+ filePath + "': " + WindowsCommon::GetErrorMessage(GetLastError());
			item->AppendMessage(new OvalMessage(errorMessage));
		}

		//////////////////////////////////////////////////////
		/////////////////////  FileType  /////////////////////
		//////////////////////////////////////////////////////

		this->GetType(hFile, filePath, item, type, fileFinder);

		//////////////////////////////////////////////////////
		//////////////////////////////////////////////////////

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
		item->AppendMessage(new OvalMessage(errorMessage));

	} else {

		ostringstream oss;
		oss << HIWORD(pFFI->dwFileVersionMS) << '.'
			<< LOWORD(pFFI->dwFileVersionMS) << '.'
			<< HIWORD(pFFI->dwFileVersionLS) << '.'
			<< LOWORD(pFFI->dwFileVersionLS);
		version->SetValue(oss.str());
		version->SetStatus(OvalEnum::STATUS_EXISTS);
	}
}

void FileProbe::GetType(HANDLE hFile, string filePath, Item *item, ItemEntity* type, FileFinder &fileFinder) {

	DWORD res = GetFileType(hFile);

	BOOL gfaRes;
	WIN32_FILE_ATTRIBUTE_DATA lpFileInformation;

	switch (res) {

		case FILE_TYPE_DISK:

			FS_REDIRECT_GUARD_BEGIN(fileFinder.GetView())
			gfaRes = GetFileAttributesEx(filePath.c_str(),				// file or directory name
										 GetFileExInfoStandard,			// attribute class
										 (LPVOID)&lpFileInformation);	// attribute information 
			FS_REDIRECT_GUARD_END

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

namespace {

	void GetFromVerInfo(LPVOID versionbuf, string filePath, Item *item,
		ItemEntity *itemEntity, const basic_string<TCHAR> &fieldName) {

		//	Get the language-code page and construct the string for file version request
		UINT vdatalen;
		DWORD *lpTransArray;

		BOOL retVal = VerQueryValue(versionbuf,  
					  TEXT("\\VarFileInfo\\Translation"),
					  (LPVOID*)&lpTransArray,
					  &vdatalen);

		if(retVal != 0) {
		
			// I guess to be TCHAR-correct, we gotta use the basic_* templates
			// directly so we can specify the char type...
			basic_ostringstream<TCHAR> oss;
			oss << TEXT("\\StringFileInfo\\")
				<< uppercase << hex << setfill(TEXT('0'))
				<< setw(4) << LOWORD(lpTransArray[0])
				<< setw(4) << HIWORD(lpTransArray[0])
				<< nouppercase
				<< TEXT('\\') << fieldName;
		
			
			LPTSTR lpszValue;
			retVal = VerQueryValue(versionbuf,
									oss.str().c_str(),
									(LPVOID *)&lpszValue, 
									&vdatalen);

			if(retVal != 0) {
				if (_tcsclen(lpszValue) > 0) {
					// except here we assume we are not in unicode mode...
					itemEntity->SetValue(lpszValue);
					itemEntity->SetStatus(OvalEnum::STATUS_EXISTS);
				}
			} else if(vdatalen == 0) {

				string errorMessage = "";
				errorMessage.append("(FileProbe) Unable to get "+itemEntity->GetName()+". No value is available for the specified version-information name, \""+fieldName+",\" for file: ");
				errorMessage.append(filePath);
				errorMessage.append("'");
				item->AppendMessage(new OvalMessage(errorMessage));

			} else {

				string errorMessage = "";
				errorMessage.append("(FileProbe) Unable to get "+itemEntity->GetName()+". Either specified name, \""+fieldName+",\" does not exist or the specified resource is not valid for file: ");
				errorMessage.append(filePath);
				errorMessage.append("'");
				item->AppendMessage(new OvalMessage(errorMessage));

			}

		} else if(vdatalen == 0) {

			string errorMessage = "";
			errorMessage.append("(FileProbe) Unable to get "+itemEntity->GetName()+". No value is available for the specified version-information name, \""+fieldName+",\" for file: ");
			errorMessage.append(filePath);
			errorMessage.append("'");
			item->AppendMessage(new OvalMessage(errorMessage));

		} else {

			string errorMessage = "";
			errorMessage.append("(FileProbe) Unable to get "+itemEntity->GetName()+". Either specified name, \""+fieldName+",\" does not exist or the specified resource is not valid for file: ");
			errorMessage.append(filePath);
			errorMessage.append("'");
			item->AppendMessage(new OvalMessage(errorMessage));

		}
	}
}
