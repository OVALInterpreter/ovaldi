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

	StringVector* paths = new StringVector();

	// does this path use variables?
	if(path->GetVarRef() == NULL) {
		
		// proceed based on operation
		if(path->GetOperation() == OvalEnum::OPERATION_EQUALS) {
			if(this->PathExists(path->GetValue())) {
				paths->push_back(path->GetValue());
			}
		} else if(path->GetOperation() == OvalEnum::OPERATION_NOT_EQUAL) {
			// turn the provided path value into a negative pattern match
			// then get all that match the pattern
			this->FindPaths(path->GetValue(), paths, false);

		} else if(path->GetOperation() == OvalEnum::OPERATION_PATTERN_MATCH) {
			this->FindPaths(path->GetValue(), paths);
		}		

	} else {

		StringVector* allPaths = new StringVector();

		if(path->GetOperation() == OvalEnum::OPERATION_EQUALS) {
			// in the case of equals simply loop through all the 
			// variable values and add them to the set of all paths
			// if they exist on the system
			VariableValueVector::iterator iterator;
			for(iterator = path->GetVarRef()->GetValues()->begin(); iterator != path->GetVarRef()->GetValues()->end(); iterator++) {				
				if(this->PathExists((*iterator)->GetValue())) {
					paths->push_back((*iterator)->GetValue());
				}
			}

		} else {
			// for not equals and pattern match fetch all paths that match
			// any of the variable values. Then analyze each path found on 
			// the system against the variable values 			

			// loop through all variable values and call findFiles
			VariableValueVector* values = path->GetVariableValues();
			VariableValueVector::iterator iterator;
			for(iterator = values->begin(); iterator != values->end(); iterator++) {
				if(path->GetOperation() == OvalEnum::OPERATION_NOT_EQUAL) {
					this->FindPaths((*iterator)->GetValue(), allPaths, false);
				} else {
					this->FindPaths((*iterator)->GetValue(), allPaths, true);
				}
			}
		}

		// only keep paths that match operation and value and var check
		ItemEntity* tmp = new ItemEntity("path","", OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS);
		StringVector::iterator it;
		for(it = allPaths->begin(); it != allPaths->end(); it++) {
			tmp->SetValue((*it));
			if(path->Analyze(tmp) == OvalEnum::RESULT_TRUE) {
				paths->push_back((*it));
			}
		}
		delete tmp;
		delete allPaths;
	}

	// apply any behaviors and consolidate the results
	StringVector* behaviorPaths = this->ProcessPathBehaviors(paths, behaviors);

	// combine all the paths into a set to ensure uniqueness
	StringSet pathSet;
	StringVector::iterator it;
	for(it = paths->begin(); it != paths->end(); it++) {
		pathSet.insert((*it));
	}
	delete paths;
	for(it = behaviorPaths->begin(); it != behaviorPaths->end(); it++) {
		pathSet.insert((*it));
	}
	delete behaviorPaths;

	
	// convert back to a vector (I know this is ugly) ...
	StringVector* uniquePaths = new StringVector();
	StringSet::iterator setIterator;
	for(setIterator = pathSet.begin(); setIterator != pathSet.end(); setIterator++) {
		uniquePaths->push_back((*setIterator));
	}

	return uniquePaths;
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
	basic_string <char>::size_type index = path.find_last_of(Common::fileSeperator);

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

void AbsFileFinder::GetFilePathsForPattern(string pattern, StringVector* filePaths, bool isRegex){
	StringVector* paths = new StringVector();
	if ( isRegex ){
		StringPair* fp = Common::SplitFilePath(pattern);
		this->FindPaths(fp->first,paths,isRegex);
		if ( fp != NULL ){
			delete fp;
			fp = NULL;
		}
	}else{
		this->FindPaths(".*",paths,isRegex);
	}
	for(StringVector::iterator it = paths->begin() ; it != paths->end() ; it++){
		this->GetFilesForPattern(*it, pattern, filePaths, isRegex, true);
	}
	delete paths;
	paths = NULL;
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

StringVector* AbsFileFinder::GetFileNames(string path, ObjectEntity* fileName, BehaviorVector* behaviors) {

	StringVector* fileNames = new StringVector();

	// does this fileName use variables?
	if(fileName->GetVarRef() == NULL) {
		
		// proceed based on operation
		if(fileName->GetOperation() == OvalEnum::OPERATION_EQUALS) {
			if(this->FileNameExists(path, fileName->GetValue())) {
				fileNames->push_back(fileName->GetValue());
			}

		} else if(fileName->GetOperation() == OvalEnum::OPERATION_NOT_EQUAL) {
			// turn the provided fileName value into a negative pattern match
			// then get all that match the pattern
			this->GetFilesForPattern(path, fileName->GetValue(), fileNames, false);

		} else if(fileName->GetOperation() == OvalEnum::OPERATION_PATTERN_MATCH) {
			this->GetFilesForPattern(path, fileName->GetValue(), fileNames);
		}		

	} else {

		StringVector* allFileNames = new StringVector();

		if(fileName->GetOperation() == OvalEnum::OPERATION_EQUALS) {
			// in the case of equals simply loop through all the 
			// variable values and add them to the set of all file names
			// if they exist on the system
			VariableValueVector::iterator iterator;
			for(iterator = fileName->GetVarRef()->GetValues()->begin(); iterator != fileName->GetVarRef()->GetValues()->end(); iterator++) {
				if(this->FileNameExists(path, (*iterator)->GetValue())) {
					fileNames->push_back((*iterator)->GetValue());
				}
			}
		} else {
	
			// for not equals and pattern match fetch all files that
			// match any of the variable values.		

			// loop through all variable values and call GetFilesForPattern
			VariableValueVector* values = fileName->GetVariableValues();
			VariableValueVector::iterator iterator;
			for(iterator = values->begin(); iterator != values->end(); iterator++) {
				if(fileName->GetOperation() == OvalEnum::OPERATION_NOT_EQUAL) {
					this->GetFilesForPattern(path, (*iterator)->GetValue(), allFileNames, false);
				} else {
					this->GetFilesForPattern(path, (*iterator)->GetValue(), allFileNames, true);
				}
			}
		}

		// only keep files that match operation and value and var check
		ItemEntity* tmp = new ItemEntity("filename","", OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS);
		StringVector::iterator it;
		for(it = allFileNames->begin(); it != allFileNames->end(); it++) {
			tmp->SetValue((*it));
			if(fileName->Analyze(tmp) == OvalEnum::RESULT_TRUE) {
				fileNames->push_back((*it));
			}
		}
		delete tmp;
		delete allFileNames;
	}

	return fileNames;
}

StringVector* AbsFileFinder::GetFilePaths(ObjectEntity* filePath) {

	StringVector* filePaths = new StringVector();

	// does this filePath use variables?
	if(filePath->GetVarRef() == NULL) {
		
		// proceed based on operation
		if(filePath->GetOperation() == OvalEnum::OPERATION_EQUALS) {
			if(this->FilePathExists(filePath->GetValue())) {
				filePaths->push_back(filePath->GetValue());
			}

		} else if(filePath->GetOperation() == OvalEnum::OPERATION_NOT_EQUAL) {
			// turn the provided filePath value into a negative pattern match
			// then get all that match the pattern
			this->GetFilePathsForPattern(filePath->GetValue(), filePaths, false);

		} else if(filePath->GetOperation() == OvalEnum::OPERATION_PATTERN_MATCH) {
			this->GetFilePathsForPattern(filePath->GetValue(), filePaths, true);
		}		

	} else {

		StringVector* allfilePaths = new StringVector();

		if(filePath->GetOperation() == OvalEnum::OPERATION_EQUALS) {
			// in the case of equals simply loop through all the 
			// variable values and add them to the set of all file names
			// if they exist on the system
			VariableValueVector::iterator iterator;
			for(iterator = filePath->GetVarRef()->GetValues()->begin(); iterator != filePath->GetVarRef()->GetValues()->end(); iterator++) {
				
				if(this->FilePathExists((*iterator)->GetValue())) {
					filePaths->push_back((*iterator)->GetValue());
				}
			}

		} else {
	
			// for not equals and pattern match fetch all files that
			// match any of the variable values.		

			// loop through all variable values and call GetFilePathsForPattern
			VariableValueVector* values = filePath->GetVariableValues();
			VariableValueVector::iterator iterator;
			for(iterator = values->begin(); iterator != values->end(); iterator++) {
				if(filePath->GetOperation() == OvalEnum::OPERATION_NOT_EQUAL) {
					this->GetFilePathsForPattern((*iterator)->GetValue(), allfilePaths, false);
				} else {
					this->GetFilePathsForPattern((*iterator)->GetValue(), allfilePaths, true);
				}
			}
		}

		// only keep files that match operation and value and var check
		ItemEntity* tmp = new ItemEntity("filepath","", OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS);
		StringVector::iterator it;
		for(it = allfilePaths->begin(); it != allfilePaths->end(); it++) {
			tmp->SetValue((*it));
			if(filePath->Analyze(tmp) == OvalEnum::RESULT_TRUE) {
				filePaths->push_back((*it));
			}
		}
		delete tmp;
		delete allfilePaths;
	}

	return filePaths;
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

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

bool AbsFileFinder::IsMatch(string pattern, string value, bool isRegex) {

	bool match = false;

	if(isRegex) {
		if(this->fileMatcher->IsMatch(pattern.c_str(), value.c_str())) {
			match = true;
		}
	} else {	
		if(value.compare(pattern) != 0) {
			match = true;
		}
	}

	return match;
}

//****************************************************************************************//
//								FileFinderException Class								  //	
//****************************************************************************************//
FileFinderException::FileFinderException(string errMsgIn, int severity, Exception* ex) : Exception(errMsgIn, severity, ex) {

}

FileFinderException::~FileFinderException() {

}
