//
//
//****************************************************************************************//
// Copyright (c) 2002-2011, The MITRE Corporation
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
#include <vector>
#include <msclr\marshal.h>
#include <msclr\marshal_cppstd.h>

#include <VectorPtrGuard.h>
#include <ItemFieldEntityValue.h>
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
#define MANAGED_EXCEPTION_GUARD_END \
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
		 * advance to the next combination.  So in effect, to the
		 * outside world, the illusion is created that instances
		 * of this class are initialized to not contain any
		 * combination.  accept() must be called once to move
		 * to the first combination.
		 */
		bool beforeFirstAdvance;
	};

	/**
	 * The list of legal cmdlet verbs.
	 */
	char const * const LEGAL_VERBS[] = {
		"foreach",
		"get",
		"group",
		"measure",
		"select",
		"where"
	};

	/**
	 * Checks whether the verb part of the given cmdlet name is
	 * legal.  It is legal if it is contained in the white-list
	 * of legal verbs defined by LEGAL_VERBS.  Comparisons are
	 * done case-insensitively.
	 *
	 * \return true if the cmdlet has a legal verb, false if not.
	 */
	bool HasLegalVerb(String ^cmdletName);

	/**
	 * Creates an item.
	 */
	auto_ptr<Item> CreateItem();

	/**
	 * Creates a restricted runspace: all cmdlets which don't satisfy
	 * certain criteria are removed.  This uses a white-list of
	 * verbs, filters out non-cmdlets, and checks the cmdlet modules
	 * for a match with the given module entities.
	 *
	 * \return a runspace, or nullptr if everything was filtered out.
	 */
	Runspace ^MakeRunspace(ObjectEntity *modNameObjEntity,
		ObjectEntity *modIdObjEntity,
		ObjectEntity *modVersionObjEntity);

	Collection<PSObject^> ^InvokeCmdlet(Runspace ^runspace, const string &verb, const string &noun,
		const map<string, StringVector> &params, const map<string, StringVector> &selects,
		const StringVector &paramCombo, const StringVector &selectCombo);

	void AddItemsFromCmdletResults(Collection<PSObject^> ^results, 
		ItemVector *items, ObjectEntity *modNameObjEntity, 
		ObjectEntity *modIdObjEntity, ObjectEntity *modVersionObjEntity,
		const string &verb, const string &noun);

	auto_ptr<Item> MakeItemFromCmdletResult(PSObject ^result,
		ObjectEntity *modNameObjEntity, ObjectEntity *modIdObjEntity,
		ObjectEntity *modVersionObjEntity, const string &verb, 
		const string &noun);
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
	auto_ptr<Item> itemTemplate;

	ObjectEntity
		*modNameObjEntity,
		*modIdObjEntity,
		*modVersionObjEntity,
		*verbObjEntity,
		*nounObjEntity,
		*paramsObjEntity,
		*selectObjEntity;

	modNameObjEntity = object->GetElementByName("module_name");
	modIdObjEntity = object->GetElementByName("module_id");
	modVersionObjEntity = object->GetElementByName("module_version");
	verbObjEntity = object->GetElementByName("verb");
	nounObjEntity = object->GetElementByName("noun");
	paramsObjEntity = object->GetElementByName("parameters");
	selectObjEntity = object->GetElementByName("select");

	StringVector verbs, nouns;
	map<string, StringVector> params, selects;

	/*OvalEnum::Flag flag =*/ verbObjEntity->GetEntityValues(verbs);
	/*OvalEnum::Flag flag =*/ nounObjEntity->GetEntityValues(nouns);
	if (!paramsObjEntity->GetNil())
		/*OvalEnum::Flag flag =*/ paramsObjEntity->GetEntityValues(params);
	if (!selectObjEntity->GetNil())
		/*OvalEnum::Flag flag =*/ selectObjEntity->GetEntityValues(selects);

	Runspace ^runspace = MakeRunspace(modNameObjEntity, modIdObjEntity,
		modVersionObjEntity);

	// we match the module entities by filtering disallowed cmdlets
	// from the runspace.  If that filtered out everything, then no
	// cmdlets match, so we return the empty vector.
	if (runspace == nullptr) {
		Log::Debug("No cmdlets matched!");
		return collectedItems.release();
	}

	// create our vectors of vectors of params and selects, so we can
	// enumerate all the combinations.  We'll have to iterate over all
	// combinations once per verb+noun combination.
	vector<const StringVector*> allParams;
	for (map<string, StringVector>::iterator iter = params.begin();
		iter != params.end();
		++iter)
		allParams.push_back(&iter->second);

	vector<const StringVector*> allSelects;
	for (map<string, StringVector>::iterator iter = selects.begin();
		iter != selects.end();
		++iter)
		allSelects.push_back(&iter->second);

	for (StringVector::iterator verbIter = verbs.begin();
		verbIter != verbs.end();
		++verbIter) {

		for (StringVector::iterator nounIter = nouns.begin();
			nounIter != nouns.end();
			++nounIter) {

			// params and select are both nillable... so there are
			// 4 cases: both are nil, neither, or exactly one or the other.
			StringVector emptyPh; // used for combo when params or select is nil'd
			Collection<PSObject^> ^results;
			if (!params.empty() && !selects.empty()) {
				ContainerValueCombinations<StringVector> paramCombos(allParams);
				while (paramCombos.advance()) {
					ContainerValueCombinations<StringVector> selectCombos(allSelects);
					while (selectCombos.advance()) {
						results = InvokeCmdlet(runspace, *verbIter, *nounIter, params, selects, 
								paramCombos.combo(), selectCombos.combo());
					}
				}
			} else if (!params.empty()) {
				ContainerValueCombinations<StringVector> paramCombos(allParams);
				while (paramCombos.advance()) {
					results = InvokeCmdlet(runspace, *verbIter, *nounIter, params, selects, 
							paramCombos.combo(), emptyPh);
				}
			} else if (!selects.empty()) {
				ContainerValueCombinations<StringVector> selectCombos(allSelects);
				while (selectCombos.advance()) {
					results = InvokeCmdlet(runspace, *verbIter, *nounIter, params, selects, 
							emptyPh, selectCombos.combo());
				}
			} else {
				// both params and select are nil
				results = InvokeCmdlet(runspace, *verbIter, *nounIter, params, selects, 
						emptyPh, emptyPh);
			}

			AddItemsFromCmdletResults(results, collectedItems.get(), 
				modNameObjEntity, modIdObjEntity, modVersionObjEntity,
				*verbIter, *nounIter);
		}
	}

//	item = this->CreateItem();
//	item->SetStatus(OvalEnum::STATUS_EXISTS);
//	item->AppendElement(new ItemEntity("verb", verbObjEntity->GetValue(), verbObjEntity->GetDatatype(), true, OvalEnum::STATUS_EXISTS));
//	item->AppendElement(new ItemEntity("noun", nounObjEntity->GetValue(), nounObjEntity->GetDatatype(), true, OvalEnum::STATUS_EXISTS));
//	item->AppendElement(new ItemEntity("arguments", argumentsEntity->GetValue(), argumentsEntity->GetDatatype(), true, (argumentsEntity->GetNil())?OvalEnum::STATUS_NOT_COLLECTED:OvalEnum::STATUS_EXISTS,argumentsEntity->GetNil()));
//	item->AppendElement(new ItemEntity("parameters", parametersEntity->GetValue(), parametersEntity->GetDatatype(), true, (parametersEntity->GetNil())?OvalEnum::STATUS_NOT_COLLECTED:OvalEnum::STATUS_EXISTS,parametersEntity->GetNil()));

//	collectedItems->push_back(item);
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

void CmdletProbe::ExecutePowerShell(string powershellInput, Item* item){
	AbsEntityValueVector fieldEntityValues;
	PowerShell^ ps = PowerShell::Create();
	InitialSessionState^ restrictedSessionState = this->InitializeRunspace();		
	ps->Runspace = Runspaces::RunspaceFactory::CreateRunspace(restrictedSessionState);
	ps->Runspace->Open();
	ps->AddScript(marshal_as<String^>(powershellInput));
		
	try{
		Collection<PSObject^> psObjects = ps->Invoke();
		if ( psObjects.Count == 0 ){
			item->AppendElement(new ItemEntity("value",fieldEntityValues,OvalEnum::DATATYPE_RECORD,false,OvalEnum::STATUS_DOES_NOT_EXIST));
		}
		for each( PSObject^ psObject in psObjects ){
			PSMemberInfoCollection<PSPropertyInfo^> ^objectProperties = psObject->Properties;
			Generic::IEnumerator<PSPropertyInfo^>^ propIterator = objectProperties->GetEnumerator();
			while ( propIterator->MoveNext() ){
				PSPropertyInfo^ psProperty = propIterator->Current;		
				string name = Common::ToLower(marshal_as<string>(psProperty->Name));
				try{
					if ( psProperty->IsGettable ){
						if ( psProperty->Value != nullptr ){
							OvalEnum::Datatype datatype = this->GetDatatype(marshal_as<string>(psProperty->Value->GetType()->ToString()));
							try{
								fieldEntityValues.push_back(new ItemFieldEntityValue(name, (datatype == OvalEnum::DATATYPE_BOOLEAN)?Common::ToLower(marshal_as<string>(psProperty->Value->ToString())):marshal_as<string>(psProperty->Value->ToString()), datatype, OvalEnum::STATUS_EXISTS));                   								
							}catch(System::Exception^ e){
								item->AppendMessage(new OvalMessage(marshal_as<string>(e->ToString()),OvalEnum::LEVEL_ERROR));
								fieldEntityValues.push_back(new ItemFieldEntityValue(name, "", datatype, OvalEnum::STATUS_ERROR));  	
							}
						}else{
							fieldEntityValues.push_back(new ItemFieldEntityValue(name, "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));                                 
						}
					}
				}catch(System::Exception^ e){
					item->AppendMessage(new OvalMessage(marshal_as<string>(e->ToString()),OvalEnum::LEVEL_ERROR));
					fieldEntityValues.push_back(new ItemFieldEntityValue(Common::ToLower(marshal_as<string>(psProperty->Name)), "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_ERROR)); 
				}
			}
			item->AppendElement(new ItemEntity("value", fieldEntityValues, OvalEnum::DATATYPE_RECORD, false, OvalEnum::STATUS_EXISTS));
			fieldEntityValues.clear();
		}
	}catch(System::Exception^ e){
		ps->Runspace->Close();	
		throw ProbeException("Error: There was an error retrieving the results of the cmdlet execution."+marshal_as<string>(e->ToString()));
	}

	ps->Runspace->Close();
}

InitialSessionState^ CmdletProbe::InitializeRunspace(){

	InitialSessionState^ restrictedSessionState = InitialSessionState::CreateDefault();
	PowerShell^ ps = PowerShell::Create();
	ps->Runspace->DefaultRunspace = Runspaces::RunspaceFactory::CreateRunspace();
	ps->Runspace->DefaultRunspace->Open();
	ps->AddScript("get-command | where-object {$_.verb -ne \"foreach\" -and $_.verb -ne \"get\" -and $_.verb -ne \"group\" -and $_.verb -ne \"measure\" -and $_.verb -ne \"select\" -and $_.verb -ne \"where\"} | select-object name");		
	Collection<String^> restrictedCmdlets;
	
	try{
		Collection<PSObject^> psObjects = ps->Invoke();
		for each( PSObject^ psObject in psObjects ){
			for each (PSPropertyInfo ^ psProperty in psObject->Properties ){
				if ( psProperty->IsGettable && psProperty->Value != nullptr ){
					restrictedCmdlets.Add(psProperty->Value->ToString());		
				}
			}
		}
	}catch(System::Exception^ e){
		ps->Runspace->DefaultRunspace->Close();
		throw ProbeException("Error: There was an error initializing the restricted runspace."+marshal_as<string>(e->ToString()));
	}

	for each (String^ cmdlet in restrictedCmdlets){
		int index = 0;
		InitialSessionStateEntryCollection<SessionStateCommandEntry^>^ commands = restrictedSessionState->Commands;		
		for each (SessionStateCommandEntry^ command in commands){
			if (command->Name->Equals(cmdlet, StringComparison::OrdinalIgnoreCase)){
				restrictedSessionState->Commands->RemoveItem(index);
				break;
			}
			index++;
		}
	}

	ps->Runspace->DefaultRunspace->Close();
	return restrictedSessionState;
}

OvalEnum::Datatype CmdletProbe::GetDatatype(std::string type){
	OvalEnum::Datatype datatype = OvalEnum::DATATYPE_STRING;

	if ( type.compare("System.Boolean") == 0 ){

		datatype = OvalEnum::DATATYPE_BOOLEAN;
	
	}else if ( type.compare("System.Byte") == 0 || 
			   type.compare("System.Int16") == 0 ||
			   type.compare("System.Int32") == 0 ||
			   type.compare("System.Int64") == 0 ||
			   type.compare("System.UInt16") == 0 ||
			   type.compare("System.UInt32") == 0 ||
			   type.compare("System.UInt64") == 0 ||
			   type.compare("System.Decimal") == 0 ||
			   type.compare("System.SByte") == 0 ){

		datatype = OvalEnum::DATATYPE_INTEGER;

	}else if ( type.compare("System.Double") == 0 ||
			   type.compare("System.Single") == 0 ){

				 datatype = OvalEnum::DATATYPE_FLOAT;

	}

	return datatype;
}

namespace {

	template<typename _Cont>
	ContainerValueCombinations<_Cont>::ContainerValueCombinations(const vector<const _Cont *> &containers)
		: containers(containers),
			atEndFlag(true), 
			beforeFirstAdvance(true) {

		// initialize iterators to the beginnings of their containers.  Also
		// we can check whether we started out at the end, which happens when
		// all containers are empty.
		iterators.reserve(containers.size());
		for (typename vector<const _Cont *>::iterator contIter = this->containers.begin();
			contIter != this->containers.end();
			++contIter) {
			iterators.push_back((*contIter)->begin());
			if (!(*contIter)->empty())
				atEndFlag = false;
		}
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

	bool HasLegalVerb(String ^cmdletName) {
		const int numVerbs = sizeof(LEGAL_VERBS)/sizeof(LEGAL_VERBS[0]);

		// I'd rather have global managed array of managed strings rather
		// than repeatedly marshaling C-strings, but apparently variables
		// of managed type can't be global!

		for (int i=0; i<numVerbs; ++i) {
			if (cmdletName->ToLower()->StartsWith(marshal_as<String^>(LEGAL_VERBS[i])->ToLower()+"-"))
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

	Runspace ^MakeRunspace(ObjectEntity *modNameObjEntity,
		ObjectEntity *modIdObjEntity,
		ObjectEntity *modVersionObjEntity) {
		//Runspace ^defRunspace = Runspace::DefaultRunspace;
		//InitialSessionState ^defInitState = defRunspace->InitialSessionState;
		/*
		PowerShell^ ps = PowerShell::Create();
		ps->AddScript("get-command | where-object {$_.verb -eq \"foreach\" -or $_.verb -eq \"get\" -or $_.verb -eq \"group\" -or $_.verb -eq \"measure\" -or $_.verb -eq \"select\" -or $_.verb -eq \"where\"} | select-object name");		
		Collection<PSObject^> psObjects = ps->Invoke();
		for each (PSObject ^obj in psObjects) {
			System::Object ^baseObj = obj->BaseObject;
			Log::Info("baseObj is a " + marshal_as<string>(baseObj->GetType()->FullName));
		}
		*/

		InitialSessionState ^iss = InitialSessionState::CreateDefault();
		// other setup, copied from Microsoft's sample code
		iss->LanguageMode = PSLanguageMode::NoLanguage;
		iss->ThrowOnRunspaceOpenError = true;
		iss->Formats->Clear();

		Log::Debug("Default InitialSessionState has " + Common::ToString(iss->Commands->Count)+" items");	

		ItemEntity modNameItemEntity("","",OvalEnum::DATATYPE_STRING);
		ItemEntity modIdItemEntity("","",OvalEnum::DATATYPE_STRING);
		ItemEntity modVersionItemEntity("","",OvalEnum::DATATYPE_VERSION);
		bool atLeastOneVisibleCmdlet = false;
		for (int i=0; i<iss->Commands->Count; ++i) {
			SessionStateCommandEntry ^cmd = iss->Commands[i];

			// special case: "select" is one of the entities, so that has to be
			// allowed.  I'm trying to make sure the cmdlet I'm allowing is the
			// right one, not just any one that is named "Select-Object".
			// I get all cmdlets with null modules, including Select-Object,
			// so I hope this restricts it appropriately.
			if (cmd->Name->Equals("Select-Object") && 
				cmd->CommandType == CommandTypes::Cmdlet &&
				cmd->Module == nullptr)
				continue;

			if (cmd->Module != nullptr) {
				modNameItemEntity.SetValue(marshal_as<string>(cmd->Module->Name));
				modIdItemEntity.SetValue("{"+marshal_as<string>(cmd->Module->Guid.ToString())+"}");
				modVersionItemEntity.SetValue(marshal_as<string>(cmd->Module->Version->ToString()));
			}

			// filter out the command if:
			// - it's not a cmdlet
			// -or- there is no module, and any of the module-related entities
			//      are not nil
			// -or- there is a module, and any of non-nil the module-related
			//      entities don't match
			// -or- the verb is not allowed
			//
			// where "filter out" means just hiding it.  This allows legal
			// cmdlets to call them as part of their implementation.
			if (cmd->CommandType != CommandTypes::Cmdlet ||
				(cmd->Module == nullptr &&
					(!modNameObjEntity->GetNil() ||
					!modIdObjEntity->GetNil() ||
					!modVersionObjEntity->GetNil())) ||
				(cmd->Module != nullptr &&
					((!modNameObjEntity->GetNil() && modNameObjEntity->Analyze(&modNameItemEntity) != OvalEnum::RESULT_TRUE) ||
					(!modIdObjEntity->GetNil() && modIdObjEntity->Analyze(&modIdItemEntity) != OvalEnum::RESULT_TRUE) ||
					(!modVersionObjEntity->GetNil() && modVersionObjEntity->Analyze(&modVersionItemEntity) != OvalEnum::RESULT_TRUE))) ||
				!HasLegalVerb(cmd->Name))
				cmd->Visibility = SessionStateEntryVisibility::Private;
			else {
				atLeastOneVisibleCmdlet = true;
				Log::Debug(marshal_as<string>(cmd->Name)+" passed");
			}
		}

		// don't bother creating the runspace if we can't find any legal
		// commands which satisfy the module requirements.
		if (!atLeastOneVisibleCmdlet)
			return nullptr;

		Runspace ^runspace = RunspaceFactory::CreateRunspace(iss);
		// copied from MS sample code
		runspace->ThreadOptions = PSThreadOptions::UseCurrentThread;
		
		runspace->Open();
		return runspace;
	}

	Collection<PSObject^> ^InvokeCmdlet(Runspace ^runspace, const string &verb, const string &noun,
		const map<string, StringVector> &params, const map<string, StringVector> &selects,
		const StringVector &paramCombo, const StringVector &selectCombo) {

		ostringstream cmdline; // for debugging

		if (verb == "Select" && noun == "Object")
			throw ProbeException("Illegal cmdlet: Select-Object.  This is only usable indirectly via the select entity!");

		PowerShell ^ps = PowerShell::Create();
		try {
			ps->Runspace = runspace;

			ps->AddCommand(marshal_as<String^>(verb + "-" + noun));
			cmdline << verb << '-' << noun;

			int i=0;
			for (map<string, StringVector>::const_iterator paramIter = params.begin();
				paramIter != params.end(); ++paramIter, ++i) {
				ps->AddParameter(marshal_as<String^>(paramIter->first),
					marshal_as<String^>(paramCombo[i]));
				cmdline << " -" << paramIter->first << ' ' << paramCombo[i];
			}

			if (!selects.empty()) {
				ps->AddCommand("Select-Object");
				cmdline << " | Select-Object";
				i = 0;
				for (map<string, StringVector>::const_iterator selectIter = selects.begin();
					selectIter != selects.end(); ++selectIter, ++i) {
					ps->AddParameter(marshal_as<String^>(selectIter->first),
						marshal_as<String^>(selectCombo[i]));
					cmdline << " -" << selectIter->first << ' ' << selectCombo[i];
				}
			}

			Log::Debug("Invoking PowerShell command: " + cmdline.str());
			return ps->Invoke();
		} finally {
			// I wanna deterministically destroy this, since I feel it
			// may suck up resources, and we will potentially be running
			// a lot of commands if the definition is large.
			delete ps;
		}
	}

	void AddItemsFromCmdletResults(Collection<PSObject^> ^results, 
		ItemVector *items, ObjectEntity *modNameObjEntity, 
		ObjectEntity *modIdObjEntity, ObjectEntity *modVersionObjEntity,
		const string &verb, const string &noun) {

		for each (PSObject ^result in results) {
			auto_ptr<Item> item = MakeItemFromCmdletResult(result,
				modNameObjEntity, modIdObjEntity, modVersionObjEntity,
				verb, noun);
			items->push_back(item.release());
		}
	}

	auto_ptr<Item> MakeItemFromCmdletResult(PSObject ^result,
		ObjectEntity *modNameObjEntity, ObjectEntity *modIdObjEntity,
		ObjectEntity *modVersionObjEntity, const string &verb, 
		const string &noun) {

		auto_ptr<Item> item = CreateItem();
		item->SetStatus(OvalEnum::STATUS_EXISTS);

// i hate repetition :-P  This creates module_* item entities
// which correspond to the module_* object entities.
#define MAKE_MODULE_ITEM_ENTITY(nameInVar, nameInEnt, entType) \
		ItemEntity *mod##nameInVar##ItemEntity = new ItemEntity("module_" #nameInEnt, \
			"", OvalEnum::DATATYPE_##entType, true, OvalEnum::STATUS_EXISTS, \
			mod##nameInVar##ObjEntity->GetNil()); \
		if (!mod##nameInVar##ObjEntity->GetNil()) \
			mod##nameInVar##ItemEntity->SetValue(mod##nameInVar##ObjEntity->GetValue()); \
		item->AppendElement(mod##nameInVar##ItemEntity);

		MAKE_MODULE_ITEM_ENTITY(Name, name, STRING)
		MAKE_MODULE_ITEM_ENTITY(Id, id, STRING)
		MAKE_MODULE_ITEM_ENTITY(Version, version, VERSION)

#undef MAKE_MODULE_ITEM_ENTITY

		item->AppendElement(new ItemEntity("verb", verb, OvalEnum::DATATYPE_STRING,
			true, OvalEnum::STATUS_EXISTS));
		item->AppendElement(new ItemEntity("noun", noun, OvalEnum::DATATYPE_STRING,
			true, OvalEnum::STATUS_EXISTS));

		ostringstream oss;
		AbsEntityValueVector fieldVals;
		for each (PSPropertyInfo ^propInfo in result->Properties) {			
			oss << marshal_as<string>(propInfo->Name) << " = ";

			// the value is sometimes retrieved lazily when first
			// accessed, and may cause an exception to be thrown.
			try {
				if (propInfo->Value == nullptr)
					fieldVals.push_back(new ItemFieldEntityValue(marshal_as<string>(propInfo->Name),
						"", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));
				else
					// TEMPORARY
					fieldVals.push_back(new ItemFieldEntityValue(marshal_as<string>(propInfo->Name),
						marshal_as<string>(propInfo->Value->ToString()), OvalEnum::DATATYPE_STRING));

				oss << (propInfo->Value == nullptr ? "(null)" : 
					marshal_as<string>(propInfo->Value->ToString())) << endl;

			} catch (System::Management::Automation::RuntimeException ^e) {
				fieldVals.push_back(new ItemFieldEntityValue(marshal_as<string>(propInfo->Name),
					"", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_ERROR));
				item->AppendMessage(new OvalMessage(marshal_as<string>(e->GetType()->Name) + ": " +
					marshal_as<string>(e->Message), OvalEnum::LEVEL_ERROR));
				oss << marshal_as<string>(e->GetType()->Name) << ": " <<
					marshal_as<string>(e->Message) << endl;
			}
		}
		item->AppendElement(new ItemEntity("value", fieldVals));

		Log::Debug(oss.str());
		Log::Debug("");

		return item;
	}
}
