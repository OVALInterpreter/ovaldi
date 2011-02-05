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
#ifndef INTERFACEPROBE_H
#define INTERFACEPROBE_H

#include "AbsProbe.h"
#include "WindowsCommon.h"
#include <iphlpapi.h>
#include <iomanip>

/** This class is responsible for collecting Windows Interface data. */

class InterfaceProbe : public AbsProbe {
    public:

        /** InterfaceProbe destructor. */
        virtual ~InterfaceProbe();

        virtual ItemVector* CollectItems ( Object* object );

        /** Ensure that the Interface probe is a singleton. */
        static AbsProbe* Instance();

    private:

        /** Interface constructor. */
        InterfaceProbe();

        virtual Item* CreateItem();

        /** Retrieve a particular Item from the ItemVector interfaces using the interface's name as the key.
         *  @param nameStr A string that contains the name of a Windows interface.
         *  @return The Item object whose name matches the specified value.
         */
        Item* GetInterface ( string nameStr );

        /** Retrieve all of the Windows interfaces on the local system.
         *  @return Void.
         */
        void GetAllInterfaces();

        /** Convert a interface type DWORD value into its corresponding string value.
         *  @param type A DWORD value that contains a bitmask of flags that represent the type of the Windows interface.
         *  @return A string representing the type of the Windows interface.
         */
        string GetInterfaceType ( DWORD interfaceType );

        /** Get the address type of a IPV4 address.
         *  @param type A DWORD value that contains a bitmask of flags that represent the type of the Windows interface address.
         *  @return A string representing the type of the Windows interface address.
         */
        StringVector * GetInterfaceAddressType ( DWORD addressType );

        /** Calculate the broadcast address manually because Microsoft's documentation for the MIB_IPADDRROW structure states
         *  "The proper value for this member is not returned by the GetIpAddrTable() function". Please consult Microsoft's documentation
         *  at http://msdn.microsoft.com/en-us/library/aa366845(VS.85).aspx for more information.
         *  @param ipAddrStr A string representing the IP address of the interface in dotted-quad notation.
         *  @param netMaskStr A string representing the subnet mask of the interface's IP address in dotted-quad notation.
         *  @return A string representing the interface's broadcast address in dotted-quad notation.
         */
        string CalculateBroadcastAddress ( string ipAddr , string netMask );

        /** Delete all of the Items in the ItemVector interfaces.
         *  @return Void.
         */
        void DeleteInterfaces();

        /** The static instance of the InterfaceProbe.
         *  All Probes are singletons. The ProbeFactory is responsible for managing instances of Probes.
         */
        static InterfaceProbe* instance;

        /** The ItemVector that holds the information about all of the Windows interfaces on the local system */
        ItemVector* interfaces;

};

#endif
