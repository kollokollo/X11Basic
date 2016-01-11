/* sound.h  (c) Markus Hoffmann*/

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#ifdef USE_SDL
#include <SDL/SDL.h>
#include <SDL/SDL_audio.h>
#else
#define Uint32 unsigned long
#define Uint16 unsigned short
#define Uint8 unsigned char
#endif


extern int soundsystem_available;

int init_soundsystem();
int do_sound(int c,double freq, double volume, double duration);
int do_wave(int c,int form, double attack, double decay, double sustain, double release);
int do_playsound(int c, char *data, int size, int pitch, int colume, int flag);
void sound_activate();

#define MAX_SOUND_CHANNELS 16
