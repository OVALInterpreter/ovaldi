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

#include "CmdletProbe.h"
#include <msclr\marshal.h>
#include <msclr\marshal_cppstd.h>

using namespace System;
using namespace System::Collections;
using namespace System::Collections::ObjectModel;
using namespace System::Management::Automation;
using namespace System::Management::Automation::Runspaces;
using namespace msclr::interop;

//****************************************************************************************//
//								CmdletProbe Class											  //	
//****************************************************************************************//
CmdletProbe* CmdletProbe::instance = NULL;

CmdletProbe::CmdletProbe() : AbsProbe() {
	
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
	string powershellInput = "";
	ItemVector* collectedItems = new ItemVector();
	Item* item = NULL;
	
	if ( object->GetName().compare("cmdlet_1_object") == 0 ){
		ObjectEntity* nounEntity = object->GetElementByName("noun");
		ObjectEntity* argumentsEntity = object->GetElementByName("arguments");
		ObjectEntity* parametersEntity = object->GetElementByName("parameters");
		powershellInput = "get-"+nounEntity->GetValue()+" "+argumentsEntity->GetValue()+" "+parametersEntity->GetValue();
		
		cmdletItem = "cmdlet_1_item";
		item = this->CreateItem();
		item->SetStatus(OvalEnum::STATUS_EXISTS);
		item->AppendElement(new ItemEntity("noun", nounEntity->GetValue(), nounEntity->GetDatatype(), true, OvalEnum::STATUS_EXISTS));
		item->AppendElement(new ItemEntity("arguments", argumentsEntity->GetValue(), argumentsEntity->GetDatatype(), true, (argumentsEntity->GetNil())?OvalEnum::STATUS_NOT_COLLECTED:OvalEnum::STATUS_EXISTS,argumentsEntity->GetNil()));
		item->AppendElement(new ItemEntity("parameters", parametersEntity->GetValue(), parametersEntity->GetDatatype(), true, (parametersEntity->GetNil())?OvalEnum::STATUS_NOT_COLLECTED:OvalEnum::STATUS_EXISTS,parametersEntity->GetNil()));

	}else if ( object->GetName().compare("cmdlet_2_object") == 0 ){
		ObjectEntity* verbEntity = object->GetElementByName("verb");
		ObjectEntity* nounEntity = object->GetElementByName("noun");
		ObjectEntity* argumentsEntity = object->GetElementByName("arguments");
		ObjectEntity* parametersEntity = object->GetElementByName("parameters");
		powershellInput = verbEntity->GetValue()+"-"+nounEntity->GetValue()+" "+argumentsEntity->GetValue()+" "+parametersEntity->GetValue();

		cmdletItem = "cmdlet_2_item";
		item = this->CreateItem();
		item->SetStatus(OvalEnum::STATUS_EXISTS);
		item->AppendElement(new ItemEntity("verb", verbEntity->GetValue(), verbEntity->GetDatatype(), true, OvalEnum::STATUS_EXISTS));
		item->AppendElement(new ItemEntity("noun", nounEntity->GetValue(), nounEntity->GetDatatype(), true, OvalEnum::STATUS_EXISTS));
		item->AppendElement(new ItemEntity("arguments", argumentsEntity->GetValue(), argumentsEntity->GetDatatype(), true, (argumentsEntity->GetNil())?OvalEnum::STATUS_NOT_COLLECTED:OvalEnum::STATUS_EXISTS,argumentsEntity->GetNil()));
		item->AppendElement(new ItemEntity("parameters", parametersEntity->GetValue(), parametersEntity->GetDatatype(), true, (parametersEntity->GetNil())?OvalEnum::STATUS_NOT_COLLECTED:OvalEnum::STATUS_EXISTS,parametersEntity->GetNil()));

	}else if( object->GetName().compare("cmdlet_3_object") == 0 ){
		ObjectEntity* cmdletEntity = object->GetElementByName("cmdlet");
		ObjectEntity* argumentsEntity = object->GetElementByName("arguments");
		ObjectEntity* parametersEntity = object->GetElementByName("parameters");
		powershellInput = cmdletEntity->GetValue()+" "+argumentsEntity->GetValue()+" "+parametersEntity->GetValue();
	
		cmdletItem = "cmdlet_3_item";
		item = this->CreateItem();
		item->SetStatus(OvalEnum::STATUS_EXISTS);
		item->AppendElement(new ItemEntity("cmdlet", cmdletEntity->GetValue(), cmdletEntity->GetDatatype(), true, OvalEnum::STATUS_EXISTS));
		item->AppendElement(new ItemEntity("arguments", argumentsEntity->GetValue(), argumentsEntity->GetDatatype(), true, (argumentsEntity->GetNil())?OvalEnum::STATUS_NOT_COLLECTED:OvalEnum::STATUS_EXISTS,argumentsEntity->GetNil()));
		item->AppendElement(new ItemEntity("parameters", parametersEntity->GetValue(), parametersEntity->GetDatatype(), true, (parametersEntity->GetNil())?OvalEnum::STATUS_NOT_COLLECTED:OvalEnum::STATUS_EXISTS,parametersEntity->GetNil()));
	}

	this->ExecutePowerShell(powershellInput, item);
	collectedItems->push_back(item);
	return collectedItems;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
Item* CmdletProbe::CreateItem() {

	Item* item = new Item(0, 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows", 
						"win-sc", 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows windows-system-characteristics-schema.xsd", 
						OvalEnum::STATUS_ERROR, 
						CmdletProbe::cmdletItem);

	return item;
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
			PSMemberInfoCollection<PSPropertyInfo^> ^objectProperties = psObject->Properties;
			Generic::IEnumerator<PSPropertyInfo^>^ propIterator = objectProperties->GetEnumerator();
			while ( propIterator->MoveNext() ){
				PSPropertyInfo^ psProperty = propIterator->Current;		
				string name = Common::ToLower(marshal_as<string>(psProperty->Name));
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