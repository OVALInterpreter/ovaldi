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

#ifndef PARTITIONPROBE_H
#define PARTITIONPROBE_H

#include <map>
#include <string>
#include "AbsProbe.h"
#include <Item.h>
#include <Object.h>
#include <REGEX.h>

/**
 * Implements the partition_test.  This gives information regarding mounted
 * filesystems.  UUID's are obtained via the cache maintained by the libblkid
 * library.  So UUID's won't be successfully collected unless that library is
 * installed and the cache exists.
 */
class PartitionProbe : public AbsProbe {
public:
	virtual ~PartitionProbe();       

	virtual ItemVector* CollectItems(Object* object);

	/** Ensure that the PartitionProbe is a singleton. */
	static AbsProbe* Instance();

private:
	PartitionProbe();
    
	virtual Item* CreateItem();

	void CachePartitions();

	/**
	 * Mount point paths as found in the mount point file (e.g. /etc/mtab) can
	 * have octal escape sequences in them.  This method replaces all such
	 * sequences with the actual character, to obtain the real path used to look
	 * into the filesystem.  An escape sequence is assumed to be of the form
	 * '\xxx' where xxx are 3 octal digits.
	 */
	void DecodeMtabMountPoint(std::string *mountPoint);

	/**
	 * This constitutes a snapshot of the mounts at the time this probe
	 * is instantiated.
	 */
	ItemVector cachedPartitionItems;

	typedef std::map<unsigned int, std::string> FsTypeMapType;

	/**
	 * Maps a fs_type value as returned from a statfs buffer to
	 * an oval enumerator value.
	 */
	FsTypeMapType fstypeMap;
	REGEX re;

	static PartitionProbe *instance;
};

#endif
