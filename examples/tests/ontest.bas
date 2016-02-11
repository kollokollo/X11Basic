
a=0
input a

on a+1 gosub hallo,ja(1.3),hier,auch
on a+1 goto 1,2,3,4

quit
1:
print "1"
2:
print "2"
3:
print "3"
4:
print "4"
quit


procedure hallo
  print "Hallo"
return
procedure ja(n)
  print "Ja "+str$(n)
return
procedure hier
  print "hier"
return
procedure auch
  print "auch"
return
