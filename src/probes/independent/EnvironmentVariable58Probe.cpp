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
#  include <string>
#  include <map>
#  include <memory>

#  include <Log.h>
#  include <DirGuard.h>
#  include <VectorPtrGuard.h>
#elif defined SUNOS
#  include <map>
#  include <memory>
#  include <string>

#  include <Log.h>
#  include <VectorPtrGuard.h>
#elif defined WIN32
#  include <Windows.h>
#  include <Tlhelp32.h>
#  include <winternl.h>
#  include <sstream>
#  include <iomanip>
#  include <map>
#  include <memory>
#  include <string>

#  include <AutoCloser.h>
#  include <VectorPtrGuard.h>
#  include <ArrayGuard.h>
#  include <PrivilegeGuard.h>

// keep other windows headers (e.g. ntsecapi.h) from
// incompatibly redefining STRING and UNICODE_STRING
// as compared to <winternl.h>!
// Should I be fancier here and restore the _NTDEF_
// definition to whatever it was before I defined it??
#  define _NTDEF_
#  include <WindowsCommon.h>
#  undef _NTDEF_

#endif

/*
#ifdef DARWIN
#include <crt_externs.h>
#define _environ (*_NSGetEnviron())
#elif*/
#ifndef WIN32
extern char ** _environ; 
#endif


#include "EnvironmentVariable58Probe.h"

using namespace std;

#ifdef LINUX
#  define PROC_DIR "/proc"
#endif


namespace {

#ifdef WIN32
	/** type of NtQueryInformationProcess(). */
	typedef NTSTATUS (NTAPI *NtQIPType)(
		HANDLE,  ///< ProcessHandle
		PROCESSINFOCLASS, ///< ProcessInformationClass
		PVOID, ///< (out) ProcessInformation
		ULONG, ///< ProcessInformationLength
		PULONG ///< (out, optional) ReturnLength
	);

	/** type of IsWow64Process(). */
	typedef BOOL (WINAPI *IwpType)(
		HANDLE, ///< hProcess
		PBOOL ///< (out) Wow64Process
	);

	/** handle to the "ntdll.dll" module */
	HINSTANCE ntdll = NULL;

	/** function pointer to NtQueryInformationProcess() */
	NtQIPType ntqip = NULL;

	/** function pointer to IsWow64Process() */
	IwpType iwp = NULL;

	/**
	 * Offsets used to get the environment in a 32-bit process.
	 */
	const ptrdiff_t OFFSETS32[] = {0x10, 0x48};

	/**
	 * Offsets used to get the environment in a 64-bit process.
	 * These were obtained via inspection of some code disassembly...
	 */
	const ptrdiff_t OFFSETS64[] = {0x20, 0x80};

	/**
	 * Converts an environment block to a std::map of
	 * names and values.  This is templated on the char type, because
	 * I can get the environment block of the current process
	 * as a narrow char block via GetEnvironmentStringsA(),
	 * whereas the environment block as directly extracted
	 * from a process' memory seems always to use wide chars.
	 * The code for decoding the block into a map is identical
	 * except for the char type, so it made sense to just
	 * write it as a template.
	 */
	template <typename CharType>
	map<basic_string<CharType>, basic_string<CharType> > envBlock2Map(const CharType *block);

	/**
	 * Detects whether the given process is 32-bit or not.
	 */
	bool is32BitProcess(HANDLE procHandle, DWORD pid);

	/**
	 * Resizes the given block, copies existing data over, and appends
	 * \p len chars of \p data to the end.
	 *
	 * \param block the memory block to add to
	 * \param blockSize the size of \p block, in characters
	 * \param data The data to append
	 * \param len The number of chars from \p data to append
	 */
	void appendToMemoryBlock(ArrayGuard<wchar_t> &block, 
		size_t blockSizeChars, const wchar_t *data, size_t lenChars);

#endif

#if defined WIN32 || defined LINUX

	/**
	 * The idea of being robust to process termination originated in
	 * the unix process58 probe.  The same issue occurs here, so I
	 * am going to use it.
	 */
	enum ProcStatus {
		PROC_OK, ///< no error
		PROC_TERMINATED, ///< the process terminated
		PROC_ERROR ///< some other error occurred
	};

	/**
	 * Gets all PIDs on this system.  Originally written to
	 * get them as strings because the linux code doesn't need
	 * to do numeric ops on them but does need to do string ops.
	 * And windows uses its own type for pids, so string seemed
	 * like a simple neutral format, so I don't have to have
	 * platform-specific prototypes...
     */
	StringVector GetAllPids();

	/**
	 * Gets the environment of the process with the given pid.
	 * If the return value is PROC_ERROR, an error message is
	 * assigned to errMsg.  Otherwise, errMsg is not modified.
	 *
	 * \param[in] pid The id of the process to check
	 * \param[out] env A map to populate with environment name/value pairs.
	 * \param[out] errMsg An error message, in the event PROC_ERROR is
	 *   returned
	 * \return A status code indicating what happened during collection of
	 *   the environment data.
	 */
	ProcStatus GetEnvForPid(const string &pid, map<string, string> *env, string *errMsg);

#endif

#ifndef MACOS

	auto_ptr<Item> CreateItem();

	/**
	 * Gets the environment map for this process.
	 * Windows: uses GetEnvironmentStrings().
	 * Linux: uses the 'environ' global var.
	 */
	map<string, string> GetEnvForThisProcess();

#endif

}

//****************************************************************************************//
//								EnvironmentVariableProbe Class							  //	
//****************************************************************************************//
EnvironmentVariable58Probe *EnvironmentVariable58Probe::instance = NULL;

EnvironmentVariable58Probe::EnvironmentVariable58Probe() {

#ifdef WIN32

#  ifndef _WIN64
	// We only support collecting info about 32-bit processes
	// from 32-bit ovaldi.  Maybe we should warn about this in a
	// 64-bit environment...
	if (WindowsCommon::Is64BitOS())
		Log::Message("Warning: this is a 32-bit build running on a "
			"64-bit OS; environment information from 64-bit processes "
			"will not be collected!");
#  endif

	// We get function pointers dynamically because (a)
	// some are considered "internal" to windows, so we
	// have to manually load DLLs and search their exports,
	// and (b) because even for DLLs linked in by default,
	// we want to be robust to the possibility that the
	// function just isn't there.

	// I think it's ok for this ctor to init private globals
	// because we know there will only ever be one instance.
	// So when the one instance is init'd, init the globals,
	// and when it is destroyed, destroy the globals.

	if (!ntdll) {
		ntdll = LoadLibraryA("ntdll.dll");
		if (!ntdll) {
			throw ProbeException("Couldn't find function NtQueryInformationProcess():"
				"  Error loading ntdll.dll:  " +
				WindowsCommon::GetErrorMessage(GetLastError()));
		}
	}

	ntqip = (NtQIPType)GetProcAddress(ntdll, "NtQueryInformationProcess");
	if (!ntqip) {
		FreeLibrary(ntdll);
		ntdll = NULL;
		throw ProbeException("Error finding NtQueryInformationProcess() in ntdll.dll:  " +
			WindowsCommon::GetErrorMessage(GetLastError()));
	}

	HINSTANCE kerneldll = GetModuleHandleA("kernel32.dll");
	if (!kerneldll) {
		FreeLibrary(ntdll);
		ntdll = NULL;
		throw ProbeException("Couldn't find function IsWow64Process():"
			"  Error getting handle to kernel32.dll:  " +
			WindowsCommon::GetErrorMessage(GetLastError()));
	}

	iwp = (IwpType)GetProcAddress(kerneldll, "IsWow64Process");
	if (!iwp) {
		FreeLibrary(ntdll);
		ntdll = NULL;
		throw ProbeException("Error finding IsWow64Process() in kernel32.dll:  " +
			WindowsCommon::GetErrorMessage(GetLastError()));
	}

#endif

}

EnvironmentVariable58Probe::~EnvironmentVariable58Probe() {

#ifdef WIN32

	if (ntdll) {
		FreeLibrary(ntdll);
		ntdll = NULL;
	}

	ntqip = NULL;
	iwp = NULL;

#endif

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

#ifndef MACOS

	VectorPtrGuard<Item> collectedItems(new ItemVector());

	// get the name from the provided object
	ObjectEntity *name = object->GetElementByName("name");
	ObjectEntity *pid = object->GetElementByName("pid");


	// Special-case nil pid, which means get env of the current process.
	if (pid->GetNil()) {
		map<string, string> env = GetEnvForThisProcess();

		string pidStr = Common::ToString(
#  ifdef WIN32
				GetCurrentProcessId()
#  else
				getpid()
#  endif
		);

		for (map<string,string>::const_iterator it = env.begin();
			it != env.end(); ++it) {

			auto_ptr<ItemEntity> nameIe(new ItemEntity("name", it->first));
			if (name->Analyze(nameIe.get()) != OvalEnum::RESULT_TRUE)
				continue;

			auto_ptr<Item> item = ::CreateItem();
			item->SetStatus(OvalEnum::STATUS_EXISTS);
			item->AppendElement(new ItemEntity("pid", pidStr, OvalEnum::DATATYPE_INTEGER));
			item->AppendElement(nameIe.release());
			item->AppendElement(new ItemEntity("value", it->second));
			collectedItems->push_back(item.release());
		}

		return collectedItems.release();
	}

#  if defined WIN32 || defined LINUX


#    ifdef WIN32
	
	// Giving ourselves this privilege seems to gain us access
	// to a lot more processes.
	PrivilegeGuard pg(SE_DEBUG_NAME, false);

#    endif
	
	StringVector pidsToCheck;
	if (pid->GetOperation() == OvalEnum::OPERATION_EQUALS)
		/*OvalEnum::Flag flag =*/ pid->GetEntityValues(pidsToCheck);
	else
		pidsToCheck = GetAllPids();

	for (StringVector::iterator pidIter = pidsToCheck.begin();
		 pidIter != pidsToCheck.end();
		 ++pidIter) {

		ItemEntity pidIe("pid", *pidIter, OvalEnum::DATATYPE_INTEGER);
		if (pid->GetNil() || pid->Analyze(&pidIe) == OvalEnum::RESULT_TRUE) {

			map<string, string> envForPid;
			string errMsg;
			ProcStatus status = GetEnvForPid(*pidIter, &envForPid, &errMsg);

			if (status == PROC_TERMINATED)
				continue;
			else if (status == PROC_ERROR) {
				auto_ptr<Item> item = ::CreateItem();
				item->SetStatus(OvalEnum::STATUS_ERROR);
				item->AppendElement(new ItemEntity(pidIe));
				item->AppendMessage(new OvalMessage(errMsg));
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

#  else

	throw ProbeException("Only collection of the environment of the current "
						 "process is currently supported on this platform.");

#  endif // #if defined WIN32 || defined LINUX

#else

	// this should never be executed because ProbeFactory
	// doesn't create an instance of this probe on a
	// mac, but... may as well be safe?
	throw ProbeException("environmentvariable58 test is not supported on this platform.");

#endif // #ifndef MACOS
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

// this is never used
Item* EnvironmentVariable58Probe::CreateItem() {
	return NULL;
}

namespace {

#ifndef MACOS
	auto_ptr<Item> CreateItem() {

		auto_ptr<Item> item(new Item(0, 
									 "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#independent", 
									 "ind-sc", 
									 "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#independent independent-system-characteristics-schema.xsd", 
									 OvalEnum::STATUS_ERROR, 
									 "environmentvariable58_item"));

		return item;
	}

#endif

#if !defined MACOS && !defined WIN32

	map<string, string> GetEnvForThisProcess() {
		map<string, string> env;
		int i = 0;
		while (_environ[i]) {
			char *eq = _environ[i];
			while (*eq && *eq != '=') ++eq;
			if (*eq)
				env[string(_environ[i],eq)] = string(eq+1);
			else
				Log::Debug(string("Found an env entry without an '=' sign: ") +
						   _environ[i]);

			++i;
		}

		return env;
	}

#endif

#ifdef LINUX
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

	ProcStatus GetEnvForPid(const string &pid, map<string, string> *env, string *errMsg) {
		string envFile = Common::BuildFilePath(
			Common::BuildFilePath(PROC_DIR, pid),
			"environ");

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
						(*env)[name] = val;
//						Log::Debug("found env var for pid "+pid+": "+name+" = "+val);
					}

					name.clear();
					val.clear();
					inName = true;
				}
			}
		}

		if (errno == ENOENT)
			return PROC_TERMINATED;
		else if (errno) {
			*errMsg = "Error reading "+envFile+": "+strerror(errno);
			return PROC_ERROR;
		}		

		return PROC_OK;
	}

#elif defined WIN32

	template <typename CType>
	map<basic_string<CType>, basic_string<CType> > envBlock2Map(const CType *block) {

		typedef basic_string<CType> SType;

		map<SType, SType> env;
		const CType *beg = block;
		while (*beg) { // while not an empty string

			// Some funny env vars start with '='; these seem to be a
			// crutch for cmd.exe.  I will ignore them for now...
			// See:
			// http://blogs.msdn.com/b/oldnewthing/archive/2010/05/06/10008132.aspx
			if (*beg == '=') {
				while (*beg) ++beg;
				++beg;
				continue;
			}

			// search for '='
			const CType *eq = beg;
			while (*eq && *eq != CType('=')) ++eq;
			if (eq) {
				env[SType(beg, eq)] = SType(eq+1);
				beg = eq;
				while (*beg) ++beg; // skip to the end of the entry
			} else {

				wostringstream woss;
				woss << "Found an env entry without an '=' sign: " << beg;
				Log::Debug(WindowsCommon::UnicodeToAsciiString(woss.str()));

				beg = eq;
			}

			++beg; // go to the next entry
		}

		return env;
	}

	map<string, string> GetEnvForThisProcess() {

		// Docs for GetEnvironmentStrings() say NULL is
		// returned on failure, but don't say anything
		// about getting an error code.  So I am resetting
		// the error code and then checking it afterword,
		// just in case...
		SetLastError(ERROR_SUCCESS);

		// I can use LPCH as my "handle" type here.
		AutoCloser<LPCH, BOOL(WINAPI&)(LPCH)> envGuard(
			GetEnvironmentStringsA(),
			FreeEnvironmentStringsA,
			"Environment strings"
		);

		if (!envGuard.get()) {
			if (GetLastError() == ERROR_SUCCESS)
				throw ProbeException("Could not get current process' environment.");
			else
				throw ProbeException("Error getting current process' environment: " +
					WindowsCommon::GetErrorMessage(GetLastError()));
		}

		return envBlock2Map(envGuard.get());
	}

	bool is32BitProcess(HANDLE procHandle, DWORD pid) {
		BOOL is32BitProc;
		if (!WindowsCommon::Is64BitOS())
			// only 32-bit processes can run on 32-bit OSs.
			is32BitProc = TRUE;
		else {
			if (!iwp(procHandle, &is32BitProc))
				throw ProbeException(
					"IsWow64Process() failed for pid " +
					Common::ToString(pid) +
					": " + WindowsCommon::GetErrorMessage(GetLastError()));
		}

		return is32BitProc != FALSE;
	}

	StringVector GetAllPids() {
		AutoCloser<HANDLE, BOOL(WINAPI&)(HANDLE)> procSnapshot(
			CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0),
			CloseHandle,
			"Process snapshot"
		);

		if (procSnapshot.get() == INVALID_HANDLE_VALUE)
			throw ProbeException("Could not get the current set of process IDs: " +
				WindowsCommon::GetErrorMessage(GetLastError()));

		StringVector pids;
		BOOL result;
		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof(PROCESSENTRY32);
		for (result = Process32First(procSnapshot.get(), &procEntry);
			result;
			result = Process32Next(procSnapshot.get(), &procEntry)) {

			string pidStr = Common::ToString(procEntry.th32ProcessID);

			// This pid is the "System" process and seems to be always
			// there but never openable.  So just add it regardless...
			// There will be a failure later, but I want it to get into
			// the list of pids.  Is there some better way of checking for
			// "special" processes??
			if (procEntry.th32ProcessID == 4) {
				pids.push_back(pidStr);
				continue;
			}

#  ifndef _WIN64
				// If this is a 32-bit build, only return PIDs of 32-bit
				// processes.  Getting the environment of a 64-bit
				// process from a 32-bit app is too hard... And always
				// be ok if the process wasn't found.  Processes
				// will be continually coming and going, and a process
				// with a given pid can disappear any time.
				AutoCloser<HANDLE, BOOL(WINAPI&)(HANDLE)> procHandle(
					OpenProcess(PROCESS_QUERY_INFORMATION,
						FALSE, procEntry.th32ProcessID),
					CloseHandle,
					"Process "+pidStr);

				if (procHandle.get() == NULL) {
					DWORD err = GetLastError();
					// this seems to be what you get if the pid
					// didn't exist... I would have expected
					// ERROR_FILE_NOT_FOUND or something.
					if (err == ERROR_INVALID_PARAMETER)
						continue;
					else
						throw ProbeException("Error obtaining handle to pid " +
							pidStr + ": " +
							WindowsCommon::GetErrorMessage(err));
				}

				if (is32BitProcess(procHandle.get(), procEntry.th32ProcessID))
#  endif
				pids.push_back(pidStr);

		}

		if (GetLastError() != ERROR_NO_MORE_FILES)
			throw ProbeException("Error enumerating processes: " + 
				WindowsCommon::GetErrorMessage(GetLastError()));

		return pids;
	}

	ProcStatus GetEnvForPid(const string &pid, map<string, string> *env, string *errMsg) {
		DWORD pidNum;

		if (!Common::FromString(pid, &pidNum))
			throw ProbeException("Couldn't convert pid to DWORD: "+pid);

		AutoCloser<HANDLE, BOOL(WINAPI&)(HANDLE)> procHandle(
			OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,
				FALSE, pidNum),
			CloseHandle,
			"Process "+pid);

		if (!procHandle.get()) {
			DWORD err = GetLastError();
			if (err == ERROR_INVALID_PARAMETER)
				// this error means the pid wasn't found.
				return PROC_TERMINATED;
			else {
				*errMsg = "Couldn't open process: "+WindowsCommon::GetErrorMessage(err);
				return PROC_ERROR;
			}
		}

		bool is32BitProc = is32BitProcess(procHandle.get(), pidNum);

#  ifndef _WIN64
		// Verify the bitness of the process again; the original process
		// may have terminated and been replaced by another with a
		// different bitness.  Doesn't hurt to play it safe...
		if (!is32BitProc)
			return PROC_TERMINATED;
#  endif

		const ptrdiff_t *offsets;

		if (is32BitProc)
			offsets = OFFSETS32;
		else
			offsets = OFFSETS64;

	// is there any easy way to get an error message for an NTSTATUS value??
#  define SET_NTSTATUS_ERROR_MSG(_msg, _nts) \
				ostringstream oss; \
				oss << _msg " for pid " << pid << \
					": nstatus error 0x" << hex << setfill('0') << setw(8) << uppercase << (_nts); \
				*errMsg = oss.str()

		// First we get a pointer to the process PEB.
		char *pebAddr;
#  ifdef _WIN64
		if (is32BitProc) {
			// The 64->32 case
			// even tho we're querying a 32-bit process, an 8-byte address will
			// be filled out here.
			ULONG_PTR tmpPtr;
			NTSTATUS nts = ntqip(procHandle.get(), ProcessWow64Information, &tmpPtr, sizeof(tmpPtr), NULL);
			if (nts) {
				SET_NTSTATUS_ERROR_MSG("Couldn't get wow64 process info", nts);
				return PROC_ERROR;
			}

			pebAddr = (char*)tmpPtr;
		} else
#  endif
		{
			// The 64->64 and 32->32 cases
			PROCESS_BASIC_INFORMATION pbi;
			NTSTATUS nts = ntqip(procHandle.get(), ProcessBasicInformation, &pbi, sizeof(pbi), NULL);

			if (nts) {
				SET_NTSTATUS_ERROR_MSG("Couldn't get process info", nts);
				return PROC_ERROR;
			}

			pebAddr = (char*)pbi.PebBaseAddress;
		}

#  undef SET_NTSTATUS_ERROR_MSG

		// First offset is relative to the PEB, and leads to
		// a RTL_USER_PROCESS_PARAMETERS (rupp) pointer.
		char *ruppPtrPtr = pebAddr + offsets[0];
		char *ruppPtr;
#  ifdef _WIN64
		if (is32BitProc) {
			// The 64->32 case
			UINT32 tmp;
			if (!ReadProcessMemory(procHandle.get(), ruppPtrPtr, &tmp, sizeof(tmp), NULL)) {
				*errMsg = "Couldn't read memory of pid " + pid + ": " +
					WindowsCommon::GetErrorMessage(GetLastError());
				return PROC_ERROR;
			}
			ruppPtr = (char*)tmp;
		} else
#  endif
		{
			// The 64->64 and 32->32 cases
			if (!ReadProcessMemory(procHandle.get(), ruppPtrPtr, &ruppPtr, sizeof(ruppPtr), NULL)) {
				*errMsg = "Couldn't read memory of pid " + pid + ": " +
					WindowsCommon::GetErrorMessage(GetLastError());
				return PROC_ERROR;
			}
		}

		// Second offset is relative to ruppPtr.  This leads to
		// the pointer to the environment block.
		char *envPtrPtr = ruppPtr + offsets[1];
		wchar_t *envPtr;
#  ifdef _WIN64
		if (is32BitProc) {
			// The 64->32 case
			UINT32 tmp;
			if (!ReadProcessMemory(procHandle.get(), envPtrPtr, &tmp, sizeof(tmp), NULL)) {
				*errMsg = "Couldn't read memory of pid " + pid + ": " +
					WindowsCommon::GetErrorMessage(GetLastError());
				return PROC_ERROR;
			}
			envPtr = (wchar_t*)tmp;
		} else
#  endif
		{
			// The 64->64 and 32->32 cases
			if (!ReadProcessMemory(procHandle.get(), envPtrPtr, &envPtr, sizeof(envPtr), NULL)) {
				*errMsg = "Couldn't read memory of pid " + pid + ": " +
					WindowsCommon::GetErrorMessage(GetLastError());
				return PROC_ERROR;
			}
		}

		// We don't know how big the env block is, so I will just
		// read fixed size chunks of memory, searching for the
		// block-end as I go.

		 // this size is in characters (wchar_t)
#  define ENV_BLOCK_SIZE_CHARS 500
		wchar_t tempEnvBlock[ENV_BLOCK_SIZE_CHARS];
		bool foundEnd = false;

		// we will incrementally append to this memory block.
		ArrayGuard<wchar_t> envBlock;
		size_t blockSizeUsed = 0, i; // block size in chars again
		do {

			if (!ReadProcessMemory(procHandle.get(), envPtr, &tempEnvBlock, sizeof(tempEnvBlock), NULL)) {
				*errMsg = "Couldn't read memory of pid " + pid + ": " +
					WindowsCommon::GetErrorMessage(GetLastError());
				return PROC_ERROR;
			}

			i = 0;
			while (!foundEnd && i < ENV_BLOCK_SIZE_CHARS) {

				// try to find a '\0'
				while (i < ENV_BLOCK_SIZE_CHARS && tempEnvBlock[i]) ++i;

				if (i < ENV_BLOCK_SIZE_CHARS) {
					// found a '\0'.  Does another '\0' follow it?
					if (i < ENV_BLOCK_SIZE_CHARS-1 && !tempEnvBlock[i+1]) {
						foundEnd = true;
						i += 2; // skip to 1-past the second '\0'
					} else
						++i; // no, skip over the '\0'
				}
			}

			appendToMemoryBlock(envBlock, blockSizeUsed, tempEnvBlock, i);
			blockSizeUsed += i;
			envPtr += i;

		} while (!foundEnd);

#  undef ENV_BLOCK_SIZE_CHARS

		map<wstring,wstring> wenv = envBlock2Map(envBlock.get());

		// now gotta narrow it all to normal strings...
		for (map<wstring,wstring>::const_iterator iter = wenv.begin();
			iter != wenv.end();
			++iter)
				(*env)[WindowsCommon::UnicodeToAsciiString(iter->first)]
				=
				WindowsCommon::UnicodeToAsciiString(iter->second);

		return PROC_OK;

	}

	void appendToMemoryBlock(ArrayGuard<wchar_t> &block, 
		size_t blockSizeChars, const wchar_t *data, size_t lenChars) {

		ArrayGuard<wchar_t> newBlock(new wchar_t[blockSizeChars + lenChars]);
		// just in case the old block was empty...
		if (block.get() && blockSizeChars > 0)
			memcpy(newBlock.get(), block.get(), blockSizeChars * sizeof(wchar_t));
		memcpy(newBlock.get() + blockSizeChars, data, lenChars * sizeof(wchar_t));

		block.reset(newBlock.release());
	}

#endif

}

