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

#include <fstream>
#include <iomanip>
#include <sstream>

#ifdef WIN32
#  include <PrivilegeGuard.h>
#  include <WindowsCommon.h>
#  include <FsRedirectionGuard.h>
// macro this so it can disappear on non-windows OSs.
#  define ADD_WINDOWS_VIEW_ENTITY \
	item->AppendElement(new ItemEntity("windows_view", \
		(fileFinder.GetView() == BIT_32 ? "32_bit" : "64_bit")));
#else
#  define ADD_WINDOWS_VIEW_ENTITY
#  define FS_REDIRECT_GUARD_BEGIN(x)
#  define FS_REDIRECT_GUARD_END
#endif

#include "FileHashProbe.h"

using namespace std;

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
	StringPairVector* filePaths = NULL;

#ifdef WIN32
	FileFinder fileFinder(WindowsCommon::behavior2view(object->GetBehaviors()));
#else
	FileFinder fileFinder;
#endif

#ifdef WIN32
	{
		PrivilegeGuard pg(SE_BACKUP_NAME, false);
#endif

		if(filePath != NULL){
			filePaths = fileFinder.SearchFiles(filePath);	
		}else{
			filePaths = fileFinder.SearchFiles(path, fileName, object->GetBehaviors());
		}

#ifdef WIN32
	}
#endif

	if(filePaths != NULL && filePaths->size() > 0) {
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
						item->AppendElement(new ItemEntity("filepath","", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));
						item->AppendElement(new ItemEntity("path", fp->first, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
						item->AppendElement(new ItemEntity("filename", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));
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

				// build the path string
                string filePath = Common::BuildFilePath(fp->first, fp->second);

				// create a new item
				Item *item = NULL;
				item = this->CreateItem();
				item->SetStatus(OvalEnum::STATUS_EXISTS);
				item->AppendElement(new ItemEntity("filepath",filePath,OvalEnum::DATATYPE_STRING,OvalEnum::STATUS_EXISTS));
				item->AppendElement(new ItemEntity("path", fp->first, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
				item->AppendElement(new ItemEntity("filename", fp->second, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));

				// call the hashing functions
				FS_REDIRECT_GUARD_BEGIN(fileFinder.GetView())
				this->GetDigest(filePath, item, Digest::MD5, "md5");
				this->GetDigest(filePath, item, Digest::SHA1, "sha1");
				FS_REDIRECT_GUARD_END

				ADD_WINDOWS_VIEW_ENTITY
				collectedItems->push_back(item);
			}

			delete fp;
		}

	} else {
		if ( filePath != NULL ){
			StringVector fpaths;
			if (fileFinder.ReportFilePathDoesNotExist(filePath,&fpaths)){
				StringVector statusValues;
				Item* item = NULL;
				StringPair* fpComponents = NULL;

				// build path ObjectEntity to pass to ReportPathDoesNotExist to retrieve the status of the path value
				ObjectEntity* pathStatus = new ObjectEntity("path","",OvalEnum::DATATYPE_STRING,OvalEnum::OPERATION_EQUALS,NULL,OvalEnum::CHECK_ALL,false);
				// build filename ObjectEntity to pass to ReportFileNameDoesNotExist to retrieve the status of the filename value
				ObjectEntity* fileNameStatus = new ObjectEntity("filename","",OvalEnum::DATATYPE_STRING,OvalEnum::OPERATION_EQUALS,NULL,OvalEnum::CHECK_ALL,false);
				
				for(StringVector::iterator iterator = fpaths.begin(); iterator != fpaths.end(); iterator++) {
					item = this->CreateItem();
					item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
					fpComponents = Common::SplitFilePath(*iterator);
					pathStatus->SetValue(fpComponents->first);
					fileNameStatus->SetValue(fpComponents->second);
					item->AppendElement(new ItemEntity("filepath", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));
					item->AppendElement(new ItemEntity("path", "", OvalEnum::DATATYPE_STRING, (fileFinder.ReportPathDoesNotExist(pathStatus,&statusValues))?OvalEnum::STATUS_DOES_NOT_EXIST:OvalEnum::STATUS_EXISTS));
					item->AppendElement(new ItemEntity("filename","", OvalEnum::DATATYPE_STRING, (fileFinder.ReportFileNameDoesNotExist(fpComponents->first,fileNameStatus,&statusValues))?OvalEnum::STATUS_DOES_NOT_EXIST:OvalEnum::STATUS_EXISTS));
					ADD_WINDOWS_VIEW_ENTITY
					collectedItems->push_back(item);
					
					if ( fpComponents != NULL ){
						delete fpComponents;
						fpComponents = NULL;
					}
				}

				if ( pathStatus != NULL ){
					delete pathStatus;
					pathStatus = NULL;
				}
				if ( fileNameStatus != NULL ){
					delete fileNameStatus;
					fileNameStatus = NULL;
				}
			}
		}else{
			StringVector paths;
			if(fileFinder.ReportPathDoesNotExist(path, &paths)) {
				Item* item = NULL;
				StringVector::iterator iterator;
				for(iterator = paths.begin(); iterator != paths.end(); iterator++) {
					item = this->CreateItem();
					item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
					item->AppendElement(new ItemEntity("path","", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));
					ADD_WINDOWS_VIEW_ENTITY
					collectedItems->push_back(item);
				}
			}
		}
	}
	if ( filePaths != NULL ){
		delete filePaths;
		filePaths = NULL;
	}

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
