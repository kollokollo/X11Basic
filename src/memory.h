
/* memory.h   (c) Markus Hoffmann*/

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#define MAXANZPOINTERBASE 8


/*Funktionen für die Umwandlung */

#if SIZEOF_VOID_P == 4
  #define POINTER2INT(a) ((unsigned int)(a))
  #define INT2POINTER(a) ((char *)(a))
#elif SIZEOF_VOID_P == 8
extern void *pointerbase[];
extern int pointerbasecnt;
static inline unsigned int catchpointerbase(void *a) {
  unsigned long long pb2=((unsigned long long)a&0xffffffffe0000000ULL);
  unsigned int ret=(unsigned int)((unsigned long long)a&0x1fffffffULL);
  unsigned int i;
  for(i=0;i<pointerbasecnt;i++) {
    if(pointerbase[i]==(void *)pb2) break;
  }
#if DEBUG
  printf("Pointer:     %p\n",a);
  printf("Base:        %p\n",(void *)pb2);
  printf("x:           %x\n",ret);
  printf("idx:         %d\n",i);
#endif
  if(i==pointerbasecnt) {  /* pointerbase is new. */
    if(pointerbasecnt<MAXANZPOINTERBASE) {
      pointerbase[pointerbasecnt++]=(void *)pb2;
    } else printf("WARNING: pointerbasetable is full.\n");
#if DEBUG
    if     (pb2>=0xffff800000000000ULL) printf("Memory is system level memory.\n");
    else if(pb2>=0x00007fff00000000ULL) printf("Memory is stack, vdso or vsyscall.\n");
    else if(pb2< 0x0000000000400000ULL) printf("Memory is illegal.\n");
    if     (pb2< 0x0000800000000000ULL) printf("Memory is user space memory.\n");
    if     (pb2>=0x00007f0000000000ULL) printf("Memory is probably shared mem/so/mmap.\n"); 
#endif
  }
  ret|=(i<<29);
#if DEBUG
  printf("--> x:       %08x\n",ret);
#endif
  return(ret);
}
static inline void *i2p(unsigned int a) {
  unsigned int i=(a&0xe0000000)>>29;
  a&=0x1fffffff;
  return((void *)((unsigned long long)pointerbase[i]+(unsigned long long)a));
}
  #define POINTER2INT(a) catchpointerbase(a)
  #define INT2POINTER(a) i2p(a)
#else
  #error Sizeof pointer unknown.
#endif


static inline void WSWAP(char *adr) {
  char a=*adr;
  *adr=adr[1];
  adr[1]=a;
}
static inline void LSWAP(short *adr) {
  short a=*adr;
  *adr=adr[1];
  adr[1]=a;
}
static inline void LWSWAP(char *p) {
  char a=*p;
  *p=p[3];
  p[3]=a;
  a=p[1];
  p[1]=p[2];
  p[2]=a;
}
