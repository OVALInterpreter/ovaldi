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

#ifndef AUTOCLOSER_H
#define AUTOCLOSER_H

#include <Windows.h>
#include <string>
#include <Noncopyable.h>
#include <Log.h>

/**
 * There seem to be a lot of "close" functions in win32 which close
 * handles of various types.  Many have the same basic signature,
 * and it was getting awfully repetitive making the same basic
 * guard class, only changing the handle type and/or what closer
 * function was being called.  So I decided to make a template for
 * it.
 * <p>
 * There are a couple different closer function prototypes I've seen,
 * and I decided to support both:
 * <ol>
 *   <li>BOOL (WINAPI *CloserType1)(_HandleType)</lib>
 *   <li>LONG (WINAPI *CloserType2)(_HandleType)</li>
 * </ol>
 * The first returns FALSE on error, and the error code is obtained
 * via GetLastError().  The second returns the error code directly,
 * which is ERROR_SUCCESS on success, and something else on error.
 * Destructors of instances of this class will call a closer function
 * on a provided handle.
 * <p>
 * There are several ways to have done this.  I could have had a
 * single class with two constructors, overloaded on the closer
 * function type.  Each sets up the AutoCloser to remember the
 * closer type (and closer function) and call it correctly.  Another
 * way, which is implemented here, is to move the closer type out to
 * a template parameter, and create specializations of a base template
 * for the closer types I want to support.  The difference between
 * those two approaches amounts to making a runtime decision about
 * which type of closer to use, vs a compile time decision.
 * Compile-time decisions are always faster at runtime, since there
 * is no runtime overhead.  This probably isn't really time-critical,
 * but I felt like doing it this way :)  The code for using it is
 * a bit uglier though: AutoCloser<HANDLE> foo(...); vs, e.g.
 * AutoCloser<HANDLE,BOOL(WINAPI&)(HANDLE)> foo(...);.
 * <p>
 * Another way would be to create separate templates, rather than
 * specializing a single base template.  What would I call them?
 * AutoCloser1 and AutoCloser2?  Then people would have to remember
 * which to use... Maybe having one template name to remember is less
 * troublesome for people.
 * <p>
 * All of these templates are noncopyable because the underlying 
 * handle isn't dupable (it would be a mistake to have two closers
 * close the same handle).
 */
template<typename _HandleType, typename _CloserType>
class AutoCloser {
private:
	// make this base template instantiate unconstructible
	// classes.  I am specializing it specifically for
	// certain closer function types; if the closer isn't
	// one of those types, the code should not compile.
	AutoCloser();
	AutoCloser(const AutoCloser&);
};

/**
 * I want to make sure a handle is not closed if it is invalid.
 * Unfortunately, I think there are different predefined invalid
 * values for different handle types.  I know variables of type
 * HANDLE can take on the value INVALID_HANDLE_VALUE.  I've also
 * seen NULL handles.  I think all handles ought to be comparable
 * to NULL, but I've seen errors trying to compare a non-HANDLE to
 * INVALID_HANDLE_VALUE.  The following template+specialization
 * basically maps a handle type to a special value representing
 * an invalid handle.  Using a template allows me to have a
 * "default" value for most types, rather than needing a full
 * set of overloads for all types, which results in shorter,
 * simpler code.
 */
template <typename _HandleType>
inline _HandleType specialInvalidValue() {
	return NULL;
}

template <>
inline HANDLE specialInvalidValue<HANDLE>() {
	return INVALID_HANDLE_VALUE;
}

// The specializations below have the same basic form... if I get too
// many of these, I'll factor the common stuff out into a base class,
// to simplify.

template<typename _HandleType>
class AutoCloser<_HandleType, BOOL(WINAPI&)(_HandleType)> : private Noncopyable {
public:
	typedef BOOL (WINAPI &CloserType)(_HandleType);

	AutoCloser(_HandleType handle, CloserType closer, const std::string &desc = "") :
	  handle(handle), closer(closer), desc(desc) {
	}

	~AutoCloser();

	_HandleType get() const {
		return handle;
	}

private:
	_HandleType handle;
	CloserType closer;
	std::string desc;
};

template<typename _HandleType>
class AutoCloser<_HandleType, LONG(WINAPI&)(_HandleType)> : private Noncopyable {
public:
	typedef LONG (WINAPI &CloserType)(_HandleType);

	AutoCloser(_HandleType handle, CloserType closer, const std::string &desc = "") :
	  handle(handle), closer(closer), desc(desc) {
	}

	~AutoCloser();

	_HandleType get() const {
		return handle;
	}

private:
	_HandleType handle;
	CloserType closer;
	std::string desc;
};

/** The dtor for BOOL(_HandleType) closers */
template<typename _HandleType>
AutoCloser<_HandleType, BOOL(WINAPI&)(_HandleType)>::~AutoCloser() {
	if (handle == NULL || handle == specialInvalidValue<_HandleType>())
		return;

	BOOL result = closer(handle);
	if (!result) {
		if (desc.empty())
			Log::Message("Couldn't close handle: " + 
				WindowsCommon::GetErrorMessage(GetLastError()));
		else
			Log::Message("Couldn't close handle ("+desc+"): " +
				WindowsCommon::GetErrorMessage(GetLastError()));
	}
}

/** The dtor for LONG(_HandleType) closers */
template<typename _HandleType>
AutoCloser<_HandleType, LONG(WINAPI&)(_HandleType)>::~AutoCloser() {
	if (handle == NULL || handle == specialInvalidValue<_HandleType>())
		return;

	LONG err = closer(handle);
	if (err != ERROR_SUCCESS) {
		if (desc.empty())
			Log::Message("Couldn't close handle: " + 
				WindowsCommon::GetErrorMessage(err));
		else
			Log::Message("Couldn't close handle ("+desc+"): " +
				WindowsCommon::GetErrorMessage(err));
	}
}

#endif
