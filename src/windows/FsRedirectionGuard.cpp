#include <Log.h>
#include <WindowsCommon.h>
#include <Windows.h>

#include "FsRedirectionGuard.h"

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
	if (view == BIT_64) {
		if (WindowsCommon::Is64BitOS()) {
			if (Wow64DisableWow64FsRedirection(&state))
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
		if (!Wow64RevertWow64FsRedirection(state))
			Log::Message("Warning: could not revert WOW64 filesystem "
				"redirection: " + WindowsCommon::GetErrorMessage(GetLastError()));
#endif
}
