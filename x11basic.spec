#
# spec file for package X11Basic (Version 1.23), by Markus Hoffmann
#
# Copyright  (c)  Markus Hoffmann 1997-2015
#
# please send bugfixes or comments to kollo@users.sourceforge.net.
#
Name: x11basic
Version: 1.23
Release: 1
License: GPL
Group: Development/Languages
Summary: A Basic Interpreter with X11-Graphics capabilities
Source: http://x11-basic.sourceforge.net/X11Basic-%{version}.tar.gz
URL: http://x11-basic.sourceforge.net/
BuildRequires:	autoconf
BuildRequires:	libX11-devel
BuildRequires:	readline
BuildRoot: %{_tmppath}/%{name}-buildroot
Vendor: Markus Hoffmann <kollo@users.sourceforge.net>

# Reste des alten Spec-files

Packager: Markus Hoffmann <kollo@users.sourceforge.net>

BuildRequires:	readline-devel
BuildRequires:	X11Basic

Requires:	readline


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
can be found in X11-Basic-examples-1.17.zip, which you can download from the
homepage (http://x11-basic.sourceforge.net/). Also an up-to-date pdf-version
of the manual can be found there.

%prep
%setup -q -n X11Basic-%{version}

%build
#cd ./src
./configure
make
make xbasic x11basic.a xbvm xbbc
touch xbc
touch bas2x11basic

%install
rm -rf $RPM_BUILD_ROOT

install -Dm 755 x11basic.a $RPM_BUILD_ROOT%{_libdir}/x11basic.a
install -Dm 644 doc/man-pages/x11basic.1 $RPM_BUILD_ROOT%{_mandir}/man1/x11basic.1
install -Dm 644 doc/man-pages/x11basic.1 $RPM_BUILD_ROOT%{_mandir}/man1/xbasic.1
install -Dm 644 doc/man-pages/xbc.1 $RPM_BUILD_ROOT%{_mandir}/man1/xbc.1
install -Dm 644 doc/man-pages/xbbc.1 $RPM_BUILD_ROOT%{_mandir}/man1/xbbc.1
install -Dm 644 doc/man-pages/xbvm.1 $RPM_BUILD_ROOT%{_mandir}/man1/xbvm.1
install -Dm 644 doc/man-pages/bas2x11basic.1 $RPM_BUILD_ROOT%{_mandir}/man1/bas2x11basic.1
install -Dm 755 libx11basic.so $RPM_BUILD_ROOT%{_libdir}/libx11basic.so
install -Dm 755 xbasic $RPM_BUILD_ROOT%{_bindir}/x11basic
install -Dm 755 xbbc $RPM_BUILD_ROOT%{_bindir}/xbbc
install -Dm 755 xbvm $RPM_BUILD_ROOT%{_bindir}/xbvm
install -Dm 755 xbc $RPM_BUILD_ROOT%{_bindir}/xbc
install -Dm 755 bas2x11basic $RPM_BUILD_ROOT%{_bindir}/bas2x11basic

%clean
rm -rf $RPM_BUILD_ROOT

%post
x11basic %{_docdir}/%{name}-%{version}/examples/compiler/xbc.bas --dynamic -o %{_bindir}/xbc
xbc %{_docdir}/%{name}-%{version}/examples/compiler/bas2x11basic.bas --dynamic -o bas2x11basic

%files
%defattr(-,root,root)
%doc README COPYING INSTALL doc/ACKNOWLEGEMENTS AUTHORS RELEASE_NOTES
%doc doc/X11-Basic-manual.txt doc/editors
%doc examples
%{_mandir}/man1/*.1.gz
%{_bindir}/*
%{_libdir}/*

%changelog
* Fri Jun 27 2014 Markus Hoffmann <kollo@users.sourceforge.net> - 1.23
- adapted for Version 1.23
* Fri Dec 27 2013 Markus Hoffmann <kollo@users.sourceforge.net> - 1.22
- adapted for Version 1.22
* Fri Apr 12 2013 Markus Hoffmann <kollo@users.sourceforge.net> - 1.21
- adapted for Version 1.21
* Fri Jan 13 2013 Markus Hoffmann <kollo@users.sourceforge.net> - 1.20
- adapted for Version 1.20
* Fri May 13 2011 Markus Hoffmann <kollo@users.sourceforge.net> - 1.16
- adapted for Version 1.16
* Fri Mar 25 2011 Huaren Zhong <huaren.zhong@gmail.com> - 1.14
- Rebuild for Fedora
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
* Tue Aug 28 2001 Markus Hoffmann <m.hoffmann@uni-bonn.de>
  1st release
