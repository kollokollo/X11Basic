/* aes.h  (c) Markus Hoffmann*/

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
#ifndef __aes__
#define __aes__

//#define GEMFONT      "-*-fixed-*-r-normal-*-15-*-ISO10646-1"
#define GEMFONT      "*8x16*"
//#define GEMFONTSMALL "-*-fixed-*-r-normal-*-8-*-ISO10646-1"
#define GEMFONTSMALL "*5x8*"

#define WORD short
#define LONG unsigned long


#ifdef USE_GEM
#include <gem.h>
#endif
/* Object Drawing Types */
						/* Graphic types of obs	*/
#define G_BOX 20
#define G_TEXT 21
#define G_BOXTEXT 22
#define G_IMAGE 23
#define G_USERDEF 24
#define G_IBOX 25
#define G_BUTTON 26
#define G_BOXCHAR 27
#define G_STRING 28
#define G_FTEXT 29
#define G_FBOXTEXT 30
#define G_ICON 31
#define G_TITLE 32
#define G_ALERTTYP 42
						/* Object flags		 */
#define NONE 0x0
#define SELECTABLE 0x1
#define DEFAULT 0x2
#define EXIT 0x4
#define EDITABLE 0x8
#define RBUTTON 0x10
#define LASTOB 0x20
#define TOUCHEXIT 0x40
#define HIDETREE 0x80
#define INDIRECT 0x100
						/* Object states	*/
#define NORMAL 0x0
#define SELECTED 0x1
#define CROSSED 0x2
#define CHECKED 0x4
#define DISABLED 0x8
#define OUTLINED 0x10
#define SHADOWED 0x20
#define WHITEBAK 0x40
#define DRAW3D 0x80


  #undef BLACK
  #undef WHITE
  #undef RED
  #undef GREEN
  #undef BLUE     
  #undef YELLOW   
  #undef MAGENTA  
						/* Object colors	*/
#define WHITE 0
#define BLACK 1
#define RED 2
#define GREEN 3
#define BLUE 4
#define CYAN 5
#define YELLOW 6
#define MAGENTA 7
#define LWHITE 8
#define LBLACK 9
#define LRED 10
#define LGREEN 11
#define LBLUE 12
#define LCYAN 13
#define LYELLOW 14
#define LMAGENTA 15
						/* bit blt rules	*/
#define ALL_WHITE 0
#define S_AND_D 1
#define S_ONLY 3
#define NOTS_AND_D 4
#define S_XOR_D 6
#define S_OR_D 7
#define D_INVERT 10
#define NOTS_OR_D 13
#define ALL_BLACK 15
						/* font types		*/
#define FONT_IBM 3
#define FONT_SMALLIBM 5
#define FONT_BIGIBM 6
#define FONT_SMALL 40
#define FONT_BIG 10
#define FONT_LARGE 20
#define FONT_DEFAULT FONT_IBM


#ifndef USE_GEM
#define EDSTART 0
#define EDINIT 1
#define EDCHAR 2
#define EDEND 3
#endif

#define TE_LEFT 0
#define TE_RIGHT 1
#define TE_CNTR 2


#define R_TREE 0
#define R_OBJECT 1
#define R_TEDINFO 2
#define R_ICONBLK 3
#define R_BITBLK 4
#define R_STRING 5
#define R_IMAGEDATA 6
#define R_OBSPEC 7
#define R_TEPTEXT 8		/* sub ptrs in TEDINFO	*/
#define R_TEPTMPLT 9
#define R_TEPVALID 10
#define R_IBPMASK 11		/* sub ptrs in ICONBLK	*/
#define R_IBPDATA 12
#define R_IBPTEXT 13
#define R_BIPDATA 14		/* sub ptrs in BITBLK	*/
#define R_FRSTR 15		/* gets addr of ptr to free strings	*/
#define R_FRIMG 16		/* gets addr of ptr to free images	*/


#define RS_SIZE 17				/* NUM_RTYPES + NUM_RN	*/

#define HDR_LENGTH (RS_SIZE + 1) * 2		/* in bytes	*/



#ifdef USE_GEM
/* AES-Definitionen   */
typedef struct {
  short	x;
  short	y;
  unsigned short w;
  unsigned short h;
} ARECT;
#else
/* AES-Definitionen   */
typedef struct {
  int	x;
  int	y;
  unsigned int w;
  unsigned int h;
} ARECT;
#endif



#ifndef USE_GEM
#define OBJECT struct object
#define TEDINFO struct text_edinfo
#define GRECT struct grect
#define ORECT	struct orect
#define ICONBLK struct icon_block
#define BITBLK struct bit_block
#define USERBLK struct user_blk
#define PARMBLK struct parm_blk

typedef union obspecptr
{
	long		index;		
	union obspecptr	*indirect;	
	// BFOBSPEC 	obspec;		
	TEDINFO		*tedinfo;	
	BITBLK		*bitblk;	
	ICONBLK		*iconblk;	
	// CICONBLK 	*ciconblk;	
	struct user_block *userblk;	
	char		*free_string;	
} OBSPEC;

OBJECT
{
	WORD		ob_next;	/* -> object's next sibling	*/
	WORD		ob_head;	/* -> head of object's children */
	WORD		ob_tail;	/* -> tail of object's children */
	unsigned WORD		ob_type;	/* type of object- BOX, CHAR,...*/
	unsigned WORD		ob_flags;	/* flags			*/
	unsigned WORD		ob_state;	/* state- SELECTED, OPEN, ...	*/
	OBSPEC		ob_spec;	/* "out"- -> anything else	*/
	WORD		ob_x;		/* upper left corner of object	*/
	WORD		ob_y;		/* upper left corner of object	*/
	WORD		ob_width;	/* width of obj			*/
	WORD		ob_height;	/* height of obj		*/
};


ORECT
{
	ORECT	*o_link;
	WORD	o_x;
	WORD	o_y;
	WORD	o_w;
	WORD	o_h;
} ;



GRECT
{
	WORD	g_x;
	WORD	g_y;
	WORD	g_w;
	WORD	g_h;
} ;





TEDINFO
{
	char		*te_ptext;	/* ptr to text (must be 1st)	*/
	char		*te_ptmplt;	/* ptr to template		*/
	char		*te_pvalid;	/* ptr to validation chrs.	*/
	WORD		te_font;	/* font				*/
	WORD		te_fontid;	/* junk word 1			*/
	WORD		te_just;	/* justification- left, right...*/
	WORD	        te_color;	/* color information word	*/
	WORD		te_fontsize;	/* junk word 2			*/
	WORD		te_thickness;	/* border thickness		*/
	WORD		te_txtlen;	/* length of text string	*/
	WORD		te_tmplen;	/* length of template string	*/
};



ICONBLK
{
	WORD	ib_pmask;
	WORD dummy1;
	WORD	ib_pdata;
	WORD dummy2;
	WORD	ib_ptext;
	WORD dummy3;
	WORD	ib_char;
	WORD	ib_xchar;
	WORD	ib_ychar;
	WORD	ib_xicon;
	WORD	ib_yicon;
	WORD	ib_wicon;
	WORD	ib_hicon;
	WORD	ib_xtext;
	WORD	ib_ytext;
	WORD	ib_wtext;
	WORD	ib_htext;
};


BITBLK
{
	WORD	bi_pdata;		/* ptr to bit forms data	*/
        WORD dummy;         /* LINUX-Spezifisch ! */
	WORD	bi_wb;			/* width of form in bytes	*/
	WORD	bi_hl;			/* height in lines		*/
	WORD	bi_x;			/* source x in bit form		*/
	WORD	bi_y;			/* source y in bit form		*/
	WORD	bi_color;		/* fg color of blt 		*/

};

USERBLK
{
	LONG	ub_code;
	LONG	ub_parm;
};

PARMBLK
{
	LONG	pb_tree;
	WORD	pb_obj;
	WORD	pb_prevstate;
	WORD	pb_currstate;
	WORD	pb_x, pb_y, pb_w, pb_h;
	WORD	pb_xc, pb_yc, pb_wc, pb_hc;
	LONG	pb_parm;
};


typedef struct objc_colorword {
   unsigned borderc : 4;
   unsigned textc   : 4;
   unsigned opaque  : 1;
   unsigned pattern : 3;
   unsigned fillc   : 4;
} OBJC_COLORWORD;
#endif

typedef struct {
    unsigned character   :  8;
    signed   framesize   :  8;
    unsigned framecol    :  4;
    unsigned textcol     :  4;
    unsigned textmode    :  1;
    unsigned fillpattern :  3;
    unsigned interiorcol :  4;
} bfobspec;

#ifndef USE_GEM

typedef struct rshdr
{
	WORD		rsh_vrsn;	/* must same order as RT_	*/
	WORD		rsh_object;
	WORD		rsh_tedinfo;
	WORD		rsh_iconblk;	/* list of ICONBLKS		*/
	WORD		rsh_bitblk;
	WORD		rsh_frstr;
	WORD		rsh_string;
	WORD		rsh_imdata;	/* image data			*/
	WORD		rsh_frimg;
	WORD		rsh_trindex;
	WORD		rsh_nobs;	/* counts of various structs	*/
	WORD		rsh_ntree;
	WORD		rsh_nted;
	WORD		rsh_nib;
	WORD		rsh_nbb;
	WORD		rsh_nstring;
	WORD		rsh_nimages;
	WORD		rsh_rssize;	/* total bytes in resource	*/
}RSHDR;
#endif
typedef struct rshdrv3
{
	WORD		rsh_vrsn;	/* must same order as RT_	*/
	LONG		rsh_object;
	LONG		rsh_tedinfo;
	LONG		rsh_iconblk;	/* list of ICONBLKS		*/
	LONG		rsh_bitblk;
	LONG		rsh_frstr;
	LONG		rsh_string;
	LONG		rsh_imdata;	/* image data			*/
	LONG		rsh_frimg;
	LONG		rsh_trindex;
	WORD		rsh_nobs;	/* counts of various structs	*/
	WORD		rsh_ntree;
	WORD		rsh_nted;
	WORD		rsh_nib;
	WORD		rsh_nbb;
	WORD		rsh_nstring;
	WORD		rsh_nimages;
	LONG		rsh_rssize;	/* total bytes in resource	*/
}RSHDRV3;

typedef struct { unsigned char r,g,b;} AESRGBCOLOR;


/* Prototypes */

extern RSHDR *rsrc;
extern int gem_colors[];


void gem_init();
void load_GEMFONT(int n);
#ifndef USE_GEM
short form_alert(short dbut,char *n);
short form_center(OBJECT *tree,short *x,short *y,short *w,short *h);
short rsrc_free();
void objc_add(OBJECT *tree,int p,int c);
void objc_delete(OBJECT *tree,int object);
short rsrc_load(const char *filename);
short objc_offset(OBJECT *tree,short object,short *x,short *y);
short objc_find(OBJECT *tree,short startob, short depth,short x,short y);
short objc_draw(OBJECT *tree,short startob, short depth,short x,short y, short w, short h);
short rsrc_gaddr(short re_gtype, unsigned short re_gindex, char **re_gaddr);
int finded(OBJECT *tree,int start, int r);
void draw_edcursor(OBJECT *tree,int ndx);
int rootob(OBJECT *tree,int onr);
void relobxy(OBJECT *tree,int ndx,int *x, int *y);
#endif
int form_alert2(int dbut,char *n,int size, char *tval);
#endif
