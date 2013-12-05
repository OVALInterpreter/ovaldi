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

#ifndef OBJECTREADER_H
#define OBJECTREADER_H

#include "Common.h"
#include "DocumentManager.h"
#include "Item.h"
#include "VariableValue.h"

XERCES_CPP_NAMESPACE_USE

/**
	This class reads collected objects in a system characteristics files.
	Two static methods are provided that will fetch the set of items for a collected
	object or the set of variable values used when collecting an object in a oval
	system characterisitcs file.
*/
class ObjectReader {
public:
	
	/** Return the flag associated with the collected object. 
		Locate teh collected object in the System characteristics document and
		return the flag attribute's value.
	*/
	static OvalEnum::Flag GetCollectedObjectFlag(std::string objectId);

	/** Return the set of items for the specified id. */
	static ItemVector* GetItemsForObject(std::string objectId);

	/** Return the set of variable values used to collect the specified object. */
	static VariableValueVector* GetVariableValuesForObject(std::string objectId);
	static StringVector* GetMessagesForObject(std::string objectId);
};

#endif
