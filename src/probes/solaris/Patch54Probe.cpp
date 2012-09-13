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

#include <algorithm>
#include <cerrno>
#include <cstring>
#include <iomanip>
#include <iterator>
#include <sstream>
#include <dirent.h>
#include <sys/stat.h>

#include <DirGuard.h>
#include "ItemEntity.h"
#include "OvalEnum.h"
#include "Patch54Probe.h"

using namespace std;

#define PKG_ROOT_DIR "/var/sadm/pkg"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Patch54Probe Class  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
Patch54Probe* Patch54Probe::instance = NULL;

Patch54Probe::Patch54Probe() {
	this->CachePatches();

	/* for debugging...
	ostringstream oss;
	list<Patch>::size_type n = 0;
	for (PatchMapByBase::iterator baseIter = patchMapByBase.begin();
		 baseIter != patchMapByBase.end();
		 ++baseIter) {
		oss << "Base patch ID: " << baseIter->first;
		oss << " Versions: ";
		copy(baseIter->second.begin(), baseIter->second.end(), ostream_iterator<Patch>(oss, " "));
		oss << endl;
		n += baseIter->second.size();
	}
	oss << "Total unique patches found: " << n << endl;
	oss << "Total unique bases found: " << patchMapByBase.size();

	Log::Info(oss.str());
	*/
}

Patch54Probe::~Patch54Probe() {
  instance = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* Patch54Probe::Instance() {

	// Use lazy initialization
	if(instance == NULL) 
		instance = new Patch54Probe();

	return instance;	
}

ItemVector* Patch54Probe::CollectItems(Object *object) {

	ItemVector *collectedItems = NULL;
	ObjectEntity *baseObjEntity = object->GetElementByName("base");
	ObjectEntity *versionObjEntity = object->GetElementByName("version");
	BehaviorVector *behaviors = object->GetBehaviors();
	string supersedenceStr;
	bool supersedence = false;

	supersedenceStr = Behavior::GetBehaviorValue(behaviors, "supersedence");
	supersedence = supersedenceStr=="true";

	StringVector baseStringValues;
	/* OvalEnum::Flag ignore = */ baseObjEntity->GetEntityValues(baseStringValues);
	list<Patch> patches;

	switch(baseObjEntity->GetOperation()) {
	case OvalEnum::OPERATION_EQUALS:
		for (StringVector::iterator baseIter = baseStringValues.begin();
			 baseIter != baseStringValues.end();
			 ++baseIter) {

			int base = atoi(baseIter->c_str());
			if (!base)
				throw ProbeException("Invalid patch base value: "+*baseIter);

			if (patchMapByBase.find(base) != patchMapByBase.end())
				patches = this->patchMapByBase[base];
		}
		break;

	default:
		// gets all the patches and versions
		this->GetAllPatches(patches);
		break;
	}

	//Log::Debug("patches.size() == "+Common::ToString(patches.size())+" after initial search");

	// refine the list: weed out patches which don't match the object
	ItemEntity *baseItemEntity = this->CreateItemEntity(baseObjEntity);
	ItemEntity *versionItemEntity = this->CreateItemEntity(versionObjEntity);

	for (list<Patch>::iterator patchIter = patches.begin();
		 patchIter != patches.end(); ) {

		baseItemEntity->SetValue(Common::ToString(patchIter->base));
		versionItemEntity->SetValue(Common::ToString(patchIter->version));
		if (baseObjEntity->Analyze(baseItemEntity) == OvalEnum::RESULT_TRUE &&
			versionObjEntity->Analyze(versionItemEntity) == OvalEnum::RESULT_TRUE)
			++patchIter;
		else
			patchIter = patches.erase(patchIter);
	}

	//Log::Debug("patches.size() == "+Common::ToString(patches.size())+" after weeding");

	// If supersedence behavior is selected, go back through and add
	// all patches which supersede previously matched patches.  We will
	// do this repeatedly to make sure we get a transitive closure.  E.g.
	// an implicitly superseding patch may itself have explicitly
	// superseding patches, and vice versa.  We will continue to iterate
	// until we can't find any superseding patches that have not already
	// been added.
	if (supersedence) {

		// Used for an optimization, to prevent checking the same patches
		// over and over.  Only check the newly added ones.
		list<Patch> newPatches;

		// Gets the initial superseding patch set
		for (list<Patch>::iterator patchIter = patches.begin();
			 patchIter != patches.end();
			 ++patchIter)
			this->GetSupersedingPatches(patches, newPatches, *patchIter);

		// Iterate until the size stops increasing...
		do {
			patches.insert(patches.end(), newPatches.begin(), newPatches.end());
			list<Patch> tmpNewPatches(newPatches);
			newPatches.clear();

			// this is the optimization... we iterate only over the ones added
			// in the previous pass.
			for (list<Patch>::iterator patchIter = tmpNewPatches.begin();
				 patchIter != tmpNewPatches.end();
				 ++patchIter)
				this->GetSupersedingPatches(patches, newPatches, *patchIter);

		} while (!newPatches.empty());
	}

	//Log::Debug("patches.size() == "+Common::ToString(patches.size())+" after supersedence");

	// Now convert our patches to items.
	collectedItems = new ItemVector;
	for (list<Patch>::iterator patchIter = patches.begin();
		 patchIter != patches.end();
		 ++patchIter) {

		Item *item = this->CreateItem();
		item->SetStatus(OvalEnum::STATUS_EXISTS);
		item->AppendElement(new ItemEntity("base", Common::ToString(patchIter->base),
										   OvalEnum::DATATYPE_INTEGER));
		item->AppendElement(new ItemEntity("version", Common::ToString(patchIter->version),
										   OvalEnum::DATATYPE_INTEGER));
		collectedItems->push_back(item);
	}

	return collectedItems;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
Item* Patch54Probe::CreateItem() {

	Item* item = new Item(0, 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#solaris", 
						"sol-sc", 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#solaris solaris-system-characteristics-schema.xsd", 
						OvalEnum::STATUS_ERROR, 
						"patch_item");

	return item;
}

void Patch54Probe::CachePatches() {
	DirGuard d(PKG_ROOT_DIR);
	dirent *dent;
	struct stat st;
	
	errno = 0;
	while ((dent = readdir(d))) {
		if (!strcmp(dent->d_name, ".") || !strcmp(dent->d_name, ".."))
			continue;
		
		string patchPath = Common::BuildFilePath(PKG_ROOT_DIR, dent->d_name);
		if (stat(patchPath.c_str(), &st) < 0)
			throw ProbeException("Couldn't stat "+patchPath+": "+strerror(errno));

		if (!S_ISDIR(st.st_mode))
			continue;

		string pkginfoPath = Common::BuildFilePath(patchPath, "pkginfo");
		if (stat(pkginfoPath.c_str(), &st) < 0) {
			if (errno == ENOENT) {
				errno = 0;
				continue; //not found, just skip
			} else
				throw ProbeException("Couldn't stat "+pkginfoPath+": "+strerror(errno));
		}

		this->ProcessPkgInfoFile(pkginfoPath);
	}

	if (errno)
		throw ProbeException(string("Error reading directory " PKG_ROOT_DIR ": ") + strerror(errno));

	// Now sort and dedupe the patches for each base in the
	// implicit supersedence map.
	for (PatchMapByBase::iterator iter = this->patchMapByBase.begin();
		 iter != this->patchMapByBase.end();
		 ++iter) {
		// ok to sort using Patch's operator< within the same base.
		iter->second.sort();
		iter->second.unique();
	}
}

void Patch54Probe::ProcessPkgInfoFile(const string &fileName) {
	ifstream in(fileName.c_str());

	if (!in)
		throw ProbeException("Can't read pkginfo file: "+fileName);

	string line;
	REGEX re;
	while(getline(in, line)) {
		if (!line.compare(0, 10, "PATCHLIST=")) {

			vector<StringVector> patchMatches;
			re.GetAllMatchingSubstrings("(\\d+)-(\\d+)", line.substr(10),
										patchMatches);

			for (vector<StringVector>::iterator matchIter = patchMatches.begin();
				 matchIter != patchMatches.end();
				 ++matchIter) {
				int base = atoi(matchIter->at(1).c_str());
				int version = atoi(matchIter->at(2).c_str());

				// I won't sort/dedupe these until the whole map is built.
				patchMapByBase[base].push_back(Patch(base,version));
			}

			break; // should be only one PATCHLIST line per file
		}
	}

	// Assumes the PATCHLIST line comes before any PATCH_INFO lines in
	// the pkginfo files.
	while(getline(in, line)) {
		if (!line.compare(0, 11, "PATCH_INFO_")) {
//			Log::Debug("Found "+line.substr(0,20));

			StringVector patchIdMatch;
			if (!re.GetMatchingSubstrings("^(\\d+)-(\\d+)", 
										  line.substr(11).c_str(), &patchIdMatch))
				continue;

			int base = atoi(patchIdMatch[0].c_str());
			int version = atoi(patchIdMatch[1].c_str());
			Patch supPatch(base, version); // superseding patch

			// Now get the "Obsoletes" part out.
			StringVector obsoletes;
			if (!re.GetMatchingSubstrings("Obsoletes:(.*?)Requires", line.c_str(), &obsoletes))
				continue;

			// Get the individual obsoleted patches
			vector<StringVector> obsPatchMatches;
			re.GetAllMatchingSubstrings("(\\d+)-(\\d+)", obsoletes[0], obsPatchMatches);
			if (obsPatchMatches.empty())
				continue;

			//Log::Debug(Common::ToString(obsPatchMatches.size())+" explicit obsoletes for "+supPatch.str());

			for (vector<StringVector>::iterator obsPatchIter = obsPatchMatches.begin();
				 obsPatchIter != obsPatchMatches.end();
				 ++obsPatchIter) {

				int obsBase = atoi(obsPatchIter->at(1).c_str());
				int obsVersion = atoi(obsPatchIter->at(2).c_str());
				Patch obsPatch(obsBase, obsVersion);

				list<Patch> &expSupersedenceList = 
					this->GetExplicitSupListForPatch(obsPatch);

				// see if the superseding patch already is in the list...
				// if not, add it.  These patches cannot be deduped after
				// the fact using the STL "unique" algorithm because it
				// requires being sorted, which is not possible with patches
				// with differing bases, as each list of explicit
				// supersedences is.  So we dedupe them here as we add
				// them.
				list<Patch>::iterator supPatchIter;
				for (supPatchIter = expSupersedenceList.begin();
					 supPatchIter != expSupersedenceList.end();
					 ++supPatchIter)
					if (*supPatchIter == supPatch)
						break;

				if (supPatchIter == expSupersedenceList.end())
					expSupersedenceList.push_back(supPatch);

				// An obsoleted patch is still an applied (albeit
				// indirectly) patch.  So add it to the implicit patch map
				// too.  We'll dedupe this later, as described above.
				this->patchMapByBase[obsPatch.base].push_back(obsPatch);
			}
		}
	}
}

list<Patch> &Patch54Probe::GetExplicitSupListForPatch(const Patch &patch) {
	for (ExplicitSupersedencePatchList::iterator iter = explicitSupersedencePatchList.begin();
		 iter != explicitSupersedencePatchList.end();
		 ++iter) {
		if (iter->first == patch)
			return iter->second;
	}

	// the patch isn't in the list yet... so we will add it
	explicitSupersedencePatchList.push_back(make_pair(patch, list<Patch>()));
	return explicitSupersedencePatchList.back().second;
}


void Patch54Probe::GetAllPatches(list<Patch> &patches) {

	// concats all patches in the implicit supersedence map
	// into one big list.
	for (PatchMapByBase::iterator baseIter = patchMapByBase.begin();
		 baseIter != patchMapByBase.end();
		 ++baseIter)
		patches.insert(patches.end(), baseIter->second.begin(), baseIter->second.end());
}

void Patch54Probe::GetSupersedingPatches(const std::list<Patch> &oldPatches,
										 std::list<Patch> &newPatches, const Patch &patch) {

	// implicitly superseding patches
	list<Patch>::iterator impIter;
	for (impIter = patchMapByBase[patch.base].begin();
		 impIter != patchMapByBase[patch.base].end();
		 ++impIter) {

		// since I have to dedupe like this, the advantage
		// gained by sorting the patches for a given base by
		// version is lost.  All implicitly superseding patches
		// must still be iterated through to check for dupes. :(
		if (impIter->version > patch.version)
			this->AddIfNotDupe(oldPatches, newPatches, *impIter);
	}

	// explicitly superseding patches
	list<Patch> &expSupPatches = this->GetExplicitSupListForPatch(patch);
	for (list<Patch>::iterator expIter = expSupPatches.begin();
		 expIter != expSupPatches.end();
		 ++expIter)
		this->AddIfNotDupe(oldPatches, newPatches, *expIter);
}

void Patch54Probe::AddIfNotDupe(const list<Patch> &oldPatches,
								list<Patch> &newPatches, const Patch &patch) {
	list<Patch>::const_iterator tmpIter;

	for (tmpIter = oldPatches.begin(); tmpIter != oldPatches.end(); ++tmpIter)
		if (*tmpIter == patch)
			break;

	if (tmpIter != oldPatches.end())
		return;

	for (tmpIter = newPatches.begin(); tmpIter != newPatches.end(); ++tmpIter)
		if (*tmpIter == patch)
			break;

	if (tmpIter != newPatches.end())
		return;

	newPatches.push_back(patch);
}

string Patch::str() const {
	ostringstream oss;
	oss << setfill('0');
	oss << setw(6) << base << '-' << setw(2) << version;
	return oss.str();
}

std::ostream &operator<<(std::ostream &out, const Patch &p) {
	out << p.str();
	return out;
}
