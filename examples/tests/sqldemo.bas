' sqldemo.bas fuer Android, linux und WINDOWS
'
' Beispielprogramm fuer SQLite Datenbanken
' Markus Hoffmann +  Joachim Wiedemann-Heinzelmann Jan 2013
'
' Anleitung f"ur Android:
' das binary sqlite3 ist in der X11-Basic app enthalten und 
' ist im Ordner bas/ abgelegt für drei Prozessor-Architekturen:
'
'
' Die Dateien test.db und error.txt finden sich anschliessend in /sdcard/
' Wenn die Datenbank test.db in einem anderen Verzeichnis liegen soll, muss
' prefix$ angepasst werden.
'

database$="test.db"

PRINT "initializing..."
FLUSH
prefix$=""
IF ANDROID?
  sqls$="/xbin/sqlite3"
  IF NOT EXIST(sqls$)
    ' Try to install it
    sqls$="/data/data/net.sourceforge.x11basic/sqlite3"
    IF NOT EXIST(sqls$)
      PRINT "sqlite3 is not yet installed."
      @android_install
    ENDIF
  ENDIF
  tmp$="/sdcard/sqldemo.err"
  prefix$="HOME=/sdcard/ "+CHR$(10)
ELSE IF WIN32?
  sqls$="sqlite3"
  tmp$="sqldemo.err"
ELSE
  sqls$="sqlite3"
  tmp$="/tmp/sqldemo.err"
ENDIF
t$=SYSTEM$(prefix$+sqls$+" -version"+" 2>&1")
IF LEN(t$)=0
  PRINT "something is wrong. sqlite3 is not responding."
  END
ENDIF
PRINT "SQL Version: "+t$

' Now use SQL functions

' Create a sample table
@sql_exec(database$,"create table t1 (t1key INTEGER PRIMARY KEY,data TEXT,num double,timeEnter DATE);")
@sql_exec(database$,"insert into t1 (data,num) values ('This is sample data',3);")
@sql_exec(database$,"insert into t1 (data,num) values ('More sample data',6);")
@sql_exec(database$,"insert into t1 (data,num) values ('And a little more',9);")

' Do a query on the data in the sample table

d$=@sql_query$(database$,"select * from t1 limit 2;")

SPLIT d$,CHR$(10),0,a$,d$
WHILE LEN(a$)
  PRINT "<-- Datensatz: "+a$
  FOR i=0 TO 2-1
    SPLIT a$,"|",0,b$,a$
  NEXT i
  PRINT "     Spalte 2: "+b$
  SPLIT d$,CHR$(10),0,a$,d$
WEND

END
QUIT


PROCEDURE sql_exec(filename$,command$)
  LOCAL t$
  PRINT "-->"+command$
  t$=SYSTEM$(prefix$+sqls$+" -batch "+filename$+" "+ENCLOSE$(command$)+" 2> "+tmp$)
  ' Hier auf Fehlermeldungen testen
  ' Das geht etwas umstaendlich, da stderr nicht zurueckkommt
  ' Man koennte stderr aber in ein file umleiten und dann auswerten.
  '
  IF LEN(t$)
    PRINT "Command produced a <"+t$+">"
  ENDIF
  IF EXIST(tmp$)
    OPEN "I",#1,tmp$
    t$=INPUT$(#1,lof(#1))
    IF LEN(t$)
       PRINT "Fehlermeldung: "+t$
    ENDIF
    CLOSE #1
    IF ANDROID?
      SYSTEM "/system/bin/rm -f "+tmp$
    ELSE IF WIN32?
      SYSTEM "del "+tmp$
    ELSE 
      SYSTEM "rm -f "+tmp$
    ENDIF
  ENDIF
RETURN
FUNCTION sql_query$(filename$,command$)
  LOCAL ret$,t$
  PRINT "-->"+command$
  ret$=SYSTEM$(prefix$+sqls$+" -batch "+filename$+" "+ENCLOSE$(command$)+" 2> "+tmp$)
  IF EXIST(tmp$)
    OPEN "I",#1,tmp$
    t$=INPUT$(#1,LOF(#1))
    IF LEN(t$)
       PRINT "Fehlermeldung: "+t$
    ENDIF
    CLOSE #1
    IF ANDROID?
      SYSTEM "/system/bin/rm -f "+tmp$
    ELSE IF WIN32?
      SYSTEM "del "+tmp$
    ELSE 
      SYSTEM "rm -f "+tmp$
    ENDIF
  ENDIF
  RETURN ret$
ENDFUNCTION
PROCEDURE android_install
  LOCAL t$
  PRINT "installing Androids sqlite3 in "+sqls$
  IF EXIST("/sdcard/bas/sqlite3.armeabi")
    PRINT "copying..."
    FLUSH
    OPEN "I",#1,"/sdcard/bas/sqlite3.armeabi"
    t$=INPUT$(#1,LOF(#1))
    CLOSE #1
    BSAVE sqls$,VARPTR(t$),LEN(t$)
' PRINT system$("cp /mnt/sdcard/bas/sqlite3 "+sqls$+" 2>&1")
    PRINT "change mode..."
    FLUSH
    PRINT SYSTEM$("/system/bin/chmod 755 "+sqls$+" 2>&1")
    FLUSH
  ELSE
    PRINT "cannot find sqlite3. please download it "
    PRINT "and put it in /sdcard/bas/"
    END
  ENDIF
RETURN
  
