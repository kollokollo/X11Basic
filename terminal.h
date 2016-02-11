/* terminal.h                            (c) Markus Hoffmann  2007-2008
*/

/* This file is part of TTconsole and X11-Basic, the TomTom Console interface
 * ======================================================================
 * TTconsole is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

void gg_outs(char *t);
void cursor_onoff(int onoff);
void textscreen_redraw(int x,int y,int w,int h);

typedef struct {
        unsigned char page;  /* for unicode */
        unsigned char c;
	unsigned short color;
	unsigned short bcolor;
	unsigned char flags;
	unsigned char fontnr;
} CINFO;


#define AT_NORMAL       0
#define AT_KEYLOCK      4
#define AT_INSERT     0x10
#define AT_SOFTSCROLL 0x20

#define AT_LINEWRAP  (1<<7)
#define AT_KEYREPEAT (1<<8)
#define AT_MOUSE     (1<<10)
#define AT_AUTOLF    (1<<20)
#define AT_CURSORON   (1<<25)

#define AT_DEFAULT (AT_LINEWRAP|AT_CURSORON)


extern int terminal_fd;
extern int col,lin;
extern int attributes;
extern void set_terminal_fd(int fd);
extern void init_textscreen();
