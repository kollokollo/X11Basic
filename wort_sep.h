/* This file is part of X11BASIC, the basic interpreter for Unix
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
int wort_sep   (const char *t,char c,int klamb ,char *w1, char *w2);
int wort_sep_destroy(char *t,char c,int klamb ,char **w1, char **w2);
int wort_sep_multi(const char *t,char *c, int klamb ,char *w1, char *w2);
int wort_sep_e (const char *t,char c,int klamb ,char *w1, char *w2);
int wort_sepr_e(const char *t,char c,int klamb ,char *w1, char *w2);
int wort_sepr  (const char *t,char c,int klamb ,char *w1, char *w2);
int wort_sep2  (const char *t,char *c,int klamb ,char *w1, char *w2);
int wort_sepr2 (const char *t,char *c,int klamb ,char *w1, char *w2);
int arg2       (const char *t,int klamb ,char *w1, char *w2);
char *searchchr(const char *buf, char c);
char *searchchr2(const char *buf, char c);
char *searchchr3(const char *buf, char c);
char *searchchr2_multi(const char *buf, const char *c);
char *rsearchchr(const char *buf, char c);
char *rsearchchr2(const char *start,char c,const char *end);
void *memmem(const void *buf,  size_t buflen,const void *pattern, size_t len);
char *rmemmem(char *s1,int len1,char *s2,int len2);
void xtrim(const char *t,int f, char *w);
void xtrim2(const char *t,int f, char *w);
int klammer_sep(const char *t,char *w1, char *w2);
int klammer_sep_destroy(char *t,char **w1, char **w2);
