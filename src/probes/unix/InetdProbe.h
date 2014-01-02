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
#ifndef INETDPROBE_H
#define INETDPROBE_H

#include <AbsProbe.h>
#include <istream>
#include <Item.h>
#include <Object.h>
#include <ObjectEntity.h>
#include <REGEX.h>
#include <set>
#include <string>

/**
 * The probe for analyzing inetd config files.
 */
class InetdProbe : public AbsProbe {

	public:

	virtual ~InetdProbe();
	static InetdProbe* Instance();

	protected:

	virtual ItemVector* CollectItems(Object* object);
	virtual Item* CreateItem();

	private:

	InetdProbe();

	/**
	 * Gets the next "line" of the given stream, where here, a "line"
	 * is a complete configuration entry, including continuation lines.
	 */
	void NextVirtualLine(std::istream& in, std::string& line);

	/**
	 * Gets the next actual line of the given stream, which is not
	 * a comment or a blank line.
	 */
	void NextLine(std::istream& in, std::string& line);

	/**
	 * Breaks up the given line into pieces and creates an item.  If
	 * one could not be created, e.g. because it didn't match the object,
	 * or the line was malformed, NULL is returned.
	 */
	Item* Line2Item(const std::string& line, ObjectEntity *nameEntity, ObjectEntity* protocolEntity);

	/** Stores our read-ahead line */
	std::string tmpLine;

	std::set<std::string> allowedEndpointTypes;
	std::set<std::string> allowedWaitValues;

	REGEX re;
	static InetdProbe* instance;
};

#endif
