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
#include <FindCloseGuard.h>
#include "FileFinder.h"

using namespace std;

FileFinder::FileFinder() {

}

FileFinder::~FileFinder() {

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

StringVector* FileFinder::ProcessPathBehaviors(StringVector* paths, BehaviorVector* behaviors) {

	StringVector* behaviorPaths = new StringVector();

	// Process the behaviors to identify any additional paths.
	// initialize these default values based on the defaults 
	// set in the oval definitions schema
	string recurseDirection = Behavior::GetBehaviorValue(behaviors, "recurse_direction");
	if(recurseDirection.compare("") == 0) {
		recurseDirection = "none";
	}

	// We only need to check the recurse_file_systems and max_depth behaviors
	// if recurse_direction is not equal to "none".
	if ( recurseDirection.compare("none") != 0 ){

		string maxDepthStr = Behavior::GetBehaviorValue(behaviors, "max_depth");
		int maxDepth = -1;
		if(maxDepthStr.compare("") != 0) {
			Common::FromString(maxDepthStr, &maxDepth);
			if(maxDepth < -1) 
				maxDepth = -1;
		}

		// only need to address recurseDirection up & down if maxDepth is not 0
		if(recurseDirection.compare("up") == 0 && maxDepth != 0) {
			StringVector::iterator path;
			for(path = paths->begin(); path != paths->end(); path++) {
				this->UpwardPathRecursion(behaviorPaths, (*path), maxDepth);
			}

		} else if(recurseDirection.compare("down") == 0 && maxDepth != 0) {
			StringVector::iterator path;
			for(path = paths->begin(); path != paths->end(); path++) {
				this->DownwardPathRecursion(behaviorPaths, (*path), maxDepth);
			}
		}
	}

	return behaviorPaths;
}

void FileFinder::FindPaths(string queryVal, StringVector* paths, OvalEnum::Operation op) {

	string fileName;

	string patternOut;
	string constPortion;

	// This optimization only applies when the regex is anchored to
	// the beginning of paths. (regex has to start with '^')
	if (op == OvalEnum::OPERATION_PATTERN_MATCH && !queryVal.empty() && queryVal[0] == '^') {		
		this->fileMatcher->GetConstantPortion(queryVal, Common::fileSeperator, &patternOut, &constPortion);
		// Remove extra slashes and normalize
		constPortion = Common::StripTrailingSeparators(
			WindowsCommon::GetActualPathWithCase(
				this->fileMatcher->RemoveExtraSlashes(constPortion)));
	}

	// Found a constant portion
	if(!constPortion.empty() && !patternOut.empty()) {

		//	Call search function
		this->GetPathsForOperation(constPortion, queryVal, paths, OvalEnum::OPERATION_PATTERN_MATCH);

	//	No constant portion.
	} else if(constPortion.empty()) { 
		
		//	Must start searching for matches on each drive.
		auto_ptr<StringVector> drives(this->GetDrives());

		StringVector::iterator drive;
		for (drive=drives->begin(); drive!=drives->end(); drive++) {

			//	Call search function
			try  {
				this->GetPathsForOperation((*drive), queryVal, paths, op);

			} catch(REGEXException ex) {
				if(ex.GetSeverity() == ERROR_WARN) {
					string pcreMsg;
					pcreMsg.append("Filefinder Warning - while searching for matching files:\n");
					pcreMsg.append("-----------------------------------------------------------------------\n");
					pcreMsg.append(ex.GetErrorMessage());
					Log::Debug(pcreMsg);
				} else {
					throw;
				}		
			}
		}
	} else if(patternOut.empty()) {

		//	There are no pattern matching chars treat this as a normal path 
		if(this->PathExists(constPortion)) {
			paths->push_back(constPortion);
		}
	}
}

StringVector* FileFinder::GetDrives() {

	StringVector* drives = new StringVector();	
	unsigned int index	= 0;
	string errMsg;
	DWORD nBufferLength = 0;
	DWORD dwResult		= 0;
	LPTSTR lpBuffer		= new char[0];

	//	Get the required buffer size
	dwResult = GetLogicalDriveStrings(	nBufferLength,	// size of buffer
										lpBuffer);		// drive strings buffer

	if(dwResult > nBufferLength) {
		//	Call the function again with the correct buffer size
		delete [] (lpBuffer); 
		lpBuffer = new char[dwResult]; 
		nBufferLength = dwResult;
		dwResult = GetLogicalDriveStrings(	nBufferLength,	// size of buffer
											lpBuffer);		// drive strings buffer
	
	} else if(dwResult == 0) {
		delete drives;
		delete [] (lpBuffer);

		//	Error check GetLastError 
		char strErrorCode[33];
		_itoa(GetLastError(), strErrorCode, 10);
		errMsg.append("Error: Unable to enumerate the drives on the system. Error code: ");
		errMsg.append(strErrorCode);
		errMsg.append("\n");
		throw FileFinderException(errMsg);

	} else {
		delete drives;
		delete [] (lpBuffer);

		//	Unknown Error
		errMsg.append("Error: Unable to enumerate the drives on the system. (Unknown error)\n");
		throw FileFinderException(errMsg);
	}

	
	if(dwResult == 0) {
		delete drives;
		delete [] (lpBuffer);

		//	Error check GetLastError 
		char strErrorCode[33];
		_itoa(GetLastError(), strErrorCode, 10);
		errMsg.append("Error: Unable to enumerate the drives on the system. Error code: ");
		errMsg.append(strErrorCode);
		errMsg.append("\n");
		throw FileFinderException(errMsg);

	//	Process the list of drives
	} else {
		while(index < dwResult) {
			
			const char *drive = lpBuffer + index;

			//	Only fixed drives
			if(GetDriveType(drive) == DRIVE_FIXED) {
				drives->push_back(WindowsCommon::GetActualPathWithCase(drive));
			}

			index += strlen(drive) + 1; // skip over the '\0' too
		}	
	}

	delete [] (lpBuffer);

	return drives;
}

void FileFinder::GetPathsForOperation(string dirIn, string queryVal, StringVector *pathVector, OvalEnum::Operation op) {

	try {

		//	Stop if a Null Dir
		if (dirIn.empty())
			return;

		if ( this->PathExists(dirIn) ){

			if(EntityComparator::CompareString(op, queryVal, dirIn) == OvalEnum::RESULT_TRUE)
				pathVector->push_back(dirIn);

			// Append a '*' to the end of the path to signify that we want to find all files
			// in given directory.
			string findDir;
	
			// Verify that the path that was passed into this function ends with a slash.  If
	 		// it doesn't, then add one.
	 		if (dirIn[dirIn.length()-1] != Common::fileSeperator)
	 			dirIn.append(1, Common::fileSeperator);

			findDir = dirIn + "*";

			// Find the first file in the directory.  If this fails, then there is no reason
			// to continue.
			WIN32_FIND_DATA FindFileData;
			HANDLE hFind = INVALID_HANDLE_VALUE;

			hFind = FindFirstFile(findDir.c_str(), &FindFileData);
			if (hFind == INVALID_HANDLE_VALUE) {

				DWORD errorNum = GetLastError();

				if(errorNum == ERROR_FILE_NOT_FOUND || errorNum == ERROR_PATH_NOT_FOUND || errorNum == ERROR_NOT_READY) {

					// if the file is not found just return no need to report an error
					return;

				} else {
					
					// report other errors that might occur
					string errorMessage =
						"Error while seaching for matching file paths. " +
						WindowsCommon::GetErrorMessage(errorNum) +
						" Directory: " +
						dirIn +
						" Pattern: " +
						queryVal;
					throw FileFinderException(errorMessage);
				}
			}

			//	Loop through each file in the directory.  
			//	If a sub-directory is found, make a recursive call to GetFilePathsForOperation to search its contents.
			//	If a file is found get the file path and check it against the queryVal
			FindCloseGuard fcg(hFind);
			do {

				// Skip ., .., and System Volume 
				if ((strncmp(FindFileData.cFileName, ".", 1) == 0) ||
					(strncmp(FindFileData.cFileName, "..", 2) == 0) ||
					(strncmp(FindFileData.cFileName, "System Volume Information", 25) == 0))
				{
				
				// Found a dir
				} else if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					GetPathsForOperation(dirIn + FindFileData.cFileName, queryVal, pathVector, op);

			} while (FindNextFile(hFind, &FindFileData));

			//	Close the handle to the file search object.
			if(!fcg.close()) {
				string errorMessage =
					"Error: Unable to close search handle while trying to search for matching paths. " + 
					WindowsCommon::GetErrorMessage(GetLastError()) +
					" Directory: " +
					dirIn +
					" Pattern: " +
					queryVal;
				throw FileFinderException(errorMessage);	
			}

		}
	//	Just need to ensure that all exceptions have a nice message. 
	//	So rethrow the exceptions I created catch the others and format them.
	} catch(Exception ex) {

		throw;

	} catch(...) {

		string errorMessage =
			"Error: "
			"An unspecified error was encountered while trying to search for matching paths. Directory: " +
			dirIn +
			" Pattern: " +
			queryVal;
		throw FileFinderException(errorMessage);
	}
}

void FileFinder::GetFilesForOperation(string path, string queryVal, StringVector* fileNames, OvalEnum::Operation op, bool isFilePath) {

	try {

		// Verify that the path that was passed into this function ends with a slash.  If
		// it doesn't, then add one.
		if (path[path.length()-1] != Common::fileSeperator)
			path.append(1, Common::fileSeperator);
		
		// Append a '*' to the end of the path to signify that we want to find all files
		// in given directory.
		string findDir;
		findDir = path + "*";

		// Find the first file in the directory.  If this fails, then there is no reason
		// to continue.
		WIN32_FIND_DATA FindFileData;
		HANDLE hFind = INVALID_HANDLE_VALUE;

		hFind = FindFirstFile(findDir.c_str(), &FindFileData);
		if (hFind == INVALID_HANDLE_VALUE) {

			DWORD errorNum = GetLastError();

			if(errorNum == ERROR_FILE_NOT_FOUND || errorNum == ERROR_PATH_NOT_FOUND || errorNum == ERROR_NOT_READY) {

				// if the file is not found just return no need to report an error
				return;

			} else {
				
				// report other errors that might occure
				string msg = WindowsCommon::GetErrorMessage(errorNum);
				string errorMessage =
					"Error while seaching for matching files. " +
					WindowsCommon::GetErrorMessage(errorNum) +
					" Directory: " +
					path +
					" Pattern: " +
					queryVal;
				throw FileFinderException(errorMessage);
			}
		}

		//	Loop through each file in the directory.  
		//	If a file is found get the file path and check it against the queryVal
		FindCloseGuard fcg(hFind);
		do {

			// Skip ., .., and System Volume 
			if ((strncmp(FindFileData.cFileName, ".", 1) == 0) ||
				(strncmp(FindFileData.cFileName, "..", 2) == 0) ||
				(strncmp(FindFileData.cFileName, "System Volume Information", 25) == 0))
			{
			
			// Found a dir
			} else if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {

				// skip
			
			} else {
			
				string fileName = FindFileData.cFileName;

				//	Check queryVal
				if ( isFilePath ){
					string filepath = Common::BuildFilePath(path, fileName);
					if(EntityComparator::CompareString(op, queryVal, filepath) ==
						OvalEnum::RESULT_TRUE)
						fileNames->push_back(filepath);
				} else {
					if(EntityComparator::CompareString(op, queryVal, fileName) ==
						OvalEnum::RESULT_TRUE)
						fileNames->push_back(fileName);
				}
			}
		} while (FindNextFile(hFind, &FindFileData));

		//	Close the handle to the file search object.
		if(!fcg.close()) {

			string errorMessage =
				"Error: Unable to close search handle while trying to search for matching files. " +
				WindowsCommon::GetErrorMessage(GetLastError()) +
				" Directory: " +
				path +
				" Pattern: " +
				queryVal;
			throw FileFinderException(errorMessage);
		}

	//	Just need to ensure that all exceptions have a nice message. 
	//	So rethrow the exceptions I created catch the others and format them.
	} catch(Exception ex) {

		throw;

	} catch(...) {

		string errorMessage =
			"Error: "
			"An unspecified error was encountered while trying to search for matching paths. \n\tDirectory: " +
			path +
			"\n\tPattern: " +
			queryVal;
		throw FileFinderException(errorMessage);
	}
}

bool FileFinder::PathExists(const string &path, string *actualPath) {

	bool exists = false;

	HANDLE hFile = INVALID_HANDLE_VALUE;

	try {
		hFile = CreateFile(path.c_str(),					// DirName
								0/*GENERIC_READ*/,				// access mode
								FILE_SHARE_READ,			// share mode
								NULL,						// SD
								OPEN_EXISTING,				// how to create
								FILE_FLAG_BACKUP_SEMANTICS,	// file attributes
								NULL);						// handle to template file


		if (hFile == INVALID_HANDLE_VALUE) {

			DWORD errorNum = GetLastError();

			if(errorNum == ERROR_FILE_NOT_FOUND) {
				// skip this not looking for a file
				
			} else if(errorNum == ERROR_PATH_NOT_FOUND) {

				// skip this the path does not exist
			} else if (errorNum == ERROR_NOT_READY) {
				// This path may be a removable drive with the media removed.
				// Test if the given path is the mount point itself or a subdirectory
				// under the mount point.
				
				DWORD attrs = GetFileAttributes(path.c_str());
				if (attrs != INVALID_FILE_ATTRIBUTES && attrs & FILE_ATTRIBUTE_REPARSE_POINT) {
					exists = true;
				}
			} else {
				string errorMessage =
					"(FileProbe) Unable to open a handle to the file '" +
					path +
					"': " +
					WindowsCommon::GetErrorMessage(errorNum);
				throw FileFinderException(errorMessage);
			}
		} else {
			exists = true;
			CloseHandle(hFile); 
		}

		if (exists && actualPath != NULL)
			*actualPath = Common::StripTrailingSeparators(WindowsCommon::GetActualPathWithCase(path));

	} catch(Exception ex) {
		if (hFile != INVALID_HANDLE_VALUE)
			CloseHandle(hFile); 
		throw;

	} catch(...) {

		if (hFile != INVALID_HANDLE_VALUE)
			CloseHandle(hFile); 
		string errorMessage;
		errorMessage.append("Error: ");
		errorMessage.append("An unspecified error was encountered while trying to search for matching paths. \n\tDirectory: ");
		errorMessage.append(path);
		throw FileFinderException(errorMessage);
	}

	return exists;
}

void FileFinder::PathExistsCaseInsensitive(const string &path, 
	StringVector *pathsFound) {

	// windows is natively case-insensitive, so we only need to do a plain
	// existence check.
	string tmpPath;
	if (this->PathExists(path, &tmpPath))
		pathsFound->push_back(tmpPath);
}

bool FileFinder::FileNameExists(string path, string fileName, string *actualFileName) {

	bool exists = false;

	string filePath = Common::BuildFilePath(path, fileName);

	HANDLE hFile = INVALID_HANDLE_VALUE;

	try {
		hFile = CreateFile(filePath.c_str(),				// file name
								0/*GENERIC_READ*/,				// access mode
								FILE_SHARE_READ,			// share mode
								NULL,						// SD
								OPEN_EXISTING,				// how to create
								FILE_ATTRIBUTE_NORMAL,		// file attributes
								NULL);						// handle to template file

		if (hFile == INVALID_HANDLE_VALUE) {

			DWORD errorNum = GetLastError();

			if(errorNum == ERROR_FILE_NOT_FOUND) {
				// skip this the file does not exist
				
			} else if(errorNum == ERROR_PATH_NOT_FOUND) {

				// skip this since if the path is not found the file can't exist

			} else if (errorNum == ERROR_NOT_READY) {
				// skip this the device is not ready.  This path may be a removable drive with the media removed.
			} else {
				string errorMessage =
					"Unable to open a handle to the file '" +
					filePath +
					"':  " + 
					WindowsCommon::GetErrorMessage(errorNum);
				throw FileFinderException(errorMessage);
			}
		} else {
			exists = true;
			CloseHandle(hFile); 
		}

		if (exists && actualFileName != NULL) {
			string actualFilepath = WindowsCommon::GetActualPathWithCase(filePath);
			auto_ptr<StringPair> split(Common::SplitFilePath(actualFilepath));
			if (split.get())
				*actualFileName = split->second;
		}

	} catch(Exception ex) {
		if (hFile != INVALID_HANDLE_VALUE)
			CloseHandle(hFile); 
		throw;

	} catch(...) {
		if (hFile != INVALID_HANDLE_VALUE)
			CloseHandle(hFile); 
		string errorMessage =
			"Error: "
			"An unspecified error was encountered while trying to search for matching paths. \n\tDirectory: " +
			path;
		throw FileFinderException(errorMessage);
	}

	return exists;
}

StringVector* FileFinder::GetChildDirectories(string path) {

	StringVector* childDirs = new StringVector();
	try {


		// Verify that the path that was passed into this function ends with a slash.  If
		// it doesn't, then add one.
		if (path[path.length()-1] != Common::fileSeperator)
			path.append(1, Common::fileSeperator);
		
		// Append a '*' to the end of the path to signify that we want to find all files
		// in given directory.
		string findDir;
		findDir = path + "*";

		// Find the first file in the directory.  If this fails, then there is no reason
		// to continue.
		WIN32_FIND_DATA FindFileData;
		HANDLE hFind = INVALID_HANDLE_VALUE;

		hFind = FindFirstFile(findDir.c_str(), &FindFileData);
		if (hFind == INVALID_HANDLE_VALUE) {
			DWORD errorNum = GetLastError();
			DWORD attrs = GetFileAttributes(path.c_str());
			if (attrs != INVALID_FILE_ATTRIBUTES && attrs & FILE_ATTRIBUTE_REPARSE_POINT && errorNum == ERROR_NOT_READY) {
				// Specified path points to an empty removable media device mounted into the NTFS filesystem.  Skip it.
				return childDirs;
			}

			delete childDirs;

			string errorMessage;
			errorMessage.append("Error: Unable to get a valid handle in GetChildDirectories(). Directory: ");
			errorMessage.append(path);
			throw FileFinderException(errorMessage);
		}

		//	Loop through each file in the directory.  
		//	If a sub-directory is found store it on the set of childDirs string vector
		do {

			// Skip ., .., and System Volume 
			if ((strncmp(FindFileData.cFileName, ".", 1) == 0) ||
				(strncmp(FindFileData.cFileName, "..", 2) == 0) ||
				(strncmp(FindFileData.cFileName, "System Volume Information", 25) == 0)) {
			
			// Found a dir
			} else if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {

				string childDir = path;					
				childDir.append(FindFileData.cFileName);
				
				childDirs->push_back(childDir);			
			}
		} while (FindNextFile(hFind, &FindFileData));

		//	Close the handle to the file search object.
		if(!FindClose(hFind)) {
			delete childDirs;

			string errorMessage;
			errorMessage.append("Error: Unable to close search handle while trying to get child directories. Parent directory: ");
			errorMessage.append(path);
			throw FileFinderException(errorMessage);	
		}

	//	Just need to ensure that all exceptions have a nice message. 
	//	So rethrow the exceptions I created catch the others and format them.
	} catch(Exception ex) {

		throw;

	} catch(...) {

		delete childDirs;

		string errorMessage;
		errorMessage.append("Error: An unspecified error was encountered while trying to get child directories. Parent Directory: ");
		errorMessage.append(path);
		throw FileFinderException(errorMessage);
	}

	return childDirs;
}