#ifndef COMMANDREADER_H
#define COMMANDREADER_H

#include <stdio.h>
#include <string>
#include <Log.h>
#include "Common.h"

using namespace std;

class CommandReader {

public:
	CommandReader(std::string command);

	virtual ~CommandReader();

	StringVector Read();

	static StringVector* Split(const std::string &s, char delim);
	
	static StringVector* Split(const std::string &s, const std::string &delim);
	
	static StringVector GetResponse(std::string command);

private:
	FILE *ifp;

};

#endif