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

#ifndef RPMGUARDS_H
#define RPMGUARDS_H

#include <rpm/rpmcli.h>
#include <rpm/rpmts.h>
#include <rpm/rpmfi.h>
#include <rpm/rpmdb.h>
#include <string>

#include <IOException.h>
#include <Noncopyable.h>

// Perhaps all these guard classes aren't strictly necessary in all cases,
// but in C++ code which uses exceptions, I feel a lot safer having them.
// And you never know, someone else may come along later and add a throw
// where there wasn't before, which without the guard classes could cause a
// resource leak.  I saw an article online somewhere describing a more
// general mechanism, where I think you could register any C function to be
// called when the object is destroyed.  I might try that, if I could find
// the article again....

/**
 * A guard class for managing an rpm transaction set.  The contained value is of
 * type \c rpmts, acquired with \c rpmtsCreate(), and released with \c
 * rpmtsFree().
 */
class RpmtsGuard : private Noncopyable {
public:
	RpmtsGuard() {
		ts = rpmtsCreate();
			
		// I don't think this can ever happen.  From my reading of the rpm
		// source, I think it would exit the program if memory allocation
		// failed.  But it doesn't hurt to check...
		if (!ts)
			throw IOException("Couldn't create RPM database transaction set");
	}

	~RpmtsGuard() {
		rpmtsFree(ts);
	}

	operator rpmts() {
		return ts;
	}

private:
	rpmts ts;
};

/**
 * A guard class for managing an rpm database iterator.  The contained value is
 * of type \c rpmdbMatchIterator, acquired with \c rpmtsInitIterator(), and
 * released with \c rpmdbFreeIterator().
 */
class RpmdbIterGuard : private Noncopyable {
public:
	RpmdbIterGuard(const rpmts ts, rpmTag rpmtag,
				   const void * keyp, size_t keylen) {
		iter = rpmtsInitIterator(ts, rpmtag, keyp, keylen);

		if (!iter)
			throw IOException("Couldn't create RPM database iterator");
	}

	~RpmdbIterGuard() {
		rpmdbFreeIterator(iter);
	}

	operator rpmdbMatchIterator() {
		return iter;
	}
		
private:
	rpmdbMatchIterator iter;
};

/**
 * A guard class for managing info about a file within an rpm package.  The
 * contained value is of type \c rpmfi, acquired with \c rpmfiNew(), and
 * released with \c rpmfiFree().
 */
class RpmfiGuard : private Noncopyable {
public:

	RpmfiGuard(rpmts ts, Header hdr, rpmTag tag, 
			   const std::string &rpmName /* for error messages */) {
		fileInfo = rpmfiNew(ts, hdr, tag, 0);
			
		if (!fileInfo)
			throw IOException("Couldn't init file iterator on RPM "+rpmName);
	}

	~RpmfiGuard() {
		rpmfiFree(fileInfo);
	}
		
	operator rpmfi() {
		return fileInfo;
	}		 

private:
	rpmfi fileInfo;
};	

#endif
