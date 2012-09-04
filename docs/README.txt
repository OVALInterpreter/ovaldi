****************************************************

                  OVAL Interpreter

 Copyright (c) 2002 - 2012 - The MITRE Corporation

****************************************************

The MITRE Corporation developed the Open Vulnerability and Assessment
Language (OVAL) Interpreter to provide the OVAL Community with an open
source reference implementation of the OVAL Language and its OVAL Definitions.
The OVAL Interpreter uses OVAL Definitions to gather security relevant
configuration information on a computer (e.g., rpm parameters, registry keys,
file information, etc.), analyze the information for vulnerabilities and
configuration issues, and report the results of the analysis for each OVAL
Definition. 

You may download the OVAL Interpreter to any computer you wish, and to as many
computers as you wish.  

BY USING THE OVAL INTERPRETER, YOU SIGNIFY YOUR ACCEPTANCE OF THE TERMS AND 
CONDITIONS OF USE.  IF YOU DO NOT AGREE TO THESE TERMS, DO NOT USE THE OVAL 
INTERPRETER.  SEE THE TERMS.TXT FILE INCLUDED WITH THE OVAL INTERPRETER.

-- CONTENTS --
	
  I	    SUPPORTED PLATFORMS
		 A. Red Hat Linux 
         B. Sun Solaris
         C. Microsoft Windows
         D. Mac OS
  II    INSTALLATION
         A. Red Hat Linux 
         B. Sun Solaris
         C. Microsoft Windows
         D. Mac OS
  III   USING THE OVAL INTERPRETER
         A. Required Privileges
         B. Data Protection
         C. Obtaining Updated Data Files
         D. Quick Usage Guide
         E. Advanced Usage
  IV  	INPUT AND OUTPUT
         A. Input
         B. Output
  V     PCRE
  VI    XERCES
  VII   XALAN
  VIII  LIBGCRYPT
  IX    OPENLDAP
  X     TROUBLESHOOTING
  XI    REPORTING PROBLEMS
  XII   USEFUL LINKS

-- I -- SUPPORTED PLATFORMS --

  A. Red Hat Linux

	  The OVAL Interpreter is currently developed and tested on the 32-bit version of Red Hat 
	  Enterprise Linux 5.
  
  B. Sun Solaris
  
	  The OVAL Interpreter is currently developed and tested on the 32-bit version of Sun 
	  Solaris 10.
  
  C. Microsoft Windows
  
	  The OVAL Interpreter should run on both 32-bit and 64-bit versions of Windows XP, 
	  Windows Server 2003, Windows Vista, Windows 2008, and Windows 7.  However, we only
	  develop and test the OVAL Interpreter on the 32-bit version of Windows XP and the 64-bit 
	  version of Windows 7.  

  D. Mac OS  

	  The OVAL Interpreter is currently developed and tested on the 32-bit version Mac 
	  OSX 10.6.
	
-- II -- INSTALLATION --

  Download an appropriate installation file of the OVAL Interpreter
  from the OVAL Interpreter SourceForge site,
  (http://sourceforge.net/projects/ovaldi/files/). Once you have
  downloaded the OVAL Interpreter, follow the corresponding
  instructions to install the OVAL Interpreter on your system.

  A. Red Hat Linux Installation

       Install the RPM simply by running the following command:

       # rpm -ivh <OVAL Interpreter RPM>

       The RPM places the following files on the system.  To learn their exact
       location after install, run:  rpm -ql ovaldi.

       Executables:
       ovaldi.sh - Shell Script to simplify execution of the OVAL Interpreter
       ovaldi - OVAL Interpreter binary

       Libraries:
       libxerces-c.so.27.0 - Xerces XML library
       libxalan-c.so.110.0 - Xalan XSL library
       libxalanMsg.so.110.0 - Xalan Message library
       libpcre.so.0.0.1 - PCRE Regular Expression library

       Documentation:
       README.txt - This file
       terms.txt - License and Terms of Use
       version.txt - OVAL Interpreter change log

       XML Schema:
       OVAL Schema files
       evaluation-ids.xsd - XML Schema that defines a format for inputing a set
                            of OVAL Definition ids to evaluate.
       xmldsig-core-schema.xsd - XML Digital Signature Schema

       XSL:
       oval-definitions-schematron.xsl - OVAL Definition Schematron validation
                                         rules
       results_to_html.xsl - A simple XSL that formats an OVAL Results document
                             as HTML
 
       The .so file for the Xerces library must be made accessible to the OVAL
       Interpreter binary.  The ovaldi RPM places a pre-compiled version in 
       /usr/lib/ovaldi and appends this directory to your /etc/ld.so.conf file.

       To run the OVAL Interpreter, run:

       # /usr/sbin/ovaldi.sh

       A result summary will be output to the screen, and a detailed OVAL System
       Characteristics and OVAL Results document will be output to the current
       working directory. By default, the OVAL System Characteristics document
       will be named system-characteristics.xml and the OVAL Results document
       will be named results.xml.
    
  B. Sun Solaris Installation

       Please see build.solaris.txt.


  C. Microsoft Windows Installation
     
       Install the OVAL Interpreter by running the installer
       executable -      ovaldi-X.X.X-setup.exe

       The installer is a self-extracting zip archive that prompts the user for
       an installation directory ('C:\Program Files\OVAL\ovaldi' by default)
       and installs the OVAL Interpreter and its supporting files.

       Executables:
       ovaldi.exe - OVAL Interpreter binary

       Libraries:
       xerces-c_2_7_0.dll - Xerces XML library
       msvcr100.dll - Microsoft C Runtime Library
       pcre.dll - Perl-compatible regular-expression library
       Xalan-C_1_10.dll - Xalan XSLT library
       XalanMessages_1_10.dll - Xalan XSLT library
	   libgcrypt-11.dll - Libgcrypt library
	   libgpg-error-0.dll - GnuPG error code library
	   

       Documentation:
       README.txt - This file
       terms.txt - License and Terms of Use
       version.txt - OVAL Interpreter change log

       XML Schema:
       OVAL Schema files
       evaluation-ids.xsd - XML Schema that defines a format for inputing a set
                            of OVAL Definition ids to evaluate.
       xmldsig-core-schema.xsd - XML Digital Signature Schema

       XSL:
       oval-definitions-schematron.xsl - OVAL Definition Schematron validation 
                                         rules
       results_to_html.xsl - A simple XSL that formats an OVAL Results document
                             as HTML

       To run the OVAL Interpreter, run:

       ovaldi.exe -m -o <OVAL Definitions document> 

       A result summary will be output to the screen, and a detailed OVAL System
       Characteristics and OVAL Results document will be output to the current
       working directory. By default, the OVAL System Characteristics document
       will be named system-characteristics.xml and the OVAL Results document
       will be named results.xml.

       Also, on some Windows systems, the OVAL Interpreter may fail with the
       following error message when executed.

         "The application failed to initialize properly (0xc0150002). Click
          OK to terminate the application."

       This error message occurs when the run-time components of Visual Studio,
       that are required to run an application developed with Visual Studio, are
       not installed. If you receive this error message while executing the OVAL
       Interpreter, please install the appropriate VC++ redistributable package 
	   that can be obtained at the following links. 

		 Visual Studio C++ 2010 Redistributable Package (x86)
         http://www.microsoft.com/en-us/download/details.aspx?id=5555

		 Visual Studio C++ 2010 Redistributable Package (x64)
         http://www.microsoft.com/en-us/download/details.aspx?id=14632
		 
       The VC++ redistributable package will install the required run-time
       components. 


  D. Mac OS Installation

       Please see build.macos.txt.


-- III -- USING THE OVAL INTERPRETER --

  A. Requirements -- IMPORTANT NOTE:

     In order to collect all of the system configuration data required to 
     correctly evaluate OVAL Definitions, the OVAL Interpreter MUST BE RUN WITH
     ADMINISTRATOR/ROOT PRIVILEGES.

     Certain system data referenced by OVAL Definitions is only
     available to privileged accounts.  This includes information about running
     processes, and potentially registry key and file information (depending on
     local security settings).  While it is possible to run the OVAL 
     Interpreter as a non-privileged user, the results of the analysis may not
     convey the true state of the system.

     On 64-bit operating systems, the same information may not be available to
     both 32 and 64-bit processes, and some may need to be accessed in a
     different way.  This is particularly true on 64-bit Windows, where the OS
     presents different views of the system to 32 and 64-bit processes.  To
     ensure that all information is available, it is recommended to use 64-bit
     builds on 64-bit systems.

  B. Data Protection -- IMPORTANT NOTE:

     The OVAL Interpreter collects system configuration data only
     available to a user with Administrator/root access.  That data
     may be stored locally in a XML file.  In addition, the 
     vulnerability status of the system is written to a file.  
     SINCE THIS IS SENSITIVE INFORMATION, IT IS STRONGLY RECOMMENDED
     THAT THE OVAL INTERPRETER DIRECTORY BE RESTRICTED TO 
     ADMINISTRATOR ACCESS ONLY.


  C. Obtaining the Latest OVAL Definitions from the OVAL Repository:

     OVAL Definitions are created and modified on a regular basis,
     therefore it is advised that you check the OVAL Repository for 
     the latest updates before running the OVAL Interpreter to ensure
     that you are using the most up-to-date OVAL Definitions.  Make sure
     to note the MD5 signature of the data file, as it will be
     needed to execute the OVAL Interpreter.

     OVAL Repository Downloads page:
     http://oval.mitre.org/repository/download/index.html

     Additionally it is recommended that you join the 
     "oval-discussion-list" email list.  A lightly moderated public
     forum for discussion related to new and previously posted OVAL
     Repository content, as well as the vulnerabilities and configuration
     issues themselves that affect OVAL Definition writing. To
     subscribe to the list, go to the Forum Sign-Up page on the
     OVAL Web Site.

     http://oval.mitre.org/community/registration.html


  D. Quick Usage Guide:

  1) As the OVAL Interpreter is only a reference implementation, it has
     purposefully been designed as solely a command-line utility.
     Therefore, to execute the OVAL Interpreter, you will first have to
     open a command window, and change to the OVAL Interpreter installation
     directory.

  2) Download a new OVAL Definitions document and move it into the OVAL Interpreter
     installation directory. Rename the file to 'definitions.xml'.

  3) Run the OVAL Interpreter, supplying the MD5 checksum of the data file as
     a command line option.

     > ovaldi MD5Hash

     After verifying the integrity of the data file using the MD5
     checksum, the OVAL Interpreter will read the 'definitions.xml' 
     file to determine what data to collect from the system, will
     analyze the collected data against the 'definitions.xml' file,
     and will report its findings.


  E. Advanced Usage:

     The OVAL Interpreter accepts a number of command-line options:

     Command Line: ovaldi [options] MD5Hash

     Options:
      -h           = show options available from the command line.

     Definition Evaluation Options:
      -o filename  = path to the oval-definitions xml file. 
                     DEFAULT="definitions.xml"
      -v filename  = path to external variable values file.
                     DEFAULT="external-variables.xml"
      -e <string>  = evaluate the specified list of definitions. Supply
                     definition ids as a comma seperated list like: 
                     oval:com.example:def:123,oval:com.example:def:234
      -f filename  = path to a file containing a list of definitions to be 
                     evaluated. The file must comply with the evaluation-id schema.

     Input Validation Options:
      -m           = do not verify the oval-definitions file with an MD5 hash
      -c filename  = perform Schematron validation on the input OVAL Definitions. The path to an xsl for Schematron validation may optionally be specified. 
                     DEFAULT="oval-definitions-schematron.xsl"    

     Data Collection Options:
      -a dir name  = path to the directory that contains the OVAL schema and other xml resources. On Windows
	                 platforms, DEFAULT="xml".  On *nix platforms, DEFAULT="/usr/share/ovaldi".
      -i filename  = path to input System Characteristics file. Evaluation will
                     be based on the contents of the file.

     Result Output Options:
      -d filename  = save system-characteristics data to the specified XML file.
                     DEFAULT="system-characteristics.xml"
	  -g filename  = path to the oval directives configuration file. DEFAULT="directives.xml". 				 
      -r filename  = save oval-results to the specified XML file.
                     DEFAULT="oval-results.xml"
      -s           = do not apply a stylesheet to the results xml.
      -t filename  = apply the specified xsl to the results xml.
                     DEFAULT="results_to_html.xsl"
      -x filename  = output xsl transform results to the specified file.
                     DEFAULT="results.html"
	  -j filename  = perform schema/schematron validation on the output OVAL System Characteristics. The path to an xsl for Schematron validation may optionally be specified. 
					 DEFAULT="oval-system-characteristics-schematron.xsl"
	  -k filename  = perform schema/schematron validation on the output OVAL Results. The path to an xsl for Schematron validation may optionally be specified. 
					 DEFAULT="oval-results-schematron.xsl"

     Other Options:
      -l <integer> = Log messages at the specified level. (DEBUG = 1, INFO = 2, MESSAGE = 3, FATAL = 4). DEFAULT=2
      -p           = print all information and error messages.
      -y dir name  = save the ovaldi.log file to a specific location.
      -z           = return md5 of current oval-definitions file.


     In more detail: 
     -h -- Displays command line options.

     -o -- Specifies the pathname of the OVAL Definition document to use.
           If a file is not specified, the OVAL Interpreter will default to
           "definitions.xml" in the OVAL Interpreter installation directory.

     -v -- Specifies the pathname of the OVAL Variables document to use.
           If a file is not specified, the OVAL Interpreter will default to
           "external-variables.xml" in the OVAL Interpreter installation
           directory.           

     -e -- Specifies a set of OVAL Definition ids to evaluate in the input
           OVAL Definitions document. OVAL Definition ids should be comma 
           separated without spaces. All OVAL Definitions in the list
           are evaluated in the input OVAL Definitions document if they
           exist in the input OVAL Definitions document. Any OVAL Definition ids
           that are not found will be assigned an error status. Any OVAL Definitions
           in the input OVAL Definitions document that are not in the list will be
           marked as 'Not Evaluated'.            

     -f -- Specifies a the path to an XML file containing the set of
           OVAL Definition ids to evaluate in the input OVAL Definitions
           document. The input XML file must comply with the evaluation-ids.xsd.
           All OVAL Definitions in the input evaluation-ids document are 
           evaluated if they exist in the input OVAL Definitions document. Any
           Definition ids not found will be assigned an error status. Any 
           OVAL Definitions in the input OVAL Definitions document that are not in the list
           will be marked as 'Not Evaluated'.  
     
     -m -- Run without requiring an MD5 checksum.  Running the OVAL
           Interpreter with this option DISABLES an important security
           feature.  In normal usage, a trusted checksum provided on the
           command line is used to verify the integrity of the OVAL
           Definitions document.

           Use of this option is recommended only when testing your own
           draft OVAL Definitions document before submitting them to the OVAL Community 
           Forum for public review.
                     
     -c -- Perform Schematron validation on the input OVAL Definitions document. If a path to an xsl
		   is specified, it will be used for Schematron validation. If a path is not specified, the 
		   OVAL Interpreter will default to "oval-definitions-schematron.xsl" in the OVAL Interpreter 
		   schema directory.

     -a -- Specifies the pathname of the directory that contains the OVAL
           Schema and other XML resources. On Windows platforms, DEFAULT="xml".
		   On *nix platforms, DEFAULT="/usr/share/ovaldi".
           
     -i -- Specifies the pathname of a OVAL System Characteristics document that
           is to be used as the basis of the analysis.  In this mode,
           the OVAL Interpreter does not perform data collection on the
           local system, but relies upon the input OVAL System Characteristics document, which may
           have been generated on another system.
          
     -d -- Specifies the pathname of the file to which collected
           configuration data is to be saved. This data is stored in the
           format defined by the OVAL Systems Characteristics Schema. 
		   
	 -g -- Specifies the pathname of the file containing the directives
		   which are used to control what information is included in
		   the generated OVAL Results document. The file must comply with
		   the OVAL Directives schema.

     -r -- Specifies the pathname of the file to which analysis results
           are to be saved.  This data is stored according to the format
           defined by the OVAL Results Schema.  If none is specified, 
           the OVAL Interpreter will default to "results.xml" in the 
           OVAL Interpreter directory.

     -s -- If set do not apply the XSL to the OVAL Results document.

     -t -- Specifies the pathname of the XSL file which should be used to
           transform the OVAL Results document. If none is specified, the OVAL Interpreter 
           will default to "results_to_html.xsl" in the OVAL Interpreter directory.

     -x -- Specifies the pathname of the file where the XSL transform results
           are to be saved.  If none is specified, the OVAL Interpreter 
           will default to "results.html" in the OVAL Interpreter directory.

	 -j -- Perform Schema and Schematron validation on the output OVAL System Characteristics document. 
		   If a path to an xsl is specified, it will be used for Schematron validation. If a path is not 
		   specified, the OVAL Interpreter will default to "oval-system-characteristics-schematron.xsl" in 
		   the OVAL Interpreter schema directory.
			
	 -k -- Perform Schema and Schematron validation on the output OVAL Results document. If a path to an xsl 
		   is specified, it will be used for Schematron validation. If a path is not specified, the OVAL 
		   Interpreter will default to "oval-results-schematron.xsl" in the OVAL Interpreter schema directory.
		   
     -l -- Logging level.  Log messages at the specified level. 
           (DEBUG = 1, INFO = 2, MESSAGE = 3, FATAL = 4). DEFAULT=2

     -p -- Verbose output.  Print all information and error message to the 
           console.

     -y -- Specifies the path to where the ovaldi.log file should be saved. 

     -z -- Calculates and prints to the screen the MD5 checksum of the
           current OVAL Definitions document (definitions.xml by default, or as specified
           by the -o option).  This can be used to manually compare the
           current OVAL Definitions document with the trusted checksum available from the 
           OVAL Web site.

     MD5Hash -- The MD5 checksum expected for the current OVAL Definitions document
           (definitions.xml by default, or as specified by the -o option).
           The OVAL Interpreter calculates the actual checksum of the
           OVAL Definitions document, and compares it to this value provided on the
           command line, to verify the OVAL Definitions document's integrity.  Checksums
           are available from the OVAL Web site.

           The checksum verification feature ensures that the OVAL Definitions document has not
           been modified: that the OVAL Definitions have not been 
           tampered with, or have had potentially malicious content added.
           Unless the -m option is specified, the MD5Hash is REQUIRED to
           run the OVAL Interpreter.


-- IV -- INPUT AND OUTPUT --

The input and the output formats used by the OVAL Interpreter are
defined by individual XML Schemas. The data collected about a system is
considered input and its format is defined by the OVAL System Characteristics
Schema. The detailed results generated by a system analysis are considered
output and their format is defined by the OVAL Results Schema.  These XML
Schemas have been provided to allow other applications to more easily
implement their own system information collection utilities and manipulate
the results of an analysis for inclusion in other applications.

  A. Input

     The OVAL Interpreter analysis engine accepts an XML data
     model in the format defined by the OVAL System Characteristics Schema
     (system-characteristics-schema.xsd). This data model can be
     generated locally by the OVAL Interpreter's data collection engine, or
     can be provided at runtime using the -i command line option.

     If the data model is provided at runtime, the only requirement is
     that it be formatted according to the OVAL System Characteristics
     Schema.  The data can be gathered using a tool other than
     the OVAL Interpreter, on another system, running a different
     operating system, but as long as the data is in the correct format,
     analysis can be performed.  What this means is, not only can one 
     system be used to run the analysis for a number of systems, but 
     data models can be altered/fabricated to conduct 'what-if' scenarios.

  B. Output

     The OVAL Interpreter provides detailed output from an analysis
     according to the OVAL Results Schema.  This output includes the
     outcome of individual tests as well as each individual OVAL Definition.

     The OVAL Results Schema is provided to allow the results of an
     analysis to be utilized by other applications.  For example, an
     application could convert the detailed results to HTML for
     display on a Web site, or use the detailed results to determine
     what, if any, patches should be applied to a system.  As mentioned
     above, the -r command line option may be used to redirect the
     output to a location other than the default.     


-- V -- PCRE --

The OVAL Interpreter uses the open source PCRE library.  The
binary was compiled with version 6.3 of the PCRE library.  
From the PCRE Web site:

  "Regular expression support is provided by the PCRE library 
   package, which is open source software, written by Philip Hazel,
   and copyright by the University of Cambridge, England."

The PCRE library is under the BSD License and is re-distributed in
ovaldi-x.x.x-setup.exe. As a result, a copy of the license
(LICENCE.pcre) has been provided. 
   
For more information about PCRE visit:
    http://www.pcre.org

To download the source code for PCRE, please use the following ftp
site:
    ftp://ftp.csx.cam.ac.uk/pub/software/programming/pcre/


-- VI -- XERCES --

The OVAL Interpreter uses the open source Xerces library.  The
binary was compiled with version 2.7 of the xerces-c library.  From
the Xerces Web site:

  "This product includes software developed by the Apache Software
  Foundation (http://www.apache.org/)."

The Xerces library is under the Apache 2.0 License and is re-distributed
in ovaldi-x.x.x-setup.exe and ovaldi-libs-x.x.x-1.i386.rpm. As a result, 
a copy of the license (LICENCE.xerces_xalan) has been provided.   
  
For more information about Xerces visit:
    http://xerces.apache.org/

The source code is also available at the above Web site.


-- VII -- XALAN --

The OVAL Interpreter uses the open source Xalan library.  The
binary was compiled with version 1.10.0 of the xalan-c library.  From
the Xalan Web site:

  "This product includes software developed by the Apache Software
  Foundation (http://www.apache.org/)."

The Xalan library is under the Apache 2.0 License and is re-distributed
in ovaldi-x.x.x-setup.exe and ovaldi-libs-x.x.x-1.i386.rpm. As a result,
a copy of the license (LICENCE.xerces_xalan) has been provided.
  
For more information about Xalan visit:
    http://xalan.apache.org/

The source code is also available at the above Web site.

-- VIII -- LIBGCRYPT --

The OVAL Interpreter uses the open-source Libgcrypt library.  The binary
was compiled with version 1.4.6 (1.1.12 on Solaris) of the Libgcrypt library.
From the Libgcrypt Web site:

	"Libgcrypt is a general purpose cryptographic library based on the
	code from GnuPG. It provides functions for all cryptographic building
	blocks: symmetric ciphers, hash algorithms, MACs, public key
	algorithms, large integer functions, random numbers and a lot of
	supporting functions."
  
For more information about Libgcrypt visit:
    http://www.gnupg.org/related_software/libraries.en.html#lib-libgcrypt

The source code is also available at the above Web site.

-- IX -- OPENLDAP --
  
The OVAL Interpreter utilizes the open source OpenLDAP library to
execute the LDAP probe.  The binary was compiled with version 2.3.27 of the
OpenLDAP library.  From the OpenLDAP Web site:

  "The OpenLDAP Project is a collaborative effort to develop a robust,  
  commercial-grade, fully featured, and open source LDAP suite of  
  applications and development tools. The project is managed by a
  worldwide community of volunteers that use the Internet to communicate,
  plan, and develop the OpenLDAP Suite and its related documentation."

For more information about OpenLDAP visit:
  http://www.openldap.org/

The source code is also available at the above Web site.

-- X -- TROUBLESHOOTING --

*********************************** 

Q:

I am trying to run the OVAL Interpreter but all I get is a
message saying "You must supply the MD5 hash for the data file or use
the -m command to skip the MD5 check."

A:

The OVAL Interpreter is set up to validate that the
Data file has not been tampered with by checking the MD5 hash (or 
checksum) generated from the data file on your computer with an MD5
hash provided by MITRE on the OVAL Web site.  In order to start the
OVAL Interpreter you must provide this MD5 hash.  From the command
line, type the program name 'ovaldi.exe' then add a space and type
the MD5 hash value from the OVAL Web site.  For example:

    ovaldi.exe 897237212305b2d7a4dd5fa6b4e226fc

If you want to use some of the advanced option flags, place them
between the program name and the MD5 hash.  For example:

    ovaldi.exe -i myData.xml 897237212305b2d7a4dd5fa6b4e226fc

If you do not want to supply the MD5 hash and are confident that the
Data file on your computer has not been tampered with, you can supply
the -m flag to skip the MD5 check.  For example:

    ovaldi.exe -m

Be careful when using the -m option.  A data file that has been
tampered with can cause misleading results to be generated.  MITRE
recommends that you always supply a valid MD5 hash from the OVAL Web
site when using the OVAL Interpreter.

*********************************** 
Q:

I ran the OVAL Interpreter with the -p flag and I got a bunch of 
errors.  Should I worry about them?  The program seemed to run 
fine.

A:

Most of the messages produced when the -p flag is set are the result
of registry keys and files not existing on your system.  This kind of
message is informational, rather than an error.  In more detail:

An OVAL Definition may have tests to retrieve information about
specified objects (files, registry keys, etc).  On some systems, 
these objects simply do not exist, perhaps because a particular
application or software component is not installed.

For example, installed patches are determined by the existence of
certain registry keys.  If a patch is not installed, then the 
registry key will not exist.  When the OVAL Interpreter evaluates an OVAL 
Definition, it attempts to collect information about this registry
key on the system.  If the key is not found, the patch is not
installed.  Since these missing objects are not really errors, they
are not normally reported to the user, but appear when the -v option
is specified.

Scan through the list of messages produced by the -p flag and look 
for errors that are not common.  These could signify something that
is working incorrectly.

*********************************** 
Q:

I ran the OVAL Interpreter and I noticed that it says it has exited
with a code of 0 or 1.  What does this mean?

A:

The exit codes of 0 or 1 represent whether or not the OVAL Interpreter
has exited successfully or with an error.  An exit code of 0 represents
that the OVAL Interpreter has completed successfully where an exit code
of 1 represents that the OVAL Interpreter has completed with an error.
If the OVAL Interpreter exits with a code of 1, check to make sure that
all of the command line options and their values, if any, are valid.

-- XI -- REPORTING PROBLEMS --

To report a problem with OVAL Interpreter, please add a bug tracker item,
or create a post on the OVAL Interpreter Help Project Forum, with a brief
description of the problem on the OVAL Interpreter SourceForge site
(http://sourceforge.net/projects/ovaldi/support). Please include the
platform that the OVAL Interpreter was run on, the version of the OVAL
Interpreter, and the OVAL Definitions document that was used.

To report a problem with an OVAL Definition from the OVAL Repository,
please send an email to the 'oval-discussion-list' with the id of the 
OVAL Definition in question and a detailed description of the issue. To
subscribe to the list, go to the Forum Sign-Up page on the
OVAL Web Site and join the "OVAL Repository Forum".

http://oval.mitre.org/community/registration.html


-- XII ­ USEFUL LINKS --

OVAL Web site -- http://oval.mitre.org/

OVAL Interpreter SourceForge Site -- http://sourceforge.net/projects/ovaldi/

OVAL Repository -- http://oval.mitre.org/repository/

OVAL Terms of Use -- http://oval.mitre.org/oval/about/termsofuse.html

OVAL Interpreter Wiki -- http://sourceforge.net/apps/mediawiki/ovaldi/index.php?title=Main_Page

CCE -- http://cce.mitre.org/

CPE -- http://cpe.mitre.org/

CWE -- http://cwe.mitre.org/

CVE -- http://cve.mitre.org/

Making Security Measurable - http://msm.mitre.org/

----------------------------------------------------------
OVAL is sponsored by US-CERT at the U.S. Department of Homeland Security. OVAL
and the OVAL logo are trademarks of The MITRE Corporation. Copyright 2002-2012,
The MITRE Corporation (www.mitre.org).
