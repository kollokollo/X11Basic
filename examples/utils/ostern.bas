'	Program OSTERN	(c) Markus Hoffmann
	
	echo off
	if len(parm$)
		jahr=val(parm$)
	else
		input "Jahreszahl (vierstellig): ",jahr
	endif
	xjahr=val(right$(date$,4))

	q=jahr div 4
	a=jahr mod 19
	b=(204-11*a) mod 30
	if b=28 or b=28
		DEC b
	endif
	i=b
	j=jahr+q+i-13
	c=j-(j DIV 7)*7
	o=28+i-c
	print "Ostersonntag ";jahr'
	if jahr<xjahr
		print "war ";
	else if jahr=xjahr
	        print "ist ";
	else
		print "wird sein ";
	endif
	print "am ";
	if o<=31
		print  o;"ten Maerz."
	else
		print o-31;"-ten April."   
	endif
end
