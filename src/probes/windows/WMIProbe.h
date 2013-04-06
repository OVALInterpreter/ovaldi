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

#ifndef WMIPROBE_H
#define WMIPROBE_H

#include <string>

#include "AbsProbe.h"

/**
	This class is responsible for collecting information from WMI.
*/
class WMIProbe : public AbsProbe {

public:
	virtual ~WMIProbe();

	/** Run the probe.
	    Get the namespace and wql query to run.  Note that niether entity
	    allows a pattern match so you don't have to worry about resolving
	    regular expressions.
	*/
	virtual ItemVector* CollectItems(Object* object);

	/** Ensure that the WMIProbe is a singleton. */
	static AbsProbe* Instance();

private:
	WMIProbe();

	static WMIProbe* instance;

    /**  Return a new Item created for storing wmi information */
	virtual Item* CreateItem();

	/** Query WMI for data. */
	Item* GetWMI(ItemEntity*, ItemEntity*);

	/** Return the set of namespaces to be collected */
	ItemEntityVector* GetNamespaces(ObjectEntity*);

	/** Return the set of wql queries to be run */
	ItemEntityVector* GetWQLs(ObjectEntity*);

	/** Parse the WQL and extract the field in the select statement.  
	    If we get the field, return true and fieldName is set to the
		name of the field.  If we cannot get the field, return false
		and fieldName is set to an error message.
	*/
	bool GetWqlField(std::string wqlIn, std::string* fieldName);
};

#endif
