/* XBASIC.H (c) Markus Hoffmann  */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
#include "ptypes.h" 
 
/* Deklarationen von Hilfsfunktionen */

int labelnr(char *n);
int procnr(char *n,int typ);
int labelzeile(char *n);
int suchep(int begin, int richtung, int such, int w1, int w2);
int init_program();
int mergeprg(char *fname);

char *indirekt2(char *funktion);
int type2(char *ausdruck);
void clear_parameters();
void clear_labelliste();
void clear_procliste();
void programmlauf();
int saveprg(char *fname);
void kommando(char *cmd);
void structure_warning(char *comment);
void loadprg(char *filename);
int make_pliste(int pmin,int pmax,short *pliste,char *n, PARAMETER **pr);
