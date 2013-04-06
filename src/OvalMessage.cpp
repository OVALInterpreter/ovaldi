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

#include "XmlCommon.h"

#include "OvalMessage.h"

using namespace std;
using namespace xercesc;

//****************************************************************************************//
//								OvalMessage Class										  //	
//****************************************************************************************//
OvalMessage::OvalMessage(string value, OvalEnum::Level level) {

	this->SetValue(value);
	this->SetLevel(level);
}

OvalMessage::OvalMessage(const OvalMessage& message ){
    this->SetValue(message.value);
    this->SetLevel(message.level);
}

OvalMessage::~OvalMessage() {
}

// ***************************************************************************************	//
//								 Public members												//
// ***************************************************************************************	//
string OvalMessage::GetValue() {

	return this->value;
}

void OvalMessage::SetValue(string value) {

	this->value = value;
}

OvalEnum::Level OvalMessage::GetLevel() {

	return this->level;
}

void OvalMessage::SetLevel(OvalEnum::Level level) {

	this->level = level;
}

string OvalMessage::ToString() {

	string msgStr = "";

	msgStr.append(OvalEnum::LevelToString(this->GetLevel()));
	msgStr.append(" - ");
	msgStr.append(this->GetValue());
	return msgStr;
}

void OvalMessage::Write(DOMDocument* doc, DOMElement* parentElm, string prefix, string ns) {

	// Create new item element
	string elementName = "message";
	if (!prefix.empty())
		elementName = prefix+':'+elementName;
	DOMElement* newOvalMessageElem = XmlCommon::CreateElementNS(doc, ns, elementName, this->GetValue().c_str());
	parentElm->appendChild(newOvalMessageElem);

	// Add the attributes
	// handling defaults in the schema
	string strLevel = OvalEnum::LevelToString(this->GetLevel());
	if(strLevel.compare("info") != 0)
		XmlCommon::AddAttribute(newOvalMessageElem, "level", strLevel);
}

void OvalMessage::Parse(DOMElement* msgElm) {

	this->SetValue(XmlCommon::GetDataNodeValue(msgElm));
	this->SetLevel(OvalEnum::ToLevel(XmlCommon::GetAttributeByName(msgElm, "level")));
}
