/* This file is part of X11BASIC, the basic interpreter for Unix
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
int wort_sep (char *t,char c,int klamb ,char *w1, char *w2);
int wort_sep_multi(char *t,char *c, int klamb ,char *w1, char *w2);
int wort_sep_e(char *t,char c,int klamb ,char *w1, char *w2);
int wort_sepr_e(char *t,char c,int klamb ,char *w1, char *w2);
int wort_sepr(char *t,char c,int klamb ,char *w1, char *w2);
int wort_sep2(char *t,char *c,int klamb ,char *w1, char *w2);
int wort_sepr2(char *t,char *c,int klamb ,char *w1, char *w2);
int arg2(char *t,int klamb ,char *w1, char *w2);
char *searchchr(char *buf, char c);
char *searchchr2(char *buf, char c);
char *searchchr2_multi(char *buf, char *c);
char *rsearchchr(char *buf, char c);
char *rsearchchr2(char *start,char c,char *end);
void *memmem(const void *buf,  size_t buflen,const void *pattern, size_t len);
char *rmemmem(char *s1,int len1,char *s2,int len2);
void xtrim(char *t,int f, char *w);
void xtrim2(char *t,int f, char *w);
