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

#include <memory>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <linux/RpmGuards.h>
#include <VectorPtrGuard.h>
#include "RPMInfoProbe.h"

using namespace std;

namespace {
	auto_ptr<Item> CreateItem();
}

// Path to the 'rpm' tool.
// Make sure this is an absolute path, to make
// sure you are not accidentally running a
// malicious app!
#define RPM_PATH "/bin/rpm"

#define RPM_COMMANDLINE RPM_PATH " -q --qf '%{SIGGPG:pgpsig}'"

static char rpmSigkeyidBuf[1024];

//****************************************************************************************//
//								RPMInfoProbe Class										  //
//****************************************************************************************//
RPMInfoProbe *RPMInfoProbe::instance = NULL;

RPMInfoProbe::RPMInfoProbe() {
	/* Read in the RPM config files */
	if (rpmReadConfigFiles( (const char*) NULL, (const char*) NULL))
		throw ProbeException("Error: (RPMInfoProbe) Could not read RPM config files, which is necessary to read the RPM database.");

	// make sure the rpm tool to run exists.
	struct stat st;
	if (stat(RPM_PATH, &st) == -1) {
		throw ProbeException(string("The 'rpm' tool is required to get signature_keyid values.  "
									"stat(\"" RPM_PATH "\"): ") + strerror(errno));
	}	
}

RPMInfoProbe::~RPMInfoProbe() {
  instance = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* RPMInfoProbe::Instance() {

	// Use lazy initialization
	if(instance == NULL)
		instance = new RPMInfoProbe();

	return instance;
}

ItemVector* RPMInfoProbe::CollectItems(Object* object) {

	ObjectEntity* name = object->GetElementByName("name");

	// check datatypes - only allow string
	if(name->GetDatatype() != OvalEnum::DATATYPE_STRING) {
		throw ProbeException("Error: invalid data type specified on name. Found: " + OvalEnum::DatatypeToString(name->GetDatatype()));
	}

	// check operation - only allow  equals, not equals and pattern match
	if(name->GetOperation() != OvalEnum::OPERATION_EQUALS && name->GetOperation() != OvalEnum::OPERATION_PATTERN_MATCH && name->GetOperation() != OvalEnum::OPERATION_NOT_EQUAL) {
		throw ProbeException("Error: invalid operation specified on name. Found: " + OvalEnum::OperationToString(name->GetOperation()));
	}

	VectorPtrGuard<Item> collectedItems(new ItemVector());
	BehaviorVector *beh = object->GetBehaviors();

	StringVector names;
	this->GetRPMNames(name, &names);
	if(!names.empty()) {
		StringVector::iterator iterator;
		for(iterator = names.begin(); iterator != names.end(); iterator++) {
			GetRPMInfo((*iterator), collectedItems.get(), beh);
		}
	} else {

		if(name->GetOperation() == OvalEnum::OPERATION_EQUALS) {

			if(name->GetVarRef() == NULL) {

				auto_ptr<Item> item = ::CreateItem();
				item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
				item->AppendElement(new ItemEntity("name", name->GetValue(), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));
				collectedItems->push_back(item.release());

			} else {

				VariableValueVector::iterator iterator;
				for(iterator = name->GetVarRef()->GetValues()->begin(); iterator != name->GetVarRef()->GetValues()->end(); iterator++) {

					auto_ptr<Item> item = ::CreateItem();
					item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
					item->AppendElement(new ItemEntity("name", (*iterator)->GetValue(), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));
					collectedItems->push_back(item.release());
				}
			}
		}
	}

	return collectedItems.release();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

// Not used.  I want to return an auto_ptr to prevent memory leaks, and I can't
// change this method's return type.
Item* RPMInfoProbe::CreateItem() {
	return NULL;
}

void RPMInfoProbe::GetRPMNames(ObjectEntity* name, StringVector *names) {
	// -----------------------------------------------------------------------
	//
	//  ABSTRACT
	//
	//  Get the set of all rpm names on the system that match the object
	//
	// -----------------------------------------------------------------------

	// does this name use variables?
	if(name->GetVarRef() == NULL) {

		// proceed based on operation
		if(name->GetOperation() == OvalEnum::OPERATION_EQUALS) {
			// if the name exists add it to the list
			if(this->RPMExists(name->GetValue())) {
				names->push_back(name->GetValue());
			}

		} else if(name->GetOperation() == OvalEnum::OPERATION_NOT_EQUAL) {

			this->GetMatchingRPMNames(name->GetValue(), false, names);

		} else if(name->GetOperation() == OvalEnum::OPERATION_PATTERN_MATCH) {
			this->GetMatchingRPMNames(name->GetValue(), true, names);
		}

	} else {

		if(name->GetOperation() == OvalEnum::OPERATION_EQUALS) {
			// in the case of equals simply loop through all the
			// variable values and add them to the set of all names
			// if they exist on the system
			VariableValueVector::iterator iterator;
			for(iterator = name->GetVarRef()->GetValues()->begin(); iterator != name->GetVarRef()->GetValues()->end(); iterator++) {

				if(this->RPMExists((*iterator)->GetValue())) {
					names->push_back((*iterator)->GetValue());
				}
			}

		} else {
			this->GetMatchingRPMNames(".*", true, names);
		}

		// loop through all names on the system
		// only keep names that match operation and value and var check
		ItemEntity* tmp = this->CreateItemEntity(name);
		StringVector::iterator it;
		for(it = names->begin(); it != names->end(); ) {
			tmp->SetValue((*it));

			if(name->Analyze(tmp) == OvalEnum::RESULT_TRUE)
				++it;
			else
				it = names->erase(it);
		}
	}
}

void RPMInfoProbe::GetMatchingRPMNames(string pattern, bool isRegex, StringVector *names) {
	// -----------------------------------------------------------------------
	//
	//  ABSTRACT
	//
	//  Get the set of all rpm names on the system that match the pattern
	//
	// -----------------------------------------------------------------------

	/* Header object for the installed package. */
	Header header;

	string installed_rpm_name;

	/* Create an rpm database transaction set. */
	RpmtsGuard ts;

	/* Create an iterator to walk the database. */
	RpmdbIterGuard iterator(ts, RPMTAG_NAME, NULL, 0);

	/* Look at each installed package matching this name.  Generally, there is only one.*/
	while ( (header = rpmdbNextIterator(iterator)) != NULL) {
		/* Get the rpm_name value for comparision. */
		if (!readHeaderString(header, RPMTAG_NAME, &installed_rpm_name))
			throw ProbeException("Encountered an rpm without a name!");

		/* Check to see if name found matches input pattern. */
		if(this->IsMatch(pattern, installed_rpm_name, isRegex)) {
			names->push_back(installed_rpm_name);
		}
	}
}

bool RPMInfoProbe::RPMExists(string name) {
	// -----------------------------------------------------------------------
	//
	//  ABSTRACT
	//
	//  return true if the specified rpm exists
	//
	// -----------------------------------------------------------------------
	bool exists = false;

	/* Header object for the installed package. */
	Header header;

	string installed_rpm_name;

	/* Create an rpm database transaction set. */
	RpmtsGuard ts;

	/* Create an iterator to walk the database. */
	RpmdbIterGuard iterator(ts, RPMTAG_NAME, NULL, 0);

	/* Look at each installed package matching this name.  Generally, there is only one.*/
	while ( (header = rpmdbNextIterator(iterator)) != NULL && !exists) {
		/* Get the rpm_name value for comparision. */
		if (!readHeaderString(header, RPMTAG_NAME, &installed_rpm_name))
			throw ProbeException("Encountered an rpm without a name!");

		/* Check to see if name found matches input pattern. */
		if(name == installed_rpm_name)
			exists = true;
	}

	return exists;
}

void RPMInfoProbe::GetRPMInfo(string name, ItemVector* items, BehaviorVector* beh) {
	//------------------------------------------------------------------------------------//
	//
	//  ABSTRACT
	//
	//  Get the data for all packages that have the given name.
	//
	//------------------------------------------------------------------------------------//
	
	/* Header object for the installed package. */
	Header header;
	/* Epoch, version, release and architecture data for output. */
	string installed_epoch, installed_version, installed_release,installed_architecture, 
		installed_evr, installed_signature_keyid, installed_extended_name;

	/* Create an rpm database transaction set. */
	RpmtsGuard ts;

	/* Create an iterator to walk the database. */
	RpmdbIterGuard iterator(ts, RPMTAG_NAME, name.c_str(), 0);

	/* Look at each installed package matching this name.  Generally, there is only one.*/
	while ( (header = rpmdbNextIterator(iterator)) != NULL) {

		/* epoch is an int_32 -- we'll display a string to handle the None case well. */
		int_32 epoch = readHeaderInt32(header, RPMTAG_EPOCH);
		string installedEpochEvr;
		if (epoch == -1 ) {
			installed_epoch = "(none)";
			installedEpochEvr = "0";
		} else {
			installed_epoch = Common::ToString(epoch);
			installedEpochEvr = installed_epoch;
		}

		OvalEnum::SCStatus verStatus, relStatus, archStatus, keyidStatus;
		verStatus = relStatus = archStatus = keyidStatus = OvalEnum::STATUS_EXISTS;

		/* the remaining arguments are all normal strings */
		if (!this->readHeaderString(header, RPMTAG_VERSION, &installed_version))
			verStatus = OvalEnum::STATUS_DOES_NOT_EXIST;
		if (!this->readHeaderString(header, RPMTAG_RELEASE, &installed_release))
			relStatus = OvalEnum::STATUS_DOES_NOT_EXIST;
		if (!this->readHeaderString(header, RPMTAG_ARCH, &installed_architecture))
			archStatus = OvalEnum::STATUS_DOES_NOT_EXIST;

		installed_signature_keyid = this->GetSigKeyId(name);
		if (installed_signature_keyid.empty())
			keyidStatus = OvalEnum::STATUS_DOES_NOT_EXIST;

		installed_evr = installedEpochEvr + ":" +
			(installed_version.empty() ? "0":installed_version) + '-' +
			(installed_release.empty() ? "0":installed_release);

		installed_extended_name = name + "-" + installedEpochEvr + ":" +
			(installed_version.empty() ? "0":installed_version) + '-' +
			(installed_release.empty() ? "0":installed_release) + "." + 
			installed_architecture;
		/* Put the data in a data object. */
		auto_ptr<Item> item = ::CreateItem();
		item->SetStatus(OvalEnum::STATUS_EXISTS);
		item->AppendElement(new ItemEntity("name", name, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
		item->AppendElement(new ItemEntity("arch", installed_architecture, OvalEnum::DATATYPE_STRING, archStatus));
		item->AppendElement(new ItemEntity("epoch", installed_epoch, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
		item->AppendElement(new ItemEntity("release", installed_release, OvalEnum::DATATYPE_STRING, relStatus));
		item->AppendElement(new ItemEntity("version", installed_version, OvalEnum::DATATYPE_STRING, verStatus));
		item->AppendElement(new ItemEntity("evr", installed_evr, OvalEnum::DATATYPE_EVR_STRING, OvalEnum::STATUS_EXISTS));
		item->AppendElement(new ItemEntity("signature_keyid", installed_signature_keyid, OvalEnum::DATATYPE_STRING, keyidStatus));
		item->AppendElement(new ItemEntity("extended_name", installed_extended_name, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));

		if (Behavior::GetBehaviorValue(beh, "filepaths") == "true"){
			RpmfiGuard fileInfo(ts, header, RPMTAG_BASENAMES, name);
			if (!rpmfiInit(fileInfo, 0))
				throw ProbeException("Couldn't init file iterator on rpm '" + name + "'");

			while(rpmfiNext(fileInfo) > -1) {
				const char *fileName = rpmfiFN(fileInfo);
				item->AppendElement(new ItemEntity("filepath", fileName, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
			}
		}

		/* add the new item to the vector. */
		items->push_back(item.release());
	}
}

bool RPMInfoProbe::readHeaderString(Header header, int_32 tag_id, string *val) {
	// This function is from the Red Hat RPM Guide //
	int_32 type;
	void *pointer;
	int_32 data_size;

	int header_status = headerGetEntry(header,
									   tag_id,
									   &type,
									   &pointer,
									   &data_size);

	if (header_status) {
		if (type == RPM_STRING_TYPE) {
			*val = (char *) pointer;
			return true;
		}
	}

	return false;
}

int_32 RPMInfoProbe::readHeaderInt32(Header header, int_32 tag_id) {
  // This function is from the Red Hat RPM Guide //
  int_32 type;
  void *pointer;
  int_32 data_size;

  int header_status = headerGetEntry(header,
				     tag_id,
				     &type,
				     (void **) &pointer,
				     &data_size);

  if (header_status) {
    if (type == RPM_INT32_TYPE) {
      int_32 *p = (int_32 *) pointer;
      return *p;
    }
  }
  return( -1 );
}

string RPMInfoProbe::GetSigKeyId(string rpmName) {

 	string sigKeyId;
	FILE *rpmFp;
	string completeCommandLine = RPM_COMMANDLINE + (" " + rpmName);

	Log::Debug("Running: "+completeCommandLine);

	if ((rpmFp = popen(completeCommandLine.c_str(), "r")) == NULL) {
		throw ProbeException("While getting signature_keyid: popen(" +
							 completeCommandLine + "): " +
							 strerror(errno));
	}
	
	if (fgets(rpmSigkeyidBuf, sizeof(rpmSigkeyidBuf), rpmFp) == NULL) {
		int origErrno = errno;
		// lets slurp up any remaining data from the stream,
		// ignoring errors...
		while(fgetc(rpmFp) != EOF) ;
		// just log a pclose error since we already have a
		// different one to report...
		if (pclose(rpmFp) == -1)
			Log::Debug("pclose(" + completeCommandLine + ") error: " +
					   strerror(errno));
		throw ProbeException("Error reading output of " + completeCommandLine +
							 ": " + strerror(origErrno));
	}

	//Log::Debug(string("Got result: ") + rpmSigkeyidBuf);

	// same cleanup code as above...
	// except this time, pclose() error will result in
	// a thrown exception.
	while(fgetc(rpmFp) != EOF) ;
	if (pclose(rpmFp) == -1)
		throw ProbeException("pclose(" + completeCommandLine + ") error: " +
					   strerror(errno));
	
	size_t len = strlen(rpmSigkeyidBuf);
	if (len >= 16) {
		// the 'rpm' tool can print out all sorts of
		// messages; find a key ID in it.
		REGEX re;
		vector<StringVector> matches;
		re.GetAllMatchingSubstrings(
			"Key ID ([0-9a-fA-F]{16})",
			rpmSigkeyidBuf,
			matches);
		if (!matches.empty() && matches[0].size() > 1)
			sigKeyId = matches[0][1];
	}

	return sigKeyId;
}

namespace {
	auto_ptr<Item> CreateItem() {

		// -----------------------------------------------------------------------
		//
		//  ABSTRACT
		//
		//  Return a new Item created for storing file information
		//
		// -----------------------------------------------------------------------

		auto_ptr<Item> item(new Item(0,
									 "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#linux",
									 "linux-sc",
									 "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#linux linux-system-characteristics-schema.xsd",
									 OvalEnum::STATUS_ERROR,
									 "rpminfo_item"));

		return item;
	}
}
