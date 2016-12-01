/* raw_mouse.h     Routinen fuer Maus-Driver (c) Markus Hoffmann    */


/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ======================================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
//#define TS_DEVICE_NAME "/dev/ts"
#define MOUSE_DEVICE "/dev/input/mice"



extern const unsigned char mousealpha[];
extern const unsigned short mousepat[];



int Fb_Mouse_Open();
void FB_close_mouse();
void FB_Query_pointer(int *rx,int *ry,int *x,int *y,unsigned int *k);
void FB_defmouse(int form);
void FB_show_mouse();
void FB_hide_mouse();
void FB_showx_mouse();
void FB_hidex_mouse();
#ifdef ANDROID
#define FB_mouse_events(a) ;
#else
void FB_mouse_events(int onoff);
#endif
