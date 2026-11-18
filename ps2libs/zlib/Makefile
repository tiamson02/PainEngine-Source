#
# Makefile for zlib (PS2) 
#

### Generic autoconf variables
TOPSRCDIR	= .
TOPOBJDIR	= .
SRCDIR		= .
TOP         = /usr/local/sce/ee
LIBDIR      = $(TOP)/lib
INCDIR      = $(TOP)/include
DLLS		= libz.a
NAME		= libz
BUILD		= DEBUG
DIR.OBJ         = out.GCC3.PS2.$(BUILD)
MKDIR		= mkdir -p
CC		= ee-gcc
CXX		= ee-g++
LD		= ee-ld

### Global settings
DEBUG_DEFINES	= -D_DEBUG -DDEBUG
DEFINES		= $(DEBUG_DEFINES) \
                         -DTRANSGAMING \
                         -DTRANSGAMING_PS2 

LIBS_STATIC	=
INCLUDE_PATH	= -I.
# -I../crosslibs/include/msvcrt

PS2LIB_C_SRCS		= 
PS2LIB_CPP_SRCS		= 
PC2PS2LIB_C_SRCS	= 	adler32.c \
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

PS2LIB_SRCS = $(PS2LIB_C_SRCS) $(PS2LIB_CPP_SRCS) $(PC2PS2LIB_C_SRCS)

vpath %.c .:../..
vpath %.cpp .:../..

DBGFLAG         = -g
OPTFLAG         = -O2
CFLAGS          = -W -Wall $(DBGFLAG) $(OPTFLAG) -I$(INCDIR) $(INCLUDE_PATH)
ASFLAGS         = $(DBGFLAG)
LDFLAGS         = $(DBGFLAG) -L$(LIBDIR) -L.
LDLIBS          = 

PS2LIB_C_OBJ = $(addprefix $(DIR.OBJ)/, $(addsuffix .o, $(basename $(PS2LIB_C_SRCS))))
PS2LIB_CPP_OBJ = $(addprefix $(DIR.OBJ)/, $(addsuffix .o, $(basename $(PS2LIB_CPP_SRCS))))
PC2PS2LIB_C_OBJ = $(addprefix $(DIR.OBJ)/, $(addsuffix .o, $(basename $(PC2PS2LIB_C_SRCS))))
PS2LIB_OBJ = $(PS2LIB_C_OBJ) $(PS2LIB_CPP_OBJ) $(PC2PS2LIB_C_OBJ)

#null:

default: $(NAME).a

clean:
	rm -f $(TARGET).prx $(NAME).a *~ *.bak $(PS2LIB_OBJ)

all: default

${PS2LIB_C_OBJ}: $(DIR.OBJ)/%.o: %.c
	$(MKDIR) $(dir $@)
	ee-gcc $(CFLAGS) -I. -o $@ -c $<

${PC2PS2LIB_C_OBJ}: $(DIR.OBJ)/%.o: %.c
	$(MKDIR) $(dir $@)
	ee-gcc $(CFLAGS) -I. -o $@ -c $<

${PS2LIB_CPP_OBJ}: $(DIR.OBJ)/%.o: %.cpp
	$(MKDIR) $(dir $@)
	ee-g++ $(CFLAGS) -I. -o $@ -c $<

$(NAME).a: $(PS2LIB_OBJ)
	ee-ar -r $@ $^



