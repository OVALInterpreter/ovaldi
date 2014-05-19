
#include "CommandReader.h"
#include <sstream>

#include "Exception.h"

#define MAXLINE 1000 /* maximum input line size */

using namespace std;

CommandReader::CommandReader(std::string command) {
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
	std::string output;
	StringVector lines;

	while (fgets(line, MAXLINE, ifp) != NULL) {
		output = line;
		Common::TrimString(output);
		lines.push_back(output);
	}
	if (!feof(ifp) && ferror(ifp))
		throw Exception(string("fgets error reading from command line: ") + strerror(errno));

	return lines;
}

StringVector* CommandReader::Split(const std::string &s, char delim) {
	StringVector* elems = new StringVector();
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
		if (item.length() > 0)
        	elems->push_back(item);
    }
    return elems;
}

StringVector* CommandReader::Split(const std::string &s, const std::string &delim) {
	StringVector* elems = new StringVector();
	std::size_t start = 0;
	std::size_t end = s.find(delim);
	string item;
	while (end != std::string::npos) {
		item = s.substr(start, end - start);
			if (item.length() > 0)
			elems->push_back(item);
	    start = end + delim.length();
	    end = s.find(delim, start);
	}
	item = s.substr(start, end);
	if (item.length() > 0)
		elems->push_back(item);
	return elems;
}

StringVector CommandReader::GetResponse(std::string command) {
	CommandReader cr(command);
	StringVector lines = cr.Read();
	return lines;
}

