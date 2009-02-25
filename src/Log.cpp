//
//
//****************************************************************************************//
// Copyright (c) 2002-2009, The MITRE Corporation
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

// Initialize static variables.
int Log::level = Log::DEBUG;
bool Log::toScreen = true;
bool Log::initialized = false;
string Log::logFilename = "oval.log";
FILE* Log::fp = NULL;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
void Log::Shutdown() {

	if(fp != NULL) {
		fclose(fp);
	}
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
		Log::fp = NULL;
		Log::fp = fopen(logFilename.c_str(), "w+"); 
		if (Log::fp == NULL) {
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
	fputs(msg.c_str(), Log::fp);
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
		Log::Message("Error setting Log level. A log level between " + Common::ToString(Log::FATAL) + " and " + Common::ToString(Log::DEBUG) + " must be specified. Setting Log level to " + Common::ToString(Log::DEBUG) + ".");
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

bool Log::IsDebug() {
	bool enabled = false;
	if(Log::GetLevel() <= Log::DEBUG)
		enabled = true;
	return enabled;
}

bool Log::IsInfo() {
	bool enabled = false;
	if(Log::GetLevel() <= Log::INFO)
		enabled = true;
	return enabled;
}

bool Log::IsMessage() {
	bool enabled = false;
	if(Log::GetLevel() <= Log::MESSAGE)
		enabled = true;
	return enabled;
}

bool Log::IsFatal() {
	bool enabled = false;
	if(Log::GetLevel() <= Log::FATAL)
		enabled = true;
	return enabled;
}

bool Log::WriteToScreen() {
	return Log::toScreen;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
void Log::WriteLog(string logMessageIn, bool fileOnly) {

	string tmp = logMessageIn + "\n";
	fputs(tmp.c_str(), fp);

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
