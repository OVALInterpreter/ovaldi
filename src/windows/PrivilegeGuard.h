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

#ifndef PRIVILEGEGUARD_H
#define PRIVILEGEGUARD_H

#include <string>
#include <Noncopyable.h>

/**
 * Allows exception-safe acquisition and release of one privilege by the 
 * current process.  The ctor acquires the privilege and the dtor
 * releases it.
 */
class PrivilegeGuard : private Noncopyable
{
public:
	/**
	 * Acquire the privilege.
	 *
	 * \param privName a privilege, e.g. one of the SE_*_NAME privilege constants.
	 * \param ctorThrow If true, this constructor will throw if privilege
	 *     acquisition fails.  If false, only a message is logged.  This is to
	 *     maintain the same behavior as pre-existing code.  Exception vs message 
	 *     logging behavior is not consistent across the codebase.
	 */
	explicit PrivilegeGuard(const std::string &privName, bool ctorThrow = true);

	/**
	 * Releases the privilege.  A message is logged if this fails.
	 */
	~PrivilegeGuard();

	/**
	 * This method is to maintain the same behavior as old code where an
	 * exception was thrown if privilege release failed.  If privilege release
	 * fails, this method will throw.  If this method is called, neither the
	 * dtor nor further calls to this method will attempt to release the 
	 * privilege again.
	 */
	void disable();

private:
	bool disabled;
};

#endif
