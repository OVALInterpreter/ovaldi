%define name	ovaldi
%define version	5.8.1
%define release	1

Summary:	The reference interpreter for the Open Vulnerability and Assessment Language
Name:		%{name}
Version:	%{version}
Release:	%{release}
Source0:	http://oval.mitre.org/DontDownload/oval/download/ovaldi-%{version}-src.tar.bz2
License:	BSD
Group:		System/Configuration/Other
BuildRoot:	%{_tmppath}/%{name}-buildroot
Prefix:		%{_prefix}
BuildArch: 	i386

%description
The OVAL Interpreter is a freely available reference 
implementation created to show how information can be
collected from a computer for testing, to evaluate and
carry out the OVAL definitions for that platform, and to
report the results of the tests.

MITRE developed the OVAL Interpreter to demonstrate the 
usability of OVAL definitions, and for definition writers
to use to ensure correct syntax and adherence to the OVAL
Language during the development of draft definitions. It is
not a fully functional scanning tool and has a simplistic user
interface, but running the Interpreter will provide you with
a list of true/false results and their references (e.g., CVE names)
for all input OVAL definitions.

MITRE offers the Interpreter, source code, schemas, and data files
at no cost to further the OVAL effort and in hopes that individuals
and organizations will build and expand upon them. If you are 
interested in contributing to the OVAL Interpreter please send an
email to oval@mitre.org.

To run:

/usr/sbin/ovaldi.sh

%package libs
Summary: Libraries required to run the OVAL Interpreter.
Group: Applications/System
%description libs
This package contains the Xerces and Xalan XML libraries needed to run
the OVAL Interpreter. These libraries are available from the Apache
Foundation, but are not yet packaged for Redhat.

%prep
[ -n "$RPM_BUILD_ROOT" -a "$RPM_BUILD_ROOT" != / ] && rm -rf $RPM_BUILD_ROOT
%setup -q -n ovaldi-%{version}-src

%build
cd project/linux
mkdir -p Release/obj
make

%install
/bin/mkdir -p $RPM_BUILD_ROOT/usr/sbin
/bin/mkdir -m 700 -p $RPM_BUILD_ROOT%{_libdir}/ovaldi
/bin/mkdir -m 700 -p $RPM_BUILD_ROOT/usr/share/ovaldi
/bin/mkdir -m 700 -p $RPM_BUILD_ROOT/var/log/ovaldi
/bin/mkdir -p $RPM_BUILD_ROOT%{_mandir}/man1

/bin/cp project/linux/Release/ovaldi $RPM_BUILD_ROOT/usr/sbin
/bin/cp project/linux/ovaldi.sh $RPM_BUILD_ROOT/usr/sbin

/usr/bin/gzip docs/ovaldi.1
/bin/cp docs/ovaldi.1.gz $RPM_BUILD_ROOT%{_mandir}/man1

/bin/cp xml/results_to_html.xsl $RPM_BUILD_ROOT/usr/share/ovaldi
/bin/cp xml/oval-definitions-schematron.xsl $RPM_BUILD_ROOT/usr/share/ovaldi
/bin/cp xml/*.xsd $RPM_BUILD_ROOT/usr/share/ovaldi

if grep "release 5" /etc/redhat-release &> /dev/null ; then
  /bin/cp project/linux/EL5/libxerces-c.so.28.0 $RPM_BUILD_ROOT%{_libdir}/ovaldi
  /bin/cp project/linux/EL5/libxalan-c.so.110.0 $RPM_BUILD_ROOT%{_libdir}/ovaldi
  /bin/cp project/linux/EL5/libxalanMsg.so.110.0 $RPM_BUILD_ROOT%{_libdir}/ovaldi
else
  echo "Unsupported Redhat version. Exiting."
  exit 1
fi

%clean
[ -n "$RPM_BUILD_ROOT" -a "$RPM_BUILD_ROOT" != / ] && rm -rf $RPM_BUILD_ROOT
/bin/rm -fr $RPM_BUILD_ROOT/%{name}

%post libs
# Add %{_libdir}/ovaldi to the end of /etc/ld.so.conf if it's not yet present.
if [ `/bin/egrep -c '^%{_libdir}/ovaldi' /etc/ld.so.conf ` -lt 1 ] ; then
        /bin/echo "%{_libdir}/ovaldi" >>/etc/ld.so.conf
#	/sbin/ldconfig
fi
/sbin/ldconfig

%preun libs
if [ $1 = 0 ]; then
    #// Do stuff specific to uninstalls
    if [ -e %{_libdir}/ovaldi/libxerces-c.so.28 ] ; then
        /bin/rm %{_libdir}/ovaldi/libxerces-c.so.28
    fi
    if [ -e %{_libdir}/ovaldi/libxerces-c.so ] ; then
        /bin/rm %{_libdir}/ovaldi/libxerces-c.so
    fi
    if [ -e %{_libdir}/ovaldi/libxalan-c.so.110 ] ; then
        /bin/rm %{_libdir}/ovaldi/libxalan-c.so.110
    fi
    if [ -e %{_libdir}/ovaldi/libxalan-c.so ] ; then
        /bin/rm %{_libdir}/ovaldi/libxalan-c.so
    fi
    if [ -e %{_libdir}/ovaldi/libxalanRMsg.so.110 ] ; then
        /bin/rm %{_libdir}/ovaldi/libxalanMsg.so.110
    fi
    if [ -e %{_libdir}/ovaldi/libxalanMsg.so ] ; then
        /bin/rm %{_libdir}/ovaldi/libxalanMsg.so
    fi
fi

%postun libs
if [ $1 = 0 ]; then
   # // Do stuff specific to uninstalls
if  [ `/bin/egrep -c '^%{_libdir}/ovaldi' /etc/ld.so.conf ` -gt 0 ] ; then
        /bin/egrep -v '^%{_libdir}/ovaldi$' /etc/ld.so.conf >%{_libdir}/etc.ld.so.conf.uninstalling-oval
        /bin/chown root:root %{_libdir}/etc.ld.so.conf.uninstalling-oval
        /bin/chmod 644 %{_libdir}/etc.ld.so.conf.uninstalling-oval
        /bin/mv %{_libdir}/etc.ld.so.conf.uninstalling-oval /etc/ld.so.conf
fi
/sbin/ldconfig
fi
if [ $1 = 1 ]; then
	/sbin/ldconfig
   # // Do stuff specific to upgrades
fi

%files 
%defattr(-,root,root,0700)
%doc docs/terms.txt docs/README.txt docs/version.txt
%doc %{_mandir}/man1/ovaldi.1.gz
%defattr(-,root,root,0600)
%attr(0700,root,root)  /usr/sbin/ovaldi
%attr(0700,root,root)  /usr/sbin/ovaldi.sh
%attr(0700,root,root)  /var/log/ovaldi
/usr/share/ovaldi/oval-definitions-schematron.xsl
/usr/share/ovaldi/results_to_html.xsl
/usr/share/ovaldi/*.xsd

%files libs
%{_libdir}/ovaldi/libxerces-c.so.28.0
%{_libdir}/ovaldi/libxalan-c.so.110.0
%{_libdir}/ovaldi/libxalanMsg.so.110.0

%changelog

* Wed May 12 2010 Bryan Worrell <bworrell@mitre.org> 5.5.4-1.0
* Updated for libxerces-c.so.28 

* Fri Oct 10 2008 Bryan Worrell <bworrell@mitre.org> 5.5.4-1.0
* Updated spec file as per instructions from Steve 'Ashcrow' Milner <me@stevemilner.org>

* Thu Oct 02 2008 Jonathen Baker <bakerj@mitre.org> 5.5.0-1.0
* Updated to 5.5 source.

* Thu Jun 28 2007 Jonathen Baker <bakerj@mitre.org> 5.3.0-1.0
* Updated to 5.3 source.

* Mon Jan 23 2007 Brendan Dolan-Gavitt <brendandg@mitre.org> 5.2.0-1.0
* Updated to 5.2 source.

* Mon Oct 30 2006 Brendan Dolan-Gavitt <brendandg@mitre.org> 5.1.0-1.0
* Created subpackage for libs, updated to 5.1 source. Some .spec cleanup performed.

* Tue May 02 2006 Matt Burton <mburton@mitre.org> 5.0.0-1.0
* New RPM using version 5 source & new SPEC file

* Tue Oct 11 2005 Matt Burton <mburton@mitre.org> 4.3.0-1.3
* Rebuild RPM with new source

* Thu Aug 11 2005 Matt Burton <mburton@mitre.org> 4.2.0-1.3
* Added new schema files (independant, unix and xmldsig), new source

* Tue Apr 12 2005 Matt Burton <mburton@mitre.org> 4.1.0-1.2
* Added libxalan libraries and new source to RPM

* Tue Feb 22 2005 Matt Burton <mburton@mitre.org> 4.0.0-1.1
* Changed SPEC file to use the 2.6 xerces library

* Thu Jan 20 2005 Jay Beale <jay@bastille-linux.org> 4.0.0-1.0
* Modified RPM for ovaldi 4.0.0's new structure

* Mon Oct 18 2004 Jay Beale <jay@bastille-linux.org> 3.4.0-1.0
* Rebuilt RPM with new source

* Wed Sep 1 2004 Jay Beale <jay@bastille-linux.org> 3.3.0-1.0
* Rebuilt RPM with new source

* Wed Jul 14 2004 Jay Beale <jay@bastille-linux.org> 3.2.0-1.0
* Rebuilt RPM with new source

* Sun Jun 6 2004 Jay Beale <jay@bastille-linux.org> 3.1.0-1.0
- Rebuilt with new locations and such

* Thu May 6 2004 Jay Beale <jay@bastille-linux.org> 1.0.0-1.3
- Removed xerces libraries symlinking

* Wed Apr 28 2004 Jay Beale <jay@bastille-linux.org> 1.0.0-1.2
- Updated to new version

* Wed Apr 21 2004 Jay Beale <jay@bastille-linux.org> 1.0.0-1.1
- Changed oval.xml to definitions.xml

* Mon Apr 19 2004 Jay Beale <jay@bastille-linux.org> 1.0.0-1.0
- Initial package creation - enjoy
