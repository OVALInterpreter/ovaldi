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

#include <cerrno>
#include <fstream>
#include <sstream>
#include <sys/statfs.h>

// we'll use libblkid to figure out UUIDs of partitions
#include <blkid/blkid.h>

#include "PartitionProbe.h"
#include <OvalMessage.h>
#include <OvalEnum.h>

using namespace std;

#define MTAB_FILE_NAME "/etc/mtab"

namespace {
	class BlkidCacheGuard {
	public:
		BlkidCacheGuard() {
			if (blkid_get_cache(&cache, NULL)) {	
				cache = NULL;
				Log::Info("Couldn't open blkid cache!");
			}
		}

		~BlkidCacheGuard() {
			if (cache)
				// it's an odd name for a function to free memory
				// or release resources, but I *think* this is the
				// function to use...
				blkid_put_cache(cache);
		}

		operator blkid_cache() {
			return cache;
		}

		bool wasCacheOpenSuccessful() const {
			return cache != NULL;
		}

	private:
		blkid_cache cache;
	};
}


//****************************************************************************************//
//								PartitionProbe Class									  //	
//****************************************************************************************//
PartitionProbe *PartitionProbe::instance = NULL;

PartitionProbe::PartitionProbe() {

	this->CachePartitions();
}

PartitionProbe::~PartitionProbe() {
	for (ItemVector::iterator iter = cachedPartitionItems.begin();
		 iter != cachedPartitionItems.end();
		 ++iter)
		delete *iter;

  instance = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* PartitionProbe::Instance() {

	// Use lazy initialization
	if(instance == NULL) 
		instance = new PartitionProbe();

	return instance;	
}

ItemVector* PartitionProbe::CollectItems(Object* object) {

	ItemVector *collectedItems = new ItemVector();
	
	for (ItemVector::iterator iter = this->cachedPartitionItems.begin();
		 iter != this->cachedPartitionItems.end();
		 ++iter)
		if (object->Analyze(*iter))
			collectedItems->push_back(new Item(**iter));

	return collectedItems;
}  

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
Item* PartitionProbe::CreateItem() {
	// -----------------------------------------------------------------------
	//
	//  ABSTRACT
	//
	//  Return a new Item created for storing file information
	//
	// -----------------------------------------------------------------------

	Item* item = new Item(0, 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#linux", 
						"linux-sc", 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#linux linux-system-characteristics-schema.xsd", 
						OvalEnum::STATUS_ERROR, 
						"partition_item");

	return item;
}

void PartitionProbe::CachePartitions() {
	ifstream in(MTAB_FILE_NAME);
	string line;
	string device, mountPoint, mountOpts, fsType;
	struct statfs buf;
	BlkidCacheGuard blkidCache;

	if (!in)
		throw ProbeException("Couldn't read" MTAB_FILE_NAME "!");

	while(getline(in, line)) {
		istringstream iss(line);
		vector<StringVector> mountOptMatches;

		// I assume here that each line of the mtab file is space-delimited,
		// with no embedded spaces within a field value... is this reasonable?
		iss >> device >> mountPoint >> fsType >> mountOpts;

		// replace any octal escape sequences...
		this->DecodeMtabMountPoint(&mountPoint);
		
		if (statfs(mountPoint.c_str(), &buf))
			throw ProbeException("statfs() error occurred on "+mountPoint+": "+strerror(errno));

		re.GetAllMatchingSubstrings("[^,]+", mountOpts, mountOptMatches);

		Item *item = this->CreateItem();
		item->SetStatus(OvalEnum::STATUS_EXISTS);
		item->AppendElement(new ItemEntity("mount_point", mountPoint, OvalEnum::DATATYPE_STRING));
		item->AppendElement(new ItemEntity("device", device));

		if (blkidCache.wasCacheOpenSuccessful()) {
			char *uuid = blkid_get_tag_value(blkidCache, "UUID", device.c_str());
			if (uuid) {
				item->AppendElement(new ItemEntity("uuid", uuid));
				// from the libblkid sources I found, the values looked to be
				// malloc'd, so I am gonna free() them.
				free(uuid);
			} else
				item->AppendElement(new ItemEntity("uuid", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));
		} else {
			item->AppendElement(new ItemEntity("uuid", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_ERROR));
			item->AppendMessage(new OvalMessage("Couldn't open blkid cache", OvalEnum::LEVEL_ERROR));
		}

		item->AppendElement(new ItemEntity("fs_type", fsType));

		if (mountOptMatches.empty()) // I don't think this should happen unless
									 // the line is malformed...
			item->AppendElement(new ItemEntity("mount_options", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));
		else {
			for (vector<StringVector>::iterator iter = mountOptMatches.begin();
				 iter != mountOptMatches.end();
				 ++iter)
				item->AppendElement(new ItemEntity("mount_options", (*iter)[0]));
		}

		item->AppendElement(new ItemEntity("total_space", Common::ToString(buf.f_blocks), OvalEnum::DATATYPE_INTEGER));

		// statfs() does not directly give you used space; it must be derived
		// from free and total space.  I chose the values I did to mimic the
		// 'df' command.  The man page claims f_bfree is number of free blocks,
		// whereas f_bavail is the number of free blocks available to non-
		// superusers.  I'm mixing both; it's not clear which value I'm
		// supposed to get.
		item->AppendElement(new ItemEntity("space_used", Common::ToString(buf.f_blocks - buf.f_bfree), OvalEnum::DATATYPE_INTEGER));
		item->AppendElement(new ItemEntity("space_left", Common::ToString(buf.f_bavail), OvalEnum::DATATYPE_INTEGER));
		
		this->cachedPartitionItems.push_back(item);
	}
}

void PartitionProbe::DecodeMtabMountPoint(string *mountPoint) {
	string::size_type writeIdx, readIdx;
	
	for (writeIdx = 0, readIdx = 0; readIdx < mountPoint->size(); ) {
		if ((*mountPoint)[readIdx] == '\\') {
			if (readIdx < mountPoint->size() - 1) {
				++readIdx;

				//read no more than 3 chars (assumed to be octal digits).  If there
				//are fewer than 3 chars left in the string, read whatever's left
				//(not likely to happen).
				string::size_type numOctCharsToRead = 
					(readIdx < mountPoint->size()-3) ? 3 : 
					(mountPoint->size() - readIdx);
				string octalStr = mountPoint->substr(readIdx, numOctCharsToRead);

				// use istringstream and manipulator to convert the octal
				// string to an int.
				istringstream iss(octalStr);
				int val;
				iss >> oct >> val;

				if (!iss)
					throw ProbeException("Could not interpret octal escape sequence '\\"+octalStr+"' in mount point '"+*mountPoint+"'.");

				// now cast to a char and write the char back into the
				// mountPoint string.
				(*mountPoint)[writeIdx++] = (char)val;

				readIdx += numOctCharsToRead;
			} else
				// last char is a '\'... treat as a normal char
				(*mountPoint)[writeIdx++] = (*mountPoint)[readIdx++];
		} else
			(*mountPoint)[writeIdx++] = (*mountPoint)[readIdx++];
	}

	mountPoint->resize(writeIdx);
}

