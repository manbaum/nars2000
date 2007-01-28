# TOOLS.MAK - Makefile for special tools used to build NARS2000 product

# Other definitions
NAME = NARS2000                         # Directory name
PROJPATH = $(SRCROOT)$(NAME)\trunk\     # Project path
BIN = $(PROJPATH)binaries\              # Binaries directory
BC=$(BIN)COM\                           # Directory for common binaries

R=$(PROJPATH)Tools\                     # Input directory
O=RET\                                  # Output directory
M98=M:\                                 # MSVC 1.52 (16-bit) compiler
CC=$(M98)BIN\cl
LD=$(M98)BIN\link
INCLUDE=.;$(M98)INCLUDE
LIB=$(M98)LIB

{$(R)}.c{$(O)}.obj:
        $(CC) /c /AS /Z7 /Fo$* $(@B).c

{$(O)}.obj{$(O)}.exe:
        $(LD) /line /co /map /st:0x3000 $*,$*,$*;

{$(O)}.exe{$(BC)}.exe:
        xc /r $< $@
        $(M98)BIN\cvpack /strip $@

ALL:    DIR                 \
         $(BC)depscan.exe   \
         $(BC)vdir.exe

DEL:
        @del /Q $(O)*.*

DIR:
        vdir -c $(O)

$(O)depscan.obj: $(R)depscan.c

$(O)depscan.exe: $(O)depscan.obj

$(O)vdir.exe: vdir.cpp
        $(CC) /c /AL /Z7 /Fo$* $(@B).cpp
        $(LD) /line /co /map /st:0x3000 $*,$*,$*,$(MAXROOT)lib\lstutil.lib;

