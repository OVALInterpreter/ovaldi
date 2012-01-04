//
//
//****************************************************************************************//
// Copyright (c) 2002-2011, The MITRE Corporation
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

#include <algorithm>
#include "AbsFileFinder.h"

using namespace std;

AbsFileFinder::AbsFileFinder() {

	fileMatcher = new REGEX();
}

AbsFileFinder::~AbsFileFinder() {

	delete fileMatcher;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
StringPairVector* AbsFileFinder::SearchFiles(ObjectEntity* path, ObjectEntity* fileName, BehaviorVector* behaviors) {

	StringPairVector* filePaths = new StringPairVector();

	// get all the paths
	StringVector* paths = this->GetPaths(path, behaviors);
	StringVector::iterator pathIt;
	for(pathIt = paths->begin(); pathIt != paths->end(); pathIt++) {
		// get all files if the file is not nil
		if(fileName->GetNil()) {

			StringPair* filePath = new StringPair((*pathIt), "");
            filePaths->push_back(filePath);

		} else {

			StringVector* fileNames = this->GetFileNames((*pathIt), fileName);
			StringVector::iterator fileNameIt;
			for(fileNameIt = fileNames->begin(); fileNameIt != fileNames->end(); fileNameIt++) {
				StringPair* filePath = new StringPair((*pathIt), (*fileNameIt));
				filePaths->push_back(filePath);
			}
			delete fileNames;
		}
	}
	delete paths;
	return filePaths;
}

StringPairVector* AbsFileFinder::SearchFiles(ObjectEntity* filePath){
	StringVector* filePaths = this->GetFilePaths(filePath);
	StringPairVector* filePathsPair = new StringPairVector();
	for(StringVector::iterator it = filePaths->begin() ; it != filePaths->end() ; it++){
		StringPair* fp = Common::SplitFilePath(*it);
		filePathsPair->push_back(fp);
	}
	delete filePaths;
	filePaths = NULL;
	return filePathsPair;
}

StringVector* AbsFileFinder::GetPaths(ObjectEntity* path, BehaviorVector* behaviors) {

	auto_ptr<StringVector> pathsFound(new StringVector());

	// make a copy of the path so I can switch its operation without
	// affecting the original.
	ObjectEntity tmpPath(path);

#ifdef WIN32
	// On windows, always ignore case sensitivity.
	// I implement this by forcibly changing the op so
	// we get the desired behavior out of EntityComparator.
	if (tmpPath.GetOperation() == OvalEnum::OPERATION_NOT_EQUAL)
		tmpPath.SetOperation(OvalEnum::OPERATION_CASE_INSENSITIVE_NOT_EQUAL);
	else if (tmpPath.GetOperation() == OvalEnum::OPERATION_EQUALS)
		tmpPath.SetOperation(OvalEnum::OPERATION_CASE_INSENSITIVE_EQUALS);
#endif

	StringVector pathsToSearch;
	tmpPath.GetEntityValues(pathsToSearch);

	for (StringVector::iterator iter = pathsToSearch.begin();
		iter != pathsToSearch.end();
		++iter) {

		switch (tmpPath.GetOperation()) {

		case OvalEnum::OPERATION_EQUALS:
			// due to the op change above, this won't be executed
			// on windows... but the case-insensitive impl is
			// identical.
			if (this->PathExists(*iter))
				pathsFound->push_back(*iter);
			break;

		case OvalEnum::OPERATION_CASE_INSENSITIVE_EQUALS:
#ifdef WIN32
			if (this->PathExists(*iter))
				pathsFound->push_back(*iter);
			break;
#else
			this->PathExistsCaseInsensitive(*iter, pathsFound.get());
#endif

		default:
			// all other ops require searching.
			this->FindPaths(*iter, pathsFound.get(), tmpPath.GetOperation());
		}
	}

	if (tmpPath.GetVarRef() != NULL) {
		// only keep paths that match operation and value and var check
		ItemEntity tmp("path","", OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS);
		StringVector::iterator it;
		for(it = pathsFound->begin(); it != pathsFound->end(); ) {
			tmp.SetValue(*it);
			if(tmpPath.Analyze(&tmp) != OvalEnum::RESULT_TRUE)
				it = pathsFound->erase(it);
			else
				++it;
		}
	}

	// apply any behaviors and consolidate the results
	StringVector* behaviorPaths = this->ProcessPathBehaviors(pathsFound.get(), behaviors);
	pathsFound->insert(pathsFound->end(), behaviorPaths->begin(), behaviorPaths->end());
	delete behaviorPaths;

	// unique() only removes adjacent dupes, so we sort first
	sort(pathsFound->begin(), pathsFound->end());
	unique(pathsFound->begin(), pathsFound->end());

	return pathsFound.release();
}

void AbsFileFinder::DownwardPathRecursion(StringVector* paths, string path, int maxDepth) {

	if(maxDepth == 0) {
		return;
	} 
	if(maxDepth < -1) {
		throw FileFinderException("Error invalid max_depth. max_depth must be -1 or more. Found: " + Common::ToString(maxDepth));
	}

	// get child directories
	StringVector* childDirectories = this->GetChildDirectories(path);
	StringVector::iterator childDirectory;
	for(childDirectory = childDirectories->begin(); childDirectory != childDirectories->end(); childDirectory++) {

		// store child directory
		(*childDirectory)+=Common::fileSeperator;
		paths->push_back((*childDirectory));
		if(maxDepth == -1) {

			// make recursive call 
			this->DownwardPathRecursion(paths, (*childDirectory), maxDepth);

		} else if(maxDepth > 0) {

			// make recursive call
			this->DownwardPathRecursion(paths, (*childDirectory), --maxDepth);
		}
	}
	delete childDirectories;
}

void AbsFileFinder::UpwardPathRecursion(StringVector* paths, string path, int maxDepth) {
	
	if(maxDepth == 0) {
		return;
	} 
	if(maxDepth < -1) {
		throw FileFinderException("Error invalid max_depth. max_depth must be -1 or more. Found: " + Common::ToString(maxDepth));
	}

	// get parent directory by parsing the string to find the last file seperator

	// check if path is currently / or c:/ and stop 
	string fileSeperatorStr = "";
	fileSeperatorStr+=Common::fileSeperator;
	string regex = "^[A-Za-z]:\\\\$";
	if(path.compare(fileSeperatorStr) == 0 || this->fileMatcher->IsMatch(regex.c_str(), path.c_str())) {
		return;
	}
	
	// remove the trailing char if it is a file seperator
	if (path[path.length()-1] == Common::fileSeperator) {
		path = path.substr(0, (path.length()-1));
	} 

	// find the last file seperator
	string::size_type index = path.find_last_of(Common::fileSeperator);

	// if no path seperator is found stop recursing up.
	if(index == string::npos) {
		return;
	} 
	string parentDirectory = path.substr(0, index+1);

	// store parent directory
	paths->push_back(parentDirectory);
	if(maxDepth == -1) {

		// make recursive call 
		this->UpwardPathRecursion(paths, parentDirectory, maxDepth);

	} else if(maxDepth > 0) {

		// make recursive call
		this->UpwardPathRecursion(paths, parentDirectory, --maxDepth);
	}
}

void AbsFileFinder::GetFilePathsForOperation(string queryVal, StringVector* filePaths, OvalEnum::Operation op){
	auto_ptr<StringVector> paths(new StringVector());
	auto_ptr<StringPair> fp;

	if ( op == OvalEnum::OPERATION_PATTERN_MATCH ) {
		fp.reset(Common::SplitFilePathRegex(queryVal));
		if (fp.get() == NULL)
			// no path sep char found to do the split... so
			// use the whole value as a regex for both paths
			// and filepaths.
			this->FindPaths(queryVal, paths.get(), op);
		else{
			this->FindPaths(fp->first, paths.get(), op);
		}

	} else {
		this->FindPaths(".*", paths.get(), op);
	}

	for(StringVector::iterator it = paths->begin() ; it != paths->end() ; it++) {
		this->GetFilesForOperation(*it, queryVal, filePaths, op, true);
	}
}

bool AbsFileFinder::FilePathExists(string filePath){
	bool exists = false;
	StringPair* fpComponents = Common::SplitFilePath(filePath);
	if ( fpComponents != NULL ){
		exists = this->FileNameExists(fpComponents->first, fpComponents->second);
		delete fpComponents;
		fpComponents = NULL;
	}
	return exists;
}

StringVector* AbsFileFinder::GetFileNames(string path, ObjectEntity* fileName) {

	auto_ptr<StringVector> fileNamesFound(new StringVector());

	// make a copy of the path so I can switch its operation without
	// affecting the original.
	ObjectEntity tmpFileName(fileName);

#ifdef WIN32
	// On windows, always ignore case sensitivity.
	// I implement this by forcibly changing the op so
	// we get the desired behavior out of EntityComparator.
	if (tmpFileName.GetOperation() == OvalEnum::OPERATION_NOT_EQUAL)
		tmpFileName.SetOperation(OvalEnum::OPERATION_CASE_INSENSITIVE_NOT_EQUAL);
	else if (tmpFileName.GetOperation() == OvalEnum::OPERATION_EQUALS)
		tmpFileName.SetOperation(OvalEnum::OPERATION_CASE_INSENSITIVE_EQUALS);
#endif

	StringVector fileNamesToSearch;
	tmpFileName.GetEntityValues(fileNamesToSearch);

	for (StringVector::iterator iter = fileNamesToSearch.begin();
		iter != fileNamesToSearch.end();
		++iter) {

		switch (tmpFileName.GetOperation()) {

		case OvalEnum::OPERATION_EQUALS:
			// due to the op change above, this won't be executed
			// on windows... but the case-insensitive impl is
			// identical.
			if (this->FileNameExists(path, *iter))
				fileNamesFound->push_back(*iter);
			break;

		case OvalEnum::OPERATION_CASE_INSENSITIVE_EQUALS:
#ifdef WIN32
			if (this->FileNameExists(path, *iter))
				fileNamesFound->push_back(*iter);
#else
			this->FileNameExistsCaseInsensitive(path, *iter, fileNamesFound.get());
#endif
			break;

		default:
			// all other ops require searching.
			this->GetFilesForOperation(path, *iter, fileNamesFound.get(),
				tmpFileName.GetOperation(), false);
		}
	}

	if (tmpFileName.GetVarRef() != NULL) {
		// only keep filenames that match operation and value and var check
		ItemEntity tmp("filename","", OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS);
		StringVector::iterator it;
		for(it = fileNamesFound->begin(); it != fileNamesFound->end(); ) {
			tmp.SetValue(*it);
			if(tmpFileName.Analyze(&tmp) != OvalEnum::RESULT_TRUE)
				it = fileNamesFound->erase(it);
			else
				++it;
		}
	}

	return fileNamesFound.release();
}

StringVector* AbsFileFinder::GetFilePaths(ObjectEntity* filePath) {

	auto_ptr<StringVector> filePathsFound(new StringVector());

	// make a copy of the filepath so I can switch its operation without
	// affecting the original.
	ObjectEntity tmpFilePath(filePath);

#ifdef WIN32
	// On windows, always ignore case sensitivity.
	// I implement this by forcibly changing the op so
	// we get the desired behavior out of EntityComparator.
	if (tmpFilePath.GetOperation() == OvalEnum::OPERATION_NOT_EQUAL)
		tmpFilePath.SetOperation(OvalEnum::OPERATION_CASE_INSENSITIVE_NOT_EQUAL);
	else if (tmpFilePath.GetOperation() == OvalEnum::OPERATION_EQUALS)
		tmpFilePath.SetOperation(OvalEnum::OPERATION_CASE_INSENSITIVE_EQUALS);
#endif

	StringVector filePathsToSearch;
	tmpFilePath.GetEntityValues(filePathsToSearch);

	for (StringVector::iterator iter = filePathsToSearch.begin();
		iter != filePathsToSearch.end();
		++iter) {

		switch (filePath->GetOperation()) {

		case OvalEnum::OPERATION_EQUALS:
			// due to the op change above, this won't be executed
			// on windows... but the case-insensitive impl is
			// identical.
			if (this->FilePathExists(*iter))
				filePathsFound->push_back(*iter);
			break;

		case OvalEnum::OPERATION_CASE_INSENSITIVE_EQUALS:
#ifdef WIN32
			// under windows, this works exactly as op=equals
			if (this->FilePathExists(*iter))
				filePathsFound->push_back(*iter);
			break;
#else
			// *nix is case-sensitive.  To do a case-insensitive existence
			// check, we have to get fancy.
			// TODO: implement this for *nix!
#endif

		default:
			// all other ops require searching.
			this->GetFilePathsForOperation(*iter, filePathsFound.get(), 
				tmpFilePath.GetOperation());
		}
	}

	if (tmpFilePath.GetVarRef() != NULL) {
		// only keep filepaths that match operation and value and var check
		ItemEntity tmp("filepath","", OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS);
		StringVector::iterator it;
		for(it = filePathsFound->begin(); it != filePathsFound->end(); ) {
			tmp.SetValue(*it);
			if(tmpFilePath.Analyze(&tmp) != OvalEnum::RESULT_TRUE)
				it = filePathsFound->erase(it);
			else
				++it;
		}
	}

	return filePathsFound.release();
}

bool AbsFileFinder::ReportPathDoesNotExist(ObjectEntity *path, StringVector* paths) {

	bool result = false;
	
	if(path->GetOperation() == OvalEnum::OPERATION_EQUALS) {		
		
		if(path->GetVarRef() == NULL) {
			if(!this->PathExists(path->GetValue())) {
				paths->push_back(path->GetValue());
				result = true;
			}
		} else {

			VariableValueVector::iterator iterator;
			for(iterator = path->GetVarRef()->GetValues()->begin(); iterator != path->GetVarRef()->GetValues()->end(); iterator++) {
				if(!this->PathExists((*iterator)->GetValue())) {
					paths->push_back((*iterator)->GetValue());
					result = true;
				}
			}
		}
	}

	return  result;
}

bool AbsFileFinder::ReportFileNameDoesNotExist(string path, ObjectEntity *fileName, StringVector *fileNames) {

	bool result = false;
	
	if(fileName->GetOperation() == OvalEnum::OPERATION_EQUALS && !fileName->GetNil()) {		
		
		if(fileName->GetVarRef() == NULL) {
			if(!this->FileNameExists(path, fileName->GetValue())) {
				fileNames->push_back(fileName->GetValue());
				result = true;
			}
		} else {

			
			VariableValueVector::iterator iterator;
			for(iterator = fileName->GetVarRef()->GetValues()->begin(); iterator != fileName->GetVarRef()->GetValues()->end(); iterator++) {
				if(!this->FileNameExists(path, (*iterator)->GetValue())) {
					fileNames->push_back((*iterator)->GetValue());
					result = true;
				}
			}
		}
	}

	return  result;
}

bool AbsFileFinder::ReportFilePathDoesNotExist(ObjectEntity *filePath, StringVector* filePaths) {

	bool result = false;
	
	if(filePath->GetOperation() == OvalEnum::OPERATION_EQUALS) {		
		
		if(filePath->GetVarRef() == NULL) {
			if(!this->FilePathExists(filePath->GetValue())) {
				filePaths->push_back(filePath->GetValue());
				result = true;
			}
		} else {

			VariableValueVector::iterator iterator;
			for(iterator = filePath->GetVarRef()->GetValues()->begin(); iterator != filePath->GetVarRef()->GetValues()->end(); iterator++) {
				if(!this->FilePathExists((*iterator)->GetValue())) {
					filePaths->push_back((*iterator)->GetValue());
					result = true;
				}
			}
		}
	}

	return  result;
}

//****************************************************************************************//
//								FileFinderException Class								  //	
//****************************************************************************************//
FileFinderException::FileFinderException(string errMsgIn, int severity, Exception* ex) : Exception(errMsgIn, severity, ex) {

}

FileFinderException::~FileFinderException() {

}
