/* kommandos.h Kommandos  (c) Markus Hoffmann  */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

void c_cont(const char *n);
void c_dump(PARAMETER *plist,int e);
void c_gosub(const char *n);
void c_new(const char *n);
void c_quit(PARAMETER *plist, int e);
void c_stop();
void c_void(const char *n);

void do_run();
void do_help(const char *w);
char *plist_paramter(PARAMETER *p);
