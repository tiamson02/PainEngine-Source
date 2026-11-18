include ../mk/conf.UNIX.mk

EXT.EXE=$(BUILD_SUFFIX).elf

#this one is only to simplify the other submakefiles. All PS2 targets define it
PS2_OSES		= YES
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

#CFLAGS.GENERAL    = -DUSE_FILE_REDIRECT -MD -ffast-math -pipe -DTORQUE_NO_MIPMAPS
#		    #-w -fno-exceptions -fno-check-new 

#CFLAGS.GENERAL    += -DPS2_DEVKIT -DPS2_ALL -DNO_OGGVORBIS
CFLAGS.GENERAL  += -DTRANSGAMING -DTRANSGAMING_PS2 -DHAVOK3 -DTRANSGAMING_NO_BINK -DTRANSGAMING_NO_GAMESPY -DTRANSGAMING_NO_ASE -DTRANSGAMING_NO_ICONV -DTRANSGAMING_NO_INPUT -DTRANSGAMING_NO_FREETYPE -DTRANSGAMING_NO_DEDICATED_SERVER -DTRANSGAMING_MEMORY_CONSTRAINTS -DTRANSGAMING_HACK -DSUPPORTS_PARTIAL_TEMPLATE_SPECIALIZATION -D_WIN64 -DTRANSGAMING_USE_PHYSFS -DTRANSGAMING_PS2_DISP_LIST

#CFLAGS.GENERAL += -DTRANSGAMING_INTERNAL -DTORQUE_NO_MIPMAPS -DTORQUE_NO_LUMINANCE 
#CFLAGS.GENERAL += -DTORQUE_FORCE_DISABLE_LIGHTING -DTORQUE_NO_QUADS -DTORQUE_NO_TRIFANS

#configuration flags for debugging features:
#  TRANSGAMING_LOGGING:         enables logging mode
#  TRANSGAMING_SILENT_DEBUG:    disables logging even in debug mode
#  TRANSGAMING_SHOW_FPS:        displays the FPS counter on screen
#  TRANSGAMING_MASTERING:       enables content access from the DVD/UMD volume
#  TRANSGAMING_NON_TNL:         enables all non-TNL code
#  TRANSGAMING_PS2_DISP_LIST    enables Display Lists
#  TRANSGAMING_MESH_WARNINGS:   enables warning messages to be generated at load time if meshes are using indexed data
#  TRANSGAMING_DEINDEX:         enables deindexed triangle list and triangle strip geometry.  Note that this just enables support for deindexed data and doesn't require it

CFLAGS.RELEASE      = -O2 -g -finline-functions -fomit-frame-pointer $(addprefix -D,$(PROJECTDEFS)) 
CFLAGS.OPTFLAG      = -O2 -g $(addprefix -D,$(PROJECTDEFS))
CFLAGS.DEBUG        = -g -DDEBUG $(addprefix -D,$(PROJECTDEFS)) -DTRANSGAMING_LOGGING -DTRANSGAMING_SHOW_FPS #-DFIND_HASH_PEAK
CFLAGS.DEBUGSILENT  = -g -DDEBUG $(addprefix -D,$(PROJECTDEFS)) -DTRANSGAMING_SILENT_DEBUG
CFLAGS.DEBUGFAST    = -O -g -finline-functions 
CFLAGS.MASTERING    = $(CFLAGS.OPTFLAG) -DTRANSGAMING_MASTERING  #-finline-functions -fomit-frame-pointer

PS2_CRT0          = $(DIR.OBJ)/crt0.o
#PS2_STDLINK       = -T $(APP_CMD) $(PS2_CRT0)
PS2_STDLINK       = -T $(APP_CMD)
 

# PS2LIBS_DIR
PS2LIBS_DIR = $(DIR.OBJ)/../../ps2libs


# ** SCE **
# link against sce libraries
SCELIBS      += graph
SCELIBS      += dma
SCELIBS      += dev
SCELIBS      += pkt
SCELIBS      += vu0
SCELIBS      += pad
SCELIBS      += cdvd
SCELIBS      += ipu
SCELIBS      += kernl
SCELIBS      += lout
SCELIBS      += mc
SCELIBS      += mpeg
SCELIBS      += msin
SCELIBS      += mtap
SCELIBS      += pc
SCELIBS      += sdr
SCELIBS      += ssyn
SCELIBS      += eenet
SCELIBS      += scf
SCELIBS      += timer
SCELIBS      += hig 
SCELIBS      += hip 
#SCELIBS      += c

SCESHORT_LIBS   := $(filter-out lib%.a, $(SCELIBS))

# find sce headers
INCDIRS   += $(SCEDIR)/ee/include $(SCEDIR)/common/include

# find sce libraries
SCELIBDIRS   += -L$(SCEDIR)/ee/lib -L$(SCEDIR)/ee/gcc/ee/lib

# ** PC2PS2 **

PC2PS2_BASENAMES   = tg_ps2_$(BUILD) 
PC2PS2_LIBDIR      = $(PS2LIBS_DIR)/tg_ps2
#PC2PS2_LIBNAMES    =
PC2PS2_LINKNAMES   = $(PC2PS2_BASENAMES)
vpath lib%.a   $(PC2PS2_LIBDIR)

PC2PS2_SHORT_LIBS   := $(filter-out lib%.a, $(PC2PS2_BASENAMES))

# ** PHYSFS **
PHYSFS_BASENAMES	=physfs
PHYSFS_LIBDIR = ../physfs/out.GCC3.PS2DEVKIT/
PHYSFS_LINKNAMES = $(PHYSFS_BASENAMES)
vpath lib%.a   $(PHYSFS_LIBDIR)

PHYSFS_SHORT_LIBS   := $(filter-out lib%.a, $(PHYSFS_BASENAMES))

# ** PS2GL **

PS2SDK            = ../ps2libs/ps2sdk
PS2GL_BASENAMES   = ps2gl ps2stuff ps2vecmat ps2shell
PS2GL_LIBDIR      = $(PS2SDK)/builtlibs
PS2GL_LINKNAMES   = $(PS2GL_BASENAMES)
PS2GL_SHORT_LIBS := $(filter-out lib%.a, $(PS2GL_BASENAMES))


# ** BINK **
BINKPS2_BASENAMES   = binkps2
BINKPS2_LIBDIRS     = $(PS2LIBS_DIR)/BinkPS2

BINKPS2SHORT_LIBS   := $(filter-out %.a, $(BINKPS2_BASENAMES))


# ** MILES **
#MILESPS2_BASENAMES   = binkps2
MILESPS2_LIBDIRS     = ./External/miles/lib/ps2

#BINKPS2SHORT_LIBS   := $(filter-out %.a, $(BINKPS2_BASENAMES))


# ** HAVOK **
# link against havok libraries...
HAVOK3_LIBDIR   = ./External/havok3/lib/ps2-gcc/release
vpath lib%.a    $(HAVOK3_LIBDIR)

HAVOK3_BASENAMES        += -lhkbase
HAVOK3_BASENAMES        += -lhkmath
HAVOK3_BASENAMES        += -lhkserialize
HAVOK3_BASENAMES        += -lhkcompat
HAVOK3_BASENAMES        += -lhkconstraintsolver
HAVOK3_BASENAMES        += -lhkcollide
HAVOK3_BASENAMES        += -lhkdynamics
HAVOK3_BASENAMES        += -lhkinternal
HAVOK3_BASENAMES        += -lhkutilities
HAVOK3_BASENAMES        += -lhkvisualize
HAVOK3_BASENAMES        += -lhkgraphics
HAVOK3_BASENAMES        += -lhkgraphicsbridge
HAVOK3_BASENAMES        += -lhkgraphicsps2
HAVOK3_BASENAMES        += -lhkdemoframework
HAVOK3_BASENAMES        += -lhkscenedata

HAVOK3_LINKNAMES        = $(HAVOK3_BASENAMES)
HAVOK3_SHORT_LIBS       := $(filter-out         lib%.a, $(HAVOK3_BASENAMES))
 

# ** ZLIB **
ZLIB_BASENAMES = z
ZLIB_LIBDIRS   = $(PS2LIBS_DIR)/zlib

ZLIBSHORT_LIBS := $(filter-out lib%.a, $(ZLIB_BASENAMES))


#LFLAGS.GENERAL    += -L$(PC2PS2_LIBDIR) -mno-crt0
LFLAGS.GENERAL    += -mno-crt0

# GLU must be statically linked, otherwise torque will crash.
# JMQNOTE: aside from gluProject/unProject, GLU doesn't work.  
# calling a GLU function that calls a GL function will cause a 
# crash.  let me know if you have a fix :)

#LINK.LIBS.GENERAL = $(LINK.LIBS.VORBIS) -Wl,-verbose --no-warn-mismatch -Wl,-static $(addprefix -l, $(PS2GL_LINKNAMES)) -Wl,-dy -lstdc++ -Wl,-dy -lm -lgcc -lc -lgcc /usr/local/sce/ee/gcc/lib/gcc-lib/ee/3.2-ee-030926/crtn.o /usr/local/sce/ee/gcc/lib/gcc-lib/ee/3.2-ee-030926/crti.o # -lefence 
#LINK.LIBS.GENERAL = $(LINK.LIBS.VORBIS) -Wl,--no-warn-mismatch -Wl,-static  -Wl,-dy -lstdc++ # -lefence
#LINK.LIBS.GENERAL = $(LINK.LIBS.VORBIS) -Wl,--no-warn-mismatch -lstdc++ # -lefence
LINK.LIBS.GENERAL = $(LINK.LIBS.VORBIS) -Wl,--start-group $(HAVOK3_BASENAMES) -Wl,--end-group -Wl,--no-warn-mismatch -lstdc++ # -lefence
# -verbose is excellent for elaborating on linking errors...

LINK.LIBS.TOOLS   = $(LINK.LIBS.VORBIS) -Wl,-verbose -static -Wl,-lGLU -Wl,-dy -L/usr/X11R6/lib -lSDL -lpthread -ldl # -lefence
# -lefence is useful for finding memory corruption problems

# Where to find PSX2 development stuff.
SCEDIR          = /usr/local/sce


# any setting in conf.UNIX.mk can be overridden here.  compiler name
# is commonly overridden, since it varies depending on the distro.
COMPILER.c      =ee-gcc
COMPILER.cc     =ee-g++
ASFLAGS         =-c -xassembler-with-cpp -Wa,-al
AS              =ee-gcc
COMPILER.asm    =$(AS) $(ASFLAGS)

#INCDIRS         += $(SCEDIR)/ee/include $(SCEDIR)/common/include
PATH.H.SYS       += -I$(SCEDIR)/ee/include/libeenet -I$(SCEDIR)/ee/include -I$(SCEDIR)/common/include -I$(PS2SDK)/ps2stuff/include -I$(PS2SDK)/ps2gl/include -I../ps2libs/tg_ps2/include


vpath %.s .:$(subst $(space),$(colon),$(SRCDIRS)):$(subst $(space),$(colon),$(SCELIBDIRS)):$(subst $(space),$(colon),$(PC2PS2_LIBDIR)):$(subst $(space),$(colon),$(HAVOKLIBDIRS))i:$(subst $(space),$(colon),$(BINKPS2_LIBDIRS)):$(subst $(space),$(colon),$(MILESPS2_LIBDIRS))
#vpath %.a .:$(subst $(space),$(colon),$(SCELIBDIRS))

OBJDIR      = .
$(DIR.OBJ)/%.o : %.s
	@echo using special rule to make assembly file
	$(AS) $(ASFLAGS) -o $@ $< > $(DIR.OBJ)/$*.lst

APP_CMD    := $(SCEDIR)/ee/lib/app.cmd

