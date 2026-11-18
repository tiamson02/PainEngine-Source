
SHELL       = /bin/sh

TOP         = /usr/local/sce/ee
LIBDIR      = $(TOP)/lib
SYSINC		= -I$(TOP)/include -I/usr/local/sce/common/include 

TARGET      = sbctest
OBJS        = ../../../ps2common/ps2common.o \
	../../../nonport.o \
	../../../darray.o\
	../../../hashtable.o\
	../../sb_crypt.o\
	../../sb_queryengine.o\
	../../sb_server.o\
	../../sb_serverbrowsing.o\
	../../sb_serverlist.o\
	../../../qr2/qr2regkeys.o\
	crt0.o\
	../$(TARGET).o

snsystems: STACKOBJ =

cisco: STACKOBJ = ../../../ps2common/nftioctl.o

eenet: STACKOBJ =

LCFILE      = $(LIBDIR)/app.cmd                
 
 LIBS        = $(STACKLIB) \
               $(LIBDIR)/libdev.a \
               $(LIBDIR)/libgraph.a \
               $(LIBDIR)/libdma.a \
               $(LIBDIR)/libpkt.a \
               $(LIBDIR)/libcdvd.a

snsystems: STACKLIB = $(LIBDIR)/sneetcp.a

cisco: STACKLIB = $(LIBDIR)/nft/libnftee.a

eenet: STACKLIB = $(LIBDIR)/libeenet.a\
                  $(LIBDIR)/eenetctl.a\
                  $(LIBDIR)/ent_smap.a\
                  $(LIBDIR)/ent_eth.a\
                  $(LIBDIR)/ent_ppp.a\
                  $(LIBDIR)/libscf.a

snsystems: STACKDEF = SN_SYSTEMS

cisco: STACKDEF = CISCO_NFT	

eenet: STACKDEF = EENET

snsytems: STACKINC =

cisco: STACKINC =

eenet: STACKINC = -I$(TOP)/include/libeenet/

PREFIX      = ee
AS          = $(PREFIX)-gcc
CC          = $(PREFIX)-gcc
LD          = $(PREFIX)-gcc
DVPASM      = $(PREFIX)-dvp-as
OBJDUMP     = $(PREFIX)-objdump
RUN         = dsedb -r run
RM          = /bin/rm -f

CFLAGS      = -g -O0 -Wall -Wa,-al -fno-common 
CXXFLAGS    = -g -Wall -Wa,-al -fno-exceptions -fno-common
ASFLAGS     = -c -xassembler-with-cpp -Wa,-al
DVPASMFLAGS = -g 
LDFLAGS     = -Wl,-Map,$(TARGET).map -mno-crt0 -L$(LIBDIR) -lm
TMPFLAGS    =
INCDIR      = $(STACKINC) $(SYSINC)

.SUFFIXES: .c .s .cc .dsm

all: FORCE
	@echo type either make eenet, make snsystems or make cisco 

$(TARGET).elf: $(OBJS) $(LIBS)
	$(LD) -o $@ -T $(LCFILE) $(OBJS) $(STACKOBJ) $(STACKLIB) $(LIBS) $(LDFLAGS)

crt0.o: $(LIBDIR)/crt0.s
	$(AS) $(ASFLAGS) $(TMPFLAGS) -o $@ $< > $*.lst

.s.o:
	$(AS) $(ASFLAGS) $(TMPFLAGS) $(INCDIR) -o $@ $< > $*.lst

.dsm.o:
	$(DVPASM) $(DVPASMFLAGS) $(INCDIR) -o $@ $< > $*.lst

.c.o:
	$(CC) $(CFLAGS) $(TMPFLAGS) -D$(STACKDEF) $(INCDIR) -c $< -o $*.o > $*.lst

.cc.o:
	$(CC) $(CXXFLAGS) $(TMPFLAGS) -D$(STACKDEF) $(INCDIR) -c $< -o $*.o > $*.lst

run: $(TARGET).elf
	$(RUN) $(TARGET).elf

snsystems: $(TARGET).elf

cisco: $(TARGET).elf

eenet: $(TARGET).elf

clean:
	$(RM) $(OBJS) *.o *.map *.lst core *.dis *.elf

FORCE:
