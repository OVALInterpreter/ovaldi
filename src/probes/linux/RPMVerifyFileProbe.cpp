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

#include <cerrno>
#include <memory>

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

#include <VectorPtrGuard.h>
#include <OvalMessage.h>
#include <OvalEnum.h>
#include <Behavior.h>
#include <Log.h>
#include <linux/RpmGuards.h>

#include "RPMVerifyFileProbe.h"

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
//								RPMVerifyFileProbe Class									  //	
//****************************************************************************************//
RPMVerifyFileProbe *RPMVerifyFileProbe::instance = NULL;

RPMVerifyFileProbe::RPMVerifyFileProbe() {

	// only should need to do this once
	if (rpmReadConfigFiles(NULL, NULL)) 
		throw ProbeException("Couldn't read rpm config files");
}

RPMVerifyFileProbe::~RPMVerifyFileProbe() {
  instance = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* RPMVerifyFileProbe::Instance() {

	// Use lazy initialization
	if(instance == NULL) 
		instance = new RPMVerifyFileProbe();

	return instance;	
}

ItemVector* RPMVerifyFileProbe::CollectItems(Object* object) {

	VectorPtrGuard<Item> collectedItems(new ItemVector());
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
Item* RPMVerifyFileProbe::CreateItem() {
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
		auto_ptr<Item> item(new Item(0, "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#linux", 
			"linux-sc", 
			"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#linux linux-system-characteristics-schema.xsd",  
			OvalEnum::STATUS_ERROR,  "rpmverifyfile_item"));

		return item;
	}

	StringVector GetPackagesForFilepaths(const StringVector &filePaths) {
		StringVector pkgNames;

		for (StringVector::const_iterator iter = filePaths.begin();
			 iter != filePaths.end();
			 ++iter) {
			
			RpmtsGuard ts;

			// If the file doesn't exist in a package, you don't seem to be able
			// to create an iterator as is done below.  So I will try it first,
			// and if it doesn't work, skip the file.  I don't know if there are
			// other reasons this might fail, that I should check for somehow...
			rpmdbMatchIterator dbIter = rpmtsInitIterator(ts, RPMTAG_BASENAMES, iter->c_str(), 0);
			if (!dbIter)
				continue;
			
			RpmdbIterGuard pkgIter(dbIter);
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

		string installed_epoch, installed_version, installed_release, installed_architecture, installed_extended_name;
		int_32 epochType, versionType, releaseType, archType;
		int_32 *epochHdrVal;
		void *versionHdrVal, *releaseHdrVal, *archHdrVal;
		//check additional object entities
		OvalEnum::SCStatus verStatus, relStatus, archStatus;
		verStatus = relStatus = archStatus = OvalEnum::STATUS_EXISTS;
			
		if(headerGetEntry(hdr, RPMTAG_EPOCH, &epochType, (void **) &epochHdrVal, &numVals) && (epochType == RPM_INT32_TYPE)){
			installed_epoch = Common::ToString((int_32)*epochHdrVal);
		}
		else {
			installed_epoch = "(none)";
		}
		if(headerGetEntry(hdr, RPMTAG_VERSION, &versionType, &versionHdrVal, &numVals) && (versionType == RPM_STRING_TYPE)){
			installed_version = (char *) versionHdrVal;
		}
		else {
			verStatus = OvalEnum::STATUS_DOES_NOT_EXIST;
		}
		if(headerGetEntry(hdr, RPMTAG_RELEASE, &releaseType, &releaseHdrVal, &numVals) && (releaseType == RPM_STRING_TYPE)){
			installed_release = (char *) releaseHdrVal;
		}
		else {
			relStatus = OvalEnum::STATUS_DOES_NOT_EXIST;
		}
		if(headerGetEntry(hdr, RPMTAG_ARCH, &archType, &archHdrVal, &numVals) && (archType == RPM_STRING_TYPE)){
			installed_architecture = (char *) archHdrVal;
		}
		else {
			archStatus = OvalEnum::STATUS_DOES_NOT_EXIST;
		}
		
		installed_extended_name = std::string(pkgName) + "-" + (installed_epoch.compare("(none)")==0 ? "0":installed_epoch) + ":" +
		(installed_version.empty() ? "0":installed_version) + '-' +
		(installed_release.empty() ? "0":installed_release) + "." + 
		installed_architecture;

		while(rpmfiNext(fileInfo) > -1) {

			const char *fileName = rpmfiFN(fileInfo);

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
			item->AppendElement(new ItemEntity("name", pkgName, OvalEnum::DATATYPE_STRING));
			item->AppendElement(new ItemEntity("epoch", installed_epoch, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
			item->AppendElement(new ItemEntity("version", installed_version, OvalEnum::DATATYPE_STRING, verStatus));
			item->AppendElement(new ItemEntity("release", installed_release, OvalEnum::DATATYPE_STRING, relStatus));
			item->AppendElement(new ItemEntity("arch", installed_architecture, OvalEnum::DATATYPE_STRING, archStatus));
			item->AppendElement(new ItemEntity("filepath", fileName, OvalEnum::DATATYPE_STRING));
			item->AppendElement(new ItemEntity("extended_name", installed_extended_name, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
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
										   OvalEnum::DATATYPE_STRING, 
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

		return ourBeh;
	}

}
