
/* bitmap.c      Bitmap-Routinen (c) Markus Hoffmann    */


/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ======================================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#ifdef HAVE_PNG_H
#include <png.h>
#endif
#ifdef HAVE_JPEGLIB_H
#include <jpeglib.h>
#endif
#include "defs.h"
#include "x11basic.h"
#include "graphics.h"

#ifdef USE_X11
  #include <X11/XWDFile.h>
#endif
#ifdef WINDOWS
  #include <windows.h>
  #include "Windows.extension/fnmatch.h"
#else
  #include <fnmatch.h>
#endif

#include "aes.h"
#include "window.h"
#include "bitmap.h"


void memdump(unsigned char *adr,int l);

/*-------------------------------------------------------------------*/
/*               Routine zum Abspeichern von X-Image                 */
/*             Bitmaps.            (c) Markus hoffmann               */
/*-------------------------------------------------------------------*/


#ifdef USE_X11

/* swap some long ints.  (n is number of BYTES, not number of longs) */
static void swapdws (unsigned char *bp, unsigned int n) {
  register unsigned char c;
  register unsigned char *ep = bp + n;
  register unsigned char *sp;

  while (bp<ep) {
    sp=bp+3;
    c=*sp;*sp=*bp; *bp++=c; sp=bp+1;
    c=*sp;*sp=*bp; *bp++=c; bp+=2;
  }
}

unsigned char *imagetoxwd(XImage *image,Visual *visual,XColor *pixc, int *len) {
    XWDFileHeader *data;
    unsigned long swaptest = 1;
    char image_name[]="X11-BASIC Grafik";
    int header_len=sizeof(XWDFileHeader)+((strlen(image_name)+1+7) & -8);
    int colortable_len,ncolors=0,i;
    XWDColor *color;
    if(image->depth==8) ncolors=256;
    colortable_len=ncolors*sizeof(XWDColor);
    *len=header_len+colortable_len+image->height*image->bytes_per_line;
    data=malloc(header_len+colortable_len+image->height*image->bytes_per_line);
    color=(XWDColor *)((long)data+header_len);
    memcpy((void *)((long)data+header_len+colortable_len),image->data,image->height*image->bytes_per_line);
    memcpy((void *)((long)data+sizeof(XWDFileHeader)),image_name,strlen(image_name));
    data->header_size=(CARD32)header_len;
    data->file_version=(CARD32)XWD_FILE_VERSION;
    data->pixmap_format=(CARD32)image->format;
    data->pixmap_depth=(CARD32)image->depth;
    data->pixmap_width=(CARD32)image->width;
    data->pixmap_height=(CARD32)image->height;
    data->xoffset=(CARD32)image->xoffset;
    data->byte_order=(CARD32)image->byte_order;
    data->bitmap_unit=(CARD32)image->bitmap_unit;
    data->bitmap_bit_order=(CARD32)image->bitmap_bit_order;
    data->bitmap_pad=image->bitmap_pad;
    data->bits_per_pixel=image->bits_per_pixel;
    data->bytes_per_line=image->bytes_per_line;

      data->colormap_entries = ncolors;
      data->ncolors          = ncolors;

    data->visual_class   = (CARD32)visual->class;
    data->red_mask       = (CARD32)visual->red_mask;
    data->green_mask     = (CARD32)visual->green_mask;
    data->blue_mask      = (CARD32)visual->blue_mask;
    data->bits_per_rgb   = (CARD32)visual->bits_per_rgb;

    data->window_width=(CARD32)image->width;
    data->window_height=(CARD32)image->height;
    data->window_x        = 0;
    data->window_y        = 0;
    data->window_bdrwidth = 0;
    if(ncolors){  /* Farbtabelle */
      for(i=0;i<ncolors;i++)  {
        color[i].pixel = (CARD32)i;
        color[i].red   = (CARD16)(pixc+i)->red;
        color[i].green = (CARD16)(pixc+i)->green;
        color[i].blue  = (CARD16)(pixc+i)->blue;
        color[i].flags = (CARD8)(DoRed | DoGreen | DoBlue);
        color[i].pad   = (CARD8)0;
      }
    }
    if (*(char *) &swaptest)    swapdws((unsigned char *)data, sizeof(XWDFileHeader));
    return((unsigned char *)data);
}
#endif

/* this function returns 1 if an alpha mask is present,
   0 on normal success and
   <0 for errors */

int bmp2bitmap(const unsigned char *data,unsigned char *fbp,int x, unsigned int bw,unsigned int bh,unsigned int depth, unsigned char *mask) {
  unsigned short *ptr1  = (unsigned short*)fbp;
  int r=0,g=0,b=0,a,i,j,w,h,offset,d,ib=0,ic=0;
  int usealpha=0;
  unsigned int compression,ncol;
  const unsigned char *buf2,*buf3;
  
  if(data==NULL) return(0);
  ptr1+=x;
  BITMAPFILEHEADER *header=(BITMAPFILEHEADER *)data;
  BITMAPINFOHEADER *iheader=(BITMAPINFOHEADER *)(data+BITMAPFILEHEADERLEN);

  if(header->bfType!=BF_TYPE) {
    printf("Put-Image: Error: wrong bitmap format!\n");
    memdump((unsigned char *)data,64);
    return(-1);
  }
  /* diese komische Akrobatik muss wohl sein (jedenfalls fuer den ARM-linux compiler */
  compression=data[30]| (data[31]<<8) | (data[32]<<16)| (data[33]<<24); 

  if(compression!=BI_RGB && compression!=BI_BITFIELDS) {
    printf("\033[H BITMAPINFOHEADERLEN=%d  \n",BITMAPINFOHEADERLEN);
    printf("&iheader-data      =%d\n",   (int)((long)iheader-(long)data));
    printf("bisize-data        =%d %d\n",(int)((long)(&(iheader->biSize))-       (long)data),iheader->biSize);
    printf("biwidth-data       =%d %d\n",(int)((long)(&(iheader->biWidth))-      (long)data),iheader->biWidth);
    printf("biHeight-data      =%d %d\n",(int)((long)(&(iheader->biHeight))-     (long)data),iheader->biHeight);
    printf("biPlanes-data      =%d %d\n",(int)((long)(&(iheader->biPlanes))-     (long)data),iheader->biPlanes);
    printf("biBitCount-data    =%d %d\n",(int)((long)(&(iheader->biBitCount))-   (long)data),iheader->biBitCount);
    printf("biCompression-data =%d %d\n",(int)((long)(&(iheader->biCompression))-(long)data),iheader->biCompression);
    printf("Put-Image: Compressed Bitmaps (%d) are not supported !\n",iheader->biCompression);
    memdump((unsigned char *)data,64);
    return(-1);
  }
#if DEBUG
  if(compression==BI_BITFIELDS) {
    unsigned long *mmm=(short *)(data+BITMAPFILEHEADERLEN+BITMAPINFOHEADERLEN);
    printf("Warning, BI-BITFIELDS not supported.\n");
    printf("R_MASK=0x%x\n",mmm[0]);
    printf("G_MASK=0x%x\n",mmm[1]);
    printf("B_MASK=0x%x\n",mmm[2]);
    printf("A_MASK=0x%x\n",mmm[3]);
  
  }
#endif
  ncol=iheader->biClrUsed;
  RGBQUAD *coltable=(RGBQUAD *)(data+BITMAPFILEHEADERLEN+BITMAPINFOHEADERLEN);
  
  w=iheader->biWidth;
  h=iheader->biHeight;
 /* d=iheader->biBitCount;  */
  d=data[28]| (data[29]<<8); 

  if(d==0) {
    printf("BITMAP: %d colors d=%d, w=%d, h=%d : something is wrong!\n",ncol,d,w,h);
    return(0);
  }
  offset=(data[10]&0xff)| ((data[11]&0xff)<<8) | 
         ((data[12]&0xff)<<16)| ((data[13]&0xff)<<24); 
// printf("offset=%d\n",offset);
  if(w<=0||h<=0|| x+w>bw||h>bh) return(-2);
  buf3=buf2=data+offset;
  
  if((depth!=24 && depth!=16&& depth!=32)||d==16) {printf("Bitmap-Konvertierung nicht moeglich!\n");}
  
  if(d==32 && mask) usealpha=1; 
  
  for(i=h-1;i>=0;i--) {
    for(j=0;j<w;j++) {
      a=0xff; /*this is the default*/
      if(d==24) {
          b=*buf2++;g=*buf2++;r=*buf2++;
      } else if(d==16) {
          
      } else if(d==8) {
        b=(*buf2++ & 0xff);
	if(b<ncol) {
	  r=coltable[b].rgbRed;g=coltable[b].rgbGreen;b=coltable[b].rgbBlue;
	} else {
	  printf("PIX%02x ",b);
          b=b%ncol;
	  r=coltable[b].rgbRed;g=coltable[b].rgbGreen;b=coltable[b].rgbBlue;
	}
      } else if(d==1) {
        if(ic==0) {
	  ib=*buf2++;
	  ic=8;
	}
	ic--;
	if(ncol>=2) {
	  r=coltable[((ib>>ic)&1)].rgbRed;
	  g=coltable[((ib>>ic)&1)].rgbGreen;
	  b=coltable[((ib>>ic)&1)].rgbBlue;	
	} else r=b=g=((ib>>ic)&1)*255;
      } else if(d==4) {
        if(ic==0) {
	  ib=*buf2++;
	  ic=8;
	}
	ic-=4;
	if(ncol>=16) {
	  r=coltable[((ib>>ic)&0xf)].rgbRed;
	  g=coltable[((ib>>ic)&0xf)].rgbGreen;
	  b=coltable[((ib>>ic)&0xf)].rgbBlue;	
	} else r=b=g=((ib>>ic)&0xf)*16;
      } else if(d==2) {
        if(ic==0) {
	  ib=*buf2++;
	  ic=8;
	}
	ic-=2;
	if(ncol>=4) {
	  r=coltable[((ib>>ic)&0x3)].rgbRed;
	  g=coltable[((ib>>ic)&0x3)].rgbGreen;
	  b=coltable[((ib>>ic)&0x3)].rgbBlue;	
	} else r=b=g=((ib>>ic)&0x3)*64;
      } else if(d==32) {        
        b=*buf2++;g=*buf2++;r=*buf2++;
        a=*buf2++;
      } else {
       // printf("bummer\n");
        r=255*(i&4);b=255*(i&4);g=255*(j&4);
      }
      if(usealpha) mask[j+i*bw]=a;
      if(depth==16) ptr1[j+i*bw]=
        ((((r>>3)&0x1f)<<11)|(((g>>2)&0x3f)<<5)|((b>>3)&0x1f));
      else if(depth==24) {
        ((char *)ptr1)[j*3+0+i*bw*3]=r;
        ((char *)ptr1)[j*3+1+i*bw*3]=g;
        ((char *)ptr1)[j*3+2+i*bw*3]=b;
      } else if(depth==32) {
        ((char *)ptr1)[j*4+0+i*bw*4]=b;
        ((char *)ptr1)[j*4+1+i*bw*4]=g;
        ((char *)ptr1)[j*4+2+i*bw*4]=r;
        ((char *)ptr1)[j*4+3+i*bw*4]=a;
      }
    }    
    buf2=(unsigned char *)(((((long)buf2-(long)buf3)+3)&0xfffffffc)+(long)buf3); /* align to 4 */
  }
  return(usealpha);  
}


void bitmap_scale(const unsigned char *oadr,unsigned int depth,unsigned int ow,unsigned int oh,unsigned char *adr,unsigned int w,unsigned int h) {
  int rl=depth/8;
  int x,y;
  int sx,sy;
  if(depth==1) {
    printf("WARNING: scaling of 1 bit depth bitmaps not yet supported!\n");
  }
  for(y=0;y<h;y++) {
    sy=y*oh/h;
    for(x=0;x<w;x++) {
      sx=x*ow/w;
      adr[y*rl*w+x*rl]=oadr[sy*rl*ow+sx*rl];
      if(depth>=16) adr[y*rl*w+x*rl+1]=oadr[sy*rl*ow+sx*rl+1];
      if(depth>=24) adr[y*rl*w+x*rl+2]=oadr[sy*rl*ow+sx*rl+2];
      if(depth==32) adr[y*rl*w+x*rl+3]=oadr[sy*rl*ow+sx*rl+3];
    }
  }

}
#ifdef USE_X11
static void extend_mask(const unsigned char *a, unsigned char *b, int n, unsigned char thres) {
  int i=0;
  for(i=0;i<n;i++) {
    if(((*a++)&0xff)>=thres) {
      *b++=0xff;
      *b++=0xff;
      *b++=0xff;
      *b++=0xff;
    } else {
      *b++=0;
      *b++=0;
      *b++=0;
      *b++=0;
    }
  }
}
static void extend_mask16(const unsigned char *a, unsigned char *b, int n, unsigned char thres) {
  int i=0;
  for(i=0;i<n;i++) {
    if(((*a++)&0xff)>=thres) {
      *b++=0xff;
      *b++=0xff;
    } else {
      *b++=0;
      *b++=0;
    }
  }
}
XImage *xwdtoximage(unsigned char *data,Visual *visual, int depth, XImage **XMask, int tres, double scale) {
  unsigned char *adr;
  unsigned char *mask;
  unsigned char *mask2;
  int usemask=0;
 // memdump(data,32);
  /* Pruefen, ob es sich um eine BMP Datei handelt. */

  if(*((unsigned short int *)data)==BF_TYPE) {
#if DEBUG 
    printf("BMP-File!\n");
#endif
    int bpl;
    
 //   BITMAPFILEHEADER *header=(BITMAPFILEHEADER *)data;
    BITMAPINFOHEADER *iheader=(BITMAPINFOHEADER *)(data+BITMAPFILEHEADERLEN);
    // int ncol=iheader->biClrUsed;
 //   RGBQUAD *coltable=(RGBQUAD *)(data+BITMAPFILEHEADERLEN+BITMAPINFOHEADERLEN);
    int w=iheader->biWidth;
    int h=iheader->biHeight;
   // int dd=iheader->biBitCount;

    if(iheader->biCompression!=BI_RGB && iheader->biCompression!=BI_BITFIELDS) {
      printf("Put-Image: Compressed Bitmaps are not supported! compression=%d\n",iheader->biCompression);
    }
   // int o=*((int *)(data+10));
    if(depth==24) bpl=w*4; 
    else bpl=w*depth/8; 
#if DEBUG 
    printf("w=%d, h=%d, d=%d, depth=%d bpl=%d\n",w,h,dd,depth,bpl);
#endif


    adr=malloc(h*bpl);
    mask=malloc(h*w);
    if(depth==24) {
      usemask=bmp2bitmap(data,adr,0,w,h,32,mask);
    } else {
      usemask=bmp2bitmap(data,adr,0,w,h,depth,mask);
    }
  //  printf("Usemask=%d\n",usemask);
  
    if(usemask) {
      if(scale!=1.0) {
        unsigned char *omask=mask;
	unsigned char *oadr=adr;
	int oh=h;
	int ow=w;
	h=(int)((double)h*scale);
	w=(int)((double)w*scale);
	
        if(depth==24) bpl=w*4; 
        else bpl=w*depth/8; 
	mask=malloc(h*w);
	adr=malloc(h*bpl);
        bitmap_scale(omask,8,ow,oh,mask,w,h);
        bitmap_scale(oadr,bpl/w*8,ow,oh,adr,w,h);
        free(omask);
	free(oadr);

      }
      mask2=malloc(h*bpl);
    //  memdump(mask,32);
      if(depth==24 || depth==32) extend_mask(mask,mask2,h*w,32);
      else extend_mask16(mask,mask2,h*w,32);
      free(mask);
      *XMask=XCreateImage(window[usewindow].display,visual,depth,ZPixmap,0,(char *)mask2,w,h,8,bpl);
    } else {
       free(mask);
      if(XMask) *XMask=NULL;
      if(scale!=1.0) {
 	int oh=h;
	int ow=w;
	unsigned char *oadr=adr;
	h=(int)((double)h*scale);
	w=(int)((double)w*scale);
        if(depth==24) bpl=w*4; 
        else bpl=w*depth/8; 
	adr=malloc(h*bpl);
        bitmap_scale(oadr,bpl/w*8,ow,oh,adr,w,h);
	free(oadr);
      }
    }
    return(XCreateImage(window[usewindow].display,visual,depth,ZPixmap,0,(char *)adr,w,h,8,bpl));
  } else {
    unsigned long swaptest = 1;
   if(XMask) *XMask=NULL;
    if (*(char *) &swaptest)    swapdws(data, sizeof(XWDFileHeader));
#if DEBUG
    printf("xwd-Name: %s\n",data+sizeof(XWDFileHeader));
    printf("Version: %d\n",((XWDFileHeader *)data)->file_version);
    printf("pixmap_format: %d\n",((XWDFileHeader *)data)->pixmap_format);
    printf("pixmap_depth: %d\n",((XWDFileHeader *)data)->pixmap_depth);
    printf("ncolors: %d\n",((XWDFileHeader *)data)->ncolors);
    printf("xoffset: %d\n",((XWDFileHeader *)data)->xoffset);
    printf("pixmap_width: %d\n",((XWDFileHeader *)data)->pixmap_width);
    printf("pixmap_height: %d\n",((XWDFileHeader *)data)->pixmap_height);
    printf("bitmap_pad: %d\n",((XWDFileHeader *)data)->bitmap_pad);
    printf("bytes_per_line: %d\n",((XWDFileHeader *)data)->bytes_per_line);
#endif
    if(((XWDFileHeader *)data)->file_version!=(CARD32)XWD_FILE_VERSION) {
      printf("Achtung: Falsche XWD Version: %d\n",(int)((XWDFileHeader *)data)->file_version);
      adr=malloc(32*32*depth/8);
      return(XCreateImage(window[usewindow].display,visual,depth,ZPixmap,0,(char *)adr,32,32,8,32*depth/8));  
    } else {
      adr=malloc(((XWDFileHeader *)data)->pixmap_height*((XWDFileHeader *)data)->bytes_per_line);
      memcpy(adr,data+((XWDFileHeader *)data)->header_size+((XWDFileHeader *)data)->ncolors*sizeof(XWDColor),((XWDFileHeader *)data)->pixmap_height*((XWDFileHeader *)data)->bytes_per_line);
    }
    return(XCreateImage(window[usewindow].display,visual,
    ((XWDFileHeader *)data)->pixmap_depth,
    ((XWDFileHeader *)data)->pixmap_format,
    ((XWDFileHeader *)data)->xoffset,
    (char *)adr,
    ((XWDFileHeader *)data)->pixmap_width,
    ((XWDFileHeader *)data)->pixmap_height,
    ((XWDFileHeader *)data)->bitmap_pad,
    ((XWDFileHeader *)data)->bytes_per_line));
  }
}
#endif
#if 0
SDL_Surface *bpmtosurface(const char *data, double scale) {
  SDL_Surface *ret;
  /* Pruefen, ob es sich um eine BMP Datei handelt. */
  if(*((unsigned short int *)data)==BF_TYPE) {
    int dd,w,o,h,bpl;
    char *adr;
    
    BITMAPFILEHEADER *header=(BITMAPFILEHEADER *)data;
    BITMAPINFOHEADER *iheader=(BITMAPINFOHEADER *)(data+BITMAPFILEHEADERLEN);
   // ncol=iheader->biClrUsed;
    RGBQUAD *coltable=(RGBQUAD *)(data+BITMAPFILEHEADERLEN+BITMAPINFOHEADERLEN);
    w=iheader->biWidth;
    h=iheader->biHeight;
    dd=iheader->biBitCount;

    if(iheader->biCompression!=BI_RGB && iheader->biCompression!=BI_BITFIELDS) {
      printf("Put-Image: Compressed Bitmaps are not supported! compression=%d\n",iheader->biCompression);
    }
    o=*((int *)(data+10));
    bpl=w*dd/8; 
#if DEBUG
    printf("w=%d, h=%d, d=%d, depth=%d bpl=%d\n",w,h,dd,0,bpl);
#endif
    adr=malloc(h*bpl);
    bmp2bitmap(data,adr,0,w,h,dd,NULL);
    
  //  printf("Usemask=%d\n",usemask);
   ret=SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA,w,h,dd, 0,0,0,0);

      if(scale!=1.0) {
 	int oh=h;
	int ow=w;
	h=(int)((double)h*scale);
	w=(int)((double)w*scale);
        bitmap_scale(adr,dd,ow,oh,ret->pixels,w,h);
	free(adr);
      } else {
//        memcpy(ret->pixels,adr,h*bpl); free(adr);
      free(ret->pixels);
      ret->pixels=adr;
      }
      
  } else {
    printf("ERROR: PUT, wrong file format.\n");
    ret=SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA, 16, 16, 32, 0,0,0,0);

  }
  return(ret);
}
#endif
#ifdef USE_SDL

/*
* Return the pixel value at (x, y)
* NOTE: The surface must be locked before calling this!
*/
Uint32 sdl_getpixel(SDL_Surface *surface, int x, int y) {
  int bpp = surface->format->BytesPerPixel;
  /* Here p is the address to the pixel we want to retrieve */
  Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
  switch(bpp) {
  case 1:
    return *p;
  case 2:
    return *(Uint16 *)p;
  case 3:
  if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
    return (p[0] << 16 | p[1] << 8 | p[2]);
  else
    return (p[0] | p[1] << 8 | p[2] << 16);
  case 4:
    return (*(Uint32 *)p)<<8|0xff;
  default:
    return 0; /* shouldn\u2019t happen, but avoids warnings */
  }
}

#endif
