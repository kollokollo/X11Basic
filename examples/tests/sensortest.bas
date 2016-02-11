' Test of the Android builtin sensors
'
'
print "SENSOR?=";sensor?
pause 1
for i=0 to 10
  print sensor(0),sensor(1),sensor(2)
  pause 0.5
next i

print "ON"
SENSOR ON

for i=0 to 10
  print sensor(0),sensor(1),sensor(2),sensor(3),sensor(4),sensor(5)
  pause 0.5
next i
print "OFF"
SENSOR OFF

for i=0 to 10
  print sensor(0),sensor(1),sensor(2)
  pause 0.5
next i
end


