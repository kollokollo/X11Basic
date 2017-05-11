# Makefile for some extensions needed by MS WINDOWS
# for X11-Basic (c) Markus Hoffmann 2005
#

SHELL=/bin/sh
# Register variables 
REGS= 

# Optimization and debugging options
OPT=-O3

# Additional header file paths
INC=

SFLAGS=
DEF=  -DSH_MEM -DSTATFS_USE_STATVFS -DINLINE=inline

# Compiler fuer Windows-Excecutable

#CC=i386-mingw32msvc-gcc
#CC=i686-pc-mingw32-gcc
CC=i586-mingw32msvc-gcc

# Preprocessor
CPP=gcc -E

CFLAGS= $(INC) $(DEF) $(OPT) $(SFLAGS)

OBJS=   fnmatch.o memfrob.o

CSRC=$(OBJS:.o=.c)

all :    fnmatch.o memfrob.o

clean :
	rm -f  *.o 

# DO NOT DELETE
