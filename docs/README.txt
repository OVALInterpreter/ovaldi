   	

****************************************************

                  OVAL Interpreter

 Copyright (c) 2002 - 2009 - The MITRE Corporation

****************************************************

The MITRE Corporation developed the Open Vulnerability and Assessment
Language (OVAL) Interpreter to provide the OVAL Community with an open
source reference implementation of the language and its Definitions.
The OVAL Interpreter uses OVAL Definitions to gather security relevant
configuration information on a computer (e.g., rpm parameters, 
registry keys, file information, etc.), analyze the information for 
vulnerabilities and configuration issues, and report the results of the 
analysis for each definition. 

You may download the Interpreter to any computer you wish, and to as
many computers as you wish.  

BY USING THE OVAL INTERPRETER, YOU SIGNIFY YOUR ACCEPTANCE OF THE
TERMS AND CONDITIONS OF USE.  IF YOU DO NOT AGREE TO THESE TERMS, DO NOT
USE THE INTERPRETER.

Please refer to the terms.txt file or 
http://oval.mitre.org/about/bsd_license.html 
for more information.


-- CONTENTS --

  I    INSTALLATION
         A. Red Hat Linux 
         B. Sun Solaris
         C. Microsoft Windows 
  II   USING THE INTERPRETER
         A. Required Privileges
         B. Data Protection
         C. Obtaining Updated Data Files
         D. Quick Usage Guide
         E. Advanced Usage
  III  INPUT AND OUTPUT
         A. Input
         B. Output
  IV   PCRE
  V    XERCES
  VI   XALAN
  VII  OPENSSL
  VIII TROUBLESHOOTING
  IX   REPORTING PROBLEMS
  X    USEFUL LINKS


-- I -- INSTALLATION --

  Download an appropriate installation file from the OVAL Web site,
  (http://oval.mitre.org) by following the 'Download' link to the
  Interpreters.  Once you have downloaded the Interpreter, follow the
  corresponding instructions to install the OVAL Interpreter on
  your system.

  A. Red Hat Linux Installation

       The OVAL Interpreter can be installed and run on Red
       Hat Enterprise Linux 3 and 4.

       Install the RPM simply by running the following command:

       # rpm -ivh <OVAL Interpreter RPM>

       The RPM places the following files on the system.  To learn their
       exact location after install, run:  rpm -ql ovaldi.

       Executables:
       ovaldi.sh - Shell Script to simplify execution of the Definition
	               Interpreter
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
       OVAL schema files
       evaluation-ids.xsd - XML Schema that defines a format for inputing a 
                            set of definition ids to evaluate.
       xmldsig-core-schema.xsd - XML digital signature schema

       XSL:
       oval-definitions-schematron.xsl - OVAL Definition Schematron validation rules
       results_to_html.xsl - A simple xsl that formats an oval-results document as html
 
       The .so file for the Xerces library must be made accessible to
       the OVAL Interpreter binary.  The ovaldi RPM places a 
       pre-compiled version in /usr/lib/ovaldi/ and appends this directory
       to your /etc/ld.so.conf file.

       To run the OVAL Interpreter, run:

       # /usr/sbin/ovaldi.sh

       A result summary will be output to the screen, and detailed System
       Characteristic and Output Results will be output to /var/log/ovaldi.
	
  B. Sun Solaris Installation

       Not yet supported.


  C. Microsoft Windows Installation
     
       The OVAL Interpreter can be installed and run on
       Microsoft Windows NT/2000/XP/Server 2003/Vista/Server 2008.

       Install the Interpreter by running the installer
       executable -  	ovaldi-X.X.X-setup.exe

       The installer is a self-extracting zip archive that prompts
       the user for an installation directory 
       ('C:\Program Files\OVAL\ovaldi\' by default)
       and installs the Interpreter and its supporting files.

       Executables:
       ovaldi.exe - OVAL Interpreter binary

       Libraries:
       xerces-c_2_7_0.dll - Xerces XML library
       msvcr71.dll - Microsoft C Runtime Library
       pcre.dll - Perl-compatible regular-expression library
       Xalan-C_1_10.dll - Xalan XSLT library
       XalanMessages_1_10.dll - Xalan XSLT library

       Documentation:
       README.txt - This file
       terms.txt - License and Terms of Use
       version.txt - OVAL Interpreter change log

       XML Schema:
       OVAL schema files
       evaluation-ids.xsd - XML Schema that defines a format for inputing a 
                            set of definition ids to evaluate.
       xmldsig-core-schema.xsd - XML digital signature schema

       XSL:
       oval-definitions-schematron.xsl - OVAL Definition Schematron validation rules
       results_to_html.xsl - A simple xsl that formats an oval-results document as html

-- II -- USING THE INTERPRETER --

  A. Required Privileges -- IMPORTANT NOTE:

     In order to collect all of the system configuration data required
     to correctly evaluate OVAL Definitions, the OVAL Interpreter
     MUST BE RUN WITH ADMINISTRATOR/ROOT PRIVILEGES.

     Certain system data referenced by OVAL Definitions is only
     available to privileged accounts.  This includes information
     about running processes, and potentially registry key and file
     information (depending on local security settings).  While it is 
     possible to run the OVAL Interpreter as a non-privileged user, 
     the results of the analysis may not convey the true state of the
     system.


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
     the latest updates before running the Interpreter to ensure
     that you are using the most up-to-date Definitions.  Make sure
     to note the MD5 signature of the data file, as it will be
     needed to execute the Interpreter.

     OVAL Repository Downloads page:
     http://oval.mitre.org/repository/download/index.html

     Additionally it is recommended that you join the 
     "oval-discussion-list" email list.  A lightly moderated public
	 forum for discussion realted to new and previously posted OVAL
	 Repository content, as well as the vulnerabilities and configuration
	 issues themselves that affect definition writing. To
     subscribe to the list, go to the Forum Sign-Up page on the
     OVAL Web Site.

     http://oval.mitre.org/community/registration.html


  D. Quick Usage Guide:

  1) As the Interpreter is only a reference implementation, it has
     purposefully been designed as solely a command-line utility.
     Therefore, to execute the Interpreter, you will first have to
     open a command window, and change to the Interpreter installation
     directory.

  2) Download a new data file and move it into the Interpreter
     installation directory. Rename the file to 'definitions.xml'.

  3) Run the Interpreter, supplying the MD5 checksum of the data file as
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
      -a dir name  = path to the directory that contains the OVAL schema and other xml resources. DEFAULT="xml"
	  -m           = do not verify the oval-definitions file with an MD5 hash
      -n           = perform Schematron validation of the oval-definitions file.
      -c filename  = path to xsl for oval-definitions Schematron validation. 
                     DEFAULT="oval-definitions-schematron.xsl"    

     Data Collection Options:
      -i filename  = path to input System Characteristics file. Evaluation will
                     be based on the contents of the file.

     Result Output Options:
      -d filename  = save system-characteristics data to the specified XML file.
                     DEFAULT="system-characteristics.xml"
      -r filename  = save oval-results to the specified XML file.
                     DEFAULT="oval-results.xml"
      -s           = do not apply a stylesheet to the results xml.
      -t filename  = apply the specified xsl to the results xml.
                     DEFAULT="results_to_html.xsl"
      -x filename  = output xsl transform results to the specified file.
                     DEFAULT="results.html"

     Other Options:
      -l <integer> = Log messages at the specified level. (DEBUG = 1, INFO = 2, MESSAGE = 3, FATAL = 4). DEFAULT=2
      -p           = print all information and error messages.
      -z           = return md5 of current oval-definitions file.


     In more detail: 
     -h -- Displays command line options.

     -o -- Specifies the pathname of the OVAL Definition file to use.
           If none is specified than the Interpreter will default to
           "definitions.xml" in the Interpreter directory.

     -v -- Specifies the pathname of the external variable file to use.
           If none is specified then the Interpreter will default to
           "external-variables.xml" in the Interpreter directory.           

     -e -- Specifies a set of OVAL Definition ids to evaluate in the input
           OVAL Definitions Document. Definition Ids should be comma 
           separated without spaces. All OVAL Definitions in the list
           are evaluated in the input OVAL Definitions Document if they
           exist in the input document. Any Definition Ids not found will
           be assigned an error status. Any Definitions in the input 
           document that are not in the list will be marked as 'Not 
           Evaluated'.            

     -f -- Specifies a the path to an xml file containing the set of
           OVAL Definition ids to evaluate in the input OVAL Definitions
           Document. The input xml file must comply with the evaluation-ids.xsd.
           All OVAL Definitions in the input evaluation-ids document are 
           evaluated if they exist in the input OVAL Definitions Document. Any
           Definition Ids not found will be assigned an error result. Any 
           Definitions in the input document that are not in the list will be 
           assigned a 'Not Evaluated' evaluated.  

     -a -- Specifies the pathname of the directory that contains the OVAL
           schema and other xml resources. DEFAULT="xml"
	 
	 -m -- Run without requiring an MD5 checksum.  Running the
           Interpreter with this option DISABLES an important security
           feature.  In normal usage, a trusted checksum provided on the
           command line is used to verify the integrity of the OVAL
           Definitions file.

           Use of this option is recommended only when testing your own
           draft definitions before submitting them to the OVAL Community 
           Forum for public review.

     -n -- If set run Schematron validation on the input OVAL Definitions
           Document. Schematron validation is currently optional. In the 
           future the OVAL Compatibility program will likely require 
           Schematron validation. When Schematron validation is required by the
           OVAL Language this reference implementation will also require 
           Schematron validation.  
                     
     -c -- Specifies the pathname of the oval-definitions-schematron.xsl
           to be used for Schematron validation. If none is specified 
           then the Interpreter will default to 
           "oval-definitions-schematron.xsl" in the Interpreter directory.

     -i -- Specifies the pathname of a System Characteristics file that
           is to be used as the basis of the analysis.  In this mode,
           the Interpreter does not perform data collection on the
           local system, but relies upon the input file, which may
           have been generated on another system.
          
     -d -- Specifies the pathname of the file to which collected
           configuration data is to be saved. This data is stored in the
           format defined by the Systems Characteristics Schema. 

     -r -- Specifies the pathname of the file to which analysis results
           are to be saved.  This data is stored according to the format
           defined by the OVAL Results Schema.  If none is specified 
           than the Interpreter will default to "results.xml" in the 
           Interpreter directory.

     -s -- If set do not apply the xsl to the OVAL Results xml.

     -t -- Specifies the pathname of the xsl file which should be used to
           transform the oval results. If none is specified then the Interpreter 
           will default to "results_to_html.xsl" in the Interpreter directory.

     -x -- Specifies the pathname of the file which xsl transform results
           are to be saved.  If none is specified then the Interpreter 
           will default to "results.html" in the Interpreter directory.

     -l -- Logging level.  Log messages at the specified level. 
	       (DEBUG = 1, INFO = 2, MESSAGE = 3, FATAL = 4). DEFAULT=2

	-p -- Verbose output.  Print all information and error message to the 
           console.

     -z -- Calculates and prints to the screen the MD5 checksum of the
           current data file (definitions.xml by default, or as specified
           by the -o option).  This can be used to manually compare the
           current file with the trusted checksum available from the 
           OVAL Web site.

     MD5Hash -- The MD5 checksum expected for the current data file
           (definitions.xml by default, or as specified by the -o option).
           The Interpreter calculates the actual checksum of the
           Data file, and compares it to this value provided on the
           command line, to verify the data file's integrity.  Checksums
           are available from the OVAL Web site.

           The checksum verification ensures that the data file has not
           been modified: that the OVAL definitions have not been 
           tampered with, or potentially malicious content added.
           Unless the -m option is specified, the MD5Hash is REQUIRED to
           run the Interpreter.


-- III -- INPUT AND OUTPUT --

The input and the output formats used by the OVAL Interpreter are
defined by individual XML Schemas. The data collected about a system is
considered input and its format is defined by System Characteristics
Schema. The detailed results generated by a system analysis are considered
output and their format is defined by the OVAL Results Schema.  These XML
Schemas have been provided to allow other applications to more easily
implement their own system information collection utilities and manipulate
the results of an analysis for inclusion in other applications.

  A. Input

     The OVAL Interpreter analysis engine accepts an XML data
     model in the format defined by the System Characteristics Schema
     (system-characteristics-schema.xsd). This data model can be
     generated locally by the Interpreter's data collection engine, or
     can be provided at runtime using the -i command line option.

     If the data model is provided at runtime, the only requirement is
     that it be formatted according to the System Characteristics
     Schema.  The data can be gathered using a tool other than
     the OVAL Interpreter, on another system, running a different
     operating system, but as long as the data is in the correct format,
     analysis can be performed.  What this means is, not only can one 
     system be used to run the analysis for a number of systems, but 
     data models can be altered/fabricated to conduct 'what-if' scenarios.

  B. Output

     The OVAL Interpreter provides detailed output from an analysis
     according to the OVAL Results Schema.  This output includes the
     outcome of individual tests, as well as each Definition as a
     whole.

     The OVAL Results Schema is provided to allow the results of an
     analysis to be utilized by other applications.  For example, an
     application could convert the detailed results to HTML for
     display on a Web site, or use the detailed results to determine
     what if any patches should be applied to a system.  As mentioned
     above, the -r command line option may be used to redirect the
     output to a location other than the default.     


-- IV -- PCRE --

The OVAL Interpreter uses the open source PCRE library.  The
binary was compiled with version 6.3 of the pcre library.  
From the PCRE Web site:

  "Regular expression support is provided by the PCRE library 
   package, which is open source software, written by Philip Hazel,
   and copyright by the University of Cambridge, England."

For more information about PCRE visit:
    http://www.pcre.org

To download the source code for PCRE, please use the following ftp
site:
    ftp://ftp.csx.cam.ac.uk/pub/software/programming/pcre/


-- V -- XERCES --

The OVAL Interpreter uses the open source Xerces library.  The
binary was compiled with version 2.7 of the xerces-c library.  From
the Xerces Web site:

  "This product includes software developed by the Apache Software
  Foundation (http://www.apache.org/)."

For more information about Xerces visit:
    http://xerces.apache.org/

The source code is also available at the above Web site.


-- VI -- XALAN --

The OVAL Interpreter uses the open source Xalan library.  The
binary was compiled with version 1.10 of the xalan-c library.  From
the Xalan Web site:

  "This product includes software developed by the Apache Software
  Foundation (http://www.apache.org/)."

For more information about Xalan visit:
    http://xalan.apache.org/

The source code is also available at the above Web site.

-- VII -- OPENSSL --

The OVAL Interpreter uses the open-source OpenSSL library.

  The OpenSSL toolkit stays under a dual license, i.e. both the conditions of
  the OpenSSL License and the original SSLeay license apply to the toolkit.
  See below for the actual license texts. Actually both licenses are BSD-style
  Open Source licenses. In case of any license issues related to OpenSSL
  please contact openssl-core@openssl.org.

Also, see the file LICENSE.openssl.

For more information about OpenSSL visit:
    http://openssl.org/

The source code is also available at the above Web site.

-- VIII -- TROUBLESHOOTING --

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
registry key will not exist.  When the Interpreter evaluates an OVAL 
definition, it attempts to collect information about this registry
key on the system.  If the key is not found, the patch is not
installed.  Since these missing objects are not really errors, they
are not normally reported to the user, but appear when the -v option
is specified.

Scan through the list of messages produced by the -p flag and look 
for errors that are not common.  These could signify something that
is working incorrectly.


-- IX -- REPORTING PROBLEMS --

To report a problem with the OVAL Interpreter, please post a bug report
on the OVAL Interpreter SourceForge site 
(https://sourceforge.net/projects/ovaldi/).

To report a problem with an OVAL Definition from the OVAL Repository,
please send an email to the 'oval-discussion-list' with the id of the 
definition in question and a detailed description of the issue. To
subscribe to the list, go to the Forum Sign-Up page on the
OVAL Web Site and join the "OVAL Repository Forum".

http://oval.mitre.org/community/registration.html


-- X ­ USEFUL LINKS --

OVAL Web site -- http://oval.mitre.org/

OVAL Interpreter SourceForge Site -- http://sourceforge.net/projects/ovaldi/

OVAL Repository -- http://oval.mitre.org/repository/

Terms of Use -- http://oval.mitre.org/about/bsd_license.html

CCE -- http://cpe.mitre.org/

CPE -- http://cce.mitre.org/

CWE -- http://cwe.mitre.org/

CVE -- http://cve.mitre.org/

Making Security Measurable - http://msm.mitre.org/

----------------------------------------------------------
OVAL is sponsored by US-CERT at the U.S. Department of Homeland
Security. OVAL and the OVAL logo are trademarks of The MITRE
Corporation. Copyright 2009, The MITRE Corporation (www.mitre.org).
