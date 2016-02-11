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


#ifdef WINDOWS
  #undef HAVE_READLINE
 // #undef HAVE_EXECVPE
  #undef HAVE_FUTIMENS
  #undef HAVE_PTY_H
  #undef HAVE_SYS_SOCKET_H
  #undef HAVE_SYS_KD_H
  #define X_DISPLAY_MISSING
  #define USE_SDL
  #define HAVE_SDL 1 
  #define HAVE_SDL_GFX 1 
#else
  #define USE_X11
#endif


#if defined FRAMEBUFFER
  #define X_DISPLAY_MISSING
#endif

#ifdef NOGRAPHICS
  #undef HAVE_SDL
  #undef USE_SDL
  #undef FRAMEBUFFER
  #define X_DISPLAY_MISSING
#endif

#if defined USE_SDL
  #undef USE_X11
#endif

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

#ifndef HAVE_SDL 
  #undef USE_SDL
#endif
#ifndef HAVE_SDL_GFX 
  #undef USE_SDL
#endif


/* If you want to compile a versin of X11-basic which uses the SDL library
   also on unix systems, you can uncomment following line. This is useful 
   to have a version which is more compatible with the WINDOWS version of
   X11-Basic.*/


#ifdef X_DISPLAY_MISSING
  #undef USE_X11
#endif

#ifdef HAVE_ALSA
  #ifndef USE_SDL
    #define USE_ALSA
  #endif
#endif



/* If you are allowed to use stron cryptographics ... */

/*#define USE_BLOWFISH*/


#endif /* OPTIONS_H */
