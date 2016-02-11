all : xbc.txt

clean : 
	rm -f xbc.html xbc.txt x11basic.html x11basic.txt
xbc.html : xbc.1
	groff -mandoc -Thtml xbc.1 > xbc.html
xbc.txt : xbc.1
	groff -mandoc -Tlatin1 xbc.1 > xbc.txt
x11basic.html : x11basic.1
	groff -mandoc -Thtml x11basic.1 > x11basic.html
x11basic.txt : x11basic.1
	groff -mandoc -Tlatin1 x11basic.1 > x11basic.txt
	
bas2x11basic.html : bas2x11basic.1
	groff -mandoc -Thtml bas2x11basic.1 > bas2x11basic.html
bas2x11basic.txt : bas2x11basic.1
	groff -mandoc -Tlatin1 bas2x11basic.1 > bas2x11basic.txt
