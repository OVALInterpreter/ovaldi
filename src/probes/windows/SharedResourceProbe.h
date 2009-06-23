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
#ifndef SHAREDRESOURCEPROBE_H
#define SHAREDRESOURCEPROBE_H

#include "AbsProbe.h"
#include "WindowsCommon.h"
#include <lm.h>

/** This class is responsible for collecting Windows shared resource data. */

class SharedResourceProbe : public AbsProbe {

    public:

        /** SharedResourceProbe destructor. */
        ~SharedResourceProbe();

        ItemVector* CollectItems ( Object* object );

        Item* CreateItem();

        /** Ensure that the SharedResourceProbe is a singleton. */
        static AbsProbe* Instance();

    private:

        /** SharedResourceProbe constructor. */
        SharedResourceProbe();

        /** Retrieve a particular Item from the shared resource's ItemVector using the shared resource's netname as the key.
         *  @param netNameStr A string that contains the share name of a Windows shared resource.
         *  @return The Item object whose share name matches the specified value.
         */
        Item* SharedResourceProbe::GetSharedResource ( string netNameStr );

        /** Retrieve all of the Windows shared resources on the local system.
         *  @return Void.
         */
        void SharedResourceProbe::GetAllSharedResources();

        /** Convert a SharedResource type DWORD value into its corresponding string value.
         *  Currently, as of Version 5.5, the OVAL Language only supports STYPE_DISKTREE, STYPE_PRINTQ,
         *  STYPE_DEVICE, STYPE_IPC, STYPE_SPECIAL, and STYPE_TEMPORARY as values for shared resource types.
         *  However, as the result of implementing this Probe, the shared resource types found in the corresponding
         *  function definition that are not mentioned above may be added to the OVAL Language in Version 5.6
         *  pending approval from the OVAL Board.  For more information regarding this issue, refer to the following link.
         *  http://n2.nabble.com/Proposal-to-Amend-the-win-def%3AEntityStateSharedResourceTypeType-Definition-td2617124.html.
         *  For Version 5.6, make sure to uncomment the case statements for the newly added shared resouce types, and to remove the STYPE_SPECIAL and STYPE_TEMPORARY case statements.
         *  @param sharedType A DWORD value that contains a bitmask of flags that represent the type of the Windows shared resource.
         *  @return A string representing the type of the Windows shared resource.
         */
        string SharedResourceProbe::GetSharedResourceType ( DWORD sharedType );

        /** Converts a wide-character string of Unicode characters into a string of ASCII characters.
         *  @param unicodeCharStr Pointer to the wide-character string of Unicode characters that you would like to convert into a string of ASCII characters.
         *  @return A string of ASCII characters.
         */
        string SharedResourceProbe::UnicodeToAsciiString ( wchar_t* unicodeCharStr );

        /** Delete all of the Items in the resources ItemVector.
         *  @return Void.
         */
        void SharedResourceProbe::DeleteSharedResources();

        /** The static instance of the SharedResourceProbe.
         *  All Probes are singletons. The ProbeFactory is responsible for managing instances of Probes.
         */
        static SharedResourceProbe* instance;

        /** The ItemVector that holds the information about all of the Windows shared resources on the local system */
        ItemVector* resources;

};

#endif