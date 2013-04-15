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

#ifndef ARRAYGUARD_H
#define ARRAYGUARD_H

#include <Noncopyable.h>

/**
 * Makes exception-safe use of new'd arrays more convenient.  Instances
 * of this class own an array, and will delete[] the array (but not its
 * contents!) when it goes out of scope, so you don't have to remember to free
 * it.  It also contains some convenience methods similar to auto_ptr, for
 * managing ownership, and convenience methods for access.  This class does not
 * implement ownership transfer semantics, and is noncopyable.
 */
template <typename T>
class ArrayGuard : private Noncopyable {
public:
	/**
	 * Initializes the guard to protect the given array.
	 */
	explicit ArrayGuard(T *arr = NULL) : arr(arr) {
	}

	/**
	 * If the guard protects an array, frees it.
	 */
	~ArrayGuard() {
		if (arr)
			delete[] arr;
	}

	/**
	 * Changes the guard to protect the given array instead of
	 * the current array (if any).  If an array is already protected,
	 * it is first freed.
	 */
	void reset(T *newArr = NULL) {
		if (newArr != arr) {
			if (arr)
				delete[] arr;
			arr = newArr;
		}
	}

	/**
	 * Returns the protected array.  The array will no longer be 
	 * protected by this guard.
	 */
	T *release() {
		T *tmp = arr;
		arr = NULL;
		return tmp;
	}

	/**
	 * Returns the protected array.  Don't free this yourself.
	 */
	T *get() {
		return arr;
	}

	/**
	 * Returns the protected array.  Don't free this yourself.
	 */
	const T *get() const {
		return arr;
	}

	/**
	 * Allows use of the guard like an array: returns a ref to the
	 * i'th element.  This is not bounds-checked!  It also does not
	 * verify that the guard is in fact protecting an array at all!
	 * You are responsible for making sure the access is safe.
	 */
	T& operator[](size_t idx) {
		return arr[idx];
	}

	/**
	 * Allows use of the guard like an array: returns a ref to the
	 * i'th element.  This is not bounds-checked!  It also does not
	 * verify that the guard is in fact protecting an array at all!
	 * You are responsible for making sure the access is safe.
	 */
	const T& operator[](size_t idx) const {
		return arr[idx];
	}

private:
	/** The protected array */
	T *arr;
};

#endif
