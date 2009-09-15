//
//
//****************************************************************************************//
// Copyright (c) 2002-2009, The MITRE Corporation
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

#ifndef PROBEFACTORY_H
#define PROBEFACTORY_H

#include <string>
#include <vector>
#include <iostream>
#include <stdlib.h>

#include "AbsDataCollector.h"


//	include the probe classes
#include "FileProbe.h"
#include "FileMd5Probe.h"
#include "FileHashProbe.h"
#include "FamilyProbe.h"
#include "UnameProbe.h"
#ifdef PACKAGE_RPM
 #include "RPMInfoProbe.h"
#endif
#ifdef PACKAGE_DPKG
 #include "DPKGInfoProbe.h"
#endif

#include "InetListeningServersProbe.h"
#include "ProcessProbe.h"
//#include "ShadowProbe.h"
#include "PasswordProbe.h"
#include "InterfaceProbe.h"
#include "EnvironmentVariableProbe.h"
#include "XmlFileContentProbe.h"
#include "TextFileContentProbe.h"
#include "VariableProbe.h"
#include "RunLevelProbe.h"

using namespace std;


class AbsProbe;

/**
	This class is a Factory class for getting probes for an object in an oval definitions file. 
	Each object defined in the oval definition schema has a corresponding probe for data collection. 
	This class provides uniform access to all probes in the application, and is responsible for determining
	which probe to associate with a given object.

	Each supported platform should implement its own ProbeFactory that manages the probes for that platform.

	All Probes are singlestons.
*/
class ProbeFactory {

public:
	/**
		Factory method. Based on the specified object name return the appropriate probe.
		@param objectName a string taht corresponds to the name of an object in the oval definition schema.
		@return The probe for the specified object or NULL.
	*/
	static AbsProbe* GetProbe(string objectName);

	/** 
		Shutdown the ProbeFactory.
	*/
	static void Shutdown();

private:
  static AbsProbeSet _probes;

};

#endif
