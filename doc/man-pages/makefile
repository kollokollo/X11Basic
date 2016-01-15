
# Makefile for X11-Basic html-man-pages (c) Markus Hoffmann V.1.13


# This file is part of X11BASIC, the basic interpreter for Unix/X
# ======================================================================
# X11BASIC is free software and comes with NO WARRANTY - read the file
# COPYING for details


VERSION=1.24
NAME=X11-Basic-manpages

DIR=$(NAME)-$(VERSION)

DIST=makefile \
bas2x11basic.1  gui2bas.1  rsc2gui.1     x11basic.1  xbbc.1  xbvm.1 \
doocsxbasic.1   myzip.1    tinexbasic.1  xb2c.1      xbc.1

MANS=$(wildcard *.1)
HTMLS=$(MANS:.1=.1.html)
TXTS=$(MANS:.1=.txt)

all : $(HTMLS) move

clean : 
	rm -f $(HTMLS) $(TXTS) 
%.1.html: %.1
	man2html $< > $@
#	groff -mandoc -Thtml $< > $@
	chmod 644 $@
%.txt: %.1
	groff -mandoc -Tlatin1 $< > $@
move : $(HTMLS)
	mv $(HTMLS) ../../htdocs/
test : xbc.1 x11basic.1
	echo  $< $@
	echo $(HTMLS)
dist :	$(DIST)
	rm -rf /tmp/$(DIR)
	mkdir /tmp/$(DIR)
	(tar cfh - $(DIST))|(cd /tmp/$(DIR); tar xpf -)
	(cd /tmp; tar cf - $(DIR)|gzip -9 > $(DIR).tar.gz)
	mv /tmp/$(DIR).tar.gz .
clean : 
	rm -f $(HTMLS) 
distclean : clean
	rm -f $(DIR).tar.gz
