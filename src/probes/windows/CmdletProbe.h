//
//
//****************************************************************************************//
// Copyright (c) 2002-2011, The MITRE Corporation
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
#ifndef CMDLETPROBE_H
#define CMDLETPROBE_H

#pragma warning(disable:4786)

#include "AbsProbe.h"

using namespace std;

using namespace System::Management::Automation::Runspaces;

typedef std::pair < std::string, StringVector* > StringStringVectorPair;
typedef std::vector < StringStringVectorPair* > SSVPVector;
typedef std::vector < StringVector* > StringVectorVector;

/**
	This class is responsible for collecting information for windows cmdlet_objects.
*/

class CmdletProbe : public AbsProbe {
public:
	virtual ~CmdletProbe();
	
	/** Run the cmdlet probe. 
	    Return a vector of Items
     */
	virtual ItemVector* CollectItems(Object* object);

	/** Ensure that the CmdletProbe is a singleton. */
	static AbsProbe* Instance();

private:
	CmdletProbe();

	static CmdletProbe* instance;

	/** Return a new Item created for storing information retrieved by executing a cmdlet. */
	virtual Item* CreateItem();

	void ExecutePowerShell(string powershellInput, Item* item);	
	InitialSessionState^ InitializeRunspace();
	OvalEnum::Datatype GetDatatype(std::string type);

	std::string cmdletItem;

};

#endif
