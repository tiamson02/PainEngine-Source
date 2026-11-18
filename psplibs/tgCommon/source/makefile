# [SCE CONFIDENTIAL DOCUMENT]
# PSP(TM) Programmer Tool Runtime Library Release 2.6.0
# Copyright (C) 2004-2005 Sony Computer Entertainment Inc.
# All Rights Reserved

#-- Directories, Definitions etc.

SDK_TOP		= /usr/local/psp/devkit
LIBDIR		= $(SDK_TOP)/lib
INCDIR		= $(SDK_TOP)/include
TGDIR       = ..
DBGFLAG		= -g -G4
OPTFLAG		= -O3
CFLAGS		= -Wall $(DBGFLAG) $(OPTFLAG) -I$(INCDIR) -I$(TGDIR)/include -D_DEBUG
CC			= psp-gcc
AR			= psp-ar

TARGET		= $(TGDIR)/lib/libtgCommon
OBJS		= tgFile.o tgMath.o tgMatrix.o tgMem.o tgPad.o tgQuaternion.o \
			  tgVector.o tgCallTrace.o tgPSMF.o strfile.o

all:		$(TARGET).a

.c.o:
			$(CC) $(CFLAGS) -c $<
			
.cpp.o:
			$(CC) $(CFLAGS) -c $<

clean:
			@-rm -f $(TARGET).a *.o *~ *.bak PathDefs

$(TARGET).a: $(OBJS)
			$(AR) -r $@ $(OBJS)

#----------- rules --------------
-include PathDefs
PathDefs:
			psp-path-setup > PathDefs || (rm -f PathDefs ; exit 1)
