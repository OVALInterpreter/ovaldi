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
#ifndef SHAREDRESOURCEPROBE_H
#define SHAREDRESOURCEPROBE_H

#include <string>

#include "AbsProbe.h"
#include "Item.h"
#include "Object.h"

/** This class is responsible for collecting Windows shared resource data. */

class SharedResourceProbe : public AbsProbe {

    public:

        /** SharedResourceProbe destructor. */
        virtual ~SharedResourceProbe();

        virtual ItemVector* CollectItems ( Object* object );

        /** Ensure that the SharedResourceProbe is a singleton. */
        static AbsProbe* Instance();

    private:

        /** SharedResourceProbe constructor. */
        SharedResourceProbe();
        
        virtual Item* CreateItem();

        /** Retrieve a particular Item from the shared resource's ItemVector using the shared resource's netname as the key.
         *  @param netNameStr A string that contains the share name of a Windows shared resource.
         *  @return The Item object whose share name matches the specified value.
         */
        Item* GetSharedResource ( std::string netNameStr );

        /** Retrieve all of the Windows shared resources on the local system.
         *  @return Void.
         */
        void GetAllSharedResources();

        /** Delete all of the Items in the resources ItemVector.
         *  @return Void.
         */
        void DeleteSharedResources();

        /** The static instance of the SharedResourceProbe.
         *  All Probes are singletons. The ProbeFactory is responsible for managing instances of Probes.
         */
        static SharedResourceProbe* instance;

        /** The ItemVector that holds the information about all of the Windows shared resources on the local system */
        ItemVector* resources;

};

#endif
