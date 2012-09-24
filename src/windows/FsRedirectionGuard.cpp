#include <Log.h>
#include <WindowsCommon.h>
#include <Windows.h>

#include "FsRedirectionGuard.h"

namespace {

	// The wow64 disable/revert functions don't exist on XP, so
	// we can't just unconditionally call them.  Look to see if
	// they exist, and store the function pointers for reuse, if 
	// we can find them.
	typedef BOOL (WINAPI *Wow64DisableFuncType) (PVOID *OldValue);
	typedef BOOL (WINAPI *Wow64RevertFuncType) (PVOID OldValue);
	Wow64DisableFuncType disableFunc;
	Wow64RevertFuncType revertFunc;

	/**
	 * Keeps track of whether we've searched for the wow64
	 * filesystem redirect disable/revert functions, and what
	 * the determination was, so we don't keep searching for
	 * them.  We treat the two functions as a pair: if one
	 * doesn't exist, neither exists, and vice versa.  So
	 * we only need this one flag.
	 */
	enum {
		UNKNOWN,
		DONT_EXIST,
		DO_EXIST
	} doWow64FuncsExist = UNKNOWN;
}

#ifdef _WIN64
// disable 4100 which is "unreferenced formal parameter".
// This is ok in a 64-bit app, since the body of the constructor
// below is intentionally blank: The built-in filesystem redirection 
// functions don't work in a 64-bit app, and I'm not yet supporting 
// simulating WOW64 in a 64-bit app that wants to query a 32-bit
// filesystem view.  So far, 64-bit apps only can query the 64-bit
// view, so there is nothing to do here.  I think I'll even macro
// out use of this guard so that it's not even used in a 64-bit app.
// But I'll leave this class in and compile it either way (with
// blanked out ctor/dtor bodies just in case someone tries to use it
// where it is not necessary), 'cause it makes the project simpler
// (won't need to have different sets of files in x64 and Win32 
// configs).  So I want to quiet that warning.
#pragma warning(push)
#pragma warning(disable:4100)
#endif

FsRedirectionGuard::FsRedirectionGuard(BitnessView view) : wasDisabled(false) {
#ifndef _WIN64

	switch (doWow64FuncsExist) {
	case DONT_EXIST:
		// if they don't exist, assume we aren't on a 64-bit platform
		// which needs them, which means no redirection is necessary.
		// So this ctor has nothing to do.
		return;
	case DO_EXIST:
		break;
	default: // UNKNOWN
		disableFunc = (Wow64DisableFuncType)GetProcAddress(
				GetModuleHandleA("kernel32"),
				"Wow64DisableWow64FsRedirection");
		revertFunc = (Wow64RevertFuncType)GetProcAddress(
				GetModuleHandleA("kernel32"),
				"Wow64RevertWow64FsRedirection");

		if (!disableFunc || !revertFunc) {
			// should I check the OS bitness here? e.g. call 
			// WindowsCommon::Is64BitOS()?  Surely if this was
			// a 64-bit OS, then the disable/revert functions
			// would be there...
			Log::Debug("Wow64 filesystem redirection disable/revert functions not found.  "
				"Not disabling filesystem redirection.");
			doWow64FuncsExist = DONT_EXIST;
			return;
		}

		doWow64FuncsExist = DO_EXIST;

		break;
	}

	if (view == BIT_64) {
		if (WindowsCommon::Is64BitOS()) {
			if (disableFunc(&state))
				wasDisabled = true;
			else
				throw Exception("Couldn't disable WOW64 filesystem redirection: " +
					WindowsCommon::GetErrorMessage(GetLastError()));
		}
	}
#endif
}

#ifdef _WIN64
#pragma warning(pop)
#endif

FsRedirectionGuard::~FsRedirectionGuard() {
#ifndef _WIN64
	if (wasDisabled)
		if (!revertFunc(state))
			Log::Message("Warning: could not revert WOW64 filesystem "
				"redirection: " + WindowsCommon::GetErrorMessage(GetLastError()));
#endif
}
