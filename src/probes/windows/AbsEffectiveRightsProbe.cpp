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

StringSet* AbsEffectiveRightsProbe::GetTrusteeSIDsForWindowsObject ( SE_OBJECT_TYPE objectType, string objectNameStr, ObjectEntity* trusteeSIDEntity,  bool resolveGroupBehavior, bool includeGroupBehavior ) {
    PACL pdacl = NULL;
    PSID ownerSid;
    PSID primaryGroupSid;
    PSECURITY_DESCRIPTOR pSD = NULL;
    StringSet* allTrusteeSIDs = new StringSet();
    StringSet* workingTrusteeSIDs = new StringSet();
    StringSet* resultingTrusteeSIDs = new StringSet();

    try {
        // Start by getting the sids from the dacl, owner, and primary group off the object's security descriptor
        string baseErrMsg = "Error: unable to get trustees for " + WindowsCommon::GetObjectType ( objectType ) + " " + objectNameStr + ".";
        DWORD res = GetNamedSecurityInfo ( const_cast<char*> ( objectNameStr.c_str() ), // Object name
                                           objectType,                                  // Object type
                                           DACL_SECURITY_INFORMATION |                  // Information type
                                           GROUP_SECURITY_INFORMATION |
                                           OWNER_SECURITY_INFORMATION,
                                           &ownerSid,                                   // Owner SID
                                           &primaryGroupSid,                            // Primary group SID
                                           &pdacl,                                      // DACL
                                           NULL,                                        // SACL
                                           &pSD );                                      // Security Descriptor

        if ( res != ERROR_SUCCESS ) {
            throw Exception ( baseErrMsg + " Unable to retrieve a copy of the security descriptor. Microsoft System Error (" + Common::ToString ( res ) + ") - " + WindowsCommon::GetErrorMessage ( res ) );
        }

        // Get sids from the dacl
        WindowsCommon::GetSidsFromPACL ( pdacl, allTrusteeSIDs );
        // Insert the owner and primary group sids
        allTrusteeSIDs->insert ( WindowsCommon::ToString ( ownerSid ) );
        allTrusteeSIDs->insert ( WindowsCommon::ToString ( primaryGroupSid ) );
        Log::Debug ( "Found " + Common::ToString ( allTrusteeSIDs->size() ) + " trustee SIDs when searching for all SIDs on the security descriptor of: " + objectNameStr );

        // Does this trusteeSIDEntity use variables?
        if ( trusteeSIDEntity->GetVarRef() == NULL ) {
            // Proceed based on operation
            if ( trusteeSIDEntity->GetOperation() == OvalEnum::OPERATION_EQUALS ) {
                // Check that the trustee SID exists
                if ( WindowsCommon::TrusteeSIDExists ( trusteeSIDEntity->GetValue() ) ) {
                    workingTrusteeSIDs->insert ( trusteeSIDEntity->GetValue() );
                }

            } else if ( trusteeSIDEntity->GetOperation() == OvalEnum::OPERATION_NOT_EQUAL ) {
                this->GetMatchingTrusteeSIDs ( trusteeSIDEntity->GetValue(), allTrusteeSIDs, workingTrusteeSIDs, false );

            } else if ( trusteeSIDEntity->GetOperation() == OvalEnum::OPERATION_PATTERN_MATCH ) {
                this->GetMatchingTrusteeSIDs ( trusteeSIDEntity->GetValue(), allTrusteeSIDs, workingTrusteeSIDs, true );
            }

        } else {
            if ( trusteeSIDEntity->GetOperation() == OvalEnum::OPERATION_EQUALS ) {
                // In the case of equals simply loop through all the variable values and add them to the set of all sids if they exist on the system
                for ( VariableValueVector::iterator iterator = trusteeSIDEntity->GetVarRef()->GetValues()->begin(); iterator != trusteeSIDEntity->GetVarRef()->GetValues()->end(); iterator++ ) {
                    if ( WindowsCommon::TrusteeSIDExists ( ( *iterator )->GetValue() ) ) {
                        allTrusteeSIDs->insert ( ( *iterator )->GetValue() );
                    }
                }
            }

            // Loop through all of the trustee SIDs on the Security Descriptor and only keep those that match operation, value, and var check
            ItemEntity* tmp = this->CreateItemEntity ( trusteeSIDEntity );

            for ( StringSet::iterator it = allTrusteeSIDs->begin(); it != allTrusteeSIDs->end(); it++ ) {
                tmp->SetValue ( ( *it ) );

                if ( trusteeSIDEntity->Analyze ( tmp ) == OvalEnum::RESULT_TRUE ) {
                    workingTrusteeSIDs->insert ( ( *it ) );
                }
            }
        }

        allTrusteeSIDs->clear();
        delete allTrusteeSIDs;

        // Apply the behaviors
        for ( StringSet::iterator it = workingTrusteeSIDs->begin(); it != workingTrusteeSIDs->end(); it++ ) {
            // Is this a group
            bool isGroup = WindowsCommon::IsGroupSID ( ( *it ) );

            if ( isGroup && resolveGroupBehavior ) {
                if ( includeGroupBehavior ) {
                    resultingTrusteeSIDs->insert ( ( *it ) );
                }

                // Get the group members and add them to the set
                StringSet* groupMembers = new StringSet();
                WindowsCommon::ExpandGroupBySID ( ( *it ), groupMembers, includeGroupBehavior, resolveGroupBehavior );

                for ( StringSet::iterator iterator = groupMembers->begin(); iterator != groupMembers->end(); iterator++ ) {
                    resultingTrusteeSIDs->insert ( ( *iterator ) );
                }

                delete groupMembers;

            } else {
                resultingTrusteeSIDs->insert ( ( *it ) );
            }
        }

    } catch ( ... ) {
        if ( pSD != NULL ) {
            LocalFree ( pSD );
        }

        if ( allTrusteeSIDs != NULL ) {
            delete allTrusteeSIDs;
            allTrusteeSIDs = NULL;
        }

        if ( workingTrusteeSIDs != NULL ) {
            delete workingTrusteeSIDs;
            workingTrusteeSIDs = NULL;
        }

        if ( resultingTrusteeSIDs != NULL ) {
            delete resultingTrusteeSIDs;
            resultingTrusteeSIDs = NULL;
        }

        throw;
    }

    if ( pSD != NULL ) {
        LocalFree ( pSD );
    }

    if ( workingTrusteeSIDs != NULL ) {
        delete workingTrusteeSIDs;
        workingTrusteeSIDs = NULL;
    }

    Log::Debug ( "Found " + Common::ToString ( resultingTrusteeSIDs->size() ) + " matching SIDs after applying behaviors on the security descriptor of: " + objectNameStr );
    return resultingTrusteeSIDs;
}

void AbsEffectiveRightsProbe::GetMatchingTrusteeSIDs ( string trusteeSIDPatternStr, StringSet* allTrusteeSIDs, StringSet* trusteeSIDs, bool isRegex ) {
    for ( StringSet::iterator iterator = allTrusteeSIDs->begin(); iterator != allTrusteeSIDs->end(); iterator++ ) {
        if ( this->IsMatch ( trusteeSIDPatternStr, ( *iterator ), isRegex ) ) {
            trusteeSIDs->insert ( ( *iterator ) );
        }
    }
}

bool AbsEffectiveRightsProbe::ReportTrusteeSIDDoesNotExist ( ObjectEntity *trusteeSIDEntity, StringSet* trusteeSIDs ) {
    bool result = false;

    if ( trusteeSIDEntity->GetOperation() == OvalEnum::OPERATION_EQUALS && !trusteeSIDEntity->GetNil() ) {
        if ( trusteeSIDEntity->GetVarRef() == NULL ) {
            if ( !WindowsCommon::TrusteeSIDExists ( trusteeSIDEntity->GetValue() ) ) {
                trusteeSIDs->insert ( trusteeSIDEntity->GetValue() );
                result = true;
            }

        } else {
            for ( VariableValueVector::iterator iterator = trusteeSIDEntity->GetVarRef()->GetValues()->begin(); iterator != trusteeSIDEntity->GetVarRef()->GetValues()->end(); iterator++ ) {
                if ( !WindowsCommon::TrusteeSIDExists ( ( *iterator )->GetValue() ) ) {
                    trusteeSIDs->insert ( ( *iterator )->GetValue() );
                    result = true;
                }
            }
        }
    }

    return  result;
}
