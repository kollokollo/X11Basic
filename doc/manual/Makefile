# Makefile for X11-Basic documentation, including the user manual
# 
# (c) Markus Hoffmann 2003-2019
#
# benoetigt:
# * imagemagic
# * latex
# * xbasic
# * pstotext
#


# This file is part of X11-Basic, the basic interpreter for Unix/X
# ======================================================================
# X11-Basic is free software and comes with NO WARRANTY - read the file
# COPYING for details

VERSION=1.27
NAME=X11-Basic-manual


DEPSRC=*.tex

BASFILES=bas/do_the_error_section.bas bas/maketex.bas

TEXFILES=tex/about.tex tex/usage.tex tex/programming.tex tex/conditions.tex \
         tex/licence.tex tex/syntax.tex tex/abstract.tex tex/cgi-internals.tex \
	 tex/faq.tex tex/main.tex \
         tex/ACKNOWLEGEMENTS.tex tex/compatibility.tex tex/gui-objects.tex tex/quick.tex

PICTURES=alert1.png alert2.png alert3.png fileselect.png form2.png menu.png \
         nedit-x11basic.jpg nano-x11basic.png biene.png form1.png form3.png \
	 nedit-x11basic.gif
EPSFILES=alert1.eps alert2.eps alert3.eps fileselect.eps form2.eps menu.eps \
         nedit-x11basic.eps nano-x11basic.eps biene.eps form1.eps form3.eps 

DIR=X11Basic-manual-$(VERSION)
DIST=Makefile sec-base/00_Logo.sec sec-base/01_Version.sec sec-base/02_Preamble.sec \
     sec-base/10_X11-Basic.sec \
     _.sec A.sec B.sec C.sec D.sec E.sec F.sec G.sec H.sec I.sec \
     J.sec K.sec L.sec M.sec N.sec O.sec P.sec Q.sec R.sec S.sec T.sec \
     U.sec V.sec W.sec X.sec $(TEXFILES) $(BASFILES) $(PICTURES) \
     ../FAQ ../HISTORY ../../RELEASE_NOTES ../../CONTRIBUTING.md


%.eps: %.jpg
	convert $<  $@
%.pdf: %.ps
	ps2pdf -dPDFsettings=/prepress $<
%.ps: %.dvi
	dvips -Ppdf -G0 $< -o $@
%.dvi: tex/%.tex
	latex $<
%.jpg: pictures/%.png
	convert $< $@
%.jpg: pictures/%.gif
	convert $< $@
%.ind: %.idx
	makeindex -o $@ $<


all : X11-Basic-manual.pdf text-manual

draft : draft.ps
brochure : brochure.pdf
Outputs/$(NAME)-$(VERSION).pdf : main.pdf
	cp main.pdf $@

X11-Basic-manual.pdf : Outputs/$(NAME)-$(VERSION).pdf
	ln -f -s $< $@
	chmod 644 $@
X11-Basic-manual.txt : Outputs/$(NAME)-$(VERSION).txt
	ln -f -s $< $@
	chmod 644 $@
text-manual: $(NAME)-$(VERSION).txt
	ln -f -s Outputs/$< X11-Basic-manual.txt
$(NAME)-$(VERSION).txt : sec-base/*.sec sec/*.sec 39_Errors.sec main.otxt ../FAQ ../HISTORY ../../RELEASE_NOTES
	cat sec-base/0*.sec main.otxt > $@
	cat ../FAQ ../HISTORY ../../RELEASE_NOTES >> $@
	echo "          Last change to this file: " `date`  >> $@
	chmod 644 $@
	mv $@ Outputs/
39_Errors.sec: bas/do_the_error_section.bas ../../src/errortxt.c
	rm -f $@; xbasic bas/do_the_error_section.bas > $@
	
main.otxt : main.dvi
	dvi2tty -w132 main.dvi -o $@
# main.otxt : main.ps
#	pstotext -cork -gs "/usr/bin/gs -dNOSAFER" -debug -output $@ $< 
main.dvi : A.tex tex/*.tex $(EPSFILES) 39_Errors.sec
	latex tex/main.tex	
	makeindex main.idx
	latex tex/main.tex
A.tex : sec/?.sec bas/maketex.bas
	xbasic bas/maketex.bas
quick.tex: 39_Errors.sec
brochure.ps : main.ps
	makebrochure $<
	rm -f main1.ps main2.ps
	mv main3.ps brochure.ps
draft.ps : main.ps
	draftit $< > $@

clean : 
	rm -f main.ps main.fot texput.log main.toc main.pdf $(EPSFILES) *.jpg main.dvi *.aux tex/*.aux tex/main.ilg tex/main.idx tex/main.ind main.log missfont.log
distclean : clean
	rm -f *.pdf A.tex 39_Errors.sec main.otxt  $(NAME)-?.??.txt main.fot main.toc
dist :	$(DIST)
	rm -rf /tmp/$(DIR)
	mkdir /tmp/$(DIR)
	(tar cfh - $(DIST))|(cd /tmp/$(DIR); tar xpf -)
	(cd /tmp; tar cf - $(DIR)|gzip -9 > $(DIR).tar.gz)
	mv /tmp/$(DIR).tar.gz .

# DO NOT DELETE
