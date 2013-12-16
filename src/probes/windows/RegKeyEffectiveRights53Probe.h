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

#ifndef REGKEYEFFECTIVRIGHTS53PROBE_H
#define REGKEYEFFECTIVRIGHTS53PROBE_H

#include <string>

#include "AbsEffectiveRightsProbe.h"

/**
    This class is responsible for collecting registry key information for windows registrykeyeffectiverights53_objects.
*/
class RegKeyEffectiveRights53Probe : public AbsEffectiveRightsProbe {

    public:
        /** RegKeyEffectiveRights53Probe destructor */
        virtual ~RegKeyEffectiveRights53Probe();

        virtual ItemVector* CollectItems ( Object* object );

        /** Ensure that the RegKeyEffectiveRights53Probe is a singleton. */
        static AbsProbe* Instance();

    private:

        /** RegKeyEffectiveRights53Probe constructor */
        RegKeyEffectiveRights53Probe();

        /** Return a new Item created for storing regkeyeffectiverights53_item information */
        virtual Item* CreateItem();

        /** Get the effective rights for a trustee name for the specified hive and registry key.
         *  @param hiveStr A string that contains the hive of the registry key that you want to get the effective rights of.
         *  @param keyStr A string that contains the name of the registry key that you want to get the effective rights of.
         *  @param trusteeSIDStr A string that contains the trustee SID of the registry key that you want to get the effective rights of.
         *  @return The item that contains the registry key effective rights of the specified hive, key, and trustee name.
         */
        Item* GetEffectiveRights ( HKEY keyHandle, const RegKey *regKey, std::string trusteeSIDStr );

        /** The static instance of the RegKeyEffectiveRights53Probe.
         *  All Probes are singletons. The ProbeFactory is responsible for managing instances of Probes.
         */
        static RegKeyEffectiveRights53Probe* instance;
};

#endif
