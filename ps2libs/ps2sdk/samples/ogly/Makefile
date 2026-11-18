##########################################################################
### Copyright (c) 2005 Transgaming TechnologiesInc.
###
### Modified for ps2sdk by Ludovic Nicolle (ludo@transgaming.com)
###
### Modified from
###
### Copyright (c) 1999, 2000, 2001 Sony Computer Entertainment America Inc.
###	All rights reserved.
### 
### Boilerplate Makefile by Bret Mogilefsky (mogul@playstation.sony.com)
### 	and Tyler Daniel (tyler_daniel@playstation.sony.com)
###
###	Use this makefile as a template for new projects!
### 
### General Features:
###
###	Just specify SRCS and go!
###	Automatic and minimal (fast!) dependency generation (for vu microcode as well)
###	Allows keeping source and headers from src and include dirs, or elsewhere.
###	Builds in a subdirectory.
###	Allows additional defines, include dirs, and lib dirs without 
###		specifying -D, -I, and -L
###	Easy to specify parallel builds (debug, optimized, release, etc)
###	Easy to add flags on a per-file, per-build, or per-file-build basis
###	Can specify parent projects to make first (libraries)
###	Builds libraries
###	Slices, dices, feeds your cat, calls your mum.
###
### VU microcode features:
###
###	Generates depencies for microcode (for .include and #include)
###	Uses a preprocessing script to optionally manage registers
###		and add a few pseudo-directives
###	Runs the c preprocessor over microcode - you can use #define and #include
###		freely (and share #defines with c/c++)
###	Support for vcl
###		automatically finds and optimizes .include "*.vcl" in vsm files
###
### Useful targets:
###
###	run		Run the executable.
###	xrun		Run the executable under a new xterminal.
###	clean		Remove everything we can rebuild.
###	tags		Generate source-browsing tags for Emacs.
###
### Using builds:
###
### 	To specify a particular build include the name of the build anywhere on
### 	the command line:
### 		make xrun optimized,
###   	make clean optimized, etc.
###
### 	Included builds (add your own!):
### 		debug
###		optimized	(default)
###		release
###
###	For more info see the "Build Options" section below
##########################################################################


##########################################################################
### Target
##########################################################################


# The name of the binary file we want to generate.  Also handles libraries! (.a)
TARGET		= ogly.elf


##########################################################################
### Files and Paths - this is probably the only section you'll need to change
##########################################################################

# gl

GLROOT		= ../..
PS2STUFFDIR	= $(GLROOT)/ps2stuff
GLDIR		= $(GLROOT)/ps2gl
GLUTDIR		= $(GLROOT)/ps2gl/glut

# vectors

VECDIR		= $(GLROOT)/ps2vecmat

# collisions

#HASHGRIDDIR	= ../hashgrid
#PRIMDISTDIR	= ../primdist

# The source files for the project.
#SRCS		+= $(wildcard *.cpp)
#SRCS		+= $(foreach DIR,$(SRCDIRS),$(subst $(DIR)/,,$(wildcard $(DIR)/*.cpp)))
SRCS		+= 	ogly.cpp \
			model.cpp \
			tglogo.c \
			pads.cpp

# Additional objects to link. Only add things that aren't built from SRCS!
OBJS		= 

# Additional libs to link with. (sce libs are listed in the section below)
LIBS		= ps2gl ps2stuff ps2vecmat

# Additional locations for header files
#INCDIRS		= $(GLUTDIR)/include
INCDIRS		+= $(GLDIR)/include
INCDIRS		+= $(PS2STUFFDIR)/include
INCDIRS		+= $(VECDIR)/include
#INCDIRS		+= $(HASHGRIDDIR)/include
#INCDIRS		+= $(PRIMDISTDIR)/include

# Additional locations for library files
#LIBDIRS		= $(GLUTDIR)/objs_optimized
#LIBDIRS		+= $(GLDIR)/objs_optimized
#LIBDIRS		+= $(PS2STUFFDIR)/objs_optimized
#LIBDIRS		+= $(HASHGRIDDIR)/lib
#LIBDIRS		+= $(PRIMDISTDIR)/lib
LIBDIRS		+= $(GLROOT)/builtlibs

# Additional locations for source files
SRCDIRS		= src

# Object files and the target will be placed in this directory with an
# underscore and the buildname appended (e.g., for the "debug" build: objs_debug/)
OBJDIRBASE	= objs

# Dependency files will be placed in this directory with an underscore and
# the buildname appended (e.g., for the "debug" build: deps_debug/)
DEPDIRBASE	= deps

# If this project depends on another (a ps2 rendering library for example) that should
# be built with make before making this one, list the directory here.
MAKEPARENTS	= 

# Where to find PSX2 development stuff.
SCEDIR		= /usr/local/sce

##########################################################################
### Common Options (shared across builds)
##########################################################################

# link against sce libraries
LIBS		+= sn
LIBS		+= graph
LIBS		+= dma
LIBS		+= dev
LIBS		+= pkt
LIBS		+= vu0
LIBS		+= pad
LIBS		+= cdvd
LIBS		+= ipu
LIBS		+= kernl
LIBS		+= lout
LIBS		+= mc
LIBS		+= mpeg
LIBS		+= msin
LIBS		+= mtap
LIBS		+= pc
LIBS		+= sdr
LIBS		+= ssyn

# find sce headers
INCDIRS		+= $(SCEDIR)/ee/include $(SCEDIR)/common/include

# find sce libraries
LIBDIRS		+= $(SCEDIR)/ee/lib

# Additional preprocessor definitions
DEFINES		= 

# Compiler optimization options
OPTFLAGS	= -fno-rtti -G 0

# Compiler debug options

# enable all warnings
DEBUGFLAGS	= -Wall
# output assembly listings with c/c++ code
DEBUGFLAGS	+= -Wa,-alh
# properly handle *(u_long128*)&someVar
DEBUGFLAGS	+= -fno-strict-aliasing
# don't generate code for c++ 'try' and 'catch'
DEBUGFLAGS	+= -fno-exceptions
# for multithreading to work properly?
DEBUGFLAGS	+= -fno-common

# Command-line arguments to be passed to the target when we run it
RUNARGS		= 


##########################################################################
### Build Options - applied per-build
##########################################################################


# Which build to do if none is specified on the command line
DEFAULTBUILD		= debug

# Specifics for the "debug" build
BUILDNAMES		+= debug
debug_DEBUGFLAGS	= -D_DEBUG -g
debug_OPTFLAGS		= -mvu0-use-vf0-vf31 

# Specifics for the "optimized" build
BUILDNAMES		+= optimized
optimized_DEBUGFLAGS	= -D_DEBUG -g
optimized_OPTFLAGS	= -ffast-math -O2 -mvu0-use-vf0-vf31 -finline-limit=30000

BUILDNAMES		+= linux
linux_INCDIRS		= ../ps2gl/modules
linux_DEFINES		= PS2_LINUX
linux_DEBUGFLAGS	= -D_DEBUG -g
linux_OPTFLAGS		= -ffast-math -O2 
linux_PLATFORM		= linux

BUILDNAMES		+= linux_debug
linux_debug_INCDIRS	= ../ps2gl/modules
linux_debug_DEFINES	= PS2_LINUX
linux_debug_DEBUGFLAGS	= -D_DEBUG -g
linux_debug_OPTFLAGS	= -ffast-math
linux_debug_PLATFORM	= linux

BUILDNAMES		+= cdrom
cdrom_DEBUGFLAGS	= -D_DEBUG -g
cdrom_OPTFLAGS	= -ffast-math -O2 -mvu0-use-vf0-vf31 
cdrom_DEFINES		= CDROM

# Specifics for the "perf" build
BUILDNAMES		+= perf
perf_DEBUGFLAGS		= -DPS2S_DO_TIMERS
perf_OPTFLAGS		= -ffast-math -O2 -mvu0-use-vf0-vf31 

# Specifics for the "release" build
BUILDNAMES		+= release
release_OPTFLAGS	= -ffast-math -O2 -mvu0-use-vf0-vf31 -finline-limit=30000

###### CodeWarrior-linkable builds #######

BUILDNAMES		+= cw_debug
cw_debug_DEBUGFLAGS	= -D_DEBUG -g
cw_debug_OPTFLAGS	= -G 0

BUILDNAMES		+= cw_optimized
cw_optimized_DEBUGFLAGS	= -D_DEBUG -g
cw_optimized_OPTFLAGS	= -ffast-math -O2 -G 0

BUILDNAMES		+= cw_release
cw_release_OPTFLAGS	= -ffast-math -O2 -G 0

# You can specify flags for a new build type "hamburger" as follows:
# BUILDNAMES		+= hamburger
# hamburger_INCDIRS	= someincdirs
# hamburger_LIBDIRS	= somelibdirs
# hamburger_DEFINES	= somedefs
# hamburger_OPTFLAGS	= someoptflags
# hamburger_DEBUGFLAGS	= somedebugflags
# hamburger_RUNARGS	= somerunargs


##########################################################################
### Per-file Options
##########################################################################


# Additional defines and include dirs can be specified on a per-file basis
# by prefixing with the stem of the filename.  For example, if I wanted special flags
# for building mucilage.cpp, I could add any of the following 
# mucilage_INCDIRS	= someincdirs 
# mucilage_LIBDIRS	= somelibdirs
# mucilage_DEFINES	= somedefs
# mucilage_OPTFLAGS	= someoptflags
# mucilage_DEBUGFLAGS	= somedebugflags

##########################################################################
### Per-file, per-build Options
##########################################################################


# Similar to above.. To apply special flags for building mucilage.cpp for
# the debug build, I could add any of the following
# mucilage_debug_INCDIRS	= someincdirs 
# mucilage_debug_LIBDIRS	= somelibdirs
# mucilage_debug_DEFINES	= somedefs
# mucilage_debug_OPTFLAGS	= someoptflags
# mucilage_debug_DEBUGFLAGS	= somedebugflags


##########################################################################
### Makefile operation
##########################################################################


# Set this to 1 to print status messages (like 'Compiling somefile.cpp...')
PRINT_MSGS			= 1

# Set this to 1 to print the exact command lines used to build files
PRINT_CMDS			= 1


##########################################################################
### include the makefile that does all the work
##########################################################################

include Makefile.work

ifeq ($(GCC_MAJOR),3)
DEBUGFLAGS		+= -Wno-deprecated
endif
