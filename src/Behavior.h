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

#ifndef BEHAVIOR_H
#define BEHAVIOR_H

#include <vector>
#include <string>
#include <xercesc/dom/DOMElement.hpp>

class Behavior;

/**
	A vector for storing Behavior objects. 
	Stores only pointers to the objects. 
*/
typedef std::vector < Behavior* > BehaviorVector;

/**
	This class represents a behavior in the oval definition schema
	Behaviors are used by objects to refine data collection.
*/
class Behavior {
public:
	/** Create a complete Behavior object. */
	Behavior(std::string name = "", std::string value = "");
	~Behavior();

	/** Parses the set of behaviors specified on an object and returns then as a vector. */ 
	static BehaviorVector* Parse(xercesc::DOMElement* behaviorsElement);

	/** Get the value of a named behavior in the specified set of behaviors. */
	static std::string GetBehaviorValue(BehaviorVector* behaviors, std::string name);

	std::string GetName();
	void SetName(std::string name);

	std::string GetValue();
	void SetValue(std::string value);

private:
	std::string name;
	std::string value;
};

#endif
