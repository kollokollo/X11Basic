open "I",#1,"/dev/mouse"
do
  a=inp(#1)
  b=inp(#1)
  c=inp(#1)

print hex$(a,2,2,1),b,c
loop


quit
