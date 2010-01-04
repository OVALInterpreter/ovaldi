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

#ifndef METABASEPROBE_H
#define METABASEPROBE_H

#pragma warning(disable:4786)

#include "AbsProbe.h"
#include "WindowsCommon.h"

#include <windows.h>
#include <initguid.h>
#include <iadmw.h>
#include <iiscnfg.h>

using namespace std;

/**
	This class is responsible for collecting metabase information for windows metabase_objects.
*/

class MetabaseProbe : public AbsProbe {

	public:
		/** Metabase probe destructor */
		virtual ~MetabaseProbe();

		virtual ItemVector* CollectItems ( Object* object );

		/** Gets single instance of the MetabaseProbe. Uses lazy initialization. */
		static AbsProbe* Instance();

	private:
		/** Metabase probe constructor */
		MetabaseProbe();

		/** Return a new Item created for storing file information */
		virtual Item* CreateItem();

		/** Initialize the IMSAdminBase interface that is used to query the metabase.
		 *  @return Void.
		 */
		void InitializeIMSAdminBaseInterface();

		/** Close the IMSAdminBase interface that is used to query the metabase.
		 * @return Void.
		 */
		void CloseIMSAdminBaseInterface();

		/** Get the set of keys on the system that match the object.
		 *  @param keyEntity A ObjectEntity that represents the key entity in an Object as defined in the OVAL Definition Schema.
		 *  @return A StringSet that contains all of the keys specified in the ObjectEntity that also match the specified key.
		 */
		StringSet* GetKeys ( ObjectEntity* keyEntity );

		/** Determine if the specified key exists.
		 *  @param keyStr A string that contains the key of a metabase item.
		 *  @return A boolean value indicating whether or not a metabase item with the specified key exists.
		 */
		bool KeyExists ( string keyStr );

		/** Get the set of keys that match the specified pattern.
		 *  @param keyStr A string that contains the key to be used during the matching process.
		 *  @param regexStr A string that contains the regular expression to be used during the matching process.
		 *  @param keys A StringSet that represents the set of matching keys.
		 *  @param isRegex A boolean value indicating whether or not the pattern represents a regular expression.
		 *  @return Void.
		 */
		void GetKeysForPattern ( string keyStr, string regexStr, StringSet *keys, bool isRegex );

		/** Get the set of all ids on the system that match the object.
		 *  @param keyStr A string that contains the key to be used during the matching process.
		 *  @param idEntity A ObjectEntity that represents the id entity in an Object as defined in the OVAL Definition Schema.
		 *  @return A StringSet that contains all of the ids specified in the ObjectEntity that also match the specified key.
		 */
		StringSet* GetIds ( string keyStr, ObjectEntity* idEntity );

		/** Determine if the specified id exists.
		 *  @param keyStr A string that contains the key of a metabase item.
		 *  @param idStr A string that contains the id of a metabase item.
		 *  @return A boolean value indicating whether or not a metabase item with the specified key and id exists.
		 */
		bool IdExists ( string keyStr, string idStr );

		/** Retrieve all of the ids for a specified key.
		 *  @param keyStr A string that contains the key of a metabase item.
		 *  @return A StringSet that contains all of the ids for the specified key.
		 */
		StringSet* GetAllIds ( string keyStr );

		/** Create a metabase item from specified key and id.
		 *  @param keyStr A string that contains the key of a metabase item.
		 *  @param idStr A string that contains the id of a metabase item.
		 *  @return The Item representing a metabase item whose key and id match the specified values.
		 */
		Item* GetMetabaseItem ( string keyStr, string idStr );

		/** Convert a metabase value datatype to its string representation.
		 *  @param type A unsigned long value that contains the datatype of the value associated with a metabase id.
		 *  @return A string value that represents a metabase value datatype.
		 */
		string GetDataType ( unsigned long type );

		/** Convert a metabase value usertype to its string representation.
		 *  @param type A unsigned long value that contains the usertype of the value associated with a metabase id.
		 *  @return A string value that represents a metabase value usertype.
		 */
		string GetUserType ( unsigned long type );

		/** Retrieves the value of the metabase item with the specified key and id.
		 *  @param keyStr A string that contains the naming context of the Active Directory item.
		 *  @param type A unsigned long value that contains the datatype of the value associated with a metabase id.
		 *  @param length A unsigned long value that contains the length of the value associated with a metabase id.
		 *  @return A StringVector that contains the values of the metabase item with the specified key and id.
		 */
		StringVector* GetDataValues ( unsigned char* data, unsigned long type, unsigned long length );

		/** Determine if there are any keys that do not exist that need to be reported.
		 *  @param keyEntity A ObjectEntity that represents the key entity in an Object as defined in the OVAL Definition Schema.
		 *  @param keysDne A StringVector that will be used to contain all of the keys specified in the ObjectEntity that do not exist on the system.
		 *  @return A boolean value that indicates whether or not there are keys that do not exist that need to be reported.
		 */
		bool ReportKeyDoesNotExist ( ObjectEntity* keyEntity, StringVector* keysDne );

		/** Determine if there are any ids that do not exist for the specified key that need to be reported.
		 *  @param keyStr A string that contains the key of the metabase item.
		 *  @param idEntity A ObjectEntity that represents the id entity in an Object as defined in the OVAL Definition Schema.
		 *  @param idsDne A StringVector that will be used to contain all of the relative distinguished names specified in the ObjectEntity that do not exist on the system.
		 *  @return A boolean value that indicates whether or not there are relative distinguished names that do not exist that need to be reported.
		 */
		bool ReportIdDoesNotExist ( string keyStr, ObjectEntity *idEntity, StringVector* idsDne );

		/** The static Singleton instance of the MetabaseProbe. */
		static MetabaseProbe* instance;

		/** Represents the '/' character which is used to separate different levels of metabase keys. */
		static const char MetabaseSeparator;

		/** A string that represents the '/' character which is used to separate different levels of metabase keys. */
		static const string MetabaseSeparatorStr;

		/** IMSAdminBase interface that is used to query the metabase. */
		IMSAdminBase* metabase;
};

#endif
