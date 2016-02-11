/* FDATA.C fuer ANSI-C , UNIX                     */
/* Fontdaten fuer Linienfont   (c) Markus Hoffmann*/
/* LINEFONT-OUTPUT > 06.04.1989 */
/* (c) by Markus Hoffmann 1995 */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

/* Kleine Aenderung an der "6" aufgrund v. Anregung 
    von Jardine A.R. <@swansea.ac.uk> Tue, 3 Aug 2010 11:53:38 
   vollst. "Uberarbeitung 2011-09-24 MH    */

const unsigned char font[128][36] = {
{0}, /* 0 */
{100,50,90,50,10,85,40,151,10,15,40}, /* 1 */
{100,50,10,50,90,85,60,151,90,15,60}, /* 2 */
{100,1,55,100,55,60,30,201,55,60,80}, /* 3 */
{100,100,55,1,55,40,30,102,55,40,80}, /* 4 */
{100,1,50,50,1,100,50,50,100,1,50,150,25,50,60,150,70,50,75}, /* 5 */
{100,18,25,30,10,70,10,82,25,50,50,50,65,151,80,50,95,101,1,100,1,50,100,1,1}, /* 6 */
{100,30,1,69,1,99,30,99,69,69,99,30,99,1,69,1,30,30,1}, /* 7 */
{100,1,50,50,100,100,1}, /* 8 */
{0}, /* 9 */
{0}, /* 10 */
{0}, /* 11 */
{0}, /* 12 */
{0}, /* 13 */
{0}, /* 14 */
{0}, /* 15 */
{100,1,100,1,30,30,1,60,1,90,25,50,50,100,80,50,100,15,100}, /* � 16 */
{100,1,100,1,30,30,1,70,1,100,30,100,100,100,50,1,50,201,1,100,2,102,1,1,2}, /* 17 � */
{100,1,60,30,40,70,40,100,60,100,100,201,70,70,100,30,100,1,80,30,60,70,60,100,80,171,20,70,21,131,20,30,21}, /* 18 � */
{100,100,30,70,21,30,21,1,30,1,70,30,100,70,100,100,70,100,30,171,1,70,2,131,1,30,2}, /* 19 �*/
{100,70,40,30,40,1,70,30,100,70,100,100,70,70,40,171,20,70,21,131,20,30,21}, /* 20 � */
{100,1,1,1,75,25,100,75,100,100,75,100,1,171,1,70,2,131,1,30,2}, /* � 21 */
{100,1,40,1,70,30,100,70,100,100,70,100,40,201,40,100,100,171,20,70,21,131,20,30,21}, /* � 22 */
{0}, /* 23 */
{0}, /* 24 */
{0}, /* 25 */
{0}, /* 26 */
{0}, /* 27 */
{0}, /* 28 */
{0}, /* 29 */
{0}, /* 30 */
{0}, /* 31 */
{70}, /* 32 */
{20,10,1,10,70,111,90,10,100}, /* 33 */
{50,10,1,10,30,141,1,40,30}, /* 34 */
{100,1,30,100,30,101,70,100,70,131,1,30,100,171,1,70,100}, /* 35 */
{100,100,25,75,1,25,1,1,25,25,50,75,50,100,75,75,100,25,100,1,75,141,1,40,110,161,1,60,110}, /* 36 */
{100,15,100,100,1,121,10,30,10,30,20,20,20,20,10,191,90,80,90,80,80,90,80,90,90}, /* 37 */
{100,100,50,40,100,1,80,60,30,40,1,10,30,100,100}, /* 38 */
{60,40,1,20,30}, /* 39 */
{100,60,1,40,30,40,70,60,100}, /* 40 */
{100,40,1,60,30,60,70,40,100}, /* 41 */
{100,10,50,90,50,151,10,50,90,121,20,80,80,181,20,20,80}, /* 42 */
{100,10,50,90,50,151,10,50,90}, /* 43 */
{30,20,90,1,120}, /* 44 */
{100,10,50,90,50}, /* 45 */
{20,1,99,1,100}, /* 46 .*/
{100,10,100,90,1}, /* 47 */
{100,100,30,70,1,30,1,1,30,1,70,30,100,70,100,100,70,100,30,141,60,60,40}, /* 48 */
{100,40,10,50,1,50,100}, /* 49 */
{100,1,30,30,1,70,1,100,20,100,40,1,75,1,100,100,100}, /* 50 */
{100,10,20,30,1,70,1,100,20,100,30,50,50,100,70,100,80,70,100,30,100,10,80}, /* 51 */
{100,70,100,70,1,10,60,100,60}, /* 52 */
{100,100,1,1,1,1,50,70,50,100,70,100,80,70,100,30,100,1,90}, /* 53 */
{100,80,1,30,1,1,30,1,70,30,100,70,100,100,75,100,75,75,50,1,50}, /* 54 '6' */
{100,1,1,100,1,100,20,30,100}, /* 55 */
{100,1,25,25,1,75,1,100,25,75,50,25,50,1,25,126,50,1,75,25,100,75,100,100,75,75,50}, /* 56 */
{100,1,25,25,1,75,1,95,25,95,75,75,100,35,100,10,85,101,25,25,50,95,50}, /* 57 */
{40,20,30,20,31,121,80,20,81}, /* 58 : */
{40,20,30,20,31,121,70,10,100}, /* 59 */
{100,90,20,1,50,90,80}, /* 60 */
{100,10,30,90,30,111,70,90,70}, /* 61 */
{100,10,20,100,50,10,80}, /* 62 */
{100,10,25,35,1,75,1,100,25,50,50,50,70,151,90,50,100}, /* 63 */
{100,100,20,70,1,30,1,1,30,1,70,30,100,70,100,90,90,201,30,100,60,70,70,50,70,35,60,35,45,50,35,65,50,65,70}, /* 64 */
{100,1,100,1,30,30,1,70,1,100,30,100,100,100,50,1,50}, /* 65 */
{100,1,100,1,1,70,1,90,10,90,40,50,50,1,50,50,50,90,60,90,90,70,100,1,100}, /* 66 */
{100,100,20,70,1,30,1,1,30,1,70,30,100,70,100,100,80}, /* 67 */
{100,1,1,1,100,70,100,100,70,100,30,70,1,1,1}, /* 68 */
{100,100,1,1,1,1,100,100,100,101,50,85,50}, /* 69 */
{100,100,1,1,1,1,100,101,50,85,50}, /* 70 */
{100,100,20,70,1,30,1,1,30,1,70,30,100,80,100,100,90,100,50,50,50}, /* 71 */
{100,1,1,1,100,101,50,100,50,201,100,100,1}, /* 72 */
{40,20,1,20,100,101,1,40,1,101,100,40,100}, /* 73 */
{50,10,1,50,1,131,1,30,90,20,100,1,100}, /* 74 */
{100,1,1,1,100,101,50,100,1,101,50,100,100}, /* 75 */
{100,1,1,1,100,100,100}, /* 76 */
{100,1,100,1,1,50,60,100,1,100,100}, /* 77 */
{100,1,100,1,1,100,100,100,1}, /* 78 */
{100,100,30,70,1,30,1,1,30,1,70,30,100,70,100,100,70,100,30}, /* 79 */
{100,1,100,1,1,75,1,100,25,75,50,1,50}, /* 80 */
{100,100,30,70,1,30,1,1,30,1,70,30,100,70,100,100,70,100,30,171,70,100,100}, /* 81 */
{100,1,100,1,1,75,1,100,25,75,50,1,50,100,100}, /* 82 */
{100,100,25,75,1,25,1,1,25,25,50,75,50,100,75,75,100,25,100,1,75}, /* 83 */
{100,1,1,100,1,151,1,50,100}, /* 84 */
{100,1,1,1,75,25,100,75,100,100,75,100,1}, /* 85 */
{100,1,1,50,100,100,1}, /* 86 */
{100,1,1,25,100,50,20,75,100,100,1}, /* 87 */
{100,1,1,100,100,201,1,1,100}, /* 88 */
{100,1,1,50,50,100,1,151,50,50,100}, /* 89 */
{100,1,1,100,1,1,100,100,100}, /* 90 */
{100,50,1,10,1,10,100,50,100}, /* 91 */
{100,10,1,90,100}, /* 92 */
{100,50,1,90,1,90,100,50,100}, /* 93 */
{100,10,30,50,1,90,30}, /* 94 */
{100,1,100,100,100}, /* 95 */
{100,50,1,60,30}, /* 96 */
{100,10,40,30,30,80,30,100,40,100,100,201,80,70,100,30,100,1,80,30,60,70,60,100,80}, /* 97 */
{100,1,60,30,30,70,30,100,60,100,70,70,100,30,100,1,80,101,1,1,100}, /* 98 */
{100,95,50,70,30,30,30,1,60,1,70,30,100,70,100,95,80}, /* 99 */
{100,100,60,70,30,30,30,1,60,1,70,30,100,70,100,100,80,201,1,100,100}, /* 100 */
{100,1,65,100,65,70,30,30,30,1,60,1,80,30,100,70,100,95,85}, /* 101 */
{80,90,10,70,1,50,20,50,110,121,40,80,40}, /* 102 */
{100,100,50,70,30,30,30,1,60,1,70,30,100,70,100,100,80,201,40,100,120,60,130,20,120,60,110,100,120}, /* 103 */
{100,1,1,1,100,101,50,25,30,75,30,100,50,100,100}, /* 104 */
{40,20,30,20,100,121,1,20,11}, /* 105 */
{40,20,30,20,110,10,130,1,130,121,1,20,11}, /* 106 */
{100,1,1,1,100,101,60,100,100,101,60,90,30}, /* 107 */
{20,10,1,10,100,15,100}, /* 108 */
{100,1,30,1,100,101,50,25,30,30,30,50,60,50,80,151,60,70,30,75,30,100,50,100,100}, /* 109 */
{100,1,30,1,100,101,50,25,30,75,30,100,50,100,100}, /* 110 */
{100,70,30,30,30,1,60,1,70,30,100,70,100,100,70,100,60,70,30}, /* 111 */
{100,1,50,30,30,70,30,100,60,100,70,70,100,30,100,1,80,102,30,1,130}, /* 112 */
{100,100,50,70,30,30,30,1,60,1,70,30,100,70,100,100,80,201,30,100,130}, /* 113 */
{70,1,100,1,30,101,50,30,30,70,30}, /* 114 */
{100,100,40,75,30,25,30,1,45,25,65,75,65,100,80,75,100,25,100,1,85}, /* 115 */
{90,50,1,50,90,70,100,111,30,90,30}, /* 116 */
{100,1,30,1,70,30,100,70,100,100,70,100,30,100,100}, /* 117 */
{100,1,30,50,100,100,30}, /* 118 */
{100,1,30,25,100,50,40,75,100,100,30}, /* 119 */
{100,1,100,100,30,101,30,100,100}, /* 120 */
{100,1,30,50,100,201,30,30,130}, /* 121 */
{100,1,30,100,30,1,100,100,100}, /* 122 */
{100,1,50,25,50,25,1,50,1,126,50,25,100,50,100}, /* 123 */
{100,50,1,50,100}, /* 124 */
{100,100,50,75,50,75,1,50,1,176,50,75,100,50,100}, /* 125 */
{100,1,50,25,25,50,50,75,75,100,50}, /* 126 */
{100,1,100,100,100,50,40,1,100} /* 127 */   };

