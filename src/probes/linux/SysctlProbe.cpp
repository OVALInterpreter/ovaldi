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

#include <algorithm>
#include <iterator>
#include <fstream>
#include <memory>
#include <string>
#include <sys/stat.h>
#include <ftw.h>

#include <Common.h>
#include <Log.h>
#include <OvalEnum.h>
#include <VectorPtrGuard.h>
#include "SysctlProbe.h"

using namespace std;

#define PROC_SYS_ROOT "/proc/sys/"

namespace {

	/**
	 * I think it's pretty dumb that you can't pass any user-defined data into
	 * the ftw() function.  So I created a class to be instantiated globally,
	 * that can hold data I need in the ftw visitor function.  Then I decided I
	 * may as well put all the visitor functionality into the class too; then
	 * the ftw visitor function can simply forward to a method of the global
	 * visitor object.
	 */
	class FtwVisitor {
		ObjectEntity *nameObjEntity;
		ItemVector *items;

	public:
		void setup(ObjectEntity *nameObjEntity, ItemVector *items) {
			this->nameObjEntity = nameObjEntity;
			this->items = items;
		}

		/**
		 * Checks the given path for a match with nameObjEntity, adding an item
		 * to the items vector, if it matches.
		 */
		int visit(const char *fpath, const struct stat *sb, int typeflag);
	};

	/**
	 * Delegate of the visitor function below.  This is preconfigured with other
	 * data before ftw() is invoked, so that it can do its job.
	 */
	FtwVisitor visitorObj;

	/**
	 * Creates a new sysctl_item.
	 */
	auto_ptr<Item> CreateItem();

	/**
	 * Returns '/' if c=='.', or c otherwise.
	 */
	char dot2slash(const char c);

	/**
	 * Returns '.' if c=='/', or c otherwise.
	 */
	char slash2dot(const char c);

	/**
	 * Converts the given sysctl property name to a path to a file under
	 * PROC_SYS_ROOT.
	 */
	string SysctlName2Path(const string &name);

	/**
	 * Converts the given path to a sysctl property name.  \p relpath is so
	 * named because the man page for ftw(3) states that the fpath param of the
	 * visitor function is a path relative to the search root directory.  I
	 * didn't find this to be the case; I found it to be an absolute directory.
	 * So this function first strips off the search directory root prefix, if
	 * found.
	 */
	string Path2SysctlName(const string &relpath);

	/**
	 * Reads the given file, adding "value" entities to the given item for each
	 * line of the file.
	 */
	void ReadIntoItem(const string &filepath, Item *item);

	/**
	 * Instead of searching all sysctl property names, this function directly
	 * gets values for the given names.  Items are created and added to the
	 * given vector.
	 */
	void GetByName(const StringVector &names, ItemVector *items);

	/**
	 * A function passed into ftw(3) to process visited files.  It delegates to
	 * visitorObj above.  Don't call this without first setting up visitorObj,
	 * as this function delegates to a method of that object.
	 */
	int visitor(const char *fpath, const struct stat *sb, int typeflag);

	/**
	 * Searches all sysctl properties for matches with the given object entity.
	 * Items are created for those which match, and added to the given vector.
	 */
	void SearchAll(ObjectEntity *nameObjEntity, ItemVector *items);
}

//****************************************************************************************//
//								SysctlProbe Class									  //	
//****************************************************************************************//
SysctlProbe *SysctlProbe::instance = NULL;

SysctlProbe::SysctlProbe() {
}

SysctlProbe::~SysctlProbe() {
  instance = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* SysctlProbe::Instance() {

	// Use lazy initialization
	if(instance == NULL) 
		instance = new SysctlProbe();

	return instance;	
}

ItemVector* SysctlProbe::CollectItems(Object* object) {

	VectorPtrGuard<Item> collectedItems(new ItemVector());

	ObjectEntity *nameObjEntity = object->GetElementByName("name");

	if (nameObjEntity->GetOperation() == OvalEnum::OPERATION_EQUALS) {
		StringVector names;
		nameObjEntity->GetEntityValues(names);
		GetByName(names, collectedItems.get());
	} else
		SearchAll(nameObjEntity, collectedItems.get());

	return collectedItems.release();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

/** this method is never called. */
Item* SysctlProbe::CreateItem() {
	return NULL;
}

namespace {
	auto_ptr<Item> CreateItem() {
		auto_ptr<Item> item(new Item(0,
									 "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#unix", 
									 "unix-sc", 
									 "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#unix unix-system-characteristics-schema.xsd", 
									 OvalEnum::STATUS_ERROR, 
									 "sysctl_item"));

		return item;
	}

	char dot2slash(const char c) {
		return c=='.' ? '/' : c;
	}

	char slash2dot(const char c) {
		return c=='/' ? '.' : c;
	}

	string SysctlName2Path(const string &name) {
		string result;
		transform(name.begin(), name.end(), back_inserter(result), dot2slash);
		return Common::BuildFilePath(PROC_SYS_ROOT, result);		
	}

	string Path2SysctlName(const string &relpath) {
		string result;
		string strippedPath;
		string procSysRoot(PROC_SYS_ROOT);

		// I wanna ensure the prefix string we're checking for ends with a slash.
		if (!procSysRoot.empty() && procSysRoot[procSysRoot.size()-1] != '/')
			procSysRoot += '/';

		// according to the ftw() man page, the path passed into the visitor
		// function is relative to the directory which is the root of the
		// search.  My experiments indicate that is not true.  I always was
		// getting an absolute path.  So I need to strip off the portion of the
		// path corresponding to the search root.  I still check to see whether
		// or not the search directory prefixes the filepath, just in case...
		if (procSysRoot.size() <= relpath.size() &&
			relpath.compare(0, procSysRoot.size(), procSysRoot, 0, procSysRoot.size()) == 0)
			strippedPath = relpath.substr(procSysRoot.size());
		else
			strippedPath = relpath;

		transform(strippedPath.begin(), strippedPath.end(), back_inserter(result), slash2dot);
		return result;
	}

	void ReadIntoItem(const string &filepath, Item *item) {

		ifstream in(filepath.c_str());
		if (in) {
			string value;
			// for non-newline terminated files, it seems that eof() gets set
			// but (void*)in is still non-null.  So the loop goes one too many
			// iterations.  So I check eof() explicitly.
			while(in && !in.eof()) {
				getline(in, value);
				if (!value.empty())
					item->AppendElement(new ItemEntity("value", value));
			}
			item->SetStatus(OvalEnum::STATUS_EXISTS);
		} else
			// i wish the STL gave more error information...
			item->AppendMessage(new OvalMessage("Could not open "+filepath, OvalEnum::LEVEL_ERROR));
	}

	void GetByName(const StringVector &names, ItemVector *items) {
		for (StringVector::const_iterator nameIter = names.begin();
			 nameIter != names.end();
			 ++nameIter) {

			auto_ptr<Item> item(CreateItem());
			item->AppendElement(new ItemEntity("name", *nameIter, OvalEnum::DATATYPE_STRING));

			string procSysPath(SysctlName2Path(*nameIter));
			ReadIntoItem(procSysPath, item.get());
			items->push_back(item.release());
		}
	}

	int FtwVisitor::visit(const char *fpath, const struct stat *sb, int typeflag) {

		// Users might want to know about failures?
		if (typeflag == FTW_NS) {
			// FTW_NS means the stat() call failed.
			Log::Debug(string("Could not stat() ")+fpath);
			return 0;
		}

		if (typeflag == FTW_DNR) {
			// FTW_NS means fpath is a directory which couldn't be read
			Log::Debug(string("Could not read directory ")+fpath);
			return 0;
		}

		// account for variations on when FTW_F is passed.  Use
		// use the stat results to make sure it's really a regular
		// file.
		if (typeflag != FTW_F || !S_ISREG(sb->st_mode))
			return 0;

		string sysctlName = Path2SysctlName(fpath);
		auto_ptr<ItemEntity> ie(new ItemEntity("name", sysctlName, OvalEnum::DATATYPE_STRING));
		if (this->nameObjEntity->Analyze(ie.get()) == OvalEnum::RESULT_TRUE) {

			auto_ptr<Item> item(CreateItem());
			item->AppendElement(ie.release());

			// the property matches, now we gotta read the file...
			ReadIntoItem(fpath, item.get());

			this->items->push_back(item.release());
		}

		return 0;
	}

	int visitor(const char *fpath, const struct stat *sb, int typeflag) {
		return visitorObj.visit(fpath, sb, typeflag);
	}

	void SearchAll(ObjectEntity *nameObjEntity, ItemVector *items) {
		visitorObj.setup(nameObjEntity, items);
		int result = ftw(PROC_SYS_ROOT, visitor, 10);
		if (result)
			throw ProbeException("Error recursing over " PROC_SYS_ROOT 
								 ": ftw() returned " + Common::ToString(result));
	}
}
