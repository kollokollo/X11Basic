/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
#ifndef OPTIONS_H
#define OPTIONS_H

/* Language setting:   english is the default   */

/* #define GERMAN  */

/* If you have to save memory space...*/

/*#define SAVE_RAM*/

/* Kontrollsystemanbindung. Nur wichtig fuer Benutzer bei ELSA in Bonn /*
/* und DESY in Hamburg. sonst bitte abschalten */

#ifndef WINDOWS
  #ifndef CONTROL
  /*#define CONTROL*/
  #endif 
#endif
#ifndef TINE
//  #define TINE  
#endif 

#ifdef WINDOWS
  #undef HAVE_READLINE
#endif

/* If you do not want to have the graphics ... */

/*#define NOGRAPHICS*/


/* If you are allowed to use stron cryptographics ... */

/*#define USE_BLOWFISH*/


#endif /* OPTIONS_H */
