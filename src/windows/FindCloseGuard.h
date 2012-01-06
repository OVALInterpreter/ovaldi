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

#ifndef FINDCLOSEGUARD_H
#define FINDCLOSEGUARD_H

// all of Windows.h just for the HANDLE type??  Yeah,
// it seems WinDef.h won't compile by itself since it
// needs an arch macro (e.g. _X86_) to be defined, and that's
// defined in windows.h.  Sighhhhhhh
#include <Windows.h>
#include <Noncopyable.h>

/**
 * Provides for exception safety when writing recursive filesystem search
 * algorithms.  As the stack unwinds, we should try to close all their
 * find handles.
 */
class FindCloseGuard : public Noncopyable {
public:
	explicit FindCloseGuard(HANDLE hFind);
	~FindCloseGuard();

	/**
	 * Closes the find handle, if it was not already closed.
	 * This flags the handle as closed, whether or not the close
	 * succeeded.  Not sure if this is the best way to do it...
	 * If the close failed, would there ever be a reason to try
	 * it again?  If the close failed, callers can call GetLastError()
	 * to find out why.
	 * <p>
	 * This guard is manually closable to be compatible with existing
	 * code which wants to close, and throw an exception if it failed.
	 *
	 * \return the return value of FindClose() if a close was attempted,
	 *   or true if not (because it was already closed).
	 */
	bool close();

private:
	bool closed;
	HANDLE findHandle;
};

#endif
