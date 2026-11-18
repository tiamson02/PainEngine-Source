include ../../mk/conf.mk

#
# Makefile for tg_psp
#

### Generic autoconf variables
TOPSRCDIR	= .
TOPOBJDIR	= .
SRCDIR		= .
TOP             = /usr/local/psp/devkit
LIBDIR		= $(TOP)/lib
INCDIR		= $(TOP)/include
DLLS		= libtg_psp.a
NAME		= libtg_psp
DIR.OBJ         = out.GCC3.PSP.$(BUILD)
MKDIR		= mkdir -p
CC		= psp-gcc
CXX		= psp-g++
LD		= psp-ld

### Global settings
DEBUG_DEFINES	= -D_DEBUG -DDEBUG #-DUSE_DEBUGPRINT
DEFINES		= $(DEBUG_DEFINES) \
                         -DTRANSGAMING \
                         -DTRANSGAMING_PSP \
                         -DTRANSGAMING_USE_PHYSFS
ifeq "$(BUILD)" "MASTERING"
DEFINES += -DTRANSGAMING_MASTERING
endif

LIBS_STATIC	=
INCLUDE_PATH	= -I./include -I../../crosslibs/include -I../tgCommon/include -I../../physfs

PSPLIB_C_SRCS		=
PSPLIB_CPP_SRCS		=
#PSPLIB_C_SRCS		= crosslibs/source/helloc.c
#PSPLIB_CPP_SRCS		= crosslibs/source/hellocpp.cpp
PC2PSPLIB_C_SRCS	= 	source/tg_pc2psp.c\
				source/TNG_stdio_PSP.c

PSPLIB_SRCS = $(PSPLIB_C_SRCS) $(PSPLIB_CPP_SRCS)i $(PC2PSPLIB_C_SRCS)

vpath %.c .:..
vpath %.cpp .:..

DBGFLAG         = -g
OPTFLAG         = 
CFLAGS          = -W -Wall $(DBGFLAG) $(OPTFLAG) -I$(INCDIR) $(INCLUDE_PATH)
ASFLAGS         = $(DBGFLAG)
LDFLAGS         = $(DBGFLAG) -L$(LIBDIR) -L.
LDLIBS          = 


PSPLIB_C_OBJ = $(addprefix $(DIR.OBJ)/, $(addsuffix .o, $(basename $(PSPLIB_C_SRCS))))
PSPLIB_CPP_OBJ = $(addprefix $(DIR.OBJ)/, $(addsuffix .o, $(basename $(PSPLIB_CPP_SRCS))))
PC2PSPLIB_C_OBJ = $(addprefix $(DIR.OBJ)/, $(addsuffix .o, $(basename $(PC2PSPLIB_C_SRCS))))
PSPLIB_OBJ = $(PSPLIB_C_OBJ) $(PSPLIB_CPP_OBJ) $(PC2PSPLIB_C_OBJ)

#null:

default: $(DIR.OBJ)/$(NAME).a

clean:
	rm -f $(TARGET).prx $(TARGET).a *~ *.bak $(PSPLIB_OBJ)
	#rm -f $(TARGET).prx $(TARGET).elf *.o *~ *.bak $(PSPLIB_OBJ)

all: default

${PSPLIB_C_OBJ}: $(DIR.OBJ)/%.o: %.c
	$(MKDIR) $(dir $@)
	psp-gcc $(CFLAGS) $(DEFINES) -I. -o $@ -c $<

${PC2PSPLIB_C_OBJ}: $(DIR.OBJ)/%.o: %.c
	$(MKDIR) $(dir $@)
	psp-gcc $(CFLAGS) $(DEFINES) -I. -o $@ -c $<

${PSPLIB_CPP_OBJ}: $(DIR.OBJ)/%.o: %.cpp
	$(MKDIR) $(dir $@)
	psp-g++ $(CFLAGS) $(DEFINES) -I. -o $@ -c $<
			
$(DIR.OBJ)/$(NAME).a: $(PSPLIB_OBJ)
	psp-ar -r $@ $^

