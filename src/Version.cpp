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
//

#include "Version.h"

// Define the version.  The build and date are automatically updated
// prior to each build.
//
#define MAJOR_VERSION "5"
#define MIN_COMPATIBLE_VERSION "5.0"
#define VENDOR "The MITRE Corporation"
#define VERSION "5.4"
#define BUILD 2

string Version::GetMinumumCompatibleVersion() {

	string version;
	version.append(MIN_COMPATIBLE_VERSION);
    return(version);
}

string Version::GetVendor() {

	string vendor;
	vendor.append(VENDOR);
    return(vendor);
}

string Version::GetVersion() {

	string version;
	version.append(VERSION);
    return(version);
}

string Version::GetBuild() {
	
	string version;
	ostringstream bld;
	bld << BUILD;
	version.append(bld.str());
    return(version);
}

string Version::GetBuildDate() {

	string version;
	version.append(__DATE__);
	version.append(" ");
	version.append(__TIME__);
    return(version);
}
