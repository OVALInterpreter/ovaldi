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

#include "TextFileContent54Probe.h"
#include "FileFinder.h"
#include "ObjectEntity.h"

//****************************************************************************************//
//								TextFileContent54Probe Class								  //	
//****************************************************************************************//
TextFileContent54Probe* TextFileContent54Probe::instance = NULL;

TextFileContent54Probe::TextFileContent54Probe() {

}

TextFileContent54Probe::~TextFileContent54Probe() {
  instance = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* TextFileContent54Probe::Instance() {

	// Use lazy initialization
	if(instance == NULL) 
		instance = new TextFileContent54Probe();

	return instance;	
}

ItemVector* TextFileContent54Probe::CollectItems(Object* object) {

	// get the path and file name
	ObjectEntity* path = object->GetElementByName("path");
	ObjectEntity* fileName = object->GetElementByName("filename");
	ObjectEntity* filePath = object->GetElementByName("filepath");
	ObjectEntity* pattern = object->GetElementByName("pattern");
	ObjectEntity* instance = object->GetElementByName("instance");

	int matchOptions = this->Behaviors2MatchOptions(object->GetBehaviors());
	ItemVector *collectedItems = new ItemVector();
	FileFinder fileFinder;
	StringPairVector* filePaths = NULL;

	#ifdef WIN32
	if ( WindowsCommon::EnablePrivilege(SE_BACKUP_NAME) == 0 ){
		Log::Message("Error: Unable to enable SE_BACKUP_NAME privilege.");
	}
	#endif
	
	if(filePath != NULL){
		if ( (object->GetBehaviors())->size() > 0 ){
			throw ProbeException("Error: Behaviors do not apply to the filepath entity and cannot be used.");
		}
		filePaths = fileFinder.SearchFiles(filePath);	
	}else{
		filePaths = fileFinder.SearchFiles(path, fileName, object->GetBehaviors());
	}

	#ifdef WIN32
	if ( WindowsCommon::DisableAllPrivileges() == 0 ){
		Log::Message("Error: Unable to disable all privileges.");
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
						item->AppendElement(new ItemEntity("filepath", Common::BuildFilePath(fp->first, *iterator), OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_DOES_NOT_EXIST));
						item->AppendElement(new ItemEntity("path", fp->first, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
						item->AppendElement(new ItemEntity("filename", (*iterator), OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_DOES_NOT_EXIST));
						collectedItems->push_back(item);
					}
				}
			} else {

				this->GetItems(fp->first, fp->second, pattern, instance, matchOptions, collectedItems);
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
					item->AppendElement(new ItemEntity("filepath", (*iterator), OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_DOES_NOT_EXIST));
					item->AppendElement(new ItemEntity("path", fpComponents->first, OvalEnum::DATATYPE_STRING, true, (fileFinder.ReportPathDoesNotExist(pathStatus,&statusValues))?OvalEnum::STATUS_DOES_NOT_EXIST:OvalEnum::STATUS_EXISTS));
					item->AppendElement(new ItemEntity("filename", fpComponents->second, OvalEnum::DATATYPE_STRING, true, (fileFinder.ReportFileNameDoesNotExist(fpComponents->first,fileNameStatus,&statusValues))?OvalEnum::STATUS_DOES_NOT_EXIST:OvalEnum::STATUS_EXISTS));
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
					item->AppendElement(new ItemEntity("path", (*iterator), OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_DOES_NOT_EXIST));
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
Item* TextFileContent54Probe::CreateItem() {

	Item* item = new Item(0, 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#independent", 
						"ind-sc", 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#independent independent-system-characteristics-schema.xsd", 
						OvalEnum::STATUS_ERROR, 
						"textfilecontent_item");

	return item;
}

void TextFileContent54Probe::GetItems(string path, string fileName,
									ObjectEntity *patternEntity,
									ObjectEntity *instanceEntity,
									int matchOptions,
									ItemVector* collectedItems) {

	// construct the file path
	string filePath = Common::BuildFilePath(path, fileName);

	// read the file line by line
	string buffer;
	string fileContents;
	ifstream infile;
	infile.open (filePath.c_str());
	if (infile.is_open()) {

		while (!infile.eof()) {
			getline (infile, buffer);
			fileContents += buffer + "\n";
		}

		infile.close();

		this->GetMatches(path, fileName, fileContents, patternEntity,
			instanceEntity, matchOptions, collectedItems);
	}
	else
		throw ProbeException(string("Couldn't open file: ")+Common::BuildFilePath(path, fileName));
}

void TextFileContent54Probe::GetMatches(const string& path,
										const string& fileName,
										const string& fileContents,
										ObjectEntity *patternEntity,
										ObjectEntity *instanceEntity,
										int matchOptions,
										ItemVector *collectedItems) {

	vector<StringVector> matches;
	StringVector patterns;
	int instance;

	// I am ignoring all operations on patterns for now... since this is an
	//   anomalous usage of pattern matching in an object.  I don't see how
	//   it would make sense to, e.g. do pattern matching on a pattern :-P
	//   Instead, I always get all matches of all patterns.
	patternEntity->GetEntityValues(patterns);

	for (StringVector::iterator patternIter = patterns.begin();
		patternIter != patterns.end();
		++patternIter) {

		instance = 0;
		matches.clear();
		this->re.GetAllMatchingSubstrings(*patternIter, fileContents, matches, matchOptions);

		for (vector<StringVector>::iterator matchIter = matches.begin();
			matchIter != matches.end();
			++matchIter) {

			++instance;

			// I don't think GetAllMatchingSubstrings() should return empty vectors, but
			//   there is an assumption in code below that it is not empty.  So I will
			//   verify that here.
			if (matchIter->empty()) {
				Log::Debug("Oops, REGEX::GetAllMatchingSubstrings() returned an empty vector!");
				continue;
			}

			// make sure this is one of the instances the object actually calls for
			ItemEntity *instanceItemEntity = new ItemEntity("instance", Common::ToString(instance), OvalEnum::DATATYPE_INTEGER, true);
			if (instanceEntity->Analyze(instanceItemEntity) != OvalEnum::RESULT_TRUE) {
				delete instanceItemEntity;
				continue;
			}

			Item *item = this->CreateItem();
			item->SetStatus(OvalEnum::STATUS_EXISTS);
			item->AppendElement(new ItemEntity("filepath", Common::BuildFilePath(path, fileName), OvalEnum::DATATYPE_STRING, true));
			item->AppendElement(new ItemEntity("path", path, OvalEnum::DATATYPE_STRING, true));
			item->AppendElement(new ItemEntity("filename", fileName, OvalEnum::DATATYPE_STRING, true));
			item->AppendElement(new ItemEntity("pattern", *patternIter, OvalEnum::DATATYPE_STRING, true));
			item->AppendElement(instanceItemEntity);

			// first element is the overall match... subsequent elements are the captures
			StringVector::iterator subMatchIter = matchIter->begin();
			item->AppendElement(new ItemEntity("text", *subMatchIter));
			++subMatchIter;

			for (;subMatchIter != matchIter->end(); ++subMatchIter)
				item->AppendElement(new ItemEntity("subexpression", *subMatchIter));

			collectedItems->push_back(item);
		}
	}
}

int TextFileContent54Probe::Behaviors2MatchOptions(BehaviorVector *behaviors) {
	int opts = 0;

	for (BehaviorVector::iterator iter = behaviors->begin();
		iter != behaviors->end();
		++iter) {
		if ((*iter)->GetName() == "ignore_case" && (*iter)->GetValue() == "true")
			opts |= REGEX::IGNORE_CASE;
		else if ((*iter)->GetName() == "multiline" && (*iter)->GetValue() == "true")
			opts |= REGEX::MULTILINE;
		else if ((*iter)->GetName() == "singleline" && (*iter)->GetValue() == "true")
			opts |= REGEX::SINGLELINE;
	}

	return opts;
}
