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

#include <cerrno>
#include <fstream>
#include <sstream>
#include <string>
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

	// These are taken from the statfs man page and other sources.  There
	// doesn't seem to be a single header that gives you all the magic values to use.
	this->fstypeMap[0xadf5] = "ADFS_SUPER_MAGIC";
	this->fstypeMap[0xadff] = "AFFS_SUPER_MAGIC";
	this->fstypeMap[0x5346414F] = "AFS_SUPER_MAGIC";
	this->fstypeMap[0x0187] = "AUTOFS_SUPER_MAGIC";
	this->fstypeMap[0x73757245] = "CODA_SUPER_MAGIC";
	this->fstypeMap[0x28cd3d45] = "CRAMFS_MAGIC";
	this->fstypeMap[0x453dcd28] = "CRAMFS_MAGIC_WEND";
	this->fstypeMap[0x64626720] = "DEBUGFS_MAGIC";
	this->fstypeMap[0x62656572] = "SYSFS_MAGIC";
	this->fstypeMap[0x73636673] = "SECURITYFS_MAGIC";
	this->fstypeMap[0xf97cff8c] = "SELINUX_MAGIC";
	this->fstypeMap[0x858458f6] = "RAMFS_MAGIC";
	this->fstypeMap[0x01021994] = "TMPFS_MAGIC";
	this->fstypeMap[0x958458f6] = "HUGETLBFS_MAGIC";
	this->fstypeMap[0x73717368] = "SQUASHFS_MAGIC";
	this->fstypeMap[0x414A53] = "EFS_SUPER_MAGIC";
	this->fstypeMap[0xabba1974] = "XENFS_SUPER_MAGIC";
	this->fstypeMap[0x9123683E] = "BTRFS_SUPER_MAGIC";
	this->fstypeMap[0xf995e849] = "HPFS_SUPER_MAGIC";
	this->fstypeMap[0x9660] = "ISOFS_SUPER_MAGIC";
	this->fstypeMap[0x72b6] = "JFFS2_SUPER_MAGIC";
	this->fstypeMap[0x09041934] = "ANON_INODE_FS_MAGIC";
	this->fstypeMap[0x137F] = "MINIX_SUPER_MAGIC";
	this->fstypeMap[0x138F] = "MINIX_SUPER_MAGIC2";
	this->fstypeMap[0x2468] = "MINIX2_SUPER_MAGIC";
	this->fstypeMap[0x2478] = "MINIX2_SUPER_MAGIC2";
	this->fstypeMap[0x4d5a] = "MINIX3_SUPER_MAGIC";
	this->fstypeMap[0x4d44] = "MSDOS_SUPER_MAGIC";
	this->fstypeMap[0x564c] = "NCP_SUPER_MAGIC";
	this->fstypeMap[0x6969] = "NFS_SUPER_MAGIC";
	this->fstypeMap[0x9fa1] = "OPENPROM_SUPER_MAGIC";
	this->fstypeMap[0x9fa0] = "PROC_SUPER_MAGIC";
	this->fstypeMap[0x002f] = "QNX4_SUPER_MAGIC";
	this->fstypeMap[0x52654973] = "REISERFS_SUPER_MAGIC";
	this->fstypeMap[0x517B] = "SMB_SUPER_MAGIC";
	this->fstypeMap[0x9fa2] = "USBDEVICE_SUPER_MAGIC";
	this->fstypeMap[0x27e0eb] = "CGROUP_SUPER_MAGIC";
	this->fstypeMap[0xBAD1DEA] = "FUTEXFS_SUPER_MAGIC";
	this->fstypeMap[0x57AC6E9D] = "STACK_END_MAGIC";
	this->fstypeMap[0x1cd1] = "DEVPTS_SUPER_MAGIC";
	this->fstypeMap[0x534F434B] = "SOCKFS_MAGIC";	
	
	// ext2, 3, 4 are indistinguishable this way...
	//this->fstypeMap[0xEF53] = "EXT2_SUPER_MAGIC";

	// some constants in the schema I think were accidentally included...
	// the _STRING constants aren't even numbers!
	// REISERFS_SUPER_MAGIC_STRING
	// REISER2FS_SUPER_MAGIC_STRING
	// REISER2FS_JR_SUPER_MAGIC_STRING

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
		
		if (statfs(mountPoint.c_str(), &buf))
			throw ProbeException("statfs() error occurred on "+mountPoint+": "+strerror(errno));

		re.GetAllMatchingSubstrings("[^,]+", mountOpts, mountOptMatches);

		Item *item = this->CreateItem();
		item->SetStatus(OvalEnum::STATUS_EXISTS);
		item->AppendElement(new ItemEntity("mount_point", mountPoint, OvalEnum::DATATYPE_STRING, true));
		item->AppendElement(new ItemEntity("device", device));

		if (blkidCache.wasCacheOpenSuccessful()) {
			char *uuid = blkid_get_tag_value(blkidCache, "UUID", device.c_str());
			if (uuid) {
				item->AppendElement(new ItemEntity("uuid", uuid));
				// from the libblkid sources I found, the values looked to be
				// malloc'd, so I am gonna free() them.
				free(uuid);
			} else
				item->AppendElement(new ItemEntity("uuid", "", OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_DOES_NOT_EXIST));
		} else {
			item->AppendElement(new ItemEntity("uuid", "", OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_ERROR));
			item->AppendMessage(new OvalMessage("Couldn't open blkid cache", OvalEnum::LEVEL_ERROR));
		}

		// filesystem type is not straightforward... sometimes the fs_type field
		// tells us, but it can't distinguish ext2,3,4.  So I'll try using the
		// 3rd field of the mtab line...
		std::string itemFsType;
		if (buf.f_type == 0xEF53) {
			if (fsType == "ext2")
				itemFsType = "EXT2_SUPER_MAGIC";
			else if (fsType == "ext3")
				itemFsType = "EXT3_SUPER_MAGIC";
			else if (fsType == "ext4")
				itemFsType = "EXT4_SUPER_MAGIC";				
		} else if (this->fstypeMap.find(buf.f_type) != this->fstypeMap.end())
			itemFsType = this->fstypeMap[buf.f_type];
		
		if (itemFsType.empty()) {
			item->AppendElement(new ItemEntity("fs_type", "", OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_ERROR));
			ostringstream oss;
			oss << showbase << hex << buf.f_type;
			item->AppendMessage(new OvalMessage("Unrecognized filesystem type: " + oss.str(), OvalEnum::LEVEL_ERROR));
		} else
			item->AppendElement(new ItemEntity("fs_type", itemFsType));

		if (mountOptMatches.empty()) // I don't think this should happen unless
									 // the line is malformed...
			item->AppendElement(new ItemEntity("mount_options", "", OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_DOES_NOT_EXIST));
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
