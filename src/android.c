/* android.C                                           (c) Markus Hoffmann

Android version.

*/

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ======================================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h> 
#include <jni.h>
#include <time.h>
#include <android/log.h>
#include <android/bitmap.h>
#include <pthread.h>    /* POSIX Threads */
#include <signal.h>
#include <math.h>

#include "config.h"
#include "defs.h"

#include "x11basic.h"
#include "android.h"
#include "io.h"
#include "framebuffer.h"
#include "variablen.h"
#include "graphics.h"
#include "window.h"

extern int termecho;
extern int terminal_last_esc;
extern char terminal_last_char;
extern int fbfd;
extern WINDOWDEF window[];
extern int usewindow;

extern int loadfile;

static JavaVM *m_vm;
static jobject x11basicActivity=NULL;
static jobject x11basicView=NULL;

int android_init=0;
static int isdirectmode=0;
int font_behaviour=0;

/*Das ist der Screen*/

AndroidBitmapInfo  screen_info;

/* Hier alle Methoden, welche aufgerufen werden sollen zwischenspeichern: */

static jmethodID nativeCrashed;
static jmethodID redrawMethod;
static jmethodID beepMethod;
static jmethodID hidekMethod;
static jmethodID showkMethod;
static jmethodID getlocationMethod;
static jmethodID playsoundfileMethod;
static jmethodID playtoneMethod; 
static jmethodID runaudioMethod; 
static jmethodID speekMethod; 
static jmethodID gpsonoffMethod; 
static jmethodID sensoronoffMethod;
static jmethodID callintentMethod; 


/* fuer Ausgabeumleitung */

int in_pipe[2];
int out_pipe[2];
void *screen_pixels;

static struct sigaction old_sa[NSIG];

/* globale veriablen, welche die GPS Informationen aufnehmen.*/

extern double gps_lon,gps_lat,gps_alt;
extern double gps_bearing,gps_accuracy,gps_speed;
extern double gps_time;
extern char *gps_provider;

static int ringbufout(char *d,int n);

/*Diese Routine wird nur ein einziges mal beim linken der x11basic lib 
  aufgerufen. */

JNIEXPORT  jint JNI_OnLoad(JavaVM* vm, void* reserved) {
  jclass cls, godcls;
  JNIEnv *env;
  NLOG("OnLoad.");

  /* Globale Referenz um Java Environment ermitteln und abspeichern. Wird bei SIGNAL Handler benÃ¶tigt, 
     da dieser dies sonst nicht ermitteln kann und dann auch nicht den Crash-Handler starten kann.
   */
  
  if((*vm)->GetEnv(vm, (void **)&env, JNI_VERSION_1_4)) return JNI_ERR; /* JNI version not supported */
  m_vm = vm;

  cls=(*env)->FindClass(env,"net/sourceforge/x11basic/X11basicView"); 
  if (cls == NULL) return JNI_ERR;
  godcls=(*env)->FindClass(env,"net/sourceforge/x11basic/X11BasicActivity"); 
  if (godcls == NULL) return JNI_ERR;
  
  
  /* Referenzen zu den Java-Proceduren ermitteln und abspeichern.*/
  
  nativeCrashed      =(*env)->GetMethodID(env,cls, "nativeCrashed", "(I)V");
  redrawMethod       =(*env)->GetMethodID(env,cls, "redraw",        "()V");
  
  beepMethod         =(*env)->GetMethodID(env,cls, "beep",	    "()V");
  hidekMethod         =(*env)->GetMethodID(env,cls, "hidek",	    "()V");
  showkMethod         =(*env)->GetMethodID(env,cls, "showk",	    "()V");
  getlocationMethod  =(*env)->GetMethodID(env,cls, "get_location",  "()V");
  playsoundfileMethod=(*env)->GetMethodID(env,cls, "playsoundfile", "(Ljava/lang/String;)V");
  runaudioMethod     =(*env)->GetMethodID(env,cls, "RunAudioThreads","()V");
  playtoneMethod     =(*env)->GetMethodID(env,cls, "playtone",      "(FFI)V");
  speekMethod        =(*env)->GetMethodID(env,cls, "speek",	    "(Ljava/lang/String;FFI)V");
  gpsonoffMethod     =(*env)->GetMethodID(env,cls, "gps_onoff",     "(I)V");
  sensoronoffMethod  =(*env)->GetMethodID(env,cls, "sensor_onoff",  "(I)V");
  callintentMethod   =(*env)->GetMethodID(env,cls, "call_intent",   "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");

  
  
  
  /* Use weak global ref to allow C class to be unloaded */

  // mcls = (*env)->NewWeakGlobalRef(env, cls);
  // if(mcls==NULL) return JNI_ERR;

  // Try to catch crashes...
  struct sigaction handler;
  memset(&handler, 0, sizeof(sigaction));
  handler.sa_sigaction =android_sigaction;
  handler.sa_flags = SA_RESETHAND;
  #define CATCHSIG(X) sigaction(X, &handler, &old_sa[X])
  CATCHSIG(SIGILL);
  CATCHSIG(SIGABRT);
  CATCHSIG(SIGBUS);
  CATCHSIG(SIGFPE);
  CATCHSIG(SIGSEGV);
  CATCHSIG(SIGSTKFLT);
  CATCHSIG(SIGPIPE);



  x11basicStartup();   /* initialisieren   */
  
  
  /* Stdout umleiten*/

  if(pipe(out_pipe)==0) {          /* make a pipe */
    dup2(out_pipe[1], STDOUT_FILENO);   /* redirect stdout to the pipe */
    close(out_pipe[1]);
    fcntl(out_pipe[0], F_SETFL, O_NONBLOCK);
  } else LOGE("ERROR pipe %d.", 0);

/* Stdin umleiten*/
 
  if(pipe(in_pipe)==0) {          /* make a pipe */
    dup2(in_pipe[0], STDIN_FILENO);   /* redirect stdin to the pipe */
    close(in_pipe[0]);
    set_terminal_fd(in_pipe[1]);
  }
  return JNI_VERSION_1_4;
}
/********************************************************************************/
  int fdm, fds, rc; 

/* Diese Funktion wird jedesmal bei OnCreate der Activity aufgerufen. Dies 
  passiert auch nach dem 
  Rotieren des Bildschirms. Hierbei soll ein Programm erhalten bleiben.
  Die initialisierung erfolgte schon beim laden der Library. 
  Es werden ggf. Kommandozeilen bearbeitet (unerheblich derzeit) und ggf. ein 
  Programm geladen. 
*/


JNIEXPORT void JNICALL Java_net_sourceforge_x11basic_X11basicView_Init(JNIEnv *env, jobject _obj, jlong  time_ms, jstring arguments) {
  static char *commandline=NULL;
  param_anzahl=0;
  if(!param_argumente) param_argumente=(char **)malloc(128*sizeof(char *));
  NLOG("vInit.");
/* evtl hier eine Kommandozeile behandeln ....*/
  const char *n = (*env)->GetStringUTFChars(env, arguments, 0);//Get the native string from javaString
  if(commandline) free(commandline);
  char *a=commandline=strdup(n);
  (*env)->ReleaseStringUTFChars(env, arguments, n);   //DON'T FORGET THIS LINE!!!

  while (isspace(*a)) ++a;
  while (*a) {
    if (param_argumente) param_argumente[param_anzahl] = a;
    while (*a && !isspace(*a)) ++a;
    if (param_argumente && *a) *a++ = '\0';
    while (isspace(*a)) ++a;
    param_anzahl++;
    if(param_anzahl>=128) break;
  }
#if 0
  /* Pseudo-Terminal nutzen */
  fdm = posix_openpt(O_RDWR);
  if (fdm<0) LOGE("Error %d on posix_openpt()\n", errno);
  else {
    rc = grantpt(fdm);
    if (rc != 0) {
    LOGE( "Error %d on grantpt()\n", errno);
  }
  rc=unlockpt(fdm);
  if(rc!=0) LOGE("Error %d on unlockpt()\n", errno);
  fds = open(ptsname(fdm), O_RDWR); /* Open the slave PTY  */
  dup2(fds,STDOUT_FILENO);          /* Stdout umleiten*/
  set_terminal_fd(fdm);
}
#endif
/********************************/
  android_init=1;
  if(param_anzahl<2) {    /* Kommandomodus */
    if(batch==0) intro();
    //else { /* Was initializes a second time while a program is still running.*/
    //  printf("continue running program: <%s> (len=%d), pc=%d\r",ifilename,prglen,pc);
    // }
  } else {
    kommandozeile(param_anzahl, param_argumente);    /* Kommandozeile bearbeiten */
    if(loadfile) {
      if(exist(ifilename)) {
        loadprg(ifilename);
	if(runfile) do_run();
      } else printf("ERROR: %s not found !\n",ifilename);
    }
  }
}



/* wird immer dann aufgerufen, wenn sich die Screengroesse veraendert hat,
   oder die Bitmap verlagert wurde....
 */

JNIEXPORT void JNICALL Java_net_sourceforge_x11basic_X11basicView_InitScreen(JNIEnv *env, jobject  obj, jobject bitmap) {
    int                ret;
    static int         init=0; 
    NLOG("i.");
    if ((ret = AndroidBitmap_getInfo(env, bitmap, &screen_info)) < 0) {
        LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
        return;
    }
    if (screen_info.format != ANDROID_BITMAP_FORMAT_RGB_565) {
        LOGE("Bitmap format is not RGB_565 !");
        return;
    }
    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &screen_pixels)) < 0) {
        LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
    }
    if(fbfd>0) {window[usewindow].flags=0;fbfd = -1;}
    Fb_Open();
    isdirectmode=0;
    LOGI("initScreen");
    AndroidBitmap_unlockPixels(env, bitmap);
}




/*  Sollte von OnCreate der Activity aufgerufen werden, da ggf. ein neues 
    Objekt erzeugt wurde.
    Dies sollte nicht zu oft geschehen, da sonst einexcessive Global reference 
    Fehler kommen kann.
*/

JNIEXPORT void JNICALL Java_net_sourceforge_x11basic_X11basicView_setObject(JNIEnv *env, jobject  obj, jobject _x11basicView) {
  NLOG("setObject.");
  if(x11basicView) (*env)->DeleteGlobalRef(env,x11basicView);
  x11basicView = (*env)->NewGlobalRef(env, _x11basicView);
}
JNIEXPORT void JNICALL Java_net_sourceforge_x11basic_X11basicView_setHomeDir(JNIEnv *env, jobject  obj, jstring in) {
  const char *n = (*env)->GetStringUTFChars(env,in, 0);
  NLOG("setHomeDir.");
  chdir(n);
  (*env)->ReleaseStringUTFChars(env,in, n);
}
JNIEXPORT void JNICALL Java_net_sourceforge_x11basic_X11basicView_terminalfontsize(JNIEnv *env, jobject  obj, jint n) {
  font_behaviour=n;
  NLOG("Fntsize.");
}

JNIEXPORT jstring JNICALL Java_net_sourceforge_x11basic_X11basicView_Stdout(JNIEnv *env, jobject  obj) {
  NLOG("Stdout.");
  fflush(stdout);
  return  (*env)->NewStringUTF(env,flush_terminal());
}
JNIEXPORT void JNICALL Java_net_sourceforge_x11basic_X11basicView_Stdin(JNIEnv *env, jobject  obj, jstring in) {
  const char *n = (*env)->GetStringUTFChars(env,in, 0);    //Get the native string from javaString
  NLOG("Stdin.");
  if(android_init) write(in_pipe[1],n,strlen(n));
  (*env)->ReleaseStringUTFChars(env,in, n);                //DON'T FORGET THIS LINE!!!
}
JNIEXPORT void JNICALL Java_net_sourceforge_x11basic_X11basicView_queueKeyEvent(JNIEnv *env, jclass c, jint key, jint state) {
  NLOG("queueKeyEvent.");
  //  if(queueKeyEvent) queueKeyEvent(key, state); 
  LOGE("Que Key event %d ...",key);  
}
JNIEXPORT void JNICALL Java_net_sourceforge_x11basic_X11basicView_Putchar(JNIEnv *env, jobject  obj, jint n) {
  NLOG("p.");
  char nn=(char)n;
  NLOG("p.");
  write(in_pipe[1],&nn,1);
  if(termecho) {
    NLOG("pe.");
   // write(out_pipe[1],&nn,1);
    putchar(nn); 
    fflush(stdout);
    invalidate_screen();
  } 
  // fflush(stdin);
}

extern int lin, CharHeight;
JNIEXPORT jint JNICALL Java_net_sourceforge_x11basic_X11basicView_getFocuscursor(JNIEnv *env, jobject  obj) {
  return(CharHeight*lin);   /* Soll cursor(y)position zurüeckgeben....*/
}

JNIEXPORT void JNICALL Java_net_sourceforge_x11basic_X11basicView_setLocation(JNIEnv *env, jobject  obj, jdouble lat,jdouble lon,jdouble alt) {
  gps_alt=alt;
  gps_lon=lon;
  gps_lat=lat;
  NLOG("setLoc.");
}
JNIEXPORT void JNICALL Java_net_sourceforge_x11basic_X11basicView_setLocationInfo(JNIEnv *env, jobject  obj, 
  jfloat bar,jfloat acu,jfloat speed,jlong time, jstring prov) {
  NLOG("setLocationInfo.");
  gps_bearing=(double)bar;
  gps_accuracy=(double)acu;
  gps_speed=(double)speed;
  gps_time=(double)time/1000.0;
/*
  printf("bar=%g\n",(double)bar);
  printf("acu=%g\n",(double)acu);
  printf("spe=%g\n",(double)speed);
*/
  const char *n  = (*env)->GetStringUTFChars(env,prov, 0);
  gps_provider=realloc(gps_provider,strlen(n)+1);
  strcpy(gps_provider,n);
  (*env)->ReleaseStringUTFChars(env,prov,n);
}
JNIEXPORT void JNICALL Java_net_sourceforge_x11basic_X11basicView_setSensorValues(
  JNIEnv *env, jobject  obj, jint offset,jint nval,jfloat a0,jfloat a1,jfloat a2) {
  NLOG("setSensorValues."); 
  if(nval>=1) sensordata[offset]=(double)a0;
  if(nval>=2) sensordata[offset+1]=(double)a1;
  if(nval>=3) sensordata[offset+2]=(double)a2;
}

JNIEXPORT void JNICALL Java_net_sourceforge_x11basic_X11basicView_SetMouse(JNIEnv *env, jobject  obj, jint x,jint y,jint k) {
 // char lbuf[100];
  XEvent e;
  NLOG("setM{");
  if(k==0) {
    e.type=ButtonPress;
    e.xbutton.button=1;
    e.xbutton.x=x;
    e.xbutton.y=y;
    e.xbutton.state=0;
    e.xbutton.x_root=x;
    e.xbutton.y_root=y;
    screen.mouse_k=1;
  } else if(k==1) {
    e.type=ButtonRelease;
    e.xbutton.button=0;
    e.xbutton.x=x;
    e.xbutton.y=y;
    e.xbutton.state=0;
    e.xbutton.x_root=x;
    e.xbutton.y_root=y;    
    screen.mouse_k=0;
  } else {
    screen.mouse_k=1;
    e.type=MotionNotify;
    e.xmotion.x=x;
    e.xmotion.y=y;
    e.xmotion.x_root=x;
    e.xmotion.y_root=y;
    e.xmotion.state=0;
  }
  FB_put_event(&e);
  NLOG("E");
  FB_hidex_mouse();
  NLOG("H");
  screen.mouse_x=x;
  screen.mouse_y=y;
  FB_showx_mouse();
  NLOG("S");
  invalidate_screen();
  NLOG("}.");
}

JNIEXPORT void JNICALL Java_net_sourceforge_x11basic_X11basicView_SetMouseMotion(JNIEnv *env, jobject  obj, jint dx,jint dy) {
  NLOG("setMM{");
  XEvent e;
  e.type=MotionNotify;
  e.xmotion.x=screen.mouse_x+dx;
  e.xmotion.y=screen.mouse_y+dy;
  e.xmotion.x_root=screen.mouse_x+dx;
  e.xmotion.y_root=screen.mouse_y+dy;
  e.xmotion.state=0;
  FB_put_event(&e);
  NLOG("}.");
}
JNIEXPORT void JNICALL Java_net_sourceforge_x11basic_X11basicView_SetKeyPressEvent(JNIEnv *env, jobject  obj, jint kk,jint a, jint hi) {
  NLOG("kpe."); 
  XEvent e;
  e.type=KeyPress;
  e.xkey.keycode=(char)kk;
  e.xkey.ks=a&255|(hi<<8);
  e.xkey.buf[0]=(char)a;
  e.xkey.buf[1]=0;
  
  FB_put_event(&e);
}
JNIEXPORT void JNICALL Java_net_sourceforge_x11basic_X11basicView_SetKeyReleaseEvent(JNIEnv *env, jobject  obj, jint kk,jint a, jint hi) {
  NLOG("kre.");
  XEvent e;
  e.type=KeyRelease;
  e.xkey.keycode=(char)kk;
  e.xkey.ks=a&255|(hi<<8);
  e.xkey.buf[0]=(char)a;
  e.xkey.buf[1]=0;
  FB_put_event(&e);
}

JNIEXPORT void JNICALL Java_net_sourceforge_x11basic_X11basicView_AudioFillStreamBuffer(JNIEnv *env, void* reserved, jshortArray location, jint size) {
  NLOG("audio{");
  /* Get the short* pointer from the Java array  */
  jboolean isCopy = JNI_TRUE;
  jshort* dst = (*env)->GetShortArrayElements(env,location, &isCopy);
/*
   ---> gAudioManager.FillStreamBuffer(dst, size);
--> sollte als rueckgabe ein flag haben, ob ueberhaupt noch 
     sound gespielt werden soll. Dann kann man evtl. den Thred abschalten. setzt
     aber voraus, dass jeder SOUND befehl das wieder in Gang setzt. (erstmal
     lassen)
*/
  NLOG("m");
  mixeAudio(dst, size);
//int i=0;
//for(i=0;i<size/4;i++) {
//  dst[2*i]=0x000+(short)(32000.0*sin(440.0/44100*2*3.141592*i));
//  dst[2*i+1]=0x000+(short)(32000.0*sin(500.0/44100*2*3.141592*i));
// // if(i<5) printf("--> %d\n",dst[2*i]);
//}
  NLOG("f");
  (*env)->ReleaseShortArrayElements(env,location, dst, 0);   /* Release the short* pointer */
  NLOG("}.");
}


extern void c_new(char *n),c_cont(),c_stop();

JNIEXPORT void JNICALL Java_net_sourceforge_x11basic_X11basicView_New(JNIEnv *env, jobject  obj) {
  NLOG("New.");
  c_new("");
}
JNIEXPORT void JNICALL Java_net_sourceforge_x11basic_X11basicView_StopCont(JNIEnv *env, jobject  obj) {
  NLOG("StopCont.");
  if(batch==0) c_cont();
  else c_stop();

}
JNIEXPORT void JNICALL Java_net_sourceforge_x11basic_X11basicView_Stop(JNIEnv *env, jobject  obj) {
  NLOG("Stop.");
  puts("** PROGRAM-STOP");
  c_stop();
}
JNIEXPORT jint JNICALL Java_net_sourceforge_x11basic_X11basicView_Batch(JNIEnv *env, jobject  obj) {
  NLOG("Batch.");
  return(batch);
}

static void send_terminate_event() {
  XEvent e;
  e.type=TerminateEventLoop;
  FB_put_event(&e);
}


JNIEXPORT void JNICALL Java_net_sourceforge_x11basic_X11basicView_Load(JNIEnv *env, jobject  obj, jstring filename) {
  NLOG("Load.");
  const char *n = (*env)->GetStringUTFChars(env,filename, 0);
  strcpy(ifilename,n);
  if(exist(ifilename)) {
    if(batch) {
      int i=10;
      c_stop();
      send_terminate_event();
      puts("** PROGRAM-STOP"); 
      backlog("wait for program to stop...");
      while(!isdirectmode && --i) usleep(10000);
    }
    loadprg(ifilename); 
    backlog("load done."); 
    invalidate_screen();
  } else LOGE("ERROR: %s not found !",ifilename);
  (*env)->ReleaseStringUTFChars(env,filename, n);
}

JNIEXPORT void JNICALL Java_net_sourceforge_x11basic_X11basicView_Run(JNIEnv *env, jobject  obj) {
  NLOG("Run.");
  graphics_setdefaults();
  do_run();
}
JNIEXPORT void JNICALL Java_net_sourceforge_x11basic_X11basicView_Loadandrun(JNIEnv *env, jobject  obj, jstring filename) {
  NLOG("Load+run.");
  const char *n = (*env)->GetStringUTFChars(env,filename, 0);
  strcpy(ifilename,n);
  if(exist(ifilename)) {
    if(batch) {
      int i=10;
      c_stop();
      send_terminate_event();
      puts("** PROGRAM-STOP"); 
      backlog("wait for program to stop...");
      while(!isdirectmode && --i) usleep(10000);
    }
    loadprg(ifilename);
    graphics_setdefaults();
    do_run();
  } else LOGE("ERROR: %s not found !",ifilename);
  (*env)->ReleaseStringUTFChars(env,filename,n);
}

/* Diese Funktion wird von einem eigenen Thread gestartet (RunThread.java)
und hat deshalb auch einen eigenen Context (env).*/

JNIEXPORT void JNICALL Java_net_sourceforge_x11basic_X11basicView_Programmlauf(JNIEnv *env, jobject _obj) {
  NLOG("Programmlauf.");
#if 0
  /* Das ganze sollte in eigenem Thread gestartet werden.  */
  pthread_t thread1;
  pthread_create (&thread1, NULL, (void *) &thread_function, NULL);
   LOGE("Thread gestartet !");
#else
  thread_function ( NULL );
#endif
}


/*             Bytecode compiler           */

#define MAX_CODE 256000
int dostrip=0;                   /* dont write symbol tables */
int donops=0;
int docomments=0;
STRING bcpc;

JNIEXPORT jint JNICALL Java_net_sourceforge_x11basic_X11basicView_Compile(JNIEnv *env, jobject  obj,jstring filename) {
  bcpc.pointer=malloc(MAX_CODE);
  int status=0;
  NLOG("Compile{");
  if(prglen>0) {
        int ret;
        const char *ofilename = (*env)->GetStringUTFChars(env,filename, 0);
 
	printf("\ncompile <%s> --> <%s>\n",ifilename,ofilename);
backlog("compile start");
	compile(1);
	backlog("compile done.");
	ret=save_bytecode(ofilename,(char *)bcpc.pointer,bcpc.len,databuffer,databufferlen);
	backlog("save bytecode done.");
	if(ret==-1) status=-3;
       (*env)->ReleaseStringUTFChars(env,filename, ofilename);//DON'T FORGET THIS LINE!!!
       invalidate_screen();
  } else status=-1;
  free(bcpc.pointer);
  NLOG("}.");
  return(status);
}

JNIEXPORT jstring JNICALL Java_net_sourceforge_x11basic_X11basicView_Getcrashinfo(JNIEnv *env, jobject  obj) {
  char message[MAXSTRLEN*10];
  char a[MAXSTRLEN];
  NLOG("Getcrashinfo.");
  sprintf(message,"pc=%d/%d, sp=%d, filename=%s, lastchar=%d/%d\n",pc,prglen,sp,ifilename,(int)terminal_last_char,terminal_last_esc);
  if(batch) {
    if(pc>0) {
      if(pc>1) {
        sprintf(a,"    pc-1  : %s\n",program[pc-2]);
	strcat(message,a);
      }
      sprintf(a,"--> pc=%d : %s\n",pc-1,program[pc-1]);strcat(message,a);
      if(pc<prglen-1) {
        sprintf(a,"    pc+1  : %s\n",program[pc]);strcat(message,a);
      }
    } 
  } else if(pc>0 && pc<prglen) {
    sprintf(a,"Program stopped at: \n");strcat(message,a);
    sprintf(a,"--> pc=%d : %s\n",pc-1,program[pc-1]);strcat(message,a);
  }
  sprintf(a,"command=<%s>\n",lastcommand);strcat(message,a);
  /*Variablen-Dumps:*/
  
  strcat(message,"\nDUMP:\n-----\n");
  char *p=dump_var(INTTYP);strcat(message,p);free(p);
  p=dump_var(FLOATTYP);strcat(message,p);free(p);
  p=dump_var(STRINGTYP);strcat(message,p);free(p);
  p=dump_arr(INTTYP);strcat(message,p);free(p);
  p=dump_arr(FLOATTYP);strcat(message,p);free(p);
  p=dump_arr(STRINGTYP);strcat(message,p);free(p);
  
  strcat(message,"\nlogtrace: ");
  ringbufout(a,MAXSTRLEN-1);
  strncat(message,a,MAXSTRLEN*10-strlen(message)-1);
  return  (*env)->NewStringUTF(env,message);
}




/*
 *Rueckruffunktionen
 */
void ANDROID_beep() {
  NLOG(">beep.");
  JNIEnv *env;
  int status = (*m_vm)->AttachCurrentThread(m_vm,&env, NULL);
  if(status < 0) LOGE("beep: ERROR, no env.");
  else if(beepMethod == NULL) LOGE("Error: Can't find Java method void beep()");
  else {
    (*env)->CallVoidMethod(env,x11basicView,beepMethod);
    (*env)->ExceptionClear(env);
  }
}
void ANDROID_hidek() {
  NLOG(">hidek.");
  JNIEnv *env;
  int status = (*m_vm)->AttachCurrentThread(m_vm,&env, NULL);
  if(status < 0) LOGE("hidek: ERROR, no env.");
  else if(hidekMethod == NULL) LOGE("Error: Can't find Java method void hidek()");
  else {
    (*env)->CallVoidMethod(env,x11basicView,hidekMethod);
    (*env)->ExceptionClear(env);
  }
}
void ANDROID_showk() {
  NLOG(">showk.");
  JNIEnv *env;
  int status = (*m_vm)->AttachCurrentThread(m_vm,&env, NULL);
  if(status < 0) LOGE("showk: ERROR, no env.");
  else if(showkMethod == NULL) LOGE("Error: Can't find Java method void showk()");
  else {
    (*env)->CallVoidMethod(env,x11basicView,showkMethod);
    (*env)->ExceptionClear(env);
  }
}
/*Triggert das auslesen der GPS-Werte. Diese befinden sich hinterher in 
  den entsprechenden globalen variablen.*/
void ANDROID_get_location() {
  NLOG(">get_location.");
  JNIEnv *env;
  int status = (*m_vm)->AttachCurrentThread(m_vm,&env, NULL);
  if(status<0) LOGE("beep: ERROR, no env.");
  else if(getlocationMethod == NULL) LOGE("Error: Can't find Java method getlocation()");
  else {
    (*env)->CallVoidMethod(env,x11basicView,getlocationMethod);
    (*env)->ExceptionClear(env);
  }
}
void ANDROID_playsoundfile(char *name) {
  NLOG(">playsoundfile.");
  JNIEnv *env;
  int status = (*m_vm)->AttachCurrentThread(m_vm,&env, NULL);
  if(status < 0) LOGE("playsoundfile: ERROR, no env.");
  else if(playsoundfileMethod == NULL) {
    LOGE("Error: Can't find Java method playsoundfile()");
  } else {
    jstring jstr = (*env)->NewStringUTF(env,name);
    (*env)->CallVoidMethod(env,x11basicView, playsoundfileMethod,jstr);
    (*env)->ExceptionClear(env);
  }
}
void ANDROID_speek(char *t,double pitch, double rate, char *enc) {
  int loc=0;
  NLOG(">speek.");
  if(enc) {
    if(toupper(*enc)=='U') loc=0;
    else if(toupper(*enc)=='E') {
      if(toupper(enc[1])=='S') loc=5; 
      else loc=1;   /*  EN */
    }
    else if(toupper(*enc)=='D') loc=2;
    else if(toupper(*enc)=='F') loc=3;
    else if(toupper(*enc)=='I') loc=4;
  }
  JNIEnv *env;
  int status = (*m_vm)->AttachCurrentThread(m_vm,&env, NULL);
  if(status < 0) LOGE("speek: ERROR, no env.");
  else if(speekMethod== NULL) LOGE("Error: Can't find Java method speek()"); 
  else {
    jstring jstr = (*env)->NewStringUTF(env,t);
    jfloat p=pitch;
    jfloat r=rate;

    (*env)->CallVoidMethod(env,x11basicView,speekMethod,jstr,p,r,loc);
    (*env)->ExceptionClear(env);
  }
}
void ANDROID_init_sound() {
  NLOG(">isound.");
  backlog("init sound");
  JNIEnv *env;
  int status = (*m_vm)->AttachCurrentThread(m_vm,&env, NULL);
  if(status < 0) LOGE("isound: ERROR, no env.");
  else if(runaudioMethod== NULL) LOGE("Error: Can't find Java method RunAudioThreads()"); 
  else {
    (*env)->CallVoidMethod(env,x11basicView,runaudioMethod);
    (*env)->ExceptionClear(env);
  }
}
void ANDROID_sound(int c,double freq, double volume, int duration) {
  NLOG(">sound{");

  JNIEnv *env;
  int status = (*m_vm)->AttachCurrentThread(m_vm,&env, NULL);
  if(status < 0) LOGE("sound: ERROR, no env.");
  else if(playtoneMethod== NULL) LOGE("Error: Can't find Java method playtone()"); 
  else {
    jfloat p=volume;
    jfloat f=freq;
    (*env)->CallVoidMethod(env,x11basicView,playtoneMethod,f,p,duration);
    (*env)->ExceptionClear(env);
  }
  NLOG("}.");
}

void ANDROID_call_intent(const char *action,char *data, char *extra) {
  NLOG(">intent.");
  JNIEnv *env;
  int status = (*m_vm)->AttachCurrentThread(m_vm,&env, NULL);
  if(status < 0) LOGE("intent: ERROR, no env.");
  else if(callintentMethod== NULL) LOGE("Error: Can't find Java method call_intent()"); 
  else {
    jstring jstraction = (*env)->NewStringUTF(env,action);
    jstring jstrdata = (*env)->NewStringUTF(env,data);
    jstring jstrextra = (*env)->NewStringUTF(env,extra);
    
    (*env)->CallVoidMethod(env,x11basicView,callintentMethod,jstraction,jstrdata,jstrextra);
    (*env)->ExceptionClear(env);
  }
}

int ANDROID_waitfor_intentresult() {

/*  ... */



  return(0);
}



/* This tells java, that it should redraw the screen  SHOWPAGE/VSYNC, but 
   also PRINT */
void invalidate_screen() { 
  JNIEnv *env;
  NLOG(">*{");
  /* first flush the stdoutput*/
  fflush(stdout);
  flush_terminal();
  int status=(*m_vm)->AttachCurrentThread(m_vm,&env, NULL);
  if(status<0)                  LOGE("invalidate screen: ERROR, no env.");
  else if(redrawMethod == NULL) LOGE("Error: Can't find Java method void redraw()");
  else {
    (*env)->CallVoidMethod(env,x11basicView, redrawMethod);
    (*env)->ExceptionClear(env);
  }
  NLOG("}.");
 } 


void android_sigaction(int signum, siginfo_t *info, void *reserved) {
  NLOG(">~.");
  LOGE("Signal received.... CRASH");
  
  JNIEnv *env;
  int status = (*m_vm)->AttachCurrentThread(m_vm,&env, NULL);
  if(status<0) {
    LOGE("ERROR: no env. dangling thread?");
    char buf[MAXSTRLEN+sizeof(int)];
    ringbufout(buf,MAXSTRLEN);
    *((int *)(&buf[MAXSTRLEN]))=signum;
    bsave("x11basic.crash",buf,sizeof(buf));
  } else if(nativeCrashed == NULL) {
    LOGE("Error: Can't find Java method for crash handler.");
  } else {
    (*env)->CallVoidMethod(env,x11basicView, nativeCrashed,signum);
    (*env)->ExceptionClear(env);
  }
  signal(signum,(sighandler_t)android_sigaction);
  old_sa[signum].sa_handler(signum);
}


void do_gpsonoff(int n) {
  JNIEnv *env;
  NLOG(">gpsonoff.");
  int status = (*m_vm)->AttachCurrentThread(m_vm,&env, NULL);
  if(status < 0) LOGE("sensoronoff: ERROR, no env.");
  else if(gpsonoffMethod == NULL) LOGE("Error: Can't find Java method ()");
  else {
    (*env)->CallVoidMethod(env,x11basicView,gpsonoffMethod,n);
    (*env)->ExceptionClear(env);
  }
}
void do_sensoronoff(int n) {
  JNIEnv *env;
  NLOG(">sensoronoff.");
  int status = (*m_vm)->AttachCurrentThread(m_vm,&env, NULL);
  if(status < 0) LOGE("sensoronoff: ERROR, no env.");
  else if(sensoronoffMethod == NULL) LOGE("Error: Can't find Java method ()");
  else {
    (*env)->CallVoidMethod(env,x11basicView,sensoronoffMethod,n);
    (*env)->ExceptionClear(env);
  }
}

char *flush_terminal() {
  static char n[256];
  int l=read(out_pipe[0],n,200);
  // if(l>=0) LOGE("read %d bytes.", l);
  if(l>0) {
    while(l>0) {
      n[l]=0;
      gg_outs(n);
      l=read(out_pipe[0],n,200);
    }
  } else *n=0;
  return(n);
}
void thread_function(void *ptr) {
  char *zw;
  /* Programmablaufkontrolle  */
  for(;;) {
    LOGE("Programmlauf start.");
    programmlauf();
    echoflag=batch=0;
    LOGE("Programmlauf stop.");
    fflush(stdout);
    flush_terminal();
    if(isdirectmode) break;
    isdirectmode=1;
    if(daemonf) zw=simple_gets("");
    else zw=do_gets("> ");
    isdirectmode=0;
    if(zw==NULL) break;
    else {
      strcpy(lastcommand,zw);
      kommando(lastcommand);
    }
  }  
  LOGE("Thread done. !");
  invalidate_screen();
}



char lastcommand[MAXSTRLEN]="never used before.";
static char logringbuf[MAXSTRLEN]="";
static int logringbufp=0;

void ringbufin(char *a) {
  if(a) {
    while(*a) {
      logringbuf[logringbufp++]=*a++;
      if(logringbufp>=MAXSTRLEN) logringbufp=0;
    }
    logringbuf[logringbufp]=0;
  }
}
static int ringbufout(char *d,int n) {
  int i=logringbufp;
  if(n==-1) n=MAXSTRLEN;
  while(n>0) {
    if(i==0) i=MAXSTRLEN;
    if(logringbuf[--i]==0) {i++;break;}
    n--;
  }
  if(d) {
    while(1) {
      if(i>=MAXSTRLEN) i=0;
      *d++=logringbuf[i];
      if(logringbuf[i]==0) break;
      i++;
    }
  }
  return n;
}

void backlog(char *n) {
  LOGE("backlog: %s",n);
}


/*Missing functions from missiong libutil*/

/* Copyright (C) 1998 Free Software Foundation, Inc.
     	This file is part of the GNU C Library.
     	Contributed by Zack Weinberg <zack@rabi.phys.columbia.edu>, 1998.

     	The GNU C Library is free software; you can redistribute it and/or
     	modify it under the terms of the GNU Lesser General Public
     	License as published by the Free Software Foundation; either
     	version 2.1 of the License, or (at your option) any later version.

     	The GNU C Library is distributed in the hope that it will be useful,
     	but WITHOUT ANY WARRANTY; without even the implied warranty of
     	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
     	Lesser General Public License for more details.

     	You should have received a copy of the GNU Lesser General Public
     	License along with the GNU C Library; if not, write to the Free
     	Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
     	02111-1307 USA. */
      
      #include <sys/types.h>
      #include <termios.h>
      #include <unistd.h>
      #include <utmp.h>
      
//      libutil_hidden_proto(openpty)
//      libutil_hidden_proto(login_tty)

int openpty (int *amaster, int *aslave, char *name, struct termios *termp,struct winsize *winp) {
  int master, slave;
  char *name_slave;

  master = open("/dev/ptmx", O_RDWR | O_NONBLOCK);
  if (master == -1) {
    backlog("Fail to open master");
    return -1;
  }

  if (grantpt(master))
    goto fail;

  if (unlockpt(master))
    goto fail;

  name_slave = ptsname(master);
  backlog("openpty: slave name %s");
  backlog(name_slave);
  slave = open(name_slave, O_RDWR | O_NOCTTY);
  if (slave == -1)
    {
      goto fail;
    }

  if(termp)
    tcsetattr(slave, TCSAFLUSH, termp);
  if (winp)
    ioctl (slave, TIOCSWINSZ, winp);

  *amaster = master;
  *aslave = slave;
  if (name != NULL)
    strcpy(name, name_slave);

  return 0;

 fail:
  close (master);
  return -1;
}


int login_tty(int fd) {
  (void) setsid();
  #ifdef TIOCSCTTY
  if(ioctl(fd,TIOCSCTTY,(char *)NULL)==-1) return (-1);
  #else
  {
    /* This might work. */
    char *fdname = ttyname (fd);
    int newfd;
    if (fdname)
      {
	if (fd != 0)
      (void) close (0);
	if (fd != 1)
      (void) close (1);
	if (fd != 2)
      (void) close (2);
	newfd = open (fdname, O_RDWR);
	(void) close (newfd);
      }
  }
   #endif
  (void) dup2(fd, 0);
  (void) dup2(fd, 1);
  (void) dup2(fd, 2);
  if(fd>2) (void) close(fd);
  return (0);
}   


int forkpty (int *amaster, char *name, struct termios *termp, struct winsize *winp) {
  int master, slave, pid;
  if(openpty(&master,&slave,name,termp,winp)==-1) return -1;    
  switch(pid=fork ()) {
  case -1:
    return -1;
  case 0:
    /* Child. */
    close (master);
    if(login_tty (slave))  _exit (1);    
    return 0;
  default:
    /* Parent. */
    *amaster = master;
    close (slave);    
    return pid;
  }
}
