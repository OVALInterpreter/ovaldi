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

#ifndef FREEGUARD_H
#define FREEGUARD_H

#include <cstdlib>
#include <Noncopyable.h>

/**
 * Factors out code common to FreeGuard<T> and its specialization for
 * T=void.
 * <p>
 * Users can't create instances of this class.  It is meant only to 
 * subclass.  But subclasses will not override anything (including 
 * destructors, since subclasses have no state of their own!), so nothing
 * is virtual; compile-time binding is fine.  I inherit privately
 * to make durn sure no one calls methods through pointer/ref to 
 * base...
 */
template <typename T>
class FreeGuardBase : private Noncopyable {
protected:
	explicit FreeGuardBase(T *ptr = NULL) : ptr(ptr) {
	}

	~FreeGuardBase() {
		if (ptr)
			std::free(ptr);
	}

public:
	/**
	 * Resets the guard to protect the given memory block.  Any memory
	 * block previously protected is first freed.
	 */
	void reset(T *newPtr) {
		if (ptr != newPtr) {
			if (ptr)
				std::free(ptr);
			ptr = newPtr;
		}
	}

	/**
	 * Return a pointer to the protected memory block.  This guard will
	 * no longer protect the block.  Returns NULL if no memory block
	 * is being protected.
	 */
	T *release() {
		T *tmp = ptr;
		ptr = NULL;
		return tmp;
	}

	/**
	 * This is a convenience method for doing a std::realloc()
	 * on the protected buffer.  It acts just like std::realloc():
	 * if \p newSize is zero, the memory is freed.  If this
	 * guard was previously not protecting anything, it
	 * acts as a malloc and allocates a new block of memory.
	 * If the realloc fails, the original memory block is
	 * left alone.  Calling realloc(0) on a guard that is
	 * not protecting anything is a no-op.
	 *
	 * \param newSize the size to reallocate the block to.
	 *   If zero, the block is freed.  This is not considered
	 *   an error (true is returned).
	 * \return true if the reallocation succeeded, or false if
	 *   it failed.  Check errno.
	 */
	bool realloc(size_t newSize) {
		// I am not sure how realloc handles this...
		// but just in case, I'll handle it myself.
		// Behave as if realloc(NULL, 0) is a no-op.
		if (ptr == NULL && newSize == 0) return true;

		void *tmp = std::realloc(ptr, newSize);
		if (tmp) {
			ptr = static_cast<T*>(tmp);
			return true;
		} else if (newSize == 0) {
			// in this case, realloc() actually
			// freed the memory and returned NULL.
			ptr = NULL;
			return true;
		}

		return false;
	}

	/** Get a pointer to the protected block.  Don't free this yourself. */
	T *get() {
		return ptr;
	}

	/** Get a pointer to the protected block.  Don't free this yourself. */
	const T *get() const {
		return ptr;
	}

	/** Only use if T is a struct! */
	T *operator->() {
		return ptr;
	}

	/** Only use if T is a struct! */
	const T *operator->() const {
		return ptr;
	}

protected:
	T *ptr;
};

/**
 * Instances of this class protect a pointer to a memory block
 * which was allocated with malloc et al.  When the object goes
 * out of scope, the memory is freed with free().
 * <p>
 * Allowing a template type for the pointee type allows 
 * an operator-> definition which I hope make this
 * class more convenient to use, when mallocing structs.
 * Of course operator-> makes no sense if T is a primitive type,
 * so don't use it then!  The compile error (on msvc) will look like:
 * \verbatim invalid return type 'int *' for overloaded 'operator ->' \endverbatim
 * where instead of 'int' you will see your primitive type.
 * <p>
 * I also added operator[] to make it easier to use with malloc'd
 * arrays.  But you can't use operator[] if T = void, since you
 * can't subscript a variable of type void*.  So to prevent compiler
 * errors, I have created a specialization for void which does
 * not define operator[].  The definition of the base template and
 * and void specialization would be identical except for the
 * lack of operator[] in the specialization, so I factored
 * the commonalities out to FreeGuardBase.
 * <p>
 * An obvious question to ask is: why would you ever use malloc
 * in a C++ program anyway??  That's what the 'new' operator is
 * for!  I don't know a good reason to use malloc, but it's used
 * in many places in this codebase.  One could make an argument
 * for using realloc though, since I don't know of a similar
 * block-resizing capability in C++.  I would say it's much less
 * common for C++ programs to need unstructured blocks of memory
 * though; most of the time you are allocating memory for a
 * specific type, or an array of a specific type.  But some kinds
 * of application do need that.  And someone might argue they need
 * to resize that memory block, and want the data-preserving
 * capability of realloc...
 *
 * \see FreeGuardBase
 */
template <typename T = void>
class FreeGuard : private FreeGuardBase<T> {
private:
	typedef FreeGuardBase<T> base_type;

	// gcc doesn't seem to search the base class
	// scope automatically, so I bring this in explicitly for use
	// by the subscript operators.
	using base_type::ptr;
public:
	// These are explicitly brought into scope so I
	// can preserve my private inheritance.  I don't
	// want people calling methods through a
	// pointer/ref to base, but I still want these
	// methods factored out to the base class so they
	// are not duplicated.
	using base_type::reset;
	using base_type::release;
	using base_type::realloc;
	using base_type::get;
	using base_type::operator->;

	/**
	 * Initializes the guard to protect the given block of
	 * memory.  If \p ptr is NULL, the guard will not
	 * protect anything.
	 */
	explicit FreeGuard(T *ptr = NULL) : base_type(ptr) {
	}

	/**
	 * A convenience constructor so you don't have to cast the
	 * return type of ?alloc().  They all return void* so you'd
	 * have to always cast it manually in the constructor call.
	 * This does it for you.
	 */
	explicit FreeGuard(void *ptr) : base_type(static_cast<T*>(ptr)) {
	}

	/**
	 * Resets the guard to protect the given memory block.  Any memory
	 * block previously protected is first freed.  This is a convenience
	 * overload which does casting from void* to T* for you.
	 */
	void reset(void *newPtr) {
		if (ptr != newPtr) {
			if (ptr)
				std::free(ptr);
			ptr = static_cast<T*>(newPtr);
		}
	}

	/** Only use if an array is being guarded! */
	T &operator[](size_t i) {
		return ptr[i];
	}

	/** Only use if an array is being guarded! */
	const T &operator[](size_t i) const {
		return ptr[i];
	}
};

/**
 * The void specialization has no operator[] because you
 * can't use the subscript operator with a void*.
 */
template<>
class FreeGuard<void> : private FreeGuardBase<void> {
private:
	typedef FreeGuardBase<void> base_type;

public:
	// These are explicitly brought into scope so I
	// can preserve my private inheritance.  I don't
	// want people calling methods through a
	// pointer/ref to base, but I still want these
	// methods factored out to the base class so they
	// are not duplicated.
	using base_type::reset;
	using base_type::release;
	using base_type::realloc;
	using base_type::get;
	using base_type::operator->;

	/**
	 * Initializes the guard to protect the given block of
	 * memory.  If \p ptr is NULL, the guard will not
	 * protect anything.
	 */
	explicit FreeGuard(void *ptr = NULL) : base_type(ptr) {
	}
};

#endif
