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
			if(!fileNames->empty()) {
				StringVector::iterator fileNameIt;
				for(fileNameIt = fileNames->begin(); fileNameIt != fileNames->end(); fileNameIt++) {
					StringPair* filePath = new StringPair((*pathIt), (*fileNameIt));
					filePaths->push_back(filePath);
				}
			}
			else {	//path exists, file name given but not found
					StringPair* filePath = new StringPair((*pathIt), "");
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
			// Windows: due to the op change above, this won't be executed.
			if (this->PathExists(*iter))
				pathsFound->push_back(*iter);
			break;

		case OvalEnum::OPERATION_CASE_INSENSITIVE_EQUALS:
			this->PathExistsCaseInsensitive(*iter, pathsFound.get());
			break;

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
		paths->push_back(*childDirectory);
		if(maxDepth == -1) {

			// make recursive call 
			this->DownwardPathRecursion(paths, *childDirectory, maxDepth);

		} else if(maxDepth > 0) {

			// make recursive call
			this->DownwardPathRecursion(paths, *childDirectory, maxDepth - 1);
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
#ifdef WIN32
	string regex = "^[A-Za-z]:\\\\$";
#endif
	if(path == Common::fileSeperatorStr
#ifdef WIN32
		|| this->fileMatcher->IsMatch(regex.c_str(), path.c_str())
#endif
		) {
		return;
	}

	// find the last file seperator
	string::size_type index = path.find_last_of(Common::fileSeperator);

	// if no path seperator is found stop recursing up.
	if(index == string::npos)
		return;

	string parentDirectory = Common::StripTrailingSeparators(path.substr(0, index));

	// store parent directory
	paths->push_back(parentDirectory);
	if(maxDepth == -1) {

		// make recursive call 
		this->UpwardPathRecursion(paths, parentDirectory, maxDepth);

	} else if(maxDepth > 0) {

		// make recursive call
		this->UpwardPathRecursion(paths, parentDirectory, maxDepth - 1);
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

bool AbsFileFinder::FilePathExists(string filePath, string *actualFilePath) {
	bool exists = false;
	auto_ptr<StringPair> fpComponents(Common::SplitFilePath(filePath));
	if ( fpComponents.get() != NULL ) {
		string actualFileName, actualPath;
		exists = this->PathExists(fpComponents->first, &actualPath);
		exists &= this->FileNameExists(actualPath, fpComponents->second, &actualFileName);
		if (exists && actualFilePath != NULL)
			*actualFilePath = Common::BuildFilePath(actualPath, actualFileName);
	}

	return exists;
}

bool AbsFileFinder::FilePathExistsCaseInsensitive(string filePath, StringVector *matchingFilePaths) {

#ifdef WIN32
	// on windows, delegate to FilePathExists, since on that platform, the case-sensitive
	// and case-insensitive methods should behave identically.  And the case-sensitive
	// version does no searching, which is faster.
	string tmpFilepath;
	bool found = FilePathExists(filePath, &tmpFilepath);
	if (found && matchingFilePaths)
			matchingFilePaths->push_back(tmpFilepath);

	return found;
#else

	auto_ptr<StringPair> fpComponents(Common::SplitFilePath(filePath));
	bool found;

	if ( fpComponents.get() != NULL ) {
		StringVector matchingPaths;

		this->PathExistsCaseInsensitive(fpComponents->first, &matchingPaths);

		for (StringVector::iterator iter = matchingPaths.begin();
			 iter != matchingPaths.end();
			 ++iter)
			if (matchingFilePaths) {
				// I guess we shouldn't assume the vector started out empty!
				size_t sizeBefore = matchingFilePaths->size();
				this->GetFilesForOperation(*iter, filePath, matchingFilePaths,
								OvalEnum::OPERATION_CASE_INSENSITIVE_EQUALS,
								true);
				found = matchingFilePaths->size() > sizeBefore;
			} else {
				// in theory, since in this case the caller indicated he 
				// didn't care what files matched, just whether there was a
				// match, we don't need to find all matches... but for
				// simplicity's sake, for now, I'll just go through the
				// regular search process.
				StringVector tmp;
				this->GetFilesForOperation(*iter, filePath, matchingFilePaths,
								OvalEnum::OPERATION_CASE_INSENSITIVE_EQUALS,
								true);
				found = !tmp.empty();
			}
	}

	return found;
#endif
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
	string actualFileName;
	tmpFileName.GetEntityValues(fileNamesToSearch);

	for (StringVector::iterator iter = fileNamesToSearch.begin();
		iter != fileNamesToSearch.end();
		++iter) {

		switch (tmpFileName.GetOperation()) {

		case OvalEnum::OPERATION_EQUALS:
			// Windows: due to the op change above, this won't be executed.
			if (this->FileNameExists(path, *iter, &actualFileName))
				fileNamesFound->push_back(actualFileName);
			break;

#ifdef WIN32
			// On windows, this is a simple existence check.  On *nix, it
			// requires searching, and the same GetFilesForOperation() method
			// will actually work in this case.  So on *nix we can skip this
			// case altogether and drop to the default.
		case OvalEnum::OPERATION_CASE_INSENSITIVE_EQUALS:
			if (this->FileNameExists(path, *iter, &actualFileName))
				fileNamesFound->push_back(actualFileName);
			break;
#endif

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

	// make a copy of the filepath so I can switch the operation on the
	// copy without affecting the original.
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
	string actualFilePath;
	tmpFilePath.GetEntityValues(filePathsToSearch);

	for (StringVector::iterator iter = filePathsToSearch.begin();
		iter != filePathsToSearch.end();
		++iter) {

		switch (tmpFilePath.GetOperation()) {

		case OvalEnum::OPERATION_EQUALS:
			// Windows: due to the op change above, this won't be executed.
			if (this->FilePathExists(*iter, &actualFilePath))
				filePathsFound->push_back(actualFilePath);
			break;

		case OvalEnum::OPERATION_CASE_INSENSITIVE_EQUALS:
			this->FilePathExistsCaseInsensitive(*iter, filePathsFound.get());
			break;

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
	
	if(path->GetOperation() == OvalEnum::OPERATION_EQUALS ||
		path->GetOperation() == OvalEnum::OPERATION_CASE_INSENSITIVE_EQUALS) {		
		
		if(path->GetVarRef() == NULL) {
			paths->push_back(path->GetValue());
			result = true;
		} else {

			VariableValueVector::iterator iterator;
			for(iterator = path->GetVarRef()->GetValues()->begin(); iterator != path->GetVarRef()->GetValues()->end(); iterator++) {
				paths->push_back((*iterator)->GetValue());
				result = true;
			}
		}
	}

	return  result;
}

bool AbsFileFinder::ReportFileNameDoesNotExist(string path, ObjectEntity *fileName, StringVector *fileNames) {

	bool result = false;
	
	if((fileName->GetOperation() == OvalEnum::OPERATION_EQUALS ||
		fileName->GetOperation() == OvalEnum::OPERATION_CASE_INSENSITIVE_EQUALS) &&
		!fileName->GetNil()) {		
		
		if(fileName->GetVarRef() == NULL) {
			fileNames->push_back(fileName->GetValue());
			result = true;
		} else {

			
			VariableValueVector::iterator iterator;
			for(iterator = fileName->GetVarRef()->GetValues()->begin(); iterator != fileName->GetVarRef()->GetValues()->end(); iterator++) {
				fileNames->push_back((*iterator)->GetValue());
				result = true;
			}
		}
	}

	return  result;
}

bool AbsFileFinder::ReportFilePathDoesNotExist(ObjectEntity *filePath, StringVector* filePaths) {

	bool result = false;
	
	if(filePath->GetOperation() == OvalEnum::OPERATION_EQUALS ||
		filePath->GetOperation() == OvalEnum::OPERATION_CASE_INSENSITIVE_EQUALS) {		
		
		if(filePath->GetVarRef() == NULL) {
			filePaths->push_back(filePath->GetValue());
			result = true;
		} else {

			VariableValueVector::iterator iterator;
			for(iterator = filePath->GetVarRef()->GetValues()->begin(); iterator != filePath->GetVarRef()->GetValues()->end(); iterator++) {
				filePaths->push_back((*iterator)->GetValue());
				result = true;
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
