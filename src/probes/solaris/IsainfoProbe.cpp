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

#include <sys/systeminfo.h>
#include <string>
#include <cstring>
#include <cstdlib>
#include <sstream>

#include "IsainfoProbe.h"
#include "ItemEntity.h"
#include "OvalEnum.h"

using namespace std;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  IsainfoProbe Class  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
IsainfoProbe* IsainfoProbe::instance = NULL;

IsainfoProbe::IsainfoProbe() {

}

IsainfoProbe::~IsainfoProbe() {
  instance = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* IsainfoProbe::Instance() {

	// Use lazy initialization
	if(instance == NULL) 
		instance = new IsainfoProbe();

	return instance;	
}

ItemVector* IsainfoProbe::CollectItems(Object *object) {

	ItemVector *collectedItems = NULL;

	// There are no object entities for this object.  We create
	// a single item with a fixed structure.

	// This implementation is inspired by the source for the
	// OpenSolaris isainfo command.

	int bits = 0;
	char *kernel_isa = NULL;
	char *application_isa = NULL;

	kernel_isa = this->RunSysinfo(SI_ARCHITECTURE_K);

	application_isa = this->RunSysinfo(SI_ARCHITECTURE_64);
	if (!application_isa)
		application_isa = this->RunSysinfo(SI_ARCHITECTURE_32);

	// one might also think that success or failure of
	// sysinfo(SI_ARCHITECTURE_64) might also indicate bitness, 
	// but their code didn't work that way...
	if (application_isa) {
		if (!strncmp(application_isa, "sparc", 5) || !strncmp(application_isa, "i386", 4))
			bits = 32;
		else if (!strncmp(application_isa, "sparcv9", 7) || !strncmp(application_isa, "amd64", 5))
			bits = 64;
	}

	Item *item = this->CreateItem();
	if (bits > 0 && kernel_isa && application_isa)
		item->SetStatus(OvalEnum::STATUS_EXISTS);

	if (bits > 0)
		item->AppendElement(new ItemEntity("bits", Common::ToString(bits), OvalEnum::DATATYPE_INTEGER));
	else
		item->AppendElement(new ItemEntity("bits", "0", OvalEnum::DATATYPE_INTEGER,
										   OvalEnum::STATUS_ERROR));

	if (kernel_isa)
		item->AppendElement(new ItemEntity("kernel_isa", kernel_isa));
	else
		item->AppendElement(new ItemEntity("kernel_isa", "", OvalEnum::DATATYPE_STRING,
										   OvalEnum::STATUS_ERROR));

	if (application_isa)
		item->AppendElement(new ItemEntity("application_isa", application_isa));
	else
		item->AppendElement(new ItemEntity("application_isa", "", OvalEnum::DATATYPE_STRING,
										   OvalEnum::STATUS_ERROR));

	if (kernel_isa)
		free(kernel_isa);
	if (application_isa)
		free(application_isa);

	collectedItems = new ItemVector();
	collectedItems->push_back(item);

	return collectedItems;
}

char* IsainfoProbe::RunSysinfo(int cmd) {
	long result = 257; // man page says 257 is a good initial estimate
	long bufSize = 0;
	char *buf = NULL;
	
	do {
		if (result > bufSize) {
			char *tmp = (char*)realloc(buf, result);
			if (!tmp) {
				if (buf) free(buf);
				ostringstream oss;
				oss << "Couldn't allocate " << result << " bytes of memory";
				throw ProbeException(oss.str());
			}
			buf = tmp;
			bufSize = result;
		}

		result = sysinfo(cmd, buf, bufSize);
	} while(result > bufSize);

	// the isainfo source code I looked at did not treat result < 0
	// (documented to be an error return) as a real error.  Instead, it
	// seemed to be interpreted as a sort of "not supported" value.  So
	// I will do the same here, and just return NULL on errors.
	if (result < 0) {
		if (buf) free(buf);
		buf = NULL;
	}

	return buf;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
Item* IsainfoProbe::CreateItem() {

	Item* item = new Item(0, 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#solaris", 
						"sol-sc", 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#solaris solaris-system-characteristics-schema.xsd", 
						OvalEnum::STATUS_ERROR, 
						"isainfo_item");

	return item;
}
