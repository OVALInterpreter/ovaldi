//
//
//****************************************************************************************//
// Copyright (c) 2002-2011, The MITRE Corporation
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

#include <cerrno>
#include <memory>
#include <VectorPtrGuard.h>

#include <rpm/rpmcli.h>
#include <rpm/rpmts.h>
#include <rpm/rpmfi.h>
#include <rpm/rpmdb.h>
#include <rpm/rpmlib.h>

/*
rpmts = transaction set
rpmfi = file information
rpmps = problem set
rpmpsm = package state machine
rpmcli = command line interface
rpmdb = database
rpmlib = misc stuff...?
*/

#include <sys/stat.h>

#include "RPMVerifyProbe.h"
#include <OvalMessage.h>
#include <OvalEnum.h>
#include <Behavior.h>
#include <Log.h>

using namespace std;

namespace {

	/**
	 * Represents the selected behaviors in a form that is more easily
	 * accessible, and interoperable with the rpm library.
	 */
	struct VerifyBehaviors {
		/** defaults to checking everything */
		VerifyBehaviors():
			verifyFlags(VERIFY_ALL), 
			noconfig(false), 
			noghost(false) {
		}

		/** 
		 * A bitmask, bits from the rpmVerifyFlags enumeration.  I am taking
		 * advantage of an enum already built into the RPM library.
		 */
		int verifyFlags;

		// these are specific to our behaviors
		bool noconfig;
		bool noghost;
	};

	/**
	 * A trick borrowed from boost... an easy way to make a class noncopyable.
	 * Wish we could just use boost :-P  Makes sure I don't mess up and make
	 * accidental copies.
	 */
	class noncopyable {
	protected:
		noncopyable(){}
		~noncopyable(){}
	private:
		noncopyable(const noncopyable&);
		noncopyable& operator=(const noncopyable&);
	};

	// Perhaps all these guard classes aren't strictly necessary in all cases,
	// but in C++ code which uses exceptions, I feel a lot safer having them.
	// And you never know, someone else may come along later and add a throw
	// where there wasn't before, which without the guard classes could cause a
	// resource leak.  I saw an article online somewhere describing a more
	// general mechanism, where I think you could register any C function to be
	// called when the object is destroyed.  I might try that, if I could find
	// the article again....

	class RpmtsGuard : private noncopyable {
	public:
		RpmtsGuard() {
			ts = rpmtsCreate();
			
			// I don't think this can ever happen.  From my reading of the rpm
			// source, I think it would exit the program if memory allocation
			// failed.  But it doesn't hurt to check...
			if (!ts)
				throw ProbeException("Couldn't create RPM database transaction set");
		}

		~RpmtsGuard() {
			rpmtsFree(ts);
		}

		operator rpmts() {
			return ts;
		}

	private:
		rpmts ts;
	};

	class RpmdbIterGuard : private noncopyable {
	public:
		RpmdbIterGuard(const rpmts ts, rpmTag rpmtag,
			const void * keyp, size_t keylen) {
			iter = rpmtsInitIterator(ts, rpmtag, keyp, keylen);

			if (!iter)
				throw ProbeException("Couldn't create RPM database iterator");
		}

		~RpmdbIterGuard() {
			rpmdbFreeIterator(iter);
		}

		operator rpmdbMatchIterator() {
			return iter;
		}
		
	private:
		rpmdbMatchIterator iter;
	};

	class RpmfiGuard : private noncopyable {
	public:

		RpmfiGuard(rpmts ts, Header hdr, rpmTag tag, 
				   const string &rpmName /* for error messages */) {
			fileInfo = rpmfiNew(ts, hdr, tag, 0);
			
			if (!fileInfo)
				throw ProbeException("Couldn't init file iterator on RPM "+rpmName);
		}

		~RpmfiGuard() {
			rpmfiFree(fileInfo);
		}
		
		operator rpmfi() {
			return fileInfo;
		}		 

	private:
		rpmfi fileInfo;
	};	

	/**
	 * Moved this out so my free functions below could create items.
	 */
	auto_ptr<Item> CreateItem();

	// Implementing the following as free functions keeps implementation detail
	// junk outta the header, which keeps it simpler.

	/**
	 * Looks up and processes the packages with the given names, returning all
	 * items which match the given object.
	 */
	void ProcessPackagesByName(const StringVector &pkgNames,
							   Object *obj, ItemVector *items);

	/**
	 * Processes all packages, returning all items which match the given object.
	 */
	void ProcessAllPackages(Object *obj, ItemVector *items);

	/**
	 * Looks up which packages own the given files.
	 */
	StringVector GetPackagesForFilepaths(const StringVector &filePaths);

	/**
	 * The rpmVerifyAttrs enumeration includes enumerators representing specific
	 * errors.  This checks the given bitset and adds messages to the given item
	 * for any of those error conditions.  If \p checkErrno is true, then the
	 * messages will include a text description of the current value of errno.
	 */
	void AddMessagesForFailures(Item *item, rpmVerifyAttrs attrs, bool checkErrno);

	/**
	 * Given a package (header) and object and behaviors, scan through the files
	 * in the package and create items for any files which match the object.
	 * The items are added to the given vector.
	 */
	void ProcessPackage(rpmts ts, Header hdr, Object *obj, VerifyBehaviors beh,
						ItemVector *items);

	/**
	 * Given a partially complete item (it has name and filepath entities which
	 * are known to match the object), perform the verification and populate the
	 * rest of the item entities.
	 */
	void VerifyCompleteItem(Item* item, rpmts ts, rpmfi fileInfo, VerifyBehaviors beh);

	/**
	 * Reads the object's behaviors and populates a handy struct from it.
	 */
	VerifyBehaviors GetBehaviors(Object *obj);

}

//****************************************************************************************//
//								RPMVerifyProbe Class									  //	
//****************************************************************************************//
RPMVerifyProbe *RPMVerifyProbe::instance = NULL;

RPMVerifyProbe::RPMVerifyProbe() {

	// only should need to do this once
	if (rpmReadConfigFiles(NULL, NULL)) 
		throw ProbeException("Couldn't read rpm config files");
}

RPMVerifyProbe::~RPMVerifyProbe() {
  instance = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* RPMVerifyProbe::Instance() {

	// Use lazy initialization
	if(instance == NULL) 
		instance = new RPMVerifyProbe();

	return instance;	
}

ItemVector* RPMVerifyProbe::CollectItems(Object* object) {

	VectorPtrGuard<Item> collectedItems;
	ObjectEntity *nameObjEntity = object->GetElementByName("name");
	ObjectEntity *filepathObjEntity = object->GetElementByName("filepath");

	StringVector pkgNames;
	StringVector filePaths;
	/* OvalEnum::Flag flag; */
	/* flag =*/ nameObjEntity->GetEntityValues(pkgNames);
	/* flag =*/ filepathObjEntity->GetEntityValues(filePaths);

	if (nameObjEntity->GetOperation() == OvalEnum::OPERATION_EQUALS)
		ProcessPackagesByName(pkgNames, object, collectedItems.get());
	else if (filepathObjEntity->GetOperation() == OvalEnum::OPERATION_EQUALS) {
		// Ignore the package names in the name entity in this case, as far as
		// searching goes.  We search only the packages containing the given
		// files.  (The resulting items still get checked against the object.)
		pkgNames = GetPackagesForFilepaths(filePaths);
		ProcessPackagesByName(pkgNames, object, collectedItems.get());
	} else
		ProcessAllPackages(object, collectedItems.get());

	return collectedItems.release();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

/** this method is never called. */
Item* RPMVerifyProbe::CreateItem() {
	return NULL;
}

namespace {
	
	void ProcessPackagesByName(const StringVector &pkgNames,
							   Object *obj, ItemVector *items) {

		VerifyBehaviors beh = GetBehaviors(obj);

		for (StringVector::const_iterator iter = pkgNames.begin();
			 iter != pkgNames.end();
			 ++iter) {

			RpmtsGuard ts;
			RpmdbIterGuard pkgIter(ts, RPMTAG_NAME, iter->c_str(), 0);
			Header hdr;

			while ((hdr = rpmdbNextIterator(pkgIter)))
				ProcessPackage(ts, hdr, obj, beh, items);
		}
	}

	void ProcessAllPackages(Object *obj, ItemVector *items) {

		VerifyBehaviors beh = GetBehaviors(obj);

		RpmtsGuard ts;
		RpmdbIterGuard pkgIter(ts, (rpmTag)RPMDBI_PACKAGES, NULL, 0);
		Header hdr;
	
		while ((hdr = rpmdbNextIterator(pkgIter)))
			ProcessPackage(ts, hdr, obj, beh, items);
	}

	auto_ptr<Item> CreateItem() {
		auto_ptr<Item> item(new Item(0, 
									 "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#linux", 
									 "linux-sc", 
									 "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#linux linux-system-characteristics-schema.xsd", 
									 OvalEnum::STATUS_ERROR, 
									 "rpmverify_item"));

		return item;
	}

	StringVector GetPackagesForFilepaths(const StringVector &filePaths) {
		StringVector pkgNames;

		for (StringVector::const_iterator iter = filePaths.begin();
			 iter != filePaths.end();
			 ++iter) {
			
			RpmtsGuard ts;
			RpmdbIterGuard pkgIter(ts, RPMTAG_BASENAMES, iter->c_str(), 0);
			Header hdr;
			
			while ((hdr = rpmdbNextIterator(pkgIter))) {
				int_32 type;
				char *pkgName;
				int_32 numVals;

				if (!headerGetEntry(hdr, RPMTAG_NAME, &type, (void**)&pkgName, &numVals))
					throw ProbeException("Could not obtain a package name from the package header");

				pkgNames.push_back(pkgName);
			}
		}

		return pkgNames;
	}	

	void AddMessagesForFailures(Item *item, rpmVerifyAttrs attrs, bool checkErrno) {

		string errnoMsg;
		if (checkErrno) errnoMsg = string(": ") + strerror(errno);

		if (attrs & RPMVERIFY_LSTATFAIL)
			item->AppendMessage(new OvalMessage(string("lstat failed")+errnoMsg,
												OvalEnum::LEVEL_ERROR));
		if (attrs & RPMVERIFY_READFAIL)
			item->AppendMessage(new OvalMessage(string("read failed")+errnoMsg,
												OvalEnum::LEVEL_ERROR));
		if (attrs & RPMVERIFY_READLINKFAIL)
			item->AppendMessage(new OvalMessage(string("readlink failed")+errnoMsg,
												OvalEnum::LEVEL_ERROR));
		if (attrs & RPMVERIFY_LGETFILECONFAIL)
			item->AppendMessage(new OvalMessage(string("lgetfilecon failed")+errnoMsg,
												OvalEnum::LEVEL_ERROR));
	}

	void ProcessPackage(rpmts ts, Header hdr, Object *obj, VerifyBehaviors beh, 
						ItemVector *items) {

		int_32 type;
		char *pkgName;
		int_32 numVals;

		if (!headerGetEntry(hdr, RPMTAG_NAME, &type, (void**)&pkgName, &numVals))
			throw ProbeException ("Could not obtain a package name from the package header");
		
		RpmfiGuard fileInfo(ts, hdr, RPMTAG_BASENAMES, pkgName);

		if (!rpmfiInit(fileInfo, 0))
			throw ProbeException("Couldn't init file iterator on rpm '" + string(pkgName) + "'");

		while(rpmfiNext(fileInfo) > -1) {

			const char *fileName = rpmfiFN(fileInfo);

			// gotta check if this is a directory.  If the stat succeeds and
			// it is a directory, skip it, per the schema docs which say we
			// can't collect info about directories.  If the stat fails,
			// we'll continue since we can't tell (we'll get another failure
			// later on).
			struct stat sb;
			if (!lstat(fileName, &sb) && S_ISDIR(sb.st_mode))
				continue;

			// docs say that a return value of 0 means "invalid", but not
			// what exactly that means.  The rpmlib source doesn't seem to
			// treat it as an error; it's just ignored.
			int_32 fileFlags = rpmfiFFlags(fileInfo);

			if ((beh.noghost && (fileFlags & RPMFILE_GHOST)) ||
				(beh.noconfig && (fileFlags & RPMFILE_CONFIG)))
				continue;

			// with a filename and package name, we can create an item and
			// check it for a match with the object.
			auto_ptr<Item> item = CreateItem();
			item->SetStatus(OvalEnum::STATUS_EXISTS);
			item->AppendElement(new ItemEntity("name", pkgName, OvalEnum::DATATYPE_STRING, true));
			item->AppendElement(new ItemEntity("filepath", fileName, OvalEnum::DATATYPE_STRING, true));
			if (!obj->Analyze(item.get()))
				continue;

			VerifyCompleteItem(item.get(), ts, fileInfo, beh);
			items->push_back(item.release());
		}
	}

	void VerifyCompleteItem(Item* item, rpmts ts, rpmfi fileInfo, VerifyBehaviors beh) {

		// It seems some of rpm's enumerations are designed to overlap: the flag
		// and attr enumerations have some corresponding enumerators.  So I will
		// just use them interchangeably here.

		// the rpm source seems to treat their enumerations rather loosely,
		// using the enumeration type to not only represent one enumerator, but
		// also the bitwise or'ing of several of them (which would result in a
		// int value which does not correspond to an enumerator).  So I am just
		// mimicking what they do.
		rpmVerifyAttrs omitAttrs = (rpmVerifyAttrs)~beh.verifyFlags;
		rpmVerifyAttrs results = RPMVERIFY_NONE;
		
		// Certain tests are not done on certain types of files, regardless of
		// the package configuration or the behaviors on the object.  The logic
		// to determine which tests to actually perform is part of the source
		// code of the rpm tool, and exactly mimicking it would require pasting
		// in parts of their source code to the interpreter (or equivalent).  I
		// think that would make the behavior of this probe tied too tightly
		// with a particular version and/or port of the tool.  So I think I will
		// simply work off of the package configuration and behaviors.  That
		// means that some tests will get a "pass", even though they were never
		// actually performed (and so should have received a "not performed").
		int_32 pkgVerifyFlags = rpmfiVFlags(fileInfo);
		int_32 fileAttrs = rpmfiFFlags(fileInfo);

		errno = 0;
		if (rpmVerifyFile(ts, fileInfo, &results, omitAttrs)) {
			AddMessagesForFailures(item, results, true);

			// If the file doesn't exist, we still have to set all the item
			// entities to "not performed".  I'll take a shortcut and reset the
			// verify flags to all be switched off, so the code below will do
			// the right thing.
			pkgVerifyFlags = RPMVERIFY_NONE;

			// If the file is not allowed to be missing, set does not exist
			// status on the item.
			if (!(fileAttrs & RPMFILE_MISSINGOK))
				item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
		} else
			// The rpm code can proceed with verification in spite of earlier
			// errors.  So I don't think errno is a reliable source of info in
			// this case.
			AddMessagesForFailures(item, results, false);

		string entityVal;

// leaving the trailing ; off the macro definition and putting it at the end of
// the macro invocation lines allows emacs' auto-indent to work better... :-P
// this is for tests not capable of producing a '?' result (as output by the
// 'rpm' tool).
#define PROCESS_VERIFY_RESULT(_flag, _entityName)						\
		entityVal = "pass";												\
		if ((pkgVerifyFlags & (_flag)) && !(omitAttrs & (_flag))) {		\
			if (results & (_flag))										\
				entityVal = "fail";										\
		} else															\
			entityVal = "not performed";								\
		item->AppendElement(new ItemEntity((_entityName), entityVal))

// this is for tests capable of producing a '?' result.  The _unkFlag is the
// flag to use for determing '?', as opposed to pass/fail.
#define PROCESS_VERIFY_RESULT_UNK(_testFlag, _unkFlag, _entityName)		\
		entityVal = "pass";												\
		if ((pkgVerifyFlags & (_testFlag)) && !(omitAttrs & (_testFlag))) { \
			if (results & (_unkFlag))									\
				entityVal = "not performed";							\
			else if (results & (_testFlag))								\
				entityVal = "fail";										\
		} else															\
			entityVal = "not performed";								\
		item->AppendElement(new ItemEntity((_entityName), entityVal))

		PROCESS_VERIFY_RESULT(RPMVERIFY_FILESIZE, "size_differs");
		PROCESS_VERIFY_RESULT(RPMVERIFY_MODE, "mode_differs");
		PROCESS_VERIFY_RESULT_UNK(RPMVERIFY_MD5, RPMVERIFY_READFAIL, "md5_differs");
		PROCESS_VERIFY_RESULT(RPMVERIFY_RDEV, "device_differs");
		PROCESS_VERIFY_RESULT_UNK(RPMVERIFY_LINKTO, RPMVERIFY_READLINKFAIL, "link_mismatch");
		PROCESS_VERIFY_RESULT(RPMVERIFY_USER, "ownership_differs");
		PROCESS_VERIFY_RESULT(RPMVERIFY_GROUP, "group_differs");
		PROCESS_VERIFY_RESULT(RPMVERIFY_MTIME, "mtime_differs");

#undef PROCESS_VERIFY_RESULT
#undef PROCESS_VERIFY_RESULT_UNK

		// this test is not supported on the version of 'rpm' which comes with
		// RHEL5, so it's hard-coded to always have status=not collected.
		item->AppendElement(new ItemEntity("capabilities_differ", "", 
										   OvalEnum::DATATYPE_STRING, false, 
										   OvalEnum::STATUS_NOT_COLLECTED));

		item->AppendElement(new ItemEntity("configuration_file", 
										   (fileAttrs & RPMFILE_CONFIG) ? "true":"false",
										   OvalEnum::DATATYPE_BOOLEAN));
		item->AppendElement(new ItemEntity("documentation_file", 
										   (fileAttrs & RPMFILE_DOC) ? "true":"false",
										   OvalEnum::DATATYPE_BOOLEAN));
		item->AppendElement(new ItemEntity("ghost_file", 
										   (fileAttrs & RPMFILE_GHOST) ? "true":"false",
										   OvalEnum::DATATYPE_BOOLEAN));
		item->AppendElement(new ItemEntity("license_file", 
										   (fileAttrs & RPMFILE_LICENSE) ? "true":"false",
										   OvalEnum::DATATYPE_BOOLEAN));
		item->AppendElement(new ItemEntity("readme_file", 
										   (fileAttrs & RPMFILE_README) ? "true":"false",
										   OvalEnum::DATATYPE_BOOLEAN));
	}

	VerifyBehaviors GetBehaviors(Object *obj) {
		BehaviorVector *ovalBeh = obj->GetBehaviors();
		VerifyBehaviors ourBeh;
	
		if (Behavior::GetBehaviorValue(ovalBeh, "nodeps") == "true")
			ourBeh.verifyFlags &= ~VERIFY_DEPS;
		if (Behavior::GetBehaviorValue(ovalBeh, "nodigest") == "true")
			ourBeh.verifyFlags &= ~VERIFY_DIGEST;
		if (Behavior::GetBehaviorValue(ovalBeh, "nofiles") == "true")
			ourBeh.verifyFlags &= ~VERIFY_FILES;
		if (Behavior::GetBehaviorValue(ovalBeh, "noscripts") == "true")
			ourBeh.verifyFlags &= ~VERIFY_SCRIPT;
		if (Behavior::GetBehaviorValue(ovalBeh, "nosignature") == "true")
			ourBeh.verifyFlags &= ~VERIFY_SIGNATURE;
		if (Behavior::GetBehaviorValue(ovalBeh, "nolinkto") == "true")
			ourBeh.verifyFlags &= ~VERIFY_LINKTO;
		if (Behavior::GetBehaviorValue(ovalBeh, "nomd5") == "true")
			ourBeh.verifyFlags &= ~VERIFY_MD5;
		if (Behavior::GetBehaviorValue(ovalBeh, "nosize") == "true")
			ourBeh.verifyFlags &= ~VERIFY_SIZE;
		if (Behavior::GetBehaviorValue(ovalBeh, "nouser") == "true")
			ourBeh.verifyFlags &= ~VERIFY_USER;
		if (Behavior::GetBehaviorValue(ovalBeh, "nogroup") == "true")
			ourBeh.verifyFlags &= ~VERIFY_GROUP;
		if (Behavior::GetBehaviorValue(ovalBeh, "nomtime") == "true")
			ourBeh.verifyFlags &= ~VERIFY_MTIME;
		if (Behavior::GetBehaviorValue(ovalBeh, "nomode") == "true")
			ourBeh.verifyFlags &= ~VERIFY_MODE;
		if (Behavior::GetBehaviorValue(ovalBeh, "nordev") == "true")
			ourBeh.verifyFlags &= ~VERIFY_RDEV;
		if (Behavior::GetBehaviorValue(ovalBeh, "noconfigfiles") == "true")
			ourBeh.noconfig = true;
		if (Behavior::GetBehaviorValue(ovalBeh, "noghostfiles") == "true")
			ourBeh.noghost = true;

		// Warnings for unsupported behaviors...
		if (Behavior::GetBehaviorValue(ovalBeh, "nodeps") == "false")
			Log::Info("rpmverify: the 'nodeps' behavior is not supported.");
		if (Behavior::GetBehaviorValue(ovalBeh, "nodigest") == "false")
			Log::Info("rpmverify: the 'nodigest' behavior is not supported.");
		if (Behavior::GetBehaviorValue(ovalBeh, "nofiles") == "true")
			Log::Info("rpmverify: the 'nofiles' behavior is not supported.");
		if (Behavior::GetBehaviorValue(ovalBeh, "noscripts") == "false")
			Log::Info("rpmverify: the 'noscripts' behavior is not supported.");
		if (Behavior::GetBehaviorValue(ovalBeh, "nosignature") == "false")
			Log::Info("rpmverify: the 'nosignature' behavior is not supported.");

		return ourBeh;
	}

}
