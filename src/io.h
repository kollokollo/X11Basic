/* io.h I/O_Routinen  (c) Markus Hoffmann  */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#ifdef _WIN32
//#define FD_SETSIZE 4096
//#define EINPROGRESS   WSAEINPROGRESS
//#define EWOULDBLOCK   WSAEWOULDBLOCK
#define gettimeofday(a,b) QueryPerformanceCounter(a)
#else 
#define send(s,b,l,f) write(s,b,l)
#define recv(s,b,l,f) read(s,b,l)
#define closesocket(s) close(s)
#define ioctlsocket(a,b,c) ioctl(a,b,c)
#endif

#ifdef __APPLE__
  /* stuff for OSX */
#endif

/*  File handling structures */

#define FT_NONE   0
#define FT_FILE   1
#define FT_DLL    2
#define FT_DEV    3
#define FT_SOCKET 4
#define FT_USB    5
#define FT_PIPE   6
#define FT_BTRC   7
#define FT_BTL2   8

typedef struct {
  int typ;
  FILE *dptr;
  unsigned int blk_len;
  unsigned char ep_in;
  unsigned char ep_out;
  #if defined(_WIN32)
  HANDLE cport;   /* COM-Port Handle*/
  #endif
} FILEINFO;


extern FILEINFO filenr[];



#ifdef ANDROID 
  #define ANZSENSORS 32
#else
  #define ANZSENSORS 0
#endif

extern double sensordata[];

char *do_gets (char *prompt);
int kbhit();

void io_error(int,const char *);

void getrowcols(int *rows, int *cols);
void getcrsrowcol(int *rows, int *cols);

int get_number(const char *w);
FILEINFO get_fileptr(int n);

void close_all_files();


int inp8(PARAMETER *plist,int e);
int inpf(PARAMETER *plist,int e);
int inp16(PARAMETER *plist,int e);
int inp32(PARAMETER *plist,int e);
void touch(PARAMETER *plist,int e);
void set_input_mode(int mi, int ti);
void set_input_mode_echo(int onoff);
void reset_input_mode();
char *terminalname(int fp);
char *fileevent();
char *inkey();

STRING print_arg(const char *ausdruck);
STRING do_using (double num,STRING format);
void memdump    (const unsigned char *adr,int l);
void speaker    (int frequency);

int spawn_shell (char *argv[]);
int f_freefile();
int f_map(PARAMETER *plist,int e);
STRING f_lineinputs(PARAMETER *plist,int e);
STRING f_fsfirsts(PARAMETER *plist,int e);
STRING f_fsnexts();
STRING f_inputs(char *n);
int f_call(PARAMETER *plist,int e);
int f_exec(PARAMETER *plist,int e);
int f_symadr(PARAMETER *plist,int e);
int f_ioctl(PARAMETER *plist,int e);




void c_print    (PARAMETER *plist,int e);
void c_msync    (PARAMETER *plist,int e);
void c_unmap    (PARAMETER *plist,int e);
void c_locate   (PARAMETER *plist,int e);
void c_input    (const char *n);
void c_lineinput(const char *n);
void c_connect  (PARAMETER *plist,int e);
void c_open     (PARAMETER *plist, int e);
void c_link     (PARAMETER *plist, int e);
void c_send     (PARAMETER *plist, int e);
void c_receive  (PARAMETER *plist, int e);
void c_close    (PARAMETER *plist,int e);
void c_call     (PARAMETER *plist,int e);
void c_exec     (PARAMETER *plist,int e);
void c_bload  (PARAMETER *plist,int e);
void c_bsave  (PARAMETER *plist,int e);
void c_bget   (PARAMETER *plist,int e);
void c_bput   (PARAMETER *plist,int e);
void c_bmove  (PARAMETER *plist,int e);
void c_pipe   (PARAMETER *plist,int e);
void c_unget  (PARAMETER *plist,int e);
void c_flush  (PARAMETER *plist,int e);
void c_seek   (PARAMETER *plist,int e);
void c_relseek(PARAMETER *plist,int e);
void c_out    (PARAMETER *plist,int e);
void c_chdir  (PARAMETER *plist,int e);
void c_chmod (PARAMETER *plist,int e);
void c_mkdir  (PARAMETER *plist,int e);
void c_rmdir  (PARAMETER *plist,int e);
void c_kill   (PARAMETER *plist,int e);
void c_rename (PARAMETER *plist,int e);
void c_watch  (PARAMETER *plist,int e);
