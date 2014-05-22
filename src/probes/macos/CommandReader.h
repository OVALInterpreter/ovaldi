#ifndef COMMANDREADER_H
#define COMMANDREADER_H

#include <cstdio> // for FILE
#include <string>

#include <StdTypedefs.h>

class CommandReader {

public:
	explicit CommandReader(const std::string &command);

	~CommandReader();

	StringVector Read();

	static StringVector Split(const std::string &s, char delim);
	
	static StringVector Split(const std::string &s, const std::string &delim);
	
	static StringVector GetResponse(const std::string &command) {
		CommandReader cr(command);
		return cr.Read();
	}

private:
	FILE *ifp;

};

#endif
