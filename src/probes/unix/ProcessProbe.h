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

#ifndef PROCESSPROBE_H
#define PROCESSPROBE_H

#include "AbsProbe.h"

#include <strings.h>
#include <dirent.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <iostream>
#include <string>

// Define some buffer lengths
#define CMDLINE_LEN 1024
#define SCHED_CLASS_LEN 128
#define TTY_LEN PATH_MAX

/**
	Data collector for process test.

	The following command produces process information sutable fro this probe.
    ps -eo class,etime,pid,uid,start_time,tty,priority,cmd
*/
class ProcessProbe : public AbsProbe {
public:
	virtual ~ProcessProbe();
    
	virtual ItemVector* CollectItems(Object* object);

	/** Ensure that the ProcessProbe is a singleton. */
	static AbsProbe* Instance();
	  
private:
	ProcessProbe();

	/** Return a new Item created for storing process information. */
	virtual Item* CreateItem();

	/** 
		Return the set of all process commands on the ssytem that match the specified object entities criteria.
		All process command names that are return have been checked and exist on the syytem.
		@param command an ObjectEntity* that represents the objects to collect on the ssytem
		@return The matching commands
	*/
	StringVector* GetCommands(ObjectEntity* command);
	
	/**
		Get all commands on the system that match the specified pattern.
		@param pattern a string used that commands are compared against.
		@param isRegex a bool that is indicates how system commands should be compared against the specified pattern
		@return The set of matching commands.
	*/
	StringVector* GetMatchingCommands(std::string pattern, bool isRegex);

	/**
		Return true if the specifeid command exists on the system.
		@param command a string that hold the name of the rpm to check for.
		@result The result of checking for the specified rpm on the system.
	*/
	bool CommandExists(std::string command);

	/**
		Get all the information for the command.
		@param command a string representing the command to collect information about.
		@param items a vector of items that matched the command.
	*/
	void GetPSInfo(std::string command, ItemVector* items);

	/**
		Read /proc/<pid>/cmdline to gather the application name and startup arguments
	*/
	int RetrieveCommandLine(char *process, char *cmdline, std::string *errMsg);

	/**
		Read the stat file for a specific process
	*/
	int RetrieveStatFile(char *process, int *uid, int *pid, int *ppid, long *priority, unsigned long *starttime, std::string *errMsg);

	/**
		 Since there appears to be no simple way to convert the 'tty' value contained in
		 '/proc/<pid>/stat' into a device name, we instead use '/proc/<pid>/fd/0', which is
		 normally linked to a device.  Note, the 'fd' directory is set read-only user, so
		 if this probe is not run as root, many of these reads will fail.  In that case, we
		 return '?' as the tty value.		
	*/
	void RetrieveTTY(char *process, char *ttyName);

	/**
		Read the value contained in '/proc/uptime/' so that we can calculate
		the start time and exec time of the running processes.
	*/
	int RetrieveUptime(unsigned long *uptime, std::string *errMsg);

	/**
		Convert the input seconds and conveert to a string format for exec time.
    */
	std::string FormatExecTime(unsigned long execTime);

	/**
		Convert the input seconds and convert to a string format for start time.
    */
	std::string FormatStartTime(unsigned long startTime);

	static ProcessProbe *instance;
};

#endif
