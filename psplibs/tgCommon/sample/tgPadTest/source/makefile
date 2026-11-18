# [SCE CONFIDENTIAL DOCUMENT]
# PSP(TM) Programmer Tool Runtime Library Release 2.6.0
# Copyright (C) 2004-2005 Sony Computer Entertainment Inc.
# All Rights Reserved

#-- Directories, Definitions etc.

VERSION		= PSP_HARDWARE

TOP 		= /usr/local/psp/devkit
CC			= psp-gcc 
CXX			= psp-g++
AR			= psp-ar

TGDIR		= ../../..
  
INCDIR		= $(TOP)/include 
LIBDIR		= $(TOP)/lib
CFLAGS		= -O0 -g -Wall -I$(INCDIR) -I$(TGDIR)/include -D_DEBUG -D $(VERSION)
CXXFLAGS	= -O0 -g -Wall -I$(INCDIR) -I$(TGDIR)/include -D_DEBUG -D $(VERSION)

LFLAGS		= -O2 -g -Wall -startfiles -L$(LIBDIR)				\
			 -lgu -lgum -lgim_picture -lm -lstdc++ -lsupc++		\
			 $(TGDIR)/lib/tgCommon.a							\
			 $(LIBDIR)/ctrl_stub.a								\
			 $(LIBDIR)/display_stub.a							\
			 $(LIBDIR)/ge_user_stub.a

#-- Target
TARGET	= tgPadTest.prx
OBJS	= main.o

#-- Rules

.PHONY: all clean

all:	$(TARGET)

$(TARGET): $(OBJS) $(TGDIR)/lib/tgCommon.a
	$(CXX) -o $@ $(OBJS) $(LFLAGS)  

$(TGDIR)/lib/tgCommon.a:
	$(MAKE) -C $(TGDIR)/source

.cpp.o:
	$(CXX) -o $@ $(CXXFLAGS) -c $<

.c.o:
	$(CC) $(CFLAGS) -c $<

#-- Operations

clean:
	@-rm -f $(TARGET) *.o *~ *.bak


