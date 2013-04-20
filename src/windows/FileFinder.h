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

#ifndef FILEFINDER_H
#define FILEFINDER_H

#include <string>
#include <Windows.h> // for basic types like DWORD and HANDLE

#include "ObjectEntity.h"
#include "Behavior.h"
#include "AbsFileFinder.h"
#include "BitnessView.h"

/**
	This class is the windows file searching implmentation used by this application
*/
class FileFinder : public AbsFileFinder {
public:

	explicit FileFinder(BitnessView view);
	~FileFinder();

	/**
	 * Overrides AbsFileFinder::SearchFiles(ObjectEntity*,ObjectEntity*,BehaviorVector*) to
	 * disable wow64 redirection if necessary, before calling the superclass method, and
	 * re-enable it afterword.
	 */
	StringPairVector* SearchFiles(ObjectEntity* path, ObjectEntity* fileName, BehaviorVector* behaviors);
	/**
	 * Overrides AbsFileFinder::SearchFiles(ObjectEntity*) to disable wow64 redirection 
	 * if necessary, before calling the superclass method, and re-enable it afterword.
	 */
	StringPairVector* SearchFiles(ObjectEntity* filePath);

	/**
	 * Gets a handle to a file or directory, to be used for whatever you want.
	 * \p access is passed as the second parameter to CreateFile().  It
	 * defaults to zero, which means you don't actually need access to the
	 * contents of the file, just some other metadata about it (whatever
	 * access 0 allows). If you need to read the contents of the file,
	 * specify a different access value.  Access to the security descriptor
	 * requires READ_CONTROL access.
	 *
	 * \param filepath the path to the file or directory to get a handle to
	 * \param access the desired access
	 * \param isDirectory set to true if filepath refers to a directory.  This
	 *   is necessary because the API is slightly different for getting a
	 *   directory handle, as opposed to a file handle.
	 * \return A handle.  On error, INVALID_HANDLE_VALUE is returned; call
	 *   GetLastError() for an error code.
	 */
	HANDLE GetFileHandle(const std::string &filepath, DWORD access = 0, bool isDirectory = false);

	/** Returns the filesystem view this finder queries. */
	BitnessView GetView() const {
		return bitnessView;
	}

private:

	/** Return the set of matching paths after applying behaviors.
	    Currently the only defined behaviors on windows are:
		recurse_direction and max_depth
		Both of these are implemented.
	*/
	virtual StringVector* ProcessPathBehaviors(StringVector* paths, BehaviorVector* behaviors);

	/** Return true if the specified path exists.  \see AbsFileFinder::PathExists 
	 * And make sure the param defaults are the same here as in AbsFileFinder... :-P
	 */
	virtual bool PathExists(const std::string &path, std::string *actualPath = NULL);

	/**
	 * \see AbsFileFinder::PathExistsCaseInsensitive
	 */
	virtual void PathExistsCaseInsensitive(const std::string &path, 
										   StringVector *pathsFound);

	/** Return true if the specified file exists on the specifeid path. */
	virtual bool FileNameExists(std::string path, std::string fileName, std::string *actualFileName = NULL);

	/** This function gets all file names that match a given pattern on the spaecified path.
	    If a match is found the filename is pushed on to a vector of strings.
	*/
	virtual void GetFilesForOperation(std::string path, std::string pattern, StringVector* fileNames, OvalEnum::Operation op, bool isFilePath);

	/** Search the file system for paths that match the provided regex.
	    Attempt to break off a constant portion of the path to spead up searching. 
	*/
	virtual void FindPaths(std::string queryVal, StringVector* paths, OvalEnum::Operation op);

	/** Get all fixed drives on the system. Return them in a string vector. */
	StringVector* GetDrives();

	/** This function gets all paths that match a given pattern.
	  	It will not attempt to match any directory that starts with a period or
	  	is named "System Volume Information". This is to avoid doing anything to the "." 
	  	and ".." files or accessing restricted direcoties.  
	  	This does call itself recursively as it must search all sub directories of dirIn.
	  	If a match is found the path is pushed on to a vector of strings.
		The \p dirIn path, which is the directory being searched, should be capitalized
		how it is in the filesystem.  Since FindFirst/NextFile() returns files
		in the proper case, there is no need internally to pass all paths through 
		WindowsCommon::GetActualPathWithCase() as this method recurses (and it is
		more efficient not to), however the initial call to this method to start off
		the recursive search should have dirIn capitalized properly.  Otherwise all
		the found paths will have an incorrectly capitalized prefix.
	*/
	void GetPathsForOperation(std::string dirIn, std::string pattern, StringVector* pathVector, OvalEnum::Operation op);

	/** Get the full path of all child directories as a StringVector. 
	    The caller is responsible for deleting the StringVector* of child paths.
	*/
	virtual StringVector* GetChildDirectories(std::string path);

	BitnessView bitnessView;
};

#endif