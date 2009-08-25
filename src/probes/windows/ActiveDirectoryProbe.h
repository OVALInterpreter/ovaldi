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

#ifndef ACTIVEDIRECTORYPROBE_H
#define ACTIVEDIRECTORYPROBE_H

#include "AbsProbe.h"
#include "Common.h"
#include "WindowsCommon.h"
#include <Winldap.h>
#include <dsgetdc.h>
#include <iads.h>
#include <activeds.h>
#include <iomanip>

/** This class is responsible for collecting Windows Active Directory data. */

typedef map < string , StringKeyedItemMap* > ActiveDirectoryMap;

class ActiveDirectoryProbe : public AbsProbe {

    public:

        /** ActiveDirectoryProbe destructor. */
        ~ActiveDirectoryProbe();

        ItemVector* CollectItems ( Object* object );

        /** Ensure that the ActiveDirectoryProbe is a singleton. */
        static AbsProbe* Instance();

    private:

        /** ActiveDirectoryProbe constructor. */
        ActiveDirectoryProbe();

        Item* CreateItem();

        /** Converts the Active Directory attribute type into its string representation.
         *  @param adsType A ADSTYPE value which represents the type of the Active Directory attribute.
         *  @return A string representation of an Active Directory attribute type.
         */
        string GetAdsType ( ADSTYPE adsType );

        /** Retrieve a particular Active Directory item from the Active Directory cache using the Active Directory's naming context, relative distinguished name, and attribute as the key.
         *  @param namingContextStr A string that contains the naming context of an Active Directory item.  Possible values are 'domain', 'configuration', and 'schema'.
         *  @param relativeDnStr A string that contains the relative distinguished name of an Active Directory item.
         *  @param attributeStr A string that contains the naming context of an Active Directory item.
         *  @return The Item representing an Active Directory item whose naming context, relative distinguished name, and attribute match the specified values.
         */
        Item* GetActiveDirectoryData ( string namingContextStr , string relativeDnStr , string attributeStr );

        /** Retrieves the object class of the Active Directory item with the specified naming context and relative distinguished name.
         *  @param namingContextStr A string that contains the naming context of the Active Directory item.
         *  @param relativeDnStr A string that contains the relative distinguished name of the Active Directory item.
         *  @return A string value that contains the object class of the Active Directory item with the specified naming context and relative distinguished name.
         */
        string GetObjectClass ( string namingContextStr , string relativeDnStr );

        /** Creates a distinguished name with the given naming context and relative distinguished name.
         *  @param namingContextStr A string that contains the naming context of an Active Directory item.  Possible values are 'domain', 'configuration', and 'schema'.
         *  @param relativeDnStr A string that contains the relative distinguished name of an Active Directory item.
         *  @return A string representing the distinguished name of an Active Directory item.
         */
        string BuildDistinguishedName ( string namingContextStr, string relativeDnStr );

        /** Retrieve the domain components of the Active Directory.
         *  @return Void.
         */
        string GetDomainComponents();

        /** Remove the base distinguished name from the distinguished name and return the relative distinguished name.
         *  @param namingContextStr A string that contains the naming context of the Active Directory item.
         *  @param distinguishedNameStr A string that contains the relative distinguished name of the Active Directory item.
         *  @return A wstring value that represents the relative distinguished name of the distinguished name that was specified.
         */
        string RemoveDnBase ( string namingContextStr, string distinguishedNameStr );

        /** Get the set of all naming context values on the system that match the object.
         *  @param namingContextEntity A ObjectEntity that represents the naming context entity in an Object as defined in the OVAL Definition Schema.
         *  @return A StringSet containing all of the naming context values specified in the ObjectEntity.
         */
        StringSet* GetNamingContexts ( ObjectEntity* namingContextEntity );

        /** Retrieve all of the matching naming context values.
         *  @param patternStr A string that contains the pattern to be matched.
         *  @param isRegex A boolean value that specifies whether or not the pattern is a regular expression.
         *  @return A StringSet containing all of the matching naming context values.
         */
        StringSet* GetMatchingNamingContexts ( string patternStr , bool isRegex );

        /** Determine if the specified naming context value exists.
         *  @param namingContextStr A string that contains the naming context of the Active Directory item.
         *  @return A boolean value indicating whether or not a Active Directory item with the specified naming context value exists.
         */
        bool NamingContextExists ( string namingContextStr );

        /** Get the set of all relative distinguished names on the system that match the object.
         *  @param namingContextStr A string that contains the naming context value to be used during the matching process.
         *  @param relativeDnEntity A ObjectEntity that represents the relative distinguished name entity in an Object as defined in the OVAL Definition Schema.
         *  @return A StringSet that contains all of the relative distinguished names specified in the ObjectEntity that also match the specified naming context.
         */
        StringSet* GetRelativeDns ( string namingContextStr , ObjectEntity* relativeDnEntity );

        /** Retrieve all of the matching relative distinguished names.
         *  @param namingContextStr A string that contains the naming context value to be used during the matching process.
         *  @param patternStr A string that contains the pattern to be matched.
         *  @param isRegex A boolean value that specifies whether or not the pattern is a regular expression.
         *  @return A StringSet containing all of the matching relative distinguished names that also match the specified naming context.
         */
        StringSet* GetMatchingRelativeDns ( string namingContextStr , string patternStr , bool isRegex );

        /** Deterimine if the specified relative distinguished name value exists.
         *  @param namingContextStr A string that contains the naming context value of the Active Directory item.
         *  @param relativeDnStr A string that contains the relative distinguished name of the Active Directory item.
         *  @return A boolean value indicating whether or not a Active Directory item with the specified naming context and relative distinguished name exists.
         */
        bool RelativeDnExists ( string namingContextStr , string relativeDnStr );

        /** Get the set of all Active Directory attributes on the system that match the object.
         *  @param namingContextStr A string that contains the naming context value to be used during the matching process.
         *  @param relativeDnStr A string that contains the relative distinguished name to be used during the matching process.
         *  @param attributeEntity A ObjectEntity that represents the attribute entity in an Object as defined in the OVAL Definition Schema.
         *  @return A StringSet that contains all of the attributes specified in the ObjectEntity that also match the specified naming context and relative distinguished name.
         */
        StringSet* GetAttributes ( string namingContextStr , string relativeDnStr , ObjectEntity* attributeEntity );

        /** Retrieve all of the matching Active Directory attributes.
         *  @param namingContextStr A string that contains the naming context value to be used during the matching process.
         *  @param relativeDnStr A string that contains the relative distinguished name to be used during the matching process.
         *  @param patternStr A string that contains the pattern to be matched.
         *  @param isRegex A boolean value that specifies whether or not the pattern is a regular expression.
         *  @return A StringSet containing all of the matching attributes that also match the specified naming context and relative distinguished name.
         */
        StringSet* GetMatchingAttributes ( string namingContextStr , string relativeDnStr , string patternStr , bool isRegex );

        /** Deterimine if the specified attribute value exists.
         *  @param namingContextStr A string that contains the naming context of the Active Directory item.
         *  @param relativeDnStr A string that contains the relative distinguished name of the Active Directory item.
         *  @param attributeStr A string that contains the attribute of the Active Directory item.
         *  @return A boolean value indicating whether or not a Active Directory item with the specified naming context, relative distinguished name, and attribute exists.
         */
        bool AttributeExists ( string namingContextStr , string relativeDnStr , string attributeStr );

        /** Retrieves data from the Active Directory.
         *  @param namingContextStr A string that contains the naming context of the Active Directory item.
         *  @param relativeDnStr A string that contains the relative distinguished name of the Active Directory item.
         *  @param activeDirectoryOperationStr A string that contains the data to retrieve from the Active Directory.  Possible values are: GET_ALL_ATTRIBUTES, GET_ALL_DISTINGUISHED_NAMES, OBJECT_EXISTS, or a the name of the attribute that you would like to retrieve.
         *  @param activeDirectoryData A pointer to a StringSet which contains all of the distinguished names or attributes depending on which value is specified for the activeDirectoryOperation.  This value should be NULL when not retrieving all of the Active Directory distinguished names or all of the attributes for a particular distinguished name.
         *  @return A boolean value indicating that operation performed was completed successfully.
         */
        bool QueryActiveDirectory ( string namingContextStr , string relativeDnStr , string activeDirectoryOperationStr , StringSet* activeDirectoryData );

        /** Converts an ADS_OCTET_STRING structure into its equivalent string representation.  Note that this function only works for the 'objectSid' and 'objectGUID' attributes.
         *  @param attributeStr A string that represents the attribute name whose value you are trying to convert.
         *  @return A string representation of the attribute's ADS_OCTET_STRING value.
         */
        string ConvertOctetString ( string attributeStr, ADS_OCTET_STRING* octetString );

        /** Converts SYSTEMTIME structure into its equivalent UTC time representation string.
         *  @param time A pointer to a SYSTEMTIME structure which represents the a time value of an Active Directory attribute.
         *  @return A string representation of the attribute's time value in UTC.
         */
        string BuildUTCTimeString ( SYSTEMTIME *time );

        /** Retrieve a string representation of the last ADSI error message.
         *  @param hResult A HRESULT that represents the return value of some ADSI API function call.
         *  @return A string representation of the last ADSI error message.
         */
        string GetLastAdsiErrorMessage ( HRESULT hResult );

        /** Delete all of the distinguished names in the in the distinguished names StringSet*.
        *  @return Void.
        */
        void DeleteDistinguishedNames();

        /** Delete all of the Active Directory items from the Active Directory cache.
        *  @return Void.
        */
        void DeleteActiveDirectoryCache();

        /** The static instance of the ActiveDirectoryProbe.
         *  All Probes are singletons. The ProbeFactory is responsible for managing instances of Probes.
         */
        static ActiveDirectoryProbe* instance;

        /** The map data structure which serves as the cache for the Active Directory items collected */
        ActiveDirectoryMap* activeDirectoryCache;

        /** The StringSet that holds all of the distinguished names in the Active Directory */
        StringSet* distinguishedNames;

        /** The string that contains the base distinguished name of the Active Directory */
        string domainName;

        /** String constant representing the domain naming context value */
        static const string NAMING_CONTEXT_TYPE_DOMAIN;

        /** String constant representing the configuration naming context value */
        static const string NAMING_CONTEXT_TYPE_CONFIGURATION;

        /** String constant representing the schema naming context value */
        static const string NAMING_CONTEXT_TYPE_SCHEMA;

        /* String constants that represent the function to be performed by the ActiveDirectoryProbe::QueryActiveDirectory() method
         * GET_ALL_ATTRIBUTES - Retrieve all of the attributes for the specified Active Directory object
         * GET_ALL_DISTINGUISHED_NAMES - Retrieve all of the distinguished names from the Active Directory
         * OBJECT_EXISTS - Determine if the specified Active Directory object exists
         */
        static const string GET_ALL_ATTRIBUTES;
        static const string GET_ALL_DISTINGUISHED_NAMES;
        static const string OBJECT_EXISTS;

        /** String constant representing the distinguishedName attribute */
        static const string DISTINGUISHED_NAME_ATTRIBUTE;

        /** String constant representing the objectClass attribute */
        static const string OBJECT_CLASS_ATTRIBUTE;

        /** String constant representing the LDAP protocol */
        static const string LDAP_PROTOCOL;
};

#endif
