/* BYTECODE.H (c) Markus Hoffmann  */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#define BC_STACKLEN 256

#define BC_VERSION 0x1203   /* Version 1.20 release 1*/

typedef struct {
  unsigned char BRAs;       /* DC_BRAs */
  unsigned char offs;       /* sizeof(BYTECODE_HEADER)-2*/
  unsigned long textseglen; /* size of the text/code segment */
  unsigned long rodataseglen; /* size of the readonly data segment */
  unsigned long sdataseglen; /* size of the data segment for DATA statements*/
  unsigned long dataseglen;   /* size of the data segment */
  unsigned long bssseglen;    /* storage space for variables */
  unsigned long symbolseglen; /* size of the symbol table segment */
  unsigned long stringseglen; /* size of the string segment */
  unsigned long relseglen;    /* size of the relocation data */
  unsigned short flags;       /* not used */
  unsigned short version;     /* version, currently=0x117 */
} BYTECODE_HEADER;


#define EXE_REL 1     /* If relocation is needed */

typedef struct {
  unsigned char typ;      /* Typ of the symbol */
  unsigned char subtyp;
  unsigned long name;      /* Pointer to Symbol string table */
  unsigned long adr;       /* address */
} BYTECODE_SYMBOL;

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

#define BC_PUSHLEER 0x20 /*   00100000  --- PUSHLEER Push an empty parameter */
#define BC_PUSHB    0x21 /*   00100001  --- PUSHB */
#define BC_PUSHCOMM 0x22 /*   00100010  --- push command  */
#define BC_PUSHI    0x23 /*   00100011  --- PUSHI Push an int constant  */
#define BC_COUNT    0x24 /*   00100100  --- COUNT the number of elements on stack */
#define BC_PUSHSYS  0x25 /*   00100101  --- PUSHSYS push system variable */
#define BC_PUSHFUNC 0x26 /*   00100110  --- push function */
                         /*   00100111  ---  */
#define BC_PUSH0    0x28 /*   00101000  --- PUSH0 push a zero (int 0) */
#define BC_PUSHSSYS 0x29 /*   00101001  --- PUSHSSYS push system string variable */
#define BC_PUSHSFUNC 0x2a /*  00101010  --- push string function */
                         /*   00101011  ---  */
#define BC_PUSH1    0x2c /*   00101100  --- Push a int 1 */
#define BC_PUSHASYS 0x2d /*   00101101  --- PUSHASYS */
#define BC_PUSHAFUNC 0x2e /*  00101110  --- push array function */
#define BC_PUSHA     0x2f /*  00101111  --- push an array constant */

/* Gruppe 3: Operand Stack Manipulation Operators 0 in 1 out */
#define BC_PUSH2    0x30 /*   00110000  ---  Push a int 2 */
#define BC_PUSHW    0x32 /*   00110010  ---  Push a short int */
#define BC_PUSHX    0x33 /*   00110011  ---  Push a keyword */
#define BC_PUSHM1   0x34 /*   00110100  ---  Push a int -1 */
#define BC_PUSHF    0x37 /*   00110111  ---  Push a double constant */
#define BC_PUSHS    0x3f /*   00111111  ---  Push a string constant */



/* Gruppe 4: Operatoren (unitaer 1 in 1 out) */
#define BC_NOT      0x40 /*   01000000  --- NOT */
#define BC_NEG      0x44 /*   01000100  --- NEG */
#define BC_X2I      0x48 /*   01001000  --- cast to int  */
#define BC_X2F      0x4c /*   01001100  --- cast to float */
/* Gruppe 5: Operatoren (unitaer 1 in 2 out) */
#define BC_DUP      0x50 /*   01010000  --- DUP */
                         /*   01010100  ---  */
                         /*   01011000  ---  */
                         /*   01011100  ---  */
/* Gruppe 6: Operatoren (binaer 2 in 1 out) Arithmetic and Math Operators  */
#define BC_ADD      0x60 /*    01100000  --- ADD */
#define BC_ADDi     0x61 /*    01100001  --- ADD int */
#define BC_ADDf     0x62 /*    01100010  --- ADD floats */
#define BC_ADDs     0x63 /*    01100011  --- ADD strings */

#define BC_SUB      0x64 /*    01100100  --- SUB */
#define BC_SUBi     0x65 /*    01100101  --- SUB int */
#define BC_SUBf     0x66 /*    01100110  --- SUB floats */

#define BC_MUL      0x68 /*    01101000  --- MUL */
#define BC_MULi     0x69 /*    01101001  --- MUL int */
#define BC_MULf     0x6a /*    01101010  --- MUL float */

#define BC_DIV      0x6c /*    01001100  --- DIV */

/* Gruppe 7: Operatoren (binaer 2 in 1 out)  Arithmetic and Math Operators */
#define BC_AND      0x70 /*    01110000  --- AND */
#define BC_OR       0x74 /*    01110100  --- OR */
#define BC_POW      0x78 /*    01111000  --- POW */
#define BC_MOD      0x7c /*    01111100  --- MOD */

/* Gruppe 8: Operatoren (binaer 2 in 1 out) Logic Math Operators */
#define BC_XOR      0x80 /*    10000000  --- XOR */
#define BC_EQUAL    0x84 /*    10000100  --- EQUAL */
#define BC_LESS     0x88 /*    10001000  --- LESS */
#define BC_GREATER  0x8c /*    10001100  --- GREATER */
/* Gruppe 9: Operatoren (2 in 2 out) */
#define BC_EXCH     0x90 /*    10010000  --- EXCHG   */

/* Gruppe 10: Operatoren (x in 0 out)   */
#define BC_POP      0xa0 /*    10100000  --- POP */
#define BC_CLEAR    0xa4 /*    10100100  --- CLEAR remove all elements from stack*/
#define BC_LOCAL    0xa6 /*    10100110  --- LOCAL */
#define BC_NOOP     0xa8 /*    10101000  --- NOOP */
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
#define BC_LOADs    0xcb /*    11001011  --- push string from address */
#define BC_LOADa    0xcf /*    11001111  --- push array from address */

/* Gruppe 13: Memory Management */
#define BC_SAVEi    0xd3 /*    11010011  --- store integer from address*/
#define BC_SAVEf    0xd7 /*    11010111  --- store float from address */
#define BC_SAVEs    0xdb /*    11011011  --- store string from address */
#define BC_SAVEa    0xdf /*    11011111  --- store array from address */


#define BC_FALSE  BC_PUSH0     /* Push a logical FALSE */
#define BC_TRUE   BC_PUSHM1     /* Push a logical TRUE */

//#define BC_PUSHUSERFUNC '@'
//#define BC_COPY  'C'
//#define BC_INDEX 'I'     /* Duplicate arbitrary element */
//#define BC_ROLL  'R'     /* Roll n elements up j times */

/* Interpretierungen */

//#define BC_LINE        1

/* Protos */

PARAMETER *virtual_machine(STRING, int *);
void compile(int verbose);
int bc_parser(const char *funktion);
int add_rodata(char *data,int len);

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
