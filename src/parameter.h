/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
#ifndef __parameter__
#define __parameter__

/* Kommandoparametertypen (unsigned char )

xxxx xxxx
0000 xxxx    Gruppe 0: Konstanten
0001 xxxx    Gruppe 1: Variablen
0010 xxxx    Gruppe 2: Labels
0011 xxxx    Gruppe 3: Spezielle

00x0 x+++----Datentyp:
             0 = NOTYP
	     1= INTTYP
	     2= FLOATTYP
	     3=ARBINTTYP
	     4=ARBFLOATTYP
	     5=COMPLEXTYP
	     6=ARBCOMPLEXTYP
	     7=STRINGTYP
     +-------Array Flag: 1=array, 0= no array
   +---------Label Flag: 1=label, 0=sonst
  +----------Variable Flag  1=variable 0=constant
0001 00++----Label-Typ: 0=Label, 1=Proc, 2=Func

TODO: Ordnung reinbringen....
*/

/* Basistypen */

#define PL_BASEMASK   0x0f
#define PL_TYPMASK    0x07
#define PL_GROUPMASK  0x30
#define PL_CONSTGROUP 0x00
#define PL_VARGROUP   0x10
#define PL_LABELGROUP 0x20
#define PL_COMBIGROUP 0x30
#define PL_SPECIALGROUP 0x40

#define PL_LEER     (PL_CONSTGROUP|NOTYP)      /* 0 */
#define PL_INT      (PL_CONSTGROUP|INTTYP)     /* 1 */
#define PL_FLOAT    (PL_CONSTGROUP|FLOATTYP)   /* 2 */
#define PL_ARBINT   (PL_CONSTGROUP|ARBINTTYP)  /* 3 */
#define PL_ARBFLOAT (PL_CONSTGROUP|ARBFLOATTYP) /* 4 */
#define PL_COMPLEX  (PL_CONSTGROUP|COMPLEXTYP) /* 5 */
#define PL_ARBCOMPLEX  (PL_CONSTGROUP|ARBCOMPLEXTYP) /* 6 */
#define PL_STRING   (PL_CONSTGROUP|STRINGTYP)  /* 7 */
/* 6 */
/* 7 */
#define PL_ARRAY    (PL_CONSTGROUP|ARRAYTYP)       /* 8 */
#define PL_IARRAY   (PL_ARRAY|PL_INT)          /* 9 */
#define PL_FARRAY   (PL_ARRAY|PL_FLOAT)        /* a */
#define PL_AIARRAY  (PL_ARRAY|PL_ARBINT)       /* b */
#define PL_CARRAY   (PL_ARRAY|PL_COMPLEX)      /* d */
#define PL_SARRAY   (PL_ARRAY|PL_STRING)       /* f */
/* e */
/* f */
/* 10 */
#define PL_IVAR     (PL_VARGROUP|INTTYP)       /* 11 */
#define PL_FVAR     (PL_VARGROUP|FLOATTYP)     /* 12 */
#define PL_AIVAR    (PL_VARGROUP|ARBINTTYP)    /* 13 */
#define PL_CVAR     (PL_VARGROUP|COMPLEXTYP)   /* 15 */
#define PL_SVAR     (PL_VARGROUP|STRINGTYP)    /* 17 */
/* 16 */
/* 17 */
#define PL_ARRAYVAR  (PL_VARGROUP|PL_ARRAY)   /* 0x18 */
#define PL_IARRAYVAR (PL_VARGROUP|PL_IARRAY)  /* 0x19 */
#define PL_FARRAYVAR (PL_VARGROUP|PL_FARRAY)  /* 0x1a */
#define PL_AIARRAYVAR (PL_VARGROUP|PL_AIARRAY)  /* 0x1b */
#define PL_CARRAYVAR (PL_VARGROUP|PL_CARRAY)  /* 0x1d */
#define PL_SARRAYVAR (PL_VARGROUP|PL_SARRAY)  /* 0x1f */
/* 1e */
/* 1f */
#define PL_LABEL    (PL_LABELGROUP|0x00) /* 0x20 */
#define PL_PROC     (PL_LABELGROUP|0x01) /* 0x21 */
#define PL_FUNC     (PL_LABELGROUP|0x02) /* 0x22 */

/* Kombinierte Parameter-Arten */

#define PL_ANY      (PL_COMBIGROUP|0x0)   /* 0x30 anything, typ wird bestimmt, kann float,int,string, array sein*/
#define PL_ANYVALUE (PL_COMBIGROUP|0x1)   /* 0x31 alles ausser array*/
#define PL_NUMBER   (PL_COMBIGROUP|0x2)   /* 0x32   = PL_INT or PL_FLOAT or PL_COMPLEX or PL_ARBINT */
#define PL_CFAI     (PL_COMBIGROUP|0x3)   /* 0x33   = PL_FLOAT or PL_COMPLEX  or PL_ARBINT  */
#define PL_CF       (PL_COMBIGROUP|0x4)   /* 0x34   = PL_FLOAT or PL_COMPLEX */

#define PL_NVAR     (PL_COMBIGROUP|0x8)   /* 0x38 */
#define PL_VAR      (PL_COMBIGROUP|0x9)   /* 0x39 */
#define PL_ALLVAR   (PL_COMBIGROUP|0xa)   /* 0x3a */
#define PL_NARRAY   (PL_COMBIGROUP|0xb)   /* 0x3b */
#define PL_CFARRAY  (PL_COMBIGROUP|0xc)   /* 0x3c */

/* Weitere Parameter-Arten */

#define PL_SPECIAL   (PL_SPECIALGROUP|0x0)   /* 0x40  Typ wird noch aufgeloest B:W:D:V:*/
#define PL_BYTECODE  (PL_SPECIALGROUP|0x1)   /* 0x41 Zeigt auf auszufuehrenden Bytecode*/
#define PL_EVAL      (PL_SPECIALGROUP|0x2)   /* 0x42 */
#define PL_FILENR    (PL_SPECIALGROUP|0x3)   /* 0x43 */
#define PL_PRINTLIST (PL_SPECIALGROUP|0x4)   /* 0x44 anytyp mit delimiter*/
#define PL_KEY       (PL_SPECIALGROUP|0x5)   /* 0x45 */



/* abgeleitete Zusatztypen */

#define PL_VALUE      PL_ANYVALUE
#define PL_ANYVAR     PL_ALLVAR
#define PL_ANYARRAY   PL_ARRAY


/*Prototypen */
int count_parameters(const char *n);
void free_parameter(PARAMETER *p);
PARAMETER double_parameter(PARAMETER *p);
void dump_parameterlist(PARAMETER *p, int n);

int prepare_vvar(char *w1,PARAMETER *p, unsigned int solltyp);

int make_parameter_stage2(char *n,unsigned short ap,PARAMETER *pret);
int make_parameter_stage3(PARAMETER *pin,unsigned short ap,PARAMETER *pret);
int make_pliste(int pmin,int pmax,unsigned short *pliste,char *n, PARAMETER **pr);
int make_pliste2(int pmin,int pmax,unsigned short *pliste,char *n, PARAMETER **pr,int ii);
int make_pliste3(int pmin,int pmax,unsigned short *pliste,PARAMETER *pin, PARAMETER **pout,int ii);

int make_preparlist(PARAMETER *p,char *n);
int get_indexliste(PARAMETER *p,int *l,int n);

/*kleinere Hilfsfunktionen als inline Makros*/

static inline void free_pliste(int anz,PARAMETER *p) {
  while(anz>0)  free_parameter(&p[--anz]);
  free(p);
}
static inline int p2int(PARAMETER *p) {
  switch(p->typ) {
  case PL_FILENR:
  case PL_INT:     return(p->integer);
  case PL_FLOAT:
  case PL_COMPLEX: return((int)p->real);
  case PL_ARBINT:  return(mpz_get_si(*(ARBINT *)p->pointer));
  default: xberror(46,""); /*  Parameter %s falsch, keine Number */
  }
  return(0);
}
static inline double p2float(PARAMETER *p) {
  switch(p->typ) {
  case PL_INT:     return((double)p->integer);
  case PL_FLOAT:
  case PL_COMPLEX: return(p->real);
  case PL_ARBINT:  return(mpz_get_d(*(ARBINT *)p->pointer));
  default: xberror(46,""); /*  Parameter %s falsch, keine Number */
  }
  return(0);
}
static inline COMPLEX p2complex(PARAMETER *p) {
  switch(p->typ) {
  case PL_INT:     p->real=(double)p->integer; 
  case PL_FLOAT:   p->imag=0;
  case PL_COMPLEX: break;
  case PL_ARBINT:  p->real=mpz_get_d(*(ARBINT *)p->pointer);p->imag=0;break;
  default: xberror(46,""); /*  Parameter %s falsch, keine Number */
  }
  return(*(COMPLEX *)(&p->real));
}
static inline STRING p2string(PARAMETER *p) {
  if(p->typ!=PL_STRING) {
    xberror(47,""); /*  Parameter %s falsch, kein String */
    return(create_string("<ERROR>"));
  }
  return(double_string((STRING *)(&p->integer)));
}

static inline void p2arbint(PARAMETER *p,ARBINT a) {
  switch(p->typ) {
  case PL_INT:     mpz_set_si(a,p->integer);break; 
  case PL_FLOAT:   
  case PL_COMPLEX: mpz_set_d(a,p->real);break;
  case PL_ARBINT:  mpz_set(a,*(ARBINT *)p->pointer);break;
  default: xberror(46,""); /*  Parameter %s falsch, keine Number */
  }
}

inline static void cast_to_real(PARAMETER *sp) {
  if(sp->typ==PL_LEER||sp->typ==PL_FLOAT) return;
  sp->real=p2float(sp);
  free_parameter(sp);
  sp->typ=PL_FLOAT;
}
inline static void cast_to_complex(PARAMETER *sp) {
  if(sp->typ==PL_LEER||sp->typ==PL_COMPLEX) return;
  *(COMPLEX *)&(sp->real)=p2complex(sp);
  free_parameter(sp);
  sp->typ=PL_COMPLEX;
}
inline static void cast_to_int(PARAMETER *sp) {
  if(sp->typ==PL_LEER||sp->typ==PL_INT) return;
  sp->integer=p2int(sp);
  free_parameter(sp);
  sp->typ=PL_INT;
}
inline static void cast_to_arbint(PARAMETER *sp) {
  if(sp->typ==PL_LEER ||sp->typ==PL_ARBINT) return;
  ARBINT a;
  mpz_init(a);
  p2arbint(sp,a);
  free_parameter(sp);
  sp->pointer=malloc(sizeof(ARBINT));
  mpz_init(*(ARBINT *)sp->pointer);
  mpz_set(*(ARBINT *)sp->pointer,a);
  sp->typ=PL_ARBINT;
  mpz_clear(a);
}
inline static void cast_to_x(PARAMETER *sp,unsigned int x) {
  if(sp->typ!=x) {
    switch(x) {
      case PL_INT:     cast_to_int(sp);     break;
      case PL_FLOAT:   cast_to_real(sp);    break;
      case PL_COMPLEX: cast_to_complex(sp); break;
      case PL_ARBINT:  cast_to_arbint(sp);  break;
      case PL_ARBFLOAT:cast_to_real(sp);    break;  /*TODO */
      default: printf("ERROR: cast2x: something is wrong. typ=%x/%x\n",sp->typ,x);
    }
  }
}







/* Evaluate generic Expression and fill PARAMETER accordingly.
 * typ of the expression is already known and given. (faster)
 * It is expected, that pret is fresh (no pointers already in use).
 * In case of an error, pred will be unchanged.
 * Return value: 
 * 0 -- OK, 
 * -1 -- ERROR 
 */

static inline int eval2partype(char *n, PARAMETER *pret,int typ) {
  typ&=(~CONSTTYP);
  if((typ&FILENRTYP)==FILENRTYP) typ=INTTYP;
  pret->typ=(PL_CONSTGROUP|typ);
  switch(typ&TYPMASK) {
  case INTTYP:
    pret->integer=(int)parser(n);
    break;
  case FLOATTYP:
    pret->real=parser(n);
    break;
  case COMPLEXTYP:
    *(COMPLEX *)&(pret->real)=complex_parser(n);
    break;
  case ARBINTTYP: 
    pret->pointer=malloc(sizeof(ARBINT));
    mpz_init(*((ARBINT *)pret->pointer));
    arbint_parser(n,*((ARBINT *)pret->pointer));
    break;
  case STRINGTYP: 
    *(STRING *)(&pret->integer)=string_parser(n);
    break;
  default:   /*heben wir uns für spaeter auf...*/
    pret->typ=PL_EVAL;
    *((STRING *)&(pret->integer))=create_string(n);
    pret->arraytyp=typ;
  }
  return(0); /* no error */
}

/* Evaluate generic Expression and fill PARAMETER accordingly.
 * The type of the expression will be determined (slow).
 * It is expected, that pret is fresh (no pointers already in use).
 * In case of an error, pred will be unchanged.
 * Return value: 
 * 0 -- OK, 
 * -1 -- ERROR 
 */

static inline int eval2par(char *n, PARAMETER *pret) {
  int typ=type(n);
  return(eval2partype(n,pret,typ));
}



/* Evaluate numeric Expression and fill PARAMETER accordingly.
 * typ of the expression is already known and given. (faster)
 * It is expected, that pret is fresh (no pointers already in use).
 * In case of an error, pred will be unchanged.
 * Return value: 
 * 0 -- OK, 
 * -1 -- ERROR 
 */

static inline int eval2parnumtype(char *n, PARAMETER *pret,int typ) {
  typ&=(~CONSTTYP);
  pret->typ=(PL_CONSTGROUP|typ);
  switch(typ&TYPMASK) {
  case INTTYP:
    pret->integer=(int)parser(n);
    return(0);
  case FLOATTYP:
    pret->real=parser(n);
    return(0);
  case COMPLEXTYP:
    *(COMPLEX *)&(pret->real)=complex_parser(n);
    return(0);
  case ARBFLOATTYP: /*behandeln wir erstmal wie ARBINT*/
    pret->typ=(PL_CONSTGROUP|ARBINTTYP);
  case ARBINTTYP: 
    pret->pointer=malloc(sizeof(ARBINT));
    mpz_init(*((ARBINT *)pret->pointer));
    arbint_parser(n,*((ARBINT *)pret->pointer));
    return(0);
  default: 
    // printf("ERROR at line %d: eval2parnumtype: parameter incompatibel in <%s>. typ=$%x\n",pc,n,typ);
    return(-1);
  }
}

/* Evaluate numeric Expression and fill PARAMETER accordingly.
 * The type of the expression will be determined (slow).
 * It is expected, that pret is fresh (no pointers already in use).
 * In case of an error, pred will be unchanged.
 * Return value: 
 * 0 -- OK, 
 * -1 -- ERROR 
 */


static inline int eval2parnum(char *n, PARAMETER *pret) {
  int typ=type(n);
  return(eval2parnumtype(n,pret,typ));
}
/* Erstellt eine Liste mit Parametertypen aus parameterliste einer 
   Procedur. Es können normale Parameter (PL_CONSTGROUP) erzeugt werden 
   oder auch VAR parameter (PL_VARGROUP) 
   */

static inline void get_ptypliste(int procnr,unsigned short *ptyp, int n) {
  int j,vnr,typ;
  if(n>0) {
    for(j=0;j<n;j++) {
      vnr=(procs[procnr].parameterliste[j]&(~V_BY_REFERENCE));
      typ=variablen[vnr].typ;
      if(typ==ARRAYTYP) typ|=variablen[vnr].pointer.a->typ;
       //   printf("PROC: %d: vnr=%d, typ=%x\n",j,vnr,typ);
      if((procs[procnr].parameterliste[j]&V_BY_REFERENCE)==V_BY_REFERENCE) {
        ptyp[j]=(PL_VARGROUP|typ);
      } else ptyp[j]=(PL_CONSTGROUP|typ);
    }
  }
}


#endif

