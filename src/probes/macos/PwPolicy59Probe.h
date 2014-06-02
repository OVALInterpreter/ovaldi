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
#ifndef PWPOLICY59PROBE_H
#define PWPOLICY59PROBE_H

#include <map>
#include <utility>
#include <vector>

#include "AbsProbe.h"
#include <Item.h>
#include <Object.h>

/**
	This class is responsible for collecting variable information from the pwpolicy command.
	This class should be platform independant.
*/
class PwPolicy59Probe : public AbsProbe {
public:
	virtual ~PwPolicy59Probe();
	
	/** Run the PwPolicy59Probe probe */
	virtual ItemVector* CollectItems(Object* object);

	/** Ensure that the PwPolicy59Probe is a singleton. */
	static AbsProbe* Instance();
	
private:

	typedef std::map<std::string, std::string> PwPolicyMap;

	static PwPolicy59Probe *instance;

	PwPolicy59Probe();

	/** Return a new Item created for storing variable information */
	virtual Item* CreateItem();

	/** Return a new filled Item created for storing variable information */
	Item* FillItem(const PwPolicyMap &record);

	PwPolicyMap GetRecord(const std::string &targetUser);
	
	ObjectEntity* ValidateStringOperations(ObjectEntity* stringOp);

	StringVector names;
	std::vector<std::pair<std::string, OvalEnum::Datatype> > propTypes;
};

#endif
