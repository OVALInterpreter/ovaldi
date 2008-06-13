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

// windows schema objects
	if(objectName.compare("file_object") == 0) {
		probe = FileProbe::Instance();
	} else if(objectName.compare("registry_object") == 0) {
		probe = RegistryProbe::Instance();
	} else if(objectName.compare("activedirectory_object") == 0) {
//		probe = ActiveDirectoryProbe::Instance();
	} else if(objectName.compare("metabase_object") == 0) {
//		probe = MetabaseProbe::Instance();
	} else if(objectName.compare("wmi_object") == 0) {
		probe = WMIProbe::Instance();
	} else if(objectName.compare("passwordpolicy_object") == 0) {
		probe = PasswordPolicyProbe::Instance();
	} else if(objectName.compare("lockoutpolicy_object") == 0) {
		probe = LockoutPolicyProbe::Instance();
	} else if(objectName.compare("auditeventpolicy_object") == 0) {
		probe = AuditEventPolicyProbe::Instance();
	} else if(objectName.compare("sid_object") == 0) {
		probe = SidProbe::Instance();
	} else if(objectName.compare("fileeffectiverights53_object") == 0) {
		probe = FileEffectiveRights53Probe::Instance();
	} else if(objectName.compare("fileeffectiverights_object") == 0) {
		Log::Info("Version 5.3 of OVAL deprecated the fileeffectiverights_object");
		probe = FileEffectiveRightsProbe::Instance();
	}  else if(objectName.compare("accesstoken_object") == 0) {
		probe = AccessTokenProbe::Instance();
	} else if(objectName.compare("group_object") == 0) {
		probe = GroupProbe::Instance();
	} else if(objectName.compare("user_object") == 0) {
		probe = UserProbe::Instance();

// independent schema objects
	} else if(objectName.compare("family_object") == 0) {
		probe = FamilyProbe::Instance();
	} else if(objectName.compare("environmentvariable_object") == 0) {
		probe = EnvironmentVariableProbe::Instance();
	} else if(objectName.compare("variable_object") == 0) {
		probe = VariableProbe::Instance();
	} else if(objectName.compare("textfilecontent_object") == 0) {
		probe = TextFileContentProbe::Instance();
	} else if(objectName.compare("xmlfilecontent_object") == 0) {
		probe = XmlFileContentProbe::Instance();
	} else if(objectName.compare("filehash_object") == 0) {
		probe = FileHashProbe::Instance();
	} else if(objectName.compare("filemd5_object") == 0) {
		probe = FileMd5Probe::Instance();
	} else {
		Log::Info(objectName + " is not currently supported.");
	}

	return probe;
}

void ProbeFactory::Shutdown() {

	AbsProbe* probe = NULL;

	probe = FileProbe::Instance();
	delete probe;

	probe = RegistryProbe::Instance();
	delete probe;

//	probe = ActiveDirectoryProbe::Instance();
//	delete probe;

//	probe = MetabaseProbe::Instance();
//	delete probe;

	probe = AccessTokenProbe::Instance();
	delete probe;

	probe = WMIProbe::Instance();
	delete probe;

	probe = FamilyProbe::Instance();
	delete probe;

	probe = EnvironmentVariableProbe::Instance();
	delete probe;

	probe = VariableProbe::Instance();
	delete probe;

	probe = XmlFileContentProbe::Instance();
	delete probe;

	probe = FileMd5Probe::Instance();
	delete probe;

	probe = LockoutPolicyProbe::Instance();
	delete probe;

	probe = AuditEventPolicyProbe::Instance();
	delete probe;

	probe = SidProbe::Instance();
	delete probe;

	probe = GroupProbe::Instance();
	delete probe;

	probe = UserProbe::Instance();
	delete probe;

	probe = FileEffectiveRights53Probe::Instance();
	delete probe;

	probe = FileEffectiveRightsProbe::Instance();
	delete probe;

	probe = TextFileContentProbe::Instance();
	delete probe;
}

