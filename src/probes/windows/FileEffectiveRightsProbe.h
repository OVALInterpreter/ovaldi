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

#ifndef FILEEFFECTIVRIGHTSPROBE_H
#define FILEEFFECTIVRIGHTSPROBE_H

#pragma warning(disable:4786)

#include "FileFinder.h"
#include "AbsProbe.h"
#include "WindowsCommon.h"

#include <aclapi.h>
#include <windows.h>

using namespace std;


/**
	This class is responsible for collecting file information for windows fileeffectiverights_objects.
*/
class FileEffectiveRightsProbe : public AbsProbe {

public:
	~FileEffectiveRightsProbe();
	
	ItemVector* CollectItems(Object* object);

	/** Return a new Item created for storing file information */
	Item* CreateItem();

	/** Ensure that the FileEffectiveRightsProbe is a singleton. */
	static AbsProbe* Instance();

private:
	FileEffectiveRightsProbe();

	static FileEffectiveRightsProbe* instance;

	/** Get the effective rights for a named trustee for the specified file. */
	Item* GetEffectiveRights(string path, string fileName, string trusteeName);

    /**
        Return a set of all the trustee names that match the specified trusteeSID entity relative to the specified file.
        @param fp The path and filename of the desired file as a StringPair
        @param trusteeSID The truseeSID ObjectEntity that will be used to create the set of matching trustee SIDs
        @param resolveGroupBehavior boolean flag used to indicate whether or not groups should be resolved
        @param includeGroupBehavior boolean flag used to indicate whether or not groups shoudl be included in the resulting set of truseee SIDs
    */
    StringSet* GetTrusteeNamesForFile(StringPair* fp, ObjectEntity* trusteeName,  bool resolveGroupBehavior, bool includeGroupBehavior);

	/** 
		Select all Trustees that match the specified trusteeNamePattern. 
	    Matching is done via regex or != depending on the isRegex boolean
		flag. All matches are added to the trusteeNames vector.
	*/
	void GetMatchingTrusteeNames(string trusteeNamePattern, StringSet* allTrusteeNames, StringSet* trusteeNames, bool isRegex = true);

	/** Return true if the calling probe should report that the trustee name does not exist.
		If a trustee name's operator is set to OPERATOR_EQUALS and the trustee name does not exist
		at the specified path the caller should report that the trustee name was not found. When 
		getting the value of the trustee name to check existence need to look for either a simple 
		element value or a variable with one or more values. If the return value is true the trusteeNames 
		StringVector* paremeter contains the set of trustee names to report as not existing. 
		Otherwise the the trusteeNames paramter is NULL. The caller is responsible for making sure
		that any memory allocated for the trusteeNames parameter is cleaned up. The trusteeNames paramter
		should be input as NULL when the function is called.
	*/
	bool ReportTrusteeNameDoesNotExist(ObjectEntity* trusteeName, StringSet* trusteeNames);
};

#endif
