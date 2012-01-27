//
//
//****************************************************************************************//
// Copyright (c) 2002-2012, The MITRE Corporation
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

#ifndef SERVICEEFFECTIVRIGHTSPROBE_H
#define SERVICEEFFECTIVRIGHTSPROBE_H

#pragma warning(disable:4786)

#include "AbsEffectiveRightsProbe.h"
#include "WindowsCommon.h"

#include <aclapi.h>
#include <windows.h>
using namespace std;

/**
    This class is responsible for collecting service information for Windows serviceeffictiverights_objects.
*/
class ServiceEffectiveRightsProbe : public AbsEffectiveRightsProbe {

    public:

        /** ServiceEffectiveRightsProbe destructor */
        virtual ~ServiceEffectiveRightsProbe();

        virtual ItemVector* CollectItems ( Object* object );

        /** Ensure that the ServiceEffectiveRightsProbe is a singleton. */
        static AbsProbe* Instance();

    private:

        /** ServiceEffectiveRightsProbe constructor */
        ServiceEffectiveRightsProbe();

        /** Return a new Item created for storing service information */
        virtual Item* CreateItem();

        /** Get the effective rights for a trustee SID for the specified service.
         *  @param serviceNameStr A string that contains the name of the service that you want to get the effective rights of.
         *  @param trusteeSIDStr A string that contains the trusteeSID of the service that you want to get the effective rights of.
         *  @return The item that contains the service effective rights of the specified service and trustee SID.
         */
        Item* GetEffectiveRights ( string serviceNameStr, string trusteeSID );

        /** Get the set of all services on the system that match the object.
         *  @param serviceNameEntity A ObjectEntity that represents the service_name entity in an Object as defined in the OVAL Definition Schema.
         *  @return A StringSet that contains all of the services specified in the ObjectEntity.
         */
        StringSet* GetServices ( ObjectEntity* serviceNameEntity );

        /** Retrieve all of the matching services.
         *  @param patternStr A string that contains the pattern to be matched.
         *  @param isRegex A boolean value that specifies whether or not the pattern is a regular expression.
         *  @return A StringSet containing all of the matching services.
         */
        StringSet* GetMatchingServices ( string patternStr , bool isRegex );

        /** Determine if the service exists on the system.
         *  @param serviceNameStr A string that contains the name of the service whose existenc you want to check.
         *  @return A boolean value that specifies whether or not the service exists on the system.
         */
        bool ServiceExists ( string serviceNameStr );

        /** Retrieve all of the services on the system.
         *  @return A pointer to a StringSet that contains all of the services on the system.
         */
        StringSet* GetAllServices();

        /** The static instance of the ServiceEffectiveRightsProbe.
         *  All Probes are singletons. The ProbeFactory is responsible for managing instances of Probes.
         */
        static ServiceEffectiveRightsProbe* instance;

        /** The StringSet that holds the information about all of the Windows services on the local system. */
        StringSet* services;
};

#endif
