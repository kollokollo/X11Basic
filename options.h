/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
#ifndef OPTIONS_H
#define OPTIONS_H

/* Some systems can't handle large BSS segments (e.g. Linux), so this may be set
 * in config.h
 */
#ifndef SMALL
#define SMALL 1
#endif

/* Kontrollsystemanbindung. Nur wichtig fuer Benutzer bei ELSA in Bonn /*
/* sonst bitte abschalten */

/*#undef CONTROL*/

#endif /* OPTIONS_H */
