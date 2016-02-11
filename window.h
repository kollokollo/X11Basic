/* Window.h  (c) Markus Hoffmann*/

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#include <X11/Xlib.h>
#include <X11/Xutil.h>


#define MAXWINDOWS 16

void handle_event(int,XEvent *);
void handle_window(int);
int create_window(char *, char *,unsigned int,unsigned int,unsigned int,unsigned int);
void open_window( int);
void close_window(int);
char *imagetoxwd(XImage *,Visual *,XColor *, int *);


/* globale Variablen */

int winbesetzt[MAXWINDOWS];
Window win[MAXWINDOWS];                 
Pixmap pix[MAXWINDOWS];
Display *display[MAXWINDOWS];            
GC gc[MAXWINDOWS];                      /* Im Gc wird Font, Farbe, Linienart, u.s.w.*/

extern int usewindow;
XSizeHints size_hints[MAXWINDOWS];       /* Hinweise fuer den WIndow-Manager..*/
XWMHints wm_hints[MAXWINDOWS];
XClassHint class_hint[MAXWINDOWS];
XTextProperty win_name[MAXWINDOWS], icon_name[MAXWINDOWS];
char wname[MAXWINDOWS][80];
char iname[MAXWINDOWS][80];

/* AES-Definitionen   */

typedef struct {
	int	x;
	int	y;
	int	w;
	int	h;
}RECT;

#ifdef 0
typedef struct objc_colorword {
   unsigned borderc : 4;
   unsigned textc   : 4;
   unsigned opaque  : 1;
   unsigned pattern : 3;
   unsigned fillc   : 4;
}OBJC_COLORWORD;

typedef struct text_edinfo {
	char           *te_ptext;     /* ptr to text */
	char           *te_ptmplt;    /* ptr to template */
	char           *te_pvalid;    /* ptr to validation chrs. */
	int            te_font;       /* font */
	int            te_fontid;     /* font id */
	int            te_just;       /* justification */
	OBJC_COLORWORD te_color;      /* color information word */
	int            te_fontsize;   /* font size */
	int            te_thickness;  /* border thickness */
	int            te_txtlen;     /* length of text string */
	int            te_tmplen;     /* length of template string */
} TEDINFO;

typedef struct {
    unsigned character   :  8;
    signed   framesize   :  8;
    unsigned framecol    :  4;
    unsigned textcol     :  4;
    unsigned textmode    :  1;
    unsigned fillpattern :  3;
    unsigned interiorcol :  4;
} bfobspec;

typedef union __u_ob_spec {
	TEDINFO           *tedinfo;
	long              index;
	char              *free_string;
  union __u_ob_spec *indirect;
	bfobspec          obspec;
/*	BITBLK            *bitblk;
	ICONBLK           *iconblk;
	CICONBLK          *ciconblk;
	struct user_block *userblk;  */
} U_OB_SPEC;

typedef struct object {
	int		ob_next;	/* -> object's next sibling	  	  */
	int		ob_head;	/* -> head of object's children 	  */
	int		ob_tail;	/* -> tail of object's children 	  */
	unsigned int	ob_type;	/* type of object			  */
	unsigned int    ob_flags;       /* flags				  */
	unsigned int    ob_state;       /* state				  */
	U_OB_SPEC  ob_spec;	        /* object-specific data			  */
	int	     ob_x;		/* upper left corner of object 		  */
	int	     ob_y;		/* upper left corner of object 		  */
	int	     ob_width; 	/* width of obj				  */
	int	     ob_height;	/* height of obj			  */
} OBJECT;
#endif
