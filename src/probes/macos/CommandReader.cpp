#include <cstdio>
#include <sstream>

#include <Log.h>
#include <Common.h>
#include <Exception.h>

#include "CommandReader.h"

#define MAXLINE 1000 /* maximum input line size */

using namespace std;

CommandReader::CommandReader(const string &command) {
	if ((ifp = popen(command.c_str(), "r")) == NULL) {
		throw Exception(string("While calling command: ") + strerror(errno));
	}
}

CommandReader::~CommandReader() {
	if (pclose(ifp) == -1) {
		Log::Message(string("pclose error: ") + strerror(errno));
	}
}

StringVector CommandReader::Read() {
	char line[MAXLINE];
	string output;
	StringVector lines;

	while (fgets(line, MAXLINE, ifp) != NULL) {
		output = line;
		Common::TrimString(output);
		lines.push_back(output);
	}
	if (ferror(ifp))
		throw Exception(string("fgets error reading from command line: ") + strerror(errno));

	return lines;
}

StringVector CommandReader::Split(const string &s, char delim) {
	StringVector elems;
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
		if (item.length() > 0)
        	elems.push_back(item);
    }
    return elems;
}

StringVector CommandReader::Split(const string &s, const string &delim) {
	StringVector elems;
	size_t start = 0;
	size_t end = s.find(delim);
	string item;
	while (end != string::npos) {
		item = s.substr(start, end - start);
		if (!item.empty())
			elems.push_back(item);
	    start = end + delim.length();
	    end = s.find(delim, start);
	}
	item = s.substr(start);
	if (!item.empty())
		elems.push_back(item);
	return elems;
}
