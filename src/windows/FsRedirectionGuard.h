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

#ifndef FSREDIRECTIONGUARD_H
#define FSREDIRECTIONGUARD_H

#include <WindowsCommon.h> // for BitnessView
#include <WinDef.h> // just need the PVOID type

/**
 * This class uses RAII to disable and re-enable WOW64 filesystem
 * redirection, if necessary, depending on the OS and app bitness
 * and the requested bitnessView.
 */
class FsRedirectionGuard {
public:
	/**
	 * If this is a 32-bit app running on 64-bit windows and view is
	 * 64_BIT, this disables WOW64 filesystem redirection.  Otherwise,
	 * this does nothing.
	 *
	 * \throw Exception if disabling fails.
	 */
	explicit FsRedirectionGuard(BitnessView view);

	/**
	 * If WOW64 redirection was previously disabled, this re-enables it.
	 * If it was not previously disabled, this does nothing.
	 */
	~FsRedirectionGuard();

private:
	bool wasDisabled;
	PVOID state;
};

// Allow macro'd use of the filesystem guard, so it can be omitted
// when not needed (in a 64-bit app).  These macros form a nested
// scope, so don't try to use them across an existing scope boundary,
// it won't work!  And of course, declaring a variable inside the
// nested scope will make it unusable outside...
#ifdef _WIN64
#  define FS_REDIRECT_GUARD_BEGIN(VIEW)
#  define FS_REDIRECT_GUARD_END
#else
#  define FS_REDIRECT_GUARD_BEGIN(VIEW) { FsRedirectionGuard __fsrg(VIEW);
#  define FS_REDIRECT_GUARD_END }
#endif

#endif
