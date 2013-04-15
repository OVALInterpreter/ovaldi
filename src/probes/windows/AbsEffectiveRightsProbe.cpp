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
#include "AbsEffectiveRightsProbe.h"

//****************************************************************************************//
//                              AbsEffectiveRightsProbe Class                         //
//****************************************************************************************//

AbsEffectiveRightsProbe::AbsEffectiveRightsProbe() {
}

AbsEffectiveRightsProbe::~AbsEffectiveRightsProbe() {
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

StringSet AbsEffectiveRightsProbe::GetTrusteesForWindowsObject ( SE_OBJECT_TYPE objectType, HANDLE objectHandle, ObjectEntity* trusteeEntity, bool isSID,  bool resolveGroupBehavior, bool includeGroupBehavior ) {
    PACL pdacl = NULL;
    PSID owner;
    PSID primaryGroup;
    PSECURITY_DESCRIPTOR pSD = NULL;
    StringSet allTrustees;
    StringSet workingTrustees;
    StringSet resultingTrustees;

    try {
        // Start by getting the sids from the dacl, owner, and primary group off the object's security descriptor
        string baseErrMsg = "Error: unable to get trustees for " + WindowsCommon::GetObjectType ( objectType ) + ".";
        DWORD res = GetSecurityInfo ( objectHandle,                                // Object name
                                      objectType,                                  // Object type
                                      DACL_SECURITY_INFORMATION |                  // Information type
                                      GROUP_SECURITY_INFORMATION |
                                      OWNER_SECURITY_INFORMATION,
                                      &owner,                                      // Owner
                                      &primaryGroup,                               // Primary group
                                      &pdacl,                                      // DACL
                                      NULL,                                        // SACL
                                      &pSD );                                      // Security Descriptor
        if ( res != ERROR_SUCCESS ) {
            throw Exception ( baseErrMsg + " Unable to retrieve a copy of the security descriptor. Microsoft System Error (" + Common::ToString ( res ) + ") - " + WindowsCommon::GetErrorMessage ( res ) );
        }

        // Get sids from the dacl and insert the owner and primary group sids
		if ( isSID ){
			WindowsCommon::GetSidsFromPACL ( pdacl, &allTrustees );
			string sidStr;
			if (!WindowsCommon::GetTextualSid(owner, &sidStr))
				throw Exception("GetTrusteesForWindowsObject: Couldn't convert SID to string: "+
					WindowsCommon::GetErrorMessage(GetLastError()));
			allTrustees.insert(sidStr);
			if (!WindowsCommon::GetTextualSid(primaryGroup, &sidStr))
				throw Exception("GetTrusteesForWindowsObject: Couldn't convert SID to string: "+
					WindowsCommon::GetErrorMessage(GetLastError()));
			allTrustees.insert(sidStr);
		}else{
			WindowsCommon::GetTrusteeNamesFromPACL(pdacl, &allTrustees);
			allTrustees.insert(WindowsCommon::GetFormattedTrusteeName(owner));
			allTrustees.insert(WindowsCommon::GetFormattedTrusteeName(primaryGroup));
		}
		
        Log::Debug ( "Found " + Common::ToString ( allTrustees.size() ) + " trustee SIDs when searching for all SIDs on the security descriptor" );

        // Does this trusteeSIDEntity use variables?
        if ( trusteeEntity->GetVarRef() == NULL ) {
            // Proceed based on operation
            if ( trusteeEntity->GetOperation() == OvalEnum::OPERATION_EQUALS ) {
                // Check that the trustee SID exists
                if ( ( isSID && WindowsCommon::TrusteeSIDExists ( trusteeEntity->GetValue() )) || (!isSID && WindowsCommon::TrusteeNameExists ( trusteeEntity->GetValue() )) ) {
                    workingTrustees.insert ( trusteeEntity->GetValue() );
                }

            } else if ( trusteeEntity->GetOperation() == OvalEnum::OPERATION_NOT_EQUAL ) {
                this->GetMatchingTrustees ( trusteeEntity->GetValue(), &allTrustees, &workingTrustees, false );
            } else if ( trusteeEntity->GetOperation() == OvalEnum::OPERATION_PATTERN_MATCH ) {
                this->GetMatchingTrustees ( trusteeEntity->GetValue(), &allTrustees, &workingTrustees, true );
			}

        } else {
            if ( trusteeEntity->GetOperation() == OvalEnum::OPERATION_EQUALS ) {
                // In the case of equals simply loop through all the variable values and add them to the set of all sids if they exist on the system
                for ( VariableValueVector::iterator iterator = trusteeEntity->GetVarRef()->GetValues()->begin(); iterator != trusteeEntity->GetVarRef()->GetValues()->end(); iterator++ ) {
					if ( ( isSID && WindowsCommon::TrusteeSIDExists ( ( *iterator )->GetValue() ) ) || ( !isSID && WindowsCommon::TrusteeNameExists ( ( *iterator )->GetValue() ) ) ) {
                        allTrustees.insert ( ( *iterator )->GetValue() );
                    }
                }
            }

            // Loop through all of the trustee SIDs on the Security Descriptor and only keep those that match operation, value, and var check
            ItemEntity* tmp = this->CreateItemEntity ( trusteeEntity );

            for ( StringSet::iterator it = allTrustees.begin(); it != allTrustees.end(); it++ ) {
                tmp->SetValue ( ( *it ) );

                if ( trusteeEntity->Analyze ( tmp ) == OvalEnum::RESULT_TRUE ) {
                    workingTrustees.insert ( ( *it ) );
                }
            }
        }

        // Apply the behaviors
        for ( StringSet::iterator it = workingTrustees.begin(); it != workingTrustees.end(); it++ ) {
            // Is this a group
            bool isGroup;

			if ( isSID ) isGroup = WindowsCommon::IsGroupSID ( ( *it ) );
			else isGroup = WindowsCommon::IsGroup ( ( *it ) );

            if ( isGroup && resolveGroupBehavior ) {
                if ( includeGroupBehavior ) {
                    resultingTrustees.insert ( ( *it ) );
                }

                // Get the group members and add them to the set
                StringSet groupMembers;
                if ( isSID ) WindowsCommon::ExpandGroupBySID ( ( *it ), &groupMembers, includeGroupBehavior, resolveGroupBehavior );
				else  WindowsCommon::ExpandGroup ( ( *it ), &groupMembers, includeGroupBehavior, resolveGroupBehavior );
                for ( StringSet::iterator iterator = groupMembers.begin(); iterator != groupMembers.end(); iterator++ ) {
                    resultingTrustees.insert ( ( *iterator ) );
                }

            } else {
                resultingTrustees.insert ( ( *it ) );
            }
        }

	} catch ( ... ) {
        if ( pSD != NULL ) {
            LocalFree ( pSD );
        }

        throw;
	}

    if ( pSD != NULL ) {
        LocalFree ( pSD );
    }

    Log::Debug ( "Found " + Common::ToString ( resultingTrustees.size() ) + " matching SIDs after applying behaviors on the security descriptor" );
    return resultingTrustees;
}

void AbsEffectiveRightsProbe::GetMatchingTrustees ( string trusteePatternStr, StringSet* allTrustees, StringSet* trustees, bool isRegex ) {
    for ( StringSet::iterator iterator = allTrustees->begin(); iterator != allTrustees->end(); iterator++ ) {
        if ( this->IsMatch ( trusteePatternStr, ( *iterator ), isRegex ) ) {
            trustees->insert ( ( *iterator ) );
        }
    }
}

bool AbsEffectiveRightsProbe::ReportTrusteeDoesNotExist ( ObjectEntity *trusteeEntity, bool isSID ) {
    bool result = false;

    if ( trusteeEntity->GetOperation() == OvalEnum::OPERATION_EQUALS && !trusteeEntity->GetNil() ) {
        if ( trusteeEntity->GetVarRef() == NULL ) {
            if ( ( isSID && !WindowsCommon::TrusteeSIDExists ( trusteeEntity->GetValue() ) ) || (!isSID && !WindowsCommon::TrusteeNameExists ( trusteeEntity->GetValue() ) ) ) {
                result = true;
            }

        } else {
            for ( VariableValueVector::iterator iterator = trusteeEntity->GetVarRef()->GetValues()->begin(); iterator != trusteeEntity->GetVarRef()->GetValues()->end(); iterator++ ) {
				if ( ( isSID && !WindowsCommon::TrusteeSIDExists ( (*iterator)->GetValue() ) ) || ( !isSID && !WindowsCommon::TrusteeNameExists ( (*iterator)->GetValue() ) ) ) {
                    result = true;
					break;
                }
            }
        }
    }

    return  result;
}
