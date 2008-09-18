//
// $Id: EntityComparator.h 4663 2008-01-23 13:58:23Z bakerj $
//
//****************************************************************************************//
// Copyright (c) 2002-2008, The MITRE Corporation
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

#ifndef ENTITYCOMPARATOR_H
#define ENTITYCOMPARATOR_H

#include "OvalEnum.h"
#include "REGEX.h"
#include "Common.h"

using namespace std;

/**
	This class is a container for all comparison methods between definition entities and item entities.
*/
class EntityComparator {
public:

	/** Compare two binary values based on the specified operation */
	static OvalEnum::ResultEnumeration CompareBinary(OvalEnum::Operation op, string defValue, string scValue);

	/** Compare two boolean values based on the specified operation.
	    May have the following input values: {true, false, 1, 0)
	*/
	static OvalEnum::ResultEnumeration CompareBoolean(OvalEnum::Operation op, string defValue, string scValue);
	
	/** Compare two evr strings based on the specified operation. 
		This code motivated (strongly) by librpm's rpmdsCompare().
	*/
	static OvalEnum::ResultEnumeration CompareEvrString(OvalEnum::Operation, string defValue, string scValue);

	/** Compare to floats based on the specified operation. */
	static OvalEnum::ResultEnumeration CompareFloat(OvalEnum::Operation op, string defValue, string scValue);

	/** Compare to integers based on the specified operation. 
		Since these integers come from an xsd:integer the code needs to allow for very large integers.
		This code allows for strings with up to 20 characters. The range of allowed integers is:
		-9,999,999,999,999,999,999 to 99,999,999,999,999,999,999
	*/
	static OvalEnum::ResultEnumeration CompareInteger(OvalEnum::Operation op, string defValue, string scValue);

	/** Compare two float values based on the specified operation. */
	static OvalEnum::ResultEnumeration CompareIosVersion(OvalEnum::Operation op, string defValue, string scValue);

	/** Compare two string values based on the specified operation. */
	static OvalEnum::ResultEnumeration CompareString(OvalEnum::Operation op, string defValue, string scValue);

	/** Compare two version strings based on the input operation. 
		@return Returns the result of the comparison.
	*/
	static OvalEnum::ResultEnumeration CompareVersion(OvalEnum::Operation op, string defValue, string scValue);

private:

	/** 
		Parse the provided string into a vector of integers using the 
		specified delmiter to split the stirng.
		If no delimiter is found convert the version string to an integer and return it as
		the only component of the version. 
		If a given component of the version string can not be converted to an integer an error is thrown.
		If a delimiter is found but no integer components are found an error is thrown.		
	*/
	static IntVector* ParseVersionStr(string versionStr);

	// copied from lib/rpmvercmp.c
	static int rpmvercmp(const char * a, const char * b);
	// copied from rpmio/rpmio.h
	static int xisalnum(int c);
	static int xisdigit(int c);
	static int xisalpha(int c);
	static int xislower(int c);
	static int xisupper(int c);

	static string GetEpochFromEVR(string evrStr);
	static string GetVersionFromEVR(string evrStr);
	static string GetReleaseFromEVR(string evrStr);

	static string ToUpper(string s);
};

#endif
