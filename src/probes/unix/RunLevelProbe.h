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

#define INIT_DIR  "/etc/rc.d/init.d"
#define RC_DIR    "/etc/rc.d"
#define BUFLEN    1024

using namespace std;

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



typedef set<char, ltchr_comparator> CharSet;
typedef set<const char *, ltstr_comparator> CharPtrSet;
typedef map<const char, CharPtrSet *, ltchr_comparator> SetMap;


/**
	Data collector for runlevel test
  The resulting ItemEnities contain information about what services run at what runlevels
  The information gathered is similar to `chkconfig --list` on chkconfig-enabled systems.
*/
class RunLevelProbe : public AbsProbe {

  public: // Public Methods
	  ~RunLevelProbe();
    
    ItemVector* CollectItems( Object* object );

	  /** Return a new Item created for storing process information. */
	  Item* CreateItem();

	  /** Ensure that the RunLevelProbe is a singleton. */
	  static AbsProbe * Instance();
 
  private: // Private Methods
	  RunLevelProbe();

    void          _verifyRunlevelObjectAttr( ObjectEntity * service_name, ObjectEntity * runlevel ) throw( ProbeException );
    bool          _isBadDir( const char * filename ) const;
    char *        _constructFullPath( const char * path, const char * filename ) const;
    void          _handleReg( const char * filename, const char runlevel );
    void          _handleLink( const char * fullPath, const char * filename, const char runlevel );
    void          _addRunlevelItem( const char * dir, const char * filename, const char runlevel );
    void          _analyzeRunlevelDir( const char * dir, const char runlevel ); 
    char *        _generateRunlevelDir( const char runlevel ) const;
    void          _analyzeRunlevels();
    void          _deallocateSet( CharPtrSet * rlSet );
    void          _deallocateMap( );
    char *        _chrToStr( char chr ) const;
    Item *        _makeRunlevelItem( const char runlevel, const char * service_name, bool kill = false, bool start = true );
    ItemVector  * _getItemEntities( CharSet * runlevelSet, CharPtrSet * services ); 
    CharSet     * _getMatchingRunlevels( string pattern, bool isRegex, bool insertUnequalNonRegex = false );
    CharPtrSet  * _getMatchingServiceNames ( string pattern, CharSet * working_runlevels, bool isRegex, bool insertUnequalNonRegex = false );
    CharPtrSet  * _analyzeContainer( CharPtrSet * workingSet, ObjectEntity * entity );
    CharSet     * _analyzeContainer( CharSet * workingSet, ObjectEntity * entity ) ;
    CharSet     * _getRunLevelData( ObjectEntity * runlevel );
    CharPtrSet  * _getServiceData( ObjectEntity * service_name, CharSet * runlevels );
    bool          _isInsertable( string pattern, string value, bool isRegex, bool insertUnequalNonRegex );
    
  private:  // Private Member Variables 
	  static RunLevelProbe *  _instance;
    SetMap  _runlevels;   
};

#endif
