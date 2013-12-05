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

#ifndef PORTPROBE_H
#define PORTPROBE_H

#include <string>

#include <OvalEnum.h>
#include "AbsProbe.h"

/** This class is responsible for collecting Windows port data. */

class PortProbe : public AbsProbe {

    public:

        /** PortProbe destructor. */
        virtual ~PortProbe();

        virtual ItemVector* CollectItems ( Object* object );

        /** Ensure that the PortProbe is a singleton. */
        static AbsProbe* Instance();

    private:

        /** PortProbe constructor. */
        PortProbe();

        virtual Item* CreateItem();

        /** Build an Item containing all of the port state entities.
         *  @param localAddressStr A string that contains the local address of a Windows port in dotted-quad notation.
         *  @param localPort A unsigned long integer that contains the local port value of a Windows port.
         *  @param protocolStr A string that contains the protocol of a Windows port.
         *  @param pid A unsigned long integer that contains the pid value of a Windows port.
         *  @return A Item representing a Windows port.
         */
        Item* BuildPortItem ( std::string localAddressStr , unsigned long localPort , std::string remoteAddressStr , unsigned long remotePort , std::string protocolStr , unsigned long pid );

        /** Retrieve a particular port from the instance of the ItemVector using the port's local address, port value, and protocol as the key.
         *  @param localAddressStr A string that contains the local address of a Windows port in dotted-quad notation.
         *  @param localPort A long long integer that contains the local port value of a Windows port.
         *  @param protocolStr A string that contains the protocol of a Windows port.
         *  @return The Item representing a Windows port whose local address, local port, and protocol match the specified values.
         */
        Item* GetPort ( std::string localAddressStr , long long localPort , std::string protocolStr );

        /** Get the set of all protocols on the system that match the object.
         *  @param protocol A ObjectEntity that represents the protocol entity in an Object as defined in the OVAL Definition Schema.
         *  @return A StringSet that contains all of the protocols specified in the ObjectEntity.
         */
        StringSet* GetProtocols ( ObjectEntity* protocol );

        /** Retrieve all of the matching protocols.
         *  @param pattern A string that contains the pattern to be matched.
         *  @param isRegex A boolean value that specifies whether or not the pattern is a regular expression.
         *  @return A StringSet containing all of the matching protocols.
         */
        StringSet* GetMatchingProtocols ( std::string pattern , bool isRegex );

        /** Determine if the specified protocol value exists.
         *  @param protocolStr A string that contains the protocol of the Windows port whose existence you are checking.
         *  @return A boolean value indicating whether or not a Windows port with the specified protocol exists.
         */
        bool ProtocolExists ( std::string protocolStr );

        /** Get the set of all local addresses on the system that match the object.
         *  @param protocolStr A string that contains the protocol to be used during the matching process.
         *  @param localAddress A ObjectEntity that represents the local_address entity in an Object as defined in the OVAL Definition Schema.
         *  @return A StringSet that contains all of the local addresses specified in the ObjectEntity that also match the specified protocol.
         */
        StringSet* GetLocalAddresses ( std::string protocolStr , ObjectEntity* localAddress );

        /** Retrieve all of the matching local addresses.
         *  @param protocolStr A string that contains the protocol to be used during the matching process.
         *  @param pattern A string that contains the pattern to be matched.
         *  @param isRegex A boolean value that specifies whether or not the pattern is a regular expression.
         *  @return A StringSet containing all of the matching local addresses that also match the specified protocol.
         */
        StringSet* GetMatchingLocalAddresses ( std::string protocolStr , std::string pattern , bool isRegex );

        /** Deterimine if the specified local address value exists.
         *  @param protocolStr A string that contains the protocol of the Windows port whose existence you are checking.
         *  @param localAddressStr A string that contains the local address of the Windows port whose existence you are checking.
         *  @return A boolean value indicating whether or not a Windows port with the specified protocol and local address exists.
         */
        bool LocalAddressExists ( std::string protocolStr , std::string localAddressStr );

        /** Get the set of all local ports on the system that match the object.
         *  @param protocolStr A string that contains the protocol to be used during the matching process.
         *  @param localAddressStr A string that contains the local address to be used during the matching process.
         *  @param localPort A ObjectEntity that represents the local_port entity in an Object as defined in the OVAL Definition Schema.
         *  @return A StringSet that contains all of the local ports specified in the ObjectEntity that also match the specified protocol and local address.
         */
        StringSet* GetLocalPorts ( std::string protocolStr , std::string localAddressStr , ObjectEntity* localPort );

        /** Retrieve all of the matching local ports.
         *  @param protocolStr A string that contains the protocol to be used during the matching process.
         *  @param localAddressStr A string that contains the local address to be used during the matching process.
         *  @param localPort A ObjectEntity that represents the local_port entity in an Object as defined in the OVAL Definition Schema.
         *  @param loopThroughAllVariables A boolean value that specifies whether or not to loop through all of the variables in the specified ObjectEntity.
         *  @return A StringSet containing all of the matching local ports that also match the specified protocol and local address.
         */
        StringSet* GetMatchingLocalPorts ( std::string protocolStr , std::string localAddressStr , ObjectEntity* localPort , bool loopThroughAllVariables );

        /** Deterimine if the specified local port value exists.
         *  @param protocolStr A string that contains the protocol of the Windows port whose existence you are checking.
         *  @param localAddressStr A string that contains the local address of the Windows port whose existence you are checking.
         *  @param localPortStr A string that contains the local port of the Windows port whose existence you are checking.
         *  @return A boolean value indicating whether or not a Windows port with the specified protocol, local address, and local port exists.
         */
        bool LocalPortExists ( std::string protocolStr , std::string localAddressStr , std::string localPortStr );

        /** Determine if the operation and value are valid for the integer datatype.
         *  @param op A OvalEnum::Operation that contains the operation whose validity you would like to check.
         *  @param port1 A long long integer that contains the actual local port value.
         *  @param port2 A long long integer that contains the specified local port value.
         *  @return A boolean value indicating whether or not the operation and port1 with respect to port2 are valid.
         */
        bool IsValidOperationAndValue ( OvalEnum::Operation op , long long port1 , long long port2 );

        /** Retrieve all of the Windows ports on the local system.
         *  @return Void.
         */
        void GetAllPorts();

        /** Delete all Items in the ports ItemVector.
         *  @return Void.
         */
        void DeletePorts();

        /** The static instance of the PortProbe.
         *  All Probes are singletons. The ProbeFactory is responsible for managing instances of Probes.
         */
        static PortProbe* instance;

        /** String constant representing the TCP protocol */
        static const std::string PROTOCOL_TCP;

        /** String constant representing the UDP protocol */
        static const std::string PROTOCOL_UDP;

        /** The ItemVector that holds the information about all of the Windows ports on the local system */
        ItemVector* ports;

};

#endif
