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
#ifndef FILEPROBE_H
#define FILEPROBE_H

#include "AbsProbe.h"

#include "FileFinder.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <time.h>
#include <cerrno>

#include <dirent.h>
#include <limits.h>

#include <iostream>
#include <string>
#include <vector>

# if defined (LINUX)
  #include <acl/libacl.h> 
#endif
/**
	This class is responsible for collecting file information for red hat file_objects.
*/
class FileProbe : public AbsProbe {

public:
	virtual ~FileProbe();
	
	/** Get all the files on the system that match the pattern and collect their attributes. */
	virtual ItemVector* CollectItems(Object* object);

	/** Ensure that the FileProbe is a singleton. */
	static AbsProbe* Instance();

private:

	FileProbe();

	/** Return a new Item created for storing file information */
	virtual Item* CreateItem();

	static FileProbe* instance;

	/** Get all attributes for the file specified in fileIn. Return them in an Item. */
	Item* GetFileAttributes(std::string path, std::string fileName);	
};

#endif
