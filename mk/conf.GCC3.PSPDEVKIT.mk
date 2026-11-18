include ../mk/conf.UNIX.mk

EXT.EXE=$(BUILD_SUFFIX).prx

#this one is only to simplify the other submakefiles. All PSP targets define it
PSP_OSES		= YES
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
CFLAGS.GENERAL   += -menable-vfpu=m0-m7 -mfast-vfpu-math

# Where to find PSP dev tools.
PSP_SDK_DIR      = /usr/local/psp/devkit
PSP_GCC_DIR      = /usr/local/psp/gcc-1.7.1
INCDIR          = $(PSP_SDK_DIR)/include
PSPLIBDIRS	= $(PSP_SDK_DIR)/lib

# Our PSP libs..
PSP_EXT_LIBS_DIR = $(DIR.OBJ)/../../psplibs

#CFLAGS.GENERAL = -DPSP_DEVKIT -DPSP_ALL -DNO_OGGVORBIS
#CFLAGS.GENERAL += -DTRANSGAMING_INTERNAL
#CFLAGS.GENERAL += -DTRANSGAMING -DTRANSGAMING_PSP -DHAVOK3 -DTRANSGAMING_NO_MILES -DTRANSGAMING_NO_BINK -DTRANSGAMING_NO_HAVOK -DTRANSGAMING_NO_GAMESPY -DTRANSGAMING_NO_ASE -DTRANSGAMING_NO_ICONV -DTRANSGAMING_NO_INPUT -DTRANSGAMING_NO_FREETYPE -DTRANSGAMING_NO_DEDICATED_SERVER -DTRANSGAMING_MEMORY_CONSTRAINTS -DTRANSGAMING_HACK -DSUPPORTS_PARTIAL_TEMPLATE_SPECIALIZATION
CFLAGS.GENERAL += -DTRANSGAMING -DTRANSGAMING_PSP -DHAVOK3 -DTRANSGAMING_NO_MILES -DTRANSGAMING_NO_BINK -DTRANSGAMING_NO_GAMESPY -DTRANSGAMING_NO_ASE -DTRANSGAMING_NO_ICONV -DTRANSGAMING_NO_INPUT -DTRANSGAMING_NO_FREETYPE -DTRANSGAMING_NO_DEDICATED_SERVER -DTRANSGAMING_MEMORY_CONSTRAINTS -DTRANSGAMING_HACK -DSUPPORTS_PARTIAL_TEMPLATE_SPECIALIZATION -DTRANSGAMING_USE_PHYSFS

#configuration flags for debugging features:
#  TRANSGAMING_LOGGING:         enables logging mode
#  TRANSGAMING_SILENT_DEBUG:    disables logging even in debug mode
#  TRANSGAMING_SHOW_FPS:        displays the FPS counter on screen
#  FIND_HASH_PEAK:              enables printouts that track the maximum used size of each hash table in the game
#  TRANSGAMING_MASTERING:       enables content access from the DVD/UMD volume
#  TRANSGAMING_NON_TNL:         enables all non-TNL code
#  TRANSGAMING_MESH_WARNINGS:   enables warning messages to be generated at load time if meshes are using indexed data
#  TRANSGAMING_DEINDEX:         enables deindexed triangle list and triangle strip geometry.  Note that this just enables support for deindexed data and doesn't require it

CFLAGS.DEBUG        = -O2 -g -DDEBUG $(addprefix -D,$(PROJECTDEFS)) -DTRANSGAMING_LOGGING -DTRANSGAMING_SHOW_FPS #-DTRANSGAMING_USE_AI  #-DFIND_HASH_PEAK
CFLAGS.DEBUGSILENT  = -O2 -g -DDEBUG $(addprefix -D,$(PROJECTDEFS)) -DTRANSGAMING_SILENT_DEBUG
CFLAGS.OPTFLAG      = -O2 -g #$(addprefix -D,$(PROJECTDEFS)) 
CFLAGS.RELEASE      = $(CFLAGS.OPTFLAG) #-finline-functions -fomit-frame-pointer 
CFLAGS.MASTERING    = $(CFLAGS.OPTFLAG) -DTRANSGAMING_MASTERING #-finline-functions -fomit-frame-pointer

CFLAGS.DEBUGFAST  = -O -g -finline-functions 

#PS2_CRT0          = $(DIR.OBJ)/crt0.o
#PS2_STDLINK       = -T $(APP_CMD) $(PS2_CRT0)
PSP_STDLINK       = -startfiles

CFLAGS.PSPDEVKIT  = -W -Wall $(CFLAGS.DEBUG) $(CFLAGS.OPTFLAG) -I$(INCDIR)
LDFLAGS         = $(CFLAGS.DEBUG) -L$(LIBDIR)


# ** PC2PSP **

PC2PSP_BASENAMES   = tg_psp
PC2PSP_LIBDIR      = $(PSP_EXT_LIBS_DIR)/tg_psp/out.GCC3.PSP.$(BUILD)
#PC2PSP_LIBNAMES    =
PC2PSP_LINKNAMES   = $(PC2PSP_BASENAMES)
vpath lib%.a   $(PC2PSP_LIBDIR)

PC2PSP_SHORT_LIBS   := $(filter-out lib%.a, $(PC2PSP_BASENAMES))

# ** PHYSFS **
PHYSFS_BASENAMES	=physfs
PHYSFS_LIBDIR = ../physfs/out.GCC3.PSPDEVKIT/
PHYSFS_LINKNAMES = $(PHYSFS_BASENAMES)
vpath lib%.a   $(PHYSFS_LIBDIR)

PHYSFS_SHORT_LIBS   := $(filter-out lib%.a, $(PHYSFS_BASENAMES))

TGCOMMON_BASENAMES   = tgCommon
TGCOMMON_LIBDIR      = $(PSP_EXT_LIBS_DIR)/tgCommon/lib
TGCOMMON_LINKNAMES   = $(TGCOMMON_BASENAMES)
vpath lib%.a   $(TGCOMMON_LIBDIR)

TGCOMMON_SHORT_LIBS   := $(filter-out lib%.a, $(TGCOMMON_BASENAMES))


# ** HAVOK3 **
HAVOK3_LIBDIR      = ./External/havok3/lib/psp-gcc/release
vpath lib%.a   $(HAVOK3_LIBDIR)

HAVOK3_BASENAMES    +=  -lhkbase
HAVOK3_BASENAMES    +=  -lhkmath
HAVOK3_BASENAMES    +=  -lhkserialize
HAVOK3_BASENAMES    +=  -lhkcompat
HAVOK3_BASENAMES    +=  -lhkconstraintsolver
HAVOK3_BASENAMES    +=  -lhkcollide
HAVOK3_BASENAMES    +=  -lhkdynamics
HAVOK3_BASENAMES    +=  -lhkinternal
HAVOK3_BASENAMES    +=  -lhkutilities
HAVOK3_BASENAMES    +=  -lhkvisualize
HAVOK3_BASENAMES    +=  -lhkgraphics
HAVOK3_BASENAMES    +=  -lhkgraphicsbridge
HAVOK3_BASENAMES    +=  -lhkgraphicspsp
HAVOK3_BASENAMES    +=  -lhkdemoframework
HAVOK3_BASENAMES    +=  -lhkscenedata

HAVOK3_LINKNAMES    = $(HAVOK3_BASENAMES)
HAVOK3_SHORT_LIBS   := $(filter-out lib%.a, $(HAVOK3_BASENAMES))


# ** BINK **
BINKPSP_BASENAMES   = binkpsp
BINKPSP_LIBDIRS     = $(PSP_EXT_LIBS_DIR)/BinkPSP

BINKPSPSHORT_LIBS   := $(filter-out %.a, $(BINKPSP_BASENAMES))


# ** PSP LIBS **
# link against sce libraries
PSPLIBS      += deflt 
PSPLIBS      += fpu
PSPLIBS      += gcov 
PSPLIBS      += gu 
PSPLIBS      += gum 
PSPLIBS      += gim_picture 
PSPLIBS      += m 
PSPLIBS      += vfpu
PSPLIBS      += wave

PSPSHORT_LIBS   := $(filter-out lib%.a, $(PSPLIBS))

# find psp libraries
#PSPLIBDIRS   += $(PSP_SDK_DIR)/lib
#PSPLIBDIRS   += -L$(PSPDIR)/lib -L$(PSPDIR)/ee/gcc/ee/lib

PSPLOADLIBES =  $(PSPLIBDIRS)/ctrl_stub.a
PSPLOADLIBES += $(PSPLIBDIRS)/display_stub.a
PSPLOADLIBES += $(PSPLIBDIRS)/ge_user_stub.a
PSPLOADLIBES += $(PSPLIBDIRS)/mediaman_stub.a
PSPLOADLIBES += $(PSPLIBDIRS)/rtc_stub.a
PSPLOADLIBES += $(PSPLIBDIRS)/power_stub.a


# ** ZLIB **
ZLIB_BASENAMES = z
ZLIB_LIBDIRS   = $(PSP_EXT_LIBS_DIR)/zlib

ZLIBSHORT_LIBS := $(filter-out lib%.a, $(ZLIB_BASENAMES))


LINK.LIBS.GENERAL = $(LINK.LIBS.VORBIS) -Wl,--start-group $(HAVOK3_BASENAMES) -Wl,--end-group -Wl,--no-warn-mismatch -Wl,-dy -lstdc++ # -lefence
# -verbose is excellent for elaborating on linking errors...

LINK.LIBS.TOOLS   = $(LINK.LIBS.VORBIS) -Wl,-verbose -static -Wl,-lGLU -Wl,-dy -L/usr/X11R6/lib -lSDL -lpthread -ldl # -lefence
# -lefence is useful for finding memory corruption problems


# any setting in conf.UNIX.mk can be overridden here.  compiler name
# is commonly overridden, since it varies depending on the distro.
COMPILER.c      =psp-gcc
COMPILER.cc     =psp-g++
#ASFLAGS         =$(CFLAGS.DEBUG)-c -xassembler-with-cpp -Wa,-al
ASFLAGS         =-c -xassembler-with-cpp -Wa,-al
AS              =psp-gcc
COMPILER.asm    =$(AS) $(ASFLAGS)

#INCDIRS         += $(SCEDIR)/ee/include $(SCEDIR)/common/include
PATH.H.SYS       += -I$(PSP_SDK_DIR)/include -I$(PSP_GCC_DIR)/include/c++/3.3.3+allegrex-2.2.2-psp-1.7.1/include -I../psplibs/tg_psp/include

# link against sce libraries
PSPLIBS      += c

PSPSHORT_LIBS   := $(filter-out lib%.a, $(PSPLIBS))

vpath %.s .:$(subst $(space),$(colon),$(SRCDIRS)):$(subst $(space),$(colon),$(PSPLIBDIRS))

OBJDIR      = .
$(DIR.OBJ)/%.o : %.s
	@echo using special rule to make assembly file
	$(AS) $(ASFLAGS) -o $@ $< > $(DIR.OBJ)/$*.lst


