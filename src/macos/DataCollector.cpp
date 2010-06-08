//
//
//****************************************************************************************//
// Copyright (c) 2002-2010, The MITRE Corporation
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

#include "DataCollector.h"

//****************************************************************************************//
//								DataCollector Class										  //	
//****************************************************************************************//
DataCollector::DataCollector() : AbsDataCollector() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//
	// -----------------------------------------------------------------------
}

DataCollector::~DataCollector() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Nothing for now
	// -----------------------------------------------------------------------
}

// ***************************************************************************************	//
//								Static Public members										//
// ***************************************************************************************	//
void DataCollector::Init() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Create a new instance of the data collector
	//	Set the oval and sc document references
	//	Add the System info for this platform
	//
	// -----------------------------------------------------------------------

	AbsDataCollector* odc = new DataCollector();
	odc->InitBase(new ObjectCollector());
	
	AbsDataCollector::instance = odc;
}


void DataCollector::WriteSystemInfo() {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Create a new instance of the data collector
	//	Set the oval and sc document references
	//	Add the System info for this platform
	//
	// -----------------------------------------------------------------------

	SystemInfo* systemInfo = SystemInfoCollector::CollectSystemInfo();
	systemInfo->Write(DocumentManager::GetSystemCharacterisitcsDocument());
}



