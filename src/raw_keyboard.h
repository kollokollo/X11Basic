/* raw_keyboard.h     Routinen fuer Event-Driver (c) Markus Hoffmann    */


/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ======================================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
#define KEYBOARD_DEVICE "/dev/input/event4"


void FB_close_keyboard();
int Fb_Keyboard_Open();
void FB_keyboard_events(int onoff);
