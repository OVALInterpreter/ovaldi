//
//
//****************************************************************************************//
// Copyright (c) 2002-2012, The MITRE Corporation
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
#ifndef XINETDPROBE_H
#define XINETDPROBE_H

#include <AbsProbe.h>
#include <Common.h>
#include <Item.h>
#include <map>
#include <Object.h>
#include <ObjectEntity.h>
#include <REGEX.h>
#include <set>
#include <string>

class XinetdParam;

// You will probably notice that there is no 'using namespace std' at the
// top of this file, and I spelled out the namespace for all the stl types
// I used below.  It seems to me that people wouldn't want headers
// importing types from other namespaces and possibly clashing with their
// own types.  So to me, 'using namespace ...' seems more appropriate for
// an impl file, not a header.

/**
 * The probe for analyzing xinetd config files.
 * <p>
 * It is written in a quick and dirty style using regexes.  This means that
 * malformed config files will not be handled well.  Either the regexes
 * will just not match, or will match in funny ways, giving funny results.
 * It makes sense perhaps, that this tool should not be used for validation,
 * so it should assume valid config files.  But be aware of its limitations.
 * <p>
 * Also, to make this probe more tractable for the time I had to implement it,
 * the 'xinetd' app is not mimicked perfectly.  E.g. there are rules about which
 * params support which operators, which are cumulative and which aren't, etc.
 * There is special code for handling all the special network address formats.
 * This probe discards all that complexity, handling all parameters uniformly.
 * In particular, no parameter is cumulative, all params support all operators,
 * all params may have multiple values, and all param values are treated as 
 * simple strings.
 */
class XinetdProbe : public AbsProbe {

	public:

	virtual ~XinetdProbe();
	static XinetdProbe* Instance();

	protected:

	virtual ItemVector* CollectItems(Object* object);
	virtual Item* CreateItem();

	private:

	/** Type representing a service configuration entry. */
	typedef std::map<std::string, std::vector<XinetdParam> > ServiceEntryMap;

	XinetdProbe();

	/**
	 * Processes the given xinetd config file.  Information about
	 * services contained therein are stored in the 'services' param.
	 * The defaults section is stored separately, in the given
	 * 'defaults' param.
	 */
	void ProcessConfigFile(const std::string& confFileName, 
						   StringVector& includeStack, 
						   std::vector<ServiceEntryMap>& services, 
						   ServiceEntryMap& defaults,
						   ObjectEntity* nameEntity,
						   ObjectEntity* protocolEntity);

	/**
	 * Processes the given directory of config files.  Information
	 * from all config files is collected into the services and
	 * defaults parameters.  Only regular files, those without a
	 * '.' in their name, and those which don't end with '~' are
	 * processed.  If there is more than one default section found,
	 * all defaults are collected together into one big group.
	 */
	void ProcessIncludeDir(const std::string& includeDir,
						   StringVector& includeStack,
						   std::vector<ServiceEntryMap>& services,
						   ServiceEntryMap& defaults,
						   ObjectEntity* nameEntity,
						   ObjectEntity* protocolEntity);

	/**
	 * Given a chunk of text containing the innards of a service description
	 * (the stuff between '{' and '}'), finds all the params and values and
	 * puts them into the given map.  The values of duplicate parameters are
	 * merged together into a single parameter.
	 */
	void ProcessEntryParams(const std::string& entryParams, ServiceEntryMap& entryMap);

	/**
	 * Combines duplicate default parameter definitions into a single parameter
	 * definition.  This is doable in the defaults entry, without looking at any
	 * other entries.  The resulting parameters will be suitable for merging into
	 * all the actual service entries.
	 */
	void NormalizeDefaults(ServiceEntryMap& defaults);

	/**
	 * Merges values from the given defaults into the given service.
	 */
	void MergeDefaultsIntoService(ServiceEntryMap& service, const ServiceEntryMap& defaults);

	/**
	 * Given a parameter definition, and some pre-existing values for it, merge all sets
	 * of values together such that the given param will only have one set of values,
	 * with an operator of XinetdParam::ASSIGN.  All the entries in the given vector
	 * are removed and replaced with a single entry with the combined values.  All
	 * elements of \c param should be for the same service parameter.
	 */
	void MergeParamValues(std::vector<XinetdParam>& param, const StringVector& preexistingVals = StringVector());

	/**
	 * xinetd itself fills in some service params according to other param values
	 * and info queried from the OS.  This method mimics that behavior, filling
	 * out some other params (where possible) that are missing in the service.
	 * <p>
	 * This method assumes the service has already been normalized (so one XinetdParam
	 * object per param).
	 */
	void FillInOtherParams(ServiceEntryMap& service);

	/** 
	 * Constructs an oval item from the given service.  If an item could not
	 * be created because either required service params were missing or did not
	 * match the given object entities, NULL is returned.
	 */
	Item* Service2Item(const ServiceEntryMap& service,
					   ObjectEntity* nameEntity,
					   ObjectEntity* protocolEntity);

	/**
	 * Determines whether the given service is enabled.  The rules regarding whether
	 * or not a service is enabled are complicated.  The xinetd.conf man page does
	 * not describe this well.
	 * <p>
	 * An individual service entry may have a "disable" parameter.  The defaults
	 * section may have either or both of "disabled" and "enabled".  Additionally,
	 * the man page says there is a "DISABLE" flag, but from inspecting the source,
	 * no such flag seems to exist.  In the defaults section, "enabled" takes
	 * precendence over "disabled".  The man page says that the "disable" param
	 * for a service takes precendence over an "enabled" param in the defaults.
	 * It does not say whether "disable" in a service takes precedence over
	 * "disabled" in the defaults.  I will assume generally that disabling takes
	 * precedence over enabling.  So even though "disable = yes" in a service entry
	 * overrides "enabled" in the defaults, I will not let "disable = no" in a
	 * service entry override "disabled" in the defaults.
	 */
	bool ResolveEnabled(const ServiceEntryMap& service);

	/** 
	 * Adds an entity to the given item with the given name, from the service param
	 * of the same name from the given service. 
	 */
	void AddItemEntity(Item *item, 
					   const ServiceEntryMap& service, 
					   const std::string& paramName,
					   OvalEnum::Datatype dataType=OvalEnum::DATATYPE_STRING);

	/** 
	 * Adds an entity to the given item with the given name, from the service param
	 * of the given name from the given service.  In this one, the entity is named
	 * differently from the service param name.
	 */
	void AddItemEntity(Item *item, 
					   const ServiceEntryMap& service, 
					   const std::string& paramName,
					   const std::string& entityName,
					   OvalEnum::Datatype dataType=OvalEnum::DATATYPE_STRING);

	/**
	 * Reads the contents of the given file into the given string.
	 */
	void ReadFileToString(const std::string& fileName, std::string &contents);

	/**
	 * \e Modifies the given filename by transforming it into canonical
     * form (resolving symlinks, making it absolute, normalizing . and .., etc.)
	 * This allows us to detect circular includes even if the same file
	 * is referred to via different paths.
	 */
	void CanonicalizeFileName(std::string& fileName);

	REGEX re;
	std::set<std::string> allowedFlags;
	std::set<std::string> allowedTypes;
	std::set<std::string> allowedSocketTypes;

	static XinetdProbe* instance;
};

/**
 * Represents a single parameter definition in a service entry.  The
 * entry has a name, an operator, and one or more values.  The name is
 * not stored in this class because it is used as a map key.  Since
 * name is stored as a map key, I didn't think it necessary to store it
 * redundantly here.  Multiple XinetdParam objects may be stored for a
 * given parameter.  This is because it is legal for some parameters to
 * be defined multiple times in an xinetd config file.  In this case,
 * the values are combined according to the operators for each definition.
 */
class XinetdParam {
	public:

	enum Op {
		ASSIGN,
		APPEND,
		REMOVE
	};

	XinetdParam(); //this ctor required when creating new map entries, e.g. map["foo"] = someParam;
	XinetdParam(const XinetdParam& param);
	XinetdParam(Op op, const std::string& paramValue);
	XinetdParam(const std::string& opStr, const StringVector& paramValues);
	XinetdParam(Op op, const StringVector& paramValues);

	/** Coverts the given string to an Op constant. */
	Op string2Op(const std::string& opStr) const;

	/**
	 * Makes a string representation of the param.  This will include all
	 * values, delimited by spaces.
	 */
	std::string str() const;

	/** 
	 * This is used for sorting the params by operator.  When applying operators
	 * to obtain a final list of values, it's important to apply them in the proper
	 * order.
	 */
	bool operator<(const XinetdParam& other) const;

	/** cause when you create a copy ctor, you're sposed to override =... right? */
	XinetdParam& operator=(const XinetdParam& other);

	/** Looks for the given value for this param, returns true if found, false if not. */
	bool IncludesValue(const std::string& value) const;

	StringVector paramValues;
	Op op;
};

#endif
