//
//
//****************************************************************************************//
// Copyright (c) 2002-2013, The MITRE Corporation
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

#include "Log.h"

using namespace std;

// Initialize static variables.
int Log::level = Log::DEBUG;
bool Log::toScreen = true;
bool Log::initialized = false;
string Log::logFilename = "";
ofstream Log::logFile;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
void Log::Shutdown() {

    if(Log::logFile.is_open())
        Log::logFile.close();

	Log::initialized = false;
}

void Log::Init(int level, string logFile, bool toScreen) {

	if(!Log::initialized) {

		// init the log level
		Log::SetLevel(level);

		// init the log file
		Log::logFilename = logFile;

		// init the to screen flag
		Log::toScreen = toScreen;

		// Reset the log file
		Log::logFile.open(logFilename.c_str(), ios::trunc | ios::out);

        if(!Log::logFile.is_open() || Log::logFile.fail()) {
            Log::logFile.close();
            throw Exception("Error initializing log system. Unable to clear log file.");
        }

		Log::initialized = true;
	}
}

void Log::UnalteredMessage(string msg) {
	if(!Log::initialized)
		throw Exception("The logging system must first be initialized.");

	bool tmp = Log::toScreen;
	Log::toScreen = false;
    Log::logFile << msg << endl;
	Log::toScreen = tmp;
}

void Log::Debug(string msg, bool fileOnly) {
	if(!Log::initialized)
		throw Exception("The logging system must first be initialized.");

	if(Log::IsDebug()) {
		msg = Common::GetTimeStamp() + " : DEBUG : " + msg;
		Log::WriteLog(msg, fileOnly);
	}
}

void Log::Info(string msg) {
	if(!Log::initialized)
		throw Exception("The logging system must first be initialized.");

	if(Log::IsInfo()) {
		msg = Common::GetTimeStamp() + " : INFO : " + msg;
		Log::WriteLog(msg);
	}
}

void Log::Message(string msg) {
	if(!Log::initialized)
		throw Exception("The logging system must first be initialized.");

	if(Log::IsMessage()) {
		msg = Common::GetTimeStamp() + " : MESSAGE : " + msg;
		Log::WriteLog(msg);
	}
}

void Log::Fatal(string msg) {
	if(!Log::initialized)
		throw Exception("The logging system must first be initialized.");

	if(Log::IsFatal()) {
		msg = Common::GetTimeStamp() + " : FATAL : " + msg;
		Log::WriteLog(msg);
	}
}

void Log::SetLevel(string strLevel) {
	
	int tmpLevel = atoi(strLevel.c_str());
	if(tmpLevel > Log::FATAL || tmpLevel < Log::DEBUG) {
		string msg = "Error setting Log level. A log level between " + Common::ToString((int)Log::FATAL) + " and " + Common::ToString((int)Log::DEBUG) + " must be specified. Setting Log level to " + Common::ToString((int)Log::DEBUG) + ". Note that this message has not been written to the log file.";
		cout << Common::GetTimeStamp() + " : MESSAGE : " << msg << endl;
		Log::SetLevel(Log::DEBUG);
	} else {
		Log::SetLevel(tmpLevel);
	}
}

void Log::SetLevel(int level) {
	Log::level = level;
}

int Log::GetLevel() {
	return Log::level;
}

void Log::SetToScreen(bool screen) {
	Log::toScreen = screen;
}

bool Log::GetToScreen(){
	return Log::toScreen;
}

bool Log::IsDebug() {
	return (Log::GetLevel() <= Log::DEBUG);
}

bool Log::IsInfo() {
	return (Log::GetLevel() <= Log::INFO);
}

bool Log::IsMessage() {
	return (Log::GetLevel() <= Log::MESSAGE);
}

bool Log::IsFatal() {
	return (Log::GetLevel() <= Log::FATAL);
}

bool Log::WriteToScreen() {
	return Log::toScreen;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
void Log::WriteLog(string logMessageIn, bool fileOnly) {

    Log::logFile << logMessageIn << endl;
    Log::logFile.flush();

	if(Log::toScreen && !fileOnly) {
		cout << logMessageIn << endl;
	}
}

string Log::LevelToString(int level) {
	string levelStr = "";
	
	switch(level) {
		case Log::FATAL:
			levelStr = "FATAL";
			break;
		case Log::INFO:
			levelStr = "INFO";
			break;
		case Log::MESSAGE:
			levelStr = "MESSAGE";
			break;
		case Log::DEBUG:
			levelStr = "DEBUG";
			break;
	}

	return levelStr;
}
