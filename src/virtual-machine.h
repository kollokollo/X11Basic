/* virtual-machine.h (c) Markus Hoffmann  */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

extern char *rodata;

PARAMETER *virtual_machine(const STRING bcpc, int o, int *np, const PARAMETER *plist, int inp);



#ifdef ATARI
#define CP4(d,s,l) { register char *dd=(char *)(d); \
                   register char *ss=(char *)(s)+3; \
                 *dd++=*ss--; \
                 *dd++=*ss--; \
                 *dd++=*ss--; \
                 *dd=*ss; l+=4;}
#define CP2(d,s,l) { register char *dd_=(char *)(d); \
                   register char *ss_=(char *)(s)+1; \
                 *dd_++=*ss_--; \
                 *dd_=*ss_; l+=2;}

#define CP8(d,s,l) { register char *dd=(char *)(d); \
                   register char *ss=(char *)(s)+7; \
                 *dd++=*ss--; \
                 *dd++=*ss--; \
                 *dd++=*ss--; \
                 *dd++=*ss--; \
                 *dd++=*ss--; \
                 *dd++=*ss--; \
                 *dd++=*ss--; \
                 *dd=*ss; l+=8;}

#else
#define CP4(d,s,l) { register char *dd=(char *)(d); \
                   register char *ss=(char *)(s); \
                 *dd++=*ss++; \
                 *dd++=*ss++; \
                 *dd++=*ss++; \
                 *dd=*ss; l+=4;}
#define CP2(d,s,l) { register char *dd_=(char *)(d); \
                   register char *ss_=(char *)(s); \
                 *dd_++=*ss_++; \
                 *dd_=*ss_; l+=2;}
#define CP8(d,s,l) { register char *dd=(char *)(d); \
                   register char *ss=(char *)(s); \
                 *dd++=*ss++; \
                 *dd++=*ss++; \
                 *dd++=*ss++; \
                 *dd++=*ss++; \
                 *dd++=*ss++; \
                 *dd++=*ss++; \
                 *dd++=*ss++; \
                 *dd=*ss; l+=8;}
#endif

