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

#include "FileMd5Probe.h"

using namespace std;

//****************************************************************************************//
//								FileMd5Probe Class										  //	
//****************************************************************************************//
FileMd5Probe* FileMd5Probe::instance = NULL;

FileMd5Probe::FileMd5Probe() {

}

FileMd5Probe::~FileMd5Probe() {
  instance = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* FileMd5Probe::Instance() {

	// Use lazy initialization
	if(instance == NULL) 
		instance = new FileMd5Probe();

	return instance;	
}

ItemVector* FileMd5Probe::CollectItems(Object* object) {

	ItemVector *collectedItems = new ItemVector();

	// get the path and file name
	ObjectEntity* path = object->GetElementByName("path");
	ObjectEntity* fileName = object->GetElementByName("filename");
    ObjectEntity* filePath = object->GetElementByName("filepath");
    if(filePath != NULL)
        throw ProbeException("The filepath entity is not currently supported.");

#ifdef WIN32
	FileFinder fileFinder(WindowsCommon::behavior2view(object->GetBehaviors()));
#else
	FileFinder fileFinder;
#endif

	StringPairVector* filePaths = NULL;
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
				if(fileFinder.ReportFileNameDoesNotExist(fp->first, fileName)) {

					item = this->CreateItem();
					item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
					item->AppendElement(new ItemEntity("path", fp->first, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
					item->AppendElement(new ItemEntity("filename", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));
					ADD_WINDOWS_VIEW_ENTITY
					collectedItems->push_back(item);
					
				} else {

					item = this->CreateItem();
					item->SetStatus(OvalEnum::STATUS_EXISTS);
					item->AppendElement(new ItemEntity("path", fp->first, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
					ADD_WINDOWS_VIEW_ENTITY
					collectedItems->push_back(item);

				}

			} else {
				Item *item;
				FS_REDIRECT_GUARD_BEGIN(fileFinder.GetView());
				item = this->GetMd5(fp->first, fp->second);
				FS_REDIRECT_GUARD_END
				if(item != NULL) {
					ADD_WINDOWS_VIEW_ENTITY
					collectedItems->push_back(item);
				}
				item = NULL;

			}

			delete fp;
		}

	} else {
		// if no filepaths check if the code should report that the path does not exist
		if(fileFinder.ReportPathDoesNotExist(path)) {

			Item* item = NULL;
			item = this->CreateItem();
			item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
			item->AppendElement(new ItemEntity("path", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));
			ADD_WINDOWS_VIEW_ENTITY
			collectedItems->push_back(item);
		}
	}
	delete filePaths;

	return collectedItems;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
Item* FileMd5Probe::CreateItem() {
	// -----------------------------------------------------------------------
	//
	//  ABSTRACT
	//
	//  Return a new Item created for storing file information
	//
	// -----------------------------------------------------------------------

	Item* item = new Item(0, 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#independent", 
						"ind-sc", 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#independent independent-system-characteristics-schema.xsd", 
						OvalEnum::STATUS_ERROR, 
						"filemd5_item");

	return item;
}

Item* FileMd5Probe::GetMd5(string path, string fileName) {
	// -----------------------------------------------------------------------
	//
	//  ABSTRACT
	//
	//	Get all attributes for the file specified in fileIn..Return them in a 
	//	FileAttribute
	// -----------------------------------------------------------------------

	string errorMessage;
	Item *item = NULL;
	string filePath = Common::BuildFilePath(path, fileName);



	try {

		//////////////////////////////////////////////////////
		////////////////////////  MD5  ///////////////////////
		//////////////////////////////////////////////////////

		item = this->CreateItem();
		item->AppendElement(new ItemEntity("path", path, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
		item->AppendElement(new ItemEntity("filename", fileName, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));

		ifstream in(filePath.c_str(), ios::in|ios::binary);

		if (!in) {
			
			errorMessage = "(FileMd5Probe) Unable to get MD5 information for the file '" +
							filePath +
							"' \n";
			item->SetStatus(OvalEnum::STATUS_ERROR);
			item->AppendMessage(new OvalMessage(errorMessage));
			ItemEntity* md5Entity = new ItemEntity("md5", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_ERROR);
			item->AppendElement(md5Entity);
		
		} else {
			string digestVal = this->digest.digest(in, Digest::MD5);
			item->SetStatus(OvalEnum::STATUS_EXISTS);
			item->AppendElement(new ItemEntity("md5", digestVal));
		}

		//////////////////////////////////////////////////////
		//////////////////////////////////////////////////////
	} catch(ProbeException ex) {	
	
		Log::Debug(ex.GetErrorMessage());

	} catch(...) {	
	
		string errMsg = "";
		errMsg.append("(FileMd5Probe) Unknown error attempting to get file attribute information for the file '");
		errMsg.append(filePath);
		errMsg.append("'");
		Log::Debug(errMsg);		
	}

	return item;
}
