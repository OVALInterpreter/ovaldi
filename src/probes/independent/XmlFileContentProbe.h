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

#ifndef XMLFILECONTENTPROBE_H
#define XMLFILECONTENTPROBE_H

#include "AbsProbe.h"

#include <string>

/**
	This class encapsulates common functionality for the XmlFileContentProbe. 
*/
class XmlFileContentProbe : public AbsProbe {
public:
	virtual ~XmlFileContentProbe();

	/**
	    Use a platform specifc FileFinder to find all files that match the 
	    specified object. Then evaulate xpath expression on each matching file and
	    create an Item to hold the resulting data.
	    
	    Note 1: xpath expressions are treated as though they always evaluate to 
	    a single node.
	    
	    Note 2: regular expressions are not allowed as part of an xpath
	    
	    Note 3: this probe should allow for a context node to be provided
	    for now the context node will default to '/' (document root)
	*/
	virtual ItemVector* CollectItems(Object* object);

	/** Gets single instance of the XmlFileContentProbe. Uses lazy initialization. */
	static AbsProbe* Instance();

private:

	XmlFileContentProbe();

	/** Return a new Item created for storing xml file content information. */
	virtual Item* CreateItem();

	/** The static Singleton instance of the XmlFileContentProbe. */
	static XmlFileContentProbe* instance;

	/** Return an Item for the specified xpath if is succeeds otherwise return NULL. */
	Item* EvaluateXpath(std::string path, std::string fileName, std::string xpath);
};

#endif

