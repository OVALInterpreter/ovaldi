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

using namespace std;

Digest::Digest() {
	EVP_MD_CTX_init(&this->context);
}

Digest::~Digest(void) {
	EVP_MD_CTX_cleanup(&this->context);
}

string Digest::digest(istream& in, DigestType digestType) {
	const EVP_MD* theDigest = getDigest(digestType);

	if (!EVP_DigestInit_ex(&this->context, theDigest, NULL))
		throw DigestException("EVP_DigestInit_ex returned error");

	const int bufSize = 1000; //our slurp size
	char buf[bufSize];

	while (in) {
		in.read(buf, bufSize);
		if (in.gcount() > 0 && !EVP_DigestUpdate(&context, buf, in.gcount()))
			throw DigestException("EVP_DigestUpdate returned error");
	}

	unsigned char digestVal[EVP_MAX_MD_SIZE];
	unsigned int byteCount;
	if (!EVP_DigestFinal_ex(&this->context, digestVal, &byteCount))
		throw DigestException("EVP_DigestFinal_ex returned error");

	ostringstream sstr;
	sstr << setfill('0') << hex;
	for (unsigned int i = 0; i<byteCount; ++i)
		// without cast, this would interpret the val as a character code and
		// print a character!
		sstr << setw(2) << (unsigned int)digestVal[i]; 

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

const EVP_MD* Digest::getDigest(DigestType digestType) {
	switch(digestType) {
		case MD5:
			return EVP_md5();
		case SHA1:
			return EVP_sha1();
		default:
			throw DigestException(string("Unknown digest type (")+Common::ToString(digestType)+")");
	}
}

DigestException::DigestException(string errMsgIn, int severity, Exception* ex) :
	Exception(errMsgIn, severity, ex) {

}

DigestException::~DigestException() {

}

