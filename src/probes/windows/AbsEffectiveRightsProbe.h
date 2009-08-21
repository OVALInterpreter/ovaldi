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

#ifndef ABSEFFECTIVERIGHTSPROBE_H
#define ABSEFFECTIVERIGHTSPROBE_H

#include "AbsProbe.h"
#include "WindowsCommon.h"

#include <aclapi.h>
#include <windows.h>

/** 
	The AbsEffectiveRightsProbe class provides several methods used by all effective rights probes including a
	method for retrieving all of the trustee SIDs for a particular Windows object, a method for retrieving all
	of the matching trustee SIDs, and a method for determining whether or not a trustee SID should be reported
	as non-existent.
*/

class AbsEffectiveRightsProbe : public AbsProbe {

    public:

        /** AbsEffectiveRightsProbe constructor. */
        AbsEffectiveRightsProbe();

        /** AbsEffectiveRightsProbe destructor. */
        ~AbsEffectiveRightsProbe();

        /**
            Return a set of all of the SIDs that match the specified trustee SID entity relative to the specified Windows object.
            @param windowsObjectType A SE_OBJECT_TYPE that specifies the type of the Windows object.
            @param objectNameStr A string value that contains the name of the Windows object.
            @param trusteeSIDEntity A pointer to the trustee SID ObjectEntity that will be used to create the set of matching trustee SIDs.
            @param resolveGroupBehavior A boolean value that specifies whether or not groups should be resolved.
            @param includeGroupBehavior A boolean value that specifies whether or not groups should be included in the resulting set of trustee SIDs.
            @return A pointer to a StringSet containing all of the SIDs that match the specified trustee SID ObjectEntity.
        */
        StringSet* GetTrusteeSIDsForWindowsObject ( SE_OBJECT_TYPE windowsObjectType, string objectNameStr, ObjectEntity* trusteeSIDEntity,  bool resolveGroupBehavior, bool includeGroupBehavior );

        /** Search the input vector of all trustee SIDs and return the set of SIDs the match the specified criteria.
            @param trusteeSIDPatternStr A string value that contains the trustee SID pattern to be matched.
            @param allTrusteeSIDs A pointer to a StringSet that contains all of the trustee SIDs.
            @param trusteeSIDs A pointer to a StringSet that is used to store all of the matching trustee SIDs.
            @param isRegex A boolean value that specifies whether or not the pattern is a regular expression.
            @return Void.
        */
        void GetMatchingTrusteeSIDs ( string trusteeSIDPatternStr, StringSet* allTrusteeSIDs, StringSet* trusteeSIDs, bool isRegex );

        /** Return true if the calling probe should report that the trustee SID does not exist.
            If a trustee SID's operator is set to OPERATOR_EQUALS and the trustee SID does not exist
            the caller should report that the trustee SID was not found. When getting the value of the
            trustee SID to check its existence, we need to look for either a simple element value or a
            variable element with one or more values. If the return value is true, the trusteeSIDs
            StringSet* parameter contains the set of trustee SIDs to report as not existing.
            Otherwise, the the trusteeSID's parameter is NULL. The caller is responsible for
            making sure that any memory allocated for the trusteeSID's parameter is cleaned up. The
            trusteeSID's parameter should be input as NULL when the function is called.
            @param trusteeSIDEntity A pointer to the trustee SID ObjectEntity that specifies the trustee SIDs whose existence should be checked.
            @param trusteeSIDS A pointer to a StringSet that contains the non-existent trustee SIDs.
            @return A boolean value that specifies whether or not the calling probe should report that a trustee SID does not exist.
        */
        bool ReportTrusteeSIDDoesNotExist ( ObjectEntity *trusteeSIDEntity, StringSet* trusteeSIDs );
};

#endif