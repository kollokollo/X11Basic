/* gkommandos.h Grafikroutinen  (c) Markus Hoffmann   */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

void c_alert    (PARAMETER *,int);
void c_allevent (char *);
void c_boundary (PARAMETER *,int);
void c_bottomw  (PARAMETER *,int);
void c_box      (PARAMETER *,int);
void c_clearw   (PARAMETER *,int);
void c_clip     (PARAMETER *,int);
void c_closew   (PARAMETER *,int);
void c_circle   (PARAMETER *,int);
void c_color    (PARAMETER *,int);
void c_copyarea  (PARAMETER *,int);
void c_deffill    (PARAMETER *,int);
void c_defline     (PARAMETER *,int);
void c_defmark    (PARAMETER *,int);
void c_defmouse    (char *);
void c_deftext     (PARAMETER *,int);
void c_draw        (char *);
void c_ellipse     (PARAMETER *,int);
void c_fileselect  (PARAMETER *,int);
void c_fill        (PARAMETER *,int);
void c_fullw       (PARAMETER *,int);
void c_get         (PARAMETER *,int);
void c_gprint      (char *);
void c_graphmode   (PARAMETER *,int);
void c_infow       (PARAMETER *,int);
void c_keyevent    (char *);
void c_line        (PARAMETER *,int);
void c_ltext       (char *);
void c_menu        (char *);
void c_menudef     (char *);
void c_menukill    (char *);
void c_menuset     (PARAMETER *,int);
void c_motionevent (char *);
void c_mouse       (PARAMETER *,int);
void c_mouseevent  (char *);
void c_movew       (PARAMETER *,int);
void c_norootwindow(char *);
void c_openw       (PARAMETER *,int);
void c_pbox        (PARAMETER *,int);
void c_pcircle     (PARAMETER *,int);
void c_pellipse    (PARAMETER *,int);
void c_plot        (PARAMETER *,int);
void c_polyfill    (char *);
void c_polyline    (char *);
void c_polymark    (char *);
void c_prbox       (PARAMETER *,int);
void c_put         (PARAMETER *,int);
void c_put_bitmap  (PARAMETER *,int);
void c_rbox        (PARAMETER *,int);
void c_rootwindow  (char *);
void c_savescreen  (PARAMETER *,int);
void c_savewindow  (PARAMETER *,int);
void c_scope       (char *);
void c_screen      (PARAMETER *,int);
void c_setfont     (char *);
void c_setmouse    (PARAMETER *,int);
void c_sget        (char *);
void c_sizew       (PARAMETER *,int);
void c_sput        (PARAMETER *,int);
void c_text        (char *);
void c_titlew      (PARAMETER *,int);
void c_topw        (PARAMETER *,int);
void c_usewindow   (PARAMETER *,int);
void c_vsync       (char *);
void c_xload       (char *);
void c_xrun        (char *);
