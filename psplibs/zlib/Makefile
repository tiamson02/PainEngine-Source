#
# Makefile for zlib (PSP) 
#

### Generic autoconf variables
TOPSRCDIR	= .
TOPOBJDIR	= .
SRCDIR		= .
TOP         = /usr/local/psp/devkit
LIBDIR      = $(TOP)/lib
INCDIR      = $(TOP)/include
DLLS		= libz.a
NAME		= libz
BUILD		= DEBUG
DIR.OBJ         = out.GCC3.PSP.$(BUILD)
MKDIR		= mkdir -p
CC		= psp-gcc
CXX		= psp-g++
LD		= psp-ld

### Global settings
DEBUG_DEFINES	= -D_DEBUG -DDEBUG
DEFINES		= $(DEBUG_DEFINES) \
                         -DTRANSGAMING \
                         -DTRANSGAMING_PSP 

LIBS_STATIC	=
INCLUDE_PATH	= -I.
# -I../crosslibs/include/msvcrt

PSPLIB_C_SRCS		= 
PSPLIB_CPP_SRCS		= 
PC2PSPLIB_C_SRCS	= 	adler32.c \
				compress.c \
				crc32.c \
				deflate.c \
				gzio.c \
				inffast.c \
				inflate.c \
				infback.c \
				inftrees.c \
				minigzip.c \
				trees.c \
				uncompr.c \
				zutil.c

PSPLIB_SRCS = $(PSPLIB_C_SRCS) $(PSPLIB_CPP_SRCS) $(PC2PSPLIB_C_SRCS)

vpath %.c .:../..
vpath %.cpp .:../..

DBGFLAG         = -g
OPTFLAG         = -O2
CFLAGS          = -W -Wall $(DBGFLAG) $(OPTFLAG) -I$(INCDIR) $(INCLUDE_PATH)
ASFLAGS         = $(DBGFLAG)
LDFLAGS         = $(DBGFLAG) -L$(LIBDIR) -L.
LDLIBS          = 

PSPLIB_C_OBJ = $(addprefix $(DIR.OBJ)/, $(addsuffix .o, $(basename $(PSPLIB_C_SRCS))))
PSPLIB_CPP_OBJ = $(addprefix $(DIR.OBJ)/, $(addsuffix .o, $(basename $(PSPLIB_CPP_SRCS))))
PC2PSPLIB_C_OBJ = $(addprefix $(DIR.OBJ)/, $(addsuffix .o, $(basename $(PC2PSPLIB_C_SRCS))))
PSPLIB_OBJ = $(PSPLIB_C_OBJ) $(PSPLIB_CPP_OBJ) $(PC2PSPLIB_C_OBJ)

#null:

default: $(NAME).a

clean:
	rm -f $(TARGET).prx $(NAME).a *~ *.bak $(PSPLIB_OBJ)

all: default

${PSPLIB_C_OBJ}: $(DIR.OBJ)/%.o: %.c
	$(MKDIR) $(dir $@)
	psp-gcc $(CFLAGS) -I. -o $@ -c $<

${PC2PSPLIB_C_OBJ}: $(DIR.OBJ)/%.o: %.c
	$(MKDIR) $(dir $@)
	psp-gcc $(CFLAGS) -I. -o $@ -c $<

${PSPLIB_CPP_OBJ}: $(DIR.OBJ)/%.o: %.cpp
	$(MKDIR) $(dir $@)
	psp-g++ $(CFLAGS) -I. -o $@ -c $<

$(NAME).a: $(PSPLIB_OBJ)
	psp-ar -r $@ $^



