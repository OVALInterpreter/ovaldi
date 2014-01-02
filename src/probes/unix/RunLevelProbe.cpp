//
//
//****************************************************************************************//
// Copyright (c) 2002-2014, The MITRE Corporation
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

#include <cstring>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "RunLevelProbe.h"

using namespace std;

//****************************************************************************************//
//                               RunLevelProbe Class                                      //
//****************************************************************************************//


RunLevelProbe * RunLevelProbe::_instance = NULL;  // static initialization




RunLevelProbe::RunLevelProbe( ) {
  _analyzeRunlevels();
}




RunLevelProbe::~RunLevelProbe(){  
  RunLevelProbe::_instance = NULL;
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//


AbsProbe * 
RunLevelProbe::Instance(){
  
  if( _instance == NULL )
    _instance = new RunLevelProbe();

  return _instance;
}




ItemVector * 
RunLevelProbe::CollectItems( Object* object ){
  ItemVector      * collectedItems  = NULL;
  CharSet         * runlevelSet     = NULL;
  RunLevelItemSet * serviceSet      = NULL;
  ObjectEntity    * service_name    = object->GetElementByName("service_name"),
                  * runlevel        = object->GetElementByName("runlevel");
    
  _verifyRunlevelObjectAttr( service_name, runlevel ); // throws ProbException
  
  collectedItems  = new ItemVector();    
  runlevelSet     = _getRunLevelData( runlevel );
  serviceSet      = _getServiceData( service_name, runlevelSet );
  collectedItems  = _getItemEntities( runlevelSet, serviceSet ); 
 
  delete  runlevelSet;
  delete  serviceSet;    
  return  collectedItems;
}




Item *
RunLevelProbe::CreateItem( ){
  Item* item = new Item ( 
                          0,
                          "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#unix",
                          "unix-sc",
                          "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#unix unix-system-characteristics-schema.xsd",
                          OvalEnum::STATUS_ERROR,
                          "runlevel_item"
                        );

  return item;
}




//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//



void
RunLevelProbe::_verifyRunlevelObjectAttr( ObjectEntity * service_name, ObjectEntity * runlevel ) const throw( ProbeException ) {
  
  // check datatypes - only allow string
  if( service_name->GetDatatype() != OvalEnum::DATATYPE_STRING ){
    throw ProbeException( "Error: invalid data type specified on service_name. Found: " + OvalEnum::DatatypeToString( service_name->GetDatatype() ));
  }
  
  if( runlevel->GetDatatype() != OvalEnum::DATATYPE_STRING ){
    throw ProbeException( "Error: invalid data type specified on runlevel. Found: " + OvalEnum::DatatypeToString( runlevel->GetDatatype() )); 
  }

  // check operation - only allow  equals, not equals and pattern match
  if( service_name->GetOperation() != OvalEnum::OPERATION_EQUALS && 
      service_name->GetOperation() != OvalEnum::OPERATION_PATTERN_MATCH && 
      service_name->GetOperation() != OvalEnum::OPERATION_NOT_EQUAL ) 
  {
    throw ProbeException("Error: invalid operation specified on service_name. Found: " + OvalEnum::OperationToString( service_name->GetOperation() ));
  }

  if( runlevel->GetOperation() != OvalEnum::OPERATION_EQUALS &&
      runlevel->GetOperation() != OvalEnum::OPERATION_PATTERN_MATCH &&
      runlevel->GetOperation() != OvalEnum::OPERATION_NOT_EQUAL )
  {
    throw ProbeException("Error: invalid operation specified on runlevel. Found: " + OvalEnum::OperationToString( runlevel->GetOperation() ));
  }

}



// MUST DELETE POINTER
char *
RunLevelProbe::_chrToStr( char chr ) const {
  char * rtnval = new char[2];
  rtnval[0] = chr;
  rtnval[1] = (char)NULL; 

  return rtnval;
}





Item *
RunLevelProbe::_makeRunlevelItem( const char runlevel, const runlevel_item &rli ) {
  Item *  item = this->CreateItem();
  char *  runlevelToStr  = _chrToStr( runlevel );
  bool    start = rli.is_start; 

  item->SetStatus( OvalEnum::STATUS_EXISTS );
  item->AppendElement( new ItemEntity( "service_name", rli.service_name, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
  item->AppendElement( new ItemEntity( "runlevel", runlevelToStr, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
  item->AppendElement( new ItemEntity( "start", ( start ) ? "true" : "false", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );
  item->AppendElement( new ItemEntity( "kill", ( !start ) ? "true" : "false", OvalEnum::DATATYPE_BOOLEAN, OvalEnum::STATUS_EXISTS ) );

  delete[] runlevelToStr;
  return item;
}





ItemVector *
RunLevelProbe::_getItemEntities( CharSet * runlevelSet, RunLevelItemSet * services ) {
  ItemVector      * items   = new ItemVector();
  RunLevelItemSet   mapSet;  
  CharSet::iterator           runlevel_iter;
  RunLevelItemSet::iterator   service_iter; 
      
  for( runlevel_iter = runlevelSet->begin(); runlevel_iter != runlevelSet->end(); runlevel_iter++ ){
    char runlevel_chr = (*runlevel_iter);
    mapSet = _runlevels[ runlevel_chr ];
    for( service_iter = mapSet.begin(); service_iter != mapSet.end(); service_iter++ ){
      runlevel_item rli = (*service_iter);
      if ( services->find( rli ) != services->end() ){
        Item * item = _makeRunlevelItem( runlevel_chr, rli );
        items->push_back( item );
      }
    }
  }
  
  return items;
}





CharSet * 
RunLevelProbe::_analyzeContainer( CharSet * workingSet, ObjectEntity * entity ) {
  CharSet     * rtnSet = new CharSet();
  ItemEntity  * item   = this->CreateItemEntity( entity );   // memory cleaned up automatically by the ProbeFactory.

  for( CharSet::iterator iter = workingSet->begin(); iter != workingSet->end(); iter++ ){
    char * str = _chrToStr( (*iter) );
    item->SetValue( str );  
    delete[] str;

    if( entity->Analyze( item ) == OvalEnum::RESULT_TRUE ) {
      rtnSet->insert( (*iter) );
    }
  }

  return rtnSet;
}





RunLevelItemSet * 
RunLevelProbe::_analyzeContainer( RunLevelItemSet * workingSet, ObjectEntity * entity ) {
  RunLevelItemSet * rtnSet = new RunLevelItemSet();
  ItemEntity * item   = this->CreateItemEntity( entity );  // memory cleaned up automatically by the ProbeFactory
  
  for ( RunLevelItemSet::iterator iter = workingSet->begin(); iter != workingSet->end(); iter++ ){
    item->SetValue( (*iter).service_name );
    if( entity->Analyze( item ) == OvalEnum::RESULT_TRUE ){
      rtnSet->insert( *iter );
    }
  }

  return  rtnSet;
}





/**
  Due to the inconsistent behavior of IsMatch(...) for regex's and regular strings we need to
  preform some boolean logic to check and see if the given value and pattern warrant an insert
  into a storage container.

  str1    |   str2    |   isRegex   |   IsMatch(...)  |   bool match    |   insertUnequalNonRegex   |   RETURN  |
  _______________________________________________________________________________________________________________
  
  "foo"       "foo"       FALSE           FALSE               TRUE                  FALSE               TRUE   
  "foo"       "foo"       FALSE           FALSE               TRUE                  TRUE                FALSE
  "foo"       "bar"       FALSE           TRUE                FALSE                 FALSE               FALSE
  "foo"       "bar"       FALSE           TRUE                FALSE                 TRUE                TRUE
  "foo"       "bar"       TRUE            FALSE               FALSE                 FALSE               FALSE
  "foo"       "foo"       TRUE            TRUE                TRUE                  FALSE               TRUE
  "foo"       "foo"       TRUE            TRUE                TRUE                  TRUE                FALSE
  "foo"       "bar"       TRUE            FALSE               FALSE                 TRUE                FALSE
 **/

bool
RunLevelProbe::_isInsertable( string pattern, string value, bool isRegex, bool insertUnequalNonRegex ) {
  bool match  = ( this->IsMatch( pattern, value, isRegex ) == isRegex );
  return ( match && !insertUnequalNonRegex ) || ( !isRegex && !match && insertUnequalNonRegex );
}






CharSet *
RunLevelProbe::_getMatchingRunlevels( string pattern, bool isRegex, bool insertUnequalNonRegex ) {
  CharSet * matches = new CharSet(); 
  SetMap::iterator iter;

  for( iter = _runlevels.begin(); iter != _runlevels.end(); iter++ ){
    char runlevel = (*iter).first;
    const char runlevel_str[] = { runlevel, (char)NULL };
    bool insert = _isInsertable( pattern, runlevel_str, isRegex, insertUnequalNonRegex );

    if( insert == true ) matches->insert( runlevel );
  }

  return matches;
}




/**
  _getMatchingServiceNames():
  Ideally this would return a SetMap * with each runlevel containing a set of associated services that match
  the criteria set by OVAL constructs.  This would easily preserving the mapping of runlevels to associated
  services without requiring a lookup on the _runlevels map.  However, the lookup isn't very difficult or procesor
  intensive so it's not a big deal.  Also, we are more than likely going to only be working with one runlevel for
  a given runlevel_test so the map loses it's appeal in that scenario.
 **/

RunLevelItemSet *
RunLevelProbe::_getMatchingServiceNames ( string pattern, CharSet * working_runlevels, bool isRegex, bool insertUnequalNonRegex ) {
  RunLevelItemSet * matches =  new RunLevelItemSet();
  
  // iterate over the set of runlevels
  for ( CharSet::iterator runlevel_iter = working_runlevels->begin(); runlevel_iter != working_runlevels->end(); runlevel_iter++ ){
    char runlevel = (*runlevel_iter);
    
    // get services for given runlevel
    SetMap::iterator  map_iter   = _runlevels.find( runlevel );
    RunLevelItemSet   rlSet      = (*map_iter).second;

    for( RunLevelItemSet::iterator iter = rlSet.begin(); iter != rlSet.end(); ++iter ){
      runlevel_item rli = *iter;
      bool insert = _isInsertable( pattern, rli.service_name, isRegex, insertUnequalNonRegex );
      if ( insert == true ) matches->insert( rli ); 
    } 
  }

  return matches;
}





CharSet *
RunLevelProbe::_getRunLevelData( ObjectEntity * runlevel ){
  bool    isRegex                 = ( runlevel->GetOperation() == OvalEnum::OPERATION_PATTERN_MATCH );
  bool    insertUnequalNonRegex   = ( runlevel->GetOperation() == OvalEnum::OPERATION_NOT_EQUAL );
  CharSet       * rtnSet          = NULL,
                * workingSet      = NULL;

  if( runlevel->GetVarRef() == NULL )
    workingSet =  _getMatchingRunlevels( runlevel->GetValue(), isRegex, insertUnequalNonRegex );
  else
    workingSet =  _getMatchingRunlevels( ".*", true, false );  // gimme all the runlevels on the box

  rtnSet = _analyzeContainer( workingSet, runlevel );

  delete  workingSet;
  return  rtnSet;
}





RunLevelItemSet *
RunLevelProbe::_getServiceData( ObjectEntity * service_name, CharSet * runlevels ){
  bool    isRegex                 = ( service_name->GetOperation() == OvalEnum::OPERATION_PATTERN_MATCH );
  bool    insertUnequalNonRegex   = ( service_name->GetOperation() == OvalEnum::OPERATION_NOT_EQUAL );
  RunLevelItemSet * rtnSet        = NULL,
                  * workingSet    = NULL;

  if( service_name->GetVarRef() == NULL )
    workingSet =  _getMatchingServiceNames( service_name->GetValue(), runlevels, isRegex, insertUnequalNonRegex );
  else
    workingSet =  _getMatchingServiceNames( ".*", runlevels, true, false );  // gimme all the runlevels on the box

  
  rtnSet = _analyzeContainer( workingSet, service_name );

  delete  workingSet;
  return  rtnSet;
}





bool
RunLevelProbe::_isBadDir( const char * filename ) const {
  bool  rtnval  = true;
  int   length  = ( filename != NULL ) ? strlen( filename ) : 0;

  if( strncmp( filename, ".", length ) != 0 ){
    if( strncmp( filename, "..", length ) != 0 ){
      rtnval = false;
    }
  }

  return rtnval;
}



/**
 *  MUST DELETE RETURNED POINTER
 **/
char *
RunLevelProbe::_constructFullPath( const char * path, const char * filename ) const {
  char * fullPath = new char[ BUFLEN + 1 ];
  memset( fullPath, (char)NULL, BUFLEN + 1 );

  strncat( fullPath, path, ( BUFLEN - (strlen( fullPath )) ));
  strncat( fullPath, "/", ( BUFLEN - (strlen( fullPath )) ));
  strncat( fullPath, filename, ( BUFLEN - (strlen( fullPath )) ));

  return fullPath;
}




void
RunLevelProbe::_populateMap( const char runlevel, const runlevel_item &rli ){
  RunLevelItemSet &rlSet = _runlevels[ runlevel ];
  rlSet.insert( rli );
}




/**
 * If the filename found within /etc/rc[0-9].d starts with a K, the service is meant to be
 * killed.  If it starts with a S it is meant to be started.
 **/
bool
RunLevelProbe::_isStart( const char * filename ){
  return filename[0] != 'K';
}



void
RunLevelProbe::_handleReg( const char * filename, const char runlevel ){
  runlevel_item item;
  REGEX re;
  StringVector matches;

  if (re.GetMatchingSubstrings("^[SK]\\d+(.+)$", filename, &matches))
    item.service_name = matches[0];
  else
    item.service_name = filename;

  item.is_start     = _isStart( filename );

  _populateMap( runlevel, item );
}





void
RunLevelProbe::_handleLink( const char * filename, const char * fullPath,  const char runlevel ){
  char            buffer[BUFLEN + 1];
  char *          serviceName   = new char[BUFLEN + 1],
       *          lastSlash     = NULL;
  int             nchars        = 0;
  runlevel_item   item;

  memset( buffer, (char)NULL, BUFLEN + 1 );
  memset( serviceName, (char)NULL, BUFLEN + 1 );

  nchars    = readlink( fullPath, buffer, BUFLEN );
  lastSlash = strrchr( buffer, '/' );

  if( lastSlash != NULL ) {
    ++lastSlash;
    strncpy( serviceName, lastSlash, BUFLEN );
  }

  item.service_name = serviceName;
  item.is_start     = _isStart( filename );
  
  _populateMap( runlevel, item );
  delete[] serviceName;
}





void
RunLevelProbe::_addRunlevelItem( const char * dir, const char * filename, const char runlevel ){
  struct stat st_file;
  char * fullPath  = NULL;

  if( _isBadDir( filename ) == false ){
    fullPath = _constructFullPath( dir, filename );
    stat( fullPath, &st_file );

	if( S_ISLNK(st_file.st_mode) ){
      _handleLink( filename, fullPath, runlevel );
    }

	else if( S_ISREG(st_file.st_mode) ){
      _handleReg( filename, runlevel );
    }
    else ;  // Directories, Pipes, Sockets, etc. cannot be used -- only Symbolic Links and Regular Files (scripts and binary executables)
  }

  if( fullPath ) delete[] fullPath;
}





void
RunLevelProbe::_analyzeRunlevelDir( const char * dir, const char runlevel ){
  DIR     * rcDir    = opendir( dir );
  dirent  * dirItem  = NULL;

  if( rcDir != NULL ){
    while( ( dirItem = readdir( rcDir ) ) != NULL ){
      _addRunlevelItem( dir, dirItem->d_name, runlevel );
    }
    closedir( rcDir );
  }
}

void
RunLevelProbe::_analyzeRunlevels ( ){
  const char runlevelTypes[] = { '0', '1', '2', '3', '4', '5', '6', 'S', 's' };

  for( unsigned int i = 0; i < sizeof( runlevelTypes ) / sizeof(char); ++i ){
    string runlevelDir = string(RC_DIR "/rc") + runlevelTypes[i] + ".d";
    _analyzeRunlevelDir( runlevelDir.c_str(), runlevelTypes[i] );
  }
}
