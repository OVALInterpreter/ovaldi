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

#ifndef ABSFILEFINDER_H
#define ABSFILEFINDER_H

#ifdef WIN32
	#pragma warning(disable:4786)
#endif

#include "Log.h"
#include "Common.h"
#include "Exception.h"
#include "REGEX.h"
#include "ObjectEntity.h"
#include "ItemEntity.h"

#include <string>
#include <vector>

/**
	This class provides a common mechanism for searching for files.
	Searches are based on a provided path and file name where each may contain
	operations like equal, not equal, and pattern match.
*/
class AbsFileFinder {
public:
	/** AbsFileFinder constructor. */
	AbsFileFinder();

	/** AbsFileFinder destructor. */
	virtual ~AbsFileFinder();

	/** Return the set of filepaths as a vector of StringPairs that match the specified criteria. 
		If a path exists and matches the path criteria it will be added to the result vector. If no 
		file exist under that path the second item in the pair will be set to NULL. The caller can then 
		determine if the filename is NULL because it was set to nil in the xml or there were no matching
		filenames at the path. The caller should check use ObjetEntity::GetNil() to determine if the 
		filename is set to nill. If not set to nill then it is safe to assume that the return value was 
		set to NULL because no matching filenames were found at the path.

		See also AbsFileFinder::ReportFileNameDoesNotExist(std::string path, ObjectEntity* fileName) to 
		determine if the fact the the filename was not found should be reported by the calling probe.
	*/
	StringPairVector* SearchFiles(ObjectEntity* path, ObjectEntity* fileName, BehaviorVector* behaviors);

	/** Return the set of filepaths as a vector of StringPairs that match the specified filepath ObjectEntity.
	 *  @param filePath A ObjectEntity that represents the filepath(s) to retrieve. 
	 *  @return A StringPairVector that contains all of matching filepaths where a StringPair represents the path and filename components of a matching filepath.
	 */
	StringPairVector* SearchFiles(ObjectEntity* filePath);

	/** Return true if the calling probe should report that the path does not exist.
		If a path's operator is set to OPERATOR_EQUALS and the path does not exist
		the caller should report that the path was not found. When getting the value 
		of the path to check existence need to look for either a simple element value
		or a variable with one or more values. If the return value is true the paths
		StringVector* paremeter contains the set of paths to report as not existing. 
		Otherwise the the paths paramter is NULL. The caller is responsible for making sure
		that any memory allocated for the paths parameter is cleaned up. The paths paramter
		should be input as NULL when the function is called.
	*/
	bool ReportPathDoesNotExist(ObjectEntity* path, StringVector* paths);

	/** Return true if the calling probe should report that the filename does not exist.
		If a filename's operator is set to OPERATOR_EQUALS and the file name does not exist
		at the specified path the caller should report that the path was not found. When 
		getting the value  of the filename to check existence need to look for either a simple 
		element value or a variable with one or more values. If the return value is true the fileNames
		StringVector* paremeter contains the set of fileNames to report as not existing. 
		Otherwise the the fileNames paramter is NULL. The caller is responsible for making sure
		that any memory allocated for the fileNames parameter is cleaned up. The fileNames paramter
		should be input as NULL when the function is called.
	*/
	bool ReportFileNameDoesNotExist(std::string path, ObjectEntity* fileName, StringVector* fileNames);

	/** Return true if the calling probe should report that the filepath does not exist.
	 *  @param filePath A ObjectEntity that represents the filePath entity in an Object as defined in the OVAL Definition Schema.
	 *  @param filePaths A StringVector that will be used to store all of the filepaths specified in the ObjectEntity that do not exist on the system.
	 *  @return A boolean value that indicates whether or not there are filepaths that do not exist on the system that need to be reported.
	 */
	bool ReportFilePathDoesNotExist(ObjectEntity* filePath, StringVector* filePaths);

protected:
	
	/** Return a vector of paths that match the specified criteria. */
	StringVector* GetPaths(ObjectEntity* path, BehaviorVector* behaviors);

	/** Return a vector of file names that match the specified criteria. 
		Search the file system for files under the specified path that match the 
	  	provided ObjectEntity.
	*/
	StringVector* GetFileNames(std::string path, ObjectEntity* fileName);

	/** Return a StringVector of filepaths that match the specified filePath ObjectEntity. 
	 *  @param filePath A ObjectEntity that represents the filepath(s) to retrieve.
	 *  @return A StringVector that contains all of the matching filepaths specified by the filePath ObjectEntity.
	 */
	StringVector* GetFilePaths(ObjectEntity* filePath);	

	/** Return the set of matching paths after applying behaviors */
	virtual StringVector* ProcessPathBehaviors(StringVector* paths, BehaviorVector* behaviors) = 0;

	/**
	 * Return true if the specified directory exists.  If it exists, the actual 
	 * path, capitalized as it is in the filesystem, is assigned to *actualPath, if
	 * actualPath is non-NULL.  The actualPath out-param is specifically for
	 * supporting windows, which has a case-insensitive but case-aware filesystem.
	 * \param[in] path The directory to check
	 * \param[out] actualPath If not NULL, receives the directory capitalized how
	 *   it is in the filesystem.
	 * \return true if the path existed, false if not
	 */
	virtual bool PathExists(const std::string &path, std::string *actualPath = NULL) = 0;

	/**
	 * On *nix, searches for case-insensitive matches to the given path, and
	 * collects them into the given vector.  On windows this does
	 * not perform a search; it is merely an existence check, since its 
	 * filesystem is natively case-insensitive.  On windows, if the path
	 * exists, it will be added to the given vector, capitalized how it
	 * actually is in the filesystem.
	 */
	virtual void PathExistsCaseInsensitive(const std::string &path, 
										   StringVector *pathsFound) = 0;
	
	/**
	 * Return true if the specified file exists.  The path is assumed to exist.
	 * If the file exists, the actual filename capitalized as it is in the 
	 * filesystem, is assigned to *actualFileName, if actualFileName is non-NULL.  The 
	 * actualFileName out-param is specifically for supporting windows, which has 
	 * a case-insensitive but case-aware filesystem.
	 * \param[in] path The directory to check
	 * \param[in] fileName the name of the file in directory \p path to check
	 * \param[out] actualFileName If not NULL, receives the file name capitalized how
	 *   it is in the filesystem.
	 * \return true if the file existed, false if not
	 */
	virtual bool FileNameExists(std::string path, std::string fileName, std::string *actualFileName = NULL) = 0;
	
	/** Get the set of all paths that match the specified pattern. */
	virtual void FindPaths(std::string queryVal, StringVector* paths, OvalEnum::Operation op) = 0;
	
	/** Get the set of files in the specified directory that match the specified pattern. */
	virtual void GetFilesForOperation(std::string path, std::string queryVal, StringVector* fileNames, OvalEnum::Operation op, bool isFilePath = false) = 0;
	
	/** Get the full path of the parent directory as a string. */
	//virtual std::string GetParentDirectory(std::string path) = 0;
	
	/** Get the full path of all child directories as a StringVector. */
	virtual StringVector* GetChildDirectories(std::string path) = 0;

	/** Do a recusive search down the file system until the specified maxDepth is hit. 
	    Each directory traversed is added to the set of paths. If maxDepth is a positive
		number it is decremented with each recursive call and recursion stops when the 
		maxDepth is 0. If maxDepth is -1 recursion continues until no child directories 
		are found and a recursive call can not be made. */
	void DownwardPathRecursion(StringVector* paths, std::string path, int maxDepth);
	
	/** Do a recusive search up the file system until the specified maxDepth is hit.
	    Each directory traversed is added to the set of paths. f maxDepth is a positive
		number it is decremented with each recursive call and recursion stops when the 
		maxDepth is 0. If maxDepth is -1 recursion continues until no parent directory 
		is found and a recursive call can not be made. */
	void UpwardPathRecursion(StringVector* paths, std::string path, int maxDepth);

	/** Get the set of filepaths that match the specified pattern. 
	 *  @param pattern A string that contains the regular expression to be used during the matching process.
	 *  @param filePaths A StringVector that will be used to store all of the matching filepaths.
	 *  @param isRegex A boolean value indicating whether or not the pattern represents a regular expression.
	 *  @return Void.
	 */
	void GetFilePathsForOperation(std::string queryVal, StringVector* filePaths, OvalEnum::Operation op);
	
	/**
	 * Return true if the specified filepath exists.  If the filepath exists, the actual
	 * path capitalized as it is in the filesystem, is assigned to *actualFilePath, if
	 * actualFilePath is non-NULL.  The actualFilePath out-param is specifically for 
	 * supporting windows, which has a case-insensitive but case-aware filesystem.
	 * \param[in] filePath the filepath to check
	 * \param[out] actualFilePath If not NULL, receives the filepath capitalized how
	 *   it is in the filesystem.
	 * \return true if the filepath existed, false if not
	 */
	bool FilePathExists(std::string filePath, std::string *actualFilePath = NULL);

	/**
	 * On *nix, searches for case-insensitive matches to the given filepath, and
	 * collects them into the given vector.  On windows this does
	 * not perform a search; it is merely an existence check, since its 
	 * filesystem is natively case-insensitive.  On windows, if the filepath
	 * exists, it will be added to the given vector, capitalized how it
	 * actually is in the filesystem.
	 */
	bool FilePathExistsCaseInsensitive(std::string filePath, StringVector *matchingFilePaths = NULL);

	REGEX *fileMatcher;

};

/** 
	This class represents an Exception that occured while useing a FileFinder.
*/
class FileFinderException : public Exception {
	public:
		FileFinderException(std::string errMsgIn = "", int severity = ERROR_FATAL, Exception* ex = NULL);
		~FileFinderException();
};

#endif
