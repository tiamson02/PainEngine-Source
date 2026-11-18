
TOP         = /usr/local/sce/ee
LIBDIR      = $(TOP)/lib
INCDIR      =  . -I../stdio-zip -I../MultiStream/ee -I$(TOP)/include -I/usr/local/sce/common/include -I$(TOP)/gcc/ee/include -I$(TOP)/gcc/lib/gcc-lib/ee/2.95.3/include -I../ps2shell

TARGET      = libps2al
OBJS        = stdio_multistream.o \
              al.o \
			  al_stubs.o \
              alut.o \
			  al_listener.o \
			  al_internal.o \
              ../MultiStream/ee/sound.o

HEADERS		= stdio_multistream.h \
			  al/al.h \
			  al/alut.h

PREFIX      = /usr/local/sce/ee/gcc/bin/ee
CC          = $(PREFIX)-gcc
AR          = $(PREFIX)-ar
RM          = /bin/rm -f

CFLAGS      = -G 0 -Wa,-al -Wall -fno-common -fno-strict-aliasing -g -DPS2 $(addprefix -D,$(PROJECTDEFS)) 
#CFLAGS      = -G 0 -Wa,-al -Wall -fno-common -fno-strict-aliasing -g -D PS2 -D USE_STDIO_ZIP

.SUFFIXES: .c


debug: all

optimized: all

all: $(TARGET).a

$(TARGET).a: $(OBJS) 
	$(AR) -r $@ $(OBJS)
	cp -a $@ ../builtlibs
	
.c.o: 
	$(CC) $(CFLAGS) $(TMPFLAGS) -I$(INCDIR) -c $< -o $*.o > $*.lst
	
.cpp.o: 
	$(CC) $(CFLAGS) $(TMPFLAGS) -I$(INCDIR) -c $< -o $*.o > $*.lst
	
clean:
	$(RM) *.o *.map *.lst core *.dis *.elf

