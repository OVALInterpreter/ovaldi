//
// $Id: FileHashProbe.h 4579 2008-01-02 17:39:07Z bakerj $
//
//****************************************************************************************//
// Copyright (c) 2002-2008, The MITRE Corporation
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

#ifndef FILEHASHPROBE_H
#define FILEHASHPROBE_H

#include "FileFinder.h"
#include "AbsProbe.h"
#include "MD5.h"

#ifdef WIN32
	#define SNPRINTF _snprintf
#else
	#define SNPRINTF snprintf
#endif

using namespace std;

/**
	This class is responsible for collecting file hash data.
	This class should be platform independant.
*/
class FileHashProbe : public AbsProbe {

public:
	~FileHashProbe();
	
	/** Get all the files on the system that match the pattern and generate and md5 and sha1 */
	ItemVector* CollectItems(Object* object);

	/** Return a new Item created for storing file hash information */
	Item* CreateItem();

	/** Ensure that the FileHashProbe is a singleton. */
	static AbsProbe* Instance();

private:
	FileHashProbe();

	static FileHashProbe* instance;

	/** Generates the md5 hash for a file. */
	void GetMd5(string filePath, Item* item);

	/** Generates the sha1 hash for a file. */
	void GetSha1(string filePath, Item* item);
};

#endif
