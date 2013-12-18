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

#ifndef META_H
#define META_H

// to give us a wchar_t type, which we need
// since ovaldi is built without wchar_t being
// builtin type.  I tried to choose a small
// header that has the typedef....
#include <BaseTyps.h>

/**
 * \file
 * This header implements some metaprogramming tools we had a need for.
 * We bumped into the need to overload a function template based on whether
 * the argument was an unsigned integral type.  The latest C++ standard
 * library has an implementation of enable_if and other metaprogramming
 * tools which allow you to do that sort of thing, but ovaldi needs to
 * build using much older toolchains, so we have to assume it won't be
 * available.  So we need to write our own.  
 */

/**
 * Base template for a SFINAE mechanism to enable/disable template
 * overloads based on a boolean condition.
 */
template<bool, typename = void>
struct EnableIf {};

template<typename T>
struct EnableIf<true, T> {
	typedef T type;
};

/**
 * Base template for a SFINAE mechanism to enable/disable template
 * overloads based on a boolean condition.  This does the opposite
 * if EnableIf.  I thought it would be clearer that two overloads
 * should be enabled under exactly opposite conditions, if you
 * used EnableIf and DisableIf with the exact same condition.
 * But you could also just stick with EnableIf and use 
 * !(original_condition).
 */
template<bool, typename = void>
struct DisableIf {};

template<typename T>
struct DisableIf<false, T> {
	typedef T type;
};

/**
 * A way to determine whether a type is a character type
 * (unsigned char, signed char, or wchar_t).
 */
template<typename>
struct IsChar {
	static const bool value = false;
};

template<>
struct IsChar<signed char> {
	static const bool value = true;
};

template<>
struct IsChar<unsigned char> {
	static const bool value = true;
};

template<>
struct IsChar<wchar_t> {
	static const bool value = true;
};

#endif
