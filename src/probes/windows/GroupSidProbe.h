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
#ifndef GROUPSIDPROBE_H
#define GROUPSIDPROBE_H

#pragma warning(disable:4786)

#include "AbsProbe.h"
#include "WindowsCommon.h"

using namespace std;

/**
    This class is responsible for collecting information for Windows group_sid_objects.
*/
class GroupSidProbe : public AbsProbe {

    public:

        /** GroupSidProbe destructor. */
        virtual ~GroupSidProbe();

        virtual ItemVector* CollectItems ( Object* object );

        /** Ensure that the GroupSidProbe is a singleton. */
        static AbsProbe* Instance();

    private:

        /** GroupSidProbe constructor. */
        GroupSidProbe();

        /** Return a new Item created for storing group SID information. */
        virtual Item* CreateItem();

        /** Retrieve an Item that contains all of the user SIDs that are members of the specified group SID.
         *  @param groupSIDStr A string that contains the SID of the group whose users you would like to enumerate.
         *  @return The Item object whose SID matches the specified value.
         */
        Item* GetGroupSidInfo ( string groupSidStr );

        /** The static instance of the GroupSidProbe.
         *  All Probes are singletons. The ProbeFactory is responsible for managing instances of Probes.
         */
        static GroupSidProbe* instance;
};

#endif