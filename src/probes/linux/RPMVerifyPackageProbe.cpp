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

#include <cerrno>
#include <memory>
#include <VectorPtrGuard.h>
#include <rpm/rpmcli.h>
#include <rpm/rpmts.h>
#include <rpm/rpmdb.h>
#include <rpm/rpmlib.h>

/*
rpmts = transaction set
rpmps = problem set
rpmpsm = package state machine
rpmcli = command line interface
rpmdb = database
rpmlib = misc stuff...?
*/

#include <sys/stat.h>
#include "RPMVerifyPackageProbe.h"
#include <OvalMessage.h>
#include <OvalEnum.h>
#include <Behavior.h>
#include <Log.h>
#include <linux/RpmGuards.h>

using namespace std;

namespace {

          /**
   * Represents the selected behaviors in a form that is more easily
   * accessible, and interoperable with the rpm library.
   */
    struct VerifyBehaviors {
     /** defaults to checking everything */
     VerifyBehaviors():
      verifyFlags(VERIFY_DEFAULT){}

     /**
      * A bitmask, bits from the rpmVerifyFlags enumeration.  I am taking
      * advantage of an enum already built into the RPM library.
      */
     int verifyFlags;
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
	 * Given a package (header) and object and behaviors, create items for any
	 * package that matches the epoch, version, release, and architecture in the object.
	 * The items are added to the given vector.
	 */
	void ProcessPackage(rpmts ts, Header hdr, Object *obj, VerifyBehaviors beh,
						ItemVector *items);

	/**
	 * Given a partially complete item (it has a name entity which
	 * is known to match the object), perform the verification and populate the
	 * rest of the item entities.
	 */
        void VerifyCompleteItem(Item* item, rpmts ts, Header hdr, VerifyBehaviors beh);

	/**
	 * Reads the object's behaviors and populates a handy struct from it.
	 */
	VerifyBehaviors GetBehaviors(Object *obj);

}

//****************************************************************************************//
//								RPMVerifyPackageProbe Class									  //	
//****************************************************************************************//
RPMVerifyPackageProbe *RPMVerifyPackageProbe::instance = NULL;

RPMVerifyPackageProbe::RPMVerifyPackageProbe() {

	// only should need to do this once
	if (rpmReadConfigFiles(NULL, NULL)) 
		throw ProbeException("Couldn't read rpm config files");
}

RPMVerifyPackageProbe::~RPMVerifyPackageProbe() {
  instance = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* RPMVerifyPackageProbe::Instance() {

	// Use lazy initialization
	if(instance == NULL) 
		instance = new RPMVerifyPackageProbe();

	return instance;	
}

ItemVector* RPMVerifyPackageProbe::CollectItems(Object* object) {

	VectorPtrGuard<Item> collectedItems(new ItemVector());
	ObjectEntity *nameObjEntity = object->GetElementByName("name");

	StringVector pkgNames;
	/* OvalEnum::Flag flag; */
	/* flag =*/ nameObjEntity->GetEntityValues(pkgNames);

	if (nameObjEntity->GetOperation() == OvalEnum::OPERATION_EQUALS){
		ProcessPackagesByName(pkgNames, object, collectedItems.get());
	} else
		ProcessAllPackages(object, collectedItems.get());

	return collectedItems.release();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

/** this method is never called. */
Item* RPMVerifyPackageProbe::CreateItem() {
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
									 "rpmverifypackage_item"));

		return item;
	}

	void ProcessPackage(rpmts ts, Header hdr, Object *obj, VerifyBehaviors beh, 
						ItemVector *items) {

		int_32 type;
		char *pkgName;
		int_32 numVals;

		if (!headerGetEntry(hdr, RPMTAG_NAME, &type, (void**)&pkgName, &numVals))
			throw ProbeException ("Could not obtain a package name from the package header");
	
		//check additional object entities
		OvalEnum::SCStatus verStatus, relStatus, archStatus;
		verStatus = relStatus = archStatus = OvalEnum::STATUS_EXISTS;
		
		string installed_epoch, installed_version, installed_release, installed_architecture, installed_extended_name;
		int_32 epochType, versionType, releaseType, archType;
		int_32 *epochHdrVal;
		void *versionHdrVal, *releaseHdrVal, *archHdrVal;
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
		(installed_release.empty() ? "0":installed_release) + "." + installed_architecture;

		auto_ptr<Item> item = CreateItem();
		item->SetStatus(OvalEnum::STATUS_EXISTS);
		item->AppendElement(new ItemEntity("name", pkgName, OvalEnum::DATATYPE_STRING, true));
		item->AppendElement(new ItemEntity("epoch", installed_epoch, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
		item->AppendElement(new ItemEntity("version", installed_version, OvalEnum::DATATYPE_STRING, true, verStatus));
		item->AppendElement(new ItemEntity("release", installed_release, OvalEnum::DATATYPE_STRING, true, relStatus));
		item->AppendElement(new ItemEntity("arch", installed_architecture, OvalEnum::DATATYPE_STRING, true, archStatus));
		item->AppendElement(new ItemEntity("extended_name", installed_extended_name, OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));
			
		// check it for a match with the object and if so verify it.
		if (obj->Analyze(item.get())){
		  VerifyCompleteItem(item.get(), ts, hdr, beh);
		  items->push_back(item.release());
		}
	}

  void VerifyCompleteItem(Item* item, rpmts ts, Header hdr, VerifyBehaviors beh) {
               
                rpmVerifyFlags omitAttrs = (rpmVerifyFlags)~beh.verifyFlags;

		errno = 0;
		QVA_t qva = &rpmQVKArgs;
		int verifyPkgResult; // will be 0 on success
		if ( omitAttrs & VERIFY_DEPS ){
		  // Check the rpm's dependencies
		  qva->qva_flags = (rpmQueryFlags) VERIFY_DEPS;
		  verifyPkgResult = showVerifyPackage(qva, ts, hdr);  //unfortunately this outputs a message to stdout.
		  item->AppendMessage(new OvalMessage("NOTE: the showVerifyPackage() function used to collect the dependency_check_passed entity prints a message to stdout if it fails. The message looks like 'Unsatisfied dependencies for (rpm): (dependency_1),...,(dependency_n).",OvalEnum::LEVEL_INFO));

		  item->AppendElement(new ItemEntity("dependency_check_passed", (!verifyPkgResult)?"true":"false",
						   OvalEnum::DATATYPE_BOOLEAN, false,OvalEnum::STATUS_EXISTS));
		}else{
		  // Otherwise don't collect it
		  item->AppendElement(new ItemEntity("dependency_check_passed","",OvalEnum::DATATYPE_BOOLEAN, false,OvalEnum::STATUS_NOT_COLLECTED));
		}

		// Check the rpm's digest. This is not collected because you need to check it against the digest in the 
		// .rpm file which is not supported in the object.
		item->AppendMessage(new OvalMessage("The digest_check_passed entity is not collected because you need to check it against the digest in the .rpm file which is not supported in the object.",OvalEnum::LEVEL_INFO));
		item->AppendElement(new ItemEntity("digest_check_passed", "",
						   OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_NOT_COLLECTED));

		if ( omitAttrs & VERIFY_SCRIPT ){
		// Check the rpm's verification script
	        qva->qva_flags = (rpmQueryFlags) VERIFY_SCRIPT;
		verifyPkgResult = showVerifyPackage(qva, ts, hdr);
		item->AppendElement(new ItemEntity("verification_script_successful", (!verifyPkgResult)?"true":"false",
						   OvalEnum::DATATYPE_BOOLEAN, false,OvalEnum::STATUS_EXISTS));
		}else{
                  // Otherwise don't collect it
                  item->AppendElement(new ItemEntity("verification_script_successful","",OvalEnum::DATATYPE_BOOLEAN, false,OvalEnum::STATUS_NOT_COLLECTED));
                }

		// Check the rpm's signature. This is not collected because you need to check it against the signature in the 
		// .rpm file which is not supported in the object.
                item->AppendMessage(new OvalMessage("The signature_check_passed entity is not collected because you need to check it against the digest in the .rpm file which is not supported in the object.",OvalEnum::LEVEL_INFO));

		item->AppendElement(new ItemEntity("signature_check_passed", "",
						   OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_NOT_COLLECTED));
	}

	VerifyBehaviors GetBehaviors(Object *obj) {
		BehaviorVector *ovalBeh = obj->GetBehaviors();
		VerifyBehaviors ourBeh;
	
		if (Behavior::GetBehaviorValue(ovalBeh, "nodeps") == "true")
			ourBeh.verifyFlags |= VERIFY_DEPS;
		if (Behavior::GetBehaviorValue(ovalBeh, "nodigest") == "true"){
		  ourBeh.verifyFlags |= VERIFY_DIGEST;
		  Log::Info("rpmverify: the 'nodigest' behavior is not supported.");
		}
		if (Behavior::GetBehaviorValue(ovalBeh, "noscripts") == "true")
			ourBeh.verifyFlags |= VERIFY_SCRIPT;
		if (Behavior::GetBehaviorValue(ovalBeh, "nosignature") == "true"){
			ourBeh.verifyFlags |= VERIFY_SIGNATURE;
			Log::Info("rpmverify: the 'nosignature' behavior is not supported.");
		}
		
		return ourBeh;
	}

}
