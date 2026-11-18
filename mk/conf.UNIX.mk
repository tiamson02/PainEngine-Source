O=.o
# there is no "exe" for x86 UNIX, but the makefiles need some extension so use this for now.
EXT.EXE=$(BUILD_SUFFIX).bin
EXT.LIB=$(BUILD_SUFFIX).a
PRE.LIBRARY.LIB=../lib/$(DIR.OBJ)/
PRE.ENGINE.LIB=../engine/$(DIR.OBJ)/

# we won't be needing any DLLs, but just leave the def'n in place.
EXT.DLL=$(BUILD_SUFFIX).dll
EXT.DLE=$(BUILD_SUFFIX).dle
EXT.DEP=.d

MKDIR=mkdir -p $(@:/=)
RM=rm -f
RMDIR=rm -rf
CP=cp
LN=cp

COMPILER.c      =gcc
COMPILER.cc     =g++
COMPILER.asm    =nasm
LINK            =ar
LINK.cc         =ld

# set this to true to build demo.  make sure you do make clean before 
# rebuilding (if you were previously building the full release)
BUILDDEMO=false

CFLAGS.GENERAL    = -DUSE_FILE_REDIRECT -I/usr/X11R6/include/ -MD -march=i586 \
		    -mcpu=i686 -ffast-math -fno-exceptions -w

# Transgaming internal to remove ignition
CFLAGS.GENERAL += -DTRANSGAMING_INTERNAL
ifeq "$(BUILDDEMO)" "true"
CFLAGS.GENERAL += -DMARBLEBLAST_DEMO
endif

CFLAGS.RELEASE    = -g -O2 -finline-functions -fomit-frame-pointer 
CFLAGS.DEBUG      = -g -DTORQUE_DEBUG #-DTORQUE_DISABLE_MEMORY_MANAGER
CFLAGS.DEBUGFAST  = -O -g -finline-functions 

ASMFLAGS          = -f elf -dLINUX

# JMQ: this MOJO flag is required only if you want to statically link 
# libstdc++.  the MOJO options are system specific (I obtain these by 
# examining the output of gcc -v during linking.)  When in doubt, don't use
# these flags and just copy libstdc++ into the "lib" subfolder with everything
# else.  also see LINK.LIBS.MOJO below.  They need to be split up because 
# order matters when you're dealing with MOJO.
LFLAGS.MOJO	      =
#LFLAGS.MOJO       = -nostdlib /usr/lib/gcc-lib/i386-redhat-linux/2.96/../../../crt1.o /usr/lib/gcc-lib/i386-redhat-linux/2.96/../../../crti.o /usr/lib/gcc-lib/i386-redhat-linux/2.96/crtbegin.o -L/usr/lib/gcc-lib/i386-redhat-linux/2.96 -L/usr/lib/gcc-lib/i386-redhat-linux/2.96/../../..

LFLAGS.GENERAL    = $(LFLAGS.MOJO) -L../lib/ignition
LFLAGS.RELEASE    = 
LFLAGS.DEBUG      =

LINK.LIBS.MOJO    =
#LINK.LIBS.MOJO  =  -Wl,-static -lstdc++ -Wl,-dy -lm -lgcc -lc -lgcc /usr/lib/gcc-lib/i386-redhat-linux/2.96/crtend.o /usr/lib/gcc-lib/i386-redhat-linux/2.96/../../../crtn.o

LINK.LIBS.IGNITION=-Wl,-static -lignition -lssl -lcrypto -Wl,-dy 
ifeq "$(BUILDDEMO)" "true"
# no ignition for demo
LINK.LIBS.IGNITION=
endif

# GLU must be statically linked, otherwise torque will crash.
#LINK.LIBS.GENERAL = -Wl,-static -Wl,-lGLU -Wl,-dy -L/usr/X11R6/lib -lSDL -lpthread -ldl -logg -lvorbis $(LINK.LIBS.IGNITION) $(LINK.LIBS.MOJO) # MOJO must be last!!
LINK.LIBS.GENERAL = -Wl,-lGLU -Wl,-dy -L/usr/X11R6/lib -lSDL -lpthread -ldl -logg -lvorbis $(LINK.LIBS.MOJO) # MOJO must be last!!

LINK.LIBS.SERVER  = -lpthread
LINK.LIBS.RELEASE = 
LINK.LIBS.DEBUG   = 

PATH.H.SYS        =
