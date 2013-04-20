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

#include "FileProbe.h"

using namespace std;

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
    FileFinder fileFinder;
	StringPairVector* filePaths = NULL;

	if(filePath != NULL){
		if ( (object->GetBehaviors())->size() > 0 ){
			throw ProbeException("Error: Behaviors do not apply to the filepath entity and cannot be used.");
		}
		filePaths = fileFinder.SearchFiles(filePath);	
	}else{
		filePaths = fileFinder.SearchFiles(path, fileName, object->GetBehaviors());
	}

	if(filePaths != NULL && !filePaths->empty()) {
		// Loop through all file paths
		StringPairVector::iterator iterator;
		for(iterator = filePaths->begin(); iterator != filePaths->end(); iterator++) {

			StringPair* fp = (*iterator);

			if(fp->second.empty()) {

				if(fileFinder.ReportFileNameDoesNotExist(fp->first, fileName)) {

					Item* item = this->CreateItem();
					item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
					item->AppendElement(new ItemEntity("filepath", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));
					item->AppendElement(new ItemEntity("path", fp->first, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
					item->AppendElement(new ItemEntity("filename", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));
					collectedItems->push_back(item);
					
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
		if ( filePath != NULL ){
			StringVector fpaths;
			if (fileFinder.ReportFilePathDoesNotExist(filePath,&fpaths)){
				StringVector statusValues;
				Item* item = NULL;
				StringPair* fpComponents = NULL;
				ObjectEntity pathStatus("path","",OvalEnum::DATATYPE_STRING,OvalEnum::OPERATION_EQUALS,NULL,OvalEnum::CHECK_ALL,false);

				for(StringVector::iterator iterator = fpaths.begin(); iterator != fpaths.end(); iterator++) {
					item = this->CreateItem();
					item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
					fpComponents = Common::SplitFilePath(*iterator);
					pathStatus.SetValue(fpComponents->first);
					item->AppendElement(new ItemEntity("filepath", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));
					if (fileFinder.ReportPathDoesNotExist(&pathStatus))
						item->AppendElement(new ItemEntity("path", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));
					else
						item->AppendElement(new ItemEntity("path", fpComponents->first));
					item->AppendElement(new ItemEntity("filename", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));
					collectedItems->push_back(item);
					
					if ( fpComponents != NULL ){
						delete fpComponents;
						fpComponents = NULL;
					}
				}
			}
		}else{
			if(fileFinder.ReportPathDoesNotExist(path)) {
				Item* item = this->CreateItem();
				item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
				item->AppendElement(new ItemEntity("path", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));
				collectedItems->push_back(item);
			}
		}
	}
	if ( filePaths != NULL ){
		delete filePaths;
	}

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

    string filePath = Common::BuildFilePath(path, fileName);


	//////////////////////////////////////////////////////
	/////////////////////  FileType  /////////////////////
	//////////////////////////////////////////////////////

	struct stat sbuf;
	if (lstat(filePath.c_str(), &sbuf) != 0) {
		if(errno == ENOENT)
			return NULL;

		throw ProbeException(strerror(errno));
	}

	// Set the status of the file to exists
	item = this->CreateItem();
	item->SetStatus(OvalEnum::STATUS_EXISTS);
	item->AppendElement(new ItemEntity("filepath", filePath, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
	item->AppendElement(new ItemEntity("path", path, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
	ItemEntity *fileNameIe = new ItemEntity("filename", fileName);
	if (fileName.empty()) {
		fileNameIe->SetStatus(OvalEnum::STATUS_NOT_COLLECTED);
		fileNameIe->SetNil(true);
	}
	item->AppendElement(fileNameIe);

	mode_t mode;
	mode = sbuf.st_mode;
	ItemEntity* type = new ItemEntity("type", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS);

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
	item->AppendElement(new ItemEntity("group_id", Common::ToString(sbuf.st_gid), OvalEnum::DATATYPE_INTEGER));

	//////////////////////////////////////////////////////
	//////////////////////  user_id  /////////////////////
	//////////////////////////////////////////////////////
	item->AppendElement(new ItemEntity("user_id", Common::ToString(sbuf.st_uid), OvalEnum::DATATYPE_INTEGER));

	//////////////////////////////////////////////////////
	/////////////////////  a_time	 /////////////////////
	//////////////////////////////////////////////////////
	item->AppendElement(new ItemEntity("a_time", Common::ToString((long)sbuf.st_atime), OvalEnum::DATATYPE_INTEGER));

	//////////////////////////////////////////////////////
	/////////////////////  c_time	 /////////////////////
	//////////////////////////////////////////////////////
	item->AppendElement(new ItemEntity("c_time", Common::ToString((long)sbuf.st_ctime), OvalEnum::DATATYPE_INTEGER));

	//////////////////////////////////////////////////////
	/////////////////////  m_time	 /////////////////////
	//////////////////////////////////////////////////////
	item->AppendElement(new ItemEntity("m_time", Common::ToString((long)sbuf.st_mtime), OvalEnum::DATATYPE_INTEGER));

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

	//////////////////////////////////////////////////////
	///////////////////////  ACL  ////////////////////////
	//////////////////////////////////////////////////////

# if defined (LINUX)
	/* From the original thread (http://making-security-measurable.1364806.n2.nabble.com/Proposal-for-UNIX-ACL-child-element-UNCLASSIFIED-td4542654.html#a4582129) the expected behavior is:
	
	   1) If an interpreter doesn't support the collection of ACL information, the status will be 'not collected'.
	   2) If there is an error trying to retrieve this information, the status will be 'error'.
	   3) If a system doesn't support ACLs, the status will be 'does not exist'.
	   4) If a system supports ACLs, the status will be 'exists'.
	   5) If a file doesn't have an ACL, or it matches the standard UNIX permissions, the value will be 'false' (this is covered by acl_extended_file() - thank you openscap)
	   6) If a file has an ACL, the value will be 'true'.
	*/
	
	int hasExtendedAcl = acl_extended_file(filePath.c_str());
	if(hasExtendedAcl > -1){ // behavior 4, 5, and 6
          item->AppendElement(new ItemEntity("has_extended_acl",Common::ToString(hasExtendedAcl),OvalEnum::DATATYPE_BOOLEAN,OvalEnum::STATUS_EXISTS,0));
	}else{
	  if(errno == EOPNOTSUPP){ // behavior 3
	    item->AppendElement(new ItemEntity("has_extended_acl","",OvalEnum::DATATYPE_BOOLEAN,OvalEnum::STATUS_DOES_NOT_EXIST,0));
          }else{ // behavior 2
	    item->AppendElement(new ItemEntity("has_extended_acl","",OvalEnum::DATATYPE_BOOLEAN,OvalEnum::STATUS_ERROR,0));
            item->AppendMessage(new OvalMessage(string("Error reading ACL data: ") + strerror(errno)));
          }
	}

# else
	// behavior 1
	item->AppendElement(new ItemEntity("has_extended_acl","",OvalEnum::DATATYPE_BOOLEAN,OvalEnum::STATUS_NOT_COLLECTED,0));
	
# endif

      return item;
}
