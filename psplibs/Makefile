#
# Makefile for tg_psp
#

### Generic autoconf variables
TOPSRCDIR	= .
TOPOBJDIR	= .
SRCDIR		= .
INCDIR		= .
LIBDIR		= .
DLLS		= libtg_psp.a
NAME		= tg_psp
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
INCLUDE_PATH	=   

PSPLIB_C_SRCS		= crosslibs/source/helloc.c
PSPLIB_CPP_SRCS		= crosslibs/source/hellocpp.cpp
PSPLIB_SRCS = $(PSPLIB_C_SRCS) $(PSPLIB_CPP_SRCS)

vpath %.c .:..
vpath %.cpp .:..

DBGFLAG         = -g
OPTFLAG         = -O2
CFLAGS          = -W -Wall $(DBGFLAG) $(OPTFLAG) -I$(INCDIR)
ASFLAGS         = $(DBGFLAG)
LDFLAGS         = $(DBGFLAG) -L$(LIBDIR) -L.
LDLIBS          = 

PSPLIB_C_OBJ = $(addprefix $(DIR.OBJ)/, $(addsuffix .o, $(basename $(PSPLIB_C_SRCS))))
PSPLIB_CPP_OBJ = $(addprefix $(DIR.OBJ)/, $(addsuffix .o, $(basename $(PSPLIB_CPP_SRCS))))
PSPLIB_OBJ = $(PSPLIB_C_OBJ) $(PSPLIB_CPP_OBJ)

#null:

default: $(NAME).a

clean:
	rm -f $(TARGET).prx $(TARGET).elf *.o *~ *.bak

all: default

${PSPLIB_C_OBJ}: $(DIR.OBJ)/%.o: %.c
	$(MKDIR) $(dir $@)
	psp-gcc $(CFLAGS) -I. -o $@ -c $<

${PSPLIB_CPP_OBJ}: $(DIR.OBJ)/%.o: %.cpp
	$(MKDIR) $(dir $@)
	psp-g++ $(CFLAGS) -I. -o $@ -c $<
			
$(NAME).a: $(PSPLIB_OBJ)
	psp-ar -r $@ $^



