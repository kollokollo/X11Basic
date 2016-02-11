#
# spec file for package X11Basic (Version 1.15), by Markus Hoffmann
#
# Copyright  (c)  Markus Hoffmann 1997-2007
#
# please send bugfixes or comments to kollo@users.sourceforge.net.
#

Summary: A Basic Interpreter with X11-Graphics capabilities
Vendor: Markus Hoffmann
Name: X11Basic
Version: 1.15
Release: 3
Copyright: GPL
Group: Development/Languages
Source: http://x11-basic.sourceforge.net/X11Basic-1.15.tar.gz
URL: http://x11-basic.sourceforge.net/
Packager: Markus Hoffmann <kollo@users.sourceforge.net>
BuildRequires:	XFree86-devel
BuildRequires:	readline
BuildRequires:	readline-devel
BuildRequires:	X11Basic

Requires:	readline

BuildRoot: /var/tmp/%{name}-buildroot

%description

X11-Basic implements a structured BASIC programming language with
X11-graphics. The structure of the language is similar to the ATARI ST
GFA-Basic. GFA-Basic programs should run with only few changes.

This package includes the basic interpreter named xbasic. It can be used as a
shell which can run basic-scripts. You can make excecutable scripts p.ex.
*.cgi-Scripts for handling web-input. A pseudo compiler (xbc) is included which
makes stand alone binaries out of the scripts. An experimental version of the
X11-Basic bytecode compiler and the X11-Basic virtual machine is now included.
Also an ANSI-Basic-to-X11-Basic-converter (bas2x11basic) is included.

You will find the compiler sourcecode and some other sample programs in
/usr/share/doc/packages/X11Basic/examples. A variety of other sample programs
can be found in X11-Basic-examples-1.15.zip, which you can download from the
homepage (http://x11-basic.sourceforge.net/). Also an up-to-date pdf-version
of the manual can be found there.

Authors:
--------
    Markus Hoffmann <kollo@users.sourceforge.net>

%prep
%setup -q

%build
(cd ./src ; ./configure )
%install
(cd ./src ; make xbasic x11basic.a xbvm bytecode xbc bas2x11basic)
rm -rf $RPM_BUILD_ROOT
install -d $RPM_BUILD_ROOT%{_mandir}/man1
mkdir $RPM_BUILD_ROOT/usr/bin
mkdir $RPM_BUILD_ROOT/usr/lib
install -s -m 755 src/xbasic.dyn $RPM_BUILD_ROOT/usr/bin/xbasic
install -m 755 src/x11basic.a $RPM_BUILD_ROOT/usr/lib/x11basic.a
install -s -m 755 src/libx11basic.so $RPM_BUILD_ROOT/usr/lib/libx11basic.so
install -s -m 755 src/xbc $RPM_BUILD_ROOT/usr/bin/xbc
install -s -m 755 src/xbvm $RPM_BUILD_ROOT/usr/bin/xbvm
install -s -m 755 src/bytecode $RPM_BUILD_ROOT/usr/bin/xbbc
install -s -m 755 src/bas2x11basic $RPM_BUILD_ROOT/usr/bin/bas2x11basic
install -m 644 doc/man-pages/x11basic.1 $RPM_BUILD_ROOT%{_mandir}/man1/xbasic.1
install -m 644 doc/man-pages/x11basic.1 $RPM_BUILD_ROOT%{_mandir}/man1/x11basic.1
install -m 644 doc/man-pages/xbc.1 $RPM_BUILD_ROOT%{_mandir}/man1/xbc.1
install -m 644 doc/man-pages/xbbc.1 $RPM_BUILD_ROOT%{_mandir}/man1/xbbc.1
install -m 644 doc/man-pages/xbvm.1 $RPM_BUILD_ROOT%{_mandir}/man1/xbvm.1
install -m 644 doc/man-pages/bas2x11basic.1 $RPM_BUILD_ROOT%{_mandir}/man1/bas2x11basic.1

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%doc README COPYING INSTALL ACKNOWLEGEMENTS AUTHORS RELEASE_NOTES
%doc doc/X11-Basic-manual.txt doc/editors
%doc examples
%doc %{_mandir}/man1/xbasic.1.gz
%doc %{_mandir}/man1/x11basic.1.gz
%doc %{_mandir}/man1/xbc.1.gz
%doc %{_mandir}/man1/xbbc.1.gz
%doc %{_mandir}/man1/xbvm.1.gz
%doc %{_mandir}/man1/bas2x11basic.1.gz

/usr/bin/xbasic
/usr/bin/xbc
/usr/bin/xbbc
/usr/bin/xbvm
/usr/bin/bas2x11basic
/usr/lib/libx11basic.so
/usr/lib/x11basic.a

%changelog
* Tue Apr 07 2007 Markus Hoffmann <kollo@users.sourceforge.net>
  New release (1.14)
* Tue Mar 17 2007 Markus Hoffmann <kollo@users.sourceforge.net>
  Corrected typos in description
* Tue Sep 16 2003 Markus Hoffmann <kollo@users.sourceforge.net>
  Added bas2x11basic + man page
* Sun Jun 22 2003 Markus Hoffmann <kollo@users.sourceforge.net>
  New release (1.09)
* Sun Jan 26 2003 Markus Hoffmann <kollo@users.sourceforge.net>
  changed manual, my email address has changed, changed installdir
* Tue Dec 16 2002 Markus Hoffmann <m.hoffmann@uni-bonn.de>
  included xbc
* Tue Mar 07 2002 Markus Hoffmann <m.hoffmann@uni-bonn.de>
  included manual
* Tue Jan 01 2002 Markus Hoffmann <m.hoffmann@uni-bonn.de>
  2nd release
* Tue Aug 28 2001 Markus Hoffmann <m.hoffmann@uni-bonn.de>
  1st release
