
' How to use the on error and on break statements.


every 10,ddd
on error gosub e
on break gosub a
do
print time$;chr$(13);
flush
if random(10000)=100
   error 22
  print 1/0
endif
loop
quit
exiot:

print "break and exit with ";err
quit

procedure a

print "no break !!!!"
on break goto exiot
return
procedure e
  print "Error ";err;" occured."
  print "continue ...."
  on error gosub e
return
procedure ddd
print "timeout"

return
