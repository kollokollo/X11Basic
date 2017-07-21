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

/* If you want to compile a versin of X11-basic which uses the SDL library
   also on unix systems, you can uncomment following line. This is useful 
   to have a version which is more compatible with the WINDOWS version of
   X11-Basic.*/

/*#define USE_SDL*/


#ifdef WINDOWS
  #undef HAVE_READLINE
 // #undef HAVE_LAPACK
  #undef HAVE_CLOG10
 // #undef HAVE_EXECVPE
  #undef HAVE_FUTIMENS
  #undef HAVE_UTIMENSAT
  #undef HAVE_FUTIMES
  #undef HAVE_GCRYPT
  #undef HAVE_PTY_H
  #undef HAVE_SYS_SOCKET_H
  #undef HAVE_SYS_KD_H
  #undef HAVE_DLOPEN
  #undef HAVE_INOTIFY_INIT
  #undef HAVE_USB
  #undef HAVE_PRIMORIAL_UI
  #undef HAVE_FFTW
  #undef HAVE_RFFTW
  #undef HAVE_BLUETOOTH 
  #undef HAVE_BACKTRACE 
  #define USE_SDL
  #define HAVE_SDL 1 
  #define HAVE_SDL_GFX 1
  #undef SIZEOF_VOID_P
  #define SIZEOF_VOID_P 4
#elif defined ATARI
  #undef HAVE_READLINE
  #undef HAVE_LAPACK
  #undef HAVE_SYS_KD_H
  #undef HAVE_COMPLEX_H
  #undef HAVE_DLOPEN
  #undef HAVE_INOTIFY_INIT
  #undef HAVE_ALSA
  #undef HAVE_EXECVPE
  #undef HAVE_GMP
  #undef HAVE_CACOS
  #undef USE_X11
 // #define USE_SDL
 // #define HAVE_SDL 1
  #define HAVE_GEM 1
  #define USE_GEM 1
  #define SAVE_RAM 1
#endif


#ifndef HAVE_SDL 
  #undef USE_SDL
#endif
#ifndef HAVE_SDL_GFX 
  #undef USE_SDL
#endif
#ifndef HAVE_X11
  #undef USE_X11
#endif
#ifndef HAVE_GEM
  #undef USE_GEM
#endif

#if !defined FRAMEBUFFER && !defined USE_SDL && !defined USE_X11 && !defined USE_GEM
/* die NOGRAPHICS version ist aber inkompatibel, da einige Kommandos ganz ausgelassen
   werden.*/
  #define NOGRAPHICS
#endif

#ifndef HAVE_TINE
  #undef TINE
  #undef DOOCS
#endif
#ifndef HAVE_DOOCS
  #undef DOOCS
#endif


#ifdef HAVE_ALSA
  #ifndef USE_SDL
    #define USE_ALSA
  #endif
#endif

/*Systems with small Memory Layout*/
#ifdef SMALL
  #define SAVE_RAM
#endif

#endif /* OPTIONS_H */
