//
//
//****************************************************************************************//
// Copyright (c) 2002-2014, The MITRE Corporation
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

#include <Exception.h>
#include <Log.h>
#include <WindowsCommon.h>
#include <Common.h>

#include "PrivilegeGuard.h"

using namespace std;

PrivilegeGuard::PrivilegeGuard(const string &privName, bool ctorThrow)
	: disabled(false) {
	if ( !WindowsCommon::EnablePrivilege ( privName ) ) {
		string msg = "Error: Security privilege " + privName +
			" could not be enabled. Microsoft System Error " +
			Common::ToString ( GetLastError() ) + " - " + 
			WindowsCommon::GetErrorMessage ( GetLastError() );

		if (ctorThrow)
			throw Exception(msg);
		else {
			// We couldn't enable the priv, so no point in trying to disable
			// it later.
			disabled = true;
			Log::Message(msg);
		}
	}
}

PrivilegeGuard::~PrivilegeGuard() {
	if (!disabled)
		if ( !WindowsCommon::DisableAllPrivileges() ) {
			Log::Message( "Error: All of the privileges could not be disabled. Microsoft System Error " +
				Common::ToString ( GetLastError() ) + " - " +
				WindowsCommon::GetErrorMessage ( GetLastError() ) );
		}
}

void PrivilegeGuard::disable() {
	if (disabled) return;

	disabled = true;

	if ( !WindowsCommon::DisableAllPrivileges() ) {
		throw Exception ( "Error: All of the privileges could not be disabled. Microsoft System Error " +
			Common::ToString ( GetLastError() ) + " - " +
			WindowsCommon::GetErrorMessage ( GetLastError() ) );
	}
}
