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

#ifndef PRINTEREFFECTIVRIGHTSPROBE_H
#define PRINTEREFFECTIVRIGHTSPROBE_H

#pragma warning(disable:4786)

#include <Windows.h> // for HANDLE
#include "AbsEffectiveRightsProbe.h"
#include <string>


/**
	This class is responsible for collecting printer information for Windows printereffictiverights_objects.
*/
class PrinterEffectiveRightsProbe : public AbsEffectiveRightsProbe {

	public:
	
		/** PrinterEffectiveRightsProbe destructor */
		virtual ~PrinterEffectiveRightsProbe();
		
		virtual ItemVector* CollectItems(Object* object);

		/** Ensure that the PrinterEffectiveRightsProbe is a singleton. */
		static AbsProbe* Instance();

	private:
	
		/** PrinterEffectiveRightsProbe constructor */
		PrinterEffectiveRightsProbe();

		/** Return a new Item created for storing printer information */
		virtual Item* CreateItem();

		/** Get the set of all printers on the system that match the object.
   		 *  @param printerNameEntity A ObjectEntity that represents the printer_name entity in an Object as defined in the OVAL Definition Schema.
		 *  @return A StringSet that contains all of the printers specified in the ObjectEntity.
		 */
		StringSet* GetPrinters ( ObjectEntity* printerNameEntity );

		/** Retrieve all of the matching printers.
		 *  @param patternStr A string that contains the pattern to be matched.
		 *  @param isRegex A boolean value that specifies whether or not the pattern is a regular expression.
		 *  @return A StringSet containing all of the matching printers.
		 */
		StringSet* GetMatchingPrinters ( std::string patternStr , bool isRegex );

		/** Determine if the printer exists on the system.
		 *  @param printerNameStr A string that contains the name of the printer whose existenc you want to check.
		 *  @return A boolean value that specifies whether or not the printer exists on the system.
		 */
		bool PrinterExists ( std::string printerNameStr );

		/** Retrieve all of the printers on the system.
		 *  @return A pointer to a StringSet that contains all of the printers on the system.
		 */
		StringSet* GetAllPrinters();

		/** Get the effective rights for a trustee SID for the specified printer.
		 *  @param printerNameStr A string that contains the name of the printer that you want to get the effective rights of.
		 *  @param trusteeSIDStr A string that contains the trusteeSID of the printer that you want to get the effective rights of.
		 *  @return The item that contains the printer effective rights of the specified printer and trustee SID.
		 */
		Item* GetEffectiveRights(HANDLE printerHandle, 
				const std::string &printerNameStr, string trusteeSIDStr);
		
		HANDLE GetHandleToPrinter(const std::string &printerName);

		/** The static instance of the PrinterEffectiveRightsProbe.
		 *  All Probes are singletons. The ProbeFactory is responsible for managing instances of Probes.
		 */
		static PrinterEffectiveRightsProbe* instance;

		/** The StringSet that holds the information about all of the Windows printers on the local system. */	
		StringSet* printers;
};

#endif
