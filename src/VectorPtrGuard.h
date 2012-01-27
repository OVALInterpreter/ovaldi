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

#ifndef VECTORPTRGUARD_H
#define VECTORPTRGUARD_H

#include <vector>

#include <Noncopyable.h>

/**
 * Makes exception-safe use of a vector of pointers more convenient.  Instances
 * of this class own a std::vector<T*>*, and will delete both the vector and its
 * contents when they go out of scope, so you don't have to remember to free
 * it.  It also contains some convenience methods similar to auto_ptr, for
 * managing ownership, and convenience methods for access.  This class does not
 * implement ownership transfer semantics, and is noncopyable.  I really didn't
 * want to have to deal with the auto_ptr_ref junk STL code has to go through
 * with auto_ptr to get assignment operators and copy constructors that could
 * modify their args...
 */
template <typename T>
class VectorPtrGuard : private Noncopyable {
public:

	/**
	 * Initialize to wrap an empty vector.
	 */
	VectorPtrGuard() : vec(NULL) {
	}

	/**
	 * Creates the guard, taking ownership of the given
	 * vector.
	 */
	explicit VectorPtrGuard(std::vector<T*> *vec) : vec(vec) {
	}

	~VectorPtrGuard() {
		DeleteVec();
	}

	/**
	 * Mimics the auto_ptr method of the same name: returns the contained
	 * vector, which will no longer be owned by this VectorPtrGuard.
	 */
	std::vector<T*>* release() {
		std::vector<T*> *tmp = vec;
		vec = NULL;
		return tmp;
	}

	/**
	 * Mimics the auto_ptr method of the same name: causes this VectorPtrGuard
	 * to take ownership of the given vector.  If there was previously an owned
	 * vector, it is first destroyed.
	 */
	void reset(std::vector<T*> *newVec = NULL) {
		if (vec != newVec) {	
			DeleteVec();
			vec = newVec;
		}
	}

	/**
	 * Returns the contained vector.  This VectorPtrGuard still owns it, so
	 * don't delete it!  You may change its contents though.
	 */
	std::vector<T*>* get() {
		return vec;
	}

	/**
	 * Returns the contained vector.  This VectorPtrGuard still owns it, so
	 * don't delete it!
	 */
	const std::vector<T*>* get() const {
		return vec;
	}

	/**
	 * Convenience: makes it easy to invoke methods on the underlying vector.
	 */
	std::vector<T*>* operator->() {
		return vec;
	}

	/**
	 * Convenience: makes it easy to invoke methods on the underlying vector.
	 */
	const std::vector<T*>* operator->() const {
		return vec;
	}

private:
	/**
	 * Called from the destructor, so can't throw!  (I hope this code doesn't
	 * throw :-P  gcc's begin(), end(), and operator++ aren't declared
	 * throw()... so I'm not sure enough to actually declare this throw().)
	 */
	void DeleteVec() {
		if (vec) {
			for (typename std::vector<T*>::iterator iter = vec->begin();
				 iter != vec->end();
				 ++iter)
				if (*iter)
					delete *iter;
			delete vec;
			vec = NULL;
		}
	}

	std::vector<T*> *vec;
};

#endif
