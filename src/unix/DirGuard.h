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

#ifndef DIRGUARD_H
#define DIRGUARD_H

#include <dirent.h>
#include <string>
#include <Noncopyable.h>

/**
 * Used to manage opening/closing a directory via opendir()/closedir(), so
 * we can write exception-safe code.
 */
class DirGuard : private Noncopyable {
public:

	/**
	 * Initializes the guard by attempting to open the given directory.  If
	 * throwOnFailure==true and the open fails, IOException is thrown.  If
	 * throwOnFailure==false and the open fails, the directory is simply marked
	 * as closed and no exception is thrown.  Callers can call isClosed()
	 * immediately after creating the object to determine whether the open
	 * succeeded.
	 */
	explicit DirGuard(const std::string &dirName, bool throwOnFailure = true);
	~DirGuard();

	/**
	 * Returns a pointer to the encapsulated DIR structure.  If the directory
	 * has been closed, this returns NULL.
	 */
	operator DIR*();

	/**
	 * Closes the directory, if not already closed.  After this, the directory
	 * is flagged as closed and will not be closed again (either by the
	 * destructor or this method).  Manual close capability is provided for
	 * compatibility with code that wants to close the directory itself, and
	 * throw an exception if closure failed.
	 *
	 * \return if the directory is not closed, returns the return value of
	 * closedir().  If the directory was already closed, returns 0.  errno can
	 * be queried to determine reasons for failure.
	 */
	int close();

	bool isClosed() const {
		return closed;
	}

private:
	DIR *d;
	bool closed;
};

#endif
