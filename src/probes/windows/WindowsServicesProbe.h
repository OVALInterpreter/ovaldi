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

#include <memory>
#include <string>
#include <vector>
#include <set>
#include <sstream>  //for std::istringstream
#include <iterator> //for std::istream_iterator


#include "AbsProbe.h"
#include <windows.h>
#include <AutoCloser.h>
#include "WindowsCommon.h"

/** This class is responsible for collecting Windows Services data.
 *
 */

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
        Item* GetService( std::string serviceName );

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
        std::auto_ptr<StringSet> GetMatchingServices ( std::string patternStr , bool isRegex );
		
		 /** Determine if the service exists on the system.
         *  @param serviceNameStr A string that contains the name of the service whose existenc you want to check.
         *  @return A boolean value that specifies whether or not the service exists on the system.
         */
        bool ServiceExists ( std::string serviceNameStr );

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


		enum WindowsServiceType	{SERVICE_KERNEL_DRIVER_TYPE=1,
								SERVICE_FILE_SYSTEM_DRIVER_TYPE=2,		
								SERVICE_WIN32_OWN_PROCESS_TYPE=16,
								SERVICE_WIN32_SHARE_PROCESS_TYPE=32,
								SERVICE_INTERACTIVE_OWN_PROCESS_TYPE=272,
								SERVICE_INTERACTIVE_SHARE_PROCESS_TYPE=288};

		
		static std::string ServiceTypeToString(DWORD type);
	
		enum WindowsServiceStartType {SERVICE_BOOT_START_TYPE=0,
									SERVICE_SYSTEM_START_TYPE=1,
									SERVICE_AUTO_START_TYPE=2,
									SERVICE_DEMAND_START_TYPE=3,
									SERVICE_DISABLED_TYPE=4};

		static std::string StartTypeToString(DWORD type);

		enum WindowsServiceCurrentState {SERVICE_STOPPED_STATE=0x1,
										SERVICE_START_PENDING_STATE=0x2,
										SERVICE_STOP_PENDING_STATE=0x3,
										SERVICE_RUNNING_STATE=0x4,
										SERVICE_CONTINUE_PENDING_STATE=0x5,
										SERVICE_PAUSE_PENDING_STATE=0x6,
										SERVICE_PAUSED_STATE=0x7};

		static std::string CurrentStateToString(DWORD type);

		enum WindowsServiceControlsAccepted {SERVICE_CONTROL_ACCEPT_STOP=0x1,
											SERVICE_CONTROL_ACCEPT_PAUSE_CONTINUE=0x2,
											SERVICE_CONTROL_ACCEPT_SHUTDOWN=0x4,
											SERVICE_CONTROL_ACCEPT_PARAMCHANGE=0x8,
											SERVICE_CONTROL_ACCEPT_NETBINDCHANGE=0x10,
											SERVICE_CONTROL_ACCEPT_HARDWAREPROFILECHANGE=0x20,
											SERVICE_CONTROL_ACCEPT_POWEREVENT=0x40,
											SERVICE_CONTROL_ACCEPT_SESSIONCHANGE=0x80,
											SERVICE_CONTROL_ACCEPT_PRESHUTDOWN=0x100,
											SERVICE_CONTROL_ACCEPT_TIMECHANGE=0x200,
											SERVICE__CONTROLACCEPT_TRIGGEREVENT=0x400};

		static std::string ControlToString(DWORD type);

		enum WindowsServiceFlag {SERVICE_NOT_IN_SYSTEM_PROCESS_FLAG=0,
								SERVICE_RUNS_IN_SYSTEM_PROCESS_FLAG=1};

		static std::string ServiceFlagToString(DWORD type);
};

#endif
