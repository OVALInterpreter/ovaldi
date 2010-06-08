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
#include "Digest.h"

#include<iomanip>
#include<fstream>
#include<sstream>
#include "Common.h"
#include "Log.h"

#if defined WIN32
#  include "WindowsCommon.h"
#else
// this #define makes sure we're using an up-to-date API
#  define GCRYPT_NO_DEPRECATED
#  include <gcrypt.h>
#endif

#if defined SUNOS
// for backward compatibility... I want to use a non-deprecated API
// where possible and at the same time not have to create another 
// dupe of all the code just for solaris.  So I code to a non-
// deprecated API and then add these special bandaids for solaris...
#  define gcry_md_hd_t GcryMDHd
#  define gcry_error_t int
#endif

namespace {
	/** implementation detail: a platform-specific digest context */
	struct DigestContext {
#if defined WIN32
		HCRYPTHASH hHash;
		DWORD digestLength;
#else
		gcry_md_hd_t hd;
		size_t digestLength;
#endif
	};
}

using namespace std;

#if !defined WIN32

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

#endif

Digest::Digest() {
#if defined WIN32
	if (!CryptAcquireContext(&this->hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_SILENT))
		throw DigestException("Error acquiring crypto context: " + WindowsCommon::GetErrorMessage(GetLastError()));
#else
	if (!Digest::IsInitialized)
		Digest::Initialize();
#endif
}

Digest::~Digest(void) {
#if defined WIN32
	if (!CryptReleaseContext(this->hProv, 0))
		// log, but don't throw... this seems unlikely...
		Log::Info("Error releasing crypto context: " + WindowsCommon::GetErrorMessage(GetLastError()));
#endif
}

void Digest::initDigest(void **context, DigestType digestType) {
#if defined WIN32

	ALG_ID algId = getDigest(digestType);
	HCRYPTHASH hHash;
	DWORD digestLength; // number of bytes in the resulting hash value
	DWORD notneeded = sizeof(digestLength); // sigh

	if (!CryptCreateHash(this->hProv, algId, 0, 0, &hHash))
		throw DigestException("Error initializing hash: " + WindowsCommon::GetErrorMessage(GetLastError()));

	if (!CryptGetHashParam(hHash, HP_HASHSIZE, (BYTE*)&digestLength, &notneeded, 0))
		throw DigestException("Could not determine size of hash: " + WindowsCommon::GetErrorMessage(GetLastError()));

	DigestContext *ctx = new DigestContext();
	if (!ctx) throw DigestException("Couldn't allocate memory for digest context");
	ctx->hHash = hHash;
	ctx->digestLength = digestLength;
	*context = ctx;

#else

	int algId = getDigest(digestType);
	gcry_md_hd_t hd; // hd = message digest handle
	unsigned int digestLength = gcry_md_get_algo_dlen(algId);

#if defined SUNOS
	hd = gcry_md_open(algId, 0);
#else
	gcry_error_t err;
	if (err = gcry_md_open(&hd, algId, 0))
		throw DigestException(string("Error opening message digest: ")+gcry_strerror(err));
#endif

	DigestContext *ctx = new DigestContext();
	if (!ctx) throw DigestException("Couldn't allocate memory for digest context");
	ctx->hd = hd;
	ctx->digestLength = digestLength;
	*context = ctx;

#endif
}

void Digest::updateDigest(void *context, void *buf, size_t bufSize) {
	DigestContext *ctx = (DigestContext*)context;

#if defined WIN32
	if (!CryptHashData(ctx->hHash, (BYTE*)buf, bufSize, 0))
		throw DigestException("Error computing hash: " + WindowsCommon::GetErrorMessage(GetLastError()));
#else
	gcry_md_write(ctx->hd, buf, bufSize);
#endif
}

string Digest::getDigestResults(void *context) {
	DigestContext *ctx = (DigestContext*)context;
	unsigned char *buf;

#if defined WIN32
	DWORD bufSize = ctx->digestLength;
	buf = new unsigned char[bufSize];
	if (!buf) throw DigestException("Couldn't allocate memory for digest results");

	if (!CryptGetHashParam(ctx->hHash, HP_HASHVAL, buf, &bufSize, 0))
	{
		delete[] buf;
		throw DigestException("Error computing hash: " + WindowsCommon::GetErrorMessage(GetLastError()));
	}
#else
	size_t bufSize = ctx->digestLength;
	buf = gcry_md_read(ctx->hd, 0);
#endif

	string digestStr = this->hashBytesToString(buf, bufSize);

#if defined WIN32
	delete[] buf;
#endif

	return digestStr;
}

void Digest::freeDigest(void *context) {
	DigestContext *ctx = (DigestContext*)context;

#if defined WIN32
	if (!CryptDestroyHash(ctx->hHash))
		Log::Info("Couldn't destroy hash object: " + WindowsCommon::GetErrorMessage(GetLastError()));
#else
	gcry_md_close(ctx->hd);
#endif

	delete ctx;
}

Digest::AlgType Digest::getDigest(DigestType digestType) {
	switch(digestType) {
#if defined WIN32
		case MD5:
			return CALG_MD5;
		case SHA1:
			return CALG_SHA1;
#else
		case MD5:
			return GCRY_MD_MD5;
		case SHA1:
			return GCRY_MD_SHA1;
#endif
		default:
			throw DigestException(string("Unknown digest type (")+Common::ToString(digestType)+")");
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


