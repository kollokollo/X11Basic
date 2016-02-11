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
#  ifndef CONTROL
  /*#define CONTROL*/
#  endif
#endif
#ifndef TINE
//  #define TINE
#endif 
#ifndef DOOCS
//#  define TINE
//#  define DOOCS
#endif

#ifdef WINDOWS
  #undef HAVE_READLINE
  #define X_DISPLAY_MISSING
  #define WINDOWS_NATIVE
#endif


/* If you do not want to have the graphics ... */

// #define NOGRAPHICS


#if defined HAVE_SDL && defined HAVE_SDL_GFX
  #ifndef NOGRAPHICS
    #ifdef WINDOWS
    #define USE_SDL
    #undef WINDOWS_NATIVE
    #endif
  #endif
#endif

//    #define USE_SDL

#ifndef X_DISPLAY_MISSING
  #ifndef USE_SDL
    #ifndef NOGRAPHICS
      #define USE_X11
    #endif
  #endif
#endif




/* If you are allowed to use stron cryptographics ... */

/*#define USE_BLOWFISH*/


#endif /* OPTIONS_H */
