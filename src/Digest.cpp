//
//
//****************************************************************************************//
// Copyright (c) 2002-2010, The MITRE Corporation
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

// this #define makes sure we're using an up-to-date API
#define GCRYPT_NO_DEPRECATED

// Important: libgcrypt uses winsock2, and unless we include winsock2 first, we
// will indirectly include winsock1 via subsequent headers, before including
// winsock2, causing a header clash.  (I guess winsock1 knows to not include if
// winsock2 is already included, but not vice versa.)  Therefore, gcrypt.h MUST
// be included before any ovaldi headers.
#include <gcrypt.h>

#include "Digest.h"

#include<iomanip>
#include<fstream>
#include<sstream>
#include "Common.h"
#include "Log.h"

#if defined SUNOS
// for backward compatibility... I want to use a non-deprecated API
// where possible and at the same time not have to create another 
// dupe of all the code just for solaris.  So I code to a non-
// deprecated API and then add these special bandaids for solaris...
#  define gcry_md_hd_t GcryMDHd
#  define gcry_error_t int
#endif

namespace {
	/**
	 * a platform-specific digest context.
	 * (Actually all supported platforms now use libgcrypt.)
	 */
	struct DigestContext {
		gcry_md_hd_t hd;
		size_t digestLength;
	};
}

using namespace std;

bool Digest::IsInitialized = false;

void Digest::Initialize() {
	gcry_error_t err;

	// on solaris 10 we have to make do with an old version... on
	// other platforms, let's require 1.4+ (that was the
	// latest RHEL version at the time of this writing)
	const char *reqVersion;
	const char *actualVersion;

#if defined SUNOS
	reqVersion = "1.1.0";
#else
	reqVersion = "1.4.0";
#endif

	actualVersion = gcry_check_version(reqVersion);
	if (!actualVersion)
		throw DigestException("libgcrypt library mismatch: version " + string(reqVersion) +
							  "+ required");

	Log::Debug(string("Found libgcrypt version: ") + actualVersion);

	err = gcry_control(GCRYCTL_INITIALIZATION_FINISHED, 0);

	if (err)
		throw DigestException(string("libgcrypt initialization error: ")+gcry_strerror(err));

	Digest::IsInitialized = true;
}

Digest::Digest() {
	if (!Digest::IsInitialized)
		Digest::Initialize();
}

Digest::~Digest(void) {
}

void Digest::initDigest(void **context, DigestType digestType) {
	int algId = getDigest(digestType);
	gcry_md_hd_t hd; // hd = message digest handle
	unsigned int digestLength = gcry_md_get_algo_dlen(algId);

#if defined SUNOS
	hd = gcry_md_open(algId, 0);
	if (!hd)
		throw DigestException("Error opening message digest ("+Common::ToString(digestType)+")");
#else
	gcry_error_t err;
	if ((err = gcry_md_open(&hd, algId, 0)))
		throw DigestException(string("Error opening message digest: ")+gcry_strerror(err));
#endif

	DigestContext *ctx = new DigestContext();
	if (!ctx) throw DigestException("Couldn't allocate memory for digest context");
	ctx->hd = hd;
	ctx->digestLength = digestLength;
	*context = ctx;
}

void Digest::updateDigest(void *context, void *buf, size_t bufSize) {
	DigestContext *ctx = (DigestContext*)context;

	gcry_md_write(ctx->hd, buf, bufSize);
}

string Digest::getDigestResults(void *context) {
	DigestContext *ctx = (DigestContext*)context;
	unsigned char *buf;

	size_t bufSize = ctx->digestLength;
	buf = gcry_md_read(ctx->hd, 0);

	string digestStr = this->hashBytesToString(buf, bufSize);

	return digestStr;
}

void Digest::freeDigest(void *context) {
	DigestContext *ctx = (DigestContext*)context;

	gcry_md_close(ctx->hd);

	delete ctx;
}

int Digest::getDigest(DigestType digestType) {
	switch(digestType) {
		case MD5:
			return GCRY_MD_MD5;
		case SHA1:
			return GCRY_MD_SHA1;
// libgcrypt which comes with solaris 10 doesn't define
// a constant for sha-224.
#if not defined SUNOS
		case SHA224:
			return GCRY_MD_SHA224;
#endif
		case SHA256:
			return GCRY_MD_SHA256;
		case SHA384:
			return GCRY_MD_SHA384;
		case SHA512:
			return GCRY_MD_SHA512;
		default:
			throw DigestException(string("Unknown or unsupported digest type (")+Common::ToString(digestType)+")");
	}
}

string Digest::digest(istream &in, DigestType digestType) {
	void *context;
	const size_t bufSize = 1000; //our slurp size
	char buf[bufSize];

	initDigest(&context, digestType);

	while (in) {
		in.read(buf, bufSize);
		if (in.gcount() > 0)
			updateDigest(context, buf, in.gcount());
	}

	string digestStr = getDigestResults(context);
	freeDigest(context);

	return digestStr;
}

std::string Digest::hashBytesToString(unsigned char *bytes, size_t numBytes) {
	ostringstream sstr;
	sstr << setfill('0') << hex;
	for (size_t i = 0; i<numBytes; ++i)
		// without cast, this would interpret the val as a character code and
		// print a character!
		sstr << setw(2) << (unsigned int)bytes[i]; 

	return sstr.str();
}

string Digest::digest(const string& fileName, DigestType digestType) {
	// open in binary so we are hashing every byte as-is, with no
	// eol translations.
	ifstream in(fileName.c_str(), ios::in|ios::binary);

	if (!in)
		throw DigestException("Could not open file: "+fileName);

	return this->digest(in, digestType);
}

DigestException::DigestException(string errMsgIn, int severity, Exception* ex) :
	Exception(errMsgIn, severity, ex) {

}

DigestException::~DigestException() {

}


