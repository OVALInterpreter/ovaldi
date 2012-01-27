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
#ifndef DNSCACHEPROBE_H
#define DNSCACHEPROBE_H

#pragma warning(disable:4786)

#include "AbsProbe.h"
#include "WindowsCommon.h"
#include <windns.h>

typedef struct _DnsCacheEntry{
    struct _DnsCacheEntry* pNext;  // Pointer to next entry
    PWSTR pszName;                 // DNS Record Name
    unsigned short wType;          // DNS Record Type
    unsigned short wDataLength;    // Not referenced
    unsigned long  dwFlags;        // DNS Record Flags
}DNSCACHEENTRY;

typedef BOOL (WINAPI DnsGetCacheDataTableAddress)(DNSCACHEENTRY** ppCacheEntry);

/**
	This class is responsible for collecting information for windows dnscache_objects.
*/
class DNSCacheProbe : public AbsProbe {
public:
	/** DNSCacheProbe destructor */
	virtual ~DNSCacheProbe();
	
	/** Run the DNSCache probe. Return a vector of Items
		Relies on the WindowsCommon class for searching for and enumerating
		DNSCaches. 
	 */
	virtual ItemVector* CollectItems(Object* object);

	/** Ensure that the DNSCacheProbe is a singleton. */
	static AbsProbe* Instance();

private:
	/** DNSCacheProbe constructor */
	DNSCacheProbe();

	/** The static instance of the DNSCacheProbe.
	*  All Probes are singletons. The ProbeFactory is responsible for managing instances of Probes.
	*/
	static DNSCacheProbe* instance;

	/** Return a new Item created for storing DNSCache information */
	virtual Item* CreateItem();

	/** Retrieve the domain names of all of the entries in the DNS cache that are of type DNS_TYPE_A.
		@return A StringSet containing the domain names of all of the entiries in the DNS cache that are of type DNS_TYPE_A. 
	*/
	StringSet* GetAllCachedDomainNames();

	/** Retrieve the IP addresses and TTL for the specified domain name and create a Item with them.
		@return A Item for the specified domain name. 
	*/
	Item* GetDnsCacheItem(string domainName);
};

#endif
