//
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

#include "RPMInfoProbe.h"

//****************************************************************************************//
//								RPMInfoProbe Class										  //	
//****************************************************************************************//
RPMInfoProbe *RPMInfoProbe::instance = NULL;

RPMInfoProbe::RPMInfoProbe() {

}

RPMInfoProbe::~RPMInfoProbe() {
  instance = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* RPMInfoProbe::Instance() {

	// Use lazy initialization
	if(instance == NULL) 
		instance = new RPMInfoProbe();

	return instance;	
}

ItemVector* RPMInfoProbe::CollectItems(Object* object) {

	ObjectEntity* name = object->GetElementByName("name");

	// check datatypes - only allow string
	if(name->GetDatatype() != OvalEnum::DATATYPE_STRING) {
		throw ProbeException("Error: invalid data type specified on name. Found: " + OvalEnum::DatatypeToString(name->GetDatatype()));
	}

	// check operation - only allow  equals, not equals and pattern match
	if(name->GetOperation() != OvalEnum::OPERATION_EQUALS && name->GetOperation() != OvalEnum::OPERATION_PATTERN_MATCH && name->GetOperation() != OvalEnum::OPERATION_NOT_EQUAL) {
		throw ProbeException("Error: invalid operation specified on name. Found: " + OvalEnum::OperationToString(name->GetOperation()));
	}

	ItemVector *collectedItems = new ItemVector();

	StringVector* names = this->GetRPMNames(name);
	if(names->size() > 0) {
		StringVector::iterator iterator;
		for(iterator = names->begin(); iterator != names->end(); iterator++) {		
			this->GetRPMInfo((*iterator), collectedItems);
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
Item* RPMInfoProbe::CreateItem() {
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
						"rpminfo_item");

	return item;
}

StringVector* RPMInfoProbe::GetRPMNames(ObjectEntity* name) {
	// -----------------------------------------------------------------------
	//
	//  ABSTRACT
	//
	//  Get the set of all rpm names on the system taht match the object
	//
	// -----------------------------------------------------------------------
	StringVector* names = NULL;

	// does this name use variables?
	if(name->GetVarRef() == NULL) {
		
		// proceed based on operation
		if(name->GetOperation() == OvalEnum::OPERATION_EQUALS) {
			names = new StringVector();
			// if the name exists add it to the list 
			if(this->RPMExists(name->GetValue())) {
				names->push_back(name->GetValue());
			}

		} else if(name->GetOperation() == OvalEnum::OPERATION_NOT_EQUAL) {
			
			names = this->GetMatchingRPMNames(name->GetValue(), false);

		} else if(name->GetOperation() == OvalEnum::OPERATION_PATTERN_MATCH) {
			names = this->GetMatchingRPMNames(name->GetValue(), true);
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
				
				if(this->RPMExists((*iterator)->GetValue())) {
					allNames.push_back((*iterator)->GetValue());
				}
			}

		} else {
            this->GetMatchingRPMNames(".*", &allNames);
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

StringVector* RPMInfoProbe::GetMatchingRPMNames(string pattern, bool isRegex) {
	// -----------------------------------------------------------------------
	//
	//  ABSTRACT
	//
	//  Get the set of all rpm names on the system that match the pattern
	//
	// -----------------------------------------------------------------------

	/* Transaction sets are the modern way to read the RPM database. */
	rpmts ts;
	/* We use an iterator to walk the RPM database. */
	rpmdbMatchIterator iterator;
	/* Header object for the installed package. */
	Header header;

	string installed_rpm_name;

	/* Read in the RPM config files */
	if (rpmReadConfigFiles( (const char*) NULL, (const char*) NULL)) 
		throw ProbeException("Error: (RPMInfoProbe) Could not read RPM config files, which is necessary to read the RPM database.");

	/* Create an rpm database transaction set. */
	ts = rpmtsCreate();

	/* Create an iterator to walk the database. */	
	iterator = rpmtsInitIterator(ts, RPMTAG_NAME, NULL, 0);
	if (iterator == NULL)
		throw ProbeException("Error: (RPMInfoProbe) Could not create an iterator to walk the RPM database.");
	
	/* Look at each installed package matching this name.  Generally, there is only one.*/
	StringVector* names = new StringVector();
	while ( (header = rpmdbNextIterator(iterator)) != NULL) {
		/* Get the rpm_name value for comparision. */
		installed_rpm_name = readHeaderString(header, RPMTAG_NAME);

		/* Check to see if name found matches input pattern. */
		if(this->IsMatch(pattern, installed_rpm_name, isRegex)) {
			names->push_back(installed_rpm_name);
		}
	}

	/* Free the iterator and transaction set data structures. */
	rpmdbFreeIterator(iterator);
	rpmtsFree(ts);

	return names;
}


bool RPMInfoProbe::RPMExists(string name) {
	// -----------------------------------------------------------------------
	//
	//  ABSTRACT
	//
	//  return true if the specifeid rpm exists
	//
	// -----------------------------------------------------------------------
	bool exists = false;

	/* Transaction sets are the modern way to read the RPM database. */
	rpmts ts;
	/* We use an iterator to walk the RPM database. */
	rpmdbMatchIterator iterator;
	/* Header object for the installed package. */
	Header header;

	string installed_rpm_name;

	/* Read in the RPM config files */
	if (rpmReadConfigFiles( (const char*) NULL, (const char*) NULL)) 
		throw ProbeException("Error: (RPMInfoProbe) Could not read RPM config files, which is necessary to read the RPM database.");

	/* Create an rpm database transaction set. */
	ts = rpmtsCreate();

	/* Create an iterator to walk the database. */	
	iterator = rpmtsInitIterator(ts, RPMTAG_NAME, NULL, 0);
	if (iterator == NULL)
		throw ProbeException("Error: (RPMInfoProbe) Could not create an iterator to walk the RPM database.");
	
	/* Look at each installed package matching this name.  Generally, there is only one.*/
	while ( (header = rpmdbNextIterator(iterator)) != NULL) {
		/* Get the rpm_name value for comparision. */
		installed_rpm_name = readHeaderString(header, RPMTAG_NAME);

		/* Check to see if name found matches input pattern. */
		if(name.compare(installed_rpm_name) == 0) {
			exists = true;
			break;
		}
	}

	/* Free the iterator and transaction set data structures. */
	rpmdbFreeIterator(iterator);
	rpmtsFree(ts);

	return exists;
}

void RPMInfoProbe::GetRPMInfo(string name, ItemVector* items) {
  //------------------------------------------------------------------------------------//
  //
  //  ABSTRACT
  //
  //  Get the data for all packages that have the name that matches rpm_name.  
  //
  //------------------------------------------------------------------------------------//

  /* Get the rpm_name form the data object. */
  const char *rpm_name = name.c_str();

  /* Create a tmp data object reference. */
  Item *item = NULL;
  
  /* Transaction sets are the modern way to read the RPM database. */
  rpmts ts;
  /* We use an iterator to walk the RPM database. */
  rpmdbMatchIterator iterator;
  /* Header object for the installed package. */
  Header header;
  /* Epoch, version, release and architecture data for output. */
  string installed_epoch, installed_version, installed_release,installed_architecture, installed_evr, installed_signature_keyid;

  /* Read in the RPM config files */
  if (rpmReadConfigFiles( (const char*) NULL, (const char*) NULL)) 
	  throw ProbeException("Error: (RPMInfoProbe) Could not read RPM config files, which is necessary to read the RPM database.");

  /* Create an rpm database transaction set. */
  ts = rpmtsCreate();

  /* Create an iterator to walk the database. */	
  iterator = rpmtsInitIterator(ts, RPMTAG_NAME, rpm_name, 0);
  if (iterator == NULL)
    throw ProbeException("Error: (RPMInfoProbe) Could not create an iterator to walk the RPM database.");

  /* Look at each installed package matching this name.  Generally, there is only one.*/
  while ( (header = rpmdbNextIterator(iterator)) != NULL) {

    /* epoch is an int_32 -- we'll display a string to handle the None case well. */
    char intermediate_string[11];
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

    /* the remaining arguments are all normal strings */
    installed_version = this->readHeaderString(header, RPMTAG_VERSION);
    installed_release = this->readHeaderString(header, RPMTAG_RELEASE);
    installed_architecture = this->readHeaderString(header, RPMTAG_ARCH);
	installed_evr = installedEpochEvr + ":" + installed_version + "-" + installed_release;

	installed_signature_keyid = this->GetSigKeyId(name);  

    /* Put the data in a data object. */
    item = this->CreateItem();
    item->SetStatus(OvalEnum::STATUS_EXISTS);
	item->AppendElement(new ItemEntity("name",  name, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
	item->AppendElement(new ItemEntity("arch",  installed_architecture, OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));
	item->AppendElement(new ItemEntity("epoch",  installed_epoch, OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));
	item->AppendElement(new ItemEntity("release",  installed_release, OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));
	item->AppendElement(new ItemEntity("version",  installed_version, OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));
	item->AppendElement(new ItemEntity("evr",  installed_evr, OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));
	item->AppendElement(new ItemEntity("signature_keyid",  installed_signature_keyid, OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));

    /* add the new item to the vector. */
    items->push_back(item);

  }

  /* Free the iterator and transaction set data structures. */
  rpmdbFreeIterator(iterator);
  rpmtsFree(ts);
}

char* RPMInfoProbe::readHeaderString(Header header, int_32 tag_id) {
  // This function is from the Red Hat RPM Guide //
  int_32 type;
  void *pointer;
  int_32 data_size;

  int header_status = headerGetEntry(header,
				     tag_id,
				     &type,
				     &pointer,
				     &data_size);

	
  if (header_status) {
    if (type == RPM_STRING_TYPE) {
      return (char *) pointer;
    }
  }

  return (NULL);
}

int_32 RPMInfoProbe::readHeaderInt32(Header header, int_32 tag_id) {
  // This function is from the Red Hat RPM Guide //
  int_32 type;
  void *pointer;
  int_32 data_size;

  int header_status = headerGetEntry(header,
				     tag_id,
				     &type,
				     (void **) &pointer,
				     &data_size);
	
  if (header_status) {
    if (type == RPM_INT32_TYPE) {
      int_32 *p = (int_32 *) pointer;
      return *p;
    }
  }
  return( -1 );
}

void RPMInfoProbe::ChildGetSigKeyId(int writeErrh, int writeh, string rpmName) {
  //------------------------------------------------------------------------------------//
  //  ABSTRACT
  //  
  //  Redirect stdout and stderr to the provided pipes (writeh, and writeErrh). 
  //  Execute rpm query with the correct options.
  //  see: http://fedora.redhat.com/docs/drafts/rpm-guide-en/ch15s05.html
  //
  //------------------------------------------------------------------------------------//

  // Point STDOUT and STDERR of child at pipe.  When exec program, output and
  // all error messages will be sent down pipe instead of to screen.
  if (writeh != STDOUT_FILENO) {
    if (dup2(writeh, STDOUT_FILENO) != STDOUT_FILENO)
      exit(-1);
  }

  if (writeErrh != STDERR_FILENO) {
    if (dup2(writeErrh, STDERR_FILENO) != STDERR_FILENO)
      exit(-1);
  }

  // Output redirected (duplicated), no longer need pipe
  close (writeh);
  close (writeErrh);

  /////////////////////////////////////////////////////
  ////////     Call the rpmcli query code  ////////////
  /////////////////////////////////////////////////////

  // recreate a set of command line args for rpmcli functions
	int count = 4;
	char* arg0 = "blah";
	char* arg1 = "-q";
	char* arg2 = "--queryformat=\"%{SIGGPG:pgpsig}\"";
	char arg3[128] = "";
	strcat(arg3, rpmName.c_str());
	char* args[count];
	args[0] = arg0;
	args[1] = arg1;
	args[2] = arg2;
	args[3] = arg3;

	/* Set up a table of options. */
	struct poptOption optionsTable[] = {
		{ NULL, '\0', POPT_ARG_INCLUDE_TABLE, rpmcliAllPoptTable, 0, "Common options for all rpm modes and executables:", NULL },
        { NULL, '\0', POPT_ARG_INCLUDE_TABLE, rpmQueryPoptTable, 0, "Query options (with -q or --query):", NULL },
        POPT_AUTOALIAS
        POPT_AUTOHELP
        POPT_TABLEEND
        };

	poptContext context;
	QVA_t qva = &rpmQVKArgs;	
	rpmts ts;
	int ec;
	context = rpmcliInit(count, args, optionsTable);

    if (context == NULL) {
		/*poptPrintUsage(context, stderr, 0);
        exit(EXIT_FAILURE);*/
		cerr << "Error: rpmcliInit returned a null context." << endl; 
    }

    ts = rpmtsCreate();

	/* Check for query mode. */
    if (qva->qva_mode == 'q') {

        /* Make sure there's something to do. */
        if (qva->qva_source != RPMQV_ALL && !poptPeekArg(context)) {
            fprintf(stderr, "no arguments given for --query");
            exit(EXIT_FAILURE);
		}
        ec = rpmcliQuery(ts, qva, (const char **) poptGetArgs(context));

	} else {
		cerr << "Unable to get sig key id for rpm: " << rpmName << endl;        
    }

	ts = rpmtsFree(ts);
    context = rpmcliFini(context);

  /////////////////////////////////////////////////////
  ////////////  end rpmcliquery code   ////////////////
  /////////////////////////////////////////////////////

	exit(0);
} 

string RPMInfoProbe::ParentGetSigKeyId(int readErrh, int readh, int pid) { 

  int bytes = 0;
  int maxFDS = 0;
  char *buf = NULL;
  fd_set readfds;
  bool errComplete = false;
  bool stdComplete = false;
  string errText = "";
  string text = "";

  // Allocate memory for  buf
  buf = (char*)malloc(sizeof(char)*1024);
  if(buf == NULL) {

    // Wait for the child process to complete
    waitpid (pid, NULL, 0);
      
    // Close the pipes
    close (readh);
    close (readErrh);

    // Set an error message
	throw ProbeException("Error: unable to allocate memory to read rpm query data into.");
  }

  // Init the maxFDS value
  if(readh >= readErrh) {
    maxFDS = readh + 1;
  } else {
    maxFDS = readErrh + 1;
  }

  // Loop over the call to select without using a timmer
  // Only stop looping when select fails. select will
  // fail when the file descriptors are closed by the 
  // child process.
  while(!errComplete || !stdComplete) {

    // Reset the fd_set
    FD_ZERO(&readfds);
    FD_SET(readErrh, &readfds);
    FD_SET(readh, &readfds);

    if(select(maxFDS, &readfds, NULL, NULL, NULL) != -1) {
      if(FD_ISSET(readErrh, &readfds)) {
		// Read some error output from command.  
		memset(buf, '\0', 1024);
		bytes = read(readErrh, buf, 1023);
		errText.append(buf);
		  if(bytes == 0) 
		    errComplete = true;
	  }

      if(FD_ISSET(readh, &readfds)) { 
	    // Read allsome std output from command. 
	    memset(buf, '\0', 1024);
	    bytes = read(readh, buf, 1023);
	    text.append(buf);

	    if(bytes == 0)
	      stdComplete = true;
      }	  
    } else {	    
       break;
    }
  }
  
  free(buf);

  // Wait for the child process to complete
  if(waitpid (pid, NULL, 0) == -1) {
    errText.append("Execution of rpm query in child process failed.");
    throw ProbeException(errText);
  }

  // Close the pipes
  close (readh);
  close (readErrh);

  if(errText.compare("") != 0) {
	  throw ProbeException("Error running rpm query in child process: " + errText);
  }
  return text;
}

string RPMInfoProbe::GetSigKeyId(string rpmName) {

	string sigKeyId = "";
	int fd1[2];
	int fd2[2];
	int pid = 0;

	// Open communication pipes between processes
	if (pipe(fd1) < 0 || pipe(fd2) < 0)
		throw ProbeException("Error: (RPMInfoProbe) Could not open pipe.");

	if ((pid = fork()) < 0) {
		throw ProbeException("Error: (RPMInfoProbe) fork error before running rpm query.");

	// Child process
	} else if (pid == 0) {

		// Close unnecessary pipes
		close (fd1[0]);
		close (fd2[0]);  

		this->ChildGetSigKeyId(fd1[1], fd2[1], rpmName);

	// Parent process
	} else {
	
		// Close unnecessary pipes
		close (fd1[1]);
		close (fd2[1]);

		// Get the results of the rpm query
		string text = this->ParentGetSigKeyId(fd1[0], fd2[0], pid);
		// parse the string and get just the key id portion - just the last 16 chars minus the quotation mark
		sigKeyId = text.substr(text.length()-17, 16);
	}  
	return sigKeyId;
}
