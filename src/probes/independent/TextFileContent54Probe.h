//
//
//****************************************************************************************//
// Copyright (c) 2002-2011, The MITRE Corporation
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

#ifndef TEXTFILECONTENT54PROBE_H
#define TEXTFILECONTENT54PROBE_H

#include "AbsProbe.h"


/**
	This class is responsible for collecting data for the ind-sc:textfilecontent_item defined OVAL System Characteristics Schema.
	This class should be platform independent.
*/
class TextFileContent54Probe : public AbsProbe {

public:
	virtual ~TextFileContent54Probe();

	virtual ItemVector* CollectItems(Object* object);

	/** Gets single instance of the TextFileContent54Probe. Uses lazy initialization. */
	static AbsProbe* Instance();

private:
	TextFileContent54Probe();

	/**  Return a new Item created for storing text file content information */
	virtual Item* CreateItem();

	/** The static Singleton instance of the TextFileContent54FileProbe. */
	static TextFileContent54Probe* instance;

	/**
	 * Gets all matches of the given pattern(s) from the given file, and appends
	 * corresponding items onto the given vector.
	 */
	void GetItems(std::string path, std::string fileName,
				ObjectEntity *patternEntity,
				ObjectEntity *instanceEntity,
				int matchOptions,
				ItemVector* collectedItems);

	/**
	 * Gets all matches of the given pattern(s) from the given file contents,
	 * and appends corresponding items onto the given vector.
	 */
	void GetMatches(const std::string& path,
					const std::string& fileName,
					const std::string& fileContents,
					ObjectEntity *patternEntity,
					ObjectEntity *instanceEntity,
					int matchOptions,
					ItemVector *collectedItems);

	/**
	 * Translates the given behaviors to match flags to tell
	 * the REGEX class how we want to do matching.
	 */
	int Behaviors2MatchOptions(BehaviorVector *behaviors);

	REGEX re;
};

#endif
