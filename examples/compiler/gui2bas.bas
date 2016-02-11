' Utility to convert gui Files with Formulars or other Graphical user interface
' Objects to X11Basic executable programs
' (c) Markus Hoffmann 2003 (letzte Bearbeitung: 10.07.2003)
'
'
dim iz$(1000)
anziz=0

anzstring=0
anztree=0
anzobj=0
anztedinfo=0

inputfile$=param$(2)
chw=8
chh=16
OPEN "I",#1,inputfile$
while not eof(#1)
  lineinput #1,t$
  if len(trim$(t$))
    iz$(anziz)=trim$(t$)
    inc anziz
  endif
wend
close #1

print "' gui2bas V.1.00   (c) Markus Hoffmann 2003"
print "' convertetd "+inputfile$+"  "+date$+" "+time$
print "bx=0"
print "by=20"
print "bw=640"
print "bh=400"
print "@formular     ! execute form"
print "quit"
print "procedure formular"
print "  local ret"
klammer=0
spaces=1
count=0
~@doit2(-1)
@addtree(0,anzobj-1)
print "return"
quit

function doit2(parent)
  local t$,klammer,idx,typ$,label$,obnext,obtail,obhead,parameter$
  ' print "# DOIT2: ",parent,count
  idx=-1
  while count<anziz
    t$=iz$(count)
    inc count
    exit if t$="}"
    if left$(t$)="'" or left$(t$)="#"
      print t$
    else
      idx=anzobj
      inc anzobj
      if right$(t$)="{"
        klammer=1 
        t$=trim$(left$(t$,len(t$)-1))
      else
        klammer=0
      endif
      wort_sep t$,":",1,label$,t$
      if len(t$)=0
        t$=label$
        label$=""
      endif
      wort_sep t$,"(",1,typ$,t$
      typ$=trim$(typ$)
      if right$(t$)=")"
        t$=left$(t$,len(t$)-1)
      endif
      if klammer=1
        obhead=anzobj
        obtail=@doit2(idx)
	if obtail=-1
	  obhead=-1
	endif
      else
        obtail=-1
	obhead=-1
      endif
      if iz$(count)="}" or count=anziz
        obnext=parent
      else
        obnext=anzobj
      endif
      if len(label$)
        print label$+"="+str$(idx)
      endif
      @doobj(idx,obnext,obhead,obtail,typ$,t$)
    endif
  wend
  ' print "# END DOIT2: ",parent,count

  return idx
endfunc

function doit(par1,par2,countee)
  local klammer,ppp,typ$,obnext,obhead,obtail,idx,label$,t2$,parameter$
  local label$,typ$,i,t$,ss
  t$=iz$(countee)
  print "# DOIT ",par1,par2,t$
  if right$(t$)="{"
    klammer=1 
    t$=trim$(left$(t$,len(t$)-1))
  else
    klammer=0
  endif
  wort_sep t$,":",1,label$,t$
  if len(t$)
  else
    t$=label$
    label$=""
  endif
  wort_sep t$,"(",1,typ$,t$
  typ$=trim$(typ$)
  if right$(t$)=")"
      t$=left$(t$,len(t$)-1)
  endif
  idx=anzobj
  inc anzobj

  obhead=anzobj
  parameter$=t$
  if klammer=1
    ss=@suchende(countee+1,idx)   
    obtail=anzobj-1
  else
    obhead=-1
    obtail=-1  
  endif
 
  if par1=1
    obnext=par2
  else
    obnext=anzobj
  endif
  if len(label$)
    print label$+"="+str$(idx)
  endif
  on=obnext
  oh=obhead
  ot=obtail
  i=idx
  @doobj(i,on,oh,ot,typ$,parameter$)
  if klammer
    return ss+1
  endif
  return countee+1
return
function suchende(start,idx)
local i,t$,k
k=0
i=start
while i<anziz
  t$=iz$(i)
  if right$(t$)="{"
    i=@doit(0,idx,i)
    inc k
  else if left$(t$)="}"
    if k=0
      return i
    endif
    dec k
  else
    i=@doit(0,idx,i)
  endif
wend
return -1
endfunc

procedure doobj(idx,obnext,obhead,obtail,a$,b$)
  if upper$(a$)="TREE"
    if anztree
      @addtree(treestart,anzobj-1)
    endif
    treelabel$=label$
    treestart=anzobj-1
    if len(treelabel$)
      print "TREE_";treelabel$;"=",anztree
    endif
  else
      obx=val(@getval$(b$,"X"))*chw
      oby=val(@getval$(b$,"Y"))*chh
      obw=val(@getval$(b$,"W"))*chw
      obh=val(@getval$(b$,"H"))*chh
      flags$=@getval$(b$,"FLAGS")
      state$=@getval$(b$,"STATE")
      obflags=@doflags(flags$)
      obstate=@dostate(state$)
    if upper$(a$)="BOX"
      obtype=20
      char$=@getval$(b$,"CHAR")
      if len(char$)=1
        char=asc(char$)
      else if len(char$)=3
        char=peek(varptr(char$)+1)
      else
        char=0
      endif
      frame=val(@getval$(b$,"FRAME"))
      framecol=val(@getval$(b$,"FRAMECOL"))
      textcol=val(@getval$(b$,"TEXTCOL"))
      bgcol=val(@getval$(b$,"BGCOL"))
      pattern=val(@getval$(b$,"PATTERN"))
      textmode=val(@getval$(b$,"TEXTMODE"))
      obspec=cvl(chr$(16*(bgcol and 15)+2*(pattern and 7)+(textmode and 1))+chr$((framecol and 15)+16*(textcol and 15))+chr$(frame)+chr$(char))
      print "obj"+str$(idx)+"$=mki$(";obnext;")+mki$(";obhead;")+mki$(";obtail;")";
      print "+mki$(";obtype;")+mki$(";obflags;")+mki$(";obstate;")+mkl$(";obspec;")";
     else if upper$(a$)="TEXT" or upper$(a$)="FTEXT" or upper$(a$)="BOXTEXT"
      obtype=21*abs(upper$(a$)="TEXT")+29*abs(upper$(a$)="FTEXT")+22*abs(upper$(a$)="BOXTEXT")
      text$=@getval$(b$,"TEXT")
      if left$(text$)=chr$(34)
        text$=right$(text$,len(text$)-1)  
      endif
      if right$(text$)=chr$(34)
        text$=left$(text$,len(text$)-1)  
      endif
      ptmp$=@getval$(b$,"PTMP")
      if left$(ptmp$)=chr$(34)
        ptmp$=right$(ptmp$,len(ptmp$)-1)  
      endif
      if right$(ptmp$)=chr$(34)
        ptmp$=left$(ptmp$,len(ptmp$)-1)  
      endif
      pvalid$=@getval$(b$,"PVALID")
      if left$(pvalid$)=chr$(34)
        pvalid$=right$(pvalid$,len(pvalid$)-1)  
      endif
      if right$(pvalid$)=chr$(34)
        pvalid$=left$(pvalid$,len(pvalid$)-1)  
      endif
      font=val(@getval$(b$,"FONT"))
      if @getval$(b$,"FONT")=""
        font=3
      endif
      just=val(@getval$(b$,"JUST"))
      color=val(@getval$(b$,"COLOR"))
      border=val(@getval$(b$,"BORDER"))
      @addtedinfo(text$,ptmp$,pvalid$,font,just,color,border)
      print "obj"+str$(idx)+"$=mki$(";obnext;")+mki$(";obhead;")+mki$(";obtail;")";
      print "+mki$(";obtype;")+mki$(";obflags;")+mki$(";obstate;")+mkl$(varptr(tedinfo";anztedinfo-1;"$))";
    else if upper$(a$)="STRING" or upper$(a$)="TITLE" or upper$(a$)="BUTTON"
      obtype=28*abs(upper$(a$)="STRING")+32*abs(upper$(a$)="TITLE")+26*abs(upper$(a$)="BUTTON")
      text$=@getval$(b$,"TEXT")
      if left$(text$)=chr$(34)
        text$=right$(text$,len(text$)-1)  
      endif
      if right$(text$)=chr$(34)
        text$=left$(text$,len(text$)-1)  
      endif
      @addstring(text$)
      print "obj"+str$(idx)+"$=mki$(";obnext;")+mki$(";obhead;")+mki$(";obtail;")";
      print "+mki$(";obtype;")+mki$(";obflags;")+mki$(";obstate;")+mkl$(varptr(string";anzstring-1;"$))";

    else if upper$(a$)="UNKNOWN"
      print "' UNKNOWN: ";b$
    else
      print a$,parent
    endif
    print "+mki$(";obx;")+mki$(";oby;")+mki$(";obw;")+mki$(";obh;")"
  endif
return
procedure addtree(aob,oobj)
  local t$,i
  t$="tree"+str$(anztree)+"$="
  for i=aobj to oobj
    t$=t$+"obj"+str$(i)+"$"
    if i<>oobj
      if len(t$)>70
        print space$(spaces*2)+t$
        t$="tree"+str$(anztree)+"$=tree"+str$(anztree)+"$+"
      else 
	t$=t$+"+"
      endif
    endif
  next i
  print space$(spaces*2)+t$
  print "  ~form_dial(0,0,0,0,0,bx,by,bw,bh)"
  print "  ~form_dial(1,0,0,0,0,bx,by,bw,bh)"
  print "'  ~objc_draw(varptr(tree"+str$(anztree)+"$),0,-1,0,0)"
  print "  ret=form_do(varptr(tree"+str$(anztree)+"$))"
  print "  ~form_dial(2,0,0,0,0,bx,by,bw,bh)"
  print "  ~form_dial(3,0,0,0,0,bx,by,bw,bh)"
  print "  vsync"
  inc anztree
return
procedure addstring(r$)
  print "string"+str$(anzstring)+"$="+chr$(34)+r$+chr$(34)+"+chr$(0)"
  inc anzstring
return
procedure addtedinfo(a$,b$,c$,d,e,f,g)
  print "string"+str$(anzstring)+"$="+chr$(34)+a$+chr$(34)+"+chr$(0)+space$("+str$(len(c$))+")"
  inc anzstring
 
   @addstring(b$)
   @addstring(c$)

  print "tedinfo"+str$(anztedinfo)+"$=mkl$(varptr(string"+str$(anzstring-3)+"$))";
  print "+mkl$(varptr(string"+str$(anzstring-2)+"$))";
  print "+mkl$(varptr(string"+str$(anzstring-1)+"$))";
  print "+mki$(";d;")+mki$(0)+mki$(";e;")+mki$(";f;")+mki$(0)+mki$(";g;")+mki$(";len(b$);")+mki$(";len(c$);")"
  inc anztedinfo
return
function getval$(t$,f$)
  local a$,val$
  val$=""
  wort_sep t$,",",1,a$,t$
  while len(a$)
    a$=trim$(a$)
    wort_sep a$,"=",1,name$,val$
    exit if upper$(name$)=upper$(f$)
    val$=""
    wort_sep t$,",",1,a$,t$
  wend
  return val$
endfunction
function doflags(t$)
  local ret,a$
  ret=0
  if left$(t$)="("
    t$=right$(t$,len(t$)-1)  
  endif
  if right$(t$)=")"
    t$=left$(t$,len(t$)-1)  
  endif

  wort_sep t$,"+",1,a$,t$
  while len(a$)
  if a$="SELECTABLE"
    ret=ret or 1
  else if a$="DEFAULT"
    ret=ret or 2
  else if a$="EXIT"
    ret=ret or 4
  else if a$="EDITABLE"
    ret=ret or 8
  else if a$="RADIOBUTTON"
    ret=ret or 16
  else if a$="LASTOB"
    ret=ret or 32
  else if a$="TOUCHEXIT"
    ret=ret or 64
  else if a$="HIDETREE"
    ret=ret or 128
  else if a$="INDIRECT"
    ret=ret or 256
  else
    print "Unknown flag:",a$
  endif
  wort_sep t$,"+",1,a$,t$
  wend
  return ret
endfunc
function dostate(t$)
  local ret,a$
  ret=0
  if left$(t$)="("
    t$=right$(t$,len(t$)-1)  
  endif
  if right$(t$)=")"
    t$=left$(t$,len(t$)-1)  
  endif
  wort_sep t$,"+",1,a$,t$
  while len(a$)
  if a$="SELECTED"
    ret=ret or 1
  else if a$="CROSSED"
    ret=ret or 2
  else if a$="CHECKED"
    ret=ret or 4
  else if a$="DISABLED"
    ret=ret or 8
  else if a$="OUTLINED"
    ret=ret or 16
  else if a$="SHADOWED"
    ret=ret or 32
  else if a$="WHITEBAK"
    ret=ret or 64
  else if a$="DRAW3D"
    ret=ret or 128
  else
    print "Unknown state:",a$
  endif

    wort_sep t$,"+",1,a$,t$
  wend
  return ret
endfunc
