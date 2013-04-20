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

#ifndef FILEAUDITEDPERMISSIONS53PROBE_H
#define FILEAUDITEDPERMISSIONS53PROBE_H

#include "AbsEffectiveRightsProbe.h"

/**
    This class is responsible for collecting file information for windows fileauditedpermissions53_objects.
*/
class FileAuditedPermissions53Probe : public AbsEffectiveRightsProbe {

    public:
        /** FileAuditedPermissions53Probe destructor. */
        virtual ~FileAuditedPermissions53Probe();

        virtual ItemVector* CollectItems ( Object* object );

        /** Ensure that the FileAuditedPermissions53Probe is a singleton. */
        static AbsProbe* Instance();

    private:

        /** FileAuditedPermissions53Probe constructor. */
        FileAuditedPermissions53Probe();

        /** Return a new Item created for storing file information. */
        virtual Item* CreateItem();

        /** Get the audited permissions for a trustee SID for the specified path and filename.
         *  @param path A string that contains the path of the file that you want to get the audited permissions of.
         *  @param fileName A string that contains the name of the file that you want to get the audited permissions of.
         *  @param trusteeSID A string that contains the trustee SID of the file that you want to get the audited permissions of.
         *  @return The item that contains the file audited permissions of the specified path, filename, and trustee SID.
         */
        Item* GetAuditedPermissions ( HANDLE fileHandle, std::string path, std::string fileName, std::string trusteeSID );

        /** The static instance of the FileAuditedPermissions53Probe.
         *  All Probes are singletons. The ProbeFactory is responsible for managing instances of Probes.
         */
        static FileAuditedPermissions53Probe* instance;
};

#endif
