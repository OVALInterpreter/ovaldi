//
//
//****************************************************************************************//
// Copyright (c) 2002-2010, The MITRE Corporation
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

Process58Probe::Process58Probe() {}




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

	ObjectEntity* command = object->GetElementByName("command");
	ObjectEntity* pid = object->GetElementByName("pid");

	// check datatypes - only allow string
	if(command->GetDatatype() != OvalEnum::DATATYPE_STRING) {
		throw ProbeException("Error: invalid data type specified on command. Found: " + OvalEnum::DatatypeToString(command->GetDatatype()));
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
				item->AppendElement(new ItemEntity("command",  command->GetValue(), OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_DOES_NOT_EXIST));
				collectedItems->push_back(item);

			} else {

				VariableValueVector::iterator iterator;
				for(iterator = command->GetVarRef()->GetValues()->begin(); iterator != command->GetVarRef()->GetValues()->end(); iterator++) {

					Item* item = this->CreateItem();
					item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
					item->AppendElement(new ItemEntity("command",  (*iterator)->GetValue(), OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_DOES_NOT_EXIST));
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
						"process_item");

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
	char schedulingClass[SCHED_CLASS_LEN + 1];

	int ruid, euid, pid, ppid;
	long priority = 0;
	unsigned long starttime = 0;

	int status = 0;

	// Grab the current time and uptime(Linux only) to calculate start and exec times later
	currentTime = time(NULL);

	unsigned long uptime = 0;
	status = RetrieveUptime(&uptime, &errMsg);
	if(status < 0) {
		throw ProbeException(errMsg);
    }

	string procDir = "/proc/" + pidStr;
	struct stat st;

	if (stat(procDir.c_str(), &st) != 0)
		return; // oops, this process went away when we weren't looking

	// Clear the ps values
	memset(cmdline, 0, CMDLINE_LEN + 1);
	memset(schedulingClass, 0, SCHED_CLASS_LEN + 1);
	memset(ttyName, 0, TTY_LEN + 1);
	euid = ruid = pid = ppid = priority = starttime = 0;
	adjustedStartTime = execTime = 0;
	errMsg = "";

	// Retrieve the command line with arguments
	status = RetrieveCommandLine(pidStr.c_str(), cmdline, &errMsg);
	if(status < 0) { 
		//				    closedir(proc);
		throw ProbeException(errMsg);

		// If the command line matches the input command line get the remaining 
		// data and add a new data object to the items
	} else if(status == 0 && command.compare(cmdline) == 0) {

		Item* item = this->CreateItem();
		item->SetStatus(OvalEnum::STATUS_EXISTS);
		item->AppendElement(new ItemEntity("command",  command, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));

		// Read the 'stat' and 'status' files for the remaining process parameters
		status = RetrieveStatFile(pidStr.c_str(), &pid, &ppid, &priority, &starttime, &errMsg);
		if (status==0) status = RetrieveStatusFile(pidStr.c_str(), &ruid, &euid, &errMsg);
		if(status < 0) {
			item->AppendMessage(new OvalMessage(errMsg));
		} else {

			// We can retrieve a value for the tty from the 'stat' file, but it's unclear
			// how you convert that to a device name.  Therefore, we ignore that value
			// and grab the device stdout(fd/0) is attached to.
			RetrieveTTY(pidStr.c_str(), ttyName);

			// The Linux start time is represented as the number of jiffies (1/100 sec)
			// that the application was started after the last system reboot.  To get an
			// actual timestamp, we have to do some gymnastics.  We then use the calculated
			// start time to determine the exec time.
			//
			if(uptime > 0) {
				adjustedStartTime = currentTime - (uptime - (starttime/100));
				execTime = currentTime - adjustedStartTime;
			}
			string execTimeStr = this->FormatExecTime(execTime);
			string adjustedStartTimeStr = this->FormatStartTime(adjustedStartTime);

			// Add the data to a new data object and add th resultVector
			item->SetStatus(OvalEnum::STATUS_EXISTS);
			if(errMsg.compare("") != 0) {
				item->AppendMessage(new OvalMessage(errMsg));
			}

			item->AppendElement(new ItemEntity("exec_time",  execTimeStr, OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));
			item->AppendElement(new ItemEntity("pid", Common::ToString(pid), OvalEnum::DATATYPE_INTEGER, true, OvalEnum::STATUS_EXISTS));
			item->AppendElement(new ItemEntity("ppid", Common::ToString(ppid), OvalEnum::DATATYPE_INTEGER, false, OvalEnum::STATUS_EXISTS));
			item->AppendElement(new ItemEntity("priority", Common::ToString(priority), OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));
			item->AppendElement(new ItemEntity("ruid", Common::ToString(ruid)));
			item->AppendElement(new ItemEntity("scheduling_class",  "-", OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));
			item->AppendElement(new ItemEntity("start_time", adjustedStartTimeStr, OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));
			item->AppendElement(new ItemEntity("tty", ttyName, OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));
			item->AppendElement(new ItemEntity("user_id", Common::ToString(euid), OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));
		}

		items->push_back(item); 
	}
}

int Process58Probe::RetrieveStatFile(const char *process, int *pid, int *ppid, long *priority, unsigned long *starttime, string *errMsg) {

  // Stat File parameters.  While we're really only concerned with gathering the parameters
  // that are passed in, these variables are good placeholders in case we decide to collect
  // something else in the future.
  //
  int pgrp, session, tty, tpgid, exit_signal, processor = 0;
  long cutime, cstime, nice, placeholder, itrealvalue, rss = 0;
  unsigned long flags, minflt, cminflt, majflt, cmajflt, utime, stime, vsize, rlim = 0;
  unsigned long startcode, endcode, startstack, kstkesp, kstkeip, signal, blocked, sigignore = 0;
  unsigned long sigcatch, wchan, nswap, cnswap = 0;
  char comm[PATH_MAX];
  char state;

  FILE *statFile = NULL;

  // Generate the absolute path name for the 'stat' file
  string statPath = "/proc/";
  statPath.append(process);
  statPath.append("/stat");

  // Open the 'stat' file and read the contents
  if((statFile = fopen(statPath.c_str(), "r")) == NULL) {
    errMsg->append("Process58Probe: Unable to obtain process information for pid: ");
    errMsg->append(process);
    return(-1);

  } else {

    // Linux gives us a nicely formatted file for fscanf to pull in
    fscanf(statFile, "%d %s %c %d %d %d %d %d %lu %lu %lu %lu %lu %lu %lu %ld %ld %ld %ld %ld %ld %lu %lu %ld %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %d %d", pid, comm, &state, ppid, &pgrp, &session, &tty, &tpgid, &flags, &minflt, &cminflt, &majflt, &cmajflt, &utime, &stime, &cutime, &cstime, priority, &nice, &placeholder, &itrealvalue, starttime, &vsize, &rss, &rlim, &startcode, &endcode, &startstack, &kstkesp, &kstkeip, &signal, &blocked, &sigignore, &sigcatch, &wchan, &nswap, &cnswap, &exit_signal, &processor);

  }
  fclose(statFile);

  return(0);
}

int Process58Probe::RetrieveStatusFile(const char *process, int *ruid, int *euid, string *errMsg) {
	string statusFileName = string("/proc/")+process+"/status";
	errno = 0;
	ifstream statusFile(statusFileName.c_str());

	if (!statusFile) {
		// Processes come and go; if it went away, that's ok.
		// No need to report.  If there was some other
		// error, throw an exception.
		if (errno == ENOENT)
			return -1;

		*errMsg = "Process58Probe: Error opening "+statusFileName+": "+strerror(errno);
		return -1;
	}

	string line, label;
	while (statusFile) {
		getline(statusFile, line);
		istringstream iss(line);
		iss >> label;
		if (label == "Uid:") {
			*ruid = *euid = -1;
			errno = 0;
			iss >> *ruid >> *euid;
			if (iss)
				return 1;

			if ((*ruid == -1 || *euid == -1) && iss.eof())
				// we didn't get values for both... eol was premature
				*errMsg = "Premature end-of-line while reading ruid and euid from "+
					statusFileName;
			else if (errno != 0)
				*errMsg = "Error encountered while reading "+statusFileName+": "+
					strerror(errno);
			//else... we dunno what happened...

			return -1;
		}
	}

	*errMsg += "'Uid:' line not found in "+statusFileName;
	return -1;
}

void Process58Probe::RetrieveTTY(const char *process, char *ttyName) {
  int bytes = 0;

  // Generate the absolute path name for the stdout(0) file in 'fd'
  string ttyPath = "/proc/";
  ttyPath.append(process);
  ttyPath.append("/fd/0");

  // Attempt to read the name of the file linked to '0'
  bytes = readlink(ttyPath.c_str(), ttyName, TTY_LEN);

  // If there is an error, set the tty string to '?'
  if(bytes < 0 || strncmp(ttyName, "/dev", 4) != 0) {
    strncpy(ttyName, "?\0", 2);
  }
}

int  Process58Probe::RetrieveUptime(unsigned long *uptime, string *errMsg) {
  // The second value in this file represents idle time - we're not concerned with this.
  unsigned long idle = 0;
  FILE *uptimeHandle = NULL;

  // Open and parse the 'uptime' file
  if((uptimeHandle = fopen("/proc/uptime", "r")) == NULL) {
    errMsg->append("Process58Probe: Could not open /proc/uptime");
    uptime = 0;
    return(-1);
  } else {
    fscanf(uptimeHandle, "%lu %lu", uptime, &idle);
  }
  fclose(uptimeHandle);

  return(0);
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
	item->AppendElement(new ItemEntity("command", command, OvalEnum::DATATYPE_STRING, true));
	item->AppendElement(new ItemEntity("exec_time", formattedExecTime));
	item->AppendElement(new ItemEntity("pid", pidStr, OvalEnum::DATATYPE_INTEGER, true));
	item->AppendElement(new ItemEntity("ppid", Common::ToString(ppid), OvalEnum::DATATYPE_INTEGER));
	item->AppendElement(new ItemEntity("priority", Common::ToString(priority)));
	item->AppendElement(new ItemEntity("ruid", Common::ToString(rUserId)));
	item->AppendElement(new ItemEntity("scheduling_class", schedClass));
	item->AppendElement(new ItemEntity("start_time", formattedStartTime));
	if (devicePath.empty())
		item->AppendElement(new ItemEntity("tty", "", OvalEnum::DATATYPE_STRING,
										   false, OvalEnum::STATUS_DOES_NOT_EXIST));
	else
		item->AppendElement(new ItemEntity("tty", devicePath));
	item->AppendElement(new ItemEntity("user_id", Common::ToString(eUserId)));

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
    item->AppendElement(new ItemEntity("command", procdata[i].kp_proc.p_comm, OvalEnum::DATATYPE_STRING, true));
    item->AppendElement(new ItemEntity("exec_time", formattedExecTime));
    item->AppendElement(new ItemEntity("pid", Common::ToString(procdata[i].kp_proc.p_pid), OvalEnum::DATATYPE_INTEGER, true));
    item->AppendElement(new ItemEntity("ppid", Common::ToString(procdata[i].kp_eproc.e_ppid), OvalEnum::DATATYPE_INTEGER));
    item->AppendElement(new ItemEntity("priority", Common::ToString(procdata[i].kp_proc.p_priority)));
    item->AppendElement(new ItemEntity("scheduling_class","",OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_NOT_COLLECTED));
    item->AppendElement(new ItemEntity("start_time", formattedStartTime));
    item->AppendElement(new ItemEntity("tty", ttyStr, OvalEnum::DATATYPE_STRING, false, (ttyStr.compare("") == 0)?OvalEnum::STATUS_DOES_NOT_EXIST:OvalEnum::STATUS_EXISTS));
    item->AppendElement(new ItemEntity("user_id", Common::ToString(procdata[i].kp_eproc.e_ucred.cr_uid)));
    
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
