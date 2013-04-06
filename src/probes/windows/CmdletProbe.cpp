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

#include <cassert>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <msclr\marshal_cppstd.h>

#include <VectorPtrGuard.h>
#include <ItemFieldEntityValue.h>
#include <OvalEnum.h>
#include <Log.h>

#include "CmdletProbe.h"

using namespace std;

using namespace System;
using namespace System::Collections;
using namespace System::Collections::ObjectModel;
using namespace System::Management::Automation;
using namespace System::Management::Automation::Host;
using namespace System::Management::Automation::Runspaces;
using namespace msclr::interop;

// some macros to surround code that should not throw
// managed exceptions.  It will convert the managed
// exceptions to unmanaged ones.
#define MANAGED_EXCEPTION_GUARD_BEGIN \
	try {

// Exceptions of an unmanaged type automatically get wrapped in
// SEHException, and if a catch clause of suitable unmanaged type
// is encountered, the SEHException is automatically unwrapped again.
// But if a matching managed-type catch clause is encountered, it gets
// the raw SEHException.  We want to let unmanaged exceptions pass
// through and let the automatic wrap/unwrap mechanism work, so we
// can't let the base catch(System::Exception ^) clause catch
// SEHExceptions.  So we have a special catch clause for SEHException.
#define MANAGED_EXCEPTION_GUARD_END \
	} catch(System::Runtime::InteropServices::SEHException ^) { \
		throw; \
	} catch (System::Exception ^e) { \
		throw ::Exception("Managed exception " + \
			marshal_as<string>(e->GetType()->FullName) + " occurred: " + \
			marshal_as<string>(e->Message)); \
	}

namespace {
	/**
	 * This class allows you to enumerate all combinations of values from
	 * a sequence of containers.  That is, each combination consists of
	 * one value from each container.  Each time you call advance(), the
	 * object internally advances to the next combination.  The values in
	 * the combination are acquired with the val() and combo() methods.
	 * <p>
	 * The object is initialized in a state representing no combination; the
	 * advance() method must be called to get the first combination.
	 * <p>
	 * Instances of this class will not own the containers whose values are
	 * combined.  You must make sure they're destroyed (after this class is
	 * done with them).
	 * <p>
	 * I decided for fun to make the class generic with respect to the container
	 * type.  (Depending on your idea of fun... I thought it was fun :))  The
	 * container must have bidirectional iterators.  Its value_type should also
	 * be default and copy-constructible.
	 */
	template <typename _Cont>
	class ContainerValueCombinations {
	public:
		/**
		 * Constructs the object to generate combinations from the given containers.
		 */
		explicit ContainerValueCombinations(const vector<const _Cont *> &containers);

		/** Returns the number of containers we're finding combinations for */
		size_t size() const {
			return containers.size();
		}

		/**
		 * Checks whether all iterators are at the ends of their containers.
		 * Returns true of all are at end; false if at least one is not at
		 * the end.
		 */
		bool atEnd() const {
			return atEndFlag;
		}

		/**
		 * Advances container iterators to their next combination.  This
		 * allows an easy while loop structure, e.g.
		 * <code>while (obj->advance()) { ... }</code>
		 * \return true if the advance succeeded, false otherwise
		 */
		bool advance();

		/**
		 * Accessor for getting the \p idx 'th value of the current combination
		 * (not the iterator pointing to it).  If the \p idx 'th container is empty,
		 * it returns a default-constructed value of the container's value_type.
		 * Not actually sure what the right thing to do is, in this case...
		 */
		typename _Cont::value_type val(size_t idx) const;

		/**
		 * Gets the current combo as a vector of values.  It calls val(size_t) to
		 * get the values, so the i'th element of the returned vector will have a
		 * default-constructed value if the i'th container is empty.
		 * \see ContainerValueCombinations::val(size_t)
		 */
		vector<typename _Cont::value_type> combo() const;

		/**
		 * Resets the object so that the combinations can be enumerated
		 * again.
		 */
		void reset();

	private:
		/** (pointers to) the containers we are iterating over */
		vector<const _Cont *> containers;

		/** The current container iterators */
		vector<typename _Cont::const_iterator> iterators;

		/** Set to true when there are no more combinations. */
		bool atEndFlag;

		/**
		 * The iterators are set up initially pointing to the
		 * beginnings of all the containers, which is the first
		 * combination.  To be easier to use, the first advance
		 * call will do nothing, and subsequent advance calls will
		 * advance to the next combination.  So in effect, to users,
		 * the illusion is maintained that instances of this class
		 * are initialized to not contain any combination; advance()
		 * must be called once to move to the first combination.
		 */
		bool beforeFirstAdvance;
	};

	/**
	 * Represents a module or snapin, identified by the 3 values corresponding
	 * to the object entities of this test: name, guid, and version.  Guid is
	 * null for snapins.
	 */
	ref struct AvailableModule {
		/**
		 * Dunno if simply testing if Guid is null or not is sufficient...
		 * it won't hurt to be explicit.  Turns out there is a whole
		 * separate API for manipulating snapins, from the API for
		 * modules.  So I gotta know which API to use.
		 */
		enum struct ModuleType {
			MODULE,
			SNAPIN
		};

		AvailableModule(String ^name, Guid ^guid, System::Version ^version)
			: name(name), guid(guid), version(version) {
				// safe to assume that null guid *always* means a snapin??
				moduleType = guid == nullptr ? ModuleType::SNAPIN : 
					ModuleType::MODULE;
		}

		AvailableModule(String ^name, System::Version ^version)
			: name(name), guid(nullptr), version(version), 
				moduleType(ModuleType::SNAPIN) {
		}

		virtual String ^ToString() override {
			return moduleType.ToString()+":"+name+"/"+
				(guid==nullptr?"":(guid+"/")) +
				version;
		}

		String ^name;
		Guid ^guid;
		System::Version ^version;
		ModuleType moduleType;
	};

	/**
	 * I want some managed global data.  I can't put it in the probe class 
	 * because the probe class is unmanaged.  I can't make it truly global
	 * (i.e. at file scope) because that's not allowed by the language.  
	 * So this is a workaround.
	 */
	ref struct ManagedGlobals {
		static Collection<AvailableModule^> ^availableModules = nullptr;
		initonly static array<String^> ^LEGAL_VERBS = {
			"approve",
			"assert",
			"compare",
			"confirm",
			"find",
			"get",
			"import",
			"measure",
			"open",
			"ping"
			"read",
			"request",
			"resolve",
			"search",
			"select",
			"show",
			"test",
			"trace",
			"watch"
		};
	};

	/**
	 * Gets available modules (via the Get-Module cmdlet) and snapins
	 * (via the Get-PSSnapin cmdlet) and returns info about them.
	 */
	Collection<AvailableModule^>^ GetAvailableModules(Runspace ^runspace);

	/**
	 * Creates a restricted runspace: all cmdlets which don't satisfy
	 * certain criteria are removed.  This uses the white-list of
	 * verbs, and filters out aliases.  The Add-PSSnapin cmdlet is
	 * allowed even though its verb is not allowed, to support
	 * snapins.
	 * <p>
	 * Logic for this is mostly copied from sample code from Microsoft.
	 *
	 * \return a runspace
	 */
	Runspace ^MakeRunspace();

	/**
	 * Checks whether the verb part of the given cmdlet name is
	 * legal.  It is legal if it is contained in the white-list
	 * of legal verbs defined by ManagedGlobals::LEGAL_VERBS.
	 * Comparisons are done case-insensitively.
	 *
	 * \return true if the cmdlet has a legal verb, false if not.
	 */
	bool HasLegalVerb(String ^cmdletName);

	/**
	 * Checks whether the given verb is in the white-list of allowed
	 * verbs.  Comparisons are done case-insensitively.
	 *
	 * \return true if the verb is in the white-list, false if not.
	 */
	bool IsLegalVerb(String ^verb);

	/**
	 * Creates an item.
	 */
	auto_ptr<Item> CreateItem();

	/**
	 * Attempts to import a module with the given name into the given
	 * runspace.
	 */
	void ImportModule(Runspace ^runspace, AvailableModule ^module);

	/**
	 * Attempts to find the module or snapin which contains the given
	 * command, which satisfies the given module entities.  If moduleName
	 * is non-NULL, it is prepended to the given verb and noun when forming
	 * a fully-qualified command name.
	 *
	 * \return an AvailableModule object describing a module or snapin with
	 *   matching name/guid/version, or null if one could not be found.
	 */
	AvailableModule ^VerifyModuleInfo(Runspace ^runspace, const string &verb, 
		const string &noun, const string *moduleName,
		ObjectEntity *modNameObjEntity, ObjectEntity *modIdObjEntity,
		ObjectEntity *modVersionObjEntity);

	/**
	 * Creates an item with entity values from the given data: the module_*
	 * entities come from modUsed, verb and noun from the corresponding
	 * params, and the parameters and select entities are built from the
	 * given vectors of names and values.
	 *
	 * \return the constructed item
	 */
	auto_ptr<Item> StartItem(AvailableModule ^modUsed, const string &verb,
		const string &noun, const StringVector &paramNames, 
		const StringVector &selectNames, const StringVector &paramCombo,
		const StringVector &selectCombo);

	/**
	 * Given a verb and noun and module info, attempts to find a matching
	 * module, and then invokes the resulting fully qualified command with all
	 * the different combinations of parameter and select values.
	 *
	 * \param runspace the runspace to use when running commands
	 * \param verb the command verb
	 * \param noun the command noun
	 * \param paramNames the command parameter names, if any (could be empty)
	 * \param selectNames the select parameter names, if any (could be empty)
	 * \param paramCombos the command parameter value combo generator
	 * \param selectCombos the select parameter value combo generator
	 * \param moduleName a module name which further qualifies which command
	 *   should be run.  Passed as a pointer so that NULL can be passed in the
	 *   case that the module_name entity was nil'd, meaning we don't care
	 *   which module the command comes from.
	 * \param modNameObjEntity module_name object entity, used to verify that
	 *   a command from the proper module is being run
	 * \param modIdObjEntity module_id object entity, used to verify that a
	 *   a command from the proper module is being run
	 * \param modVersionObjEntity module_version object entity, used to verify
	 *   that a command from the proper module is being run
	 * \param items pointer to a vector which receive matching items
	 */
	void RunVerbNoun(Runspace ^runspace, const string &verb,
		const string &noun, const StringVector &paramNames,
		const StringVector &selectNames,
		ContainerValueCombinations<StringVector> &paramCombos,
		ContainerValueCombinations<StringVector> &selectCombos,
		const string *moduleName, ObjectEntity *modNameObjEntity,
		ObjectEntity *modIdObjEntity, ObjectEntity *modVersionObjEntity,
		ItemVector *items);

	/**
	 * Invokes the given command from the given module, using the given
	 * command and select parameters.  Matching items are added to the
	 * given vector.
	 */
	void InvokeCmdlet(Runspace ^runspace, AvailableModule ^modUsed,
		const string &verb, const string &noun,
		const StringVector &paramNames, const StringVector &selectNames,
		const StringVector &paramCombo, const StringVector &selectCombo,
		ItemVector *items);

	/**
	 * Adds a value entity to the given item, which contains the given
	 * command result.
	 */
	void AddResultToItem(PSObject ^result, Item *item);

	/**
	 * Maps a .net type to an Oval type.
	 */
	OvalEnum::Datatype GetDatatype(Type ^type);
}

//****************************************************************************************//
//								CmdletProbe Class											  //	
//****************************************************************************************//
CmdletProbe* CmdletProbe::instance = NULL;

CmdletProbe::CmdletProbe() {
}

CmdletProbe::~CmdletProbe() {
  instance = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* CmdletProbe::Instance() {

	// Use lazy initialization
	if(instance == NULL) 
		instance = new CmdletProbe();

	return instance;	
}

ItemVector* CmdletProbe::CollectItems(::Object *object) {

	MANAGED_EXCEPTION_GUARD_BEGIN

	VectorPtrGuard<Item> collectedItems(new ItemVector());

	ObjectEntity
		*modNameObjEntity = object->GetElementByName("module_name"),
		*modIdObjEntity = object->GetElementByName("module_id"),
		*modVersionObjEntity = object->GetElementByName("module_version"),
		*verbObjEntity = object->GetElementByName("verb"),
		*nounObjEntity = object->GetElementByName("noun"),
		*paramsObjEntity = object->GetElementByName("parameters"),
		*selectObjEntity = object->GetElementByName("select");

	StringVector verbs, nouns, moduleNames, paramNames, selectNames;
	map<string, StringVector> params, selects;
	vector<const StringVector*> allParams, allSelects;


	if (!modNameObjEntity->GetNil())
	/*OvalEnum::Flag flag =*/ modNameObjEntity->GetEntityValues(moduleNames);

	/*OvalEnum::Flag flag =*/ verbObjEntity->GetEntityValues(verbs);
	/*OvalEnum::Flag flag =*/ nounObjEntity->GetEntityValues(nouns);

	if (!paramsObjEntity->GetNil())
		/*OvalEnum::Flag flag =*/ paramsObjEntity->GetEntityValues(params);
	if (!selectObjEntity->GetNil())
		/*OvalEnum::Flag flag =*/ selectObjEntity->GetEntityValues(selects);

	// Before going any farther, make sure the given verbs are legal.  If we
	// just let this go, the cmdlet will not be found due to the restricted
	// runspace, which will result in a collected object flag of "does not exist".
	// That's the wrong behavior; it should be error.  So we check it here
	// and throw an exception, which will cause an error flag.
	for (StringVector::iterator verbIter = verbs.begin();
		verbIter != verbs.end();
		++verbIter)
		if (!IsLegalVerb(marshal_as<String^>(*verbIter)))
			throw ProbeException("Illegal cmdlet verb encountered: "+*verbIter);

	// I decided against reusing a global runspace, because the potential
	// module importing that would be done could change collection behavior
	// for future collections.  So a fresh runspace is created each time
	// to ensure consistent behavior.
	Runspace ^runspace = MakeRunspace();

	// It seems scripts are evaluated in this default runspace; I get
	// errors when evaluating some property values, unless I set this.
	Runspace::DefaultRunspace = runspace;

	// the list of available modules need only be obtained once.  Each
	// AvailableModule object has no connection to any particular
	// runspace.
	if (ManagedGlobals::availableModules == nullptr)
		ManagedGlobals::availableModules = GetAvailableModules(runspace);

	// create our vectors of vectors of params and selects, so we can
	// enumerate all the combinations.  We'll have to iterate over all
	// combinations once per verb+noun+module combination.  I'll also
	// create a vector of param and select names.  This will be handy
	// to pair up with each param/select value combo.
	for (map<string, StringVector>::iterator iter = params.begin();
		iter != params.end();
		++iter) {
		paramNames.push_back(iter->first);
		allParams.push_back(&iter->second);
	}

	for (map<string, StringVector>::iterator iter = selects.begin();
		iter != selects.end();
		++iter) {
		selectNames.push_back(iter->first);
		allSelects.push_back(&iter->second);
	}

	ContainerValueCombinations<StringVector> paramCombos(allParams);
	ContainerValueCombinations<StringVector> selectCombos(allSelects);

	for (StringVector::iterator verbIter = verbs.begin();
		verbIter != verbs.end();
		++verbIter) {

		for (StringVector::iterator nounIter = nouns.begin();
			nounIter != nouns.end();
			++nounIter) {

			if (Common::EqualsIgnoreCase(*verbIter, "Select") && 
				Common::EqualsIgnoreCase(*nounIter, "Object")) {
				Log::Info("Illegal cmdlet: Select-Object.  This is only usable indirectly via the select entity.  Ignoring...");
				continue;
			}

			if (modNameObjEntity->GetNil())
				RunVerbNoun(runspace, *verbIter, *nounIter, paramNames,
					selectNames, paramCombos, selectCombos, NULL, 
					modNameObjEntity, modIdObjEntity, modVersionObjEntity, 
					collectedItems.get());
			else
				for (StringVector::iterator modNameIter = moduleNames.begin();
					modNameIter != moduleNames.end();
					++modNameIter)
					RunVerbNoun(runspace, *verbIter, *nounIter, paramNames, 
						selectNames, paramCombos, selectCombos, &*modNameIter,
						modNameObjEntity, modIdObjEntity, modVersionObjEntity,
						collectedItems.get());
		}
	}

	return collectedItems.release();



	MANAGED_EXCEPTION_GUARD_END
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// not used.
Item* CmdletProbe::CreateItem() {
	return NULL;
}

namespace {

	template<typename _Cont>
	ContainerValueCombinations<_Cont>::ContainerValueCombinations(const vector<const _Cont *> &containers)
		: containers(containers),
			atEndFlag(true), 
			beforeFirstAdvance(true) {

		reset();
	}

	template<typename _Cont>
	bool ContainerValueCombinations<_Cont>::advance() {

		if (atEnd())
			return false;

		if (beforeFirstAdvance) {
			beforeFirstAdvance = false;
			return true;
		}

		// start iterators off pointing to the last element of their vectors
		// If we're here, the following container sizes are at least one, so
		// the iterator decrements are ok.
		assert(!iterators.empty()); // guess it's ok to check tho...
		assert(!containers.empty());
		typename vector<typename _Cont::const_iterator>::iterator iterIter = iterators.end();
		--iterIter;
		typename vector<const _Cont *>::iterator contIter = containers.end();
		--contIter;

		// We start at the end, working backward, resetting iterators
		// from the end of their containers to the beginning, until
		// we find one that doesn't need to be reset.  That one is
		// just incremented.  This is repeated until the first iterator
		// hits the end, then we're done.
		for (;
			iterIter != iterators.begin();
			--iterIter, --contIter) {

			// behave gracefully for empty containers...
			if (*iterIter != (*contIter)->end()) {
				if (++*iterIter == (*contIter)->end())
					*iterIter = (*contIter)->begin();
				else
					break;
			}
		}

		if (iterIter == iterators.begin()) {
			// we get in here if all iterators were reset to the beginning of
			// their containers, save for the first one.  If the first one
			// is at the end of its container, we are done.
			if (*iterIter == (*contIter)->end() ||
				++*iterIter == (*contIter)->end()) {
				// empty container case
				atEndFlag = true;
				return false;
			}
		}

		return true;
	}

	template <typename _Cont>
	typename _Cont::value_type ContainerValueCombinations<_Cont>::val(size_t idx) const {
		if (beforeFirstAdvance)
			throw ProbeException("Must advance at least once before calling");
		if (atEnd())
			throw ProbeException("No more combinations!");

		if (containers[idx]->empty())
			// what to do if an empty container at this position...?
			return typename _Cont::value_type();

		// We make sure that none of the iterators ever point to 
		// container end, unless the container is empty, or there
		// are no more combinations.  (Both of which we've already
		// checked for.)
		return *iterators[idx];
	}

	template <typename _Cont>
	vector<typename _Cont::value_type> ContainerValueCombinations<_Cont>::combo() const {
		vector<typename _Cont::value_type> vals;
		for (size_t idx=0; idx<size(); ++idx)
			vals.push_back(val(idx));

		return vals;
	}

	template <typename _Cont>
	void ContainerValueCombinations<_Cont>::reset() {

		atEndFlag = beforeFirstAdvance = true;
		iterators.clear();

		// initialize iterators to the beginnings of their containers.  Also
		// we can check whether we started out at the end, which happens when
		// all containers are empty.
		for (typename vector<const _Cont *>::iterator contIter = containers.begin();
			contIter != containers.end();
			++contIter) {
			iterators.push_back((*contIter)->begin());
			if (!(*contIter)->empty())
				atEndFlag = false;
		}
	}

	bool HasLegalVerb(String ^cmdletName) {
		for each (String ^legalVerb in ManagedGlobals::LEGAL_VERBS) {
			if (cmdletName->StartsWith(legalVerb+"-", 
				StringComparison::OrdinalIgnoreCase))
				return true;
		}

		return false;
	}

	bool IsLegalVerb(String ^verb) {
		for each (String ^legalVerb in ManagedGlobals::LEGAL_VERBS) {
			if (verb->Equals(legalVerb, StringComparison::OrdinalIgnoreCase))
				return true;
		}

		return false;
	}

	auto_ptr<Item> CreateItem() {
		return auto_ptr<Item>(new Item(0, 
							"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows", 
							"win-sc", 
							"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows windows-system-characteristics-schema.xsd", 
							OvalEnum::STATUS_ERROR, 
							"cmdlet_item"));
	}

	Runspace ^MakeRunspace() {
		InitialSessionState ^iss = InitialSessionState::CreateDefault();
		// other setup, copied from Microsoft's sample code
		iss->LanguageMode = PSLanguageMode::NoLanguage;
		iss->ThrowOnRunspaceOpenError = true;
		iss->Formats->Clear();

		//Log::Debug("Default InitialSessionState has " + Common::ToString(iss->Commands->Count)+" commands");

		for each (SessionStateCommandEntry ^cmd in iss->Commands) {

			// filter out the command if:
			// - it's an alias
			// -or- the verb is not allowed (except if it is the Add-PSSnapin
			//      cmdlet, which we need to support snapins)
			//
			// where "filter out" means just hiding it.  This allows legal
			// cmdlets to call them as part of their implementation.
			//
			// We must also allow Select-Object so that we can process the
			// "select" object entity, Get-Module so we can get available
			// modules, and Import-Module so we can load commands from other
			// available modules as necessary.  Currently, all ought to be
			// allowed automatically, since their verbs are legal, so no
			// special action is necessary.
			//
			// Update: snapins require the use of totally separate commands: 
			// Get-PSSnapin to list available snapins, and Add-PSSnapin to
			// add them to our session.  "Add" is not one of our allowed
			// verbs, but I will have to make an exception here so that we
			// can support snapins.
			if (cmd->CommandType == CommandTypes::Alias ||
				(!HasLegalVerb(cmd->Name) && !cmd->Name->Equals("Add-PSSnapin")))
				cmd->Visibility = SessionStateEntryVisibility::Private;
/*			else
				Log::Debug(marshal_as<string>(cmd->Name)+" passed");*/
		}

		Runspace ^runspace = RunspaceFactory::CreateRunspace(iss);
		// copied from MS sample code
		runspace->ThreadOptions = PSThreadOptions::UseCurrentThread;
		
		runspace->Open();
		return runspace;
	}

	Collection<AvailableModule^> ^GetAvailableModules(Runspace ^runspace) {
		PowerShell ^ps = PowerShell::Create();
		ps->Runspace = runspace;
		ps->AddCommand("Get-Module")->AddParameter("ListAvailable")
			->AddCommand("Select-Object")->AddParameter("Property", 
				gcnew array<String^> {"Name","Guid","Version"});

		Collection<AvailableModule^> ^modules = gcnew Collection<AvailableModule^>();

		try {
			Collection<PSObject ^> ^results = ps->Invoke();
			for each (PSObject ^result in results) {
				System::Object ^nameObj = result->Properties["Name"]->Value;
				System::Object ^guidObj = result->Properties["Guid"]->Value;
				System::Object ^versionObj = result->Properties["Version"]->Value;

				String ^name = safe_cast<String^>(nameObj);
				Guid ^guid = guidObj == nullptr ? nullptr : safe_cast<Guid^>(guidObj);
				System::Version ^version = safe_cast<System::Version^>(versionObj);

				modules->Add(gcnew AvailableModule(name, guid, version));
			}
		} catch(System::Exception ^e) {
			Log::Info("Failed to get available powershell modules: " + 
				marshal_as<string>(e->GetType()->FullName)+": " +
				marshal_as<string>(e->Message));
			Log::Info("No modules will be searched for commands.");
			modules->Clear();
		} finally {
			delete ps;
		}

		// run a separate command for getting snapins
		ps = PowerShell::Create();
		ps->Runspace = runspace;
		ps->AddCommand("Get-PSSnapin")->AddParameter("Registered")
			->AddCommand("Select-Object")->AddParameter("Property",
				gcnew array<String^> {"Name", "Version"});

		// capture into a 2nd collection, which we can drop/delete on error.  So
		// we'd lose the snapins, but not the modules, if getting the modules
		// was successful.
		Collection<AvailableModule^> ^snapins = gcnew Collection<AvailableModule^>();
		try {
			Collection<PSObject ^> ^results = ps->Invoke();
			for each (PSObject ^result in results) {
				System::Object ^nameObj = result->Properties["Name"]->Value;
				System::Object ^versionObj = result->Properties["Version"]->Value;

				String ^name = safe_cast<String^>(nameObj);
				System::Version ^version = safe_cast<System::Version^>(versionObj);

				snapins->Add(gcnew AvailableModule(name, version));
			}

			// No addAll() equivalent on collections???
			for each (AvailableModule ^snapin in snapins)
				modules->Add(snapin);

		} catch(System::Exception ^e) {
			Log::Info("Failed to get available powershell snapins: " + 
				marshal_as<string>(e->GetType()->FullName)+": " +
				marshal_as<string>(e->Message));
			Log::Info("No snapins will be searched for commands.");
		} finally {
			delete ps;
		}

		for each (AvailableModule ^mod in modules)
			Log::Debug("Found "+marshal_as<string>(mod->ToString()));

		return modules;
	}

	void ImportModule(Runspace ^runspace, AvailableModule ^module) {
		Log::Debug("Importing "+marshal_as<string>(module->ToString()));
		PowerShell ^ps = PowerShell::Create();
		try {
			ps->Runspace = runspace;

			switch (module->moduleType) {
			case AvailableModule::ModuleType::MODULE:
				ps->AddCommand("Import-Module")->AddParameter("Name", module->name);
				break;
			case AvailableModule::ModuleType::SNAPIN:
				// we made special provisions to make sure this command with an
				// otherwise-disallowed verb will run.
				ps->AddCommand("Add-PSSnapin")->AddParameter("Name", module->name);
				break;
			default:
				Log::Info("Unhandled module type; will not be imported: " +
					marshal_as<string>(module->moduleType.ToString()));
				return;
			}

			ps->Invoke();
		} finally {
			delete ps;
		}
	}

	AvailableModule ^VerifyModuleInfo(Runspace ^runspace, const string &verb, 
		const string &noun, const string *moduleName,
		ObjectEntity *modNameObjEntity, ObjectEntity *modIdObjEntity,
		ObjectEntity *modVersionObjEntity) {

		// we will reuse these item entities in a couple places
		ItemEntity modNameIe("module_name");
		ItemEntity modIdIe("module_id");
		ItemEntity modVersionIe("module_version", "", OvalEnum::DATATYPE_VERSION);

		string fullyQualifiedName;
		if (moduleName)
			fullyQualifiedName = *moduleName + "\\" + verb + "-" + noun;
		else
			fullyQualifiedName = verb + "-" + noun;

		CommandInfo ^ci = runspace->SessionStateProxy->InvokeCommand->
			GetCommand(marshal_as<String^>(fullyQualifiedName), 
				CommandTypes::Cmdlet|CommandTypes::Function);

		if (modNameObjEntity->GetNil() && ci == nullptr)
			// if module_name obj entity was nil'd and there is no command
			// found using the plain Verb-Noun syntax, then we don't look
			// any further and assume the command doesn't exist.
			return nullptr;

		if (ci == nullptr) {
			// A module name was given but we didn't turn up the command
			// with a fully qualified command name.  Search through the
			// available modules for a match, import the matches, and
			// try again to find the command.
			for each (AvailableModule ^availMod in ManagedGlobals::availableModules) {

				// if the oval definition specifies a guid but this module
				// doesn't have one, it can't match.  So skip it.
				if (!modIdObjEntity->GetNil() && availMod->guid == nullptr)
					continue;

				// so marshal_as will work with members of managed classes....
				String ^dumbhack;
				dumbhack = availMod->name;
				modNameIe.SetValue(marshal_as<string>(dumbhack));

				modIdIe.SetValue("");
				modIdIe.SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
				if (availMod->guid != nullptr) {
					modIdIe.SetStatus(OvalEnum::STATUS_EXISTS);
					modIdIe.SetValue(marshal_as<string>(availMod->guid->ToString()));
				}

				modVersionIe.SetValue(marshal_as<string>(availMod->version->ToString()));

				bool match =
					(modNameObjEntity->GetNil() || 
						(modNameObjEntity->Analyze(&modNameIe) == OvalEnum::RESULT_TRUE)) &&
					(modIdObjEntity->GetNil() || 
						(modIdObjEntity->Analyze(&modIdIe) == OvalEnum::RESULT_TRUE)) &&
					(modVersionObjEntity->GetNil() || 
						(modVersionObjEntity->Analyze(&modVersionIe) == OvalEnum::RESULT_TRUE));

				if (match)
					ImportModule(runspace, availMod);
			}

			// after all matching modules are imported, try once again
			// to find the command...
			ci = runspace->SessionStateProxy->InvokeCommand->
				GetCommand(marshal_as<String^>(fullyQualifiedName), 
					CommandTypes::Cmdlet|CommandTypes::Function);
		}

		if (ci == nullptr) {
			// if still no command found, we give up
			return nullptr;
		}

		// else, we found a command.  Double check all its module
		// properties against the object entities.
		Guid ^modGuid = nullptr;
		System::Version ^modVersion = nullptr;
		String ^modName = nullptr;

		if (ci->Module == nullptr) {
			// no module... see if it comes from a snapin
			CmdletInfo ^cmi = safe_cast<CmdletInfo^>(ci);
			if (cmi != nullptr) {
				modVersion = cmi->PSSnapIn->Version;
				modName = cmi->PSSnapIn->Name;
				// snapins don't have a GUID.
			}
		} else {
			modGuid = ci->Module->Guid;
			modVersion = ci->Module->Version;
			modName = ci->Module->Name;
		}

		// If the user asked for a particular name/guid/version
		// and we don't have that value, there can be no match.
		if ((!modVersionObjEntity->GetNil() && modVersion==nullptr) ||
			(!modIdObjEntity->GetNil() && modGuid==nullptr) ||
			(!modNameObjEntity->GetNil() && modName==nullptr))
			return nullptr;

		// for those non-nil'd entities, a final check against the actual
		// module info.  Reset the entities to make sure there aren't old
		// values dangling in there...
		modIdIe.SetValue("");
		modIdIe.SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
		modNameIe.SetValue("");
		modNameIe.SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
		modVersionIe.SetValue("");
		modVersionIe.SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
		if (modGuid != nullptr) { 
			modIdIe.SetValue(marshal_as<string>(modGuid->ToString()));
			modIdIe.SetStatus(OvalEnum::STATUS_EXISTS);
		}
		if (modName != nullptr) {
			modNameIe.SetValue(marshal_as<string>(modName));
			modNameIe.SetStatus(OvalEnum::STATUS_EXISTS);
		}
		if (modVersion != nullptr) {
			modVersionIe.SetValue(marshal_as<string>(modVersion->ToString()));
			modVersionIe.SetStatus(OvalEnum::STATUS_EXISTS);
		}

		bool match =
			(modNameObjEntity->GetNil() || 
				(modNameObjEntity->Analyze(&modNameIe) == OvalEnum::RESULT_TRUE)) &&
			(modIdObjEntity->GetNil() || 
				(modIdObjEntity->Analyze(&modIdIe) == OvalEnum::RESULT_TRUE)) &&
			(modVersionObjEntity->GetNil() || 
				(modVersionObjEntity->Analyze(&modVersionIe) == OvalEnum::RESULT_TRUE));

		if (match)
			return gcnew AvailableModule(modName, modGuid, modVersion);

		return nullptr;
	}

	auto_ptr<Item> StartItem(AvailableModule ^modUsed, const string &verb,
		const string &noun, const StringVector &paramNames, 
		const StringVector &selectNames, const StringVector &paramCombo,
		const StringVector &selectCombo) {

		auto_ptr<Item> item = CreateItem();
		item->SetStatus(OvalEnum::STATUS_EXISTS);
		String ^dumbhack = modUsed->name;
		item->AppendElement(new ItemEntity("module_name",
			(dumbhack==nullptr ? "":marshal_as<string>(dumbhack)),
			OvalEnum::DATATYPE_STRING, (dumbhack==nullptr ? OvalEnum::STATUS_NOT_COLLECTED : OvalEnum::STATUS_EXISTS),
			dumbhack==nullptr));

		dumbhack = modUsed->guid == nullptr ? nullptr : modUsed->guid->ToString();
		item->AppendElement(new ItemEntity("module_id",
			(dumbhack==nullptr ? "":marshal_as<string>(dumbhack)),
			OvalEnum::DATATYPE_STRING, (dumbhack==nullptr ? OvalEnum::STATUS_NOT_COLLECTED : OvalEnum::STATUS_EXISTS),
			dumbhack==nullptr));

		dumbhack = modUsed->version == nullptr ? nullptr : modUsed->version->ToString();
		item->AppendElement(new ItemEntity("module_version",
			(dumbhack==nullptr ? "":marshal_as<string>(dumbhack)),
			OvalEnum::DATATYPE_VERSION, (dumbhack==nullptr ? OvalEnum::STATUS_NOT_COLLECTED : OvalEnum::STATUS_EXISTS),
			dumbhack==nullptr));

		item->AppendElement(new ItemEntity("verb", verb, OvalEnum::DATATYPE_STRING,
			OvalEnum::STATUS_EXISTS));
		item->AppendElement(new ItemEntity("noun", noun, OvalEnum::DATATYPE_STRING,
			OvalEnum::STATUS_EXISTS));

		AbsEntityValueVector paramEntityValues;
		StringVector::const_iterator paramNameIter, paramComboIter;
		for (paramNameIter = paramNames.begin(), paramComboIter = paramCombo.begin();
			paramNameIter != paramNames.end();
			++paramNameIter, ++paramComboIter)
			paramEntityValues.push_back(new ItemFieldEntityValue(
				*paramNameIter, *paramComboIter));

		if (paramEntityValues.empty())
			item->AppendElement(new ItemEntity("parameters", paramEntityValues,
				OvalEnum::DATATYPE_RECORD, OvalEnum::STATUS_NOT_COLLECTED,
				true));
		else
			item->AppendElement(new ItemEntity("parameters", paramEntityValues,
				OvalEnum::DATATYPE_RECORD, OvalEnum::STATUS_EXISTS));

		AbsEntityValueVector selectEntityValues;
		StringVector::const_iterator selectNameIter, selectComboIter;
		for (selectNameIter = selectNames.begin(), selectComboIter = selectCombo.begin();
			selectNameIter != selectNames.end();
			++selectNameIter, ++selectComboIter)
			selectEntityValues.push_back(new ItemFieldEntityValue(
				*selectNameIter, *selectComboIter));

		if (selectEntityValues.empty())
			item->AppendElement(new ItemEntity("select", selectEntityValues,
				OvalEnum::DATATYPE_RECORD, OvalEnum::STATUS_NOT_COLLECTED,
				true));
		else
			item->AppendElement(new ItemEntity("select", selectEntityValues,
				OvalEnum::DATATYPE_RECORD, OvalEnum::STATUS_EXISTS));

		return item;
	}

	void RunVerbNoun(Runspace ^runspace, const string &verb,
		const string &noun, const StringVector &paramNames,
		const StringVector &selectNames,
		ContainerValueCombinations<StringVector> &paramCombos,
		ContainerValueCombinations<StringVector> &selectCombos,
		const string *moduleName, ObjectEntity *modNameObjEntity,
		ObjectEntity *modIdObjEntity, ObjectEntity *modVersionObjEntity,
		ItemVector *items) {

		AvailableModule ^modUsed = VerifyModuleInfo(runspace, verb, noun, 
			moduleName, modNameObjEntity, modIdObjEntity, modVersionObjEntity);

		if (modUsed == nullptr)
			// couldn't find the command in any modules
			return;

		// params and select are both nillable... so there are
		// 4 cases: both are nil, neither, or exactly one or the other.
		StringVector emptyPh; // placeholder for combo, when there are no
								// names (the entity was nil'd)
		if (!paramNames.empty() && !selectNames.empty()) {
			paramCombos.reset();
			while (paramCombos.advance()) {
				selectCombos.reset();
				while (selectCombos.advance()) {
					InvokeCmdlet(runspace, modUsed, verb, noun, paramNames,
						selectNames, paramCombos.combo(), selectCombos.combo(),
						items);
				}
			}
		} else if (!paramNames.empty()) {
			paramCombos.reset();
			while (paramCombos.advance()) {
				InvokeCmdlet(runspace, modUsed, verb, noun, paramNames,
					selectNames, paramCombos.combo(), emptyPh, 
					items);
			}
		} else if (!selectNames.empty()) {
			selectCombos.reset();
			while (selectCombos.advance()) {
				InvokeCmdlet(runspace, modUsed, verb, noun, paramNames, 
					selectNames, emptyPh, selectCombos.combo(),
					items);
			}
		} else {
			// both params and select are nil
			InvokeCmdlet(runspace, modUsed, verb, noun, paramNames, 
				selectNames, emptyPh, emptyPh, items);
		}
	}

	void InvokeCmdlet(Runspace ^runspace, AvailableModule ^modUsed,
		const string &verb, const string &noun,
		const StringVector &paramNames, const StringVector &selectNames,
		const StringVector &paramCombo, const StringVector &selectCombo,
		ItemVector *items) {

		auto_ptr<Item> item = StartItem(modUsed, verb, noun, paramNames,
			selectNames, paramCombo, selectCombo);

		ostringstream cmdline; // for debugging
		PowerShell ^ps = PowerShell::Create();

		try {
			ps->Runspace = runspace;

			if (modUsed == nullptr) {
				ps->AddCommand(marshal_as<String^>(verb + "-" + noun));
				cmdline << verb << '-' << noun;
			} else {
				ps->AddCommand(modUsed->name+"\\"+marshal_as<String^>(verb)+
					"-"+marshal_as<String^>(noun));
				// marshal_as has fits with members of managed classes... :(
				String ^dumbhack = modUsed->name;
				cmdline << marshal_as<string>(dumbhack) << '\\' << verb << '-' << noun;
			}

			// For command parameters, I need to support multiple comma-
			// delimited values.  As long as the parameter type is string, a
			// string array (of length 1) will work even if the parameter
			// doesn't support more than one value.  So always splitting on 
			// "," will give us uniform handling of single and multi-values, 
			// in that case.  If the parameter type is not string and only 
			// expects a single value, then even if the individual array 
			// elements are convertible to the proper type, it won't work.  
			// You can convert a string to an int, for example, but not a 
			// length 1 array of strings to an int.  I don't know whether a 
			// param that expects multiple non-string values will work with a 
			// string array.  I think you can get type info for a parameter, 
			// but I couldn't handle any old type anyway.  I hope I can always
			// provide strings, and let their library handle conversions.  I 
			// just need to be careful to not use an array if the value doesn't
			// contain a comma.
			int i = 0;
			for (StringVector::const_iterator paramIter = paramNames.begin();
				paramIter != paramNames.end(); ++paramIter, ++i) {
				if (paramCombo[i].empty())
					ps->AddParameter(marshal_as<String^>(*paramIter));
				else {
					if (paramCombo[i].find(',') == string::npos)
						ps->AddParameter(marshal_as<String^>(*paramIter),
							marshal_as<String^>(paramCombo[i]));
					else
						ps->AddParameter(marshal_as<String^>(*paramIter),
							marshal_as<String^>(paramCombo[i])->Split(','));
				}
				cmdline << " -" << *paramIter << ' ' << paramCombo[i];
			}

			if (!selectNames.empty()) {
				ps->AddCommand("Select-Object");
				cmdline << " | Select-Object";
				i = 0;
				for (StringVector::const_iterator selectIter = selectNames.begin();
					selectIter != selectNames.end(); ++selectIter, ++i) {
					if (selectCombo[i].empty())
						ps->AddParameter(marshal_as<String^>(*selectIter));
					else {
						if (selectCombo[i].find(',') == string::npos)
							ps->AddParameter(marshal_as<String^>(*selectIter),
								marshal_as<String^>(selectCombo[i]));
						else
							ps->AddParameter(marshal_as<String^>(*selectIter),
								marshal_as<String^>(selectCombo[i])->Split(','));
					}
					cmdline << " -" << *selectIter << ' ' << selectCombo[i];
				}
			}

			Log::Debug("Invoking PowerShell command: " + cmdline.str());
			Collection<PSObject^> ^results = ps->Invoke();

			if (results != nullptr) {
				for each (PSObject ^result in results)
					AddResultToItem(result, item.get());
			}

		// We handle all errors at this point and continue, since we
		// are at the point here that we know all object entities match
		// the item.
		// I think if the exception is neither System::Exception or
		// ::Exception, it may be serious enough to let it propagate and
		// interrupt collection.
		} catch (System::Exception ^e) {
			item->SetStatus(OvalEnum::STATUS_ERROR);
			item->AppendMessage(new OvalMessage(marshal_as<string>(e->GetType()->FullName)+": "+marshal_as<string>(e->Message), OvalEnum::LEVEL_ERROR));
		} catch (::Exception &e) {
			item->SetStatus(OvalEnum::STATUS_ERROR);
			item->AppendMessage(new OvalMessage(e.GetErrorMessage(), OvalEnum::LEVEL_ERROR));
		} finally {
			// I wanna deterministically destroy this, since I feel it
			// may suck up resources, and we will potentially be running
			// a lot of commands if the definition is large.
			delete ps;
		}

		items->push_back(item.release());
	}

	void AddResultToItem(PSObject ^result, Item *item) {

#ifdef _DEBUG
		ostringstream oss;
#endif
		AbsEntityValueVector fieldVals;
		for each (PSPropertyInfo ^propInfo in result->Properties) {
#ifdef _DEBUG
			oss << marshal_as<string>(propInfo->Name) << " = ";
#endif

			// the value is sometimes retrieved lazily when first
			// accessed, and may cause an exception to be thrown.
			try {
				if (propInfo->Value == nullptr)
					fieldVals.push_back(new ItemFieldEntityValue(marshal_as<string>(propInfo->Name->ToLower()),
						"", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));
				else
					fieldVals.push_back(new ItemFieldEntityValue(marshal_as<string>(propInfo->Name->ToLower()),
						marshal_as<string>(propInfo->Value->ToString()), GetDatatype(propInfo->Value->GetType())));
#ifdef _DEBUG
				oss << (propInfo->Value == nullptr ? "(null)" : 
					marshal_as<string>(propInfo->Value->ToString())) << endl;
#endif
			} catch (System::Management::Automation::RuntimeException ^e) {
				fieldVals.push_back(new ItemFieldEntityValue(marshal_as<string>(propInfo->Name->ToLower()),
					"", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_ERROR));
				item->AppendMessage(new OvalMessage(marshal_as<string>(e->GetType()->Name) + ": " +
					marshal_as<string>(e->Message), OvalEnum::LEVEL_ERROR));
#ifdef _DEBUG
				oss << marshal_as<string>(e->GetType()->FullName) << ": " <<
					marshal_as<string>(e->Message) << endl;
#endif
			}
		}

		item->AppendElement(new ItemEntity("value", fieldVals));

#ifdef _DEBUG
		//Log::Debug(oss.str());
		//Log::Debug("");
#endif
	}

	OvalEnum::Datatype GetDatatype(Type ^type){

		if (type == Boolean::typeid)
			return OvalEnum::DATATYPE_BOOLEAN;

		else if (type == SByte::typeid ||
			type == Byte::typeid ||
			type == Int16::typeid ||
			type == Int32::typeid ||
			type == Int64::typeid ||
			type == UInt16::typeid ||
			type == UInt32::typeid ||
			type == UInt64::typeid ||
			type == Decimal::typeid)
			return OvalEnum::DATATYPE_INTEGER;

		else if (type == Double::typeid ||
			type == Single::typeid)
			return OvalEnum::DATATYPE_FLOAT;

		return OvalEnum::DATATYPE_STRING;
	}
}
