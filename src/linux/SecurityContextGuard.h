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

#ifndef SECURITYCONTEXTGUARD_H
#define SECURITYCONTEXTGUARD_H

#include <selinux/selinux.h>
#include <selinux/context.h>
#include <cerrno>
#include <string>
#include <Noncopyable.h>
#include <ErrnoException.h>

/**
 * Enables exception-safe use of selinux security_context_t values.
 */
class SecurityContextGuard : private Noncopyable {
public:
	explicit SecurityContextGuard(security_context_t ctx) : ctx(ctx) {
	}

	~SecurityContextGuard() {
		if (ctx)
			freecon(ctx);
	}

	operator security_context_t() {
		return ctx;
	}

private:
	security_context_t ctx;
};

/**
 * Enables exception-safe use of selinux context_t values.
 */
class ContextGuard : private Noncopyable {
public:
	explicit ContextGuard(context_t ctx) : ctx(ctx) {
	}

	explicit ContextGuard(security_context_t sCtx) : ctx(NULL) {
		ctx = context_new(sCtx);
		if (!ctx)
			throw ErrnoException("context_new("+std::string(sCtx)+")", errno);
	}

	~ContextGuard() {
		if (ctx)
			context_free(ctx);
	}

	operator context_t() {
		return ctx;
	}

private:
	context_t ctx;
};


#endif
