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

#ifndef WMI57PROBE_H
#define WMI57PROBE_H

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
// Jean-François Dubé pointed out that a better solution is to add a
// #undef to undefine DOMDocument.  Thanks!
//
/***********************************************************************/

#undef __DOMDocument_FWD_DEFINED__
#include <comdef.h>
#include <Dsgetdc.h>
#include <Lm.h>
#include <Wbemidl.h>
#include <Wmiutils.h>
#include <windows.h>
#include "AbsEntityValue.h"
using namespace std;

/**
	This class is responsible for collecting information from WMI.
*/
class WMI57Probe : public AbsProbe {

public:
	virtual ~WMI57Probe();

	/** Run the probe.
	    Get the namespace and wql query to run.  Note that niether entity
	    allows a pattern match so you don't have to worry about resolving
	    regular expressions.
	*/
	virtual ItemVector* CollectItems(Object* object);

	/** Ensure that the WMI57Probe is a singleton. */
	static AbsProbe* Instance();

private:
	/** WMI57Probe constructor */
	WMI57Probe();
	
	/** The singleton instsance of the WMI57Probe */
	static WMI57Probe* instance;

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
	enum WQLFieldType {
		SELECT, //Get values listed after the SELECT statement.
		FROM,   //Get values listed after the FROM statement.
		WHERE   //Get values listed after the WHERE statement.
	};

	/** Retrieve the parameters from the SELECT, FROM, or WHERE part of the wql.
		@param wqlIn the input wql.
		@param wqlFieldType the part of the wql from which to retrieve the parameters.
		@return A StringVector* containing the list of wql parameters that were retreived.
	*/
	StringVector* GetWqlFields(string wqlIn, WQLFieldType wqlFieldType);
};

#endif
