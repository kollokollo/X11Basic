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
extern AndroidBitmapInfo  screen_info;
extern int out_pipe[];
extern int in_pipe[];
extern char lastcommand[];



void ringbufin(char *a);
void backlog(char *n);
void android_sigaction(int signal, siginfo_t *info, void *reserved);
void invalidate_screen();
char *flush_terminal();
void thread_function ( void *ptr );
