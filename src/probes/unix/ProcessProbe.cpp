//
//
//****************************************************************************************//
// Copyright (c) 2002-2008, The MITRE Corporation
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

//****************************************************************************************//
//								                  ProcessProbe Class										                ?/
//****************************************************************************************//

ProcessProbe *ProcessProbe::instance = NULL;




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

	StringVector* commands = this->GetCommands(command);
	if(commands->size() > 0) {
		StringVector::iterator iterator;
		for(iterator = commands->begin(); iterator != commands->end(); iterator++) {		
			this->GetPSInfo((*iterator), collectedItems);
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

	commands->clear();
	delete commands;

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


StringVector* ProcessProbe::GetCommands(ObjectEntity* command) {

	StringVector* commands = NULL;

	// does this name use variables?
	if(command->GetVarRef() == NULL) {
		
		// proceed based on operation
		if(command->GetOperation() == OvalEnum::OPERATION_EQUALS) {
			commands = new StringVector();
			// if the command exists add it to the list 
			if(this->CommandExists(command->GetValue())) {
				commands->push_back(command->GetValue());
			}

		} else if(command->GetOperation() == OvalEnum::OPERATION_NOT_EQUAL) {
			
			commands = this->GetMatchingCommands(command->GetValue(), false);

		} else if(command->GetOperation() == OvalEnum::OPERATION_PATTERN_MATCH) {
			commands = this->GetMatchingCommands(command->GetValue(), true);
		}		

	} else {

		commands = new StringVector();

		// Get all names
		StringVector allCommands;

		if(command->GetOperation() == OvalEnum::OPERATION_EQUALS) {
			// in the case of equals simply loop through all the 
			// variable values and add them to the set of all names
			// if they exist on the system
			VariableValueVector::iterator iterator;
			for(iterator = command->GetVarRef()->GetValues()->begin(); iterator != command->GetVarRef()->GetValues()->end(); iterator++) {
				
				if(this->CommandExists((*iterator)->GetValue())) {
					allCommands.push_back((*iterator)->GetValue());
				}
			}

		} else {
            this->GetMatchingCommands(".*", &allCommands);
		}
	
		// loop through all names on the system
		// only keep names that match operation and value and var check
		ItemEntity* tmp = this->CreateItemEntity(command);
		StringVector::iterator it;
		for(it = allCommands.begin(); it != allCommands.end(); it++) {
			tmp->SetValue((*it));
			
			if(command->Analyze(tmp) == OvalEnum::RESULT_TRUE) {
				commands->push_back((*it));
			}
		}
	}

	return commands;
}

void ProcessProbe::GetPSInfo(string command, ItemVector* items) {

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
				memset(schedulingClass, 0, SCHED_CLASS_LEN + 1);
				memset(ttyName, 0, TTY_LEN + 1);
				uid = pid = ppid = priority = starttime = 0;
				adjustedStartTime = execTime = 0;
				errMsg = "";

				// Retrieve the command line with arguments
				status = RetrieveCommandLine(readProc->d_name, cmdline, &errMsg);
				if(status < 0) { 
				    closedir(proc);
					throw ProbeException(errMsg);

				// If the command line matches the input command line get the remaining 
				// data and add a new data object to the items
				} else if(status == 0 && command.compare(cmdline) == 0) {

					Item* item = this->CreateItem();
					item->SetStatus(OvalEnum::STATUS_EXISTS);
					item->AppendElement(new ItemEntity("command",  command, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));

					// Read the 'stat' file for the remaining process parameters
					status = RetrieveStatFile(readProc->d_name, &uid, &pid, &ppid, &priority, &starttime, &errMsg);
					if(status < 0) {
						item->AppendMessage(new OvalMessage(errMsg));
					} else {

						// We can retrieve a value for the tty from the 'stat' file, but it's unclear
						// how you convert that to a device name.  Therefore, we ignore that value
						// and grab the device stdout(fd/0) is attached to.
						RetrieveTTY(readProc->d_name, ttyName);

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
			}
		} // else
    closedir(proc);
  }
}

bool ProcessProbe::CommandExists(string command) {
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
					break;
				}
			}
		} // else
    closedir(proc);
  }
  return exists;
}

StringVector* ProcessProbe::GetMatchingCommands(string pattern, bool isRegex) {
	StringVector* commands = new StringVector();
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

				// If the command line matches the input command line store it
				} else if(status == 0 && this->IsMatch(pattern, cmdline, isRegex)) {
					commands->push_back(cmdline);
				}
			}
		} // else
    closedir(proc);
  }
  return commands;
}

int ProcessProbe::RetrieveCommandLine(char *process, char *cmdline, string *errMsg) {
	int i = 0;
	int bytes = 0;
	FILE *cmdlineFile = NULL;

	// Build the absolute path to the command line file
	string cmdlinePath = "/proc/";
	cmdlinePath.append(process);
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

	return(0);
}

int ProcessProbe::RetrieveStatFile(char *process, int *uid, int *pid, int *ppid, long *priority, unsigned long *starttime, string *errMsg) {

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

void ProcessProbe::RetrieveTTY(char *process, char *ttyName) {
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

string ProcessProbe::FormatExecTime(unsigned long execTime) {

	/**
		This is the cumulative CPU time, formatted in [DD-]HH:MM:SS where DD is the number of days when execution time is 24 hours or more.

		Input is seconds 

		Divide by 86400 to get days
		Divide remainder of above division by 3600 to get hours
		Divide remainder by 60 to get minutes, remainder is seconds
	*/

	string execTimeStr = "";
	unsigned long days = execTime/86400;
	if(days > 0) {
		if(days > 9) execTimeStr = Common::ToString(days);
		else execTimeStr = "0" + Common::ToString(days);

		execTimeStr.append("-");
	}	

	unsigned long hours = (execTime%86400)/3600;
	if(hours > 9) execTimeStr.append("" + Common::ToString(hours));
	else execTimeStr.append("0" + Common::ToString(hours));

	execTimeStr.append(":");

	unsigned long minutes = ((execTime%86400)%3600)/60;
	if(minutes > 9) execTimeStr.append("" + Common::ToString(minutes));
	else execTimeStr.append("0" + Common::ToString(minutes));

	execTimeStr.append(":");

	unsigned long seconds = ((execTime%86400)%3600)%60;
	if(seconds > 9) execTimeStr.append("" + Common::ToString(seconds));
	else execTimeStr.append("0" + Common::ToString(seconds));

	return execTimeStr;
}

string ProcessProbe::FormatStartTime(unsigned long startTime) {

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
