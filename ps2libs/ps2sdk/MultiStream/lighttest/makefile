
SHELL       = /bin/sh

TOP         = /usr/local/sce/ee
LIBDIR      = $(TOP)/lib
INCDIR      = $(TOP)/include  -I/usr/local/sce/common/include

TARGET      = main
OBJS        = crt0.o \
              tim2.o \
              graphics.o \
              $(TARGET).o

MS5OBJDIR    = ../ee
MS5OBJS      = $(MS5OBJDIR)/sound.o


LCFILE      = app.cmd
LIBS        = $(LIBDIR)/libgraph.a \
              $(LIBDIR)/libdma.a \
              $(LIBDIR)/libdev.a \
              $(LIBDIR)/libpkt.a \
              $(LIBDIR)/libcdvd.a \
              $(LIBDIR)/libpad.a \
              $(LIBDIR)/libmrpc.a \
              $(LIBDIR)/libsdr.a \
              $(LIBDIR)/libmc.a \
	      $(LIBDIR)/libvu0.a

PREFIX      = ee
AS          = $(PREFIX)-gcc
CC          = $(PREFIX)-gcc
LD          = $(PREFIX)-gcc
DVPASM      = $(PREFIX)-dvpasm
OBJDUMP     = $(PREFIX)-objdump
RUN         = dsedb -r run
RM          = /bin/rm -f

CFLAGS      = -O2 -Wa,-al -fno-common -fno-strict-aliasing
CXXFLAGS    = -O2 -Wa,-al -fno-exceptions -fno-common -fno-strict-aliasing
ASFLAGS     = -c -xassembler-with-cpp -Wa,-al
DVPASMFLAGS = 
LDFLAGS     = -Wl,-Map,$(TARGET).map -mno-crt0 -L$(LIBDIR) -lm
TMPFLAGS    =

.SUFFIXES: .c .s .cc .dvpasm

all: $(TARGET).elf

$(TARGET).elf: $(OBJS) $(MS5OBJS) $(LIBS)
	$(LD) -o $@ -T $(LCFILE) $(OBJS) $(MS5OBJS) $(LIBS) $(LDFLAGS)

crt0.o: $(LIBDIR)/crt0.s
	$(AS) $(ASFLAGS) $(TMPFLAGS) -o $@ $< > $*.lst

.s.o:
	$(AS) $(ASFLAGS) $(TMPFLAGS) -I$(INCDIR) -o $@ $< > $*.lst

.dvpasm.o:
	$(DVPASM) $(DVPASMFLAGS) -I$(INCDIR) -o $@ $< > $*.lst

.c.o:
	$(CC) $(CFLAGS) $(TMPFLAGS) -I$(INCDIR) -c $< -o $*.o > $*.lst

.cc.o:
	$(CC) $(CXXFLAGS) $(TMPFLAGS) -I$(INCDIR) -c $< -o $*.o > $*.lst

run: $(TARGET).elf
	$(RUN) $(TARGET).elf

clean:
	$(RM) *.o *.map *.lst core *.dis *.elf
