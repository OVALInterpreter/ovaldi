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

#ifndef ENTITYCOMPARATOR_H
#define ENTITYCOMPARATOR_H

#include "OvalEnum.h"
#include "AbsEntityValue.h"

/**
	This class is a container for all comparison methods between definition entities and item entities.
*/
class EntityComparator {
public:

	/** Compare two binary values based on the specified operation 
        Binary values must match the following regex "[0-9a-fA-F]"
    */
	static OvalEnum::ResultEnumeration CompareBinary(OvalEnum::Operation op, std::string defValue, std::string scValue);

	/** Compare two boolean values based on the specified operation.
	    May have the following input values: {true, false, 1, 0)
	*/
	static OvalEnum::ResultEnumeration CompareBoolean(OvalEnum::Operation op, std::string defValue, std::string scValue);
	
	/** Compare two evr strings based on the specified operation. 
		This code motivated (strongly) by librpm's rpmdsCompare().
	*/
	static OvalEnum::ResultEnumeration CompareEvrString(OvalEnum::Operation, std::string defValue, std::string scValue);

	/** 
     * Compare to floats based on the specified operation. 
     * 
     * TODO - need to align with W3C notion of a float.
     * This datatype conforms to the W3C Recommendation for float data meaning it is patterned after the IEEE 
     * single-precision 32-bit floating point type.  The format consists of a decimal followed, optionally, 
     * by the character 'E' or 'e', followed by an integer exponent.  The special values positive and negative
     * infinity and not-a-number have are represented by INF, -INF and NaN, respectively.
     */
	static OvalEnum::ResultEnumeration CompareFloat(OvalEnum::Operation op, std::string defValue, std::string scValue);

	/** Compare to integers based on the specified operation. 
	 *	Since these integers come from an xsd:integer the code needs to allow for very large integers.
     *	The range of allowed integers is: -9223372036854775808 to 9223372036854775807.
	*/
	static OvalEnum::ResultEnumeration CompareInteger(OvalEnum::Operation op, std::string defValue, std::string scValue);

	/** Compare two float values based on the specified operation. */
	static OvalEnum::ResultEnumeration CompareIosVersion(OvalEnum::Operation op, std::string defValue, std::string scValue);

	/** Compare two record values based on the specified operation. */
	static OvalEnum::ResultEnumeration CompareRecord(OvalEnum::Operation op, AbsEntityValueVector defValue, AbsEntityValueVector scValue);

	/** Compare two string values based on the specified operation. */
	static OvalEnum::ResultEnumeration CompareString(OvalEnum::Operation op, std::string defValue, std::string scValue);

	/** Compare two version strings based on the input operation. 
		@return Returns the result of the comparison.
	*/
	static OvalEnum::ResultEnumeration CompareVersion(OvalEnum::Operation op, std::string defValue, std::string scValue);

	/** Compare two ipv4_address strings based on the input operation. 
		@return Returns the result of the comparison.
	*/
	static OvalEnum::ResultEnumeration CompareIpv4Address(OvalEnum::Operation op, std::string defValue, std::string scValue);
	/** Compare two ipv6_address strings based on the input operation. 
		@return Returns the result of the comparison.
	*/
	static OvalEnum::ResultEnumeration CompareIpv6Address(OvalEnum::Operation op, std::string defValue, std::string scValue);
private:

	/** 
		Parse the provided string into a vector of integers using the 
		specified delmiter to split the stirng.
		If no delimiter is found convert the version string to an integer and return it as
		the only component of the version. 
		If a given component of the version string can not be converted to an integer an error is thrown.
		If a delimiter is found but no integer components are found an error is thrown.		
	*/
	static LongLongVector* ParseVersionStr(std::string versionStr);

	// copied from lib/rpmvercmp.c
	static int rpmvercmp(const char * a, const char * b);
	// copied from rpmio/rpmio.h
	static int xisalnum(int c);
	static int xisdigit(int c);
	static int xisalpha(int c);
	static int xislower(int c);
	static int xisupper(int c);

	static std::string GetEpochFromEVR(std::string evrStr);
	static std::string GetVersionFromEVR(std::string evrStr);
	static std::string GetReleaseFromEVR(std::string evrStr);

};

#endif
