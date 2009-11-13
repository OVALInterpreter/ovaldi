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

#include "FileHashProbe.h"
#include <fstream>
#include <iomanip>
#include <sstream>

//****************************************************************************************//
//								FileHashProbe Class										  //	
//****************************************************************************************//
FileHashProbe* FileHashProbe::instance = NULL;

FileHashProbe::FileHashProbe() {
}

FileHashProbe::~FileHashProbe() {
  instance = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* FileHashProbe::Instance() {

	// Use lazy initialization
	if(instance == NULL) 
		instance = new FileHashProbe();

	return instance;	
}

ItemVector* FileHashProbe::CollectItems(Object* object) {

	ItemVector *collectedItems = new ItemVector();

	// get the path and file name
	ObjectEntity* path = object->GetElementByName("path");
	ObjectEntity* fileName = object->GetElementByName("filename");
    ObjectEntity* filePath = object->GetElementByName("filepath");
    if(filePath != NULL)
        throw ProbeException("The filepath entity is not currently supported.");

	FileFinder fileFinder;
	StringPairVector* filePaths = fileFinder.SearchFiles(path, fileName, object->GetBehaviors());

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
						item->AppendElement(new ItemEntity("path", fp->first, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
						item->AppendElement(new ItemEntity("filename", (*iterator), OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_DOES_NOT_EXIST));
						collectedItems->push_back(item);
					}
					
				} else {

					item = this->CreateItem();
					item->SetStatus(OvalEnum::STATUS_EXISTS);
					item->AppendElement(new ItemEntity("path", fp->first, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
					collectedItems->push_back(item);

				}

			} else {

				// build the path string
                string filePath = Common::BuildFilePath(fp->first, fp->second);

				// create a new item
				Item *item = NULL;
				item = this->CreateItem();
				item->SetStatus(OvalEnum::STATUS_EXISTS);
				item->AppendElement(new ItemEntity("path", fp->first, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
				item->AppendElement(new ItemEntity("filename", fp->second, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));

				// call the hashing functions
				this->GetDigest(filePath, item, Digest::MD5, "md5");
				this->GetDigest(filePath, item, Digest::SHA1, "sha1");

				collectedItems->push_back(item);
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
				item->AppendElement(new ItemEntity("path", (*iterator), OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_DOES_NOT_EXIST));
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
Item* FileHashProbe::CreateItem() {

	Item* item = new Item(0, 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#independent", 
						"ind-sc", 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#independent independent-system-characteristics-schema.xsd", 
						OvalEnum::STATUS_ERROR, 
						"filehash_item");

	return item;
}

void FileHashProbe::GetDigest(const std::string& filePath,
								Item* item,
								Digest::DigestType digestType,
								const std::string& entityName) {

	ItemEntity *digestEntity = new ItemEntity(entityName);
	item->AppendElement(digestEntity);

	try {
		string digest = this->digest.digest(filePath, digestType);
		digestEntity->SetValue(digest);
	} catch (DigestException e) {
		item->SetStatus(OvalEnum::STATUS_ERROR);
		digestEntity->SetStatus(OvalEnum::STATUS_ERROR);
		item->AppendMessage(new OvalMessage("Couldn't compute digest: "+e.GetErrorMessage()));
	}
}
