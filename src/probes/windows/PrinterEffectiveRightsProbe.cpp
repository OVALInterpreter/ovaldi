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

#include <AutoCloser.h>
#include <WindowsCommon.h>

#include "PrinterEffectiveRightsProbe.h"

using namespace std;

//****************************************************************************************//
//                              PrinterEffectiveRightsProbe Class                         //
//****************************************************************************************//
PrinterEffectiveRightsProbe* PrinterEffectiveRightsProbe::instance = NULL;

PrinterEffectiveRightsProbe::PrinterEffectiveRightsProbe() {
	printers = NULL;
}

PrinterEffectiveRightsProbe::~PrinterEffectiveRightsProbe() {
    instance = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* PrinterEffectiveRightsProbe::Instance() {
    // Use lazy initialization
    if ( instance == NULL )
        instance = new PrinterEffectiveRightsProbe();

    return instance;
}

ItemVector* PrinterEffectiveRightsProbe::CollectItems ( Object* object ) {

	ObjectEntity* printerNameEntity = object->GetElementByName ( "printer_name" );
    ObjectEntity* trusteeSIDEntity = object->GetElementByName ( "trustee_sid" );

    // Check printer name datatypes - only allow string
    if ( printerNameEntity->GetDatatype() != OvalEnum::DATATYPE_STRING ) {
        throw ProbeException ( "Error: invalid data type specified on printer_name. Found: " + OvalEnum::DatatypeToString ( printerNameEntity->GetDatatype() ) );
    }

    // Check printer name operation - only allow  equals, not equals and pattern match
    if ( printerNameEntity->GetOperation() != OvalEnum::OPERATION_EQUALS
            && printerNameEntity->GetOperation() != OvalEnum::OPERATION_PATTERN_MATCH
            && printerNameEntity->GetOperation() != OvalEnum::OPERATION_NOT_EQUAL ) {
        throw ProbeException ( "Error: invalid operation specified on printer_name. Found: " + OvalEnum::OperationToString ( printerNameEntity->GetOperation() ) );
    }

    // Check trustee datatypes - only allow string
    if ( trusteeSIDEntity->GetDatatype() != OvalEnum::DATATYPE_STRING ) {
        throw ProbeException ( "Error: invalid data type specified on trustee_sid. Found: " + OvalEnum::DatatypeToString ( trusteeSIDEntity->GetDatatype() ) );
    }

    // Check trustee operation - only allow  equals, not equals and pattern match
    if ( trusteeSIDEntity->GetOperation() != OvalEnum::OPERATION_EQUALS
            && trusteeSIDEntity->GetOperation() != OvalEnum::OPERATION_PATTERN_MATCH
            && trusteeSIDEntity->GetOperation() != OvalEnum::OPERATION_NOT_EQUAL ) {
        throw ProbeException ( "Error: invalid operation specified on trustee_sid. Found: " + OvalEnum::OperationToString ( trusteeSIDEntity->GetOperation() ) );
    }
    
	// Support behaviors - init with defaults.
	bool includeGroupBehavior = true;
	bool resolveGroupBehavior = false;
	if(object->GetBehaviors()->size() != 0) {
		BehaviorVector* behaviors = object->GetBehaviors();
		BehaviorVector::iterator iterator;
		for(iterator = behaviors->begin(); iterator != behaviors->end(); iterator++) {
			Behavior* behavior = (*iterator);
            if(behavior->GetName().compare("include_group") == 0)  {
                if(behavior->GetValue().compare("false") == 0) {
				    includeGroupBehavior = false;
                }
				Log::Info("Deprecated behavior found when collecting " + object->GetId() + " Found behavior: " + behavior->GetName() + " = " + behavior->GetValue());
            } else if(behavior->GetName().compare("resolve_group") == 0) {
                if(behavior->GetValue().compare("true") == 0) {
				    resolveGroupBehavior = true;
                }
				Log::Info("Deprecated behavior found when collecting " + object->GetId() + " Found behavior: " + behavior->GetName() + " = " + behavior->GetValue());
            } else {
                Log::Info("Unsupported behavior found when collecting " + object->GetId() + " Found behavior: " + behavior->GetName() + " = " + behavior->GetValue());
			}
		}		
	}

	if ( printers == NULL ){
		printers = PrinterEffectiveRightsProbe::GetAllPrinters();
	}

	ItemVector *collectedItems = new ItemVector();
	
	StringSet* allPrinters = PrinterEffectiveRightsProbe::GetPrinters(printerNameEntity);
    
	for ( StringSet::iterator iterator1 = allPrinters->begin(); iterator1 != allPrinters->end(); iterator1++ ) {
	
		try {
			AutoCloser<HANDLE, BOOL(WINAPI&)(HANDLE)> printerHandle(
				GetHandleToPrinter(*iterator1), ClosePrinter, "printer "+*iterator1);
			if (printerHandle.get() == INVALID_HANDLE_VALUE) {
				Log::Message("Couldn't open printer "+*iterator1+": "+WindowsCommon::GetErrorMessage(GetLastError()));
				continue;
			}

			StringSet trusteeSIDs = this->GetTrusteesForWindowsObject( 
				SE_PRINTER, printerHandle.get(), trusteeSIDEntity, 
				true, resolveGroupBehavior, includeGroupBehavior );

            if ( !trusteeSIDs.empty() ) {

				for ( StringSet::iterator iterator2 = trusteeSIDs.begin(); iterator2 != trusteeSIDs.end(); iterator2++ ) {
                    try {
                        Item* item = this->GetEffectiveRights ( printerHandle.get() , *iterator1, ( *iterator2 ) );

                        if ( item != NULL ) {
                            collectedItems->push_back ( item );
                        }

                    } catch ( ProbeException ex ) {
                        Log::Message ( "ProbeException caught when collecting: " + object->GetId() + " " +  ex.GetErrorMessage() );

                    } catch ( Exception ex ) {
                        Log::Message ( "Exception caught when collecting: " + object->GetId() + " " +  ex.GetErrorMessage() );
                    }
                }
            } else {
                Log::Debug ( "No matching SIDs found when getting the effective rights for object: " + object->GetId() );

				if ( this->ReportTrusteeDoesNotExist( trusteeSIDEntity, true ) ) {
                    Item* item = this->CreateItem();
                    item->SetStatus ( OvalEnum::STATUS_DOES_NOT_EXIST );
                    item->AppendElement ( new ItemEntity ( "printer_name", ( *iterator1 ), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
                    item->AppendElement ( new ItemEntity ( "trustee_sid", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST ) );
                    collectedItems->push_back ( item );
                }
            }

        } catch ( ProbeException ex ) {
            Log::Message ( "ProbeException caught when collecting: " + object->GetId() + " " +  ex.GetErrorMessage() );

        } catch ( Exception ex ) {
            Log::Message ( "Exception caught when collecting: " + object->GetId() + " " +  ex.GetErrorMessage() );

        } catch ( ... ) {
            Log::Message ( "Unknown error when collecting " + object->GetId() );
        }
    }

    allPrinters->clear();
    delete allPrinters;

	return collectedItems;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
Item* PrinterEffectiveRightsProbe::CreateItem() {
    Item* item = new Item ( 0,
                            "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows",
                            "win-sc",
                            "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows windows-system-characteristics-schema.xsd",
                            OvalEnum::STATUS_ERROR,
                            "printereffectiverights_item" );
    return item;
}

Item* PrinterEffectiveRightsProbe::GetEffectiveRights ( HANDLE printerHandle,
		const string &printerNameStr, string trusteeSIDStr ) {

    Item* item = NULL;
    PSID pSid = NULL;
    PACCESS_MASK pAccessRights = NULL;
    string baseErrMsg = "Error unable to get effective rights for: " + printerNameStr + " trustee_sid: " + trusteeSIDStr;

    try {
        // Get the sid for the trustee name
        pSid = WindowsCommon::GetSIDForTrusteeSID ( trusteeSIDStr );
        
		// The printer exists and the trustee_sid looks valid so we can create the new item now.
        item = this->CreateItem();
        item->SetStatus ( OvalEnum::STATUS_EXISTS );
		item->AppendMessage(new OvalMessage("The job_access_administer and job_access_read rights are not collected because they are associated with Windows job objects.",OvalEnum::LEVEL_INFO));
        item->AppendElement ( new ItemEntity ( "printer_name", printerNameStr, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "trustee_sid", trusteeSIDStr, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
        
		// Build structure to hold the rights
        pAccessRights = reinterpret_cast<PACCESS_MASK> ( ::LocalAlloc ( LPTR, sizeof ( PACCESS_MASK ) + sizeof ( ACCESS_MASK ) ) );

        if ( pAccessRights == NULL ) {
            throw ProbeException ( baseErrMsg + " Out of memory! Unable to allocate memory for access rights." );
        }
		
        // Get the rights
        Log::Debug ( "Getting rights mask for printer_name: " + printerNameStr + " and trustee_sid: " + trusteeSIDStr );
        WindowsCommon::GetEffectiveRightsForWindowsObject ( SE_PRINTER, pSid, printerHandle, pAccessRights );
		
        if ( ( *pAccessRights ) & DELETE )
            item->AppendElement ( new ItemEntity ( "standard_delete", "1", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        else
            item->AppendElement ( new ItemEntity ( "standard_delete", "0", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        if ( ( *pAccessRights ) & READ_CONTROL )
            item->AppendElement ( new ItemEntity ( "standard_read_control", "1", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        else
            item->AppendElement ( new ItemEntity ( "standard_read_control", "0", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        if ( ( *pAccessRights ) & WRITE_DAC )
            item->AppendElement ( new ItemEntity ( "standard_write_dac", "1", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        else
            item->AppendElement ( new ItemEntity ( "standard_write_dac", "0", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        if ( ( *pAccessRights ) & WRITE_OWNER )
            item->AppendElement ( new ItemEntity ( "standard_write_owner", "1", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        else
            item->AppendElement ( new ItemEntity ( "standard_write_owner", "0", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        if ( ( *pAccessRights ) & SYNCHRONIZE )
            item->AppendElement ( new ItemEntity ( "standard_synchronize", "1", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        else
            item->AppendElement ( new ItemEntity ( "standard_synchronize", "0", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        if ( ( *pAccessRights ) & ACCESS_SYSTEM_SECURITY )
            item->AppendElement ( new ItemEntity ( "access_system_security", "1", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        else
            item->AppendElement ( new ItemEntity ( "access_system_security", "0", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        if ( ( *pAccessRights ) & PRINTER_READ )
            item->AppendElement ( new ItemEntity ( "generic_read", "1", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        else
            item->AppendElement ( new ItemEntity ( "generic_read", "0", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        if ( ( *pAccessRights ) & PRINTER_WRITE )
            item->AppendElement ( new ItemEntity ( "generic_write", "1", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        else
            item->AppendElement ( new ItemEntity ( "generic_write", "0", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        if ( ( *pAccessRights ) & PRINTER_EXECUTE )
            item->AppendElement ( new ItemEntity ( "generic_execute", "1", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        else
            item->AppendElement ( new ItemEntity ( "generic_execute", "0", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        if ( ( *pAccessRights ) & PRINTER_ALL_ACCESS )
            item->AppendElement ( new ItemEntity ( "generic_all", "1", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        else
            item->AppendElement ( new ItemEntity ( "generic_all", "0", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        if ( ( *pAccessRights ) & PRINTER_ACCESS_ADMINISTER )
            item->AppendElement ( new ItemEntity ( "printer_access_administer", "1", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        else
            item->AppendElement ( new ItemEntity ( "printer_access_administer", "0", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        if ( ( *pAccessRights ) & PRINTER_ACCESS_USE    )
            item->AppendElement ( new ItemEntity ( "printer_access_use", "1", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

        else
            item->AppendElement ( new ItemEntity ( "printer_access_use", "0", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

		item->AppendElement ( new ItemEntity ( "job_access_administer", "", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_NOT_COLLECTED ) );
        item->AppendElement ( new ItemEntity ( "job_access_read", "", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_NOT_COLLECTED ) );

    } catch ( Exception ex ) {
        if ( item != NULL ) {
            item->SetStatus ( OvalEnum::STATUS_ERROR );
            item->AppendMessage ( new OvalMessage ( ex.GetErrorMessage(), OvalEnum::LEVEL_ERROR ) );

        } else {
            if ( pAccessRights != NULL ) {
                LocalFree ( pAccessRights );
                pAccessRights = NULL;
            }

            if ( pSid != NULL ) {
                LocalFree ( pSid );
                pSid = NULL;
            }

            throw;
        }
    }

    if ( pAccessRights != NULL ) {
        LocalFree ( pAccessRights );
        pAccessRights = NULL;
    }

    if ( pSid != NULL ) {
        LocalFree ( pSid );
        pSid = NULL;
    }

    return item;
}

StringSet* PrinterEffectiveRightsProbe::GetPrinters ( ObjectEntity* printerNameEntity ) {
    StringSet* printers = NULL;

    // Does this ObjectEntity use variables?
    if ( printerNameEntity->GetVarRef() == NULL ) {
        // Proceed based on operation
        if ( printerNameEntity->GetOperation() == OvalEnum::OPERATION_EQUALS ) {
            printers = new StringSet();

            // If the printer exists add it to the list
            if ( this->PrinterExists ( printerNameEntity->GetValue() ) ) {
                printers->insert ( printerNameEntity->GetValue() );
            }

        } else if ( printerNameEntity->GetOperation() == OvalEnum::OPERATION_NOT_EQUAL ) {
            printers = this->GetMatchingPrinters ( printerNameEntity->GetValue() , false );

        } else if ( printerNameEntity->GetOperation() == OvalEnum::OPERATION_PATTERN_MATCH ) {
            printers = this->GetMatchingPrinters ( printerNameEntity->GetValue() , true );
        }

    } else {
        printers = new StringSet();
        // Get all printers
        StringSet* allPrinters = new StringSet();

        if ( printerNameEntity->GetOperation() == OvalEnum::OPERATION_EQUALS ) {
            // In the case of equals simply loop through all the
            // variable values and add them to the set of all printers
            // if they exist on the system
			VariableValueVector vals = printerNameEntity->GetVarRef()->GetValues();
            VariableValueVector::iterator iterator;

            for ( iterator = vals.begin() ; iterator != vals.end() ; iterator++ ) {
                if ( this->PrinterExists ( iterator->GetValue() ) ) {
                    allPrinters->insert ( iterator->GetValue() );
                }
            }

        } else {
            allPrinters = this->GetMatchingPrinters ( ".*" , true );
        }

        // Loop through all printers on the system
        // only keep printers that match operation and value and var check
        ItemEntity* tmp = this->CreateItemEntity ( printerNameEntity );
        StringSet::iterator it;

        for ( it = allPrinters->begin() ; it != allPrinters->end() ; it++ ) {
            tmp->SetValue ( ( *it ) );

            if ( printerNameEntity->Analyze ( tmp ) == OvalEnum::RESULT_TRUE ) {
				printers->insert ( ( *it ) );
            }
        }
    }

    return printers;
}

StringSet* PrinterEffectiveRightsProbe::GetMatchingPrinters ( string patternStr , bool isRegex ) {
    StringSet* matchingPrinters = new StringSet();
	
    for (StringSet::iterator iterator = PrinterEffectiveRightsProbe::printers->begin() ; iterator != PrinterEffectiveRightsProbe::printers->end() ; iterator++ ) {
        if ( this->IsMatch ( patternStr , *iterator , isRegex ) ) {
			matchingPrinters->insert ( *iterator );
        }
    }

    return matchingPrinters;
}

bool PrinterEffectiveRightsProbe::PrinterExists ( string printerNameStr ) {

	if ( PrinterEffectiveRightsProbe::printers->find(printerNameStr) != PrinterEffectiveRightsProbe::printers->end() ){
		return true;
	}

	return false;
}


StringSet* PrinterEffectiveRightsProbe::GetAllPrinters() {

    StringSet* printers = new StringSet();
    PRINTER_INFO_4* printerDataBuffer = NULL;
    DWORD printerBufferSize = 0;
    DWORD printerDataSize = 0;
    DWORD numberOfPrinters = 0;
    printerDataBuffer = ( PRINTER_INFO_4* ) malloc ( printerBufferSize );

    if ( !EnumPrinters ( PRINTER_ENUM_LOCAL, NULL, 4, ( LPBYTE ) printerDataBuffer, printerBufferSize, &printerDataSize, &numberOfPrinters ) ) {
        if ( printerDataBuffer != NULL ) {
            free ( printerDataBuffer );
            printerDataBuffer = NULL;
        }

        printerDataBuffer = ( PRINTER_INFO_4* ) malloc ( printerDataSize );
        printerBufferSize = printerDataSize;
        printerDataSize = 0;
        numberOfPrinters = 0;

        if ( !EnumPrinters ( PRINTER_ENUM_LOCAL, NULL, 4, ( LPBYTE ) printerDataBuffer, printerBufferSize, &printerDataSize, &numberOfPrinters ) ) {
            throw ProbeException ( "ERROR: The function EnumPrinters() could not enumerate printers. Microsoft System Error (" + Common::ToString ( GetLastError() ) + ") - " + WindowsCommon::GetErrorMessage ( GetLastError() ) );
		}
    }

    for ( unsigned int i = 0; i < numberOfPrinters; i++ ) {
		if ( printerDataBuffer[i].Attributes & PRINTER_ATTRIBUTE_LOCAL ) {
			printers->insert ( printerDataBuffer[i].pPrinterName );
       }
	}

    return printers;
}

HANDLE PrinterEffectiveRightsProbe::GetHandleToPrinter(const string &printerName) {
	HANDLE printerHandle = INVALID_HANDLE_VALUE;
	// I'm not sure what access I should request...
	PRINTER_DEFAULTS defs = {NULL, NULL, PRINTER_ALL_ACCESS};

	//lame-o API only takes non-const char strings?!?!?
	BOOL res = OpenPrinter(const_cast<char*>(printerName.c_str()), &printerHandle, &defs);
	if (!res) printerHandle = INVALID_HANDLE_VALUE; // cause I'm paranoid.....
	return printerHandle;
}
