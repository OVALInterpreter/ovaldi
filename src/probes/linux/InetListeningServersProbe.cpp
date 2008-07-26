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

#include "InetListeningServersProbe.h"

//****************************************************************************************//
//						InetListeningServersProbe Class									  //	
//****************************************************************************************//
InetListeningServersProbe *InetListeningServersProbe::instance = NULL;

InetListeningServersProbe::InetListeningServersProbe() {

	netstatResult = NULL;
}

InetListeningServersProbe::~InetListeningServersProbe() {

	delete this->netstatResult;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* InetListeningServersProbe::Instance() {
	// -----------------------------------------------------------------------
	//
	//  ABSTRACT
	//
	//	Ensure that the InetListeningServersProbe is a singleton.
	// -----------------------------------------------------------------------

	// Use lazy initialization
	if(instance == NULL) 
		instance = new InetListeningServersProbe();

	return instance;	
}


ItemVector* InetListeningServersProbe::CollectItems(Object* object) {
	//------------------------------------------------------------------------------------//
	//
	//  ABSTRACT
	//
	//------------------------------------------------------------------------------------//

	ItemVector *collectedItems = new ItemVector();

	ObjectEntity* protocol = object->GetElementByName("protocol");
	ObjectEntity* localAddress = object->GetElementByName("local_address");
	ObjectEntity* localPort = object->GetElementByName("local_port");


	// check operation - only allow  equals, not equals and pattern match
	if(protocol->GetOperation() != OvalEnum::OPERATION_EQUALS && protocol->GetOperation() != OvalEnum::OPERATION_PATTERN_MATCH && protocol->GetOperation() != OvalEnum::OPERATION_NOT_EQUAL) {
		throw ProbeException("Error: invalid operation specified on protocol. Found: " + OvalEnum::OperationToString(protocol->GetOperation()));
	}

	// check datatypes - only allow string
	if(protocol->GetDatatype() != OvalEnum::DATATYPE_STRING) {
		throw ProbeException("Error: invalid data type specified on protocol. Found: " + OvalEnum::DatatypeToString(protocol->GetDatatype()));
	}

	// check operation - only allow  equals, not equals and pattern match
	if(localAddress->GetOperation() != OvalEnum::OPERATION_EQUALS && localAddress->GetOperation() != OvalEnum::OPERATION_PATTERN_MATCH && localAddress->GetOperation() != OvalEnum::OPERATION_NOT_EQUAL) {
		throw ProbeException("Error: invalid operation specified on protocol. Found: " + OvalEnum::OperationToString(localAddress->GetOperation()));
	}

	// check datatypes - only allow string
	if(localAddress->GetDatatype() != OvalEnum::DATATYPE_STRING) {
		throw ProbeException("Error: invalid data type specified on local_address. Found: " + OvalEnum::DatatypeToString(localAddress->GetDatatype()));
	}

	// check operation - only allow  equals, not equals and pattern match
	if(localPort->GetOperation() != OvalEnum::OPERATION_EQUALS && localPort->GetOperation() != OvalEnum::OPERATION_PATTERN_MATCH && localPort->GetOperation() != OvalEnum::OPERATION_NOT_EQUAL) {
		throw ProbeException("Error: invalid operation specified on local_port. Found: " + OvalEnum::OperationToString(localPort->GetOperation()));
	}

	// check datatypes - only allow string
	if(localPort->GetDatatype() != OvalEnum::DATATYPE_STRING) {
		throw ProbeException("Error: invalid data type specified on program_name. Found: " + OvalEnum::DatatypeToString(localPort->GetDatatype()));
	}

	// passed initial checks make call to netstat
	if(this->netstatResult == NULL) {
		this->ExecNetstat();
		this->ParseNetstatResult();
	}

	StringVector* protocols = this->GetProtocols(protocol);

	if(protocols->size() > 0) {
		StringVector::iterator iterator;
		for(iterator = protocols->begin(); iterator != protocols->end(); iterator++) {
			string protocolStr = (*iterator);

			StringVector* localAddresses = this->GetLocalAddresses(protocolStr, localAddress);

			if(localAddresses->size() > 0) {

				StringVector::iterator iterator2;
				for(iterator2 = localAddresses->begin(); iterator2 != localAddresses->end(); iterator2++) {
					string localAddressStr = (*iterator2);

					StringVector* localPorts = this->GetLocalPorts(protocolStr, localAddressStr, localPort);

					if(localPorts->size() > 0) {

						StringVector::iterator iterator3;
						for(iterator3 = localPorts->begin(); iterator3 != localPorts->end(); iterator3++) {
							string localPortStr = (*iterator3);

							Item* item = this->GetNetstat(protocolStr, localAddressStr, localPortStr);
							if(item != NULL) {
								collectedItems->push_back(item);
							}				
						}

					} else {

						if(localPort->GetOperation() == OvalEnum::OPERATION_EQUALS) {

							if(localPort->GetVarRef() == NULL) {

								Item* item = this->CreateItem();
								item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
								item->AppendElement(new ItemEntity("protocols",  protocolStr, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
								item->AppendElement(new ItemEntity("local_address",  localAddressStr, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
								item->AppendElement(new ItemEntity("local_port", localPort->GetValue(), OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_DOES_NOT_EXIST));
								collectedItems->push_back(item);

							} else {

								VariableValueVector::iterator iterator;
								for(iterator = localPort->GetVarRef()->GetValues()->begin(); iterator != localPort->GetVarRef()->GetValues()->end(); iterator++) {

									Item* item = this->CreateItem();
									item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
									item->AppendElement(new ItemEntity("protocols",  protocolStr, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
									item->AppendElement(new ItemEntity("local_address",  localAddressStr, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
									item->AppendElement(new ItemEntity("local_port", (*iterator)->GetValue(), OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_DOES_NOT_EXIST));
									collectedItems->push_back(item);
								}
							}
						}
					}

					localPorts->clear();
					delete localPorts;
				}

			} else {

				if(localAddress->GetOperation() == OvalEnum::OPERATION_EQUALS) {

					if(localAddress->GetVarRef() == NULL) {

						Item* item = this->CreateItem();
						item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
						item->AppendElement(new ItemEntity("protocols",  protocolStr, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
						item->AppendElement(new ItemEntity("local_address",  localAddress->GetValue(), OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_DOES_NOT_EXIST));
						collectedItems->push_back(item);

					} else {

						VariableValueVector::iterator iterator;
						for(iterator = localAddress->GetVarRef()->GetValues()->begin(); iterator != localAddress->GetVarRef()->GetValues()->end(); iterator++) {

							Item* item = this->CreateItem();
							item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
							item->AppendElement(new ItemEntity("protocols",  protocolStr, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
							item->AppendElement(new ItemEntity("local_address",  (*iterator)->GetValue(), OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_DOES_NOT_EXIST));
							collectedItems->push_back(item);
						}
					}
				}
			}

			localAddresses->clear();
			delete localAddresses;
		}

	} else {

		if(protocol->GetOperation() == OvalEnum::OPERATION_EQUALS) {

			if(protocol->GetVarRef() == NULL) {

				Item* item = this->CreateItem();
				item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
				item->AppendElement(new ItemEntity("protocols",  protocol->GetValue(), OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_DOES_NOT_EXIST));
				collectedItems->push_back(item);

			} else {

				VariableValueVector::iterator iterator;
				for(iterator = protocol->GetVarRef()->GetValues()->begin(); iterator != protocol->GetVarRef()->GetValues()->end(); iterator++) {

					Item* item = this->CreateItem();
					item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
					item->AppendElement(new ItemEntity("protocols",  (*iterator)->GetValue(), OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_DOES_NOT_EXIST));
					collectedItems->push_back(item);
				}
			}
		}
	}

	protocols->clear();
	delete protocols;

	return collectedItems;
}  

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
Item* InetListeningServersProbe::CreateItem() {
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
						"inetlisteningserver_item");

	return item;
}

StringVector* InetListeningServersProbe::GetProtocols(ObjectEntity* protocol) {
	// -----------------------------------------------------------------------
	//
	//  ABSTRACT
	//
	//  Get the set of all protocols on the system that match the object
	//
	// -----------------------------------------------------------------------
	StringVector* protocols = NULL;

	// does this name use variables?
	if(protocol->GetVarRef() == NULL) {
		
		// proceed based on operation
		if(protocol->GetOperation() == OvalEnum::OPERATION_EQUALS) {
			protocols = new StringVector();
			// if the protocol exists add it to the list 
			if(this->ProtocolExists(protocol->GetValue())) {
				protocols->push_back(protocol->GetValue());
			}

		} else if(protocol->GetOperation() == OvalEnum::OPERATION_NOT_EQUAL) {
			
			protocols = this->GetMatchingProtocols(protocol->GetValue(), false);

		} else if(protocol->GetOperation() == OvalEnum::OPERATION_PATTERN_MATCH) {
			protocols = this->GetMatchingProtocols(protocol->GetValue(), true);
		}		

	} else {

		protocols = new StringVector();

		// Get all names
		StringVector allProtocols;

		if(protocol->GetOperation() == OvalEnum::OPERATION_EQUALS) {
			// in the case of equals simply loop through all the 
			// variable values and add them to the set of all names
			// if they exist on the system
			VariableValueVector::iterator iterator;
			for(iterator = protocol->GetVarRef()->GetValues()->begin(); iterator != protocol->GetVarRef()->GetValues()->end(); iterator++) {
				
				if(this->ProtocolExists((*iterator)->GetValue())) {
					allProtocols.push_back((*iterator)->GetValue());
				}
			}

		} else {
            this->GetMatchingProtocols(".*", &allProtocols);
		}
	
		// loop through all names on the system
		// only keep names that match operation and value and var check
		ItemEntity* tmp = this->CreateItemEntity(protocol);
		StringVector::iterator it;
		for(it = allProtocols.begin(); it != allProtocols.end(); it++) {
			tmp->SetValue((*it));			
			if(protocol->Analyze(tmp) == OvalEnum::RESULT_TRUE) {
				protocols->push_back((*it));
			}
		}
	}

	return protocols;
}

StringVector* InetListeningServersProbe::GetMatchingProtocols(string pattern, bool isRegex) {

	StringVector* protocols = new StringVector();

	// loop through the NetstatRecord Vector
	NetstatRecordVector::iterator iterator;
	for(iterator = this->nrv.begin(); iterator != this->nrv.end(); iterator++) {
		NetstatRecord* nr = (NetstatRecord*)(*iterator);

		if(this->IsMatch(pattern, nr->protocol, isRegex)) {

			// make sure local addresses are unique
			StringVector::iterator iterator2;
			bool isUnique = true;
			for(iterator2 = protocols->begin(); iterator2 != protocols->end(); iterator2++) {	
				if((*iterator2).compare(nr->protocol) == 0) {
					isUnique = false;
					break;
				}
			}
			if(isUnique) {
				protocols->push_back(nr->protocol);
			}
		}			
	}
	return protocols;
}

bool InetListeningServersProbe::ProtocolExists(string protocol) {

	bool exists = false;
	// loop through the NetstatRecord Vector
	NetstatRecordVector::iterator iterator;
	for(iterator = this->nrv.begin(); iterator != this->nrv.end(); iterator++) {
		NetstatRecord* nr = (NetstatRecord*)(*iterator);

		if(protocol.compare(nr->protocol) == 0) {
			exists = true;
			break;
		}			
	}
	 
	return exists;
}

StringVector* InetListeningServersProbe::GetLocalAddresses(string protocolStr, ObjectEntity* localAddress) {
	// -----------------------------------------------------------------------
	//
	//  ABSTRACT
	//
	//  Get the set of all protocols on the system that match the object
	//
	// -----------------------------------------------------------------------
	StringVector* localAddresses = NULL;

	// does this name use variables?
	if(localAddress->GetVarRef() == NULL) {
		
		// proceed based on operation
		if(localAddress->GetOperation() == OvalEnum::OPERATION_EQUALS) {
			localAddresses = new StringVector();
			// if the localAddress exists add it to the list 
			if(this->LocalAddressExists(protocolStr, localAddress->GetValue())) {
				localAddresses->push_back(localAddress->GetValue());
			}

		} else if(localAddress->GetOperation() == OvalEnum::OPERATION_NOT_EQUAL) {
			
			localAddresses = this->GetMatchingLocalAddresses(protocolStr, localAddress->GetValue(), false);

		} else if(localAddress->GetOperation() == OvalEnum::OPERATION_PATTERN_MATCH) {
			localAddresses = this->GetMatchingLocalAddresses(protocolStr, localAddress->GetValue(), true);
		}		

	} else {

		localAddresses = new StringVector();

		// Get all names
		StringVector allLocalAddresses;

		if(localAddress->GetOperation() == OvalEnum::OPERATION_EQUALS) {
			// in the case of equals simply loop through all the 
			// variable values and add them to the set of all names
			// if they exist on the system
			VariableValueVector::iterator iterator;
			for(iterator = localAddress->GetVarRef()->GetValues()->begin(); iterator != localAddress->GetVarRef()->GetValues()->end(); iterator++) {
				
				if(this->LocalAddressExists(protocolStr, (*iterator)->GetValue())) {
					allLocalAddresses.push_back((*iterator)->GetValue());
				}
			}

		} else {
            this->GetMatchingLocalAddresses(protocolStr,".*", &allLocalAddresses);
		}
	
		// loop through all names on the system
		// only keep names that match operation and value and var check
		ItemEntity* tmp = this->CreateItemEntity(localAddress);
		StringVector::iterator it;
		for(it = allLocalAddresses.begin(); it != allLocalAddresses.end(); it++) {
			tmp->SetValue((*it));			
			if(localAddress->Analyze(tmp) == OvalEnum::RESULT_TRUE) {
				localAddresses->push_back((*it));
			}
		}
	}

	return localAddresses;
}

StringVector* InetListeningServersProbe::GetMatchingLocalAddresses(string protocolStr, string pattern, bool isRegex) {

	StringVector* localAddresses = new StringVector();

	// loop through the NetstatRecord Vector
	NetstatRecordVector::iterator iterator;
	for(iterator = this->nrv.begin(); iterator != this->nrv.end(); iterator++) {
		NetstatRecord* nr = (NetstatRecord*)(*iterator);

		if(protocolStr.compare(nr->protocol) == 0) {
			if(this->IsMatch(pattern, nr->local_address, isRegex)) {

				// make sure local addresses are unique
				StringVector::iterator iterator2;
				bool isUnique = true;
				for(iterator2 = localAddresses->begin(); iterator2 != localAddresses->end(); iterator2++) {	
					if((*iterator2).compare(nr->local_address) == 0) {
						isUnique = false;
						break;
					}
				}
				if(isUnique) {
					localAddresses->push_back(nr->local_address);
				}
			}
		}
	}
	return localAddresses;
}

bool InetListeningServersProbe::LocalAddressExists(string protocolStr, string localAddress) {

	bool exists = false;
	// loop through the NetstatRecord Vector
	NetstatRecordVector::iterator iterator;
	for(iterator = this->nrv.begin(); iterator != this->nrv.end(); iterator++) {
		NetstatRecord* nr = (NetstatRecord*)(*iterator);

		if(protocolStr.compare(nr->protocol) == 0) {
			if(localAddress.compare(nr->local_address) == 0) {
				exists = true;
				break;
			}			
		}
	}
	 
	return exists;
}

StringVector* InetListeningServersProbe::GetLocalPorts(string protocolStr, string localAddressStr, ObjectEntity* localPort) {
	// -----------------------------------------------------------------------
	//
	//  ABSTRACT
	//
	//  Get the set of all protocols on the system that match the object
	//
	// -----------------------------------------------------------------------
	StringVector* localPorts = NULL;

	// does this name use variables?
	if(localPort->GetVarRef() == NULL) {
		
		// proceed based on operation
		if(localPort->GetOperation() == OvalEnum::OPERATION_EQUALS) {
			localPorts = new StringVector();
			// if the localPort exists add it to the list 
			if(this->LocalPortExists(protocolStr, localAddressStr, localPort->GetValue())) {
				localPorts->push_back(localPort->GetValue());
			}

		} else if(localPort->GetOperation() == OvalEnum::OPERATION_NOT_EQUAL) {
			
			localPorts = this->GetMatchingLocalPorts(protocolStr, localAddressStr, localPort->GetValue(), false);

		} else if(localPort->GetOperation() == OvalEnum::OPERATION_PATTERN_MATCH) {
			localPorts = this->GetMatchingLocalPorts(protocolStr, localAddressStr, localPort->GetValue(), true);
		}		

	} else {

		localPorts = new StringVector();

		// Get all names
		StringVector allLocalPorts;

		if(localPort->GetOperation() == OvalEnum::OPERATION_EQUALS) {
			// in the case of equals simply loop through all the 
			// variable values and add them to the set of all names
			// if they exist on the system
			VariableValueVector::iterator iterator;
			for(iterator = localPort->GetVarRef()->GetValues()->begin(); iterator != localPort->GetVarRef()->GetValues()->end(); iterator++) {
				
				if(this->LocalPortExists(protocolStr, localAddressStr, (*iterator)->GetValue())) {
					allLocalPorts.push_back((*iterator)->GetValue());
				}
			}

		} else {
            this->GetMatchingLocalPorts(protocolStr, localAddressStr, ".*", &allLocalPorts);
		}
	
		// loop through all names on the system
		// only keep names that match operation and value and var check
		ItemEntity* tmp = this->CreateItemEntity(localPort);
		StringVector::iterator it;
		for(it = allLocalPorts.begin(); it != allLocalPorts.end(); it++) {
			tmp->SetValue((*it));			
			if(localPort->Analyze(tmp) == OvalEnum::RESULT_TRUE) {
				localPorts->push_back((*it));
			}
		}
	}

	return localPorts;
}

StringVector* InetListeningServersProbe::GetMatchingLocalPorts(string protocolStr, string localAddressStr, string pattern, bool isRegex) {

	StringVector* localPorts = new StringVector();

	// loop through the NetstatRecord Vector
	NetstatRecordVector::iterator iterator;
	for(iterator = this->nrv.begin(); iterator != this->nrv.end(); iterator++) {
		NetstatRecord* nr = (NetstatRecord*)(*iterator);

		if(protocolStr.compare(nr->protocol) == 0) {
			if(localAddressStr.compare(nr->local_address) == 0) {
				if(this->IsMatch(pattern, nr->local_port, isRegex)) {
					
					// make sure local addresses are unique
					StringVector::iterator iterator2;
					bool isUnique = true;
					for(iterator2 = localPorts->begin(); iterator2 != localPorts->end(); iterator2++) {	
						if((*iterator2).compare(nr->local_port) == 0) {
							isUnique = false;
							break;
						}
					}
					if(isUnique) {
						localPorts->push_back(nr->local_port);
					}
				}
			}
		}
	}
	return localPorts;
}

bool InetListeningServersProbe::LocalPortExists(string protocolStr, string localAddressStr, string localPort) {

	bool exists = false;
	// loop through the NetstatRecord Vector
	NetstatRecordVector::iterator iterator;
	for(iterator = this->nrv.begin(); iterator != this->nrv.end(); iterator++) {
		NetstatRecord* nr = (NetstatRecord*)(*iterator);

		if(protocolStr.compare(nr->protocol) == 0) {
			if(localAddressStr.compare(nr->local_address) == 0) {
				if(localPort.compare(nr->local_port) == 0) {
					exists = true;
					break;
				}
			}
		}
	}
	 
	return exists;
}

Item* InetListeningServersProbe::GetNetstat(string protocol, string localAddress, string localPort) {
  //------------------------------------------------------------------------------------//
  //  ABSTRACT
  //  
  //  Gather the listening servers data that matches dataIn's program name
  //  Add each match to the result vector with the test id from dataIn
  //
  //------------------------------------------------------------------------------------//

	Item* item = NULL;
	
	// loop through the NetstatRecord Vector
	NetstatRecordVector::iterator iterator;
	for(iterator = this->nrv.begin(); iterator != this->nrv.end(); iterator++) {
		NetstatRecord* nr = (NetstatRecord*)(*iterator);

		if(protocol.compare(nr->protocol) == 0) {
			if(localAddress.compare(nr->local_address) ==0) {
				if(localPort.compare(nr->local_port) == 0) {
					/* Put the data in a data object. */
					item = this->CreateItem();
					item->SetStatus(OvalEnum::STATUS_EXISTS);
					item->AppendElement(new ItemEntity("protocol", nr->protocol, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
					item->AppendElement(new ItemEntity("local_address", nr->local_address, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
					item->AppendElement(new ItemEntity("local_port", nr->local_port, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
					item->AppendElement(new ItemEntity("local_full_address", nr->local_full_address, OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));
					item->AppendElement(new ItemEntity("program_name", nr->program_name, OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));				
					item->AppendElement(new ItemEntity("foreign_address", nr->foreign_address, OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));
					item->AppendElement(new ItemEntity("foreign_port", nr->foreign_port, OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));
					item->AppendElement(new ItemEntity("foreign_full_address", nr->foreign_full_address, OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));
					item->AppendElement(new ItemEntity("pid", Common::ToString(nr->pid), OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));
					item->AppendElement(new ItemEntity("user_id", nr->user_id, OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));
				}
			}
		}
	}

	return item;
}
void InetListeningServersProbe::ExecNetstat() {

	int fd1[2];
	int fd2[2];
	int pid = 0;
	//char *buf = NULL;
	//char *errbuf = NULL;

	// Open communication pipes between processes
	if (pipe(fd1) < 0 || pipe(fd2) < 0)
		throw ProbeException("Error: (InetListeningServersProbe) Could not open pipe.");

	if ((pid = fork()) < 0) {
		throw ProbeException("Error: (InetListeningServersProbe) fork error before running netstat -tuwlnpe.");

	// Child process
	} else if (pid == 0) {

		// Close unnecessary pipes
		close (fd1[0]);
		close (fd2[0]);  

		ChildExecNetstat(fd1[1], fd2[1]);

	// Parent process
	} else {
	
		// Close unnecessary pipes
		close (fd1[1]);
		close (fd2[1]);

		// Get the results of netstat
		this->netstatResult = ParentGetChildResult(fd1[0], fd2[0], pid);
	}  
}

void InetListeningServersProbe::ChildExecNetstat(int writeErrh, int writeh) {
  //------------------------------------------------------------------------------------//
  //  ABSTRACT
  //  
  //  Redirect stdout and stderr to the provided pipes (writeh, and writeErrh). 
  //  Execute nestate with the correct options.
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

  // Execute the command
  execl("/bin/netstat", "netstat", "-tuwlnpe", NULL);

  exit(0);
} 

NetstatResult* InetListeningServersProbe::ParentGetChildResult(int readErrh, int readh, int pid) { //, char* buf, char* errbuf) {
  //------------------------------------------------------------------------------------//
  //  ABSTRACT
  //  
  //  Read readErrh and readh until there is no more data to be read. Wait for the 
  //  child process to complere. Return a NetstatResult object with the data from
  //  netstat.
  //
  //------------------------------------------------------------------------------------//

  NetstatResult *result = new NetstatResult("", "");
  int bytes = 0;
  int maxFDS = 0;
  char *buf = NULL;
  fd_set readfds;
  bool errComplete = false;
  bool stdComplete = false;

  // Allocate memory for  buf
  buf = (char*)malloc(sizeof(char)*1024);
  if(buf == NULL) {

    // Wait for the child process to complete
    waitpid (pid, NULL, 0);
      
    // Close the pipes
    close (readh);
    close (readErrh);

    // Set an error message
    result->errText.append("Error: unable to allocate memory to read netstat data into.");
    
    return result;
  }

  // Init the maxFDS value
  if(readh >= readErrh) {
    maxFDS = readh + 1;
  }else {
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
	result->errText.append(buf);

	if(bytes == 0) 
	  errComplete = true;
      }

      if(FD_ISSET(readh, &readfds)) { 
	// Read allsome std output from command. 
	memset(buf, '\0', 1024);
	bytes = read(readh, buf, 1023);
	result->text.append(buf);

	if(bytes == 0)
	  stdComplete = true;
      }
	  
    }else {	    
      break;
    }
  }
  
  // Wait for the child process to complete
  if(waitpid (pid, NULL, 0) == -1) {
    result->errText.append("Execution of netstat in child process failed.");
    return result;
  }

  // Close the pipes
  close (readh);
  close (readErrh);

  return result;
}

void InetListeningServersProbe::ParseNetstatResult() {
		
	// Walk through the netstat output lines, parsing them.
	string tmpSTR;
	string PIDandProgramName;

	int curDelim;
	int preDelim;
	int curColon;
	int preColon;
	size_t curPos;
	size_t tmpPos;

	string Protocol;
	string LocalFullAddress;
	string LocalAddress;
	string LocalPort;
	string ForeignFullAddress;
	string ForeignAddress;
	string ForeignPort;
	string UserID;
	string PID;
	string ProgramName;
	bool skip;

	// Toss out first two lines
	curPos = this->netstatResult->text.find("\n") + 1;
	curPos = this->netstatResult->text.find("\n", curPos) + 1;
	
	// Get the first line end 
	tmpPos = this->netstatResult->text.find("\n", curPos);

	// Parse the remaining lines.
	while (tmpPos != string::npos) {
		NetstatRecord *nr = new NetstatRecord();
		
		// Get the next string
		tmpSTR = this->netstatResult->text.substr(curPos, (tmpPos - curPos));
		curPos = tmpPos + 1;
		
		curDelim = 0;
		preDelim = 0;

		////////////////////////////////////////////
		///// Protocol:  TCP or UDP ////////////////
		////////////////////////////////////////////

		// Find the first space and grab the characters between preDelim and that point
		curDelim = tmpSTR.find(" ",preDelim);
		nr->protocol = tmpSTR.substr(preDelim,(curDelim - preDelim));

		// Now find the end of that whitespace
		preDelim = tmpSTR.find_first_not_of(" ",curDelim);

		////////////////////////////////////////////
		///// Skip two fields //////////////////////
		////////////////////////////////////////////
		
		// Find the next space and then skip past it
		curDelim = tmpSTR.find(" ",preDelim);
		preDelim = tmpSTR.find_first_not_of(" ",curDelim); 

		// Do that again.
		curDelim = tmpSTR.find(" ",preDelim);
		preDelim = tmpSTR.find_first_not_of(" ",curDelim); 

		////////////////////////////////////////////
		///// LocalFullAddress //// ////////////////
		////////////////////////////////////////////

		curDelim = tmpSTR.find(" ",preDelim);
		LocalFullAddress = tmpSTR.substr(preDelim,(curDelim - preDelim));
		preDelim = tmpSTR.find_first_not_of(" ",curDelim);
		nr->local_full_address = LocalFullAddress;

		////////////////////////////////////////////
		///// LocalAddress /////////////////////////
		////////////////////////////////////////////

		preColon = 0;
		curColon = LocalFullAddress.find(":",preColon);
		nr->local_address = LocalFullAddress.substr(preColon,(curColon - preColon));

		////////////////////////////////////////////
		///// LocalPort ////////////////////////////
		////////////////////////////////////////////

		preColon = curColon + 1;
		curColon = LocalFullAddress.length();
		nr->local_port = LocalFullAddress.substr(preColon,(curColon - preColon));
	
		////////////////////////////////////////////
		///// ForeignFullAddress ///////////////////
		////////////////////////////////////////////

		curDelim = tmpSTR.find(" ",preDelim);
		ForeignFullAddress = tmpSTR.substr(preDelim,(curDelim - preDelim));
		preDelim = tmpSTR.find_first_not_of(" ",curDelim);
		nr->foreign_full_address = ForeignFullAddress;

		if((preDelim - curDelim) > 20)
			skip = false;
		else
			skip = true;

		////////////////////////////////////////////
		///// ForeignAddress ///////////////////////
		////////////////////////////////////////////

		preColon = 0;
		curColon = ForeignFullAddress.find(":",preColon);
		nr->foreign_address = ForeignFullAddress.substr(preColon,(curColon - preColon));
		

		////////////////////////////////////////////
		///// ForeignPort //////////////////////////
		////////////////////////////////////////////

		preColon = curColon + 1;
		curColon = ForeignFullAddress.length();
		nr->foreign_port = ForeignFullAddress.substr(preColon,(curColon - preColon));

		///////////////////////////////////////////
		///// Skip one field //////////////////////
		///////////////////////////////////////////
		if(skip) {
			curDelim = tmpSTR.find(" ",preDelim);
			preDelim = tmpSTR.find_first_not_of(" ",curDelim);
		}
		///////////////////////////////////////////
		///// UserID //////////////////////////////
		///////////////////////////////////////////

		curDelim = tmpSTR.find(" ",preDelim);
		nr->user_id = tmpSTR.substr(preDelim,(curDelim - preDelim));
		preDelim = tmpSTR.find_first_not_of(" ",curDelim);

		///////////////////////////////////////////
		///// Skip one field //////////////////////
		///////////////////////////////////////////

		curDelim = tmpSTR.find(" ",preDelim);
		preDelim = tmpSTR.find_first_not_of(" ",curDelim);

		///////////////////////////////////////////
		///// PID/ProgramName /////////////////////
		///////////////////////////////////////////

		curDelim = tmpSTR.find(" ",preDelim);
		PIDandProgramName = tmpSTR.substr(preDelim,(curDelim - preDelim));
		preDelim = tmpSTR.find_first_not_of(" ",curDelim);
		
		////////////////////////////////////////////
		///// PID  /////////////////////////////////
		////////////////////////////////////////////

		preColon = 0;
		curColon = PIDandProgramName.find("/",preColon);
		PID = PIDandProgramName.substr(preColon,(curColon - preColon));
		nr->pid = atoi(PID.c_str());
			
		////////////////////////////////////////////
		///// ProgramName //////////////////////////
		////////////////////////////////////////////

		preColon = curColon + 1;
		curColon = PIDandProgramName.length();
		nr->program_name = PIDandProgramName.substr(preColon,(curColon - preColon));


		nrv.push_back(nr);

		// Get the next line end 
		tmpPos = this->netstatResult->text.find("\n", curPos);  
	}  
}


  
