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

#include "ProcessProbe.h"

using namespace std;

//****************************************************************************************//
//								                  ProcessProbe Class										                ?/
//****************************************************************************************//

ProcessProbe *ProcessProbe::instance = NULL;

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

ProcessProbe::ProcessProbe() {}




ProcessProbe::~ProcessProbe() {
  instance = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//


AbsProbe* ProcessProbe::Instance() {

	// Use lazy initialization
	if(instance == NULL) 
		instance = new ProcessProbe();

	return instance;	
}




ItemVector* ProcessProbe::CollectItems(Object* object) {

	ObjectEntity* command = object->GetElementByName("command");

	// check datatypes - only allow string
	if(command->GetDatatype() != OvalEnum::DATATYPE_STRING) {
		throw ProbeException("Error: invalid data type specified on command. Found: " + OvalEnum::DatatypeToString(command->GetDatatype()));
	}

	// check operation - only allow  equals, not equals and pattern match
	if(command->GetOperation() != OvalEnum::OPERATION_EQUALS && command->GetOperation() != OvalEnum::OPERATION_PATTERN_MATCH && command->GetOperation() != OvalEnum::OPERATION_NOT_EQUAL) {
		throw ProbeException("Error: invalid operation specified on name. Found: " + OvalEnum::OperationToString(command->GetOperation()));
	}

	ItemVector *collectedItems = new ItemVector();

	StringPairVector* commands = this->GetCommands(command);
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





Item* ProcessProbe::CreateItem() {

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


StringPairVector* ProcessProbe::GetCommands(ObjectEntity* command) {

	StringPairVector* commands;
	StringVector commandEntityValues;
	StringVector::size_type numEntityValues;
	string pid;

	command->GetEntityValues(commandEntityValues); // ignoring flag return value for now
	numEntityValues = commandEntityValues.size();
	
	switch(command->GetOperation()) {

	case OvalEnum::OPERATION_EQUALS:
		commands = new StringPairVector();
		for (StringVector::iterator iter = commandEntityValues.begin();
			 iter != commandEntityValues.end();
			 ++iter) {
			if (this->CommandExists(*iter, pid))
				commands->push_back(new pair<string,string>(*iter, pid));
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

	// If var_ref was specified, we need to make a second pass since
	// in some cases above, we punted on actually satisfying the criteria
	// (and just got everything).  And at the least, we should check that
	// the var_check is satisfied.
	if (command->GetVarRef() != NULL) {
		ItemEntity* tmp = this->CreateItemEntity(command);
		StringPairVector::iterator it;
		for(it = commands->begin(); it != commands->end(); ) {
			tmp->SetValue((*it)->first);
			if(command->Analyze(tmp) == OvalEnum::RESULT_TRUE)
				++it;
			else {
				delete *it;
				it = commands->erase(it);
			}
		}
	}

	return commands;
}

#ifdef LINUX
void ProcessProbe::GetPSInfo(string command, string pidStr, ItemVector* items) {

	string errMsg = "";

	// Time parameters
	time_t currentTime;
	unsigned long adjustedStartTime, execTime = 0;

	// TTY String
	char ttyName[TTY_LEN + 1];

	// Process Parameters
	char cmdline[CMDLINE_LEN + 1];
	char schedulingClass[SCHED_CLASS_LEN + 1];

	int uid, pid, ppid;
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

	/*
	DIR *proc;
	struct dirent *readProc;

	// Step into the /proc directory
	if((proc = opendir("/proc")) == NULL) {
		errMsg.append("ProcessProbe: Could not open /proc");
		throw ProbeException(errMsg);

	} else {

		// Loop through all of the files - we're only concerned with those that
		// start with a digit
		while((readProc = readdir(proc)) != NULL) {
		if(isdigit(readProc->d_name[0])) {
*/
	if (stat(procDir.c_str(), &st) != 0)
		return; // oops, this process went away when we weren't looking

				// Clear the ps values
				memset(cmdline, 0, CMDLINE_LEN + 1);
				memset(schedulingClass, 0, SCHED_CLASS_LEN + 1);
				memset(ttyName, 0, TTY_LEN + 1);
				uid = pid = ppid = priority = starttime = 0;
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

					// Read the 'stat' file for the remaining process parameters
					status = RetrieveStatFile(pidStr.c_str(), &uid, &pid, &ppid, &priority, &starttime, &errMsg);
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
						item->AppendElement(new ItemEntity("pid", Common::ToString(pid), OvalEnum::DATATYPE_INTEGER, false, OvalEnum::STATUS_EXISTS));
						item->AppendElement(new ItemEntity("ppid", Common::ToString(ppid), OvalEnum::DATATYPE_INTEGER, false, OvalEnum::STATUS_EXISTS));
						item->AppendElement(new ItemEntity("priority", Common::ToString(priority), OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));
						item->AppendElement(new ItemEntity("scheduling_class",  "-", OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));
						item->AppendElement(new ItemEntity("start_time", adjustedStartTimeStr, OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));
						item->AppendElement(new ItemEntity("tty", ttyName, OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));
						item->AppendElement(new ItemEntity("user_id", Common::ToString(uid), OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));
					}

					items->push_back(item); 
				}
				/*			}
		} // else
    closedir(proc);
  }
				*/
}

int ProcessProbe::RetrieveStatFile(const char *process, int *uid, int *pid, int *ppid, long *priority, unsigned long *starttime, string *errMsg) {

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

  // While we're here, stat() the 'stat' file to get the uid for the process.  If
  // we want to convert this to a user name, feed the uid to getpwuid().
  //
  struct stat statBuf;

  if((stat(statPath.c_str(), &statBuf)) < 0) {
    errMsg->append("ProcessProbe: Unable to obtain uid information for pid:  ");
    errMsg->append(process);
    return(-1);
  } else {
    *uid = statBuf.st_uid;
  }
    
  // Open the 'stat' file and read the contents
  if((statFile = fopen(statPath.c_str(), "r")) == NULL) {
    errMsg->append("ProcessProbe: Unable to obtain process information for pid: ");
    errMsg->append(process);
    return(-1);

  } else {

    // Linux gives us a nicely formatted file for fscanf to pull in
    fscanf(statFile, "%d %s %c %d %d %d %d %d %lu %lu %lu %lu %lu %lu %lu %ld %ld %ld %ld %ld %ld %lu %lu %ld %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %d %d", pid, comm, &state, ppid, &pgrp, &session, &tty, &tpgid, &flags, &minflt, &cminflt, &majflt, &cmajflt, &utime, &stime, &cutime, &cstime, priority, &nice, &placeholder, &itrealvalue, starttime, &vsize, &rss, &rlim, &startcode, &endcode, &startstack, &kstkesp, &kstkeip, &signal, &blocked, &sigignore, &sigcatch, &wchan, &nswap, &cnswap, &exit_signal, &processor);

  }
  fclose(statFile);

  return(0);
}

void ProcessProbe::RetrieveTTY(const char *process, char *ttyName) {
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

int  ProcessProbe::RetrieveUptime(unsigned long *uptime, string *errMsg) {
  // The second value in this file represents idle time - we're not concerned with this.
  unsigned long idle = 0;
  FILE *uptimeHandle = NULL;

  // Open and parse the 'uptime' file
  if((uptimeHandle = fopen("/proc/uptime", "r")) == NULL) {
    errMsg->append("ProcessProbe: Could not open /proc/uptime");
    uptime = 0;
    return(-1);
  } else {
    fscanf(uptimeHandle, "%lu %lu", uptime, &idle);
  }
  fclose(uptimeHandle);

  return(0);
}

#elif defined SUNOS

void ProcessProbe::GetPSInfo(string command, string pidStr, ItemVector* items) {

	string psinfoFileName = "/proc/"+pidStr+"/psinfo";
	psinfo_t info;
	FILE *fp;
	pid_t pid, ppid;
	dev_t tty;
	timestruc_t startTime, execTime;
	int priority;
	string schedClass;
	uid_t userId;
	string formattedStartTime, formattedExecTime;
	string devicePath;
	Item *item;

	errno = 0;
	if ((fp = fopen(psinfoFileName.c_str(), "rb")) == NULL) {
		// Processes come and go; if it went away, that's ok.
		// No need to report.  If there was some other
		// error, throw an exception.
		if (errno == ENOENT)
			return;

		throw ProbeException("ProcessProbe: Error opening "+psinfoFileName+": "+strerror(errno));
	}

	if (fread(&info, sizeof(psinfo_t), 1, fp) < 1) {
		// the psinfo file for some reason was too small, or
		// there was some other error...
		fclose(fp);
		throw ProbeException("ProcessProbe: Error reading process info from "+psinfoFileName+": "+strerror(errno));
	}

	fclose(fp);

	execTime = info.pr_time;
	startTime = info.pr_start;
	pid = info.pr_pid;
	ppid = info.pr_ppid;

	//note that this priority is for the "representative" lwp.  I guess
	//in general, priority is not a property of a process, but a lwp
	//(a thread).
	priority = info.pr_lwp.pr_pri;

	// also a lwp property, not a process property
	schedClass = info.pr_lwp.pr_clname;
	
	tty = info.pr_ttydev;
	userId = info.pr_uid;
	
	formattedExecTime = this->FormatExecTime(execTime.tv_sec);
	formattedStartTime = this->FormatStartTime(startTime.tv_sec);

	devicePath = this->GetDevicePath(tty);

	item = this->CreateItem();
	item->SetStatus(OvalEnum::STATUS_EXISTS);
	item->AppendElement(new ItemEntity("command", command, OvalEnum::DATATYPE_STRING, true));
	item->AppendElement(new ItemEntity("exec_time", formattedExecTime));
	item->AppendElement(new ItemEntity("pid", Common::ToString(pid), OvalEnum::DATATYPE_INTEGER));
	item->AppendElement(new ItemEntity("ppid", Common::ToString(ppid), OvalEnum::DATATYPE_INTEGER));
	item->AppendElement(new ItemEntity("priority", Common::ToString(priority)));
	item->AppendElement(new ItemEntity("scheduling_class", schedClass));
	item->AppendElement(new ItemEntity("start_time", formattedStartTime));
	if (!devicePath.empty())
		item->AppendElement(new ItemEntity("tty", devicePath));
	item->AppendElement(new ItemEntity("user_id", Common::ToString(userId)));
	
	items->push_back(item);
}

string ProcessProbe::GetDevicePath(dev_t tty) {
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
			Log::Message("ProcessProbe: Error occurred during search for device path under "+*iter+": "+strerror(errno));
	}

	return "";
}

StringVector ProcessProbe::GetDeviceSearchDirs() {
	StringVector dirs;
	string line;
	ifstream file("/etc/ttysrch");	

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
			string::iterator firstSpace = find_if(line.begin(), line.end(), (int (*)(int))isspace);
			dirs.push_back(line.substr(0, firstSpace - line.begin()));
		}
	}

	return dirs;
}

#endif

bool ProcessProbe::CommandExists(string command, string &pid) {
	bool exists = false;
	string errMsg = "";

	// Process Parameters
	char cmdline[CMDLINE_LEN + 1];

	DIR *proc;
	struct dirent *readProc;

	// Step into the /proc directory
	if((proc = opendir("/proc")) == NULL) {
		errMsg.append("ProcessProbe: Could not open /proc");
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
		} // else
    closedir(proc);
  }
  return exists;
}

StringPairVector* ProcessProbe::GetMatchingCommands(string pattern, bool isRegex) {
	StringPairVector* commands = new StringPairVector();
	string errMsg = "";

	// Process Parameters
	char cmdline[CMDLINE_LEN + 1];

	DIR *proc;
	struct dirent *readProc;

	// Step into the /proc directory
	if((proc = opendir("/proc")) == NULL) {
		errMsg.append("ProcessProbe: Could not open /proc");
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
		} // else
    closedir(proc);
  }

  return commands;
}

int ProcessProbe::RetrieveCommandLine(const char *process, char *cmdline, string *errMsg) {
	FILE *cmdlineFile = NULL;

	// Build the absolute path to the command line file
	string cmdlinePath = "/proc/";
	cmdlinePath.append(process);

#ifdef LINUX
	int i = 0;
	int bytes = 0;
	cmdlinePath.append("/cmdline");

	// Open the file for reading
	if((cmdlineFile = fopen(cmdlinePath.c_str(), "r")) == NULL) {
		errMsg->append("ProcessProbe: Unable to obtain command line for pid: ");
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
	if ((cmdlineFile = fopen(cmdlinePath.c_str(), "rb")) == NULL) {
		*errMsg = string("ProcessProbe: Unable to obtain command line for pid: ") + process;
		return(-1);
	} else {

		psinfo_t info;
		errno = 0;
		if (fread(&info, sizeof(psinfo_t), 1, cmdlineFile) < 1) {
			*errMsg = string("ProcessProbe: Unable to obtain commandline for pid ")+process+": error reading "+cmdlinePath+": "+strerror(errno);
			fclose(cmdlineFile);
			return -1;
		}

		fclose(cmdlineFile);

		strncpy(cmdline, info.pr_psargs, CMDLINE_LEN);
		cmdline[CMDLINE_LEN] = '\0'; // ensure null-termination		
	}

#endif
	return(0);
}

string ProcessProbe::FormatExecTime(time_t execTime) {

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

string ProcessProbe::FormatStartTime(time_t startTime) {

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

void ProcessProbe::DeleteCommands(StringPairVector *commands) {
	if (commands == NULL) return;
	
	for(StringPairVector::iterator iter = commands->begin();
		iter != commands->end();
		++iter)
		delete *iter;

	delete commands;
}
