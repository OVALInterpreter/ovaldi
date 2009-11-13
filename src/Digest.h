//
//
//****************************************************************************************//
// Copyright (c) 2002-2009, The MITRE Corporation
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
#ifndef DIGEST_H
#define DIGEST_H

#include "Exception.h"
#include <string>
#include <iostream>
#include <openssl/evp.h>

/**
 * Encapsulates an openssl message digest context.  Instances of this
 * class compute digests on any sort of data.
 */
class Digest
{
public:
	enum DigestType {
		MD5,
		SHA1
	};

	Digest();
	virtual ~Digest(void);

	/**
	 * Digests data from the given stream and returns the digest value
	 * as a hex string.
	 */
	std::string digest(std::istream &in, DigestType digestType);

	/**
	 * Convenience function for generating a digest of the given file.
	 */
	std::string digest(const std::string& fileName, DigestType digestType);

private:
	const EVP_MD* getDigest(DigestType digestType);

	EVP_MD_CTX context;
};

class DigestException : public Exception {
public:
	DigestException(std::string errMsgIn = "", int severity = ERROR_FATAL, Exception *ex = NULL);
	~DigestException();
};


#endif
