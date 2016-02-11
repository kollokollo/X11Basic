/* bitmap.h  (c) Markus Hoffmann*/

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#ifndef WINDOWS
/* Wegen alignment (auf 4 !) */

typedef struct {                     /**** BMP file header structure ****/
    unsigned short bfType;           /*0 Magic number for file */
    char        bfSize;                /*2 Size of file */
    char a; char aa; char aaa;
    unsigned short bfReserved1;      /*6 Reserved */
    unsigned short bfReserved2;      /*8 ... */
    char     bfOffBits;              /*10 Offset to bitmap data */
    char b;
    char c;
    char d;
} BITMAPFILEHEADER;
#endif

#define BF_TYPE 0x4D42             /* "MB" */
#define BITMAPFILEHEADERLEN 14

#ifndef WINDOWS
typedef struct   {                   /**** BMP file info structure ****/
    unsigned int   biSize;           /*14 Size of info header */
    int            biWidth;          /*18 Width of image */
    int            biHeight;         /*22 Height of image */
    unsigned short biPlanes;         /*26 Number of color planes */
    unsigned short biBitCount;       /*28 Number of bits per pixel */
    unsigned int   biCompression;    /*30 Type of compression to use */
    unsigned int   biSizeImage;      /*34 Size of image data */
    int            biXPelsPerMeter;  /*38 X pixels per meter */
    int            biYPelsPerMeter;  /*42 Y pixels per meter */
    unsigned int   biClrUsed;        /*46 Number of colors used */
    unsigned int   biClrImportant;   /*50 Number of important colors */
} BITMAPINFOHEADER;
#endif
#define BITMAPINFOHEADERLEN sizeof(BITMAPINFOHEADER)

/*
 * Constants for the biCompression field...
 */

#  define BI_RGB       0             /* No compression - straight BGR data */
#  define BI_RLE8      1             /* 8-bit run-length compression */
#  define BI_RLE4      2             /* 4-bit run-length compression */
#  define BI_BITFIELDS 3             /* RGB bitmap with RGB masks */

#ifndef WINDOWS
typedef struct  {                    /**** Colormap entry structure ****/
    unsigned char  rgbBlue;          /* Blue value */
    unsigned char  rgbGreen;         /* Green value */
    unsigned char  rgbRed;           /* Red value */
    unsigned char  rgbReserved;      /* Reserved */
} RGBQUAD;
#endif

int bmp2bitmap(const unsigned char *data,unsigned char *fbp,int x, unsigned int bw,unsigned int bh,unsigned int depth, unsigned char *mask);

#ifdef USE_X11
XImage *xwdtoximage(unsigned char *data,Visual *visual, int depth, XImage **XMask, int tres, double scale);
unsigned char *imagetoxwd(XImage *image,Visual *visual,XColor *pixc, int *len);
#endif
void bitmap_scale(const unsigned char *oadr,unsigned int depth,unsigned int ow,unsigned int oh,unsigned char *adr,unsigned int w,unsigned int h);
