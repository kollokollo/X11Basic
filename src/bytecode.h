/* BYTECODE.H (c) Markus Hoffmann  */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#define BC_STACKLEN 256

#define BC_VERSION 0x1271   /* Version 1.27 release 1*/


/* File heade for bytecode */

typedef struct {
  unsigned char BRAs;       /* DC_BRAs */
  unsigned char offs;       /* sizeof(BYTECODE_HEADER)-2*/
  unsigned short rsv;       /*  not used */
  uint32_t textseglen; /* size of the text/code segment */
  uint32_t rodataseglen; /* size of the readonly data segment */
  uint32_t sdataseglen; /* size of the data segment for DATA statements*/
  uint32_t dataseglen;   /* size of the data segment */
  uint32_t bssseglen;    /* storage space for variables */
  uint32_t symbolseglen; /* size of the symbol table segment */
  uint32_t stringseglen; /* size of the string segment */
  uint32_t relseglen;    /* size of the relocation data */
  unsigned short flags;       /* not used */
  unsigned short version;     /* version */
} BYTECODE_HEADER;

#ifdef ATARI
#define MAX_CODE 16000
#else
#define MAX_CODE 256000
#endif
#define EXE_REL 1     /* If relocation is needed */

typedef struct {
  unsigned char typ;      /* Typ of the symbol */
  unsigned char subtyp;
  unsigned short rsv;       /*  not used */
  uint32_t name;      /* Pointer to Symbol string table */
  uint32_t adr;       /* address */
} BYTECODE_SYMBOL;

typedef struct {
  unsigned short bc_version;    /* Bytecode version used */
  unsigned short status;        /* 0 fresh, 1 dont free segments */
  unsigned short comp_flags;    /* flags for compilation */
  char *textseg;                /* Memory for text segment */
  unsigned int  textseglen;     /* size of the text/code segment */
  char *rodataseg;              /* Memory for readonly data segment */
  unsigned int rodataseglen;    /* size of the readonly data segment */
  char *sdataseg;               /* Memory for data segment  for DATA statements*/
  unsigned int sdataseglen;     /* size of the data segment for DATA statements*/
  char *dataseg;                /* Memory for data segment */
  unsigned int dataseglen;      /* size of the data segment */
  
  char *stringseg;
  unsigned int stringseglen;    /* size of the string segment */
  char *bsseg;               /* Memory for block starage segment */
  unsigned int bsseglen;    
  BYTECODE_SYMBOL *symtab;
  unsigned int anzsymbols; 
  int *reltab;
  unsigned int relseglen;       /* in bytes. */
  int anzreloc;   
  int *bc_index;
  int prglen;
  
  int last_proc_number;    /*internal: proc number of the last PROCEDURE/FUNCTION statement */
  int expected_typsp;      /*internal: Expected type stack position.*/
} COMPILE_BLOCK;


/* Symbol types */

#define STT_NOTYPE  0
#define STT_OBJECT  1
#define STT_FUNC    2
#define STT_LABEL   3
#define STT_DATAPTR 4


/* Bitmuster fuer die Kommandos, so codiert, dass die anzahl der folgenden
   parameter-bytes mit drin codiert ist:
   
   00000000
         ^^----0,1,2,4 folgende Bytes
*/
/* Gruppe 0: Ausnahmen und Interrupts */

#define BC_BRK     0x00   /* 00000000  --- BRK Breakpoint, this will cause illegal instruction error */
#define BC_INT1    0x01   /* 00000001  --- Interrupt 1 */
#define BC_INT2    0x02   /* 00000010  --- Interrupt 2 */
#define BC_INT3    0x03   /* 00000011  --- Interrupt 3 */
#define BC_INT4    0x04   /* 00000100  --- Interrupt 4 */
#define BC_INT5    0x05   /* 00000101  --- Interrupt 5 */
#define BC_INT6    0x06   /* 00000110  --- Interrupt 6 */
#define BC_INT7    0x07   /* 00000111  --- Interrupt 7 */

#define BC_TRAP0   0x08   /* 00001000  --- TRAP0 */
#define BC_TRAP1   0x09   /* 00001001  --- TRAP1 */
#define BC_TRAP2   0x0a   /* 00001010  --- TRAP2 */
#define BC_TRAP3   0x0b   /* 00001011  --- TRAP3 */
#define BC_TRAP4   0x0c   /* 00001100  --- TRAP4 */
#define BC_TRAP5   0x0d   /* 00001101  --- TRAP5 */
#define BC_TRAP6   0x0e   /* 00001110  --- TRAP6 */
#define BC_TRAP7   0x0f   /* 00001111  --- TRAP7 */

/* Gruppe 1: Programmlauf-Kontrolle */

#define BC_HALT    0x10 /*   00010000  --- HLT Halt */
#define BC_BRAs    0x11 /*   00010001  --- BRA.s  very short Branch */
#define BC_BRA     0x12 /*   00010010  --- BRA short Branch */
#define BC_JMP     0x13 /*   00010011  --- JMP Long Jump */
#define BC_RTS     0x14 /*   00010100  --- RTS Return from Subroutine */
                        /*   00010101  --- */
#define BC_BSR     0x16 /*   00010110  --- BSR Branch to Subroutine */
#define BC_JSR     0x17 /*   00010111  --- JSR jump to Subroutine */
#define BC_BLKSTART 0x18 /*   00011000  --- Save context */
#define BC_BEQs    0x19 /*   00011001  --- BEQ.s short Branch if 0 */
#define BC_BEQ     0x1a /*   00011010  --- BEQ   short Branch if 0 */
#define BC_JEQ     0x1b /*   00011011  --- JEQ jump if 0 */
#define BC_BLKEND  0x1c /*   00011100  --- Restore context */
                        /*   00011101  ---  */
                        /*   00011110  ---  */
#define BC_RESTORE 0x1f /*   00011111  --- RESTORE Restore datapointer */

/* Gruppe 2: Operand Stack Manipulation Operators 0 in 1 out */

#define BC_PUSHLEER 0x20 /*     --- PUSHLEER Push an empty parameter */

#define BC_PUSHCOMM 0x22 /*     --- push command  */

#define BC_COUNT    0x24 /*     --- COUNT the number of elements on stack */
#define BC_PUSHSYS  0x25 /*     --- PUSHSYS push system variable */
#define BC_PUSHFUNC 0x26 /*     --- push function */
#define BC_PUSHPROC 0x27 /*     --- push procedure label */
#define BC_PUSHK    0x28 /*     ---  Push a keyword by index */
#define BC_PUSHSSYS 0x29 /*     --- PUSHSSYS push system string variable */
#define BC_PUSHSFUNC 0x2a /*    --- push string function */
#define BC_PUSHLABEL 0x2b /*    --- push a label  */
#define BC_PUSHX     0x2c /*    ---  Push a keyword */
#define BC_PUSHASYS 0x2d /*     --- PUSHASYS */
#define BC_PUSHAFUNC 0x2e /*    --- push array function */
#define BC_PUSHA     0x2f /*    --- push an array constant */

/* Gruppe 3: Operand Stack Manipulation Operators 0 in 1 out */

#define BC_PUSH0    0x30 /*   ---  PUSH0 push a zero (int 0) */
#define BC_PUSH1    0x31 /*   ---  Push a int 1 */
#define BC_PUSH2    0x32 /*   ---  Push a int 2 */
#define BC_PUSHM1   0x33 /*   ---  Push a int -1 */
#define BC_PUSHB    0x34 /*   ---  Push a byte int constant */
#define BC_PUSHW    0x35 /*   ---  Push a short int */

#define BC_PUSHI    0x39 /*   ---  Push an int constant  */
#define BC_PUSHF    0x3a /*   ---  Push a double constant */
#define BC_PUSHAI   0x3b /*   ---  Push a big int constant */
#define BC_PUSHAF   0x3c /*   ---  Push a big float constant */
#define BC_PUSHC    0x3d /*   ---  Push a complex constant */
#define BC_PUSHAC   0x3e /*   ---  Push a big complex constant */
#define BC_PUSHS    0x3f /*   ---  Push a string constant */



/* Gruppe 4: Operatoren (unitaer 1 in 1 out) */
#define BC_NOT      0x40 /*     --- NOT big int*/
#define BC_NOTi     0x41 /*     --- NOT int */
#define BC_NEG      0x44 /*     --- NEG */

#define BC_I2FILE   0x46 /*     --- cast int --> file#  */
#define BC_F2C      0x47 /*     --- cast  float --> complex */
#define BC_I2F      0x48 /*     --- cast int --> float  */
#define BC_X2I      0x49 /*     --- cast to int  */
#define BC_X2F      0x4a /*     --- cast to float */
#define BC_X2AI     0x4b /*     --- cast to big int  */
#define BC_X2AF     0x4c /*     --- cast to big float */
#define BC_X2C      0x4d /*     --- cast to complex */
#define BC_X2AC     0x4e /*     --- cast to big complex */



/* Gruppe 5: Operatoren (unitaer 1 in 2 out) */
#define BC_DUP      0x50 /*   01010000  --- DUP */
                         /*   01010100  ---  */
                         /*   01011000  ---  */
                         /*   01011100  ---  */
/* Gruppe 6: Operatoren (binaer 2 in 1 out) Arithmetic and Math Operators  */
#define BC_ADD      0x60 /*      --- ADD */
#define BC_ADDi     0x61 /*      --- ADD int */
#define BC_ADDf     0x62 /*      --- ADD floats */
#define BC_ADDc     0x63 /*      --- ADD complexs */
#define BC_ADDs     0x64 /*      --- ADD strings */
#define BC_SUB      0x65 /*      --- SUB */
#define BC_SUBi     0x66 /*      --- SUB int */
#define BC_SUBf     0x67 /*      --- SUB floats */
#define BC_SUBc     0x68 /*      --- SUB complexs */
#define BC_MUL      0x69 /*      --- MUL */
#define BC_MULi     0x6a /*      --- MUL int */
#define BC_MULf     0x6b /*      --- MUL float */
#define BC_MULc     0x6c /*      --- MUL complexs */
#define BC_DIV      0x6d /*      --- DIV */
#define BC_DIVf     0x6e /*      --- DIV floats*/
#define BC_DIVc     0x6f /*      --- DIV complexs*/

/* Gruppe 7: Operatoren (binaeout)  Arithmetic and Math Operators */
#define BC_AND      0x70 /*     --- AND big int*/
#define BC_ANDi     0x71 /*     --- AND int */
#define BC_OR       0x74 /*     --- OR */
#define BC_ORi      0x75 /*     --- OR int */
#define BC_POW      0x78 /*     --- POW */
#define BC_MOD      0x7c /*     --- MOD */

/* Gruppe 8: Operatoren (binaer 2 in 1 out) Logic Math Operators */
#define BC_XOR      0x80 /*      --- XOR big int */
#define BC_XORi     0x81 /*      --- XOR int */
#define BC_EQUAL    0x84 /*      --- EQUAL */
#define BC_LESS     0x88 /*      --- LESS */
#define BC_GREATER  0x8c /*      --- GREATER */
/* Gruppe 9: Operatoren (2 in 2 out) */
#define BC_EXCH     0x90 /*    10010000  --- EXCHG   */

/* Gruppe 10: Operatoren (x in 0 out)   */
#define BC_POP      0xa0 /*    10100000  --- POP */
#define BC_CLEAR    0xa4 /*    10100100  --- CLEAR remove all elements from stack*/
#define BC_LOCAL    0xa6 /*    10100110  --- LOCAL */
#define BC_NOOP     0xa8 /*    10101000  --- NOOP */

#define BC_ZUWEISc  0xab /*    10101011  --- ZUWEIS complex */
#define BC_ZUWEISf  0xac /*    10101100  --- ZUWEIS float */
#define BC_ZUWEISi  0xad /*    10101101  --- ZUWEIS integer */
#define BC_ZUWEIS   0xae /*    10101110  --- ZUWEIS */
#define BC_COMMENT  0xaf /*    10101111  --- Comment */
   
/* Gruppe 11: Variablen Management */
#define BC_PUSHVVI    0xb0 /*    10110000  --- */
#define BC_ZUWEISINDEX 0xb3 /*    10110011  --- */
#define BC_EVAL     0xb4 /*    10110100  --- EVAL */
#define BC_PUSHV    0xb8 /*    10111000  --- PUSHV */
#define BC_PUSHVV   0xbc /*    10111100  --- PUSHVV */
#define BC_PUSHARRAYELEM 0xbf /* 10111111  --- PUSHARRAYELEM */

/* Gruppe 12: Memory Management */
#define BC_LOADi    0xc3 /*    11000011  --- push integer from address*/
#define BC_LOADf    0xc7 /*    11000111  --- push float from address */
#define BC_LOADc    0xc8 /*    11001000  --- push complex from address */
#define BC_LOADs    0xcb /*    11001011  --- push string from address */
#define BC_LOADa    0xcf /*    11001111  --- push array from address */

/* Gruppe 13: Memory Management */
#define BC_SAVEi    0xd3 /*    11010011  --- store integer from address*/
#define BC_SAVEf    0xd7 /*    11010111  --- store float from address */
#define BC_SAVEc    0xd8 /*    11011000  --- store complex from address */
#define BC_SAVEs    0xdb /*    11011011  --- store string from address */
#define BC_SAVEa    0xdf /*    11011111  --- store array from address */

/* Gruppe 14: Frei */
#define BC_PUSHDIMARGI    0xe0 /*    11100000  --- */

/* Gruppe 15: Frei */

#define BC_FALSE  BC_PUSH0     /* Push a logical FALSE */
#define BC_TRUE   BC_PUSHM1     /* Push a logical TRUE */

//#define BC_PUSHUSERFUNC '@'
//#define BC_COPY  'C'
//#define BC_INDEX 'I'     /* Duplicate arbitrary element */
//#define BC_ROLL  'R'     /* Roll n elements up j times */

/* Interpretierungen */

//#define BC_LINE        1




#ifdef ATARI
#define CP4(d,s,l) { register char *dd=(char *)(d); \
                   register char *ss=(char *)(s)+3; \
                 *dd++=*ss--; \
                 *dd++=*ss--; \
                 *dd++=*ss--; \
                 *dd=*ss; l+=4;}
#define CP2(d,s,l) { register char *dd_=(char *)(d); \
                   register char *ss_=(char *)(s)+1; \
                 *dd_++=*ss_--; \
                 *dd_=*ss_; l+=2;}

#define CP8(d,s,l) { register char *dd=(char *)(d); \
                   register char *ss=(char *)(s)+7; \
                 *dd++=*ss--; \
                 *dd++=*ss--; \
                 *dd++=*ss--; \
                 *dd++=*ss--; \
                 *dd++=*ss--; \
                 *dd++=*ss--; \
                 *dd++=*ss--; \
                 *dd=*ss; l+=8;}

#else
#define CP4(d,s,l) { register char *dd=(char *)(d); \
                   register char *ss=(char *)(s); \
                 *dd++=*ss++; \
                 *dd++=*ss++; \
                 *dd++=*ss++; \
                 *dd=*ss; l+=4;}
#define CP2(d,s,l) { register char *dd_=(char *)(d); \
                   register char *ss_=(char *)(s); \
                 *dd_++=*ss_++; \
                 *dd_=*ss_; l+=2;}
#define CP8(d,s,l) { register char *dd=(char *)(d); \
                   register char *ss=(char *)(s); \
                 *dd++=*ss++; \
                 *dd++=*ss++; \
                 *dd++=*ss++; \
                 *dd++=*ss++; \
                 *dd++=*ss++; \
                 *dd++=*ss++; \
                 *dd++=*ss++; \
                 *dd=*ss; l+=8;}
#endif


/* Compile flags */

#define COMPILE_DEFAULT  0
#define COMPILE_NOOPS    1
#define COMPILE_COMMENTS 2
#define COMPILE_VERBOSE  8


/* Protos */
int bytecode_make_bss(BYTECODE_HEADER *bytecode,char **,int );
int compile(COMPILE_BLOCK *cb,int );
int bc_parser(COMPILE_BLOCK *cb,const char *funktion);
int add_rodata(COMPILE_BLOCK *cb,char *data,int len);
int fix_bytecode_header(BYTECODE_HEADER *bytecode);
void free_cb(COMPILE_BLOCK *cb);
COMPILE_BLOCK *bytecode_init(char *adr);
int save_bytecode(const char *name,COMPILE_BLOCK *cb,int);

#if 0
int vm_add(PARAMETER *sp);
int vm_sub(PARAMETER *sp);
int vm_mul(PARAMETER *sp);
int vm_pow(PARAMETER *sp);
int vm_mod(PARAMETER *sp);
int vm_equal(PARAMETER *sp);
int vm_greater(PARAMETER *sp);
int vm_less(PARAMETER *sp);
int vm_sysvar(PARAMETER *sp,int n);
int vm_ssysvar(PARAMETER *sp,int n);
int vm_asysvar(PARAMETER *sp,int n);
int vm_dup(PARAMETER *sp);
int vm_exch(PARAMETER *sp);
int vm_neg(PARAMETER *sp);

void cast_to_real(PARAMETER *sp);
void cast_to_int(PARAMETER *sp);
int vm_sfunc(PARAMETER *sp,int i, int anzarg);
int vm_afunc(PARAMETER *sp,int i, int anzarg);
int vm_func(PARAMETER *sp,int i, int anzarg);
int vm_comm(PARAMETER *sp,int i, int anzarg);
int vm_pushvv(int vnr,PARAMETER *sp);
int vm_zuweis(int vnr,PARAMETER *sp);
void zuweis_v_parameter(VARIABLE *v,PARAMETER *p);
int vm_zuweisindex(int vnr,PARAMETER *sp,int dim);
int vm_pusharrayelem(int vnr,PARAMETER *sp, int dim);
int vm_pushv(int vnr,PARAMETER *sp);
void  push_v(PARAMETER *p, VARIABLE *v);
int vm_eval(PARAMETER *sp);
#endif
