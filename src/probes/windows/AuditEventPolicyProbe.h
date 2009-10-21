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

#ifndef AUDITEVENTPOLICYPROBE_H
#define AUDITEVENTPOLICYPROBE_H

#pragma warning(disable:4786)

#include "AbsProbe.h"
#include "WindowsCommon.h"

#include <Ntsecapi.h>
#include <windows.h>

using namespace std;

class AuditEventPolicyProbe : public AbsProbe {
public:
	virtual ~AuditEventPolicyProbe();
	
	/** Run the AuditEventPolicyProbe */
	virtual ItemVector* CollectItems(Object* object);

	/** Ensure that the AuditEventPolicyProbe is a singleton. */
	static AbsProbe* Instance();

private:
	AuditEventPolicyProbe();

    /** Return a new Item created for storing audit event policy information. */
	virtual Item* CreateItem();

	/** Read the Audit options and set the value of the ItemEntity.
		LSA Policy defines a mask for the valid event auditing options. 
		The POLICY_AUDIT_EVENT_MASK mask evaluates to TRUE if it is set 
		equal to any of the preceding event auditing options.
	*/
	void ReadAuditOptions(Item* item, ItemEntity* itemElm, ULONG auditPolicy);

	static AuditEventPolicyProbe *instance;
};

#endif
