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

#include <cassert>
#include <DirGuard.h>
#include "FileFinder.h"

using namespace std;

namespace {
	/**
	 * This is applicable when op=case insensitive equals.  On unix systems,
	 * this implies searching, since their file systems are case-sensitive.
	 *
	 * \param pathToCompare[in] the complete path we are trying to find
	 * 	case-insensitive matches for.
	 * \param currPath[in] an actual path in the filesystem to begin the
	 * 	search.  This should correspond to \p pathToCompare or some ancestor
	 * 	directory of it.
	 * \param pathCompIdx[in] the index of the first character in the path
	 * 	component of \p pathToCompare which corresponds to the current recursion
	 * 	level.  This must not refer to a separator character.
	 * \param pathsFound[out] accumulates matching paths.
	 */
	void FindPathsCaseInsensitive(const string &pathToCompare,
								  const string &currPath,
								  size_t pathCompIdx,
								  StringVector *pathsFound);
}

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

	// We only need to check the recurse, recurse_file_systems, and max_depth behaviors
	// if recurse_direction is not equal to "none".
	if ( recurseDirection.compare("none") != 0 ){

		string maxDepthStr = Behavior::GetBehaviorValue(behaviors, "max_depth");
		int maxDepth = -1;
		if(maxDepthStr.compare("") != 0) {
			maxDepth = atoi(maxDepthStr.c_str());
			if(maxDepth < -1) 
				maxDepth = -1;
		}

		string recurseStr = Behavior::GetBehaviorValue(behaviors, "recurse");
		if(recurseStr.compare("") != 0) {
			throw ProbeException("Unsupported behavior: recurse");
		}

		string recurse_file_systemStr = Behavior::GetBehaviorValue(behaviors, "recurse_file_system");
		if(recurse_file_systemStr.compare("") != 0) {
			throw ProbeException("Unsupported behavior: recurse_file_system");
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
//void FileFinder::FindPaths(string regex, StringVector* paths, bool isRegex) {
	// -----------------------------------------------------------------------
	//
	//  ABSTRACT
	//
	//	Search the file system for paths that match the provided regex.
	//	Attempt to break off a constant portion of the path to spead up searching
	// -----------------------------------------------------------------------

	string fileName = "";

	string patternOut= "";
	string constPortion= "";
	string fileSeperatorStr = Common::fileSeperatorStr;

	// This optimization only applies when the regex is anchored to
	// the beginning of paths. (regex has to start with '^')
	if (op == OvalEnum::OPERATION_PATTERN_MATCH && !queryVal.empty() && queryVal[0] == '^') {
		this->fileMatcher->GetConstantPortion(queryVal, Common::fileSeperator, &patternOut, &constPortion);
		// Remove extra slashes
		constPortion = this->fileMatcher->RemoveExtraSlashes(constPortion);
	}
	
	// Found a constant portion
	if(constPortion.compare("") != 0 && patternOut.compare("") != 0) {

		//	Call search function
		this->GetPathsForPattern(constPortion, queryVal, paths, op);

		//	No constant portion.
	} else if(constPortion.compare("") == 0) { 
		
		try  {

			this->GetPathsForPattern(fileSeperatorStr, queryVal, paths, op);

		} catch(REGEXException ex) {
			if(ex.GetSeverity() == ERROR_WARN) {
				string pcreMsg = "";
				pcreMsg.append("Filefinder Warning - while searching for matching files:\n");
				pcreMsg.append("-----------------------------------------------------------------------\n");
				pcreMsg.append(ex.GetErrorMessage());
				Log::Debug(pcreMsg);
			} else {
				throw;
			}		
		}

	} else if(patternOut.compare("") == 0) {

		//	There are no pattern matching chars treat this as a normal path 
		if(this->PathExists(constPortion)) {
			paths->push_back(constPortion);
		}
	}

}

void FileFinder::GetPathsForPattern(string dirIn, string pattern, StringVector* pathVector, OvalEnum::Operation op) {
	// -----------------------------------------------------------------------
	//
	//  ABSTRACT
	//
	//  This function gets all paths that match a given pattern.
	//	This does call itself recursively as it must search all sub directories of dirIn.
	//	If a match is found the path is pushed on to a vector of strings.
	//
	// -----------------------------------------------------------------------

	try {

		struct stat statbuf;
		struct dirent *dirp;
		string tmp;

		//	Call stat 
		if(lstat(dirIn.c_str(), &statbuf) < 0) {
			//if(errno == ENOTDIR) {
			//	throw ProbeException("A component of the path prefix is not a directory.");
			//} else if(errno == ENAMETOOLONG) {
			//	throw ProbeException("A component of a pathname exceeded {NAME_MAX} characters, or an entire path name exceeded {PATH_MAX} characters.");		
			//} else if(errno == EACCES) {
			//	throw ProbeException("Search permission is denied for a component of the path prefix.");
			//} else if(errno == ELOOP) {
			//	throw ProbeException("Too many symbolic links were encountered in translating the pathname.");
			//} else if(errno == EFAULT) {
			//	throw ProbeException("Sb or name points to an invalid address.");
			//} else if(errno == EIO) {	
			//	throw ProbeException("An I/O error occurred while reading from or writing to the file system.");
			//} else if(errno == ENOENT) {
			//	// does not exist
			//}

			return; 
		}

		// only consider dirs
		if(S_ISDIR(statbuf.st_mode)) {

			// record it if it matches the regex.
			if (EntityComparator::CompareString(op, pattern, dirIn) == OvalEnum::RESULT_TRUE)
				pathVector->push_back(dirIn);

			//	Open the directory
			DirGuard dp(dirIn, false);

			if(dp.isClosed()) {
				//	Error opening directory
				//	not sure this error matters
				// cout << "Failed to open the directory" << endl;
				return;
			}

			//if the directory doesn't end in a file separator add it

			if (dirIn.at(dirIn.length()-1) != Common::fileSeperator)
			  dirIn.append(1, Common::fileSeperator);


			//	Loop through all names in the directory and make recursive call
			while((dirp = readdir(dp)) != NULL) {
				//	Ignore dot and dot-dot
				if(strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)
					continue;

				//	append the name
			      
				tmp = dirIn + dirp->d_name;

				// Nake recursive call
				GetPathsForPattern(tmp, pattern, pathVector, op);
			}


			//	Close the directory
			if(dp.close() < 0) {
				//	Error closing the directory
				//	not sure this error matters
				// cout << "Failed to close the directory" << endl;
				return;
			}
		}

	//	Just need to ensure that all exceptions have a nice message. 
	//	So rethrow the exceptions I created catch the others and format them.
	} catch(Exception ex) {
		throw;
	} catch(...) {

		string errorMessage = "";
		errorMessage.append("Error: ");
		errorMessage.append("An unspecified error was encountered while trying to search for matching paths. \n\tDirectory: ");
		errorMessage.append(dirIn);
		errorMessage.append("\n\tPattern: ");
		errorMessage.append(pattern);
		throw FileFinderException(errorMessage);
	}
}

void FileFinder::GetFilesForPattern(string path, string pattern, StringVector* fileNames, OvalEnum::Operation op, bool isFilePath) {
//void FileFinder::GetFilesForPattern(string path, string pattern, StringVector* fileNames, bool isRegex, bool isFilePath) {
	// -----------------------------------------------------------------------
	//
	//  ABSTRACT
	//
	//  This function gets all file names that match a given pattern on the spaecified path.
	//	If a match is found the filename is pushed on to a vector of strings.
	//
	// -----------------------------------------------------------------------

	try {

		// Verify that the path that was passed into this function ends with a slash.  If
		// it doesn't, then add one.
		if (path[path.length()-1] != Common::fileSeperator)
			path.append(1, Common::fileSeperator);

		//	Open the directory
		DirGuard dp(path, false);
		if(dp.isClosed()) {
			string errorMessage = "Error opening directory " + path + ": " +
				strerror(errno);
			throw FileFinderException(errorMessage);
		}

		//	Loop through all names in the directory
		struct dirent *dirp;
		while((dirp = readdir(dp)) != NULL) {
			//	Ignore dot and dot-dot
			if(strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)
				continue;

			//	Call stat 
			struct stat statbuf;
			string filepath = path;
			filepath.append(dirp->d_name);
			if(lstat(filepath.c_str(), &statbuf) < 0) {
				//if(errno == ENOTDIR) {
				//	throw ProbeException("A component of the path prefix is not a directory.");
				//} else if(errno == ENAMETOOLONG) {
				//	throw ProbeException("A component of a pathname exceeded {NAME_MAX} characters, or an entire path name exceeded {PATH_MAX} characters.");		
				//} else if(errno == EACCES) {
				//	throw ProbeException("Search permission is denied for a component of the path prefix.");
				//} else if(errno == ELOOP) {
				//	throw ProbeException("Too many symbolic links were encountered in translating the pathname.");
				//} else if(errno == EFAULT) {
				//	throw ProbeException("Sb or name points to an invalid address.");
				//} else if(errno == EIO) {	
				//	throw ProbeException("An I/O error occurred while reading from or writing to the file system.");
				//} else if(errno == ENOENT) {
				//	throw ProbeException("Does not exist");
				//}
				// no errors to report
			}

			//	If not a directory check if a match
			if(S_ISREG(statbuf.st_mode)) {
				string fileName = dirp->d_name;
				if ( isFilePath ){
					string filepath = Common::BuildFilePath(path,fileName);
					if (EntityComparator::CompareString(op, pattern, filepath) == OvalEnum::RESULT_TRUE)
						fileNames->push_back(filepath);
				}else{
					if (EntityComparator::CompareString(op, pattern, fileName) == OvalEnum::RESULT_TRUE)
						fileNames->push_back(fileName);
				}
			}
		}

		//	Close the directory
		if(dp.close() < 0) {
			//	Error closing the directory
			//	not sure this error matters
			// cout << "Failed to close the directory" << endl;
			return;
		}

	//	Just need to ensure that all exceptions have a nice message. 
	//	So rethrow the exceptions I created catch the others and format them.
	} catch(Exception ex) {

		throw;

	} catch(...) {

		string errorMessage = "";
		errorMessage.append("Error: ");
		errorMessage.append("An unspecified error was encountered while trying to search for matching paths. \n\tDirectory: ");
		errorMessage.append(path);
		errorMessage.append("\n\tPattern: ");
		errorMessage.append(pattern);
		throw FileFinderException(errorMessage);
	}
}

bool FileFinder::PathExists(string path) {
	// -----------------------------------------------------------------------
	//
	//  ABSTRACT
	//
	//  Return true if the specified path exists.
	//
	// -----------------------------------------------------------------------

	struct stat st;
	return (stat(path.c_str(), &st) != -1 && S_ISDIR(st.st_mode));
}

void FileFinder::PathExistsCaseInsensitive(const string &path, StringVector *pathsFound) {
	if (path.empty()) return;

	// I am handling repeated separator chars here, for robustness' sake.
	// Find the first non-sep char...
	size_t i=0;
	while (i < path.size() && path[i] == Common::fileSeperator)
		++i;

	// if path starts with a separator char, it's absolute, and we will start our
	// search at the filesystem root.  Otherwise, start at the current
	// directory.  If it consists of only sep chars, we only check for existence
	// of the root directory (which should succeed, right?)
	if (i >= path.size()) {
		// they were all sep chars!
		if (this->PathExists(Common::fileSeperatorStr))
			pathsFound->push_back(Common::fileSeperatorStr);
	} else {
		string searchStartPath = (i==0) ? "." : Common::fileSeperatorStr;
		FindPathsCaseInsensitive(path, searchStartPath, i, pathsFound);
	}
}

bool FileFinder::FileNameExists(string path, string fileName) {
	// -----------------------------------------------------------------------
	//
	//  ABSTRACT
	//
	//  Return true if the specified file exists on the specifeid path.
	//
	// -----------------------------------------------------------------------

	bool exists = false;

	// Verify that the path that was passed into this function ends with a slash.  If
	// it doesn't, then add one.
	if (path[path.length()-1] != Common::fileSeperator)
		path.append(1, Common::fileSeperator);

	//	Call stat 
	struct stat statbuf;
	string filepath = path + fileName;
	if(stat(filepath.c_str(), &statbuf) == 0) {
		exists = true;
	}

	return exists;
}

void FileFinder::FileNameExistsCaseInsensitive(const string &path, 
											   const string &fileName, 
											   StringVector *fileNamesFound) {
	// We assume the given path is already known to exist, and just
	// search the files within it.  That's how AbsFileFinder will
	// invoke us: as the second step of a 2-step process.
	dirent *dirp;
	struct stat st;
	DirGuard dir(path, false);

	// copy behavior from GetFilesForPattern
	if(dp.isClosed()) {
		string errorMessage = "Error opening directory " + path + ": " +
			strerror(errno);
		throw FileFinderException(errorMessage);
	}
	// ignore dir open error
	if (dir.isClosed())
		return;

	while((dirp = readdir(dir)) != NULL) {
		if (!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, ".."))
			continue;

		if (Common::EqualsIgnoreCase(fileName, dirp->d_name)) {
			// ignore non-regular-files and stat() errors
			string tmpPath = Common::BuildFilePath(path, dirp->d_name);
			if (stat(tmpPath.c_str(), &st) == -1 || !S_ISREG(st.st_mode))
				continue;

			fileNamesFound->push_back(dirp->d_name);
		}
	}	
}

StringVector* FileFinder::GetChildDirectories(string path) {
	// -----------------------------------------------------------------------
	//
	//  ABSTRACT
	//
	//  This function returns the set of child directories in the specified directory.
	//	The caller is responsible for deleting the StringVector* of child paths.
	//
	// -----------------------------------------------------------------------

	StringVector* childDirs = new StringVector();
	try {

		struct stat statbuf;
		struct dirent *dirp;
		DIR *dp;

		//	Call stat 
		if(lstat(path.c_str(), &statbuf) < 0) {
			// dir does not exist
			return childDirs; 
		}

		// only consider dirs
		if(S_ISDIR(statbuf.st_mode) == 1) {

			//	Append a '/'
			if(path.at(path.length()-1) != Common::fileSeperator)
				path.append("/");

			//	Open the directory
			dp = opendir(path.c_str());
			if(dp == NULL) {
				//	Error opening directory
				//	not sure this error matters
				// cout << "Failed to open the directory" << endl;
				return childDirs;
			}

			//	Loop through all names in the directory and make recursive call
			while((dirp = readdir(dp)) != NULL) {
				//	Ignore dot and dot-dot
				if(strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)
					continue;

				//	append the name after the "/"
				string childDir = path;	
				childDir.append(dirp->d_name);
				
				childDirs->push_back(childDir);
			}


			//	Close the directory
			if(closedir(dp) < 0) {
				//	Error closing the directory
				//	not sure this error matters
				// cout << "Failed to close the directory" << endl;
				return childDirs;
			}
		}

	//	Just need to ensure that all exceptions have a nice message. 
	//	So rethrow the exceptions I created catch the others and format them.
	} catch(Exception ex) {
		throw;
	} catch(...) {

		string errorMessage = "";
		errorMessage.append("Error: An unspecified error was encountered while trying to get child directories. Parent Directory: ");
		errorMessage.append(path);
		throw FileFinderException(errorMessage);
	}

	return childDirs;
}

namespace {

	void FindPathsCaseInsensitive(const string &pathToCompare,
								  const string &currPath,
								  size_t pathCompIdx,
								  StringVector *pathsFound) {

		// make sure they didn't give us an invalid pathCompIdx...
		assert(pathCompIdx < pathToCompare.size() &&
			   pathToCompare[pathCompIdx] != Common::fileSeperator);

		size_t nextSepIdx = pathToCompare.find(Common::fileSeperator, pathCompIdx);

		// the path component we need to case insensitively match against
		string pathComp(pathToCompare, pathCompIdx, 
						nextSepIdx == string::npos ? 
							string::npos : (nextSepIdx-pathCompIdx));

		// I feel the need, for robustness' sake, to handle repeated separator
		// chars.  So I will not assume there's only one.  I search for the next
		// non-sep char (or end of string).
		size_t nextPathCompIdx = nextSepIdx;
		if (nextPathCompIdx != string::npos)
			while (nextPathCompIdx < pathToCompare.size() &&
				   pathToCompare[nextPathCompIdx] == Common::fileSeperator)
				++nextPathCompIdx;

		dirent *dirp;
		struct stat st;
		DirGuard dir(currPath, false);

		// ignore dir open error.
		if (dir.isClosed())
			return;

		while((dirp = readdir(dir)) != NULL) {
			if (!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, ".."))
				continue;

			if (Common::EqualsIgnoreCase(dirp->d_name, pathComp)) {
				// ignore non-directories and stat() errors
				string tmpPath = Common::BuildFilePath(currPath, dirp->d_name);
				if (stat(tmpPath.c_str(), &st) == -1 || !S_ISDIR(st.st_mode))
					continue;

				if (nextSepIdx == string::npos ||
					nextPathCompIdx == string::npos || // technically not necessary
					nextPathCompIdx >= pathToCompare.size()) {
					// found a complete match.  The latter
					// disjuncts above handle the case where
					// the path ends with separator char(s).
					pathsFound->push_back(tmpPath);
				} else
					// found a partial match.. need to continue
					// recursing.
					FindPathsCaseInsensitive(pathToCompare,
											 tmpPath,
											 nextPathCompIdx,
											 pathsFound);
			}
		}
	}

}
