include ../../mk/conf.mk

#
# Makefile for tg_ps2
#

### Generic autoconf variables
TOPSRCDIR	= .
TOPOBJDIR	= .
SRCDIR		= .
TOP         = /usr/local/sce
LIBDIR      = $(TOP)/ee/lib
NAME		= libtg_ps2_$(BUILD)
#BUILD		= DEBUG
DIR.OBJ         = out.GCC3.PS2.$(BUILD)
MKDIR		= mkdir -p
CC		= ee-gcc
CXX		= ee-g++
LD		= ee-ar #ee-ld

### Global settings
DEBUG_DEFINES	= -D_DEBUG -DDEBUG #-DUSE_DEBUGPRINT
DEFINES		= $(DEBUG_DEFINES) \
                         -DTRANSGAMING \
                         -DTRANSGAMING_PS2 \
                         -D_WIN64 \
                         -DTRANSGAMING_USE_PHYSFS
ifeq "$(BUILD)" "MASTERING"
DEFINES += -DTRANSGAMING_MASTERING
endif

LIBS_STATIC	=
INCLUDE_PATH	= -I./include -I../../crosslibs/include -I../../physfs -I../ps2sdk/ps2shell
# -I../crosslibs/include/msvcrt

PS2LIB_C_SRCS		= crosslibs/source/helloc.c
PS2LIB_CPP_SRCS		= crosslibs/source/hellocpp.cpp
PC2PS2LIB_C_SRCS	= 	source/tg_pc2ps2.c \
				source/TNG_stdio_PS2.c

PS2LIB_SRCS = $(PS2LIB_C_SRCS) $(PS2LIB_CPP_SRCS) $(PC2PS2LIB_C_SRCS)

vpath %.c .:../..
vpath %.cpp .:../..

DBGFLAG         = -g
OPTFLAG         = -O2
CFLAGS          = -W -Wall $(DBGFLAG) $(OPTFLAG) -I$(TOP)/ee/include -I$(TOP)/common/include $(INCLUDE_PATH)
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
	$(CC) $(CFLAGS) $(DEFINES) -I. -o $@ -c $<

${PC2PS2LIB_C_OBJ}: $(DIR.OBJ)/%.o: %.c
	$(MKDIR) $(dir $@)
	$(CC) $(CFLAGS) $(DEFINES) -I. -o $@ -c $<

${PS2LIB_CPP_OBJ}: $(DIR.OBJ)/%.o: %.cpp
	$(MKDIR) $(dir $@)
	$(CXX) $(CFLAGS) $(DEFINES) -I. -o $@ -c $<

$(NAME).a: $(PS2LIB_OBJ)
	$(LD) -r $@ $^



