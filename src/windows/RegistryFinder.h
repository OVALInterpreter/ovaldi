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

#ifndef REGISTRYFINDER_H
#define REGISTRYFINDER_H

#pragma warning(disable:4786)

#include "Log.h"
#include "Common.h"
#include "Exception.h"
#include "REGEX.h"
#include "ObjectEntity.h"
#include "ItemEntity.h"
#include "WindowsCommon.h"

#include <iostream>

using namespace std;

/**
    This class provides a data structure for storing and accessing registry keys and values
    that are collected when using the AbsRegistryFinder class.
*/
class RegKey {
    public:
        /** RegKey constructor. */
        RegKey();
        
		/** RegKey constructor. */
        RegKey ( string hive, string key, string name );

        /** Return the hive value of the registry key item.
         *  @return A string representing the hive value of the registry key item.
         */
        string GetHive();

        /** Return the key value of the registry key item.
         *  @return A string representing the key value of the registry key item.
         */
        string GetKey();

        /** Return the name value of the registry key item.
         *  @return A string representing the name value of the registry key item.
         */
        string GetName();

        /** Set the hive value of the registry key item.
         *  @param hive A string value representing the hive of the registry item.
         *  @return Void.
         */
        void SetHive ( string hive );

        /** Set the key value of the registry key item.
         *  @param key A string value representing the key of the registry item.
         *  @return Void.
         */
        void SetKey ( string key );

        /** Set the name value of the registry key item.
         *  @param name A string value representing the name of the registry item.
         *  @return Void.
         */
        void SetName ( string name );

    private:
        string hive;
        string key;
        string name;
};

typedef vector<RegKey*> RegKeyVector;

/**
    This class is the windows registry searching implementation used by this application
*/
class RegistryFinder {
    public:
        /** RegistryFinder constructor. */
        RegistryFinder();

        /** RegistryFinder destructor. */
        ~RegistryFinder();

		/** Return the set of registry keys as a vector of RegKey objects that match the specified criteria.
         *  @param hiveEntity A ObjectEntity that represents the hive entity in an Object as defined in the OVAL Definition Schema.
         *  @param keyEntity A ObjectEntity that represents the key entity in an Object as defined in the OVAL Definition Schema.
         *  @param nameEntity A ObjectEntity that represents the name entity in an Object as defined in the OVAL Definition Schema.
         *  @param behaviors A BehaviorVector that represents the behaviors that should be applied to the registry keys.
         *  @return A RegKeyVector that contains all of the registry key items that match the specified criteria.
        */
        RegKeyVector* SearchRegistries ( ObjectEntity* hiveEntity, ObjectEntity* keyEntity, ObjectEntity* nameEntity, BehaviorVector* behaviors );

		/** Get the set of all hives on the system that match the object.
         *  @param hiveEntity A ObjectEntity that represents the hive entity in an Object as defined in the OVAL Definition Schema.
         *  @return A StringSet that contains all of the hives specified in the ObjectEntity.
         */
        StringSet* GetHives ( ObjectEntity* hiveEntity );

        /** Get the set of all keys on the system that match the object.
         *  @param hiveStr A string that contains the hive to be used during the matching process.
         *  @param keyEntity A ObjectEntity that represents the key entity in an Object as defined in the OVAL Definition Schema.
         *  @param behaviors A BehaviorVector that represents the behaviors that should be applied to the registry keys.
         *  @return A StringSet that contains all of the keys specified in the ObjectEntity.
         */
        StringSet* GetKeys ( string hiveStr, ObjectEntity* keyEntity, BehaviorVector* behaviors );

        /** Get the set of all names on the system that match the object.
         *  @param hiveStr A string that contains the hive to be used during the matching process.
         *  @param keyStr A string that contains the key to be used during the matching process.
         *  @param nameEntity A ObjectEntity that represents the name entity in an Object as defined in the OVAL Definition Schema.
         *  @return A StringSet that contains all of the names specified in the ObjectEntity.
         */
        StringSet* GetNames ( string hiveStr, string keyStr, ObjectEntity* nameEntity );

		/** Return a StringSet of hives, if any, that do not exist on the system.
         *  @param hiveEntity A ObjectEntity that represents the hive entity in an Object as defined in the OVAL Definition Schema.
         *  @return A StringSet that contains all of the names specified in the ObjectEntity that do not exist on the system.  If all of the hives exist on the system this method returns NULL.
         */
        StringSet* ReportHiveDoesNotExist ( ObjectEntity* hiveEntity );

        /** Return a StringSet of keys, if any, that do not exist on the system.
         *  @param hiveStr A string that contains the hive to be used during the matching process.
         *  @param keyEntity A ObjectEntity that represents the key entity in an Object as defined in the OVAL Definition Schema.
         *  @return A StringSet that contains all of the keys specified in the ObjectEntity that do not exist on the system.  If all of the keys exist on the system this method returns NULL.
         */
        StringSet* ReportKeyDoesNotExist ( string hiveStr, ObjectEntity* keyEntity );

        /** Return a StringSet of names, if any, that do not exist on the system.
         *  @param hiveStr A string that contains the hive to be used during the matching process.
         *  @param keyStr A string that contains the key to be used during the matching process.
         *  @param nameEntity A ObjectEntity that represents the name entity in an Object as defined in the OVAL Definition Schema.
         *  @return A StringSet that contains all of the names specified in the ObjectEntity that do not exist on the system.  If all of the names exist on the system this method returns NULL.
         */
        StringSet* ReportNameDoesNotExist ( string hiveStr, string keyString, ObjectEntity* nameEntity );
		
		/** Retrieve the handle of a specified registry key under the specified handle. In order to retrieve the handle of a hive, simply pass the empty string as the value for keyStr.
         *  @param hiveStr A string that contains the hive of the key whose handle you are trying to retrieve.
         *  @param keyStr A string that contains the key whose handle you are trying to retrieve.
         *  @return A handle to the registry entry with the specified hive and key.
         */
        static HKEY GetHKeyHandle ( string hiveStr, string keyStr );
		
		/** Build a valid registry key out of the input hive and key. If the input key is empty or null the hive is returned.
		 *	@param hiveStr A non-NULL string representing the hive portion of the registry key.
		 *	@param keyStr A string representing the key portion of the registry key.
         *  @return A string representing the registry key.
		 */
		static string BuildRegistryKey(const string hiveStr, const string keyStr);
				
		/** Convert the hive name to the abbreviated hive name used by the Windows object for registry keys.
		 *	@param hiveStr A string representing the hive portion of the registry key.
         *  @return A string representing the abbreviated hive name used by the Windows object for registry keys.
		 */
		static string ConvertHiveForWindowsObjectName( string hiveStr );
		
		/** Represents the '\' character which is used to separate different registry keys. */
		static char keySeparator;

    private:
        /** Determine if the hive exists in the registry on the local system.
         *  @param hiveStr A string that contains the hive whose existence you would like to check. Possible values are HKEY_CLASSES_ROOT, HKEY_CURRENT_CONFIG, HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE, and HKEY_USERS.
         *  @return A boolean value indicating whether or not the specified hive exists.
         */
        bool HiveExists ( string hiveStr );

        /** Determine if the key exists in the registry on the local system under the specified hive.
         *  @param hiveStr A string that contains the hive of the key whose existence you would like to check.
         *  @param keyStr A string that contains the key whose existence you would like to check.
         *  @return A boolean value indicating whether or not the specified key exists in the specified hive.
         */
        bool KeyExists ( string hiveStr, string keyStr );

        /** Determine if the name exists in the registry on the local system under the specified hive and key.
         *  @param hiveStr A string that contains the hive of the name whose existence you would like to check.
         *  @param keyStr A string that contains the key of the name whose existence you would like to check.
         *  @param nameStr A string that contains the name of the registry entry whose existence you would like to check.
         *  @return A boolean value indicating whether or not the specified key exists in the specified hive.
         */
        bool NameExists ( string hiveStr, string keyStr, string nameStr );

		/** Retrieve all of the hives that match the specified pattern.
         *  @param patternStr A string that contains the pattern to be matched.
         *  @param hives A StringSet that represents the set of matching hives.
         *  @param isRegex A boolean value indicating whether or not the pattern represents a regular expression.
         *  @return Void.
         */
        void FindHives ( string patternStr, StringSet* hives, bool isRegex = true );

        /** Retrieve all of the keys that match the specified hive and pattern.
         *  @param hiveStr A string that contains the hive to be matched.
         *  @param patternStr A string that contains the pattern to be matched.
         *  @param keys A StringSet that represents the set of matching keys.
         *  @param isRegex A boolean value indicating whether or not the pattern represents a regular expression.
         *  @return Void.
         */
        void FindKeys ( string hiveStr, string regex, StringSet* keys, bool isRegex = true );

        /** Retrieve all of the names that match the specified hive, key, and pattern.
         *  @param hiveStr A string that contains the hive to be matched.
         *  @param keyStr A string that contains the key to be matched.
         *  @param patternStr A string that contains the pattern to be matched.
         *  @param names A StringSet that represents the set of matching names.
         *  @param isRegex A boolean value indicating whether or not the pattern represents a regular expression.
         *  @return Void.
         */
        void FindNames ( string hiveStr, string keyStr, string patternStr, StringSet* names, bool isRegex = true );

        /** Retrieve all of the hives on the system.
         *  @return A pointer to a StringSet of values that contains all of the hives retrieved from the system.
         */
        StringSet* GetAllHives();

        /** Retrieve all of the sub-keys under a specified registry hive and key.
         *  @param hiveStr A string that contains the hive of the key whose sub-keys you are trying to retrieve.
         *  @param keyStr A string that contains the key whose sub-keys you are trying to retrieve.
         *  @return A pointer to a StringSet of keys that contain all of the sub-keys retrieved under the specified hive and key.
         */
        StringSet* GetAllSubKeys ( string hiveStr, string keyStr );

        /** Retrieve all of the names under a specified registry hive and key.
         *  @param hiveStr A string that contains the hive of the key whose names you are trying to retrieve.
         *  @param keyStr A string that contains the key whose names you are trying to retrieve.
         *  @return A pointer to a StringSet of values that contains all of the names retrieved under the specified hive and key.
         */
        StringSet* GetAllNames ( string hiveStr, string keyStr );

        /** Retrieve the set of matching keys after applying the behaviors recurse_direction and max_depth.
         *  @param keys A pointer to a StringSet of keys which need to be iterated in the context of the behaviors.
         *  @param behaviors A pointer to a BehaviorVector that influences the collection of keys on the local system.
         *  @return A pointer to a StringVector of the keys that were found
         */
        StringSet* ProcessKeyBehaviors ( string hiveStr, StringSet* keys, BehaviorVector* behaviors );

        /** Get the set of keys in the specified hive that match the specified pattern.
         *  @param hiveStr A string that contains the hive to be used during the matching process.
         *  @param keyStr A string that contains the key to be used during the matching process.
         *  @param regexStr A string that contains the regular expression to be used during the matching process.
		 *  @param keys A StringSet that represents the set of matching keys.
         *  @param isRegex A boolean value indicating whether or not the pattern represents a regular expression.
         *  @return Void.
         */
        void GetRegistriesForPattern ( string hiveStr, string key, string regexStr, StringSet *keys, bool isRegex );

		/** Return true if the specified value matches the specified pattern.
         *  If the isRegex flag is true the match is treated as a regex, otherwise
         *  a string comparison is preformed to check that the value is not equal to
         *  the pattern.
         *  @param patternStr A string value representing the pattern to be matched.
         *  @param valueStr A string value representing the value that you would like to match against the pattern.
         *  @param isRegex A boolean value indicating whether or not the pattern represents a regular expression.
         *  @return A boolean value that indicates whether or not the value matches the pattern.
         */
        bool IsMatch ( string patternStr, string valueStr, bool isRegex = true );

        /** Do a recusive search down the registry until the specified maxDepth is hit.
         *  Each key traversed is added to the set of keys. If maxDepth is a positive
         *  number it is decremented with each recursive call and recursion stops when the
         *  maxDepth is 0. If maxDepth is -1 recursion continues until no sub-keys
         *  are found and a recursive call can not be made.
         *  @param keys A StringSet that represents the set of matching keys.
         *  @param hiveStr A string that contains the hive to be used during the matching process.
         *  @param keyStr A string that contains the key to be used during the matching process.
         *  @param maxDepth A integer value that represents the maximum depth to recurse.
         *  @return Void.
         */
        void DownwardRegistryRecursion ( StringSet* keys, string hiveStr, string keyStr, int maxDepth );

        /** Do a recusive search up the registry until the specified maxDepth is hit.
         *  Each registry key traversed is added to the set of keys. If maxDepth is a positive
         *  number it is decremented with each recursive call and recursion stops when the
         *  maxDepth is 0. If maxDepth is -1 recursion continues until no parent registry key
         *  is found and a recursive call can not be made.
         *  @param keys A StringSet that represents the set of matching keys.
         *  @param hiveStr A string that contains the hive to be used during the matching process.
         *  @param keyStr A string that contains the key to be used during the matching process.
         *  @param maxDepth A integer value that represents the maximum depth to recurse.
         *  @return Void.
         */
        void UpwardRegistryRecursion ( StringSet* keys, string hiveStr, string keyStr, int maxDepth );

        REGEX *registryMatcher;
};

/**
    This class represents an Exception that occured while using a RegistryFinder.
*/
class RegistryFinderException : public Exception {
    public:
		RegistryFinderException ( string errMsgIn = "", int severity = ERROR_FATAL, Exception* ex = NULL );
		~RegistryFinderException();
};

#endif