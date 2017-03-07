/* io_basic.c I/O_Routinen  (c) Markus Hoffmann  */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
 
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "defs.h"
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
#include "x11basic.h"




/* This is a handy helper function which prints out a 
   hex dump of the memory arey pointed to by adr of length l
   The output is in magenta and fints on a 80 char screen.
   (c) by Markus Hoffmann 1997 */

void memdump(const unsigned char *adr,int l) {
  int i;
  printf("\033[35m");
  while(l>16) {
    printf("%p: ",(void *)adr);	
    for(i=0;i<16;i++) printf("%02x ",adr[i]);
    printf(" ");
    for(i=0;i<16;i++) {
      if(adr[i]>31) printf("%c",adr[i]);
      else printf(".");
    }
    printf("\n");
    adr+=16;
    l-=16;
  }
  if(l>0) {
    printf("%p: ",(void *)adr);
    for(i=0;i<16;i++) {
      if(i<l) printf("%02x ",adr[i]);
      else printf("   ");
    }
    printf(" ");
    for(i=0;i<l;i++) {
      if(adr[i]>31) printf("%c",adr[i]);
      else printf(".");
    }
    printf("\n");
  }
  printf("\033[m");
#ifdef ANDROID
  invalidate_screen();
#endif
}

static const struct {int sf; char xf; } ioemaptable[] = {
    { 0,   7 }, /* 0: No error status currently */
    { EPERM,   -51 }, /* 1: Not super-user */
    { ENOENT,  -33 }, /* 2: No such file or directory*/
    { ESRCH,    -3 }, /* 3: No such process*/
    { EINTR,   -53 }, /* 4: Interrupted system call*/
    { EIO,      -1 }, /* 5: Allgemeiner IO-Fehler */
    { ENXIO,   -36 }, /* 6: No such device or address */
    { E2BIG,    -7 }, /* 7: Arg list too long */
    { ENOEXEC, -66 }, /* 8: Exec format error */
    { EBADF,   -37 }, /* 9: Bad file number*/
    { ECHILD,  -55 }, /* 10: No children*/
    { EAGAIN,  -72 }, /* 11: Try again*/
    { ENOMEM,  -12 }, /* 12: Not enough core*/
    { EACCES,  -13 }, /* 13: Permission denied*/
    { EFAULT,  -57 }, /* 14: Bad address*/
#ifdef ENOTBL
    { ENOTBLK, -58 }, /* 15: Block device required*/
#endif
    { EBUSY,   -59 }, /* 16: Mount device busy*/
    { EEXIST,  -17 }, /* 17: File exists*/
    { EXDEV,   -18 }, /* 18: Cross-device link*/
    { ENODEV,  -19 }, /* 19: No such device*/
    { ENOTDIR, -20 }, /* 20: Not a directory*/
    { EISDIR,  -21 }, /* 21: Is a directory*/
    { EINVAL,  -22 }, /* 22: Invalid argument*/
    { ENFILE,  -23 }, /* 23: File table overflow */
    { EMFILE,  -24 }, /* 24: Too many open files */
    { ENOTTY,  -25 }, /* 25: Not a typewriter */
#ifdef ETXTBSY
    { ETXTBSY, -26 }, /* 26: Text file busy */
#endif
    { EFBIG,   -27 }, /* 27: File too large */
    { ENOSPC,  -28 }, /* 28: No space left on device */
    { ESPIPE,  -29 }, /* 29: Illegal seek */
    { EROFS,   -30 }, /* 30: Read-Only File-System */
    { EMLINK,  -31 }, /* 31: Too many links*/
    { EPIPE,   -32 }, /* 32: Broken pipe*/
    { EDOM,    -62 }, /* 33: Math arg out of domain of func*/
    { ERANGE,  -63 }, /* 34: Math result not representable*/
    { EDEADLK,      -69 }, /* 35: Resource deadlock would occur*/
    { ENAMETOOLONG, -70 }, /* 36: File name too long */

    { ENOSYS,       -38 }, /* 38: Function not implemented */
    { ENOTEMPTY,    -39 }, /* 39: Directory not empty */
#ifdef ELOOP
    { ELOOP,        -71 }, /* 40: Too many symbolic links encountered */
#endif
#ifdef EWOULDBLOCK
    { EWOULDBLOCK,  -41 }, /* 41: Operation would block */
#endif
#ifdef ENOMSG
    { ENOMSG,       -42 }, /* 42: No message of desired type*/
#endif
#ifdef EIDRM
    { EIDRM,        -43 }, /* 43: Identifier removed*/
#endif
#ifdef ELNRNG
    { ELNRNG,       -48 }, /* 48: Link number out of range*/
#endif
#ifdef EBADE 
    { EBADE,        -52 }, /* 52: Invalid exchange*/
#endif
#ifdef EXFULL
    { EXFULL,       -54 }, /* 54: Exchange full*/
#endif
#ifdef ENOSTR
    { ENOSTR,       -60 }, /* 60: Device not a stream */
#endif  
#ifdef ENOTSOCK
    { ENOTSOCK,     -88 }, /* 88: Socket operation on non-socket */

    { EOPNOTSUPP,   -95 }, /* 95: Operation not supported on transport endpoint */

    { EADDRINUSE,   -98 }, /* 98: Address already in use */
    { EADDRNOTAVAIL,-99 }, /* 99: Cannot assign requested address */

    { ENETDOWN,    -100 }, /* 100: Network is down */
    { ENETUNREACH, -101 }, /* 101: Network is unreachable */

    { ENETRESET,   -102 }, /* 102: Network dropped connection because of reset */
    { ECONNABORTED,-103 }, /* 103: Software caused connection abort */

    { ECONNRESET,  -104 }, /* 104: Connection reset by peer*/

    { ENOBUFS,     -105 }, /* 105: No buffer space available */
    { EISCONN,     -106 }, /* 106: Transport endpoint is already connected*/
    { ENOTCONN,    -107 }, /* 107: Transport endpoint is not connected */
    { ETIMEDOUT,   -110 }, /* 110: Connection timed out */
    { ECONNREFUSED,-111 }  /* 111: Connection refused */
#endif  
  };
static const int anztabs=sizeof(ioemaptable)/sizeof(struct {int sf; char xf; });
  
void io_error(int n, const char *s) {
  int i;
  for(i=0;i<anztabs;i++) {
    if(n==ioemaptable[i].sf) {
      xberror(ioemaptable[i].xf,s);
      return;    
    }
  }
  printf("errno=%d\n",n);
  xberror(-1,s);  /* Allgemeiner IO-Fehler */
}
