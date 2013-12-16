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
#ifndef PROCESS58PROBE_H
#define PROCESS58PROBE_H

#include <map>

#include "AbsProbe.h"

/** This class is responsible for collecting Windows 32-bit process data.  It is important to note that
 *  the OpenProcess() API call does not have access to all of the processes on the system.  Most
 *  notably, it cannot access the System process or the System Idle process.
 *  Please refer to Microsoft's documentation regarding the OpenProcess API call at the following
 *  website for more information.
 *
 *  http://msdn.microsoft.com/en-us/library/ms684320(VS.85).aspx
 *
 */
class Process58Probe : public AbsProbe {

    public:

        /** Process58Probe destructor. */
        virtual ~Process58Probe();

        virtual ItemVector* CollectItems ( Object* object );

        /** Ensure that the Process58Probe is a singleton. */
        static AbsProbe* Instance();

    private:

        /** Process58Probe constructor. */
        Process58Probe();

        virtual Item* CreateItem();

        /** Retrieve all of the Windows processes on the local system.
         *  @return Void.
         */
        void GetAllProcesses();


        /** Retrieve a particular Item from the ItemVector processes using the process' command line as the key.
         *  @param commandLineStr A string that contains the command line of a Windows process.
         *  @return The Item object whose command line matches the specified value.
         */
        Item* GetProcess ( std::string commandLineStr );

        /** Build a process Item from the data collected using the various Windows APIs.  All Items are placed in the processes ItemVector.
         *  @param processEntry  A PROCESSENTRY32 which contains data about the process whose Item you are building.
         *  @return Void.
         */
        void BuildProcessItem ( PROCESSENTRY32 processEntry );

        /** Retrieve all of the device paths and their corresponding logical paths.
         *  @return Void.
         */
        void GetLogicalPathToDevicePath();

        /** Retrieve all of the Windows processes' command line values.
         *  @return Void.
         */
        void GetAllCommandLineValues();

        /** Delete the StringStringMultiMap commandLineMap.
         *  @return Void.
         */
        void DeleteCommandLineMap();

	
        /** Delete the StringStringMultiMap pathMap.
         *  @return Void.
         */
        void DeletePathMap();

        /** Delete all of the Items in the processes ItemVector.
         *  @return Void.
         */
        void DeleteProcesses();

        /** The static instance of the Process58Probe.
         *  All Probes are singletons. The ProbeFactory is responsible for managing instances of Probes.
         */
        static Process58Probe* instance;

        /** The ItemVector that holds the information about all of the Windows processes on the local system. */
        ItemVector* processes;

		typedef std::multimap<std::string, std::string> StringStringMultiMap;
		typedef std::multimap<DWORD, std::string> DwordStringMultiMap;

        /** The StringStringMultiMap that holds the mapping between the device drives and the logical drives. */
        StringStringMultiMap* pathMap;

        /** The StringStringMultiMap that holds the mapping between the process identifiers and the command line values. */
        StringStringMultiMap* commandLineMap;
};

#endif
