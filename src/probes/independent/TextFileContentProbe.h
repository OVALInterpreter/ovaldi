//
//
//****************************************************************************************//
// Copyright (c) 2002-2009, The MITRE Corporation
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

#ifndef TEXTFILECONTENTPROBE_H
#define TEXTFILECONTENTPROBE_H

#include "FileFinder.h"
#include "AbsProbe.h"
#include <fstream>

#ifdef WIN32
	#define SNPRINTF _snprintf
#else
	#define SNPRINTF snprintf
#endif

using namespace std;

/**
	This class is responsible for collecting data for the ind-sc:textfilecontent_item defined OVAL System Characteristics Schema.
	This class should be platform independent.

	--- SCHEMA BUG AND RELATED INTERPRETER BUG ---
	There is a known issue with the schema definition of this object in the oval-definitions-schema and corresponding
	item in the oval-system-characteristics-schema. There are three key issues and a pending feature request related to
	this object. Each is described below:

	- Feature Request - Allow matching to occur over multiple lines. When checking system configurations contained 
	in text files sections of the text file must frequently first be identified before looking for a particular 
	setting. A request was made to allow for regular expressions that would match multi lines at once.

	- BUG - If a pattern used in a textfilecontent_object identifies two identical lines in a file only one item will
	be written to the system characteristics file. This is due to the fact that all items must be unique. The only way 
	to uniquely identify a line in a text file is line number and/or character position. One possible fix to this bug would
	be to add a position element to the textfilecontent_object. This solution would allow lines to be uniquely identified.

	- BUG - If 2 different textfilecontent_objects identify the same file and the regular expressions identified by the 
	2 objects' line entity identify overlapping lines only the first set of subexpressions for the overlapping line matches
	will be recorded in the item. This is due to the fact that collection efforts for one regex are not currently seperable
	from collection efforts for another regex.

	- BUG - If a textfilecontent_object has a line regular expression which identifies more than one substring in a line
	it is not possible to know which resulting substrings in the item correspond with which subexpression in the line regex. 
	When one or more of the subexpressions in the line regex do not match any substring it is not possible to convey that
	no substring was matched for a specific subexpression.

	Below is a proposed change to the textfilecontent object and state:
	<textfilecontent_object ...>
      <path>C:\mydir</path>
      <filename>myfile.txt</filename>
      <position>6:0</position><!-- line:char index possibly to include length too -->
      <pattern>(ol).+(ak)\.\n(ok).</pattern><!-- defined in schema to be a regular expression only that identifies one substring -->
	</textfilecontent_object>
	<textfilecontent_state ...>
      <substring>ol</substring><!-- one substring -->
    </textfilecontent_state>

*/
class TextFileContentProbe : public AbsProbe {

public:
	~TextFileContentProbe();
	
	ItemVector* CollectItems(Object* object);

	/** Gets single instance of the TextFileContentProbe. Uses lazy initialization. */
	static AbsProbe* Instance();

private:
	TextFileContentProbe();

	/**  Return a new Item created for storing text file content information */
	Item* CreateItem();

	/** The static Singleton instance of the TextFileContentFileProbe. */
	static TextFileContentProbe* instance;

	/** Return true if the specified line matches the criteria in the ObjectEntity. 
		If true any resulting substrings are returned on the substrings vector. 
	*/
	bool GetSubstrings(string lineFromFile, ObjectEntity* line, StringVector* substrings);

	/** Read the specified file one line at time if any matching line is found with matching 
		subexpressions create a new item and add it topt he set of collectedItems. If none 
		are found the set is returned empty. Calls out to the GetSubstrings function to 
		determine if a given line in a file matches the criteria specified in the ObjectEntity. 
	*/
	void GetLines(string path, string fileName, ObjectEntity* line, ItemVector* collectedItems);
};

#endif
