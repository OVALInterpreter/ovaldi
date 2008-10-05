//
//
//****************************************************************************************//
// Copyright (c) 2002-2008, The MITRE Corporation
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

#ifndef WUAUPDATESEARCHERPROBE_H
#define WUAUPDATESEARCHERPROBE_H

#pragma warning(disable:4786)

#include "AbsProbe.h"
#include "WindowsCommon.h"

/***********************************************************************/
// The comdef.h include statment is needed for the code in this probe to
// work. However, comdef.h includes comdefsp.h which utilizes a DOMDocument.
// This application uses Xerces for xml parsing. Xerces defines a DOMDocument
// This application also uses windows apis which include references to MsXml.h
// MsXml.h defines a DOMDocument as well. Either Microsoft or Xerces is not 
// correctly qualifying thier classes. this causes the following error message:
/*
c:\Program Files\Microsoft Visual Studio .NET 2003\Vc7\include\comdefsp.h(1305) : error C2872: 'DOMDocument' : ambiguous symbol
        could be 'c:\Program Files\Microsoft Visual Studio .NET 2003\Vc7\PlatformSDK\Include\MsXml.h(161) : DOMDocument'
        or       'c:\xerces-c_2_6_0-windows_nt-msvc_60\include\xercesc\dom\DOMDocument.hpp(63) : xercesc_2_6::DOMDocument'
*/
// One solution would be to edit comdefsp.h by simply commenting out line
// 1305 allowing you to compile and run the aplication.  The line to
// comment looks like:
//   _COM_SMARTPTR_TYPEDEF(DOMDocument, __uuidof(DOMDocument));
//
// Jean-Fran�ois Dub� pointed out that a better solution is to add a
// #undef to undefine DOMDocument.  Thanks!
//
/***********************************************************************/

#undef __DOMDocument_FWD_DEFINED__
#include <comdef.h>
#include <Dsgetdc.h>
#include <Lm.h>
#include <windows.h>
#include <Wuapi.h>

using namespace std;

/** 
    This class is responsible for collecting information for windows wuaupdatesearcher_object.

    The wuaupdatesearcher_object element is used by a wuaupdatesearcher_test to 
    define the specific search criteria to be evaluated.

    NOTICE: This interpreter only searches the default update service for the
    system. This is likely update.micorsoft.com.
*/
class WUAUpdateSearcherProbe : public AbsProbe {

public:
	~WUAUpdateSearcherProbe();

	/** Run the probe. */
	ItemVector* CollectItems(Object* object);

	/**  Return a new Item created for storing wuaupdatesearcher information */
	Item* CreateItem();

	/** Ensure that the WUAUpdateSearcherProbe is a singleton. */
	static AbsProbe* Instance();

    /** Delete the Singlton instance of the probe. */
    static void DeleteInstance();

private:
	WUAUpdateSearcherProbe();

	static WUAUpdateSearcherProbe* instance;

    /** Get the set of all search criteria string for the specified ObjectEntity. */
    ItemEntityVector* GetSearchCriteria(ObjectEntity* search_criteria);

    /** 
        Do the search and return an Item with the results. 

        Sets the update_id entity on the returned item. The update_id specifies
        a string that represents a revision-independent identifier of an update.  
        This information is part of the IUpdateIdentity interface that is part 
        of the result of the IUpdateSearcher interface's Search method.  Note 
        that multiple update identifiers can be associated with a give search 
        criteria and thus multiple entities can exist for this item.
    */
    Item* DoWUASearch(ItemEntity* search_criteria);

    /**
        Initializes the WUA Searcher. 
        Creates a COM connection and gets an update searcher.
    */
    void InitWUASearcher();

    /**
        Shutdown the update searcher and clean up all the allocated resources. 
        Closes COM interface.
    */
    void ShutdownWUASearcher();


    // Variables
    IUpdateSession* pIUpdateSession;
	IUpdateServiceManager* pIUpdateServiceManager;	
	IUpdateSearcher* pIUSearcher;

    IUpdateCollection* pIUpdateCollection;
	ISearchResult* pISearchResult;
};

#endif