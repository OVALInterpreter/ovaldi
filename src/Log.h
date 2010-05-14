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

#ifndef LOG_H
#define LOG_H

#include "Common.h"
#include <fstream>
#include <string>
#include <stdio.h>
#include <iostream>

/** 
	This class provides logging for the application.

	In general each log message is associated with a level. The logging
	system can be configured to write messages at or above a specified level.
	For example, if the log system is configured such that the current Logg::level
	is Log::INFO then Log::INFO level messages and above will be displayed. This means
	that Log::MESSAGE and Log::FATAL will be displayed, but not Log::DEBUG.
*/
class Log {
public:

	/** Initializes the Logger for the application.

		The logging system can only be initialized once.
		This function clears the existing log file.

		@param level Sets the log level to be used by the logging system.
		@param logFile Specifies the log file to be used.
		@param toScreen when true the log messages will be written to std out.
		@throws Exception Thrown when the existing log file can not be cleared.
	*/
	static void Init(int level = DEBUG, std::string logFile = "", bool toScreen = false);

	/** Shutdown the logger. Simply has to close the log file. */
	static void Shutdown();

	/** Write the specified message at Log::DEBUG level.

		@param msg The message to write
		@param fileOnly When true log to the file only, not to std out.
		@throws Exception Thrown when the logger has not been initialized.
	*/
	static void Debug(std::string msg, bool fileOnly = false);

	/** Write the specified message at Log::INFO level.

		@param msg The message to write
		@throws Exception Thrown when the logger has not been initialized.
	*/
	static void Info(std::string msg);
	
	/** Write the specified message at Log::MESSAGE level.

		@param msg The message to write
		@throws Exception Thrown when the logger has not been initialized.
	*/
	static void Message(std::string msg);
	
	/** Write the specified message at Log::FATAL level.

		@param msg The message to write
		@throws Exception Thrown when the logger has not been initialized.
	*/
	static void Fatal(std::string msg);

	static int  GetLevel();		
	static void SetLevel(std::string strLevel);
	static void SetLevel(int level);
	static void SetToScreen(bool screen);
	static bool GetToScreen();

	/** Return true if the Logger the current level is less than or equal to DEBUG. */
	static bool IsDebug();
	
	/** Return true if the Logger the current level is less than or equal to INFO. */
	static bool IsInfo();
	
	/** Return true if the Logger the current level is less than or equal to MESSAGE. */
	static bool IsMessage();
	
	/** Return true if the Logger the current level is less than or equal to FATAL. */
	static bool IsFatal();
	
	/** Writes an unaltered messaged to the log.
		This method can be used for ensuring that the formatting of a message
		is preserved when it is written to the log.

		@param msg The message to be written.
	*/
	static void UnalteredMessage(std::string msg);

	static const int DEBUG = 1;
	static const int INFO = 2;
	static const int MESSAGE = 3;
	static const int FATAL = 4;

	/** Return true if the Logger is configured to write to the screen. */
	static bool WriteToScreen();

private:

	/** Writes the given message to the log file.
		Messages may optionally be written to std out if
		the input fileOnly parameter is false and the Log::toScreen
		variable is set to true.

		@param logMessageIn The message to be logged.
		@param fileOnly If true write the log message only to file, not to screen.
	*/
	static void WriteLog(std::string logMessageIn, bool fileOnly = false);

	/** Convert the level to a string. */
	static std::string LevelToString(int level);

	static std::string logFilename;
	static int level;
	static bool toScreen;
	static bool initialized;
    static std::ofstream logFile;

};

#endif
