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

#ifndef RUNLEVELPROBE_H
#define RUNLEVELPROBE_H

#include "../../AbsProbe.h"

#include <iostream>
#include <cstring>
#include <set>
#include <map>
#include <iterator>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifndef INIT_DIR
#define INIT_DIR  "/etc/rc.d/init.d"
#endif  /* INIT_DIR */

#ifndef RC_DIR
#define RC_DIR    "/etc/rc.d"
#endif  /* RC_DIR */

#define BUFLEN    1024

using namespace std;

/**
  A service that is started or killed at a particular runlevel. This would most likely be stored
  in a map where the key is the runlevel.  Because of this, the class itself does not contain a
  runlevel identifing member. It is an item for a GIVEN runlevel.
 */
class runlevel_item
{
  public:
    runlevel_item() : is_start( false ) {}
    runlevel_item( const string &name, const bool is_start ) : service_name(name), is_start(is_start) {}
    ~runlevel_item(){}

    runlevel_item( const runlevel_item &r1 ) { this->copy_item( r1 ); }
    runlevel_item & operator= ( const runlevel_item &rhs ) { this->copy_item( rhs ); return *this; }

    string  service_name;
    bool    is_start;
  

  private:
    void copy_item( const runlevel_item &r1 ) { this->service_name = r1.service_name; this->is_start = r1.is_start; }
};



/**
  Comparator used by STL (char *) containers with uniqueness constructs.
 */
struct ltstr_comparator
{
  bool operator()( const char * s1, const char * s2 )
  {
    return ( strcmp( s1, s2 ) < 0 );
  }
};



/**
  Comparator used by STL (char) containers with uniqueness constructs.
 */
struct ltchr_comparator
{
  bool operator() ( const char c1, const char c2 )
  {
    return ( c1 < c2 );
  }
};




/**
  Comparator used by STL (runlevel_item) containers with uniqueness constructs.
 */
struct ltrunlevel_item_comparator
{
  bool operator() ( const runlevel_item &r1, const runlevel_item &r2 )
  {
    return ( strcmp( r1.service_name.c_str(), r2.service_name.c_str() ) < 0 );
  }

};



typedef set<char, ltchr_comparator> CharSet;
typedef set<const char *, ltstr_comparator> CharPtrSet;
typedef set<runlevel_item, ltrunlevel_item_comparator> RunLevelItemSet;
typedef map<const char, RunLevelItemSet, ltchr_comparator> SetMap;


/**
	Data collector for runlevel test
  The resulting ItemEnities contain information about what services run at what runlevels
  The information gathered is similar to `chkconfig --list` on chkconfig-enabled systems.
*/
class RunLevelProbe : public AbsProbe {

  public: // Public Methods
	  virtual ~RunLevelProbe();

	  virtual ItemVector* CollectItems( Object* object );

	  /** Ensure that the RunLevelProbe is a singleton. */
	  static AbsProbe * Instance();
 
  private: // Private Methods
	  RunLevelProbe();    

	  /** Return a new Item created for storing runlevel information. */
	  virtual Item* CreateItem();

    /**
      Checks runlevel object entity operations and datatypes
      @param service_name the service_name runlevel_object entity
      @param runlevel the runlevel runlevel_object entity
      @throws ProbeException if invalid operations or datatypes are encountered
    */
    void          _verifyRunlevelObjectAttr( ObjectEntity * service_name, ObjectEntity * runlevel ) const throw( ProbeException );

    /**
      Checks to see if the given filename is either "." or ".."
      @param filename the file we are working with
      @return true if the filename is either "." or ".."
    */
    bool          _isBadDir( const char * filename ) const;

    /**
      Constructs a complete path to a given file. Usually this is because the method is expecting the path
      to point to a runlevel directory like /etc/rc.d/rc0.d and we are trying to construct a full path
      to one of the directories contents as returned by readdir( path );
      @param path system path like /etc/rc.d/rc0.d
      @param filename file contained by path like K01foobar
      @return a pointer to a constructed full path string like /etc/rc.d/rc0.d/K01foobar.  This must be deleted after using.
    */
    char *        _constructFullPath( const char * path, const char * filename ) const;

    /**
      Adds the given @filename to the _runlevels map at key @runlevel
      @param filename the path to the file we are storing in the _runlevels map
      @param runlevel the runlevel in which @filename is being executed at
    */
    void          _handleReg( const char * filename, const char runlevel );

    /**
      Dereferences the symbolic link desegnated by fullPath.  Then sends the derefernced path to _handleReg(...)
      @param the name of the symbolic link
      @param fullPath the path to the symbolic link
      @param the runlevel in which the service pointed to by @fullPath is to be executed at
    */
    void          _handleLink( const char * filename,  const char * fullPath,  const char runlevel );

    /**
      Adds the script to the _runlevels map for given runlevel if the file pointed to by dir and filename is valid
      @param dir diretory in which the script exists
      @param filename the filename of the script
      @param runlevel the runleveel in which the script is being executed at
    */
    void          _addRunlevelItem( const char * dir, const char * filename, const char runlevel );

    /**
      Reads the runlevel script directory designated by the given directory
      @param dir the directory which contains runlevel directories ( /etc/rc.d )
      @param runlevel the runlevel being analyzed [0,1,2,3,4,5,6,s,S] 
    */
    void          _analyzeRunlevelDir( const char * dir, const char runlevel ); 

    /**
      Creates a string representing the appropriate runlevel directory designated by the given runlevel 
      @param runlevel [0,1,2,3,4,5,6,s,S]
      @return a pointer to the new directory string.  This must be deleted after using.
    */
    char *        _generateRunlevelDir( const char runlevel ) const;

    /**
      Generates a map (_runlevels) of runlevel information for the system.
    */
    void          _analyzeRunlevels();
    
    /**
      Erases the contents of the _runlevels  map which contains sets of dynamically allocated memory.
    */
    void          _deallocateMap( );

    /**
      Converts a character to a single character string.
      @param chr the character being converted into a string
      @return a pointer to the new string.  This must be deleted after use. 
    */
    char *        _chrToStr( char chr ) const;

    /**
      Creates an runelevel_item for the given paramters
      @param runlevel the runlevel_item runlevel entity
      @param service_name the runlevel_item service_name entity
      @param kill the runlevel_item kill entity ( should always be false )
      @param start the runlevel_item start entity ( should always be true )
      @return a new runlevel_item object.  This is cleaned up by the ProbeFactory.
    */ 
    Item *        _makeRunlevelItem( const char runlevel, const runlevel_item &r1 );

    /**
      Generates all the runlevel_item objects for the given runlevel_object we are processing
      @param runlevelSet the set of runlevels dictated by the runlevel runlevel_object entity
      @param services the set of services dictated by the service_name runlevel_object entity
      @return a vectory of runlevel_item objects.  This should be cleared up by the ProbeFactory.
    */
    ItemVector  * _getItemEntities( CharSet * runlevelSet, RunLevelItemSet * services ); 

    /**
      Generates a set of runlevels dictated by the runlevel runlevel_object entity
      @param pattern the value supplied by the runlevel runlevel_object entity
      @param isRegex true if the pattern parameter is a regular expression
      @param insertUnequalNonRegex true if the runlevel runlevel_object entity has a "not equal" operation
      @return a set of runlevels being used in the runlevel_test
    */ 
    CharSet     * _getMatchingRunlevels( string pattern, bool isRegex, bool insertUnequalNonRegex = false );

    /**
      Generates a set of service names dictated by the service_name runlevel_object enitty 
      @param pattern the value supplied by the service_name runlevel_object entity
      @param working_runlevels the runlevels we need to look at for services
      @param isRegex is the given pattern parameter a regular expression
      @insertUnequalNonRegex true if the service_name runlevel_object entity has a "not equal" operation
      @return a set of services being used in the given runlevels 
    */
    RunLevelItemSet  * _getMatchingServiceNames ( string pattern, CharSet * working_runlevels, bool isRegex, bool insertUnequalNonRegex = false );

    /**
      Calls  ItemEntity::Analyze() method to validate the data collected for the given entity (service_name)
      @param workingSet the set of services collected for the given entity
      @param entity the service_name entity for the working runlevel_object
      @return a set of validated services.  This must be deleted after use.
    */
    RunLevelItemSet  * _analyzeContainer( RunLevelItemSet * workingSet, ObjectEntity * entity );

    /**
      Calls  ItemEntity::Analyze() method to validate the data collected for the given entity (runlevel)
      @param workingSet the set of runlevels collected for the given entity
      @param entity the runlevel entity for the working runlevel_object
      @return a set of validated runlevels.  This must be deleted after use.
    */
    CharSet     * _analyzeContainer( CharSet * workingSet, ObjectEntity * entity ) ;

    /**
      Generates a set of runevels as dictated by the runlevel entity 
      @param runelvel the runlevel runlevel_object entity being worked with
      @return a set of validated runlevels as dictated by the object entity.  This must be deleted after use.
    */
    CharSet     * _getRunLevelData( ObjectEntity * runlevel );
      
    /** 
      Generates a set of services as dictated by the services entity for the given set of runlevels
      @param service_name the service_name runlevel_object entity being worked with
      @return a set of validated services as dictated by the object entity.  This must be deleted after use.
    */
    RunLevelItemSet  * _getServiceData( ObjectEntity * service_name, CharSet * runlevels );

    /**
      Determines if a script is a "Start" script or a "Kill" script based on the first character of the filename:
      an 'K' determines a "Kill" script and anything else means a "Start" script.
      @param  fileName the script (could be a symlink) filename
      @return true if the given filename corresponds to a start script.
    */
    bool  _isStart( const char * fileName );

    /**
      Places the given run level item in the _runlevel map
      @param runlevel the runlevel in which the runlevel item executes
      @param rli the runlevel item

    */
    void _populateMap( const char runlevel, const runlevel_item &rli );

    /**
      Determines whether or not the supplied value should be inserted into a container.
      @pattern the pattern (regex or otherwise) to be used in comparison
      @value the value we are considering for insertion
      @isRegex true if the pattern is a regular expression
      @insertUnequalNonRegex true if the value should be inserted though pattern and value do not match and isRegex is false
      @return true if the value warrants an insertion
    */
    bool          _isInsertable( string pattern, string value, bool isRegex, bool insertUnequalNonRegex );
  
  private:  // Private Member Variables 
	  static RunLevelProbe *  _instance;

    /** Storage container of system runlevel information **/
    SetMap  _runlevels;   
};

#endif
