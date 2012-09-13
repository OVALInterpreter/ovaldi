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
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT using namespace std;


// SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
// OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
// TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//****************************************************************************************//

#ifdef WIN32
#  include <FsRedirectionGuard.h>
#  include <PrivilegeGuard.h>
// macro this so it can disappear on non-windows OSs.
#  define ADD_WINDOWS_VIEW_ENTITY \
	item->AppendElement(new ItemEntity("windows_view", \
		(fileFinder.GetView() == BIT_32 ? "32_bit" : "64_bit")));
#else
#  define ADD_WINDOWS_VIEW_ENTITY
#  define FS_REDIRECT_GUARD_BEGIN(x)
#  define FS_REDIRECT_GUARD_END
#endif

#include "TextFileContentProbe.h"

using namespace std;

//****************************************************************************************//
//								TextFileContentProbe Class								  //	
//****************************************************************************************//
TextFileContentProbe* TextFileContentProbe::instance = NULL;

TextFileContentProbe::TextFileContentProbe() {

}

TextFileContentProbe::~TextFileContentProbe() {
  instance = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* TextFileContentProbe::Instance() {

	// Use lazy initialization
	if(instance == NULL) 
		instance = new TextFileContentProbe();

	return instance;	
}

ItemVector* TextFileContentProbe::CollectItems(Object* object) {


	// get the path and file name
	ObjectEntity* path = object->GetElementByName("path");
	ObjectEntity* fileName = object->GetElementByName("filename");
    ObjectEntity* filePath = object->GetElementByName("filepath");

    if(filePath != NULL)
        throw ProbeException("The filepath entity is not currently supported.");

	ObjectEntity* line = object->GetElementByName("line");

	if (line->GetDatatype() != OvalEnum::DATATYPE_STRING) {
		throw ProbeException("Error: invalid datatype specified on line. Found: " + OvalEnum::DatatypeToString(line->GetDatatype()));
	}
	if (line->GetOperation() != OvalEnum::OPERATION_PATTERN_MATCH) {
		throw ProbeException("Error: invalid operation specified on line. Found: " + OvalEnum::OperationToString(line->GetOperation()));
	}

	ItemVector *collectedItems = new ItemVector();

#ifdef WIN32
	FileFinder fileFinder(WindowsCommon::behavior2view(object->GetBehaviors()));
#else
	FileFinder fileFinder;
#endif

	StringPairVector *filePaths;
#ifdef WIN32
	{
		PrivilegeGuard pg(SE_BACKUP_NAME, false);
#endif
		filePaths = fileFinder.SearchFiles(path, fileName, object->GetBehaviors());

#ifdef WIN32
	}
#endif

	if(filePaths->size() > 0) {
		// Loop through all file paths
		StringPairVector::iterator iterator;
		for(iterator = filePaths->begin(); iterator != filePaths->end(); iterator++) {

			StringPair* fp = (*iterator);

			if(fp->second.compare("") == 0) {

				Item* item = NULL;

				// check if the code should report that the filename does not exist.
				StringVector fileNames;
				if(fileFinder.ReportFileNameDoesNotExist(fp->first, fileName, &fileNames)) {
					StringVector::iterator iterator;
					for(iterator = fileNames.begin(); iterator != fileNames.end(); iterator++) {

						item = this->CreateItem();
						item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
						item->AppendElement(new ItemEntity("path", fp->first, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
						item->AppendElement(new ItemEntity("filename", (*iterator), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));
						ADD_WINDOWS_VIEW_ENTITY
						collectedItems->push_back(item);
					}
					
				} else {

					item = this->CreateItem();
					item->SetStatus(OvalEnum::STATUS_EXISTS);
					item->AppendElement(new ItemEntity("path", fp->first, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
					ADD_WINDOWS_VIEW_ENTITY
					collectedItems->push_back(item);

				}

			} else {

				FS_REDIRECT_GUARD_BEGIN(fileFinder.GetView())
				this->GetLines(fp->first, fp->second, line, collectedItems, fileFinder);
				FS_REDIRECT_GUARD_END
			}

			delete fp;
		}

	} else {

		// if no filepaths check if the code should report that the path does not exist
		StringVector paths;
		if(fileFinder.ReportPathDoesNotExist(path, &paths)) {

			Item* item = NULL;
			StringVector::iterator iterator;
			for(iterator = paths.begin(); iterator != paths.end(); iterator++) {

				item = this->CreateItem();
				item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
				item->AppendElement(new ItemEntity("path", (*iterator), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));
				ADD_WINDOWS_VIEW_ENTITY
				collectedItems->push_back(item);
			}
		}
	}
	delete filePaths;

	return collectedItems;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
Item* TextFileContentProbe::CreateItem() {

	Item* item = new Item(0, 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#independent", 
						"ind-sc", 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#independent independent-system-characteristics-schema.xsd", 
						OvalEnum::STATUS_ERROR, 
						"textfilecontent_item");

	return item;
}

void TextFileContentProbe::GetLines(string path, string fileName, 
	ObjectEntity* line, ItemVector* collectedItems, FileFinder &fileFinder) {

	Item *item = NULL;

	// construct the file path
	string filePath = Common::BuildFilePath((const string)path, (const string)fileName);

	// read the file line by line
	string buffer;
	ifstream infile;
	infile.open (filePath.c_str());
	if (infile.is_open()) {

		while (!infile.eof()) {
			
			StringVector* results = new StringVector();
			
			// call the GetSubstrings method	
			getline (infile, buffer);
			if (this->GetSubstrings (buffer, line, results)) {

				// create one item if there were any matching substrings
				item = this->CreateItem();
				collectedItems->push_back(item);
				item->SetStatus(OvalEnum::STATUS_EXISTS);
				item->AppendElement(new ItemEntity("path", path, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
				item->AppendElement(new ItemEntity("filename", fileName, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
				item->AppendElement(new ItemEntity("line", buffer, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));

				StringVector::iterator iterator;
				for (iterator = results->begin(); iterator != results->end(); iterator++) {
					// add a line for each matching subexpression
					item->AppendElement(new ItemEntity("subexpression", (*iterator), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));					
				}

				ADD_WINDOWS_VIEW_ENTITY
			} 
		}
		infile.close();
	}
}

bool TextFileContentProbe::GetSubstrings(string lineFromFile, ObjectEntity* line, StringVector* substrings) {

	bool retVal = false;
	string pattern = line->GetValue();

	if (this->myMatcher->GetMatchingSubstrings(pattern.c_str(), lineFromFile.c_str(), substrings) ) {
		retVal = true;
	}

	return retVal;
}
