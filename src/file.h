/* file.h  (c) Markus Hoffmann   */
/* Erweiterungen fuer die Datei Ein- und Ausgabe ....   */


/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
 
 
 
 
/* prototypes */ 

char *lineinput( FILE *, char *);
char *input( FILE *, char *);

size_t lof(FILE *);
int myeof( FILE *);

int bsave( char *, char *, size_t);
size_t bload( char *, char *, size_t);

int exist( char *);

int stat_device(char *filename);
int stat_inode(char *filename);
int stat_mode(char *filename);
int stat_nlink(char *filename);
int stat_uid(char *filename);
int stat_gid(char *filename);
int stat_size(char *filename);
