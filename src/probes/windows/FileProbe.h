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

#ifndef FILEPROBE_H
#define FILEPROBE_H

#pragma warning(disable:4786)

#include <windows.h>
#include <string>
#include <map>

#include "Item.h"
#include "Object.h"
#include "FileFinder.h"
#include "AbsProbe.h"


/**
	This class is responsible for collecting file information for windows file_objects.
*/
class FileProbe : public AbsProbe {

public:
	virtual ~FileProbe();
	
	/** Get all the files on the system that match the pattern. */
	virtual ItemVector* CollectItems(Object* object);

	/** Gets single instance of the FileProbe. Uses lazy initialization. */
	static AbsProbe* Instance();

private:
	FileProbe();

	/** The static Singleton instance of the FileProbe. */
	static FileProbe* instance;
    
	/** Return a new Item created for storing file information */
	virtual Item* CreateItem();

	/** Collectect all file attributes for the specified file on the specified path. */
	Item* GetFileAttributes(std::string path, std::string fileName, FileFinder &fileFinder);

	/** Get the version of the file. */
	void GetVersion(LPVOID versionbuf, std::string filePath, Item *item, ItemEntity* version);
	/** Get the type of the file. */
	void GetType(HANDLE hFile, std::string filePath, Item *item, ItemEntity* type, FileFinder &fileFinder);

	/* holds the language mappings for the file language data */
	static std::map<WORD,std::string> langMap;
};

#endif
