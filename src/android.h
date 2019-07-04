/* android.h Kommandos  (c) Markus Hoffmann  */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */


#define  LOG_TAG    "libx11basic"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#define NLOG(a) ringbufin(a)


extern int android_init;
extern int out_pipe[];
extern int in_pipe[];
extern char lastcommand[];



void ringbufin(char *a);
void backlog(char *n);
void android_sigaction(int signal, siginfo_t *info, void *reserved);
void invalidate_screen();
char *flush_terminal();
void thread_function ( void *ptr );
void do_sensoronoff(int n);
void do_gpsonoff(int n);
void ANDROID_beep();
void ANDROID_call_intent(const char *action,char *data, char *extra);
int  ANDROID_waitfor_intentresult();
void ANDROID_init_sound();
void ANDROID_playsoundfile(char *name);
void ANDROID_get_location();
void ANDROID_hidek();
void ANDROID_showk();
