
/* bitmap.c      Bitmap-Routinen (c) Markus Hoffmann    */


/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ======================================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
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
#endif

#include "aes.h"
#include "window.h"
#include "bitmap.h"


void memdump(unsigned char *adr,int l);

/*-------------------------------------------------------------------*/
/*               Routine zum Abspeichern von X-Image                 */
/*             Bitmaps.            (c) Markus hoffmann               */
/*-------------------------------------------------------------------*/

/* We need these because ARM has 32 Bit alignment (and the compiler has a bug)*/
static void writeint(unsigned char *p,unsigned int n) {
  *p++=(unsigned char)(n&0xff);
  *p++=(unsigned char)((n&0xff00)>>8);
  *p++=(unsigned char)((n&0xff0000)>>16);
  *p++=(unsigned char)((n&0xff000000)>>24);
}
static void writeshort(unsigned char *p,unsigned short n) {
  *p++=(unsigned char)(n&0xff);
  *p++=(unsigned char)((n&0xff00)>>8);
}

#ifdef USE_X11
STANDARDBITMAP imagetostdbm(XImage *image,Visual *visual,XColor *pixc,int usealpha, int bcolor) {
  STANDARDBITMAP ret;
  ret.w=(unsigned int)image->width;
  ret.h=(unsigned int)image->height;
  ret.image=malloc(ret.w*ret.h*4);
  int i,j,p;
  unsigned char r=0,g=0,b=0,a;
  unsigned char *buf=ret.image;
  for(i=0;i<image->height;i++) {
    for(j=0;j<image->width;j++) {
      p=XGetPixel(image, j, i);
      if(usealpha && bcolor==p) {r=b=g=a=0;}
      else {
        a=255;
        if(image->depth==8) {
          r=(CARD16)(pixc+p)->red;
	  g=(CARD16)(pixc+p)->green;
	  b=(CARD16)(pixc+p)->blue;
        } else if(image->depth==16) {
	  // TODO
	} else {
          b=p&0xff;
          g=(p&0xff00)>>8;
          r=(p&0xff0000)>>16;
	}
      }
      *buf++=r;
      *buf++=g;
      *buf++=b;
      *buf++=a;
    }    
  }
  return(ret);
}
#endif

unsigned char *stdbmtobmp(STANDARDBITMAP bmp, int *len) {
  int l=BITMAPFILEHEADERLEN+BITMAPINFOHEADERLEN+bmp.h*bmp.w*4;
  unsigned char *data=malloc(l);
  BITMAPFILEHEADER *header=(BITMAPFILEHEADER *)data;
  BITMAPINFOHEADER *iheader=(BITMAPINFOHEADER *)(data+BITMAPFILEHEADERLEN);
  header->bfType=BF_TYPE;
  writeint((unsigned char *)&data[10],BITMAPFILEHEADERLEN+BITMAPINFOHEADERLEN);
  writeint((unsigned char *)&(iheader->biSize),BITMAPINFOHEADERLEN);
  writeint((unsigned char *)&(iheader->biWidth),bmp.w);
  writeint((unsigned char *)&(iheader->biHeight),bmp.h);
  writeshort((unsigned char *)&(iheader->biPlanes),1);
  writeshort((unsigned char *)&(iheader->biBitCount),32);
  writeint((unsigned char *)&(iheader->biCompression),BI_RGB);
  writeint((unsigned char *)&(iheader->biSizeImage),0);
  writeint((unsigned char *)&(iheader->biXPelsPerMeter),0);
  writeint((unsigned char *)&(iheader->biYPelsPerMeter),0);
  writeint((unsigned char *)&(iheader->biClrUsed),0);
  writeint((unsigned char *)&(iheader->biClrImportant),0);
  unsigned char *buf2=data+BITMAPFILEHEADERLEN+BITMAPINFOHEADERLEN;
  unsigned char *buf3=buf2;
  unsigned char *scanline;
  int i,j;
  for(i=bmp.h-1;i>=0;i--) {
    scanline=(unsigned char *)bmp.image+i*bmp.w*4;
    for(j=0;j<bmp.w;j++) {
      *buf2++=scanline[2];  /*  B  */
      *buf2++=scanline[1];  /*  G  */
      *buf2++=scanline[0];  /*  R  */
      *buf2++=scanline[3];  /*  A  */
      scanline+=4;
    }    
    buf2=buf3+(((buf2-buf3)+3)&0xfffffffc); /* align to 4 */
  }
  l=(buf2-buf3)+BITMAPFILEHEADERLEN+BITMAPINFOHEADERLEN;
  writeint(&data[2],l);
  if(len) *len=l;
  return(data);
}


STANDARDBITMAP bmp2stdbm(const unsigned char *data) {
  STANDARDBITMAP ret;
  BITMAPFILEHEADER *header=(BITMAPFILEHEADER *)data;
  BITMAPINFOHEADER *iheader=(BITMAPINFOHEADER *)(data+BITMAPFILEHEADERLEN);
  if(data==NULL || header->bfType!=BF_TYPE) {
    printf("Put-Image: Error: wrong bitmap format!\n");
    ret.w=0;
    ret.h=0;
    ret.image=malloc(4);
    return(ret);
  }
  /* diese komische Akrobatik muss wohl sein (jedenfalls fuer den ARM-linux compiler */
  unsigned int compression=data[30]| (data[31]<<8) | (data[32]<<16)| (data[33]<<24);
  if(compression!=BI_RGB && compression!=BI_BITFIELDS) {
#if DEBUG
    printf("\033[H BITMAPINFOHEADERLEN=%d  \n",BITMAPINFOHEADERLEN);
    printf("&iheader-data      =%d\n",   (int)((long)iheader-(long)data));
    printf("bisize-data        =%d %d\n",(int)((long)(&(iheader->biSize))-       (long)data),iheader->biSize);
    printf("biwidth-data       =%d %d\n",(int)((long)(&(iheader->biWidth))-      (long)data),iheader->biWidth);
    printf("biHeight-data      =%d %d\n",(int)((long)(&(iheader->biHeight))-     (long)data),iheader->biHeight);
    printf("biPlanes-data      =%d %d\n",(int)((long)(&(iheader->biPlanes))-     (long)data),iheader->biPlanes);
    printf("biBitCount-data    =%d %d\n",(int)((long)(&(iheader->biBitCount))-   (long)data),iheader->biBitCount);
    printf("biCompression-data =%d %d\n",(int)((long)(&(iheader->biCompression))-(long)data),iheader->biCompression);
#endif
    printf("Put-Image: Compressed Bitmaps (%d) are not supported !\n",iheader->biCompression);
#if DEBUG
    memdump((unsigned char *)data,64);
#endif
    ret.w=0;
    ret.h=0;
    ret.image=malloc(4);
    return(ret);
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
  unsigned int ncol=iheader->biClrUsed;
  int w=iheader->biWidth;
  int h=iheader->biHeight;
  /* d=iheader->biBitCount;  */
  int d=data[28]| (data[29]<<8); 
  if(d==0 || w<=0 || h<=0) {
    printf("BITMAP: %d colors d=%d, w=%d, h=%d : something is wrong!\n",ncol,d,w,h);
    ret.w=0;
    ret.h=0;
    ret.image=malloc(4);
    return(ret);
  }
  if(d==16) {printf("ERROR: Bitmap depth not supported!\n");}
  int offset=(data[10]&0xff)| ((data[11]&0xff)<<8) | ((data[12]&0xff)<<16)| ((data[13]&0xff)<<24); 
// printf("offset=%d\n",offset);
  const unsigned char *buf2,*buf3;
  buf3=buf2=data+offset;
  RGBQUAD *coltable=(RGBQUAD *)(data+BITMAPFILEHEADERLEN+BITMAPINFOHEADERLEN);
  ret.w=w;
  ret.h=h;
  unsigned char *ptr1=ret.image=malloc(4*w*h);
  
  int r=0,g=0,b=0,a=0xff,i,j,ib=0,ic=0;
  for(i=h-1;i>=0;i--) {
    for(j=0;j<w;j++) {
      if(d==24) {
        b=*buf2++;g=*buf2++;r=*buf2++;
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

      ((char *)ptr1)[j*4+0+i*w*4]=r;
      ((char *)ptr1)[j*4+1+i*w*4]=g;
      ((char *)ptr1)[j*4+2+i*w*4]=b;
      ((char *)ptr1)[j*4+3+i*w*4]=a;
    }    
    buf2=(unsigned char *)(((((long)buf2-(long)buf3)+3)&0xfffffffc)+(long)buf3); /* align to 4 */
  }
  return(ret);  
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

XImage *stdbmtoximage(STANDARDBITMAP bmp,Visual *visual, int depth, XImage **XMask) {
  int bpl;
  if(depth==24) bpl=bmp.w*4; 
  else bpl=bmp.w*depth/8; 

  /* Extrahiere aus dem Alpha-Channel eine Maske*/
  unsigned char *adr=malloc(bmp.h*bpl);
  unsigned char *mask=malloc(bmp.h*bmp.w);
  int i,usemask=0;
  for(i=0;i<bmp.h*bmp.w;i++) mask[i]=bmp.image[i*4+3];
  
  for(i=0;i<bmp.h*bmp.w;i++) {
    if(mask[i]!=0xff) {
      usemask=1;
      break;
    }
  }
  if(usemask) {
    unsigned char *mask2=malloc(bmp.h*bpl);
    if(depth==24 || depth==32) extend_mask(mask,mask2,bmp.h*bmp.w,32);
    else extend_mask16(mask,mask2,bmp.h*bmp.w,32);
    if(XMask) *XMask=XCreateImage(window[usewindow].display,visual,depth,ZPixmap,0,(char *)mask2,bmp.w,bmp.h,8,bpl);
    else free(mask2);
  } else {
    if(XMask) *XMask=NULL;
  }
  free(mask);
  for(i=0;i<bmp.h*bmp.w;i++) {
    adr[i*4+2]=bmp.image[i*4];
    adr[i*4+1]=bmp.image[i*4+1];
    adr[i*4+0]=bmp.image[i*4+2];
  }
  return(XCreateImage(window[usewindow].display,visual,depth,ZPixmap,0,(char *)adr,bmp.w,bmp.h,8,bpl));
}
#endif

#include "lodepng.h"

STRING pngtobmp(unsigned char *pngdata,size_t pngsize) {
  STANDARDBITMAP stdbmp;
  STRING ergebnis;
  unsigned int error = lodepng_decode32(&stdbmp.image, &stdbmp.w, &stdbmp.h,pngdata,pngsize);
  if(error) printf("PNGDECODE: error %u\n", error);
  ergebnis.pointer=(char *)stdbmtobmp(stdbmp,(int *)&ergebnis.len);
  free(stdbmp.image);
  return(ergebnis);
}

STRING bmptopng(unsigned char *data) {
  STRING ergebnis;
  STANDARDBITMAP bm;
  unsigned char* png;
  size_t pngsize;
  /* Pruefen, ob es sich um eine BMP Datei handelt. */
  if(*((unsigned short int *)data)==BF_TYPE) {
    bm=bmp2stdbm(data);
  } else {
    printf("ERROR: PNGENCODE, wrong data format.\n");
    bm.w=0;
    bm.h=0;
    bm.image=malloc(16);
  }
  unsigned error = lodepng_encode32(&png, &pngsize, bm.image, bm.w, bm.h);
  if(error) printf("ERROR encoding bitmap to PNG.\n");
  free(bm.image);
  ergebnis.pointer=(char *)png;
  ergebnis.len=(int)pngsize;
  return(ergebnis);
}

#ifdef USE_SDL

/** Return the pixel value at (x, y)
* NOTE: The surface must be locked before calling this!*/

Uint32 sdl_getpixel(SDL_Surface *surface, int x, int y) {
  int bpp = surface->format->BytesPerPixel;
  /* Here p is the address to the pixel we want to retrieve */
  Uint8 *p=(Uint8 *)surface->pixels+y*surface->pitch+x*bpp;
  switch(bpp) {
  case 1:   return *p;
  case 2:   return *(Uint16 *)p;
  case 3:
    if(SDL_BYTEORDER==SDL_BIG_ENDIAN)  return(p[0]<<16|p[1]<<8|p[2]);
    else                               return(p[0]|p[1]<<8|p[2]<<16);
  case 4:   return (*(Uint32 *)p)<<8|0xff;
  default:  return 0; /* shouldn't happen, but avoids warnings */
  }
}
#endif
