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

#include "GroupSidProbe.h"

//****************************************************************************************//
//                              GroupSidProbe Class                                       //
//****************************************************************************************//
GroupSidProbe* GroupSidProbe::instance = NULL;

GroupSidProbe::GroupSidProbe() : AbsProbe() {
}

GroupSidProbe::~GroupSidProbe() {
    instance = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* GroupSidProbe::Instance() {
    // Use lazy initialization
    if ( instance == NULL ) {
        instance = new GroupSidProbe();
    }

    return instance;
}

ItemVector* GroupSidProbe::CollectItems ( Object *object ) {
    ObjectEntity* groupSid = object->GetElementByName ( "group_sid" );

    if ( groupSid->GetDatatype() != OvalEnum::DATATYPE_STRING ) {
        throw ProbeException ( "Error: invalid data type specified on group_sid. Found: " + OvalEnum::DatatypeToString ( groupSid->GetDatatype() ) );
    }

    if ( groupSid->GetOperation() != OvalEnum::OPERATION_EQUALS && groupSid->GetOperation() != OvalEnum::OPERATION_PATTERN_MATCH && groupSid->GetOperation() != OvalEnum::OPERATION_NOT_EQUAL ) {
        throw ProbeException ( "Error: invalid operation specified on group_sid. Found: " + OvalEnum::OperationToString ( groupSid->GetOperation() ) );
    }

    ItemVector *collectedItems = new ItemVector();

    if ( groupSid->GetVarRef() == NULL ) {
        if ( groupSid->GetOperation() == OvalEnum::OPERATION_EQUALS ) {
            Item* item =  this->GetGroupSidInfo ( groupSid->GetValue() );

            if ( item != NULL ) {
                collectedItems->push_back ( item );
            }

        } else {
            bool isRegex = false;

            if ( groupSid->GetOperation() == OvalEnum::OPERATION_PATTERN_MATCH ) {
                isRegex = true;
            }

            StringSet* allLocalGroupSids = new StringSet();
            StringSet* allLocalGroups = WindowsCommon::GetAllGroups();
            WindowsCommon::ConvertTrusteeNamesToSidStrings ( allLocalGroups, allLocalGroupSids );
            allLocalGroups->clear();
            delete allLocalGroups;
            allLocalGroups = NULL;

            for ( StringSet::iterator iterator = allLocalGroupSids->begin(); iterator != allLocalGroupSids->end(); iterator++ ) {
                if ( this->IsMatch ( groupSid->GetValue(), ( *iterator ), isRegex ) ) {
                    Item* item =  this->GetGroupSidInfo ( *iterator );

                    if ( item != NULL ) {
                        collectedItems->push_back ( item );
                    }
                }
            }

            allLocalGroupSids->clear();
            delete allLocalGroupSids;
            allLocalGroupSids = NULL;
        }

    } else {
        StringSet* allLocalGroupSids = new StringSet();
        StringSet* allLocalGroups = WindowsCommon::GetAllGroups();
        WindowsCommon::ConvertTrusteeNamesToSidStrings ( allLocalGroups, allLocalGroupSids );
        allLocalGroups->clear();
        delete allLocalGroups;
        allLocalGroups = NULL;
        ItemEntity* tmp = this->CreateItemEntity ( groupSid );

        for ( StringSet::iterator iterator = allLocalGroupSids->begin(); iterator != allLocalGroupSids->end(); iterator++ ) {
            tmp->SetValue ( ( *iterator ) );

            if ( groupSid->Analyze ( tmp ) == OvalEnum::RESULT_TRUE ) {
                Item* item = this->GetGroupSidInfo ( ( *iterator ) );

                if ( item != NULL ) {
                    collectedItems->push_back ( item );
                }
            }
        }

        allLocalGroupSids->clear();
        delete allLocalGroupSids;
        allLocalGroupSids = NULL;
        delete tmp;
        tmp = NULL;
    }

    return collectedItems;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
Item* GroupSidProbe::CreateItem() {
    Item* item = new Item ( 0,
                            "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows",
                            "win-sc",
                            "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows windows-system-characteristics-schema.xsd",
                            OvalEnum::STATUS_ERROR,
                            "group_sid_item" );
    return item;
}

Item* GroupSidProbe::GetGroupSidInfo ( string groupSidStr ) {
    string userNameStr;
    StringSet* userSids = new StringSet();
    Item* item = this->CreateItem();

    if ( WindowsCommon::ExpandGroupBySID ( groupSidStr, userSids, false, false ) ) {
        item->SetStatus ( OvalEnum::STATUS_EXISTS );
        item->AppendElement ( new ItemEntity ( "group_sid", groupSidStr, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS ) );

        if ( userSids->size() > 0 ) {
            for ( StringSet::iterator iterator = userSids->begin() ; iterator != userSids->end() ; iterator++ ) {
                item->AppendElement ( new ItemEntity ( "user_sid", *iterator, OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS ) );
            }

        } else {
            item->AppendElement ( new ItemEntity ( "user_sid", "", OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_DOES_NOT_EXIST ) );
        }

    } else {
        item->SetStatus ( OvalEnum::STATUS_DOES_NOT_EXIST );
        item->AppendElement ( new ItemEntity ( "group_sid", groupSidStr, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_DOES_NOT_EXIST ) );

        if ( WindowsCommon::IsGroupSID ( groupSidStr ) == 0 ) {
            item->AppendMessage ( new OvalMessage ( "The specified SID is not a group SID." ) );
        }
    }

    userSids->clear();
    delete userSids;
    userSids = NULL;
    return item;
}