/* virtual-machine.h (c) Markus Hoffmann  */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */


PARAMETER *virtual_machine(const STRING bcpc, int o, int *np, const PARAMETER *plist, int inp,char *rodata);

extern int program_adr; /* bytecode offset of the last token to be processed.
                           used by runtime.c for error messages */
