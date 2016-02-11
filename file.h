/* file.h  (c) Markus Hoffmann   */
/* Erweiterungen fuer die Datei Ein- und Ausgabe ....   */


/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
 

/* prototypes */ 

char *lineinput( FILE *, char *,int );
char *input( FILE *, char *,int );

size_t lof(FILE *);
int myeof( FILE *);

int bsave(const char *, char *, size_t);
size_t bload(const char *, char *, size_t);

int exist(const char *);

int stat_device(const char *filename);
int stat_inode( const char *filename);
int stat_mode(  const char *filename);
int stat_nlink( const char *filename);
int stat_uid(   const char *filename);
int stat_gid(   const char *filename);
int stat_size(  const char *filename);
