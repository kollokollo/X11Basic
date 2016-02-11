' A Comparison of Various Sorting Algorithms
' by Edward D. Collins (September 1, 1998)
' ------------------------------------------
randomize
' This demo compares the times to sort a numeric array using a
' variety (currently 12) of sorting algorithms.

' I did not write these algorithms, other than to enhance them a bit,
' change some of the numeric types to long integers so that the program
' could handle runs with larger arrays, rename some of the variable
' names to something more descriptive, etc., etc.

' The number of items to sort is held in the variable "nmbr.of.items".
' QBASIC users may wish to start with maybe 3,000 or so, which is the
' default.  PowerBASIC users (because of the speed of PowerBASIC) will
' wish to increase this number to 10,000 or so or even more.

' If you're just comparing the results of the faster sorts, you can up
' the number to 32,000 or so.

' PowerBASIC users... note the two changes listed below if you wish to
' run this program.

' The program displays the results to the screen and also appends the
' results to a file called "c:\srt-rslt.txt."  If you do not wish to
' save the results to this file, remark out the call to the
' "Save.Rslts.To.File" subroutine.

' The program also will write the first 50 and the last 50 elements of
' the array before and after the sort, to the file "C:\proof.txt" if
' you so wish.  (To prove the the algorithms work.)  This is currently
' commented out.

' To add your own algorithm...
' ----------------------------
'  1) Increase the variable "nmbr.of.algorithms" by one
'  2) name your algorithm in the algorithm$() array
'  3) Create your own subroutine for it.  The array named "the.array()"
'         holds the data to sort
'  4) call this new subroutine from the subroutine named "Sort.The.Array"


nmbr.of.items=400        ! change this for larger or smaller arrays
nmbr.of.algorithms=12      ! change this if you add another algorithm




' (2) If you're using PowerBASIC, remove the word
      SHARED in the following three statements.
'     That's it.
' ------------------------------------------------------
DIM the.array(nmbr.of.items)
DIM results!(nmbr.of.algorithms)
DIM algorithm$(nmbr.of.algorithms)
size=nmbr.of.items
DIM temp.array(size)

 
algorithm$(1)="Bubble Sort #1              "
algorithm$(2)="Bubble Sort #2              "
algorithm$(3)="Bubble Sort #3              "
algorithm$(4)="Bubble Sort #4              "
algorithm$(5)="Exchange (Substitution) Sort"
algorithm$(6)="Heap Sort                   "
algorithm$(7)="Insertion Sort              "
algorithm$(8)="Merge Sort                  "
algorithm$(9)="Quicksort #1 (recursive)    "
algorithm$(10)="Quicksort #2 (recursive)    "
algorithm$(11)="Quicksort #3 (non-recur)    "
algorithm$(12)="Shell Sort                  "

@Dsply.Main.Scrn
FOR array.type=1 TO 5
   FOR algorithm=1 TO nmbr.of.algorithms
      print at(algorithm+4+fltp,31+((array.type-1)*10));
      @COLOR(23,1)
      PRINT "----.----";
      flush
      @Store.Array.Items(array.type)
      
 '    CALL Save.Array.To.File(0)   ' to show the results before the sort
      @Sort.The.Array(algorithm)
 '    CALL Save.Array.To.File(1)   ' to prove that the algorithm worked

      IF clr.switch=5 
        clr.switch=3 
      ELSE 
        clr.switch=5
      endif
      @COLOR(clr.switch,1)
      print at(algorithm+4+fltp,31+((array.type-1)*10));
      PRINT results!(array.type,algorithm) USING "####.####";
      flush
   NEXT algorithm
   clr.switch=7
NEXT array.type

@Save.Rslts.To.File
@COLOR(15,2)
print at(2,69);
PRINT " Done !! ";
@COLOR(15,1)
BEEP
END



procedure Bubble.Sort1
' ============================== Bubble Sort ===========================
' The Bubble Sort algorithm cycles through the array, comparing adjacent
' elements and swapping pairs that are out of order.
' ======================================================================
local a,b
FOR a=1 TO nmbr.of.items-1
  FOR b=1 TO nmbr.of.items-1
     IF the.array(b)>the.array(b+1)
        @swap(b,b+1)
     ENDIF
  NEXT b
NEXT a
return



procedure Bubble.Sort2
' =========================== Bubble Sort #2 ===========================
' This sort differs from the first Bubble Sort because of the addition
' of the "array.is.sorted" flag.  As soon as it makes a pass where no
' swapping occurred it the array is sorted and the loop terminates.
' ======================================================================

repeat
  array.is.sorted=1
  FOR count=1 TO nmbr.of.items-1
     IF the.array(count)>the.array(count+1)
        @SWAP(count,count+1)
        array.is.sorted=0
     ENDIF
  NEXT count
UNTIL array.is.sorted=1
return



procedure Bubble.Sort3
' ============================ Bubble Sort #3 =========================
' After the inside FOR loop finishes for the first time, the largest
' element is positioned in its proper place.  Therefore, we don't ever
' have to compare that number anymore.  So for that next cycle, we have
' one less element to compare each time.  This is an enhancement over
' the previous two Bubble Sorts.  This one does have the array.is.sorted
' flag as well, like Bubble Sort #2.
' =====================================================================

FOR a=nmbr.of.items TO 1 STEP -1
  array.is.sorted=1
  FOR b=2 TO a
     IF the.array(b-1)>the.array(b)
        @SWAP(b-1,b)
        array.is.sorted=0
     ENDIF
  NEXT
  exit IF array.is.sorted=1
NEXT a
return



procedure Bubble.Sort4
' ============================ Bubble Sort #4 =========================
'  This Bubble Sort sorts on the next pass only to where the last
'  switch was made.  This takes the enhancement to Bubble Sort #3 one
'  step further.
' ======================================================================

limit=nmbr.of.items
repeat
  switch=0
  FOR count=1 TO limit-1
     IF the.array(count)>the.array(count+1)
        @SWAP(count,count+1)
        switch=count
     ENDIF
  NEXT
  limit=switch
until switch=0
return



procedure Dsply.Main.Scrn
  fltp=6
  @COLOR(15,1)
  CLS
  @COLOR(15,2)
  print at(2,20);" Comparison of Various Sorting Algorithms ";
  @COLOR(0,1)
  PRINT STRING$(2,chr$(2))
  print at(3,22);STRING$(42,chr$(2))
  @COLOR(2,2)
  FOR count=4 TO 7
    print at(count,2);
    PRINT STRING$(26,chr$(2))
  NEXT
  @COLOR(15,2)
  print at(4,2);" The times listed are the "
  print at(5,2);" number of seconds that	"
  print at(6,2);" it takes to sort a "
  @COLOR(14,2)
  print at(6,22);nmbr.of.items;
  @COLOR(15,2)
  print at(7,2);" element numeric array.   "

  @COLOR(0,1)
  FOR count=5 TO 7
    print at(count,28);STRING$(1,chr$(2))
  NEXT
  print at(8,3);STRING$(26,chr$(2))

  @COLOR(15,2)
  print at(fltp,40);" -- Type of array to be sorted -- ";
@COLOR(0,1)
PRINT STRING$(1,chr$(2))
print at(fltp+1,41);STRING$(34,chr$(2))

@COLOR(7,1)
print at(fltp+2,33);"Already  Complete   Random    Nearly    Merge"
print at(fltp+3,33);"Sorted   Opposite   Mix-up    Sorted     Two "
print at(fltp+4,33);STRING$(45,"=")

FOR count=1 TO nmbr.of.algorithms
   IF clr.switch=3
     clr.switch=5
   ELSE 
     clr.switch=3
   endif
   @COLOR(clr.switch,1)
   print at(count+4+fltp,2);algorithm$(count)
NEXT count

  @COLOR(8,1)
  FOR array.type=1 TO 5
    FOR algorithm=1 TO nmbr.of.algorithms
      print at(algorithm+4+fltp,31+(array.type-1)*10);
      IF results!(array.type,algorithm)=0
        PRINT "----.----";
      ELSE
        PRINT results!(array.type,algorithm)
      ENDIF
    NEXT
  NEXT
 
  @COLOR(6,2)
  print at(2,69);" Working ";
  @COLOR(0,1)
  PRINT STRING$(1,chr$(2))
  print at(3,70);STRING$(9,chr$(2))
return



procedure Exchange.Sort
' ============================= Exchange Sort ========================
' The Exchange Sort compares each element in the array, starting with
' the first element, with every following element.  If any of the
' following elements are smaller than the current element, it is
' exchanged with the current element and the process is repeated
' for the next element in the array.
' ====================================================================

  FOR a=1 TO nmbr.of.items
    less=a
    FOR b=less+1 TO nmbr.of.items
      IF the.array(b)<the.array(less) 
        less=b
      endif
    NEXT
    IF less>a
      @SWAP(a,less)
    ENDIF
  NEXT
return

procedure SWAP(a,b)
local c
c=the.array(a)
the.array(a)=the.array(b)
the.array(b)=c
return

procedure Heap.Sort
' =============================== Heap Sort ===============================
'  The Heap Sort procedure works by calling two other procedures -
'  Percolate.Up and Percolate.Down.  Percolate Up turns the array into
'  a "heap," which has the properties outlined in the diagram below:
'
'                               the.array(1)
'                               /          \
'                    the.array(2)           the.array(3)
'                   /          \             /         \
'         the.array(4)   the.array(5)    the.array(6)  the.array(7)
'          /      \       /       \        /      \      /      \
'        ...      ...   ...       ...    ...      ...  ...      ...
'
'
'  where each "parent node" is greater than each of its "child nodes"; for
'  example, array(1) is greater than array(2) or array(3), array(3) is
'  greater than array(6) or array(7), and so forth.
'
'  Therefore, once the first FOR...NEXT loop in the Heap Sort is finished,
'  the largest element is in array(1).
'
'  The second FOR...NEXT loop in Heap Sort swaps the element in array(1)
'  with the element in nmbr.of.items, rebuilds the heap (with Percolate Down)
'  for nmbr.of.items - 1, then swaps the element in array(1) with the element
'  in nmbr.of.items - 1, rebuilds the heap for nmbr.of.items - 2, and
'  continues in this way until the array is sorted.


'  Note: I discovered that it is slightly faster to have this SUB execute
'  call the Percolate up and Percolate Down procedures via GOSUB statement
'  rather than having this SUB call another SUB (It was about 3/10's of a
'  second faster for a full 32,000 element array.
' =========================================================================

   FOR count=2 TO nmbr.of.items
      maxlevel=count
      GOSUB Percolate.Up
   NEXT count

   FOR count=nmbr.of.items TO 2 STEP -1
      @SWAP(1,count)
      maxlevel=count-1
      GOSUB Percolate.Down
  NEXT count
return



procedure Percolate.Up
   i=maxlevel

' --------------------------------------------------------------------
' Move the value in the array(maxlevel) up the heap until it has
' reached its proper node (that is, until it is greater than either
' of its child nodes, or until it has reached 1, the top of the heap)
' --------------------------------------------------------------------
   while i<>1
      parent=i\2            ' Get the subscript for the parent node.

' --------------------------------------------------------------------
' The value at the current node is still bigger than the value at
' its parent node, so swap these two array elements
' --------------------------------------------------------------------
      IF the.array(i)>the.array(parent)
         @SWAP(parent,i)
         i=parent

' --------------------------------------------------------------------
' Otherwise, the element has reached its proper place in the heap,
' so exit this procedure
' --------------------------------------------------------------------
      ELSE
         EXIT if true
      ENDIF
   wend
RETURN




procedure Percolate.Down
   i&=1

' ----------------------------------------------------------------
' Move the value in the array(1) down the heap until it has reached
' its proper node (that is, until it is less than its parent node
' or until it has reached maxlevel, the bottom of the current heap)
' ----------------------------------------------------------------
   DO
      child&=2*i&             ! Get the subscript for the child node.

' Reached the bottom of the heap, so exit this procedure
      exit IF child&>maxlevel

' ----------------------------------------------------------
' If there are two child nodes, find out which one is bigger
' ----------------------------------------------------------
      IF child&+1<=maxlevel
         IF the.array(child&+1)>the.array(child&)
            inc child&
         ENDIF
      ENDIF

' ---------------------------------------------
' Move the value down if it is still not bigger
' than either one of its children
' ---------------------------------------------
    IF the.array(i&)<the.array(child&)
         @SWAP(i&,child&)
         i&=child&

' -------------------------------------------
' Otherwise, the array has been restored to a
' heap from 1 to maxlevel, so exit:
' -------------------------------------------
      ELSE
         EXIT if true
      ENDIF
   LOOP
RETURN



procedure Insertion.Sort
' ============================= Insertion Sort =========================
' The Insertion Sort procedure compares the length of each successive
' element in the array with the lengths of all the preceding elements.
' When the procedure finds the appropriate place for the new element, it
' inserts the element in its new place, and moves all the other elements
' down one place.
' ======================================================================

FOR a=2 TO nmbr.of.items
   temp1=the.array(a)
   temp2=temp1
   FOR b=a TO 2 STEP -1
      IF the.array(b-1)>temp2
         the.array(b)=the.array(b - 1)
      ELSE
         EXIT if true
      ENDIF
   NEXT b
   the.array(b)=temp1
NEXT a
return



procedure Merge.Sort
' ============================ Merge Sort =============================
' Compared to the other n*log2(n) algorithms a Merge Sort is very easy
' to analyse and the constant hidden by the asymptotic notation is
' even smaller than that of Quicksort.  Unfortunately, it requires an
' extra workspace, and this alone makes it unsuitable for large scale
' use.
' =====================================================================
size=nmbr.of.items

FOR i=1 TO size-(size AND 1) STEP 2
   IF the.array(i)>the.array(i+1)
      @SWAP(i,i+1)
   ENDIF
NEXT i

listsize&=2
WHILE listsize&<size
   left=1
   yptr=1
   right&=listsize&+1
   WHILE right&<=size
      leftend=left+listsize&
      IF right&+listsize&>size
         rightend=size+1
      ELSE
         rightend=right&+listsize&
      ENDIF

      WHILE right&<rightend AND left<leftend
         IF the.array(left)<the.array(right&)
            temp.array(yptr)=the.array(left)
            inc left
         ELSE
            temp.array(yptr)=the.array(right&)
            right&=right&+1
         ENDIF
         inc yptr
      WEND
  
      IF right&=rightend
         FOR left=left TO leftend-1
            temp.array(yptr)=the.array(left)
            inc yptr
         NEXT left
      ELSE
          FOR right&=right& TO rightend-1
             temp.array(yptr)=the.array(right&)
             inc yptr
          NEXT right&
      ENDIF

      left=right&
      right&=right&+listsize&
   WEND
  
   FOR i=yptr TO size
      temp.array(i)=the.array(i)
   NEXT i
   listsize&=listsize&*2
                                       
   IF listsize&<size
      left=1
      yptr=1
      right&=listsize&+1
                                       
      WHILE right&<=size
                                       
         leftend=left+listsize&
         IF right&+listsize&>size
            rightend=size+1
         ELSE
            rightend=right&+listsize&
         ENDIF
                                          
         WHILE right&<rightend AND left<leftend
            IF temp.array(left)<temp.array(right&)
               the.array(yptr)=temp.array(left)
              left=left+1
            ELSE
               the.array(yptr)=temp.array(right&)
               right&=right&+1
            ENDIF
            inc yptr
         WEND
     
         IF right&=rightend
               FOR left=left TO leftend-1
                  the.array(yptr)=temp.array(left)
                  yptr=yptr+1
               NEXT left
         ELSE
               FOR right&=right& TO rightend-1
                  the.array(yptr)=temp.array(right&)
                  inc yptr
               NEXT right&
         ENDIF

         left=right&
         right&=right&+listsize&
      WEND
      listsize&=listsize&*2
     
      FOR i=yptr TO size
         the.array(i)=temp.array(i)
      NEXT i
   ELSE
      FOR i=1 TO size
         the.array(i)=temp.array(i)
      NEXT i
   ENDIF
WEND
' ERASE temp.array()
return



procedure Quicksort1(low,high)
' ============================== Quicksort ===============================
' Quicksort works by picking a random "pivot" element in the array, then
' moving every element that is bigger to one side of the pivot, and every
' element that is smaller to the other side.  Quicksort is then called
' recursively with the two subdivisions created by the pivot.  Once the
' number of elements in a subdivision reaches two, the recursive calls
' end and the array is sorted.

' Note: Quicksort is indeed one word, the name assigned to this algorithm
' by its inventor C. A. R. Hoare.
' ========================================================================
local l,h
IF low<high

 ' ----------------------------------------------------------
 ' Only two elements in this subdivision; swap them
 ' if they are out of order, then end recursive calls.
 ' ----------------------------------------------------------
  IF high-low=1
     IF the.array(low)>the.array(high)
        @SWAP(low,high)
     ENDIF
  ELSE

 ' ----------------------------------------------------------
 ' Pick a pivot element at random, then move it to the end
 ' ----------------------------------------------------------
     random.nmbr= INT(RND(0)*(high-low+1))+low
     @SWAP(high,random.nmbr)
     partition=the.array(high)
     l=low 
     h=high
     repeat

 ' ----------------------------------------------------------
 ' Move in from both sides towards the pivot element
 ' ----------------------------------------------------------
        WHILE (l<h) AND (the.array(l)<=partition)
            inc l
        wend
        WHILE (h>l) AND (the.array(h)>=partition)
            dec h
        wend

 ' ----------------------------------------------------------
 ' If we haven't reached the pivot element, it means that two
 ' elements on either side are out of order, so swap them
 ' ----------------------------------------------------------
        IF l<h 
	  @SWAP(l,h)
        endif
     until not l<h

 ' ----------------------------------------------------------
 ' Move the pivot element back to its proper place in the array
 ' ----------------------------------------------------------
     @SWAP(l,high)
       
 ' ----------------------------------------------------------
 ' Recursively call the QuickSort procedure (pass the
 ' smaller subdivision first to use less stack space)
 ' ----------------------------------------------------------
     IF (l-low)<(high-l)
         @Quicksort1(low,l-1)
         @Quicksort1(l+1,high)
     ELSE
         @Quicksort1(l+1,high)
         @Quicksort1(low,l-1)
     ENDIF
  ENDIF
ENDIF
return



procedure Quicksort2(low,high)
local k,h,l,amid
' ============================== Quicksort ===============================
' This is a Quicksort routine that Mark Aitchison e-mailed to me.
' ========================================================================
k=0    ! still experimenting myself on what is best here

IF (high<low+k) OR (qlevel>20)
   FOR i=low+1 TO high
       best=i
       FOR j=i+1 TO high
           IF the.array(j)<the.array(best)
	    best=j
	   endif
       NEXT j
       @SWAP(best,i)
   NEXT i
   return
ENDIF

inc qlevel
l=low
h=high
amid=the.array(INT(RND(0)*(h-l)+1)+l)

WHILE l<h
   WHILE (the.array(l)<amid) AND (l<h)
       inc l
   wend
   WHILE (the.array(h)>amid) AND (l<h)
       dec h
   wend
   IF l<h
       @SWAP(l,h)
   ENDIF
wend

  IF h<l
    IF low<h 
      @Quicksort2(low,h)
    endif
    IF h+1<high
      @Quicksort2(h+1, high)
    endif
  ELSE
    IF low<l
      @Quicksort2(low,l)
    endif
    IF l+1<high
      @Quicksort2(l+1,high)
    endif
  ENDIF
  inc qlevel
return



procedure Quicksort3
' ===================== Quicksort (without recursion) ====================
' The Quick Sort version presented here avoids recursion, and instead uses
' a local array as a form of stack.  This array stores the upper and lower
' bounds showing which section of the array is currently being considered.
' Another refinement added is to avoid making a copy of elements in the
' array.  As the Quicksort progresses, it examines one element selected
' arbitrarily from the middle of the array, and compares it to the elements
' that lie above and below it.  To avoid assigning a temporary copy this
' version simply keeps track of the selected element number.
' =========================================================================

first=1
last=dim?(the.array())
DIM QStack(INT(LOG(last)/LOG(2)*2+12))
stackptr=0
DO
  repeat
    pivot=the.array(int((last+first)/2))
    pivot=the.array(INT(RND(0)*(last-first)+1)+first)

    i=first
    j=last
    repeat
      WHILE the.array(i)<pivot
        inc i
      wend

      WHILE the.array(j)>pivot
        dec j
      wend

      exit IF i>j
      IF i<j
        @SWAP(i,j)
      endif
      inc i
      dec j
    until i>j

    IF i<last
      QStack(stackptr)=i
      QStack(stackptr+1)=last
      stackptr=stackptr+2
    ENDIF

    last=j
  until first>=last

  exit IF stackptr=0
  stackptr=stackptr-2
  first=QStack(stackptr)
  last=QStack(stackptr+1)
LOOP

ERASE QStack()
return



procedure Save.Array.To.File(sw)
  IF sw=0
    text$="BEFORE" 
  ELSE 
    text$="AFTER"
  endif
  OPEN "A",#1,"proof.txt"

' ---------------------------
' show the first 50 elements.
' ---------------------------
PRINT #1,"The 1st 50 items of the array ";text$;
PRINT #1," the ";RTRIM$(algorithm$(algorithm));
PRINT #1," with array type #";array.type
FOR count=1 TO 50
    IF count<=nmbr.of.items
       PRINT #1,the.array(count);
    ENDIF
NEXT
PRINT #1,""
PRINT #1,""


' --------------------------------------
' ...and let's show the last 50 elements
' --------------------------------------
PRINT #1,"The 1st 50 items of the array ";text$;
PRINT #1," the ";RTRIM$(algorithm$(algorithm));
PRINT #1," with array type #";array.type
FOR count=nmbr.of.items-50+1 TO nmbr.of.items
    IF count<=nmbr.of.items
       PRINT #1,the.array(count);
    ENDIF
NEXT
PRINT #1,""
PRINT #1,""
PRINT #1,STRING$(72, "-")
PRINT #1,""
CLOSE #1
return



procedure Save.Rslts.To.File
OPEN "A",#1,"srt-rslt.txt"

PRINT #1,"The times listed are the number of seconds that"
PRINT #1,"it takes to sort a";nmbr.of.items;"element numeric array."

PRINT #1," "
PRINT #1,STRING$(38," "); "-- Type of array to be sorted --"
PRINT #1," "
PRINT #1,STRING$(31," "); "Already   Complete   Random    Nearly    Merge"
PRINT #1,STRING$(31," "); "Sorted    Opposite   Mix-up    Sorted     Two "
PRINT #1,STRING$(31," "); "----------------------------------------------"
PRINT #1," "

FOR algorithm=1 TO nmbr.of.algorithms
    PRINT #1,algorithm$(algorithm);
    FOR array.type=1 TO 5
       IF array.type=5
          PRINT #1,USING "####.####";results!(array.type,algorithm)
      ELSE
          PRINT #1,USING "####.####";results!(array.type,algorithm);
          PRINT #1," ";
       ENDIF
    NEXT
NEXT
PRINT #1,""
PRINT #1,STRING$(77,chr$(196))
PRINT #1,""

CLOSE #1
return


procedure Shell.Sort
' =============================== Shell Sort ===============================
' The Shell Sort procedure is similar to the Bubble Sort procedure.
' However, the Shell Sort begins by comparing elements that are far apart
' (separated by the value of the offset variable, which is initially half
' the distance between the first and last element), then comparing elements
' that are closer together. (When offset is one, the last iteration of this
' procedure is merely a bubble sort.)

' I think the Shell Sort is my favorite.  No potential problems with
' recursion, no additional overhead is needed as far as requiring another
' temporary array, no calls to other subroutines, a simple easy to
' understand algorithm... and very impressive sort times too.
' =========================================================================

center=int(nmbr.of.items/2)
WHILE center>0
  boundary=nmbr.of.items-center
  repeat
     flag=0
     FOR a=1 TO boundary
        IF the.array(a)>the.array(a+center)
           @SWAP(a,a+center)
           flag=a
        ENDIF
     NEXT a
     boundary=flag-center
  until flag=0
  center=int(center/2)
wend
return



procedure Sort.The.Array(algorithm)
  start!=timer
  qlevel=0
  on algorithm gosub Bubble.Sort1,Bubble.Sort2,Bubble.Sort3,Bubble.Sort4,Exchange.Sort,Heap.Sort,Insertion.Sort,Merge.Sort,Quicksort1(1,nmbr.of.items),Quicksort2(1,nmbr.of.items),Quicksort3,Shell.Sort
  finish!=TIMER
  results!(array.type,algorithm)=finish!-start!
return



procedure Store.Array.Items(array.type)
SELECT array.type
  CASE 1
  ' ----------------------------------------
  ' Best case scenario.  The array is already
  ' in the order that we want.
  ' ----------------------------------------
    FOR count=1 TO nmbr.of.items
       the.array(count)=count
    NEXT


  CASE 2
  ' ----------------------------------------
  ' Worst case scenario. The array is in the
  ' exact opposite order of what we want!
  ' ----------------------------------------
   FOR count=nmbr.of.items TO 1 STEP -1
      position=position+1
      the.array(position)=count
   NEXT


  CASE 3
  ' -----------------------------------------------------
  ' A completely random scenario, like a well shuffled
  ' deck of cards.  Note that this shuffling routine will
  ' do away with the need for an additonal array that
  ' holds the sorted array before it is sorted.  It
  ' will shuffle the array the same way every time so that
  ' each algorithm will be sorting the exact same shuffled
  ' array, for a true speed comparison.
  ' -----------------------------------------------------
   
    FOR count=1 TO nmbr.of.items
       the.array(count)=count
    NEXT
    seed=-1
    FOR count=1 TO nmbr.of.items
      random.nmbr=INT(RND(seed)*nmbr.of.items)+1
      SWAP the.array(count),the.array(random.nmbr)
      seed=nmbr.of.items-count+1
    NEXT


  CASE 4
    x=20
    ' ----------------------------------------------
    ' The first (nmbr.of.items - x) are in order.
    ' However the final x items, located all at the end
    ' of the array, need to be sorted within the main
    ' body.  For example an array of 14 elements with a
    ' value of 3 for x would look like this:
    '    1  2  3  5  6  7  9  10  11  13  14  4  12  8
    ' -----------------------------------------------
    FOR count=1 TO nmbr.of.items
       the.array(count)=count
    NEXT
    seed=-1
    FOR a=1 TO x
        random.nmbr=INT(RND(seed)*nmbr.of.items)+1
        SWAP the.array(random.nmbr),the.array(nmbr.of.items)
        FOR b = random.nmbr TO (nmbr.of.items - 2)
            SWAP the.array(b), the.array(b + 1)
        NEXT
        seed = nmbr.of.items - a + 1
    NEXT a


  CASE 5
    ' ----------------------------------------------------
    ' This description on the screen is called "Merge Two"
    ' which isn't quite accurate... it's still one array.
    ' However the data is basically split into two halves,
    ' of which each half is already sorted.  It simply needs
    ' to be merged together.  For example, given an array of
    ' 15 elements, the array that needs to be sorted would be:
    '    1  3  5  7  9  11  13  2  4  6  8  10  12  14  15
    ' the code below assembles it together this way nicely.
    ' ----------------------------------------------------

    midpoint = nmbr.of.items \ 2
    FOR count = 1 TO midpoint
        the.array(count) = (count * 2) - 1
        the.array(midpoint + count) = (count * 2)
    NEXT

    ' a little more work if the nmbr.of.items is an odd number...
    remainder = nmbr.of.items MOD 2
    IF remainder = 1 THEN the.array(nmbr.of.items) = nmbr.of.items

ENDSELECT
return
procedure color(c1,c2)
  print chr$(27)+"["+str$(1)+";"+str$(30+c1)+";"+str$(40+c2)+"m";
return
