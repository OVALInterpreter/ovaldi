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
#include "XinetdProbe.h"

#include <algorithm>
#include <arpa/inet.h>
#include <cerrno>
#include <cstdlib>
#include <dirent.h>
#include <fstream>
#include <Log.h>
#include <netdb.h>
#include <ObjectEntity.h>
#include <OvalEnum.h>
#include <sys/stat.h>
#include <vector>

using namespace std;

static const string XINETD_CONF_FILENAME = "/etc/xinetd.conf";

/**
 * services have names, but there is no name parameter defined in the documentation to hold it
 * (it comes not from a parameter but a different part of the syntax).  To treat it uniformly
 * w.r.t. other attributes of the service, I am creating a special param name to hold the
 * service name.
 */
static const string NAME_PARAM = "__NAME__";

XinetdProbe* XinetdProbe::instance = NULL;

XinetdProbe::XinetdProbe() {
	// we must restrict enumerated values to those the schema supports.
	//   The only check implemented at the time of this writing is for
	//   allowedypes.
	allowedFlags.insert("INTERCEPT");
	allowedFlags.insert("NORETRY");
	allowedFlags.insert("IDONLY");
	allowedFlags.insert("NAMEINARGS");
	allowedFlags.insert("NODELAY");
	allowedFlags.insert("KEEPALIVE");
	allowedFlags.insert("NOLIBWRAP");
	allowedFlags.insert("SENSOR");
	allowedFlags.insert("IPv4");
	allowedFlags.insert("IPv6");
	allowedFlags.insert("LABELED");
	allowedFlags.insert("REUSE");

	allowedTypes.insert("RPC");
	allowedTypes.insert("INTERNAL");
	allowedTypes.insert("UNLISTED");
	allowedTypes.insert("TCPMUX");
	allowedTypes.insert("TCPMUXPLUS");

	allowedSocketTypes.insert("stream");
	allowedSocketTypes.insert("dgram");
	allowedSocketTypes.insert("raw");
	allowedSocketTypes.insert("seqpacket");
}

XinetdProbe::~XinetdProbe() {
	XinetdProbe::instance = NULL;
}

XinetdProbe* XinetdProbe::Instance() {
	if (XinetdProbe::instance == NULL)
		XinetdProbe::instance = new XinetdProbe();

	return XinetdProbe::instance;
}

Item* XinetdProbe::CreateItem() {
	Item* item = new Item(0,
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#unix",
						"unix-sc",
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#unix unix-system-characteristics-schema.xsd",
						OvalEnum::STATUS_ERROR,
						"xinetd_item");

	return item;
}

ItemVector* XinetdProbe::CollectItems(Object* object) {
	ObjectEntity *nameEntity = object->GetElementByName("service_name");
	ObjectEntity *protocolEntity = object->GetElementByName("protocol");

	StringVector includeStack; // will allow us to catch circular includes
	string confFileName = XINETD_CONF_FILENAME;
	vector<ServiceEntryMap> services;
	ServiceEntryMap defaults; // will be merged into the individual services

	this->CanonicalizeFileName(confFileName);

	includeStack.push_back(confFileName);
	this->ProcessConfigFile(confFileName, includeStack, services, 
							defaults, nameEntity, protocolEntity);
	includeStack.pop_back();

	//Log::Debug("Done reading config files; now normalizing data...");

	// normalize defaults...
	this->NormalizeDefaults(defaults);

	// Merge defaults into each service to complete the description.
	// The xinetd app itself fills in more params automatically, so we
	// we must simulate that behavior to create accurate service entries.
	//   As we go, we'll also create our items.
	ItemVector *items = new ItemVector();
	for (vector<ServiceEntryMap>::iterator iter = services.begin();
		 iter != services.end();
		 ++iter) {

		//Log::Debug(string("Normalizing service: ")+(*iter)[NAME_PARAM][0].str());
		this->MergeDefaultsIntoService(*iter, defaults);
		this->FillInOtherParams(*iter);

		Item *item = this->Service2Item(*iter, nameEntity, protocolEntity);

		// will be null if the service doesn't match the object.
		if (item != NULL)
			items->push_back(item);
	}

	return items;
}

void XinetdProbe::ProcessConfigFile(const string& confFileName, 
									StringVector& includeStack, 
									vector<ServiceEntryMap>& services, 
									ServiceEntryMap& defaults,
									ObjectEntity* nameEntity,
									ObjectEntity* protocolEntity) {

	vector<StringVector> entryMatches;
	vector<StringVector> includeDirMatches;
	StringVector includeMatch;
	string confFileContents;

	this->ReadFileToString(confFileName, confFileContents);

	// Look for an "include" line.  That tells us to ignore this file and parse the
	// specified file from the beginning.  It is NOT like pasting the file into
	// the current file at the point of the include.  Would be nice if
	// GetMatchingSubstrings allowed you to pass some match options (so I
    // wouldn't need the "^|\n" stuff...)
	// This code will follow the first "include" line in the file.  Others are
	// ignored.  (And actually, this code wipes out everything read thus far,
	// whether from this fine or another file, when an 'include' line is found.
	// Is that right?)
	this->re.GetMatchingSubstrings("(?:^|\\n)\\s*include\\s+(.*?)(?:\\n|$)",
								   confFileContents.c_str(), &includeMatch);

	if (!includeMatch.empty()) {

		// We replace everything we've read so far with data from the
		// given file.
		string& includeFileName = includeMatch[0];
		Common::TrimString(includeFileName);
		this->CanonicalizeFileName(includeFileName);
		Log::Debug(string("Including '")+includeFileName+"'");

		if (find(includeStack.begin(), includeStack.end(), includeFileName) == includeStack.end()) {
			services.clear();
			defaults.clear();

			includeStack.push_back(includeFileName);
			this->ProcessConfigFile(includeFileName, includeStack, services, defaults, 
									nameEntity, protocolEntity);
			includeStack.pop_back();
			return;
		} else
			throw ProbeException(string("Found xinetd config file 'include' loop!  Start/end file is: ")+
								 includeFileName);
	}

	// Look for "includedir" lines.  We parse all files in the given directory, and
	// combine them with the data we've already read.
	this->re.GetAllMatchingSubstrings("^\\s*includedir\\s+(.*?)$", confFileContents,
									  includeDirMatches, REGEX::MULTILINE);

	for (vector<StringVector>::iterator iter = includeDirMatches.begin();
		 iter != includeDirMatches.end();
		 ++iter) {
		string dir = iter->at(1);
		Common::TrimString(dir);
		this->CanonicalizeFileName(dir);
		Log::Debug(string("Including directory ")+dir);
		this->ProcessIncludeDir(dir, includeStack, services, defaults, nameEntity, protocolEntity);
	}

	// Matches all services.  The extra newline before the close brace
	//   prevents factorized IP addresses from causing false matches (e.g.
	//   1.2.3.{4,5,6}).  It also requires the close brace to be on a new line,
	//   but xinetd insists on this anyway.  So thats how (valid) config files
	//   will look.
	this->re.GetAllMatchingSubstrings("^\\s*(?:defaults|(?:service\\s+(\\w+)))\\s*\\{(.*?)\\n\\s*\\}",
									  confFileContents, entryMatches,
									  REGEX::SINGLELINE|REGEX::MULTILINE);

	for(vector<StringVector>::iterator entryIter = entryMatches.begin();
		entryIter != entryMatches.end();
		++entryIter) {

		string& entryName =  entryIter->at(1);
		string& entryParams = entryIter->at(2);
		bool isDefaultsEntry = entryName.empty();

		Common::TrimString(entryName);

		ServiceEntryMap entryMap;
		this->ProcessEntryParams(entryParams, entryMap);

		// for service entries, add the name as a special case param.
		// also, if no 'id' param for a service, it defaults to the service name.
		if (!isDefaultsEntry) {
			
			entryMap[NAME_PARAM].push_back(XinetdParam(XinetdParam::ASSIGN, entryName));

			if (entryMap.find("id") == entryMap.end())
				entryMap["id"] = entryMap[NAME_PARAM];
		}

		if (isDefaultsEntry) {
			// This pretends the new defaults are a "service" and merges existing
			//   defaults into them.  We're probably way beyond what xinetd does,
			//   so whether we replace or merge multiple defaults entries prolly
			//   doesn't even matter :-P
			this->MergeDefaultsIntoService(entryMap, defaults);
			defaults = entryMap;
		} else
			services.push_back(entryMap);

		Log::Debug(string("Found ")+(isDefaultsEntry ? "defaults":"service") + " entry: "+entryName);
	}
}

void XinetdProbe::ProcessIncludeDir(const string& includeDir,
									StringVector& includeStack, 
									vector<ServiceEntryMap>& services, 
									ServiceEntryMap& defaults,
									ObjectEntity* nameEntity,
									ObjectEntity* protocolEntity) {

	DIR *dir;
	struct dirent *ent;
	StringVector fileNamesToProcess;

	errno = 0;
	dir = opendir(includeDir.c_str());
	if (dir==NULL)
		throw ProbeException(string("Can't open includedir directory ")+
							 includeDir+": "+strerror(errno));

	do {
		errno = 0;
		ent = readdir(dir);

		if (ent == NULL) {
			if (errno != 0) {
				closedir(dir);
				throw ProbeException(string("Error reading includedir directory ")+
									 includeDir+": "+strerror(errno));
			}
			// else, reached end of directory
		} else {

			// we will mimic the sanity checks of xinetd: process only regular files, and
			//   ignore any file whose name is 1) empty, 2) contains a period anywhere in it, 
			//   or 3) ends with a ~.

			if (strlen(ent->d_name) == 0 || strchr(ent->d_name, '.') != NULL || 
				ent->d_name[strlen(ent->d_name)-1] == '~')
				continue;

			string includedFileName = includeDir + "/" + ent->d_name;
			this->CanonicalizeFileName(includedFileName);

			struct stat st;
			errno=0;
			int result = stat(includedFileName.c_str(), &st);

			// If stat() fails, I will log a message, and try to continue...
			if (result==-1) {
				Log::Message(string("Could not stat file ")+includedFileName+": "+strerror(errno));
				continue;
			}

			if (!S_ISREG(st.st_mode))
				continue;

			fileNamesToProcess.push_back(includedFileName);
		}

	} while (ent != NULL);

	closedir(dir);

	// process all the files we found...
	for (StringVector::iterator iter = fileNamesToProcess.begin();
		 iter != fileNamesToProcess.end();
		 ++iter) {

		// prevent loops
		if (find(includeStack.begin(), includeStack.end(), *iter) != includeStack.end())
			throw ProbeException(string("Found xinetd config file 'include' loop!  Start/end file is: ")+
								 *iter);

		Log::Debug(string("Including (via includedir) ")+*iter);

		includeStack.push_back(*iter);
		this->ProcessConfigFile(*iter, includeStack, services, defaults, nameEntity, protocolEntity);
		includeStack.pop_back();
	}
}

void XinetdProbe::ProcessEntryParams(const string& entryParams, ServiceEntryMap& entryMap) {
	vector<StringVector> paramMatches;

	// This regex insists there is a value for a param.  A param definition like
	// "foo =" is ignored.
	this->re.GetAllMatchingSubstrings("^\\s*(\\w+)\\s*([+-]?=)\\s*(.+?)$", entryParams,
									  paramMatches, REGEX::MULTILINE);
	for (vector<StringVector>::iterator paramIter = paramMatches.begin();
		 paramIter != paramMatches.end();
		 ++paramIter) {

		// split the part to the right of the operator into whitespace delimited substrings.
		vector<StringVector> valMatches;
		this->re.GetAllMatchingSubstrings("\\S+", paramIter->at(3), valMatches, 0);

		StringVector vals;
		for (vector<StringVector>::iterator valIter = valMatches.begin();
			 valIter != valMatches.end();
			 ++valIter)
			vals.push_back(valIter->at(0));

		entryMap[paramIter->at(1)].push_back(XinetdParam(paramIter->at(2), vals));

		//Log::Debug(string("Found param ")+paramIter->at(1)+" "+paramIter->at(2)+" "+paramIter->at(3));
	}
}

void XinetdProbe::NormalizeDefaults(ServiceEntryMap& defaults) {

	for (ServiceEntryMap::iterator iter = defaults.begin();
		 iter != defaults.end();
		 ++iter) {

		this->MergeParamValues(iter->second);

		//Log::Debug(string("Normalized default param ") + iter->first+" to "+iter->second[0].str());
	}
}

void XinetdProbe::MergeDefaultsIntoService(ServiceEntryMap& service, const ServiceEntryMap& defaults) {

	for (ServiceEntryMap::const_iterator defIter = defaults.begin();
		 defIter != defaults.end();
		 ++defIter) {

		if (service.find(defIter->first) == service.end()) {
			// Service doesn't have these params, so just add them.
			service.insert(*defIter);
			//Log::Debug(string("Added param ")+defIter->first+" from defaults to service");
		} else {
			// Service has them, so merge in the defaults.
			//   This assumes defaults have already been normalized, so each
			//   key maps to a size 1 vector.
			this->MergeParamValues(service[defIter->first], defIter->second[0].paramValues);
			//Log::Debug(string("Normalized service param ")+defIter->first+" to "+service[defIter->first][0].str());
		}
	}
}

void XinetdProbe::MergeParamValues(vector<XinetdParam>& param, const StringVector& preexistingVals) {

	// add preexisting values first
	StringVector combinedVals;
	combinedVals.insert(combinedVals.begin(),
						preexistingVals.begin(),
						preexistingVals.end());

	// put the operators in order...
	sort(param.begin(), param.end());

	// then, apply all the operators from the parameter
	for (vector<XinetdParam>::iterator defnIter = param.begin();
		 defnIter != param.end();
		 ++defnIter) {

		switch (defnIter->op) {

		case XinetdParam::ASSIGN: // default was overridden.  This effectively clears
			combinedVals.clear(); // previous values and then adds new ones.
			// no break!

		case XinetdParam::APPEND: // append only, don't clear
			combinedVals.insert(combinedVals.end(),
								defnIter->paramValues.begin(),
								defnIter->paramValues.end());
			break;

		case XinetdParam::REMOVE: // remove the given values
			for(StringVector::iterator valIter = defnIter->paramValues.begin();
				valIter != defnIter->paramValues.end();
				++valIter)
				combinedVals.erase(remove(combinedVals.begin(),
										  combinedVals.end(),
										  *valIter),
								   combinedVals.end());

			break;
		}
	}

	param.clear();
	param.push_back(XinetdParam(XinetdParam::ASSIGN, combinedVals));
}

void XinetdProbe::FillInOtherParams(ServiceEntryMap& service) {

	// if protocol is missing and we have a socket_type, fill in
	//   protocol according to the socket type if possible.
	//     (stream => tcp, dgram => udp)
	if (service.find("socket_type") != service.end() && 
		service.find("protocol") == service.end()) {

		const string& socketType = service["socket_type"][0].paramValues[0];
		if (socketType == "stream")
			service["protocol"].push_back(XinetdParam(XinetdParam::ASSIGN, "tcp"));
		else if (socketType == "dgram")
			service["protocol"].push_back(XinetdParam(XinetdParam::ASSIGN, "udp"));
	}

	// if socket_type is missing and we have a protocol, fill in
	//   socket_type according to the protocol if possible.
	//     (tcp => stream, udp => dgram)
	if (service.find("protocol") != service.end() &&
		service.find("socket_type") == service.end()) {

		const string& protocol = service["protocol"][0].paramValues[0];
		if (protocol == "tcp")
			service["socket_type"].push_back(XinetdParam(XinetdParam::ASSIGN, "stream"));
		else if (protocol == "udp")
			service["socket_type"].push_back(XinetdParam(XinetdParam::ASSIGN, "dgram"));
	}

	// If port is missing and we have a service & protocol, try to
	//   look up the service and determine its port.  This will look
	//   up in /etc/services.  Other restrictions just copied out of
	//   the xinetd source code... If the service type is not
	//   "UNLISTED" (or one of the other special types), we expect
	//   to be able to look it up and find a port.  Otherwise I think
	//   xinetd expects a port to have been explicitly configured.
	//   We won't check for this.
	if (service.find("port") == service.end() &&
		service.find("protocol") != service.end()) {

		bool lookupPort = false;
		if (service.find("type") == service.end())
			lookupPort = true;
		else {
			const XinetdParam& types = service["type"][0];
			if (!types.IncludesValue("UNLISTED") && 
				!types.IncludesValue("TCPMUX") &&
				!types.IncludesValue("TCPMUXPLUS") &&
				!types.IncludesValue("RPC"))
				lookupPort = true;
		}

		if (lookupPort) {
			const string& serviceName = service[NAME_PARAM][0].paramValues[0];
			const string& protocol = service["protocol"][0].paramValues[0];
			struct servent *se = getservbyname(serviceName.c_str(), protocol.c_str());
			if (se != NULL)
				service["port"].push_back(XinetdParam(XinetdParam::ASSIGN, Common::ToString(ntohs(se->s_port))));

		}
	}
}

Item* XinetdProbe::Service2Item(const ServiceEntryMap& service, 
								ObjectEntity* nameEntity,
								ObjectEntity* protocolEntity) {

	ServiceEntryMap::const_iterator paramIter;

	paramIter = service.find(NAME_PARAM);
	if (paramIter == service.end()) {
		Log::Debug("Skipping service: missing name");
		return NULL;
	}
	
	ItemEntity* nameItemEntity = new ItemEntity("service_name", paramIter->second[0].str(),
												OvalEnum::DATATYPE_STRING, true);
	if (nameEntity->Analyze(nameItemEntity) != OvalEnum::RESULT_TRUE) {
		delete nameItemEntity;
		return NULL;
	}

	paramIter = service.find("protocol");
	if (paramIter == service.end()) {
		Log::Debug(string("Skipping service ")+service.find(NAME_PARAM)->second[0].str()+": missing protocol");
		delete nameItemEntity;
		return NULL;
	}

	ItemEntity* protItemEntity = new ItemEntity("protocol", paramIter->second[0].str(),
												OvalEnum::DATATYPE_STRING, true);
	if (protocolEntity->Analyze(protItemEntity) != OvalEnum::RESULT_TRUE) {
		delete nameItemEntity;
		delete protItemEntity;
		return NULL;
	}

	Item* item = this->CreateItem();

	item->AppendElement(protItemEntity);
	item->AppendElement(nameItemEntity);

	// the rest are optional
	this->AddItemEntity(item, service, "flags");
	this->AddItemEntity(item, service, "no_access");
	this->AddItemEntity(item, service, "only_from");
	this->AddItemEntity(item, service, "port", NULL, OvalEnum::DATATYPE_INTEGER);
	this->AddItemEntity(item, service, "server");
	this->AddItemEntity(item, service, "server_args", "server_arguments");
	this->AddItemEntity(item, service, "socket_type");

	// type of 'type' entity is an enumeration... must verify the value
	//	this->AddItemEntity(item, service, "type");
	paramIter = service.find("type");
	if (paramIter != service.end()) {
		// this is not multi-valued... will treat it as a single valued param
		const string& type = paramIter->second[0].paramValues[0];
		ItemEntity *typeEntity;
		if (allowedTypes.find(type) == allowedTypes.end()) {

			typeEntity = new ItemEntity("type", "", 
										OvalEnum::DATATYPE_STRING, false,
										OvalEnum::STATUS_NOT_COLLECTED);
			item->AppendMessage(new OvalMessage(string("type '")+type+
													   "' is not one of the enumerated legal values."));
		}
		else
			typeEntity = new ItemEntity("type", type);

		item->AppendElement(typeEntity);
	}

	this->AddItemEntity(item, service, "user");

	// The 'wait' param is boolean valued, but the legal
	// xinetd params differ from the legal schema values.
	// The schema wants true/false, but xinetd expects yes/no.
	// So do a special conversion here...
	
	paramIter = service.find("wait");
	string serviceVal;
	string valToUse;

	if (paramIter != service.end()) {
		serviceVal = paramIter->second[0].paramValues[0];

		if (serviceVal == "yes")
			valToUse = "true";
		else
			valToUse = "false";

		item->AppendElement(new ItemEntity("wait", valToUse, OvalEnum::DATATYPE_BOOLEAN));
	}

	// special treatment for the "disabled" entity too, since its value
	// is even more complicated to figure out...
	bool enabled = this->ResolveEnabled(service);
	if (enabled)
		valToUse= "false";
	else
		valToUse = "true";

	item->AppendElement(new ItemEntity("disabled", valToUse, OvalEnum::DATATYPE_BOOLEAN));

	item->SetStatus(OvalEnum::STATUS_EXISTS);

	return item;
}

bool XinetdProbe::ResolveEnabled(const ServiceEntryMap& service) {

	bool enabledInDefaults, enabledInService;
	string serviceName = service.find(NAME_PARAM)->second[0].paramValues[0];

	ServiceEntryMap::const_iterator iter;

	// first determine what the defaults section said.  Everything's merged together
	// in each service; luckily the defaults attr names and service attr names don't
	// conflict, so they coexist in each service.
	if ((iter = service.find("enabled")) != service.end()) {

		const XinetdParam& param = iter->second[0];
		enabledInDefaults = false;
		if (find(param.paramValues.begin(), param.paramValues.end(), serviceName) != param.paramValues.end())
			enabledInDefaults = true;

	}  else if ((iter = service.find("disabled")) != service.end()) {

		const XinetdParam& param = iter->second[0];
		enabledInDefaults = true;
		if (find(param.paramValues.begin(), param.paramValues.end(), serviceName) != param.paramValues.end())
			enabledInDefaults = false;

	} else
		enabledInDefaults = true; // if neither was specified, it's enabled.

	// now, determine what the service entry itself said.
	if ((iter = service.find("disable")) != service.end()) {

		const XinetdParam& param = iter->second[0];
		enabledInService = true;
		if (param.paramValues[0] == "yes")
			enabledInService = false;

	} else
		enabledInService = true; // if not specifically disabled, assume it is enabled.

	// resolve the two.
	if (!enabledInDefaults)
		return false;
	else if (!enabledInService)
		return false;

	return true;
}

void XinetdProbe::AddItemEntity(Item *item, 
								const ServiceEntryMap& service, 
								const std::string& paramName,
								OvalEnum::Datatype dataType) {
	this->AddItemEntity(item, service, paramName, paramName, dataType);
}

void XinetdProbe::AddItemEntity(Item *item, 
								const ServiceEntryMap& service, 
								const std::string& paramName,
								const std::string& entityName,
								OvalEnum::Datatype dataType) {

	ServiceEntryMap::const_iterator iter;
	if ((iter = service.find(paramName)) != service.end())
		item->AppendElement(new ItemEntity(entityName.c_str(), iter->second[0].str(),
										   dataType, false));
}

void XinetdProbe::ReadFileToString(const string& fileName, string &contents) {
	char buf[100];
	streamsize count = sizeof(buf);
	ifstream in(fileName.c_str());

	if (!in)
		throw ProbeException(string("Couldn't open file ")+fileName);

	while(in && count == sizeof(buf)) {
		in.read(buf, sizeof(buf));
		count = in.gcount();
		contents.append(buf, count);
	}

	// ifstream destructor should close the file
}

void XinetdProbe::CanonicalizeFileName(string& fileName) {

	// There is discussion all over the web about how portable this is.
	// Some say that in some situations, PATH_MAX can
	// give a value which is too small, resulting in potential buffer
	// overflows:
	// http://bugs.debian.org/cgi-bin/bugreport.cgi?bug=152136
	// Some say that using PATH_MAX or passing NULL as the second
	// argument to realpath() is the way to go:
	// https://www.securecoding.cert.org/confluence/display/seccode/FIO02-C.+Canonicalize+path+names+originating+from+untrusted+sources?rootCommentId=17924802#comments
	// There is a GNU C library extension named canonicalize_file_name()
	// which is supposed to be safer, but it's not a POSIX standard.
	// Passing NULL as the second argument to realpath is only supported
	// in later versions of POSIX.  I'm going to assume compliance with
	// later POSIX standards and pass NULL as the second arg to realpath(),
	// since I have to choose something, and I don't feel like implementing
	// this myself.  That seems relatively safe (hopefully most C libs
	// support later versions of POSIX by now...)

	errno = 0;
	char *canonFileName = realpath(fileName.c_str(), NULL);

	if (errno != 0) {
		// I found that on solaris, passing NULL as the second param is
		// not supported!  Arrggghhh.... ok, so if passing NULL fails,
		// I will try again, with a buffer of size PATH_MAX.  If that
		// fails, then I will throw an exception!
		int pathMaxBytes = PATH_MAX*sizeof(char);
		char *resolvedName = (char*)malloc(pathMaxBytes);
		if (resolvedName == NULL)
			throw ProbeException("Error canonicalizing xinetd config file "+fileName+": could not allocate PATH_MAX chars ("+Common::ToString(pathMaxBytes)+" bytes) for canonicalized name: "+strerror(errno));

		errno = 0;
		canonFileName = realpath(fileName.c_str(), resolvedName);
		if (errno != 0) {
			free(resolvedName);
			throw ProbeException("Error canonicalizing xinetd config file "+fileName+": "+strerror(errno));
		}
	}

	//Log::Debug("Canonicalized "+fileName+" to "+canonFileName);

	fileName = canonFileName;
	free(canonFileName);
}

/* ============================================================================= */
/* ============================ XinetdParam methods ============================ */
/* ============================================================================= */

XinetdParam::XinetdParam(){
}

XinetdParam::XinetdParam(const XinetdParam& param) :
	paramValues(param.paramValues), op(param.op) {
}

XinetdParam::XinetdParam(Op op, const string& paramValue) :
	op(op) {
	paramValues.push_back(paramValue);
}

XinetdParam::XinetdParam(const string& opStr, const vector<string>& paramValues) :
	paramValues(paramValues), op(string2Op(opStr)) {
}

XinetdParam::XinetdParam(Op op, const vector<string>& paramValues) :
	paramValues(paramValues), op(op) {
}

XinetdParam::Op XinetdParam::string2Op(const string& opStr) const {
	if (opStr == "=")
		return ASSIGN;
	if (opStr == "+=")
		return APPEND;
	if (opStr == "-=")
		return REMOVE;

	throw ProbeException(string("Unrecognized operator in xinetd conf file: ") + opStr);
}

std::string XinetdParam::str() const {
	string s;
	StringVector::const_iterator iter = paramValues.begin();

	if (iter != paramValues.end()) {		
		s += *iter;

		++iter;
		for (;
			 iter != paramValues.end();
			 ++iter)
			s += " " + *iter;
	}

	return s;
}

bool XinetdParam::operator<(const XinetdParam& other) const {
	return op < other.op;
}

XinetdParam& XinetdParam::operator=(const XinetdParam& other) {
	op = other.op;
	paramValues = other.paramValues;
	return *this;
}

bool XinetdParam::IncludesValue(const string& value) const {
	return find(paramValues.begin(), paramValues.end(), value) != paramValues.end();
}
