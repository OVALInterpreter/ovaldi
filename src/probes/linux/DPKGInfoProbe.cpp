//
// $Id: DPKGInfoProbe.cpp 4194 2007-07-10 15:33:35Z blaze $
//
//****************************************************************************************//
// Copyright (c) 2002-2008, The MITRE Corporation
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

#include "DPKGInfoProbe.h"

//****************************************************************************************//
//				DPKGInfoProbe Class					  //	
//****************************************************************************************//
DPKGInfoProbe *DPKGInfoProbe::instance = NULL;

DPKGInfoProbe::DPKGInfoProbe() {
	this->StatusFile = "/var/lib/dpkg/status";
}

DPKGInfoProbe::~DPKGInfoProbe() {

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* DPKGInfoProbe::Instance() {

	// Use lazy initialization
	if(instance == NULL) 
		instance = new DPKGInfoProbe();

	return instance;	
}

//bad
ItemVector* DPKGInfoProbe::CollectItems(Object* object) {

	ItemVector *collectedItems = new ItemVector();

	ObjectEntity* name = object->GetElementByName("name");

	// check datatypes - only allow string
	if(name->GetDatatype() != OvalEnum::DATATYPE_STRING) {
		throw ProbeException("Error: invalid data type specified on name. Found: " + OvalEnum::DatatypeToString(name->GetDatatype()));
	}

	// check operation - only allow  equals, not equals and pattern match
	if(name->GetOperation() != OvalEnum::OPERATION_EQUALS && name->GetOperation() != OvalEnum::OPERATION_PATTERN_MATCH && name->GetOperation() != OvalEnum::OPERATION_NOT_EQUAL) {
		throw ProbeException("Error: invalid operation specified on name. Found: " + OvalEnum::OperationToString(name->GetOperation()));
	}

	StringVector* names = this->GetDPKGNames(name);
	if(names->size() > 0) {
		StringVector::iterator iterator;
		for(iterator = names->begin(); iterator != names->end(); iterator++) {		
			this->GetDPKGInfo((*iterator), collectedItems);
		}
	} else {

		if(name->GetOperation() == OvalEnum::OPERATION_EQUALS) {

			if(name->GetVarRef() == NULL) {

				Item* item = this->CreateItem();
				item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
				item->AppendElement(new ItemEntity("name",  name->GetValue(), OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_DOES_NOT_EXIST));
				collectedItems->push_back(item);

			} else {

				VariableValueVector::iterator iterator;
				for(iterator = name->GetVarRef()->GetValues()->begin(); iterator != name->GetVarRef()->GetValues()->end(); iterator++) {

					Item* item = this->CreateItem();
					item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
					item->AppendElement(new ItemEntity("name",  (*iterator)->GetValue(), OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_DOES_NOT_EXIST));
					collectedItems->push_back(item);
				}
			}
		}
	}
	names->clear();
	delete names;

	return collectedItems;
}  

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
Item* DPKGInfoProbe::CreateItem() {
	// -----------------------------------------------------------------------
	//
	//  ABSTRACT
	//
	//  Return a new Item created for storing file information
	//
	// -----------------------------------------------------------------------

	Item* item = new Item(0, 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#linux", 
						"linux-sc", 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#linux linux-system-characteristics-schema.xsd", 
						OvalEnum::STATUS_ERROR, 
						"dpkginfo_item");

	return item;
}

StringVector* DPKGInfoProbe::GetDPKGNames(ObjectEntity* name) {
	// -----------------------------------------------------------------------
	//
	//  ABSTRACT
	//
	//  Get the set of all rpm names on the system that match the object
	//
	// -----------------------------------------------------------------------
	StringVector* names = NULL;

	// does this name use variables?
	if(name->GetVarRef() == NULL) {
		
		// proceed based on operation
		if(name->GetOperation() == OvalEnum::OPERATION_EQUALS) {
			names = new StringVector();
			// if the name exists add it to the list 
			if(this->DPKGExists(name->GetValue())) {
				names->push_back(name->GetValue());
			}

		} else if(name->GetOperation() == OvalEnum::OPERATION_NOT_EQUAL) {
			
			names = this->GetMatchingDPKGNames(name->GetValue(), false);

		} else if(name->GetOperation() == OvalEnum::OPERATION_PATTERN_MATCH) {
			names = this->GetMatchingDPKGNames(name->GetValue(), true);
		}		

	} else {

		names = new StringVector();

		// Get all names
		StringVector allNames;

		if(name->GetOperation() == OvalEnum::OPERATION_EQUALS) {
			// in the case of equals simply loop through all the 
			// variable values and add them to the set of all names
			// if they exist on the system
			VariableValueVector::iterator iterator;
			for(iterator = name->GetVarRef()->GetValues()->begin(); iterator != name->GetVarRef()->GetValues()->end(); iterator++) {
				
				if(this->DPKGExists((*iterator)->GetValue())) {
					allNames.push_back((*iterator)->GetValue());
				}
			}

		} else {
            		this->GetMatchingDPKGNames(".*", &allNames);
		}
	
		// loop through all names on the system
		// only keep names that match operation and value and var check
		ItemEntity* tmp = this->CreateItemEntity(name);
		StringVector::iterator it;
		for(it = allNames.begin(); it != allNames.end(); it++) {
			tmp->SetValue((*it));
			
			if(name->Analyze(tmp) == OvalEnum::RESULT_TRUE) {
				names->push_back((*it));
			}
		}
	}

	return names;
}

//bad
StringVector* DPKGInfoProbe::GetMatchingDPKGNames(string pattern, bool isRegex) {
	// -----------------------------------------------------------------------
	//
	//  ABSTRACT
	//
	//  Get the set of all rpm names on the system that match the pattern
	//
	// -----------------------------------------------------------------------
	
	string installed_dpkg_name;
	string installed_dpkg_status;

	StringVector* names = new StringVector ();

	FileFd Fd(this->StatusFile, FileFd::ReadOnly);
	pkgTagFile Tags(&Fd);

	if (_error->PendingError() == true)
		throw ProbeException("Error: (DPKGInfoProbe) Could not read DPKG status file, which is necessary to read the packages status.");
   
	pkgTagSection Section;
		
	while (Tags.Step(Section) == true)
   	{
		/* Fetch the name, auto-detecting if this is a source file or a package file */
		installed_dpkg_name = readHeaderString (Section, "Package");
      		installed_dpkg_status = readHeaderString (Section, "Status");
		if ( installed_dpkg_status.compare ("install ok installed") == 0 ) 
		{
			if (this->IsMatch (pattern, installed_dpkg_name, isRegex))
				names->push_back (installed_dpkg_name);
		}
   	}
	
   	if (_error->PendingError() == true)
   		throw ProbeException("Error: (DPKGInfoProbe) Error while walking DPKG database.");

	return names;
}

bool DPKGInfoProbe::DPKGExists(string name) {
	// -----------------------------------------------------------------------
	//
	//  ABSTRACT
	//
	//  return true if the specifeid deb exists
	//
	// -----------------------------------------------------------------------
	bool exist = false;
	bool found = false;
	
	FileFd Fd(this->StatusFile, FileFd::ReadOnly);
	pkgTagFile Tags(&Fd);

	if (_error->PendingError() == true) {
		throw ProbeException("Error: (DPKGInfoProbe) Could not read DPKG status file, which is necessary to read the DPKG database.");
	}
   
	pkgTagSection Section;
	
	while (Tags.Step(Section) == true && found == false)
   	{
    		/* Fetch the name, auto-detecting if this is a source file or a package file */
      		string installed_dpkg_name = readHeaderString (Section, "Package");
      		string installed_dpkg_status = readHeaderString (Section, "Status");
      		
      		if (installed_dpkg_name.empty() == true)
      			throw ProbeException("Error: (DPKGInfoProbe) Error while walking DPKG database.");
      
      		if ( installed_dpkg_name.compare (name) == 0 ) 
      		{
      			if (installed_dpkg_status.compare ("install ok installed") == 0 ) 
      			{
      				exist = true;
	      		}	
	      		found = true; 
      		}
		}
	
   	if (_error->PendingError() == true)
   		throw ProbeException("Error: (DPKGInfoProbe) Error while walking DPKG database.");
   	
   	return exist;
}

//bad
void DPKGInfoProbe::GetDPKGInfo(string name, ItemVector* items) {
  //------------------------------------------------------------------------------------//
  //
  //  ABSTRACT
  //
  //  Get the data for all packages that have the name that matches dpkg_name.  
  //
  //------------------------------------------------------------------------------------//

  /* Get the dpkg_name form the data object. */
  const char *dpkg_name = name.c_str();

  /* Create a tmp data object reference. */
  Item *item = NULL;

  /* Epoch, version, release and architecture data for output. */
  string installed_epoch, installed_version, installed_release,installed_architecture, installed_evr;
  string installedEpochEvr;

    /* Header object for the installed package. */
  pkgTagSection section;

  /* Read in the DPKG status files */
  FileFd Fd(this->StatusFile, FileFd::ReadOnly);
  pkgTagFile Tags(&Fd);

  if (_error->PendingError() == true) {
	  throw ProbeException("Error: (DPKGInfoProbe) Could not read DPKG status file, which is necessary to read the DPKG database.");
  }

  while (Tags.Step(section) == true)
  {
	  /* Fetch the name, auto-detecting if this is a source file or a package file */
	  string installed_dpkg_name = readHeaderString (section, "Package");
	  string installed_dpkg_status = readHeaderString (section, "Status");
    		
	  if ( installed_dpkg_name.compare (dpkg_name) == 0 ) 
	  {
		  if (installed_dpkg_status.compare ("install ok installed") == 0 ) 
		  {
			  /* epoch is an int_32 -- we'll display a string to handle the None case well. */
			  /*    char intermediate_string[11];
			      int_32 epoch = readHeaderInt32(header, RPMTAG_EPOCH);
			  	string installedEpochEvr;
			  	if (epoch == -1 ) {
			        installed_epoch = "(none)";
			  		installedEpochEvr = "0";
			  	} else {
			        snprintf(intermediate_string,11,"%d",epoch);
			        installed_epoch = intermediate_string;
			  	  installedEpochEvr = installed_epoch;
			      }
			*/
			  	//installed_epoch = "(none)";
				installed_epoch = "0";
			  	installedEpochEvr = installed_epoch;
			  /* the remaining arguments are all normal strings */
				string version = this->readHeaderString(section, "Version");
				string::size_type find = version.rfind('-');
				if (find != string::npos) {
					installed_version = version.substr(0, find);
					installed_release = version.substr(find+1, version.length());
				} else {
					installed_version = version;
					installed_release = "0";
				}
			    	installed_architecture = this->readHeaderString(section, "Architecture");
				installed_evr = installedEpochEvr + ":" + installed_version + "-" + installed_release;

			    /* Put the data in a data object. */
			    item = this->CreateItem();
			    item->SetStatus(OvalEnum::STATUS_EXISTS);
				item->AppendElement(new ItemEntity("name",  name, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
				item->AppendElement(new ItemEntity("arch",  installed_architecture, OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));
				item->AppendElement(new ItemEntity("epoch",  installed_epoch, OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));
				item->AppendElement(new ItemEntity("release",  installed_release, OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));
				item->AppendElement(new ItemEntity("version",  installed_version, OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));
				item->AppendElement(new ItemEntity("evr",  installed_evr, OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));

			    /* add the new item to the vector. */
			    items->push_back(item);
		  }
	  }
  }
}

string DPKGInfoProbe::readHeaderString(pkgTagSection Section, char* sectionName) {
	string value;

	value = Section.FindS(sectionName);

	return (value);
}
