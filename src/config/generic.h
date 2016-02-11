
#ifndef CONFIG_H
#define CONFIG_H

/* Generic configuration file - a starting point for porting */

/* do we have a register to keep the status register in? */
#undef SRREG

/* do we have a register for the PC? */
#undef PCREG

/* ... one for the NZ flag value? */
#undef NZREG

/* do we need a small "memory model"? */
/* #define SMALL */

/* Make sure we know about the endianness of this system */
#ifndef IS_BIG_ENDIAN
#error Please set IS_BIG_ENDIAN to 0 or 1 in the Makefile!
#endif

#endif /* CONFIG_H */
