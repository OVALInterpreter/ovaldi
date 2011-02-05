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

#ifndef FILEFINDER_H
#define FILEFINDER_H

#pragma warning(disable:4786)

#include "AbsFileFinder.h"
#include "WindowsCommon.h"

#include <iostream>

using namespace std;

class  AbsFileFinder;

/**
	This class is the windows file searching implmentation used by this application
*/
class FileFinder : public AbsFileFinder {
public:
	FileFinder();
	~FileFinder();

private:

	/** Return the set of matching paths after applying behaviors.
	    Currently the only defined behaviors on windows are:
		recurse_direction and max_depth
		Both of these are implemented.
	*/
	StringVector* ProcessPathBehaviors(StringVector* paths, BehaviorVector* behaviors);

	/** Return true if the specified path exists. */
	bool PathExists(string path);

	/** Return true if the specified file exists on the specifeid path. */
	bool FileNameExists(string path, string fileName);

	/** This function gets all file names that match a given pattern on the spaecified path.
	    If a match is found the filename is pushed on to a vector of strings.
	*/
	void GetFilesForPattern(string path, string pattern, StringVector* fileNames, bool isRegex = true, bool isFilePath = false);

	/** Search the file system for paths that match the provided regex.
	    Attempt to break off a constant portion of the path to spead up searching. 
	*/
	void FindPaths(string regex, StringVector* paths, bool isRegex = true);

	/** Get all fixed drives on the system. Return them in a string vector. */
	StringVector* GetDrives();

	/** This function gets all paths that match a given pattern.
	  	It will not attempt to match any directory that starts with a period or
	  	is named "System Volume Information". This is to avoid doing anything to the "." 
	  	and ".." files or accessing restricted direcoties.  
	  	This does call itself recursively as it must search all sub directories of dirIn.
	  	If a match is found the path is pushed on to a vector of strings.
	*/
	void GetPathsForPattern(string dirIn, string pattern, StringVector* pathVector, bool isRegex = true);

	/** Get the full path of all child directories as a StringVector. 
	    The caller is responsible for deleting the StringVector* of child paths.
	*/
	StringVector* GetChildDirectories(string path);
};

#endif