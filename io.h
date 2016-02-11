/* io.h I/O_Routinen  (c) Markus Hoffmann  */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
 
#ifdef WINDOWS
//#define FD_SETSIZE 4096
#define EINPROGRESS   WSAEINPROGRESS
#define EWOULDBLOCK   WSAEWOULDBLOCK
#define gettimeofday(a,b) QueryPerformanceCounter(a)
#else 
#define send(s,b,l,f) write(s,b,l)
#define recv(s,b,l,f) read(s,b,l)
#define closesocket(s) close(s)
#define ioctlsocket(a,b,c) ioctl(a,b,c)
#endif

char *do_gets (char *prompt);
int kbhit();

void io_error(int,char *);

void getrowcols(int *rows, int *cols);

STRING longlineinput(FILE *n);
int get_number(char *w);
int inp8(PARAMETER *plist,int e);
int inpf(PARAMETER *plist,int e);
int inp16(PARAMETER *plist,int e);
int inp32(PARAMETER *plist,int e);
void set_input_mode(int mi, int ti);
void set_input_mode_echo(int onoff);
void reset_input_mode();
char *terminalname(int fp);
char *fileevent();
char *inkey();
STRING print_arg(char *ausdruck);
STRING do_using(double num,STRING format);
void memdump(unsigned char *adr,int l);
void speaker(int frequency);


int f_freefile();
int f_map(PARAMETER *plist,int e);
STRING f_lineinputs(PARAMETER *plist,int e);
STRING f_inputs(char *n);
int f_exec(char *n);
int f_symadr(PARAMETER *plist,int e);
int f_ioctl(PARAMETER *plist,int e);

void c_print(PARAMETER *plist,int e);

void c_msync(PARAMETER *plist,int e);
void c_unmap(PARAMETER *plist,int e);
void c_locate(PARAMETER *plist,int e);
void c_input(char *n);
void c_lineinput(char *n);
void c_connect(PARAMETER *plist,int e);
void c_open(PARAMETER *plist, int e);
void c_link(PARAMETER *plist, int e);
void c_send(PARAMETER *plist, int e);
void c_receive(PARAMETER *plist, int e);
void c_close(PARAMETER *plist,int e);
void c_exec(char *n);
void c_bload(PARAMETER *plist,int e);
void c_bsave(PARAMETER *plist,int e);
void c_bget(PARAMETER *plist,int e);
void c_bput(PARAMETER *plist,int e);
void c_bmove(PARAMETER *plist,int e);
void c_pipe(PARAMETER *plist,int e);
void c_unget(PARAMETER *plist,int e);
void c_flush(PARAMETER *plist,int e);
void c_seek(PARAMETER *plist,int e);
void c_relseek(PARAMETER *plist,int e);
void c_out(char *n);
void c_chdir(PARAMETER *plist,int e);
void c_watch(PARAMETER *plist,int e);
