//
//
//****************************************************************************************//
// Copyright (c) 2002-2011, The MITRE Corporation
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
	method for retrieving all of the trustees for a particular Windows object, a method for retrieving all
	of the matching trustees, and a method for determining whether or not a trustee should be reported
	as non-existent. In the following documentation, trustee refers to both trustee SIDs and trustee names.
*/

class AbsEffectiveRightsProbe : public AbsProbe {

    public:

        /** AbsEffectiveRightsProbe constructor. */
        AbsEffectiveRightsProbe();

        /** AbsEffectiveRightsProbe destructor. */
        virtual ~AbsEffectiveRightsProbe();

        /**
            Return a set of all of the trustees that match the specified trustee entity relative to the specified Windows object.
            @param windowsObjectType A SE_OBJECT_TYPE that specifies the type of the Windows object.
            @param objectNameStr A string value that contains the name of the Windows object.
            @param trusteeEntity A pointer to the trustee ObjectEntity that will be used to create the set of matching trustees.
			@param isSID A boolean value that specifies whether or not the trusteeEntity is a SID.
            @param resolveGroupBehavior A boolean value that specifies whether or not groups should be resolved.
            @param includeGroupBehavior A boolean value that specifies whether or not groups should be included in the resulting set of trustees.
            @return A pointer to a StringSet containing all of the trustees that match the specified trustee ObjectEntity.
        */
        StringSet* GetTrusteesForWindowsObject ( SE_OBJECT_TYPE windowsObjectType, string objectNameStr, ObjectEntity* trusteeEntity, bool isSID, bool resolveGroupBehavior, bool includeGroupBehavior );

        /** Search the input vector of all trustees and return the set of trustees the match the specified criteria.
            @param trusteePatternStr A string value that contains the trustee pattern to be matched.
            @param allTrustees A pointer to a StringSet that contains all of the trustees.
            @param trustees A pointer to a StringSet that is used to store all of the matching trustees.
            @param isRegex A boolean value that specifies whether or not the pattern is a regular expression.
            @return Void.
        */
        void GetMatchingTrustees ( string trusteePatternStr, StringSet* allTrustees, StringSet* trustees, bool isRegex );

        /** Return true if the calling probe should report that the trustee does not exist.
            If a trustees operator is set to OPERATOR_EQUALS and the trustee does not exist
            the caller should report that the trustee was not found. When getting the value of the
            trustee to check its existence, we need to look for either a simple element value or a
            variable element with one or more values. If the return value is true, the trustees
            StringSet* parameter contains the set of trustees to report as not existing.
            Otherwise, the the trustee's parameter is NULL. The caller is responsible for
            making sure that any memory allocated for the trustee's parameter is cleaned up. The
            trustee's parameter should be input as NULL when the function is called.
            @param trusteeEntity A pointer to the trustee ObjectEntity that specifies the trustees whose existence should be checked.
            @param trustees A pointer to a StringSet that contains the non-existent trustees.
			@param isSID A boolean value that specifies whether or not the trustee entity is a SID.
            @return A boolean value that specifies whether or not the calling probe should report that a trustee does not exist.
        */
        bool ReportTrusteeDoesNotExist ( ObjectEntity *trusteeEntity, StringSet* trustees, bool isSID );
};

#endif