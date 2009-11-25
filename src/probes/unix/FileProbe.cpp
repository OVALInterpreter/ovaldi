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

#include "FileProbe.h"

//****************************************************************************************//
//								FileProbe Class											  //	
//****************************************************************************************//
FileProbe* FileProbe::instance = NULL;

FileProbe::FileProbe() {

}

FileProbe::~FileProbe() {
  instance = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* FileProbe::Instance() {

	// Use lazy initialization
	if(instance == NULL) 
		instance = new FileProbe();

	return instance;	
}

ItemVector* FileProbe::CollectItems(Object* object) {

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

				StringVector fileNames;
				if(fileFinder.ReportFileNameDoesNotExist(fp->first, fileName, &fileNames)) {
					StringVector::iterator iterator;
					for(iterator = fileNames.begin(); iterator != fileNames.end(); iterator++) {

						Item* item = this->CreateItem();
						item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
						item->AppendElement(new ItemEntity("path", fp->first, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
						item->AppendElement(new ItemEntity("filename", (*iterator), OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_DOES_NOT_EXIST));
						collectedItems->push_back(item);
					}
					
				} else {

					Item* item = this->GetFileAttributes(fp->first, "");
					if(item != NULL) {
						collectedItems->push_back(item);
					}
					item = NULL;

				}				

			} else {

				Item* item = this->GetFileAttributes(fp->first, fp->second);
				if(item != NULL) {
					collectedItems->push_back(item);
				}
				item = NULL;

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
Item* FileProbe::CreateItem() {

	Item* item = new Item(0, 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#unix", 
						"unix-sc", 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#unix unix-system-characteristics-schema.xsd", 
						OvalEnum::STATUS_ERROR, 
						"file_item");

	return item;
}

Item* FileProbe::GetFileAttributes(string path, string fileName) {

	string errorMessage = "";

	Item *item = NULL;

    string filePath = Common::BuildFilePath((const string)path, (const string)fileName);


	//////////////////////////////////////////////////////
	/////////////////////  FileType  /////////////////////
	//////////////////////////////////////////////////////

	struct stat sbuf;
	if (lstat(filePath.c_str(), &sbuf) != 0) {

		// check error messages
		if(errno == ENOTDIR) {
			throw ProbeException("A component of the path prefix is not a directory.");
		} else if(errno == ENAMETOOLONG) {
			throw ProbeException("A component of a pathname exceeded {NAME_MAX} characters, or an entire path name exceeded {PATH_MAX} characters.");		
		} else if(errno == EACCES) {
			throw ProbeException("Search permission is denied for a component of the path prefix.");
		} else if(errno == ELOOP) {
			throw ProbeException("Too many symbolic links were encountered in translating the pathname.");
		} else if(errno == EFAULT) {
			throw ProbeException("Sb or name points to an invalid address.");
		} else if(errno == EIO) {	
			throw ProbeException("An I/O error occurred while reading from or writing to the file system.");
		} else if(errno == ENOENT) {
			return NULL;
		}
	}

	// Set the status of the file to exists
	item = this->CreateItem();
	item->SetStatus(OvalEnum::STATUS_EXISTS);
	item->AppendElement(new ItemEntity("path", path, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
	if(fileName.compare("") != 0) {
		item->AppendElement(new ItemEntity("filename", fileName, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
	}

	mode_t mode;
	mode = sbuf.st_mode;
	ItemEntity* type = new ItemEntity("type", "", OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS);

	if ((mode & 0xF000) == S_IFIFO) type->SetValue("fifo");
	else if ((mode & 0xF000) == S_IFCHR) type->SetValue("character");
	else if ((mode & 0xF000) == S_IFDIR) type->SetValue("directory");
	else if ((mode & 0xF000) == S_IFBLK) type->SetValue("block special");
	else if ((mode & 0xF000) == S_IFREG) type->SetValue("regular");
	else if ((mode & 0xF000) == S_IFLNK) type->SetValue("symbolic link");
	else if ((mode & 0xF000) == S_IFSOCK) type->SetValue("socket");
	item->AppendElement(type);

	//////////////////////////////////////////////////////
	/////////////////////// group_id /////////////////////
	//////////////////////////////////////////////////////
	char gidBuf[16];
    snprintf(gidBuf, sizeof(gidBuf), "%u", sbuf.st_gid);
	item->AppendElement(new ItemEntity("group_id", gidBuf));

	//////////////////////////////////////////////////////
	//////////////////////  user_id  /////////////////////
	//////////////////////////////////////////////////////
	char uidBuf[16];
    snprintf(uidBuf, sizeof(uidBuf), "%u", sbuf.st_uid);
	item->AppendElement(new ItemEntity("user_id", uidBuf));

	//////////////////////////////////////////////////////
	/////////////////////  a_time	 /////////////////////
	//////////////////////////////////////////////////////
	item->AppendElement(new ItemEntity("a_time", Common::ToString((long)sbuf.st_atime)));

	//////////////////////////////////////////////////////
	/////////////////////  c_time	 /////////////////////
	//////////////////////////////////////////////////////
	item->AppendElement(new ItemEntity("c_time", Common::ToString((long)sbuf.st_ctime)));

	//////////////////////////////////////////////////////
	/////////////////////  m_time	 /////////////////////
	//////////////////////////////////////////////////////
	item->AppendElement(new ItemEntity("m_time", Common::ToString((long)sbuf.st_mtime)));

	//////////////////////////////////////////////////////
	//////////////////////  size     /////////////////////
	//////////////////////////////////////////////////////
	item->AppendElement(new ItemEntity("size", Common::ToString((long)sbuf.st_size), OvalEnum::DATATYPE_INTEGER));

	// File permissions

	//////////////////////////////////////////////////////
	///////////////////////  SUID  ///////////////////////
	//////////////////////////////////////////////////////
	int suid = 0;
	if (mode & S_ISUID)
		suid = 1;

	item->AppendElement(new ItemEntity("suid", Common::ToString(suid), OvalEnum::DATATYPE_BOOLEAN));

	//////////////////////////////////////////////////////
	///////////////////////  SGID  ///////////////////////
	//////////////////////////////////////////////////////
	int sgid = 0;
	if (mode & S_ISGID)
		sgid = 1;

	item->AppendElement(new ItemEntity("sgid", Common::ToString(sgid), OvalEnum::DATATYPE_BOOLEAN));

	//////////////////////////////////////////////////////
	//////////////////////  STICKY  //////////////////////
	//////////////////////////////////////////////////////
	int sticky = 0;
	if (mode & S_ISVTX)
		sticky = 1;

	item->AppendElement(new ItemEntity("sticky", Common::ToString(sticky), OvalEnum::DATATYPE_BOOLEAN));

	//////////////////////////////////////////////////////
	///////////////////////  UREAD  //////////////////////
	//////////////////////////////////////////////////////
	int uread = 0;
	if (mode & S_IRUSR)
		uread = 1;

	item->AppendElement(new ItemEntity("uread", Common::ToString(uread), OvalEnum::DATATYPE_BOOLEAN));

	//////////////////////////////////////////////////////
	//////////////////////  UWRITE  //////////////////////
	//////////////////////////////////////////////////////

	int uwrite = 0;
	if (mode & S_IWUSR)
		uwrite = 1;

	item->AppendElement(new ItemEntity("uwrite", Common::ToString(uwrite), OvalEnum::DATATYPE_BOOLEAN));

	//////////////////////////////////////////////////////
	///////////////////////  UEXEC  //////////////////////
	//////////////////////////////////////////////////////

	int uexec = 0;
	if (mode & S_IXUSR)
		uexec = 1;

	item->AppendElement(new ItemEntity("uexec", Common::ToString(uexec), OvalEnum::DATATYPE_BOOLEAN));

	//////////////////////////////////////////////////////
	///////////////////////  GREAD  //////////////////////
	//////////////////////////////////////////////////////

	int gread = 0;
	if (mode & S_IRGRP)
		gread = 1;

	item->AppendElement(new ItemEntity("gread", Common::ToString(gread), OvalEnum::DATATYPE_BOOLEAN));

	//////////////////////////////////////////////////////
	//////////////////////  GWRITE  //////////////////////
	//////////////////////////////////////////////////////

	int gwrite = 0;
	if (mode & S_IWGRP)
		gwrite = 1;

	item->AppendElement(new ItemEntity("gwrite", Common::ToString(gwrite), OvalEnum::DATATYPE_BOOLEAN));

	//////////////////////////////////////////////////////
	///////////////////////  GEXEC  //////////////////////
	//////////////////////////////////////////////////////

	int gexec = 0;
	if (mode & S_IXGRP)
		gexec = 1;

	item->AppendElement(new ItemEntity("gexec", Common::ToString(gexec), OvalEnum::DATATYPE_BOOLEAN));

	//////////////////////////////////////////////////////
	///////////////////////  OREAD  //////////////////////
	//////////////////////////////////////////////////////

	int oread = 0;
	if (mode & S_IROTH)
		oread = 1;

	item->AppendElement(new ItemEntity("oread", Common::ToString(oread), OvalEnum::DATATYPE_BOOLEAN));

	//////////////////////////////////////////////////////
	//////////////////////  OWRITE  //////////////////////
	//////////////////////////////////////////////////////

	int owrite = 0;
	if (mode & S_IWOTH)
		owrite = 1;

	item->AppendElement(new ItemEntity("owrite", Common::ToString(owrite), OvalEnum::DATATYPE_BOOLEAN));

	//////////////////////////////////////////////////////
	///////////////////////  OEXEC  //////////////////////
	//////////////////////////////////////////////////////

	int oexec = 0;
	if (mode & S_IXOTH)
		oexec = 1;

	item->AppendElement(new ItemEntity("oexec", Common::ToString(oexec), OvalEnum::DATATYPE_BOOLEAN));

	return item;
}
