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

#ifndef REGISTRYFINDER_H
#define REGISTRYFINDER_H

#include <string>

#include "REGEX.h"
#include "Exception.h"
#include "ObjectEntity.h"
#include "BitnessView.h"
#include "Behavior.h"

/**
    This class provides a data structure for storing and accessing registry keys and values
    that are collected when using the AbsRegistryFinder class.
*/
class RegKey {
    public:
		RegKey(){}

		/** RegKey constructor. */
        RegKey (std::string hive,std::string key,std::string name ) :
		  hive(hive), key(key), name(name)
		{}

        /** Return the hive value of the registry key item.
         *  @return A string representing the hive value of the registry key item.
         */
        std::string GetHive() const {
			return hive;
		}

        /** Return the key value of the registry key item.
         *  @return A string representing the key value of the registry key item.
         */
        std::string GetKey() const {
			return key;
		}

        /** Return the name value of the registry key item.
         *  @return A string representing the name value of the registry key item.
         */
        std::string GetName() const {
			return name;
		}

        /** Set the hive value of the registry key item.
         *  @param hive A string value representing the hive of the registry item.
         *  @return Void.
         */
        void SetHive ( const std::string &hive ) {
			this->hive = hive;
		}

        /** Set the key value of the registry key item.
         *  @param key A string value representing the key of the registry item.
         *  @return Void.
         */
        void SetKey ( const std::string &key ) {
			this->key = key;
		}

        /** Set the name value of the registry key item.
         *  @param name A string value representing the name of the registry item.
         *  @return Void.
         */
        void SetName ( const std::string &name ) {
			this->name = name;
		}

		/** Returns a string representation of this registry key. */
		std::string ToString() const;

    private:
        std::string hive;
        std::string key;
        std::string name;

		/**
		 * Concatenation of hive plus key, for debug messages.  Or
		 * wherever else you might need it.  Delayed creation.
		 */
		mutable std::string hivePlusKey;
		// have to make this mutable so it can spring into
		// existence when needed, even on a const object...
};

typedef std::vector<RegKey*> RegKeyVector;

/**
 * This class provides access to Windows' registry.
 *
 * For safety's sake, it is a good idea to instantiate RegistryFinder
 * with a particular bitness view (32 or 64-bit), and obtain all keys
 * through that object.  That way you can be sure you're always
 * looking at a correct and consistent view of the registry.
 */
class RegistryFinder {
    public:

        /**
		 * Create a RegistryFinder for a given view of the registry.
		 * The given view isn't necessarily honored.  E.g. you can't
		 * query a 64-bit view from a 32-bit app running on 32-bit
		 * windows.  Currently, 32-bit apps always query 32-bit views,
		 * regardless of this setting.
		 */
		explicit RegistryFinder(BitnessView view);

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
        StringSet* GetKeys ( std::string hiveStr, ObjectEntity* keyEntity, BehaviorVector* behaviors );

        /** Get the set of all names on the system that match the object.
         *  @param hiveStr A string that contains the hive to be used during the matching process.
         *  @param keyStr A string that contains the key to be used during the matching process.
         *  @param nameEntity A ObjectEntity that represents the name entity in an Object as defined in the OVAL Definition Schema.
         *  @return A StringSet that contains all of the names specified in the ObjectEntity.
         */
        StringSet* GetNames ( std::string hiveStr, std::string keyStr, ObjectEntity* nameEntity );

		/** Determine whether non-existence of a hive should be reported.
         *  @param hiveEntity A ObjectEntity that represents the hive entity in an Object as defined in the OVAL Definition Schema.
         *  @return true if non-existence should be reported, false otherwise
         */
        bool ReportHiveDoesNotExist ( ObjectEntity* hiveEntity );

        /** Determine whether non-existence of a key should be reported.
         *  @param hiveStr A string that contains the hive to be used during the matching process.
         *  @param keyEntity A ObjectEntity that represents the key entity in an Object as defined in the OVAL Definition Schema.
         *  @return true if non-existence should be reported, false otherwise
         */
        bool ReportKeyDoesNotExist ( std::string hiveStr, ObjectEntity* keyEntity );

        /** Determine whether non-existence of a name should be reported.
         *  @param hiveStr A string that contains the hive to be used during the matching process.
         *  @param keyStr A string that contains the key to be used during the matching process.
         *  @param nameEntity A ObjectEntity that represents the name entity in an Object as defined in the OVAL Definition Schema.
         *  @return true if non-existence should be reported, false otherwise
         */
        bool ReportNameDoesNotExist ( std::string hiveStr, std::string keyString, ObjectEntity* nameEntity );
		
		/**
		 * Retrieve the handle of a specified registry key under the 
		 * specified handle. In order to retrieve the handle of a hive, 
		 * simply pass the empty string as the value for \p keyStr.
		 *
		 * @param[out] keyHandle Receives the handle to the opened key, if 
		 *	the open was successful.
		 * @param[in] hiveStr The hive name of the key whose handle you are 
		 *	trying to retrieve.
		 * @param[in] keyStr The key name whose handle you are trying to
		 *	retrieve.
		 * @param[in] access The desired key access.  Defaults to KEY_READ.
		 * @return An error status, as returned from RegOpenKeyEx().  This
		 *	will be ERROR_SUCCESS on success.  Docs imply that ERROR_SUCCESS
		 *	is zero; so non-zero return value indicates error.
		 */
		LONG GetHKeyHandle ( HKEY *keyHandle, std::string hiveStr, 
				std::string keyStr = "", REGSAM access = KEY_READ);

		/**
		 * Retrieve the handle of a specified registry subkey under the 
		 * specified superkey. In order to retrieve the handle of a hive, 
		 * simply pass the empty string as the value for \p subKeyStr.  But
		 * you'd have to pass the hive handle as \p superKey, which means
		 * you already have it.  So that usage is unlikely.
		 *
		 * @param[out] keyHandle Receives the handle to the opened key, if 
		 *	the open was successful.
		 * @param[in] superKey The super key under which the given subkey is
		 *	located.
		 * @param[in] subKeyStr A path, under \p superKey to the key to open.
		 * @param[in] access The desired key access.  Defaults to KEY_READ.
		 * @return An error status, as returned from RegOpenKeyEx().  This
		 *	will be ERROR_SUCCESS on success.  Docs imply that ERROR_SUCCESS
		 *	is zero; so non-zero return value indicates error.
		 */
		LONG GetHKeyHandle ( HKEY *keyHandle, HKEY superKey, 
			std::string subKeyStr = "", REGSAM access = KEY_READ);

		/** Build a valid registry key out of the input hive and key. If the input key is empty or null the hive is returned.
		 *	@param hiveStr A non-NULL string representing the hive portion of the registry key.
		 *	@param keyStr A string representing the key portion of the registry key.
         *  @return A string representing the registry key.
		 */
		static std::string BuildRegistryKey(const std::string hiveStr, const std::string keyStr);
				
		/** Convert the hive name to the abbreviated hive name used by the Windows object for registry keys.
		 *	@param hiveStr A string representing the hive portion of the registry key.
         *  @return A string representing the abbreviated hive name used by the Windows object for registry keys.
		 */
		static std::string ConvertHiveForWindowsObjectName( std::string hiveStr );

        /** Determine if the hive exists in the registry on the local system.
         *  @param hiveStr A string that contains the hive whose existence you would like to check. Possible values are HKEY_CLASSES_ROOT, HKEY_CURRENT_CONFIG, HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE, and HKEY_USERS.
         *  @return A boolean value indicating whether or not the specified hive exists.
         */
        bool HiveExists ( std::string hiveStr );

        /** Determine if the key exists in the registry on the local system under the specified hive.
         *  @param hiveStr A string that contains the hive of the key whose existence you would like to check.
         *  @param keyStr A string that contains the key whose existence you would like to check.
         *  @return A boolean value indicating whether or not the specified key exists in the specified hive.
         */
        bool KeyExists ( std::string hiveStr, std::string keyStr );

        /** Determine if the name exists in the registry on the local system under the specified hive and key.
         *  @param hiveStr A string that contains the hive of the name whose existence you would like to check.
         *  @param keyStr A string that contains the key of the name whose existence you would like to check.
         *  @param nameStr A string that contains the name of the registry entry whose existence you would like to check.
         *  @return A boolean value indicating whether or not the specified key exists in the specified hive.
         */
        bool NameExists ( std::string hiveStr, std::string keyStr, std::string nameStr );

		/** Returns the registry view this finder queries. */
		BitnessView GetView() const {
			return bitnessView;
		}

		/** Represents the '\' character which is used to separate different registry keys. */
		static char keySeparator;

    private:
		/** Retrieve all of the hives that match the specified pattern.
         *  @param patternStr A string that contains the pattern to be matched.
         *  @param hives A StringSet that represents the set of matching hives.
         *  @param isRegex A boolean value indicating whether or not the pattern represents a regular expression.
         *  @return Void.
         */
        void FindHives ( std::string patternStr, StringSet* hives, bool isRegex = true );

        /** Retrieve all of the keys that match the specified hive and pattern.
         *  @param hiveStr A string that contains the hive to be matched.
         *  @param patternStr A string that contains the pattern to be matched.
         *  @param keys A StringSet that represents the set of matching keys.
         *  @param isRegex A boolean value indicating whether or not the pattern represents a regular expression.
         *  @return Void.
         */
        void FindKeys ( std::string hiveStr, std::string regex, StringSet* keys, bool isRegex = true );

        /** Retrieve all of the names that match the specified hive, key, and pattern.
         *  @param hiveStr A string that contains the hive to be matched.
         *  @param keyStr A string that contains the key to be matched.
         *  @param patternStr A string that contains the pattern to be matched.
         *  @param names A StringSet that represents the set of matching names.
         *  @param isRegex A boolean value indicating whether or not the pattern represents a regular expression.
         *  @return Void.
         */
        void FindNames ( std::string hiveStr, std::string keyStr, std::string patternStr, StringSet* names, bool isRegex = true );

        /** Retrieve all of the hives on the system.
         *  @return A pointer to a StringSet of values that contains all of the hives retrieved from the system.
         */
        StringSet* GetAllHives();

        /** Retrieve all of the sub-keys under a specified registry hive and key.
         *  @param hiveStr A string that contains the hive of the key whose sub-keys you are trying to retrieve.
         *  @param keyStr A string that contains the key whose sub-keys you are trying to retrieve.
         *  @return A pointer to a StringSet of keys that contain all of the sub-keys retrieved under the specified hive and key.
         */
        StringSet* GetAllSubKeys ( std::string hiveStr, std::string keyStr );

        /** Retrieve all of the names under a specified registry hive and key.
         *  @param hiveStr A string that contains the hive of the key whose names you are trying to retrieve.
         *  @param keyStr A string that contains the key whose names you are trying to retrieve.
         *  @return A pointer to a StringSet of values that contains all of the names retrieved under the specified hive and key.
         */
        StringSet* GetAllNames ( std::string hiveStr, std::string keyStr );

        /** Retrieve the set of matching keys after applying the behaviors recurse_direction and max_depth.
         *  @param keys A pointer to a StringSet of keys which need to be iterated in the context of the behaviors.
         *  @param behaviors A pointer to a BehaviorVector that influences the collection of keys on the local system.
         *  @return A pointer to a StringVector of the keys that were found
         */
        StringSet* ProcessKeyBehaviors ( std::string hiveStr, StringSet* keys, BehaviorVector* behaviors );

        /** Get the set of keys in the specified hive that match the specified pattern.
         *  @param hiveStr A string that contains the hive to be used during the matching process.
         *  @param keyStr A string that contains the key to be used during the matching process.
         *  @param regexStr A string that contains the regular expression to be used during the matching process.
		 *  @param keys A StringSet that represents the set of matching keys.
         *  @param isRegex A boolean value indicating whether or not the pattern represents a regular expression.
         *  @return Void.
         */
        void GetRegistriesForPattern ( std::string hiveStr, std::string key, std::string regexStr, StringSet *keys, bool isRegex );

		/** Return true if the specified value matches the specified pattern.
         *  If the isRegex flag is true the match is treated as a regex, otherwise
         *  a string comparison is preformed to check that the value is not equal to
         *  the pattern.
         *  @param patternStr A string value representing the pattern to be matched.
         *  @param valueStr A string value representing the value that you would like to match against the pattern.
         *  @param isRegex A boolean value indicating whether or not the pattern represents a regular expression.
         *  @return A boolean value that indicates whether or not the value matches the pattern.
         */
        bool IsMatch ( std::string patternStr, std::string valueStr, bool isRegex = true );

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
        void DownwardRegistryRecursion ( StringSet* keys, std::string hiveStr, std::string keyStr, int maxDepth );

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
        void UpwardRegistryRecursion ( StringSet* keys, std::string hiveStr, std::string keyStr, int maxDepth );

        REGEX *registryMatcher;

		BitnessView bitnessView;
};

/**
    This class represents an Exception that occured while using a RegistryFinder.
*/
class RegistryFinderException : public Exception {
    public:
		RegistryFinderException ( std::string errMsgIn = "", int severity = ERROR_FATAL, Exception* ex = NULL );
		~RegistryFinderException();
};

#endif