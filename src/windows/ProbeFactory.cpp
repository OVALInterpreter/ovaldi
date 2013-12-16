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

#include <set>

//	include the probe classes
#include "ActiveDirectoryProbe.h"
#include "FileProbe.h"
#include "FileMd5Probe.h"
#include "FileHashProbe.h"
#include "FileHash58Probe.h"
#include "DNSCacheProbe.h"
#include "FamilyProbe.h"
#include "RegistryProbe.h"
#include "EnvironmentVariableProbe.h"
#include "TextFileContentProbe.h"
#include "XmlFileContentProbe.h"
#include "VariableProbe.h"
#include "WMIProbe.h"
#include "WMI57Probe.h"
#include "PasswordPolicyProbe.h"
#include "LockoutPolicyProbe.h"
#include "AuditEventPolicyProbe.h"
#include "SidProbe.h"
#include "SidSidProbe.h"
#include "FileEffectiveRightsProbe.h"
#include "FileEffectiveRights53Probe.h"
#include "AccessTokenProbe.h"
#include "GroupProbe.h"
#include "UserProbe.h"
#include "AuditEventPolicySubcategoriesProbe.h"
#include "WUAUpdateSearcherProbe.h"
#include "UserSidProbe.h"
#include "UserSid55Probe.h"
#include "SharedResourceProbe.h"
#include "VolumeProbe.h"
#include "InterfaceProbe.h"
#include "PortProbe.h"
#include "ProcessProbe.h"
#include "Process58Probe.h"
#include "PrinterEffectiveRightsProbe.h"
#include "ServiceEffectiveRightsProbe.h"
#include "RegKeyEffectiveRightsProbe.h"
#include "RegKeyEffectiveRights53Probe.h"
#include "TextFileContent54Probe.h"
#include "GroupSidProbe.h"
#include "FileAuditedPermissionsProbe.h"
#include "FileAuditedPermissions53Probe.h"
#include "RegKeyAuditedPermissionsProbe.h"
#include "RegKeyAuditedPermissions53Probe.h"
#include "MetabaseProbe.h"
#include "LDAPProbe.h"
#include "CmdletProbe.h"
#include "EnvironmentVariable58Probe.h"

#include "ProbeFactory.h"

using namespace std;

typedef set<AbsProbe*> AbsProbeSet;
static AbsProbeSet _probes;

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
		probe = ActiveDirectoryProbe::Instance();
	} else if(objectName.compare("wmi_object") == 0) {
		probe = WMIProbe::Instance();
	} else if(objectName.compare("wmi57_object") == 0) {
		probe = WMI57Probe::Instance();
	} else if(objectName.compare("passwordpolicy_object") == 0) {
		probe = PasswordPolicyProbe::Instance();
	} else if(objectName.compare("lockoutpolicy_object") == 0) {
		probe = LockoutPolicyProbe::Instance();
	} else if(objectName.compare("auditeventpolicy_object") == 0) {
		probe = AuditEventPolicyProbe::Instance();
	} else if(objectName.compare("sid_object") == 0) {
		probe = SidProbe::Instance();
	} else if(objectName.compare("sid_sid_object") == 0) {
		probe = SidSidProbe::Instance();
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
	}else if(objectName.compare("sharedresource_object") == 0){
		probe = SharedResourceProbe::Instance();
	}else if(objectName.compare("volume_object") == 0){
		probe = VolumeProbe::Instance();
	}else if(objectName.compare("interface_object") == 0){
		probe = InterfaceProbe::Instance();
	}else if(objectName.compare("port_object") == 0){
		probe = PortProbe::Instance();
	}else if (objectName.compare("process_object") == 0){
		probe = ProcessProbe::Instance();
	}else if (objectName.compare("process58_object") == 0){
		probe = Process58Probe::Instance();
	}else if (objectName.compare("printereffectiverights_object") == 0){
		probe = PrinterEffectiveRightsProbe::Instance();
	}else if (objectName.compare("serviceeffectiverights_object") == 0){
		probe = ServiceEffectiveRightsProbe::Instance();
	}else if (objectName.compare("regkeyeffectiverights_object") == 0 ){
		probe = RegKeyEffectiveRightsProbe::Instance();
	}else if (objectName.compare("regkeyeffectiverights53_object") == 0 ){
		probe = RegKeyEffectiveRights53Probe::Instance();
	}else if (objectName.compare("group_sid_object") == 0 ){
		probe = GroupSidProbe::Instance();
	}else if (objectName.compare("fileauditedpermissions_object") == 0 ){
		probe = FileAuditedPermissionsProbe::Instance();
	}else if (objectName.compare("fileauditedpermissions53_object") == 0 ){
		probe = FileAuditedPermissions53Probe::Instance();
	}else if (objectName.compare("regkeyauditedpermissions_object") == 0 ){
		probe = RegKeyAuditedPermissionsProbe::Instance();
	}else if (objectName.compare("regkeyauditedpermissions53_object") == 0 ){
		probe = RegKeyAuditedPermissions53Probe::Instance();
	}else if (objectName.compare("metabase_object") == 0 ){
		probe = MetabaseProbe::Instance();
	}else if(objectName.compare("auditeventpolicysubcategories_object") == 0) {
		
		// Supported on vista and later
		if(WindowsCommon::IsVistaOrLater()) {
			probe = AuditEventPolicySubcategoriesProbe::Instance();
		} else {
			Log::Message("Found auditeventpolicysubcategories_object. auditeventpolicysubcategories_objects are only supported on Vista and later.");
		}
		
	} else if(objectName.compare("wuaupdatesearcher_object") == 0) {
		probe = WUAUpdateSearcherProbe::Instance();
	} else if(objectName.compare("user_sid_object") == 0) {
		Log::Info("Version 5.5 of OVAL deprecated the user_sid_object");
		probe = UserSidProbe::Instance();
	} else if(objectName.compare("user_sid55_object") == 0) {
		probe = UserSid55Probe::Instance();
	} else if(objectName.compare("dnscache_object") == 0){
		probe = DNSCacheProbe::Instance();
	// independent schema objects
	} else if(objectName.compare("family_object") == 0) {
		probe = FamilyProbe::Instance();
	} else if(objectName.compare("environmentvariable_object") == 0) {
		probe = EnvironmentVariableProbe::Instance();
	} else if(objectName.compare("environmentvariable58_object") == 0) {
		probe = EnvironmentVariable58Probe::Instance();
	} else if(objectName.compare("variable_object") == 0) {
		probe = VariableProbe::Instance();
	} else if(objectName.compare("textfilecontent_object") == 0) {
		probe = TextFileContentProbe::Instance();
	} else if(objectName.compare("textfilecontent54_object") == 0) {
		probe = TextFileContent54Probe::Instance();
	} else if(objectName.compare("xmlfilecontent_object") == 0) {
		probe = XmlFileContentProbe::Instance();
	} else if(objectName.compare("filehash_object") == 0) {
		probe = FileHashProbe::Instance();
	} else if(objectName.compare("filehash58_object") == 0) {
		probe = FileHash58Probe::Instance();
	} else if(objectName.compare("filemd5_object") == 0) {
		probe = FileMd5Probe::Instance();
	} else if(objectName.compare("ldap_object") == 0) {
		probe = LDAPProbe::Instance();
	} else if(objectName.compare("cmdlet_object") == 0) {
		probe = CmdletProbe::Instance();
	} else {
		Log::Info(objectName + " is not currently supported.");
	}

  _probes.insert( probe );

	return probe;
}

void ProbeFactory::Shutdown() {
  for( AbsProbeSet::iterator iter = _probes.begin(); iter != _probes.end(); ){
    delete (*iter);  // the probe better set it's instance pointer to NULL inside of its destructor
    _probes.erase( iter++ );
  }
}
