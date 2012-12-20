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
#ifndef WINDOWSSERVICESPROBE_H
#define WINDOWSSERVICESPROBE_H

#include "AbsProbe.h"
#include <windows.h>
#include <AutoCloser.h>
#include "WindowsCommon.h"

/** This class is responsible for collecting Windows Services data.
 *
 */

//typedef multimap<string, string> StringStringMultiMap;

class WindowsServicesProbe : public AbsProbe {

    public:

        /** ServiceProbe destructor. */
        virtual ~WindowsServicesProbe();

        virtual ItemVector* CollectItems ( Object* object );

        /** Ensure that the WindowsServicesProbe is a singleton. */
        static AbsProbe* Instance();

    private:

        /** WindowsServicesProbe constructor. */
        WindowsServicesProbe();

        virtual Item* CreateItem();

        /** Retrieve all of the Windows services on the local system.
         *  @return Void.
         */
        void GetAllServiceNames(ItemVector * services);

        /** Retrieve a particular Item from the ItemVector services using the services' name as the key.
         *  @param serviceName
         *  @return The Item object whose command line matches the specified value.
         */
        Item* GetService( string serviceName );

        /** The static instance of the WindowsServicesProbe.
         *  All Probes are singletons. The ProbeFactory is responsible for managing instances of Probes.
         */
        static WindowsServicesProbe* instance;

        /** The ItemVector that holds the information about all of the Windows services on the local system. */
        //ItemVector* services;

		//std::vector<std::string> serviceNames;
		
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

        /** The StringSet that holds the information about all of the Windows services on the local system. */
        StringSet* services;

		/**
		 * Holds a handle to the service control manager which we can use
		 * to query info about services.  We open it once when the probe
		 * singleton is created, and close it when the singleton is deleted.
		 */
		std::auto_ptr<AutoCloser<SC_HANDLE, BOOL(WINAPI&)(SC_HANDLE)> > serviceMgr;

};

#endif