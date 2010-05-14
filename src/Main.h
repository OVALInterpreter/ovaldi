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

#ifndef MAIN_H
#define MAIN_H

#ifdef WIN32
	#pragma warning(disable:4786)
	#include "WindowsCommon.h"
	#include <windows.h>
#endif

#ifdef LINUX
	#define STRNICMP strnicmp
#elif defined SUNOS
#  define STRNICMP strncasecmp
#endif


#define EXIT_SUCCESS	0
#define	EXIT_FAILURE	1


//	other includes
#include <time.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>


#include "XmlProcessor.h"
#include "AbsDataCollector.h"
#include "Version.h"
#include "Analyzer.h"
#include "DocumentManager.h"
#include "DataCollector.h"
#include "XslCommon.h"
#include "EntityComparator.h"
#include "OvalEnum.h"


#define BUFFER_SIZE 4096

/** The starting point for the application. */
int main(int argc, char* argv[]);

/** 
 *  Processes the commandline arguments and enforces required arguments. 
 *  There must be at least two arguments.  The program name and the xmlfile hash. (or
 *  the -m flag signifing no hash is required)
 */
void ProcessCommandLine(int argc, char* argv[]);

/** Prints out a list of option flags that can be used with this exe. */
void Usage();

#endif
