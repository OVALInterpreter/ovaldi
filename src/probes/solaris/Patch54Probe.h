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

#ifndef PATCH54PROBE_H
#define PATCH54PROBE_H

#include <list>
#include <map>
#include <ostream>
#include <string>
#include <utility>

#include "AbsProbe.h"
#include "Item.h"
#include "Object.h"

/**
 * Represents a patch, with an integer base and version value.
 */
struct Patch {
	int base;
	int version;

	Patch(int base, int version)
		: base(base), version(version)
	{}

	bool operator==(const Patch &p) const {
		return base == p.base && version == p.version;
	}

	bool operator!=(const Patch &p) const {
		return !(*this == p);
	}

	/**
	 * NOT suitable for use as a general comparator for patches!
	 * This only works for patches with the same base.
	 */
	bool operator<(const Patch &p) const {
		return version < p.version;
	}

	/**
	 * Makes a string representation of this patch in the
	 * standard format: xxxxxx-xx 
	 */
	std::string str() const;
};

/**
 * Allows writing patches to output streams.  This uses Patch::str()
 * to create the string representation.
 */
std::ostream &operator<<(std::ostream &out, const Patch &p);

/**
 * Probe for getting solaris patch information.
 */
class Patch54Probe : public AbsProbe {
	public:
	virtual ~Patch54Probe();
	
	virtual ItemVector* CollectItems(Object* object);
		
	/** Ensure that the Patch54Probe is a singleton. */
	static AbsProbe* Instance();

	private:
	Patch54Probe();

	/** Return a new Item created for storing patch information */
	virtual Item* CreateItem();

	/** Create the cached patch structures. */
	void CachePatches();

	/** Extracts patch info out of the given 'pkginfo' file. */
	void ProcessPkgInfoFile(const std::string &fileName);

	/**
	 * Gets the list of explicitly superseding patches for the given
	 * patch.  If the given patch was not found, it is added, and its
	 * corresponding empty list returned.
	 */
	std::list<Patch>& GetExplicitSupListForPatch(const Patch &patch);

	/**
	 * Gets all patches, appending them to the given list.
	 * \param patches[out] the list to append the patches to.
	 */
	void GetAllPatches(std::list<Patch> &patches);

	/**
	 * Finds patches which directly supersede \p patch implicitly or
	 * explicitly, and adds each one to \p newPatches which does not
	 * already exist in either \p newPatches or \p oldPatches.
	 * \p oldPatches is not modified.  \p addedPatches effectively
	 * builds up into a delta of all new superseding patches.
	 */
	void GetSupersedingPatches(const std::list<Patch> &oldPatches,
							   std::list<Patch> &newPatches, const Patch &patch);

	/**
	 * Adds the given patch to \p newPatches if neither list already
	 * contains it.  Does a slow linear search through the lists... cause
	 * the STL doesn't have a hashtable implementation.... sigh.
	 */
	void AddIfNotDupe(const std::list<Patch> &oldPatches, 
					  std::list<Patch> &newPatches, const Patch &patch);

	typedef std::map<int, std::list<Patch> > PatchMapByBase;
	typedef std::list<std::pair<Patch, std::list<Patch> > > ExplicitSupersedencePatchList;

	/**
	 * Maps patch base values to a list of patches with that base value,
	 * sorted by version.  This makes searches by base+version or just
	 * by base, faster.  This also allows for faster computation of
	 * implicit supersedence.  This will contain all patches found on the
	 * system, even those in the Obsoletes section of a pkginfo file.
	 */
	PatchMapByBase patchMapByBase;

	/**
	 * A list of patches, each paired with all patches which explicitly
	 * supersede it.
	 */
	ExplicitSupersedencePatchList explicitSupersedencePatchList;

	static Patch54Probe *instance;
};

#endif
