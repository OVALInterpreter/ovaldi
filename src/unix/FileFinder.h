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

#ifndef FILEFINDER_H
#define FILEFINDER_H

#include <string>

#include "StdTypedefs.h"
#include "AbsFileFinder.h"

/**
	This class is the linux file searching implmentation used by this application
*/
class FileFinder : public AbsFileFinder {
public:
	FileFinder();
	~FileFinder();

private:
	/** Return the set of matching paths after applying behaviors.
	    The only defined behaviors for unix files are:
		recurse_direction, max_depth, recurse, and recurse_file_system
		Currently only recurse_direction and max_depth are implemented.
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

	virtual bool FileNameExists(std::string path, std::string fileName, std::string *actualFileName = NULL);

	virtual void FindPaths(std::string queryVal, StringVector* paths, OvalEnum::Operation op);

	virtual void GetFilesForOperation(std::string path, std::string queryVal, StringVector* fileNames, OvalEnum::Operation op, bool isFilePath = false);

	void GetPathsForOperation(std::string dirIn, std::string queryVal, StringVector* pathVector, OvalEnum::Operation op);

	/** Get the full path of all child directories as a StringVector. 
	    The caller is responsible for deleting the StringVector* of child paths.
	*/
	virtual StringVector* GetChildDirectories(std::string path);
};

#endif
