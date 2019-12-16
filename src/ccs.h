/* ccs.h  (c) Markus Hoffmann*/

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

void mqtt_init();
void mqtt_exit();
void c_publish(PARAMETER *plist, int e);
void c_subscribe(PARAMETER *plist, int e);
void c_broker(PARAMETER *plist, int e);
