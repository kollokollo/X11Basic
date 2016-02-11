/* BYTECODE.H (c) Markus Hoffmann  */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#define BC_STACKLEN 128



typedef struct {
  unsigned char BRAs;       /* DC_BRAs */
  unsigned char offs;       /* sizeof(BYTECODE_HEADER)*/
  unsigned long textseglen; /* size of the text/code segment */
  unsigned long dataseglen; /* size of the data segment */
  unsigned long bssseglen;    /* not used */
  unsigned long symbolseglen; /* not used */
  unsigned long reserved1;    /* reserved, not used */
  unsigned long reserved2;    /* reserved, not used */
  unsigned short version;     /* version, currently=1 */
} BYTECODE_HEADER;

/* Operand Stack Manipulation Operators */

#define BC_PUSH0  '0'     /* Push a int 0 */
#define BC_FALSE  BC_PUSH0     /* Push a logical FALSE */
#define BC_PUSH1  '1'     /* Push a int 1 */
#define BC_PUSH2  '2'     /* Push a int 2 */
#define BC_PUSHM1 '9'     /* Push a int -1 */
#define BC_TRUE  BC_PUSHM1     /* Push a logical TRUE */
#define BC_PUSHA  'a'
#define BC_PUSHB  'b'
#define BC_PUSHI  'i'     /* Push a int constant */
#define BC_PUSHF  'f'     /* Push a double constant */
#define BC_PUSHS  's'     /* Push a string constant */
#define BC_PUSHX  'x'
#define BC_PUSHLEER 'l'     /* Push an empty parameter */
#define BC_PUSHSYS 'y'
#define BC_PUSHSSYS 'w'
#define BC_PUSHASYS 'z'
#define BC_PUSHV 'v'
#define BC_PUSHARRAYELEM 'V'
#define BC_PUSHCOMM 'c'
#define BC_PUSHFUNC 'e'
#define BC_PUSHAFUNC 'g'
#define BC_PUSHSFUNC 'h'
#define BC_PUSHUSERFUNC '@'
#define BC_BRK   'B'     /* Breakpoint, this will cause illegal instruction error */
#define BC_COPY  'C'
#define BC_DUP   'D'
#define BC_EXCH  'E'
#define BC_CLEAR 'F'     /* remove all elements from stack */
#define BC_COUNT 'G'     /* count the number of elements on stack */
#define BC_INDEX 'I'     /* Duplicate arbitrary element */
#define BC_ROLL  'R'     /* Roll n elements up j times */

/* Arithmetic and Math Operators */

#define BC_ADD  '+'
#define BC_SUB  '-'
#define BC_MUL  '*'
#define BC_DIV  '/'
#define BC_POW  '^'
#define BC_AND  '&'
#define BC_OR  '|'
#define BC_NOT  '!'
#define BC_NEG  '~'
#define BC_MOD  '%'
#define BC_XOR  '§'
#define BC_EQUAL  '='
#define BC_LESS  '<'
#define BC_GREATER  '>'

/* Programmlauf-Kontrolle */
#define BC_HALT 0
#define BC_JMP  0x10         /* Long Jump */
#define BC_BRA  0x11         /* short Branch */
#define BC_BRAs 0x12         /* very short Branch */
#define BC_JEQ  0x13         /* jump if 0 */
#define BC_BEQ  0x14         /* short Branch if 0 */
#define BC_BEQs 0x15         /* short Branch if 0 */
#define BC_BSR  0x16         /* Branch to Subroutine */
#define BC_JSR  0x17         /* jump to Subroutine */
#define BC_RTS  0x18         /* Return from Subroutine */

/* Interpretierungen */

#define BC_LINE        1
#define BC_POP        ':'
#define BC_NOOP       ' '
#define BC_COMMENT    '\''
#define BC_ZUWEIS      2
#define BC_ZUWEISINDEX 3


/* Protos */

PARAMETER *virtual_machine(STRING, int *);

