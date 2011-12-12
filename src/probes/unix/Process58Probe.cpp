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

#ifdef LINUX
#  include <selinux/selinux.h>
#  include <selinux/context.h>
#  include <sys/capability.h>
#  include <SecurityContextGuard.h>
#endif

#include <fstream>
#include <memory>
#include <vector>

#include <Log.h>
#include "Process58Probe.h"

using namespace std;

//****************************************************************************************//
//				 Process58Probe Class	                                  //
//****************************************************************************************//

Process58Probe *Process58Probe::instance = NULL;

#ifdef SUNOS

namespace {

	// private stuff used to find tty device paths on solaris.
	string devPath;
	dev_t dev;

	int searcher(const char *filename, const struct stat *st, int filetype) {
		if (filetype != FTW_F || !S_ISCHR(st->st_mode) || dev != st->st_rdev)
			return 0;
		
		devPath = filename;
		return 1;
	}
}

#endif

#ifdef LINUX
namespace {
	/**
	 * Maps integral capability constants to string names as required by the
	 * OVAL schema.
	 */
	vector<string> capMap;
}
#endif


Process58Probe::Process58Probe() {
#ifdef LINUX

	// Implicit in this map idea is the assumption that none of these integral
	// constants have the same value.
	// The following macro allows us to more easily create a vector of just the
	// right size to map all capability constants to strings.  I decided a
	// vector makes more sense than a map when the keys are plain integers.
#define ADD_CAP(_x)								\
    if (capMap.size() <= (_x))					\
		capMap.resize((_x)+1);					\
	capMap[(_x)] = #_x


	// so the repeated resizes will hopefully not require any reallocation.
	capMap.reserve(50);

	ADD_CAP(CAP_CHOWN);
	ADD_CAP(CAP_DAC_OVERRIDE);
	ADD_CAP(CAP_DAC_READ_SEARCH);
	ADD_CAP(CAP_FOWNER);
	ADD_CAP(CAP_FSETID);
	ADD_CAP(CAP_KILL);
	ADD_CAP(CAP_SETGID);
	ADD_CAP(CAP_SETUID);
	ADD_CAP(CAP_SETPCAP);
	ADD_CAP(CAP_LINUX_IMMUTABLE);
	ADD_CAP(CAP_NET_BIND_SERVICE);
	ADD_CAP(CAP_NET_BROADCAST);
	ADD_CAP(CAP_NET_ADMIN);
	ADD_CAP(CAP_NET_RAW);
	ADD_CAP(CAP_IPC_LOCK);
	ADD_CAP(CAP_IPC_OWNER);
	ADD_CAP(CAP_SYS_MODULE);
	ADD_CAP(CAP_SYS_RAWIO);
	ADD_CAP(CAP_SYS_CHROOT);
	ADD_CAP(CAP_SYS_PTRACE);
	ADD_CAP(CAP_SYS_ADMIN);
	ADD_CAP(CAP_SYS_BOOT);
	ADD_CAP(CAP_SYS_NICE);
	ADD_CAP(CAP_SYS_RESOURCE);
	ADD_CAP(CAP_SYS_TIME);
	ADD_CAP(CAP_SYS_TTY_CONFIG);
	ADD_CAP(CAP_MKNOD);
	ADD_CAP(CAP_LEASE);
	ADD_CAP(CAP_AUDIT_WRITE);
	ADD_CAP(CAP_AUDIT_CONTROL);
	// linux doesn't have CAP_SETFCAP, CAP_MAC_OVERRIDE, CAP_MAC_ADMIN

#undef ADD_CAP

#endif
}

Process58Probe::~Process58Probe() {
  instance = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//


AbsProbe* Process58Probe::Instance() {

	// Use lazy initialization
	if(instance == NULL) 
		instance = new Process58Probe();

	return instance;	
}

ItemVector* Process58Probe::CollectItems(Object* object) {

	ObjectEntity* command = object->GetElementByName("command_line");
	ObjectEntity* pid = object->GetElementByName("pid");

	// check datatypes - only allow string
	if(command->GetDatatype() != OvalEnum::DATATYPE_STRING) {
		throw ProbeException("Error: invalid data type specified on command_line. Found: " + OvalEnum::DatatypeToString(command->GetDatatype()));
	}

	// check operation - only allow  equals, not equals and pattern match
	if(command->GetOperation() != OvalEnum::OPERATION_EQUALS && command->GetOperation() != OvalEnum::OPERATION_PATTERN_MATCH && command->GetOperation() != OvalEnum::OPERATION_NOT_EQUAL) {
		throw ProbeException("Error: invalid operation specified on name. Found: " + OvalEnum::OperationToString(command->GetOperation()));
	}

	ItemVector *collectedItems = new ItemVector();

	StringPairVector* commands = this->GetCommands(pid, command);
	if(commands->size() > 0) {
		StringPairVector::iterator iterator;
		for(iterator = commands->begin(); iterator != commands->end(); iterator++) {		
			this->GetPSInfo((*iterator)->first, (*iterator)->second, collectedItems);
		}
	} else {

		if(command->GetOperation() == OvalEnum::OPERATION_EQUALS) {

			if(command->GetVarRef() == NULL) {

				Item* item = this->CreateItem();
				item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
				item->AppendElement(new ItemEntity("command_line",  command->GetValue(), OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_DOES_NOT_EXIST));
				collectedItems->push_back(item);

			} else {

				VariableValueVector::iterator iterator;
				for(iterator = command->GetVarRef()->GetValues()->begin(); iterator != command->GetVarRef()->GetValues()->end(); iterator++) {

					Item* item = this->CreateItem();
					item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
					item->AppendElement(new ItemEntity("command_line",  (*iterator)->GetValue(), OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_DOES_NOT_EXIST));
					collectedItems->push_back(item);
				}
			}
		}
	}

	this->DeleteCommands(commands);

	return collectedItems;
}  





Item* Process58Probe::CreateItem() {

	Item* item = new Item(0, 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#unix", 
						"unix-sc", 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#unix unix-system-characteristics-schema.xsd", 
						OvalEnum::STATUS_ERROR, 
						"process58_item");

	return item;
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//


StringPairVector* Process58Probe::GetCommands(ObjectEntity *pid, ObjectEntity* command) {

	StringPairVector* commands;
	StringVector commandEntityValues;
	StringVector::size_type numEntityValues;
#if defined DARWIN
	string pidStr;
#else
	StringVector pids;
#endif

	command->GetEntityValues(commandEntityValues); // ignoring flag return value for now
	numEntityValues = commandEntityValues.size();
	
	switch(command->GetOperation()) {

	case OvalEnum::OPERATION_EQUALS:
		commands = new StringPairVector();
		for (StringVector::iterator iter = commandEntityValues.begin();
			 iter != commandEntityValues.end();
			 ++iter) {
#if defined DARWIN
			if (this->CommandExists(*iter, pidStr))
				commands->push_back(new pair<string,string>(*iter, pidStr));
#else
			if (this->CommandExists(*iter, pids))
				for (StringVector::iterator pidIter = pids.begin();
					 pidIter != pids.end();
					 ++pidIter)
					commands->push_back(new pair<string,string>(*iter, *pidIter));
#endif
		}
		break;

	case OvalEnum::OPERATION_NOT_EQUAL:
		if (numEntityValues == 1)
			commands = this->GetMatchingCommands(commandEntityValues[0], false);
		else
			commands = this->GetMatchingCommands("", true); //gets all commands
		break;
		
	case OvalEnum::OPERATION_PATTERN_MATCH:
		if (numEntityValues == 1)
			commands = this->GetMatchingCommands(commandEntityValues[0], true);
		else
			commands = this->GetMatchingCommands("", true); //gets all commands
		break;
		
	default:
		throw ProbeException("ProcessProbe: unsupported operation: "+OvalEnum::OperationToString(command->GetOperation()));
		break;
	}

	// The code above in some cases winnows down the process items
	// retrieved to only include those which match the command object
	// entity.  But not always.  We need to do a second, more cpu
	// intensive pass to definitively filter out those which don't
	// match the command or pid.  And in any case, we have to make
	// sure var_checks are satisfied, in case a var_ref was used.

	ItemEntity* tmpCmd = this->CreateItemEntity(command);
	ItemEntity* tmpPid = this->CreateItemEntity(pid);
	StringPairVector::iterator it;
	for(it = commands->begin(); it != commands->end(); ) {
		tmpCmd->SetValue((*it)->first);
		tmpPid->SetValue((*it)->second);
		if(command->Analyze(tmpCmd) == OvalEnum::RESULT_TRUE &&
		   pid->Analyze(tmpPid) == OvalEnum::RESULT_TRUE)
			++it;
		else {
			delete *it;
			it = commands->erase(it);
		}
	}

	return commands;
}

#ifdef LINUX
void Process58Probe::GetPSInfo(string command, string pidStr, ItemVector* items) {

	string errMsg = "";

	// Time parameters
	time_t currentTime;
	unsigned long adjustedStartTime, execTime = 0;

	// TTY String
	char ttyName[TTY_LEN + 1];

	// Process Parameters
	char cmdline[CMDLINE_LEN + 1];

	uid_t ruid, *ruidp=&ruid, euid, *euidp=&euid;
	pid_t pid, ppid;
	long priority = 0;
	unsigned long starttime = 0;
	pid_t sessionId;
	uid_t loginuid;
	uint64_t effCap, *effCapp=&effCap;
	string selinuxDomainLabel;

	Process58Probe::ProcStatus statStatus, statusStatus, ttyStatus, loginuidStatus;

	if (!Common::FromString(pidStr, &pid))
		throw ProbeException("Couldn't interpret \""+pidStr+"\" as a pid!");

	// Grab the current time and uptime(Linux only) to calculate start and exec times later
	currentTime = time(NULL);

	unsigned long uptime = 0;
	if(!RetrieveUptime(&uptime, &errMsg)) {
		throw ProbeException(errMsg);
    }

	string procDir = "/proc/" + pidStr;
	struct stat st;

	if (stat(procDir.c_str(), &st) != 0)
		return; // oops, this process went away when we weren't looking

	// Clear the ps values
	memset(cmdline, 0, CMDLINE_LEN + 1);
	memset(ttyName, 0, TTY_LEN + 1);
	euid = ruid = ppid = priority = starttime = 0;
	adjustedStartTime = execTime = 0;
	effCap = 0;

	auto_ptr<Item> item(this->CreateItem());
	item->SetStatus(OvalEnum::STATUS_EXISTS);
	item->AppendElement(new ItemEntity("command_line",  command, OvalEnum::DATATYPE_STRING, true));

	// Below, I try to get everything, but behave correctly if the /proc/<pid>
	// directory unexpectedly goes away (just ignore that process, because it
	// must have terminated), or if there is an error (set the proper error
	// message and status, but keep looking for other information).
	
	statStatus = RetrieveStatFile(pidStr, &ppid, &priority, 
								  &starttime, &sessionId, &errMsg);
	if (statStatus == PROC_TERMINATED) {
		Log::Debug("Process "+pidStr+" went away (stat), skipping...");
		return;
	} else if (statStatus == PROC_ERROR) {
		item->AppendMessage(new OvalMessage(errMsg, OvalEnum::LEVEL_ERROR));
		item->SetStatus(OvalEnum::STATUS_ERROR);
	}

	statusStatus = RetrieveStatusFile(pidStr, &ruidp, &euidp, &effCapp, &errMsg);
	if (statusStatus == PROC_TERMINATED) {
		Log::Debug("Process "+pidStr+" went away (status), skipping...");
		return;
	} else if (statusStatus == PROC_ERROR) {
		item->AppendMessage(new OvalMessage(errMsg, OvalEnum::LEVEL_ERROR));
		item->SetStatus(OvalEnum::STATUS_ERROR);
	} else {
		// I assume the things I am looking for are not optional in the status
		// file, so if they aren't found, that's an error that should be
		// reported.
		if (!ruidp || !euidp) {		
			item->AppendMessage(
				new OvalMessage("Couldn't locate euid or ruid in " + procDir +
								"/status", OvalEnum::LEVEL_ERROR));
			item->SetStatus(OvalEnum::STATUS_ERROR);
		}

		if (!effCapp) {
			item->AppendMessage(
				new OvalMessage("Couldn't locate effective capabilities in " + 
								procDir + "/status", OvalEnum::LEVEL_ERROR));
			item->SetStatus(OvalEnum::STATUS_ERROR);
		}
	}

	// We can retrieve a value for the tty from the 'stat' file, but it's unclear
	// how you convert that to a device name.  Therefore, we ignore that value
	// and grab the device stdout(fd/0) is attached to.
	ttyStatus = RetrieveTTY(pidStr, ttyName, &errMsg);
	if (ttyStatus == PROC_TERMINATED) {
		Log::Debug("Process "+pidStr+" went away (tty), skipping...");
		return;
	} else if (ttyStatus == PROC_ERROR)
		item->AppendMessage(new OvalMessage(errMsg, OvalEnum::LEVEL_ERROR));
	// we will skip setting error status on item for this, to be consistent with
	// previous behavior.

	loginuidStatus = RetrieveLoginUid(pid, &loginuid, &errMsg);
	if (loginuidStatus == PROC_TERMINATED) {
		Log::Debug("Process "+pidStr+" went away (tty), skipping...");
		return;
	} else if (loginuidStatus == PROC_ERROR) {
		item->AppendMessage(new OvalMessage(errMsg, OvalEnum::LEVEL_ERROR));
		item->SetStatus(OvalEnum::STATUS_ERROR);
	}

	// this one doesn't require reading anything in /proc
	if (!RetrieveSelinuxDomainLabel(pid, &selinuxDomainLabel, &errMsg)) {
		item->AppendMessage(new OvalMessage(errMsg, OvalEnum::LEVEL_ERROR));
		item->SetStatus(OvalEnum::STATUS_ERROR);
	}

	// The Linux start time is represented as the number of jiffies (1/100 sec)
	// that the application was started after the last system reboot.  To get an
	// actual timestamp, we have to do some gymnastics.  We then use the calculated
	// start time to determine the exec time.
	// We won't have starttime if we couldn't read the stat file, so make sure
	// we have it.
	//
	string execTimeStr, adjustedStartTimeStr;
	if(uptime > 0 && statStatus == PROC_OK) {
		execTime = uptime - (starttime/100);
		adjustedStartTime = currentTime - execTime;
		execTimeStr = this->FormatExecTime(execTime);
		adjustedStartTimeStr = this->FormatStartTime(adjustedStartTime);
		item->AppendElement(new ItemEntity("exec_time",  execTimeStr, OvalEnum::DATATYPE_STRING));
	} else
		item->AppendElement(new ItemEntity("exec_time",  "", OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_ERROR));

	item->AppendElement(new ItemEntity("pid", pidStr, OvalEnum::DATATYPE_INTEGER, true));

	if (statStatus == PROC_OK) {
		item->AppendElement(new ItemEntity("ppid", Common::ToString(ppid), OvalEnum::DATATYPE_INTEGER));
		item->AppendElement(new ItemEntity("priority", Common::ToString(priority), OvalEnum::DATATYPE_INTEGER));
	} else {
		item->AppendElement(new ItemEntity("ppid", "", OvalEnum::DATATYPE_INTEGER, false, OvalEnum::STATUS_ERROR));
		item->AppendElement(new ItemEntity("priority", "", OvalEnum::DATATYPE_INTEGER, false, OvalEnum::STATUS_ERROR));
	}

	if (statusStatus == PROC_OK && ruidp) {
		item->AppendElement(new ItemEntity("ruid", Common::ToString(ruid), OvalEnum::DATATYPE_INTEGER));
	} else
		item->AppendElement(new ItemEntity("ruid", "", OvalEnum::DATATYPE_INTEGER, false, OvalEnum::STATUS_ERROR));

	item->AppendElement(new ItemEntity("scheduling_class",  "", OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_NOT_COLLECTED));

	if (statStatus == PROC_OK)
		item->AppendElement(new ItemEntity("start_time", adjustedStartTimeStr));

	// To remain consistent with previous behavior for this entity, "?" will be
	// used as the value and no error status set on the entity, even if there
	// was an error.
	item->AppendElement(new ItemEntity("tty", ttyName));

	if (statusStatus == PROC_OK && euidp)
		item->AppendElement(new ItemEntity("user_id", Common::ToString(euid), OvalEnum::DATATYPE_INTEGER, false, OvalEnum::STATUS_EXISTS));
	else
		item->AppendElement(new ItemEntity("user_id", "", OvalEnum::DATATYPE_INTEGER, false, OvalEnum::STATUS_ERROR));

	// for now...
	item->AppendElement(new ItemEntity("exec_shield", "", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_NOT_COLLECTED));

	if (loginuidStatus == PROC_OK)
		item->AppendElement(new ItemEntity("loginuid", Common::ToString(loginuid), OvalEnum::DATATYPE_INTEGER));
	else
		item->AppendElement(new ItemEntity("loginuid", "", OvalEnum::DATATYPE_INTEGER, false, OvalEnum::STATUS_ERROR));

	if (statusStatus == PROC_OK && effCapp)
		this->AddCapabilities(item.get(), effCap);
	else
		// I add an entity with error status here, so consumers know that posix
		// capabilities couldn't be determined due to error, not that there just
		// aren't any.
		item->AppendElement(new ItemEntity("posix_capability", "", OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_ERROR));

	if (selinuxDomainLabel.empty())
		item->AppendElement(new ItemEntity("selinux_domain_label", "", OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_ERROR));
	else
		item->AppendElement(new ItemEntity("selinux_domain_label", selinuxDomainLabel));

	if (statStatus == PROC_OK)
		item->AppendElement(new ItemEntity("session_id", Common::ToString(sessionId), OvalEnum::DATATYPE_INTEGER));
	else
		item->AppendElement(new ItemEntity("session_id", "", OvalEnum::DATATYPE_INTEGER, false, OvalEnum::STATUS_ERROR));

	items->push_back(item.release());
}

Process58Probe::ProcStatus Process58Probe::RetrieveStatFile(const string &process, int *ppid, long *priority, unsigned long *starttime, pid_t *session, string *errMsg) {

	// Stat File parameters.  While we're really only concerned with gathering the parameters
	// that are passed in, these variables are good placeholders in case we decide to collect
	// something else in the future.
	//
	pid_t pid; // obviously we already know this...
	int pgrp, tty, tpgid, exit_signal, processor = 0;
	long cutime, cstime, nice, placeholder, itrealvalue, rss = 0;
	unsigned long flags, minflt, cminflt, majflt, cmajflt, utime, stime, vsize, rlim = 0;
	unsigned long startcode, endcode, startstack, kstkesp, kstkeip, signal, blocked, sigignore = 0;
	unsigned long sigcatch, wchan, nswap, cnswap = 0;
	char comm[PATH_MAX];
	char state;

	FILE *statFile = NULL;

	// Generate the absolute path name for the 'stat' file
	string statPath = "/proc/"+process+"/stat";

	// Open the 'stat' file and read the contents
	if((statFile = fopen(statPath.c_str(), "r")) == NULL) {
		if (errno == ENOENT)
			return PROC_TERMINATED;

		*errMsg = "Process58Probe: Error opening "+statPath+": "+strerror(errno);
		return PROC_ERROR;

	} else {

		// Linux gives us a nicely formatted file for fscanf to pull in
		fscanf(statFile, "%d %s %c %d %d %d %d %d %lu %lu %lu %lu %lu %lu %lu %ld %ld %ld %ld %ld %ld %lu %lu %ld %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %d %d", &pid, comm, &state, ppid, &pgrp, session, &tty, &tpgid, &flags, &minflt, &cminflt, &majflt, &cmajflt, &utime, &stime, &cutime, &cstime, priority, &nice, &placeholder, &itrealvalue, starttime, &vsize, &rss, &rlim, &startcode, &endcode, &startstack, &kstkesp, &kstkeip, &signal, &blocked, &sigignore, &sigcatch, &wchan, &nswap, &cnswap, &exit_signal, &processor);

	}

	if (fclose(statFile) == EOF) {
		*errMsg = "Error closing "+statPath+": "+strerror(errno);
		return PROC_ERROR;
	}

	return PROC_OK;
}

Process58Probe::ProcStatus Process58Probe::RetrieveStatusFile(const string &process, uid_t **ruid, uid_t **euid, uint64_t **effCap, string *errMsg) {
	string statusFileName = "/proc/"+process+"/status";
	bool foundUid=false, foundCapEff=false;
	errno = 0;
	ifstream statusFile(statusFileName.c_str());

	if (!statusFile) {
		// Processes come and go; if it went away, that's ok.
		// No need to report.  If there was some other
		// error, throw an exception.
		if (errno == ENOENT)
			return PROC_TERMINATED;

		*errMsg = "Process58Probe: Error opening "+statusFileName+": "+strerror(errno);
		return PROC_ERROR;
	}

	string line;
	while (statusFile) {
		getline(statusFile, line);

		// eof is ok, other errors are not
		if (!statusFile) {
			if (statusFile.eof())
				break;
			else {
				*errMsg = "Error reading "+statusFileName+": "+strerror(errno);
				return Process58Probe::PROC_ERROR;
			}
		}

		istringstream iss(line);
		string label;
		errno = 0;
		iss >> label;

		if (!iss) {
			*errMsg = "Couldn't read label from line \""+line+"\" in "+statusFileName;
			return PROC_ERROR;
		}

		if (label == "Uid:") {
			iss >> **ruid >> **euid;
			foundUid = true;

		} else if (label == "CapEff:") {
			iss >> hex >> **effCap >> dec;
			foundCapEff = true;
		}

		if (!iss) {
			if (iss.eof())
				// eol was premature
				*errMsg = "Premature end-of-line while reading "+label+" from "+
					statusFileName;
			else
				*errMsg = "Error encountered while reading "+statusFileName+": "+
					strerror(errno);

			return PROC_ERROR;
		}
	}

	if (!foundUid) *ruid = *euid = NULL;
	if (!foundCapEff) *effCap = NULL;

	return PROC_OK;
}

Process58Probe::ProcStatus Process58Probe::RetrieveTTY(const string &process, char *ttyName, string *err) {
	int bytes = 0;

	// Generate the absolute path name for the stdout(0) file in 'fd'
	string ttyPath = "/proc/" + process + "/fd/0";

	// Attempt to read the name of the file linked to '0'
	bytes = readlink(ttyPath.c_str(), ttyName, TTY_LEN);

	// If there is an error, set the tty string to '?'
	if(bytes < 0 || strncmp(ttyName, "/dev", 4) != 0) {
		strncpy(ttyName, "?\0", 2);

		if (bytes < 0) {
			// we can't check for ENOENT and return PROC_TERMINATED here, since
			// not all processes have a /proc/<pid>/fd/0 symlink.  If the link
			// isn't there, we just assume the process is there but doesn't have
			// a stdout.  Actually, that shouldn't be an error either.  It
			// should only be an error if the link was there but we just
			// couldn't read it.  So let's say ENOENT is ok.
			if (errno == ENOENT)
				return PROC_OK;

			*err = "readlink("+ttyPath+"): "+strerror(errno);
			return PROC_ERROR;
		}
	}

	return PROC_OK;
}

bool Process58Probe::RetrieveUptime(unsigned long *uptime, string *errMsg) {
	// The second value in this file represents idle time - we're not concerned with this.
	unsigned long idle = 0;
	FILE *uptimeHandle = NULL;

	// Open and parse the 'uptime' file
	if((uptimeHandle = fopen("/proc/uptime", "r")) == NULL) {
		*errMsg = string("Process58Probe: Could not open /proc/uptime: ") + strerror(errno);
		uptime = 0;
		return false;
	} else {
		fscanf(uptimeHandle, "%lu %lu", uptime, &idle);
	}

	if (fclose(uptimeHandle) == EOF) {
		*errMsg = string("fclose(/proc/uptime): ") + strerror(errno);
		return false;
	}

	return true;
}

Process58Probe::ProcStatus Process58Probe::RetrieveLoginUid(pid_t pid, uid_t *loginUid, string *err) {
	string loginUidFilename = "/proc/" + Common::ToString(pid) + "/loginuid";
	errno = 0;
	ifstream in(loginUidFilename.c_str());

	if (!in) {
		if (errno == ENOENT)
			return PROC_TERMINATED;

		*err = "Couldn't open " + loginUidFilename + ": " + strerror(errno);
		return PROC_ERROR;
	}

	in >> *loginUid;

	if (in)
		return PROC_OK;

	*err = "Couldn't interpret contents of " + loginUidFilename +
		" as a uid";

	return PROC_ERROR;
}

void Process58Probe::AddCapabilities(Item *item, uint64_t effCap) {
	// the capgetp() function is not available by default (since gcc sets
	// _POSIX_SOURCE), so I guess I won't use that.  Instead, I've read a value
	// from /proc/<pid>/status, which I interpret as a bit set.  Here I iterate
	// through the bits.
	for (size_t capEnum=0; capEnum<capMap.size(); ++capEnum) {

		// in case there's gaps in the sequence of capability constants
		if (capMap[capEnum].empty())
			continue;

		if (effCap & (1ULL << capEnum))
			item->AppendElement(new ItemEntity("posix_capability",
											   capMap[capEnum]));
	}
}

bool Process58Probe::RetrieveSelinuxDomainLabel(pid_t pid, string *label, string *err) {
	security_context_t sctx;
	int ec = getpidcon(pid, &sctx);
	if (ec == -1) {
		// getpidcon man page doesn't say errno is set... so we can't get a
		// reason for the error.
		*err = "getpidcon() failed";
		return false;
	}

	SecurityContextGuard scg(sctx);
	ContextGuard cg(sctx);

	const char *tmp = context_type_get(cg);
	if (!tmp) {
		*err = string("context_get_type(")+sctx+"): "+strerror(errno);
		return false;
	}

	*label = tmp;
	return true;
}

#elif defined SUNOS

void Process58Probe::GetPSInfo(string command, string pidStr, ItemVector* items) {

	string psinfoFileName = "/proc/"+pidStr+"/psinfo";
	psinfo_t info;
	pid_t ppid;
	dev_t tty;
	timestruc_t startTime, execTime;
	int priority;
	string schedClass;
	uid_t rUserId, eUserId;
	string formattedStartTime, formattedExecTime;
	string devicePath;
	Item *item;
	string errMsg;

	if (!this->ReadPSInfoFromFile(psinfoFileName, info, errMsg)) {
		// if empty, the file was just not found which is ok.  A
		// process can go away at any time.
		if (errMsg.empty())
			return;

		throw ProbeException(errMsg);
	}
	
	execTime = info.pr_time;
	startTime = info.pr_start;
	ppid = info.pr_ppid;

	//note that this priority is for the "representative" lwp.  I guess
	//in general, priority is not a property of a process, but a lwp
	//(a thread).
	priority = info.pr_lwp.pr_pri;

	// also a lwp property, not a process property
	schedClass = info.pr_lwp.pr_clname;
	
	tty = info.pr_ttydev;
	rUserId = info.pr_uid;
	eUserId = info.pr_euid;
	
	formattedExecTime = this->FormatExecTime(execTime.tv_sec);
	formattedStartTime = this->FormatStartTime(startTime.tv_sec);

	devicePath = this->GetDevicePath(tty);

	item = this->CreateItem();
	item->SetStatus(OvalEnum::STATUS_EXISTS);
	item->AppendElement(new ItemEntity("command_line", command, OvalEnum::DATATYPE_STRING, true));
	item->AppendElement(new ItemEntity("exec_time", formattedExecTime));
	item->AppendElement(new ItemEntity("pid", pidStr, OvalEnum::DATATYPE_INTEGER, true));
	item->AppendElement(new ItemEntity("ppid", Common::ToString(ppid), OvalEnum::DATATYPE_INTEGER));
	item->AppendElement(new ItemEntity("priority", Common::ToString(priority), OvalEnum::DATATYPE_INTEGER));
	item->AppendElement(new ItemEntity("ruid", Common::ToString(rUserId), OvalEnum::DATATYPE_INTEGER));
	item->AppendElement(new ItemEntity("scheduling_class", schedClass));
	item->AppendElement(new ItemEntity("start_time", formattedStartTime));
	if (devicePath.empty())
		item->AppendElement(new ItemEntity("tty", "", OvalEnum::DATATYPE_STRING,
										   false, OvalEnum::STATUS_DOES_NOT_EXIST));
	else
		item->AppendElement(new ItemEntity("tty", devicePath));
	item->AppendElement(new ItemEntity("user_id", Common::ToString(eUserId), OvalEnum::DATATYPE_INTEGER));
	item->AppendElement(new ItemEntity("exec_shield", "", OvalEnum::DATATYPE_BOOLEAN, false, OvalEnum::STATUS_NOT_COLLECTED));
	item->AppendElement(new ItemEntity("loginuid", "", OvalEnum::DATATYPE_INTEGER, false, OvalEnum::STATUS_NOT_COLLECTED));
 	item->AppendElement(new ItemEntity("posix_capability", "", OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_NOT_COLLECTED));
 	item->AppendElement(new ItemEntity("selinux_domain_label", "", OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_NOT_COLLECTED));
	item->AppendElement(new ItemEntity("session_id", Common::ToString(info.pr_sid), OvalEnum::DATATYPE_INTEGER));

	items->push_back(item);
}

string Process58Probe::GetDevicePath(dev_t tty) {
	StringVector deviceDirs = this->GetDeviceSearchDirs();

	// the last ditch fallback... search all of /dev
	deviceDirs.push_back("/dev");

	for (StringVector::iterator iter = deviceDirs.begin();
		 iter != deviceDirs.end();
		 ++iter) {

		::dev = tty;
		errno = 0;
		int err = ftw(iter->c_str(), ::searcher, 10);

		if (err > 0)
			return ::devPath;
		else if (err < 0 && errno != ENOENT)
			// if an error occurs finding the tty device path, we will just
			// leave off the entity, not halt the collection.  Don't show
			// a message for ENOENT because /etc/ttysrch can list directories
			// which don't exist, and we don't want to spam users...
			Log::Message("Process58Probe: Error occurred during search for device path under "+*iter+": "+strerror(errno));
	}

	return "";
}

StringVector Process58Probe::GetDeviceSearchDirs() {
	StringVector dirs;
	string line;
	ifstream file("/etc/ttysrch");
	int (*intisspace)(int)  = isspace;
	
	if (file) {
		while(getline(file, line)) {
			Common::TrimString(line);
			if (line.empty() || line[0] == '#')
				continue; // skip blanks and comments

			// Mimic documented behavior of ttyname(3C), and ignore
			// dirs which don't start with "/dev".
			if (!this->IsMatch("^/dev", line, true))
				continue;

			// each directory can have some modifier chars... ignore them.  Just
			// grab the first space-delimited substring.
			string::iterator firstSpace = find_if(line.begin(), line.end(), intisspace);
			dirs.push_back(line.substr(0, firstSpace - line.begin()));
		}
	}

	return dirs;
}

bool Process58Probe::ReadPSInfoFromFile(const string psinfoFileName, psinfo_t &info, string &errMsg) {
	errno = 0;
	ifstream psinfoFile(psinfoFileName.c_str(), ios::in | ios::binary);

	if (!psinfoFile) {
		// Processes come and go; if it went away, that's ok.
		// No need to report.  If there was some other
		// error, throw an exception.
		if (errno == ENOENT)
			return false;

		errMsg = "Process58Probe: Error opening "+psinfoFileName+": "+strerror(errno);
		return false;
	}

	errno = 0;
	psinfoFile.read((char*)&info, sizeof(psinfo_t));
	
	if (!psinfoFile) {
		errMsg = "Process58Probe: Error reading "+psinfoFileName;

		// give errno a chance to tell us what happened.  If it's zero, we check
		// how many bytes were read.  If too few, make up our own error message
		// (there is no errno value for premature end-of-file afaik).
		// If neither occurred, we can't usefully describe the error.
		if (errno != 0)
			errMsg += string(": ") + strerror(errno);
		else if (psinfoFile.gcount() < (streamsize)sizeof(psinfo_t))
			errMsg += string(": ") + "Premature end-of-file encountered";

		return false;
	}

	return true;
}

#elif defined DARWIN

void Process58Probe::GetPSInfo(string command, string pidStr, ItemVector* items) {
  int i, mib[4];
  size_t length, count;
  char* data = NULL;
  struct kinfo_proc *procdata = NULL;
  Item *item = NULL;
  char* tty = NULL;
  string ttyStr = "";
  string formattedExecTime = "";
  string formattedStartTime = "";
  
  mib[0] = CTL_KERN;
  mib[1] = KERN_PROC;
  mib[2] = KERN_PROC_PID;                                                                                                                                                     
  mib[3] = atoi(pidStr.c_str());

  if ( sysctl(mib, 4, data, &length, NULL, 0) == -1 ){
    throw ProbeException("Error: sysctl error while trying to determine data length");
  }

  data = (char*) malloc (length);

  if ( data == NULL ){
    throw ProbeException("Error: couldn't allocate memory");
  }

  if ( sysctl(mib, 4, data, &length, NULL, 0) == -1 ){
    free(data);
    throw ProbeException("Error: sysctl error while trying to retrieve data");
  }

  procdata = (struct kinfo_proc*) data;
  count = length/sizeof(kinfo_proc);
  
  for(i = 0 ; i < (int)count ; i++){
    
    if( (tty = devname(procdata[i].kp_eproc.e_tdev,S_IFCHR) ) != NULL ){
      ttyStr = tty;
    }
    formattedStartTime = this->FormatStartTime(procdata[i].kp_proc.p_starttime.tv_sec);
    
    item = this->CreateItem();
    item->SetStatus(OvalEnum::STATUS_EXISTS);
    item->AppendMessage(new OvalMessage("The unix-def:process_probe for OSX currently only collects the command of the process and not the command line arguments.  Note that this may cause the number of processes collected to be inaccurate"));
    item->AppendElement(new ItemEntity("command_line", procdata[i].kp_proc.p_comm, OvalEnum::DATATYPE_STRING, true));
    item->AppendElement(new ItemEntity("exec_time", formattedExecTime, OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_NOT_COLLECTED));
    item->AppendElement(new ItemEntity("pid", Common::ToString(procdata[i].kp_proc.p_pid), OvalEnum::DATATYPE_INTEGER, true));
    item->AppendElement(new ItemEntity("ppid", Common::ToString(procdata[i].kp_eproc.e_ppid), OvalEnum::DATATYPE_INTEGER));
    item->AppendElement(new ItemEntity("priority", Common::ToString(procdata[i].kp_proc.p_priority), OvalEnum::DATATYPE_INTEGER));
    item->AppendElement(new ItemEntity("ruid", Common::ToString(procdata[i].kp_eproc.e_pcred.p_ruid), OvalEnum::DATATYPE_INTEGER));
    item->AppendElement(new ItemEntity("scheduling_class","",OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_NOT_COLLECTED));
    item->AppendElement(new ItemEntity("start_time", formattedStartTime));
    item->AppendElement(new ItemEntity("tty", ttyStr, OvalEnum::DATATYPE_STRING, false, (ttyStr.compare("") == 0)?OvalEnum::STATUS_DOES_NOT_EXIST:OvalEnum::STATUS_EXISTS));
    item->AppendElement(new ItemEntity("user_id", Common::ToString(procdata[i].kp_eproc.e_ucred.cr_uid), OvalEnum::DATATYPE_INTEGER));
    
    items->push_back(item);
  }
  free(data);
  return;
}

#endif

#ifdef DARWIN
bool Process58Probe::CommandExists(string command, string &pid) {
  int i, mib[4];
  size_t length, count;
  bool exists = false;
  char* data = NULL;
  struct kinfo_proc *procdata = NULL;

  mib[0] = CTL_KERN;
  mib[1] = KERN_PROC;
  mib[2] = KERN_PROC_ALL;

  if ( sysctl(mib, 3, data, &length, NULL, 0) == -1 ){
    throw ProbeException("Error: sysctl error while trying to determine data length");
  }

  data = (char*) malloc (length);

  if ( data == NULL ){
    throw ProbeException("Error: couldn't allocate memory");
  }

  if ( sysctl(mib, 3, data, &length, NULL, 0) == -1 ){
    free(data);
    throw ProbeException("Error: sysctl error while trying to retrieve data");
  }

  procdata = (struct kinfo_proc*) data;
  count = length/sizeof(kinfo_proc);

  for(i = 0 ; i < (int)count ; i++){
    if( command.compare(procdata[i].kp_proc.p_comm) == 0 ) {
      exists = true;
      pid = Common::ToString(procdata[i].kp_proc.p_pid);
    }
  }
  free(data);
  return exists;
}
#else
bool Process58Probe::CommandExists(string command, string &pid) {

	bool exists = false;
	string errMsg = "";

	// Process Parameters
	char cmdline[CMDLINE_LEN + 1];

	DIR *proc;
	struct dirent *readProc;

	// Step into the /proc directory
	if((proc = opendir("/proc")) == NULL) {
		errMsg.append("Process58Probe: Could not open /proc");
		throw ProbeException(errMsg);

	} else {

		// Loop through all of the files - we're only concerned with those that
		// start with a digit
		while((readProc = readdir(proc)) != NULL) {
			if(isdigit(readProc->d_name[0])) {
				// Clear the ps values
				memset(cmdline, 0, CMDLINE_LEN + 1);
				errMsg = "";

				// Retrieve the command line with arguments
				int status = RetrieveCommandLine(readProc->d_name, cmdline, &errMsg);
				if(status < 0) { 
					closedir(proc);
					throw ProbeException(errMsg);

				// If the command line matches the input command line return true
				} else if(status == 0 && command.compare(cmdline) == 0) {
					exists = true;
					pid = readProc->d_name;
					break;
				}
			}
		}
		closedir(proc);
	} // else
	return exists;
}
bool Process58Probe::CommandExists(string command, StringVector &pids) {

	bool exists = false;
	string errMsg = "";

	// Process Parameters
	char cmdline[CMDLINE_LEN + 1];

	DIR *proc;
	struct dirent *readProc;

	// Step into the /proc directory
	if((proc = opendir("/proc")) == NULL) {
		errMsg.append("Process58Probe: Could not open /proc");
		throw ProbeException(errMsg);

	} else {

		// Loop through all of the files - we're only concerned with those that
		// start with a digit
		while((readProc = readdir(proc)) != NULL) {
			if(isdigit(readProc->d_name[0])) {
				// Clear the ps values
				memset(cmdline, 0, CMDLINE_LEN + 1);
				errMsg = "";

				// Retrieve the command line with arguments
				int status = RetrieveCommandLine(readProc->d_name, cmdline, &errMsg);
				if(status < 0) { 
					closedir(proc);
					throw ProbeException(errMsg);

				// If the command line matches the input command line return true
				} else if(status == 0 && command.compare(cmdline) == 0) {
					exists = true;
					pids.push_back(readProc->d_name);
				}
			}
		}
		closedir(proc);
	} // else
	return exists;
}
#endif 

#ifdef DARWIN
StringPairVector* Process58Probe::GetMatchingCommands(string pattern, bool isRegex){
  int i, mib[4];
  size_t length, count;
  char* data = NULL;
  struct kinfo_proc *procdata = NULL;
  StringPairVector *commands = NULL;
  
  mib[0] = CTL_KERN;
  mib[1] = KERN_PROC;
  mib[2] = KERN_PROC_ALL;

  if ( sysctl(mib, 3, data, &length, NULL, 0) == -1 ){
    throw ProbeException("Error: sysctl error while trying to determine data length");
  }

  data = (char*) malloc (length);

  if ( data == NULL ){
    throw ProbeException("Error: couldn't allocate memory");
  }

  if ( sysctl(mib, 3, data, &length, NULL, 0) == -1 ){
    free(data);
    throw ProbeException("Error: sysctl error while trying to retrieve data");
  }

  procdata = (struct kinfo_proc*) data;
  count = length/sizeof(kinfo_proc);
  commands = new StringPairVector();

  for(i = 0 ; i < (int)count ; i++){
      if( this->IsMatch(pattern, procdata[i].kp_proc.p_comm, isRegex)) {
	commands->push_back(new pair<string,string>(procdata[i].kp_proc.p_comm, Common::ToString(procdata[i].kp_proc.p_pid)));
      }
  }
  free(data);
  return commands;
}
#else
StringPairVector* Process58Probe::GetMatchingCommands(string pattern, bool isRegex) {
	StringPairVector* commands = new StringPairVector();
	string errMsg = "";

	// Process Parameters
	char cmdline[CMDLINE_LEN + 1];

	DIR *proc;
	struct dirent *readProc;

	// Step into the /proc directory
	if((proc = opendir("/proc")) == NULL) {
		errMsg.append("Process58Probe: Could not open /proc");
		this->DeleteCommands(commands);
		throw ProbeException(errMsg);

	} else {

		// Loop through all of the files - we're only concerned with those that
		// start with a digit
		while((readProc = readdir(proc)) != NULL) {
			if(isdigit(readProc->d_name[0])) {
				// Clear the ps values
				memset(cmdline, 0, CMDLINE_LEN + 1);
				errMsg = "";

				// Retrieve the command line with arguments
				int status = RetrieveCommandLine(readProc->d_name, cmdline, &errMsg);
				
				if(status < 0) { 
					closedir(proc);
					this->DeleteCommands(commands);
					throw ProbeException(errMsg);

				// If the command line matches the input command line store it
				} else if(status == 0 && this->IsMatch(pattern, cmdline, isRegex)) {
					commands->push_back(new pair<string,string>(cmdline, readProc->d_name));
				}
			}
		}
		closedir(proc);
	} // else

	return commands;
}
#endif

int Process58Probe::RetrieveCommandLine(const char *process, char *cmdline, string *errMsg) {
	// Build the absolute path to the command line file
	string cmdlinePath = "/proc/";
	cmdlinePath.append(process);

#ifdef LINUX
	FILE *cmdlineFile = NULL;
	int i = 0;
	int bytes = 0;

	cmdlinePath.append("/cmdline");

	// Open the file for reading
	if((cmdlineFile = fopen(cmdlinePath.c_str(), "r")) == NULL) {
		errMsg->append("Process58Probe: Unable to obtain command line for pid: ");
		errMsg->append(process);
		return(-1);

	} else {
    
		// Read the contents of the file, and convert all of the NULL's
		// separating the args to spaces.
		//
		bytes = fread(cmdline, sizeof(char), CMDLINE_LEN, cmdlineFile);
	    
		// Remember to leave the trailing NULL (bytes - 1).
		for(i = 0; i < bytes - 1; i++) {
			if(cmdline[i] == '\0') {
				cmdline[i] = ' ';
			}
		}
	}

	fclose(cmdlineFile); 

#elif defined SUNOS

	cmdlinePath += "/psinfo";
	psinfo_t info;
	
	if (!this->ReadPSInfoFromFile(cmdlinePath, info, *errMsg))
		return -1;

	strncpy(cmdline, info.pr_psargs, CMDLINE_LEN);
	cmdline[CMDLINE_LEN] = '\0'; // ensure null-termination		

#endif
	return(0);
}

string Process58Probe::FormatExecTime(time_t execTime) {

	/**
		This is the cumulative CPU time, formatted in [DD-]HH:MM:SS where DD is the number of days when execution time is 24 hours or more.

		Input is seconds 

		Divide by 86400 to get days
		Divide remainder of above division by 3600 to get hours
		Divide remainder by 60 to get minutes, remainder is seconds
	*/

	time_t days = execTime/86400;
	time_t hours = (execTime%86400)/3600;
	time_t minutes = ((execTime%86400)%3600)/60;
	time_t seconds = ((execTime%86400)%3600)%60;

	ostringstream sstr;
	sstr << setfill('0');

	if (days > 0)
		sstr << setw(2) << days << '-';

	sstr << setw(2) << hours << ':'
		 << setw(2) << minutes << ':'
		 << setw(2) << seconds;

	return sstr.str();
}

string Process58Probe::FormatStartTime(time_t startTime) {

	/** 
		This is the time of day the process started formatted in HH:MM:SS if the 
		same day the process started or formatted as MMM_DD (Ex.: Feb_5) if 
		process started the previous day or further in the past.

		Input resolution is seconds.
	*/

	char formattedTime[16];

	// current time info
	time_t rawtime;
	tm* timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	int currentDays = timeinfo->tm_yday;

	string startTimeStr;
	time_t sTime = startTime;
	timeinfo = localtime(&sTime);

	size_t size = 0;
	if(timeinfo->tm_yday == currentDays) {
		// no days so just format the hours, minutes, and seconds
		size = strftime(formattedTime, 15, "%H:%M:%S", timeinfo);
	} else {
		size = strftime(formattedTime, 15, "%b_%d", timeinfo);		
	}

	if(size == 0) {
		throw ProbeException("Insufficient memory allocated for process start time data. Unable to collect process information.");
	} else {
		startTimeStr.append(formattedTime);
	}

	return startTimeStr;
}

void Process58Probe::DeleteCommands(StringPairVector *commands) {
	if (commands == NULL) return;
	
	for(StringPairVector::iterator iter = commands->begin();
		iter != commands->end();
		++iter)
		delete *iter;

	delete commands;
}
