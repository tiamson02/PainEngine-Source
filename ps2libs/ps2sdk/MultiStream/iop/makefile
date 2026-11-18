ifeq ($(wildcard PathDefs),)
PathDefs:
	iop-path-setup > PathDefs || (rm -f PathDefs ; exit 1)
	make all
else
include PathDefs
endif

INCOPT = -I/usr/local/sce/common/include/ -I/usr/local/sce/iop/include
#INCOPT = -I/usr/local/sce/common/include/ -I/usr/local/sce/iop/include -I/usr/local/scream/iop/include

CFLAGS  = $(INCOPT) -I. -G0 -Wall -O2
ASFLAGS = $(INCOPT) -G0
RM          = /bin/rm -f

#---------- UnComment these two lines if using the DTS Middleware -------------
# DTSOBJDIR    = ../dts
# DTSOBJS      = $(DTSOBJDIR)/iopdts.o

COMPILE.s = $(CC) -xassembler-with-cpp $(ASFLAGS) $(CPPFLAGS) $(TARGET_MACH) -c

#----------- customize section --------------
PROGNAME = stream

OBJS     = entry.o $(DTSOBJS) bgm_r2s.o ms_dsp.o stream.o
ILIBS	=

#----------- rules --------------
all:	$(PROGNAME).irx

clean:
	rm -f *.o $(PROGNAME).irx *.obj *.map $(DTSOBJS)

$(PROGNAME).irx: $(OBJS) $(DTSOBJS)
	$(LINK.o)  -o $@ \
		$(OBJS) -L/ee/lib -L./  -ilb=libsd.ilb  -ilb=cdvdman.ilb -ilb=msifrpc.ilb 

#$(OBJS) -L/ee/lib -L./  -ilb=libsd.ilb  -ilb=cdvdman.ilb -ilb=msifrpc.ilb -ilb=../../scream/iop/lib/scream.ilb $(ILIBS)

entry.o:	stream.h
iopdts.o:	stream.h
ms_dsp.o:	stream.h
stream.o:	stream.h

