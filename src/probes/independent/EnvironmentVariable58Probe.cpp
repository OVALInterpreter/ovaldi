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

#ifdef LINUX
#  include <stdlib.h>
#  include <sys/types.h>
#  include <dirent.h>
#  include <unistd.h>
#  include <cerrno>
#  include <cstring>
#  include <map>
#  include <memory>

#  include <DirGuard.h>
#  include <VectorPtrGuard.h>
#endif

/*
#ifdef DARWIN
#include <crt_externs.h>
#define _environ (*_NSGetEnviron())
#elif !defined WIN32
extern char ** _environ; 
#endif
*/

#include "EnvironmentVariable58Probe.h"

using namespace std;

#ifdef LINUX

#define PROC_DIR "/proc"

namespace {

	auto_ptr<Item> CreateItem();

	/**
	 * Gets all PIDs on this system.  Gets them as strings
	 * because this code doesn't directly need to perform
	 * numeric ops on them, but does need to do string ops.
     */
	StringVector GetAllPids();

	/**
	 * Gets the environment of the process with the given pid.
	 */
	map<string, string> GetEnvForPid(const string &pid);
}
#endif

//****************************************************************************************//
//								EnvironmentVariableProbe Class							  //	
//****************************************************************************************//
EnvironmentVariable58Probe *EnvironmentVariable58Probe::instance = NULL;

EnvironmentVariable58Probe::EnvironmentVariable58Probe() {
}

EnvironmentVariable58Probe::~EnvironmentVariable58Probe() {
  instance = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* EnvironmentVariable58Probe::Instance() {

	// Use lazy initialization
	if(instance == NULL) 
		instance = new EnvironmentVariable58Probe();

	return instance;	
}

ItemVector* EnvironmentVariable58Probe::CollectItems(Object *object) {

#ifdef LINUX

	VectorPtrGuard<Item> collectedItems(new ItemVector());

	// get the name from the provided object
	ObjectEntity *name = object->GetElementByName("name");
	ObjectEntity *pid = object->GetElementByName("pid");

	StringVector pidsToCheck;
	if (pid->GetNil()) // nil means to check the current process.
		pidsToCheck.push_back(Common::ToString(getpid()));
	else if (pid->GetOperation() == OvalEnum::OPERATION_EQUALS)
		/*OvalEnum::Flag flag =*/ pid->GetEntityValues(pidsToCheck);
	else
		pidsToCheck = GetAllPids();

	for (StringVector::iterator pidIter = pidsToCheck.begin();
		 pidIter != pidsToCheck.end();
		 ++pidIter) {

		ItemEntity pidIe("pid", *pidIter, OvalEnum::DATATYPE_INTEGER);
		if (pid->GetNil() || pid->Analyze(&pidIe) == OvalEnum::RESULT_TRUE) {

			map<string, string> envForPid = GetEnvForPid(*pidIter);

			// let ENOENT go, since processes can appear and disappear at any time.
			if (envForPid.empty() && errno && errno != ENOENT) {
				auto_ptr<Item> item = ::CreateItem();
				item->SetStatus(OvalEnum::STATUS_ERROR);
				item->AppendElement(new ItemEntity(pidIe));
				item->AppendMessage(new OvalMessage(
										"Error reading " PROC_DIR "/"+
										*pidIter+"/environ: "+strerror(errno),
										OvalEnum::LEVEL_ERROR));
				collectedItems->push_back(item.release());
				continue;
			}

			for (map<string,string>::iterator envIter = envForPid.begin();
				 envIter != envForPid.end();
				 ++envIter) {
				
				auto_ptr<ItemEntity> nameIe(new ItemEntity("name", envIter->first));
				if (name->Analyze(nameIe.get()) == OvalEnum::RESULT_TRUE) {

					auto_ptr<Item> item = ::CreateItem();
					item->SetStatus(OvalEnum::STATUS_EXISTS);
					item->AppendElement(new ItemEntity(pidIe));
					item->AppendElement(nameIe.release());
					item->AppendElement(new ItemEntity("value", envIter->second));
					collectedItems->push_back(item.release());
				}
			}
		}
	}

	return collectedItems.release();

#else

	throw ProbeException("environmentvariable58 test is not supported on this platform.");

#endif
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

// this is never used
Item* EnvironmentVariable58Probe::CreateItem() {
	return NULL;
}

#ifdef LINUX

namespace {

	auto_ptr<Item> CreateItem() {

		auto_ptr<Item> item(new Item(0, 
									 "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#independent", 
									 "ind-sc", 
									 "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#independent independent-system-characteristics-schema.xsd", 
									 OvalEnum::STATUS_ERROR, 
									 "environmentvariable58_item"));

		return item;
	}

	StringVector GetAllPids() {
		StringVector pids;
		DirGuard procDir(PROC_DIR);
		dirent *entry;

		errno = 0;
		while((entry = readdir(procDir)) != NULL) {
			if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
				continue;

			// find the names made up of all digits
			size_t i;
			for (i=0; i<strlen(entry->d_name) && isdigit(entry->d_name[i]); ++i)
				;

			if (i < strlen(entry->d_name))
				continue;

			pids.push_back(entry->d_name);
		}

		if (errno)
			throw ProbeException(string("Error while reading " PROC_DIR ": ") + strerror(errno));

		return pids;
	}

	map<string, string> GetEnvForPid(const string &pid) {
		string envFile = Common::BuildFilePath(
			Common::BuildFilePath(PROC_DIR, pid),
			"environ");
		map<string, string> env;

		// On linux, the format of the environ file is:
		// <entry>\0<entry>\0 ... <entry>\0
		// Normally formatted entries look like <name>=<val>.

		string name, val;
		bool inName = true;
		errno = 0;
		// binary mode.  I was able to get binary junk into
		// an env variable value via setenv().  So I will
		// not assume values are text, and just read raw bytes out.
		ifstream in(envFile.c_str(), ios::in|ios::binary);
		while (in) {
			int c = in.get();
			if (in.good()) {
				if (c == '=')
					inName = false;
				else if (c) {
					if (inName) name += (char)c;
					else val += (char)c;
				} else {
					if (inName)
						// The following actually produces a lot of spam.
						// There seem to be a lot of /proc/<pid>/environ
						// files for processes with weird stuff in them which
						// does not conform to the normal rules for environment
						// variables.  So I commented it out.
//						Log::Debug("Found env value for pid "+pid+": "+name+" with no '=' in it!  Ignoring...");
						;
					
					else {
						env[name] = val;
//						Log::Debug("found env var for pid "+pid+": "+name+" = "+val);
					}

					name.clear();
					val.clear();
					inName = true;
				}
			}
		}

		return env;
	}

}

#endif
