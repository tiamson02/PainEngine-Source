include ../mk/conf.UNIX.mk

EXT.EXE=$(BUILD_SUFFIX).app

#this one is only to simplify the other submakefiles. All Mac targets define it
MAC_OSES		= YES
# Noteworthy compiler options:

# -DUSE_FILE_REDIRECT: enable fileio redirection to home directory.  The 
#   exact location is ~/.PREF_DIR_ROOT/PREF_DIR_GAME_NAME.  These are set in
#   platformX86UNIX.h.  You can disable redirection at run time with the
#   -nohomedir command line paramater.  Files will then be stored in the game
#   directory, like the windows version.
# -MD: generate .d dependency makefiles.  These are necessary if you want
#   to cleanly do partial builds.  
# -w: disable all warnings
# -fno-exceptions is safe to use (if your code doesn't use exceptions), 
#    but it is disabled by default
# -fno-check-new is not tested
CFLAGS.GENERAL    = -MD -ffast-math -pipe

# Where to find MAC dev tools.
MAC_SYS_DIR      := /System/Library
MAC_FRAMEWORK_DIR = /Volumes/Panther/System/Library/Frameworks
INCDIR          = $(MAC_SYS_TOP)/include

# Find the psp libraries
MACLIBDIRS   = $(MAC_SYS_DIR)/lib

#CFLAGS.GENERAL = -DMAC_DEVKIT -DMAC_ALL -DNO_OGGVORBIS
#CFLAGS.GENERAL += -DTRANSGAMING_INTERNAL
CFLAGS.GENERAL += -DTRANSGAMING -DTRANSGAMING_MACOSX -DSUPPORTS_PARTIAL_TEMPLATE_SPECIALIZATION

CFLAGS.DEBUG      = -g -DDEBUG
CFLAGS.OPTFLAG    = -O2
CFLAGS.RELEASE    = $(CFLAGS.OPTFLAG) -finline-functions -fomit-frame-pointer 
CFLAGS.DEBUGFAST  = -O -g -finline-functions 

#PS2_CRT0          = $(DIR.OBJ)/crt0.o
#PS2_STDLINK       = -T $(APP_CMD) $(PS2_CRT0)

CFLAGS.MAC  = -W -Wall $(CFLAGS.DEBUG) $(CFLAGS.OPTFLAG) -I$(INCDIR)
LDFLAGS         = $(CFLAGS.DEBUG) -L$(LIBDIR)
MAC_LDLIBS      = -lgum -lgu -lm


# any setting in conf.UNIX.mk can be overridden here.  compiler name
# is commonly overridden, since it varies depending on the distro.
COMPILER.c      =gcc
COMPILER.cc     =gcc
COMPILER.cxx	=g++
COMPILER.c++	=g++
ASFLAGS         =$(CFLAGS.DEBUG)-c -xassembler-with-cpp -Wa,-al
AS              =psp-gcc
COMPILER.asm    =$(AS) $(ASFLAGS)

#INCDIRS         += $(SCEDIR)/ee/include $(SCEDIR)/common/include
PATH.H.SYS       += -I$(MAC_SYS_DIR)/include

# link against sce libraries
MACLIBS      += c

MACSHORT_LIBS   := $(filter-out lib%.a, $(MACLIBS))

OBJDIR      = .
$(DIR.OBJ)/%.o : %.s
	@echo using special rule to make assembly file
	$(AS) $(ASFLAGS) -o $@ $< > $(DIR.OBJ)/$*.lst

#APP_CMD    := $(SCEDIR)/ee/lib/app.cmd

