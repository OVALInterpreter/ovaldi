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

#ifndef OVALMESSAGE_H
#define OVALMESSAGE_H

#include <string>
#include <vector>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMElement.hpp>

#include "OvalEnum.h"

/**
	This class represents an OvalMessage in the oval schema
*/
class OvalMessage  {
public:
	/** Create a complete OvalMessage object. */
	OvalMessage(std::string value = "", OvalEnum::Level level = OvalEnum::LEVEL_INFO);
    
    /** OvalMessage copy constructor. */
    OvalMessage(const OvalMessage& message);

	~OvalMessage();

	/**
	 * Write this message to the specified document.
	 * These messages can go into either results or sc documents,
	 * and each has its own &lt;message&gt; element.
	 * So callers must tell us which kind of element to create
	 * (which namespace to use) and which qname prefix.
	 */
	void Write(xercesc::DOMDocument* scFile, xercesc::DOMElement* itemElm, std::string prefix, std::string ns);
	/** Parse the supplied message element into a message object. */
	void Parse(xercesc::DOMElement* msgElm);

	/** Return the value field's value. */
	std::string GetValue();
	/** Set the value field's value. */
	void SetValue(std::string value);

	/** Return the Level field's value. */
	OvalEnum::Level GetLevel();
	/** Set the Level field's value. */
	void SetLevel(OvalEnum::Level level);

	/** Make string representation of object. */
	std::string ToString();

private:
	std::string value;
	OvalEnum::Level level;
};

/**
	A vector for storing OvalMessage objects. 
	Stores only pointers to the objects. 
*/
typedef std::vector < OvalMessage* > OvalMessageVector;

#endif
