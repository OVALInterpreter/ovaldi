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

#include <cctype>
#include <cerrno>
#include <cstring>
#include <memory>
#include <string>

#include <dirent.h>
#include <selinux/selinux.h>
#include <selinux/context.h>
#include <sys/types.h>
#include <unistd.h> // for getpid()

#include <Behavior.h>
#include <Common.h>
#include <FileFinder.h>
#include <REGEX.h>
#include <VectorPtrGuard.h>
#include <DirGuard.h>
#include <Noncopyable.h>
#include <SecurityContextGuard.h>
#include "SelinuxSecurityContextProbe.h"

using namespace std;

namespace {

	auto_ptr<Item> CreateItem();

	/**
	 * Searches for items by pid.
	 */
	void SearchByPid(ObjectEntity *pidObjEntity, ItemVector *items);	

	/**
	 * Given a pid, get the selinux security contexts and populate the given
	 * vector with item entities.  The pid entity is already assumed to be
	 * there, since it had to have been created to check whether the pid matches
	 * the object.  I am not adding entities directly to an item, because I
	 * don't want to add them in order.  The Item class doesn't have a way to
	 * prepend an entity or insert one in the middle.  But it does have a way to
	 * set all entities at once, so I build the entities up in a vector (which
	 * has all the methods I need), then set them all at once.
	 */
	void CompletePidItem(ItemEntityVector *itemEntities, pid_t pid);

	/**
	 * Searches the filesystem for items.
	 */
	void SearchByFiles(ObjectEntity *filepathObjEntity, ObjectEntity *pathObjEntity, 
					   ObjectEntity *filenameObjEntity, BehaviorVector *beh,
					   ItemVector *item);

	/**
	 * Gets the selinux security contexts for the given file, which is assumed
	 * to exist, create an item, and return it.  If fileName is empty, then this
	 * is interpreted as asking to get the context for the directory.
	 */
	auto_ptr<Item> CompleteFileItem(const string &dir, const string &fileName);

	/**
	 * Given the initial entities of an item that matches the object (this will
	 * include the file-oriented entities and the pid entity), and regular and
	 * raw selinux contexts, populate the rest of the entities.
	 */
	void CompleteItem(ItemEntityVector *itemEntities, context_t ctx, context_t rawCtx);

	/**
	 * Parses the given range string into its components.  The string must be in
	 * the form: aaa[:bbb][-ccc[:ddd]], where [] means optional.  Any of the
	 * latter four params corresponding to missing parts of the range are left
	 * unmodified.
	 *
	 * \throw ProbeException if the range couldn't be parsed.
	 */
	void ParseRange(const char *range, string *lowSensitivity, string *lowCategory, 
					string *highSensitivity, string *highCategory);
}

//****************************************************************************************//
//								SelinuxSecurityContext Class							  //	
//****************************************************************************************//
SelinuxSecurityContextProbe *SelinuxSecurityContextProbe::instance = NULL;

SelinuxSecurityContextProbe::SelinuxSecurityContextProbe() {
}

SelinuxSecurityContextProbe::~SelinuxSecurityContextProbe() {
	instance = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* SelinuxSecurityContextProbe::Instance() {
	// Use lazy initialization
	if(instance == NULL) 
		instance = new SelinuxSecurityContextProbe();
	
	return instance;
}

ItemVector* SelinuxSecurityContextProbe::CollectItems(Object* object) {

	VectorPtrGuard<Item> collectedItems(new ItemVector());
	ObjectEntity *filepathObjEntity = object->GetElementByName("filepath");
	ObjectEntity *pathObjEntity = object->GetElementByName("path");
	ObjectEntity *filenameObjEntity = object->GetElementByName("filename");
	ObjectEntity *pidObjEntity = object->GetElementByName("pid");

	// In v5.8 of the language when this test was introduced, behaviors were
	// erroneously omitted from the objects.  So I am creating hard-coded
	// behaviors to use, until that is fixed.
	VectorPtrGuard<Behavior> beh(new BehaviorVector());
	beh->push_back(new Behavior("recurse_direction", "none"));
	if (pidObjEntity)
		SearchByPid(pidObjEntity, collectedItems.get());
	else
		SearchByFiles(filepathObjEntity, pathObjEntity, filenameObjEntity, 
					  beh.get(), collectedItems.get());

	return collectedItems.release();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

/** This is never called */
Item* SelinuxSecurityContextProbe::CreateItem() {
	return NULL;
}

namespace {

	auto_ptr<Item> CreateItem() {
		// -----------------------------------------------------------------------
		//
		//  ABSTRACT
		//
		//  Return a new Item created for storing file information
		//
		// -----------------------------------------------------------------------

		auto_ptr<Item> item(new Item(0, 
									 "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#linux", 
									 "linux-sc", 
									 "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#linux linux-system-characteristics-schema.xsd", 
									 OvalEnum::STATUS_ERROR, 
									 "selinuxsecuritycontext_item"));

		return item;
	}

	void SearchByPid(ObjectEntity *pidObjEntity, ItemVector *items) {

		StringVector pids;

		// Create an item for current process, if the entity is nil'd, ignoring
		// everything else.
		if (pidObjEntity->GetNil()) {
			pid_t currentPid = getpid();
			VectorPtrGuard<ItemEntity> itemEntities(new ItemEntityVector());

			itemEntities->push_back(new ItemEntity("pid", Common::ToString(currentPid), 
												   OvalEnum::DATATYPE_INTEGER));
			auto_ptr<Item> item;
			try {
				CompletePidItem(itemEntities.get(), currentPid);

				item = CreateItem();
				item->SetStatus(OvalEnum::STATUS_EXISTS);
				// Some awkwardness here because the Item::SetElements
				// method causes the item to assume ownership of the
				// contents of the vector, but not the vector itself.  So we
				// gotta make sure to only delete the vector.  We pass the
				// vector contents into the item, then clear the contents
				// out of our guarded copy, so they're not deleted.
				item->SetElements(itemEntities.get());
				itemEntities->clear();
			} catch (Exception &e) {
				item = CreateItem();
				item->SetElements(itemEntities.get());
				itemEntities->clear(); // the item now owns the entities!
				item->AppendMessage(new OvalMessage(e.GetErrorMessage(), OvalEnum::LEVEL_ERROR));
			}

			items->push_back(item.release());
			return;
		}

		if (pidObjEntity->GetOperation() == OvalEnum::OPERATION_EQUALS) {
			/* OvalEnum::Flag flag = */ pidObjEntity->GetEntityValues(pids);

			for (StringVector::iterator pidIter = pids.begin();
				 pidIter != pids.end();
				 ++pidIter) {

				auto_ptr<ItemEntity> pidItemEntity(new ItemEntity("pid", *pidIter, 
																  OvalEnum::DATATYPE_INTEGER));
				if (pidObjEntity->Analyze(pidItemEntity.get()) != OvalEnum::RESULT_TRUE)
					continue;

				VectorPtrGuard<ItemEntity> itemEntities(new ItemEntityVector());
				itemEntities->push_back(pidItemEntity.release());

				// we already know this item matches, so catch exceptions, flag
				// an error on the item, and try to continue collection.
				auto_ptr<Item> item;
				try {
					// convert pid from string to pid_t
					pid_t pid;
					if (!Common::FromString(*pidIter, &pid))
						throw ProbeException("Couldn't convert to a pid_t: " + *pidIter);

					CompletePidItem(itemEntities.get(), pid);
					item = CreateItem();
					item->SetStatus(OvalEnum::STATUS_EXISTS);
					item->SetElements(itemEntities.get());
					itemEntities->clear(); // the item now owns the entities!

				} catch (Exception &e) {
					item = CreateItem();
					item->SetElements(itemEntities.get());
					itemEntities->clear(); // the item now owns the entities!
					item->AppendMessage(new OvalMessage(e.GetErrorMessage(), OvalEnum::LEVEL_ERROR));
				}

				items->push_back(item.release());
			}
		} else {
			// Search /proc for process IDs, and collect all those which match
			// the object.
			DirGuard procDir("/proc");
			struct dirent *d;

			errno = 0;
			while ((d = readdir(procDir))) {
				if (!strcmp(d->d_name, ".") || !strcmp(d->d_name, ".."))
					continue;

				// directories consisting only of decimal digits will be
				// interpreted as process IDs.  So check to see which chars
				// are in the dir name.
				const char *beg = &d->d_name[0];
				const char *end = beg + strlen(d->d_name);
				const char *ptr;
				for (ptr = beg; ptr != end && isdigit(*ptr); ++ptr)
					;
				if (ptr != end)
					continue;

				auto_ptr<ItemEntity> pidItemEntity(new ItemEntity("pid", d->d_name, 
																  OvalEnum::DATATYPE_INTEGER));
				if (pidObjEntity->Analyze(pidItemEntity.get()) != OvalEnum::RESULT_TRUE)
					continue;

				pid_t pid;
				if (!Common::FromString(d->d_name, &pid)) {
					// but this really ought to work, since we just verified it
					// had only decimal digits in it.
					Log::Debug("Couldn't convert to a pid_t: " + string(d->d_name));
					continue;
				}

				VectorPtrGuard<ItemEntity> itemEntities(new ItemEntityVector());
				itemEntities->push_back(pidItemEntity.release());

				// we already know this item matches, so catch exceptions, flag
				// an error on the item, and try to continue collection.
				auto_ptr<Item> item;
				try {
					CompletePidItem(itemEntities.get(), pid);
					item = CreateItem();
					item->SetStatus(OvalEnum::STATUS_EXISTS);
					item->SetElements(itemEntities.get());
					itemEntities->clear(); // the item now owns the entities!

				} catch (Exception &e) {
					item = CreateItem();
					item->SetElements(itemEntities.get());
					itemEntities->clear(); // the item now owns the entities!
					item->AppendMessage(new OvalMessage(e.GetErrorMessage(), OvalEnum::LEVEL_ERROR));
				}

				items->push_back(item.release());
			}

			if (errno)
				throw ProbeException(string("Error searching /proc for processes: ") + strerror(errno));
		}
	}

	void CompletePidItem(ItemEntityVector *itemEntities, pid_t pid) {

		// I guess in an item giving security context info about a process, add
		// the file entities with status=not collected?
		itemEntities->insert(itemEntities->begin(),
							 new ItemEntity("filename", "", OvalEnum::DATATYPE_STRING,
											OvalEnum::STATUS_NOT_COLLECTED));
		itemEntities->insert(itemEntities->begin(),
							 new ItemEntity("path", "", OvalEnum::DATATYPE_STRING,
											OvalEnum::STATUS_NOT_COLLECTED));
		itemEntities->insert(itemEntities->begin(),
							 new ItemEntity("filepath", "", OvalEnum::DATATYPE_STRING,
											OvalEnum::STATUS_NOT_COLLECTED));

		// get raw and non-raw security_context_t for the pid
		security_context_t pidSc;
		int ec = getpidcon(pid, &pidSc);
		if (ec)
			throw ProbeException("Error getting security context of pid " +
								 Common::ToString(pid));
		SecurityContextGuard pidScG(pidSc);

		security_context_t pidRawSc;
		ec = getpidcon_raw(pid, &pidRawSc);
		if (ec) 
			throw ProbeException("Error getting raw security context of pid " +
								 Common::ToString(pid));
		SecurityContextGuard pidRawScG(pidRawSc);

		// convert to a context_t so we can use the context_* functions
		ContextGuard pidC(pidSc);
		ContextGuard pidRawC(pidRawSc);

		CompleteItem(itemEntities, pidC, pidRawC);
	}

	void SearchByFiles(ObjectEntity *filepathObjEntity, ObjectEntity *pathObjEntity, 
					   ObjectEntity *filenameObjEntity, BehaviorVector *beh,
					   ItemVector *items) {

		FileFinder fileFinder;
		VectorPtrGuard<StringPair> filePaths(new StringPairVector());

		if(filepathObjEntity)
			filePaths.reset(fileFinder.SearchFiles(filepathObjEntity));
		else
			filePaths.reset(fileFinder.SearchFiles(pathObjEntity, filenameObjEntity, beh));

		if(filePaths.get() != NULL && !filePaths->empty()) {
			// Loop through all file paths
			StringPairVector::iterator iterator;
			for(iterator = filePaths->begin(); iterator != filePaths->end(); iterator++) {

				StringPair* fp = (*iterator);

				if(fp->second.empty()) {

					if(fileFinder.ReportFileNameDoesNotExist(fp->first, filenameObjEntity)) {

							auto_ptr<Item> item = CreateItem();
							item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
							item->AppendElement(new ItemEntity("filepath", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));
							item->AppendElement(new ItemEntity("path", fp->first, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
							item->AppendElement(new ItemEntity("filename", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));
							item->AppendElement(new ItemEntity("pid", "", OvalEnum::DATATYPE_INTEGER, OvalEnum::STATUS_NOT_COLLECTED));
							items->push_back(item.release());
					
					} else {

						auto_ptr<Item> item = CompleteFileItem(fp->first, fp->second);
						items->push_back(item.release());
					}

				} else {

					auto_ptr<Item> item = CompleteFileItem(fp->first, fp->second);
					items->push_back(item.release());

				}
			}
		} else {
			if ( filepathObjEntity != NULL ) {
				StringVector fpaths;
				if (fileFinder.ReportFilePathDoesNotExist(filepathObjEntity,&fpaths)){
					StringVector statusValues;
					StringPair* fpComponents = NULL;

					// build path ObjectEntity to pass to ReportPathDoesNotExist to retrieve the status of the path value
					ObjectEntity pathStatus("path","",OvalEnum::DATATYPE_STRING,OvalEnum::OPERATION_EQUALS,NULL,OvalEnum::CHECK_ALL,false);

					for(StringVector::iterator iterator = fpaths.begin(); iterator != fpaths.end(); iterator++) {
						auto_ptr<Item> item = CreateItem();
						item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
						fpComponents = Common::SplitFilePath(*iterator);
						pathStatus.SetValue(fpComponents->first);
						item->AppendElement(new ItemEntity("filepath", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));
						if (fileFinder.ReportPathDoesNotExist(&pathStatus))
							item->AppendElement(new ItemEntity("path", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));
						else
							item->AppendElement(new ItemEntity("path", fpComponents->first));
						item->AppendElement(new ItemEntity("filename", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));
						items->push_back(item.release());
					
						if ( fpComponents != NULL ){
							delete fpComponents;
							fpComponents = NULL;
						}
					}
				}
			} else {
				if(fileFinder.ReportPathDoesNotExist(pathObjEntity)) {
					auto_ptr<Item> item = CreateItem();
					item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
					item->AppendElement(new ItemEntity("path", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));
					items->push_back(item.release());
				}
			}
		}
	}

	auto_ptr<Item> CompleteFileItem(const string &dir, const string &fileName) {
		auto_ptr<Item> item = CreateItem();
		item->SetStatus(OvalEnum::STATUS_EXISTS);
		
		if (fileName.empty())
			item->AppendElement(new ItemEntity("filepath", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_NOT_COLLECTED));
		else
			item->AppendElement(new ItemEntity("filepath", Common::BuildFilePath(dir, fileName), OvalEnum::DATATYPE_STRING));
		
		item->AppendElement(new ItemEntity("path", dir, OvalEnum::DATATYPE_STRING));
		
		if (fileName.empty())
			item->AppendElement(new ItemEntity("filename", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_NOT_COLLECTED, true));
		else
			item->AppendElement(new ItemEntity("filename", fileName, OvalEnum::DATATYPE_STRING));

		item->AppendElement(new ItemEntity("pid", "", OvalEnum::DATATYPE_INTEGER, OvalEnum::STATUS_NOT_COLLECTED));

		string filePath;
		if (fileName.empty())
			filePath = dir;
		else
			filePath = Common::BuildFilePath(dir, fileName);

		int ec;
		security_context_t sc;
		ec = getfilecon(filePath.c_str(), &sc);
		if (ec == -1)
			throw ProbeException("Error getting security context of "+filePath+": "+strerror(errno));
		SecurityContextGuard scG(sc);

		security_context_t rawSc;
		ec = getfilecon_raw(filePath.c_str(), &rawSc);
		if (ec == -1)
			throw ProbeException("Error getting raw security context of "+filePath+": "+strerror(errno));
		SecurityContextGuard rawScG(rawSc);

		ContextGuard c(sc);
		ContextGuard rawC(rawSc);

		CompleteItem(item->GetElements(), c, rawC);

		return item;
	}

	void CompleteItem(ItemEntityVector *itemEntities, context_t ctx, context_t rawCtx) {

		itemEntities->push_back(new ItemEntity("user", context_user_get(ctx)));
		itemEntities->push_back(new ItemEntity("role", context_role_get(ctx)));
		itemEntities->push_back(new ItemEntity("type", context_type_get(ctx)));

// leaving the trailing ; off the macro definition and putting it at the end of
// the macro invocation lines allows emacs' auto-indent to work better... :-P
#define ADD_ENTITY(_entityName, _var)									\
		if ((_var).empty())												\
			itemEntities->push_back(new ItemEntity((_entityName), "",	\
												   OvalEnum::DATATYPE_STRING, \
												   OvalEnum::STATUS_DOES_NOT_EXIST)); \
		else 															\
			itemEntities->push_back(new ItemEntity((_entityName), (_var)))

		const char *range = context_range_get(ctx);
		string sensLow, catLow, sensHigh, catHigh;
		ParseRange(range, &sensLow, &catLow, &sensHigh, &catHigh);

		ADD_ENTITY("low_sensitivity", sensLow);
		ADD_ENTITY("low_category", catLow);
		ADD_ENTITY("high_sensitivity", sensHigh);
		ADD_ENTITY("high_category", catHigh);

		const char *rawRange = context_range_get(rawCtx);
		sensLow = catLow = sensHigh = catHigh = "";
		ParseRange(rawRange, &sensLow, &catLow, &sensHigh, &catHigh);

		ADD_ENTITY("rawlow_sensitivity", sensLow);
		ADD_ENTITY("rawlow_category", catLow);
		ADD_ENTITY("rawhigh_sensitivity", sensHigh);
		ADD_ENTITY("rawhigh_category", catHigh);

#undef ADD_ENTITY
	}

	void ParseRange(const char *range, string *lowSensitivity, string *lowCategory, 
					string *highSensitivity, string *highCategory) {
		REGEX re;
		vector<StringVector> matches;
		string low, high;

		if (!range)
			return;
		
		// split into low and high parts
		re.GetAllMatchingSubstrings("[^-]+", range, matches);
		if (matches.empty())
			throw ProbeException("Couldn't parse as range: \""+string(range)+"\"");

		low = matches[0][0];
		if (matches.size() > 1)
			high = matches[1][0];
		
		// split low and high parts into sensitivity and category parts
		// We ought to have at least a low part here.
		matches.clear();
		re.GetAllMatchingSubstrings("[^:]+", low, matches);
		if (matches.empty())
			throw ProbeException("Couldn't parse as range: \""+string(range)+"\"");
		else if (matches.size() == 1)
			*lowSensitivity = low;
		else {
			*lowSensitivity = matches[0][0];
			*lowCategory = matches[1][0];
		}

		if (!high.empty()) {
			matches.clear();
			re.GetAllMatchingSubstrings("[^:]+", high, matches);
			if (matches.empty())
				throw ProbeException("Couldn't parse as range: \""+string(range)+"\"");
			else if (matches.size() == 1)
				*highSensitivity = high;
			else {
				*highSensitivity = matches[0][0];
				*highCategory = matches[1][0];
			}
		}
	}
}
