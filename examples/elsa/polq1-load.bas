'
' Laed die Einstellungen fuer die Quelle 1.
' Letzte Bearbeitung: Markus Hoffmann Sept. 1999
'
echo off
dir$="/sgt/elsa/data/polq1/"
file$=dir$+"data.dat"

open "I",#1,file$
while not eof(#1)
  input #1,t$
  if left$(t$)<>"#"
    input #1,wert
    csset t$,wert
  endif
wend
close #1


quit

