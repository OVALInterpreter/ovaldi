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
#ifndef DISKUTILPROBE_H
#define DISKUTILPROBE_H

#include "AbsProbe.h"
#include <Item.h>
#include <Object.h>
#include <Log.h>

/**
	Store a single record from a call to diskutil verifyPermissions it its nicely parsed form
*/
class PermissionsRecord {

public: 
	PermissionsRecord(){};
	PermissionsRecord(std::string dev, std::string fp, std::string ur, std::string uw, 
	  std::string ue, std::string gr, std::string gw, std::string ge, 
	  std::string ore, std::string ow, std::string oe) :
	  device(dev), filepath(fp), uread(ur), uwrite(uw), uexec(ue), gread(gr),
	  gwrite(gw), gexec(ge), oread(ore), owrite(ow), oexec(oe){};
	~PermissionsRecord(){};
	
	std::string device;
	std::string filepath;
	std::string uread;
	std::string uwrite;
	std::string uexec;
	std::string gread;
	std::string gwrite;
	std::string gexec;
	std::string oread;
	std::string owrite;
	std::string oexec;
};

/**
	A vector for storing PermissionsRecord objects.
	All objects are stored by reference.
*/
typedef std::vector < PermissionsRecord* > PermissionsRecordVector;

/**
	This class is responsible for collecting variable information from the nvram command.
	This class should be platform independant.
*/
class DiskutilProbe : public AbsProbe {
public:
	virtual ~DiskutilProbe();
	
	/** Run the diskutil probe */
	virtual ItemVector* CollectItems(Object* object);

	/** Ensure that the DiskutilProbe is a singleton. */
	static AbsProbe* Instance();
	
private:

	static DiskutilProbe *instance;

	DiskutilProbe();

	/** Return a new Item created for storing variable information */
	virtual Item* CreateItem();

	/** Return a new filled Item created for storing variable information */
	Item* FillItem(PermissionsRecord* record);

	ObjectEntity* ValidateStringOperations(ObjectEntity* stringOp);

	StringVector* GetDeviceNames();
	
	PermissionsRecordVector* GetPermissions(std::string device);

	std::string GetPermissionCompareType(const char val, const char expected);
	
	void DeleteRecords();
	
	PermissionsRecordVector* records;
	
	std::string deviceName;
};

#endif
