//
//
//****************************************************************************************//
// Copyright (c) 2002-2010, The MITRE Corporation
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
#ifndef VOLUMEPROBE_H
#define VOLUMEPROBE_H

#include "AbsProbe.h"
#include "WindowsCommon.h"
#include <TChar.h>

using namespace std;

/** This class is responsible for collecting Windows volume data. */

class VolumeProbe : public AbsProbe {

    public:

        /** VolumeProbe destructor. */
        virtual ~VolumeProbe();

        virtual ItemVector* CollectItems ( Object* object );

        /** Ensure that the VolumeProbe is a singleton. */
        static AbsProbe* Instance();

    private:

        /** VolumeProbe constructor. */
        VolumeProbe();

        virtual Item* CreateItem();

        /** Retrieve all of the Windows volumes on the local system.
         *  @return Void.
         */
        void GetAllVolumes();

        /** Collect all of the information associated with a particular Windows volume and use it to populate a Item object.
         *  @param rootPathStr A pointer to a TCHAR string that represents the rootpath of the Windows volume whose infomation you would like to enumerate.
         *  @return A Item object that contains all of the specified volume's associated information.
         */
        Item* BuildVolumeObject ( LPTSTR rootPathStr );

        /** Retrieve a particular Item from the instance of the ItemVector using the volume's rootpath as the key.
         *  @param rootPathStr A string that contains the rootpath of a Windows volume.
         *  @return The Item object whose roothpath matches the specified value.
         */
        Item* GetVolume ( string rootPathStr );

        /** Retrieve the pathname of a particular volume using the specified volume name.
         *  @param volumeNameStr Pointer to a TCHAR string that represents the name of the volume whose path you would like to enumerate.
         *  @return A string representing the path name of the specified volume.
         */
        string GetPathName ( LPTSTR volumeNameStr );

        /** Delete all of the Items in the volumes ItemVector.
         *  @return Void.
         */
        void DeleteVolumes();

        /** The static instance of the VolumeProbe.
        All Probes are singletons. The ProbeFactory is responsible for managing instances of Probes. */
        static VolumeProbe *instance;

        /** The ItemVector that holds the information about all of the Windows volumes on the local system. */
        ItemVector* volumes;

};

#endif
