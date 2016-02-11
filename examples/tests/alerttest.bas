fileselect "Alert-Datei auswaehlen:","./alerts.*","",f$
if len(f$)
  if exist(f$)
    open "I",#1,f$
    while not eof(#1)
      lineinput #1,t$
      print form_alert(1,t$)
    wend
    close #1
  endif
endif


alert 2,"Welchen Test ?",1,"QUIT|Nr. 1|Nr. 2",balert
ALERT 0,"   Max. Jod-131 Ingestion:   |Kat.A Personen 700000 Bq/Jahr |Bevîlkerung     12600 Bq/Jahr | ",1,"  Ok  ",balrt
ALERT 0,"   Max. Cs-137 Ingestion:   |Kat.A Personen 4000000 Bq/Jahr |Bevîlkerung      24000 Bq/Jahr | ",1,"  Ok  ",balrt
ALERT 0,"    Max. Sr-90 Ingestion:    |Kat.A Personen 300000 Bq/Jahr |Bevîlkerung      9000 Bq/Jahr | ",1,"  Ok  ",balrt
ALERT 0,"    Max. Jod-131 Inhalation:    |Kat.A Personen 1000000 Bq/Jahr |Bevîlkerung      18000 Bq/Jahr | ",1,"  Ok  ",balrt
ALERT 0,"    Max. Cs-137 Inhalation:    |Kat.A Personen 6000000 Bq/Jahr |Bevîlkerung      36000 Bq/Jahr | ",1,"  Ok  ",balrt
ALERT 0,"     Max. Sr-90 Inhalation:    |Kat.A Personen 50000 Bq/Jahr |Bevîlkerung     1800 Bq/Jahr | ",1,"  Ok  ",balrt
ALERT 0,"  Max. eff. Ganzkîrperdosis:  |Kat.A Personen 5000 mrem/Jahr |Bevîlkerung     300 mrem/Jahr | ",1,"  Ok  ",balrt
ALERT 0,"   Kosmische Strahlung   |       am Erdboden |    30 Millirem/Jahr | ",1,"  Ok  ",balrt
ALERT 0,"   Bodenstrahlung   | |50-100 Millirem/Jahr | ",1,"  Ok  ",balrt
ALERT 0,"    GebÑudestrahlung    | |    15 Millirem/Jahr | ",1,"  Ok  ",balrt
ALERT 0,"         Nahrung         |Kalium-40 in Lebensmitteln | liefert 20 Millirem/Jahr | ",1,"  Ok  ",balrt
ALERT 0,"Med. Diagnostik   50 mrem/Jahr |Kernkraftwerke     1 mrem/Jahr |Atombombenversuche 1 mrem/Jahr |VerbrauchsgÅter    1 mrem/Jahr",1,"  Ok  ",balrt

quit
