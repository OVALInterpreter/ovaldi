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

#ifndef COMPONENTVALUE_H
#define COMPONENTVALUE_H

#include "OvalEnum.h"
#include "StdTypedefs.h"

/**
	This class represents a the value of a variable component.
	Component values are used in when computing the values of Variables.
	Each Component has a Vector of string values and a flag used to indicate 
	the status of the component.
*/
class ComponentValue {
public:
	/** Create a complete ComponentValue object */
	ComponentValue(OvalEnum::Flag flag = OvalEnum::FLAG_ERROR, StringVector* value = new StringVector(), StringVector* msgs = new StringVector());
	/** make sure the vectors are deleted. */
	~ComponentValue();

	StringVector* GetValues();
	void SetValues(StringVector* values);
	void AppendValue(std::string value);

	OvalEnum::Flag GetFlag();
	void SetFlag(OvalEnum::Flag flag);

	StringVector* GetMessages();
	void SetMessages(StringVector* msgs);
	void AppendMessage(std::string msg);
	void AppendMessages(StringVector* newMsgs);


private:
	OvalEnum::Flag flag;
	StringVector* values;
	StringVector* msgs;

};

typedef std::vector < ComponentValue* > ComponentValueVector;

#endif
