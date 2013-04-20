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

#include <Windows.h>
#include <windns.h>

#include "WindowsCommon.h"

#include "DNSCacheProbe.h"

using namespace std;

typedef struct _DnsCacheEntry{
    struct _DnsCacheEntry* pNext;  // Pointer to next entry
    PWSTR pszName;                 // DNS Record Name
    unsigned short wType;          // DNS Record Type
    unsigned short wDataLength;    // Not referenced
    unsigned long  dwFlags;        // DNS Record Flags
}DNSCACHEENTRY;

typedef BOOL (WINAPI DnsGetCacheDataTableAddress)(DNSCACHEENTRY** ppCacheEntry);


//****************************************************************************************//
//								DNSCacheProbe Class											  //	
//****************************************************************************************//
DNSCacheProbe* DNSCacheProbe::instance = NULL;

DNSCacheProbe::DNSCacheProbe() : AbsProbe() {
}

DNSCacheProbe::~DNSCacheProbe() {
  instance = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* DNSCacheProbe::Instance() {
	if(instance == NULL) 
		instance = new DNSCacheProbe();

	return instance;	
}

ItemVector* DNSCacheProbe::CollectItems(Object *object) {

	ObjectEntity* domainName = object->GetElementByName("domain_name");
	ItemVector *collectedItems = new ItemVector();

	if ( domainName->GetOperation() == OvalEnum::OPERATION_EQUALS ){
		StringVector dnsCaches;
		//We are ignoring the flag for now
		/*OvalEnum::Flag flag =*/domainName->GetEntityValues(dnsCaches);
		for(StringVector::iterator it = dnsCaches.begin(); it != dnsCaches.end(); it++){
			collectedItems->push_back(this->GetDnsCacheItem(*it));
		}
	}else{
		StringSet* domainNames = this->GetAllCachedDomainNames();
		ItemEntity* tmpDomainName = new ItemEntity("domain_name","",OvalEnum::DATATYPE_STRING,OvalEnum::STATUS_EXISTS);
		for(StringSet::iterator it = domainNames->begin(); it != domainNames->end(); it++){
			tmpDomainName->SetValue(*it);
			if ( domainName->Analyze(tmpDomainName) == OvalEnum::RESULT_TRUE ){
				collectedItems->push_back(this->GetDnsCacheItem(*it));
			}
		}
		delete domainNames;
		delete tmpDomainName;
	}

	return collectedItems;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
Item* DNSCacheProbe::CreateItem() {

	Item* item = new Item(0, 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows", 
						"win-sc", 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows windows-system-characteristics-schema.xsd", 
						OvalEnum::STATUS_ERROR, 
						"dnscache_item");

	return item;
}

StringSet* DNSCacheProbe::GetAllCachedDomainNames(){
	StringSet* domainNames = new StringSet();
	//Load dnsapi.dll
	HMODULE hModule = LoadLibrary("dnsapi.dll");
	if ( hModule == NULL ){
		delete domainNames;
		throw ProbeException("Error: Failed to load dnsapi.dll.");
	}
	DNSCACHEENTRY *cacheEntries = NULL;
	DnsGetCacheDataTableAddress *DnsGetCacheDataTable = (DnsGetCacheDataTableAddress*)GetProcAddress(hModule, "DnsGetCacheDataTable");
	if (!DnsGetCacheDataTable || TRUE != DnsGetCacheDataTable(&cacheEntries)){
		throw ProbeException("Error: Failed to retrieve the address of the DnsGetCacheDataTable function.");
	}
	
	if (cacheEntries!= NULL){
		DNSCACHEENTRY *currentCacheEntry = cacheEntries;
		while (currentCacheEntry != NULL){
			
			//We are only interested in DNS_TYPE_A records
			if ( currentCacheEntry->wType == DNS_TYPE_A ){
				domainNames->insert(WindowsCommon::UnicodeToAsciiString((wchar_t*)currentCacheEntry->pszName));
			}
			DNSCACHEENTRY *previousCacheEntry = currentCacheEntry;
			currentCacheEntry = currentCacheEntry->pNext;

			//Free allocated memory
			DnsFree(previousCacheEntry->pszName, DnsFreeFlat);
			DnsFree(previousCacheEntry, DnsFreeFlat);
		}
	}
	//Free resources associated with loading dnsapi.dll.
	FreeLibrary(hModule);
	return domainNames;
}

Item* DNSCacheProbe::GetDnsCacheItem(string domainName){
	DNS_RECORD* dnsRecords = NULL;
	DNS_STATUS dnsStatus = DNS_ERROR_RECORD_DOES_NOT_EXIST;
	Item* item = this->CreateItem();
	//We are only interested in DNS_TYPE_A records
	if ((dnsStatus = DnsQuery(domainName.c_str(), DNS_TYPE_A, DNS_QUERY_CACHE_ONLY, NULL, &dnsRecords, NULL)) == ERROR_SUCCESS ){
		item->SetStatus(OvalEnum::STATUS_EXISTS);
		item->AppendElement(new ItemEntity("domain_name",domainName,OvalEnum::DATATYPE_STRING,OvalEnum::STATUS_EXISTS));
		item->AppendElement(new ItemEntity("ttl",Common::ToString(dnsRecords->dwTtl),OvalEnum::DATATYPE_INTEGER,OvalEnum::STATUS_EXISTS));
		if ( dnsRecords != NULL ){
			//Retrieve every IP address associated with this domain name. 
			while ( dnsRecords != NULL ){
				IN_ADDR ipAddress;
				ipAddress.S_un.S_addr = dnsRecords->Data.A.IpAddress;
				char* ipAddressStr = inet_ntoa(ipAddress);
				if ( ipAddressStr != NULL ){
					item->AppendElement(new ItemEntity("ip_address",ipAddressStr,OvalEnum::DATATYPE_STRING,OvalEnum::STATUS_EXISTS));	
				}else{
					//Free allocated memory
					delete item;
					DnsRecordListFree(dnsRecords,DnsFreeRecordList);
					throw ProbeException("Error: There was an error while converting the IP address to its string representation with inet_ntoa().");
				}
				dnsRecords = dnsRecords->pNext;
			}
		}else{
			item->AppendElement(new ItemEntity("ip_address","",OvalEnum::DATATYPE_STRING,OvalEnum::STATUS_DOES_NOT_EXIST));	
		}
	}else{
		if ( dnsStatus == DNS_ERROR_RECORD_DOES_NOT_EXIST ){
			//If the domain name cannot be found report that it does not exist.
			item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);	
			item->AppendElement(new ItemEntity("domain_name","",OvalEnum::DATATYPE_STRING,OvalEnum::STATUS_DOES_NOT_EXIST));
		}else{
			//Otherwise an error has occurred report error for entity and item.
			item->AppendElement(new ItemEntity("domain_name","",OvalEnum::DATATYPE_STRING,OvalEnum::STATUS_ERROR));
			item->AppendMessage(new OvalMessage("ERROR: There was an error while retrieving the domain_name: '"+domainName+"'.  Microsoft System Error (" + Common::ToString ( dnsStatus ) + ") - " + WindowsCommon::GetErrorMessage (GetLastError())));
		}
	}
	//Free allocated memory
	DnsRecordListFree(dnsRecords,DnsFreeRecordList);
	return item;
}