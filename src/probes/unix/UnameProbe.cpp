//
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
#include "UnameProbe.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  UnameProbe Class  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
UnameProbe* UnameProbe::instance = NULL;

UnameProbe::UnameProbe() {

}

UnameProbe::~UnameProbe() {

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* UnameProbe::Instance() {

	// Use lazy initialization
	if(instance == NULL) 
		instance = new UnameProbe();

	return instance;	
}

ItemVector* UnameProbe::CollectItems(Object *object) {

	ItemVector *collectedItems = NULL;

	struct utsname tmpName;
	
	if (uname(&tmpName) > -1) {

		collectedItems = new ItemVector();

		// create a new uname item
		Item* item = this->CreateItem();
		item->SetStatus(OvalEnum::STATUS_EXISTS);
		collectedItems->push_back(item);

		item->AppendElement(new ItemEntity("machine_class",  tmpName.machine, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
		item->AppendElement(new ItemEntity("node_name",  tmpName.nodename, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
		item->AppendElement(new ItemEntity("os_name",  tmpName.sysname, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
		item->AppendElement(new ItemEntity("os_release",  tmpName.release, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
		item->AppendElement(new ItemEntity("os_version",  tmpName.version, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
		// These two appear to be the same on a Linux system
		item->AppendElement(new ItemEntity("processor_type",  tmpName.machine, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));

	} else {
		throw ProbeException("Error: UnameProbe() unable to get uname information");
	}

	//I am laving this Solaris condition here for reference use
	//When I have to write a Solaris uname probe. I will just use this probe as a template
	/*#ifdef SUNOS

	char buf[64];
	
	memset(buf, '\0', sizeof(buf));
	if (sysinfo(SI_ARCHITECTURE, buf, sizeof(buf)) > -1) 
	{
		processorType = buf;
	}
	
	memset(buf, '\0', sizeof(buf));
	if (sysinfo(SI_PLATFORM, buf, sizeof(buf)) > -1) 
	{
		hardwarePlatform = buf;
	}
	#endif*/

	return collectedItems;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
Item* UnameProbe::CreateItem() {

	Item* item = new Item(0, 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#unix", 
						"unix-sc", 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#unix unix-system-characteristics-schema.xsd", 
						OvalEnum::STATUS_ERROR, 
						"uname_item");

	return item;
}
