/* consolefont.h                            (c) Markus Hoffmann  2007-2008
*/

/* This file is part of TTconsole, the TomTom Console interface
 * ======================================================================
 * TTconsole is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

extern const unsigned char unicode_font[];
extern const unsigned short *unicode_pages[];
extern const unsigned char unicode_font57[];
extern const unsigned short *unicode_pages57[];

void Fb_BlitCharacter(int x, int y, unsigned short aColor, unsigned short aBackColor, unsigned char character,int flags,int);
void Fb_BlitText(int x, int y, unsigned short aColor, unsigned short aBackColor, char *string);

void Fb_BlitText57(int x, int y, unsigned short aColor, unsigned short aBackColor, const char *str);
void Fb_BlitText816(int x, int y, unsigned short aColor, unsigned short aBackColor, const char *str);
void Fb_BlitText1632(int x, int y, unsigned short aColor, unsigned short aBackColor, const char *str);
void Fb_BlitText3264(int x, int y, unsigned short aColor, unsigned short aBackColor, const char *str);
void Fb_BlitText816_scale(int x, int y, unsigned short aColor, unsigned short aBackColor, const char *str,int w,int h);



void change_fontsize(int big);
void cursor_onoff(int onoff);
void Fb_BlitUnicodeCharacter(int x, int y, unsigned short aColor, unsigned short aBackColor, unsigned int unicode, int flags, int fontnr);
int utf8(unsigned char a,unsigned short *runicode);
const unsigned char *unicode2glyph816(unsigned short unicode);
const unsigned char *unicode2glyph57(unsigned short unicode);
