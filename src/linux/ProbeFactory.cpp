//
// $Id: ProbeFactory.cpp 4579 2008-01-02 17:39:07Z bakerj $
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

#include "ProbeFactory.h"

// ***************************************************************************************	//
//								 Public members												//
// ***************************************************************************************	//
AbsProbe* ProbeFactory::GetProbe(string objectName) {

	AbsProbe* probe = NULL;

// here are the objects defined in the independent schema
	if(objectName.compare("family_object") == 0) {
		probe = FamilyProbe::Instance();
	} else if(objectName.compare("filemd5_object") == 0) {
		probe = FileMd5Probe::Instance();
	} else if(objectName.compare("filehash_object") == 0) {
		probe = FileHashProbe::Instance();
	} else if(objectName.compare("environmentvariable_object") == 0) {
		probe = EnvironmentVariableProbe::Instance();
	} else if(objectName.compare("textfilecontent_object") == 0) {
		probe = TextFileContentProbe::Instance();
	} else if(objectName.compare("variable_object") == 0) {
		probe = VariableProbe::Instance();
	} else if(objectName.compare("xmlfilecontent_object") == 0) {
		probe = XmlFileContentProbe::Instance();

// here are the objects defined in the unix schema
	} else if(objectName.compare("file_object") == 0) {
		probe = FileProbe::Instance();	
	} else if(objectName.compare("inetd_object") == 0) {
		// Not currently implemented for any unix systems
	} else if(objectName.compare("interface_object") == 0) {
		// Not currently implemented for any unix systems
	} else if(objectName.compare("password_object") == 0) {
		// Not currently implemented for any unix systems
	} else if(objectName.compare("process_object") == 0) {
		probe = ProcessProbe::Instance();
	} else if(objectName.compare("runlevel_object") == 0) {
		// Not currently implemented for any unix systems
	} else if(objectName.compare("sccs_object") == 0) {
		// Not currently implemented for any unix systems
	} else if(objectName.compare("shadow_object") == 0) {
		// Not currently implemented for any unix systems
	} else if(objectName.compare("uname_object") == 0) {
		probe = UnameProbe::Instance();

// here are the objects defined in the linux schema
#ifdef PACKAGE_DPKG
	} else if(objectName.compare("dpkginfo_object") == 0) {
		probe = DPKGInfoProbe::Instance();
#endif
	} else if(objectName.compare("inetlisteningservers_object") == 0) {
		probe = InetListeningServersProbe::Instance();
#ifdef PACKAGE_RPM
	} else if(objectName.compare("rpminfo_object") == 0) {
		probe = RPMInfoProbe::Instance();
#endif
	}

	return probe;
}

void ProbeFactory::Shutdown() {

	AbsProbe* probe = NULL;

	probe = FileProbe::Instance();
	delete probe;

	probe = UnameProbe::Instance();
	delete probe;

#ifdef PACKAGE_RPM
 	probe = RPMInfoProbe::Instance();
 	delete probe;
#endif
 	
#ifdef PACKAGE_DPKG
 	probe = DPKGInfoProbe::Instance();
 	delete probe;
#endif

	probe = ProcessProbe::Instance();
	delete probe;

	probe = InetListeningServersProbe::Instance();
	delete probe;

	probe = FamilyProbe::Instance();
	delete probe;

	probe = EnvironmentVariableProbe::Instance();
	delete probe;

	probe = VariableProbe::Instance();
	delete probe;

	probe = XmlFileContentProbe::Instance();
	delete probe;

	probe = TextFileContentProbe::Instance();
	delete probe;

	probe = FileHashProbe::Instance();
	delete probe;
}
