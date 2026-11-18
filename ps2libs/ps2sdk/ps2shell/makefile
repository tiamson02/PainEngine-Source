
TOP         = /usr/local/sce/ee
LIBDIR      = $(TOP)/lib
INCDIR      =  . -I$(TOP)/include -I/usr/local/sce/common/include

TARGET      = libps2shell
OBJS        = shellCore.o \
					shellFile.o \
					shellPad.o \
					shellMemCard.o \
					AsciiToJis.o

PREFIX      = /usr/local/sce/ee/gcc/bin/ee
CC          = $(PREFIX)-gcc
AR          = $(PREFIX)-ar
RM          = /bin/rm -f

CFLAGS      = -G 0 -O2 -Wa,-al -Wall -fno-common -fno-strict-aliasing -g -DPS2 $(addprefix -D,$(PROJECTDEFS)) 

.SUFFIXES: .c


debug: all

optimized: all

all: $(TARGET).a

$(TARGET).a: $(OBJS) 
	$(AR) -r $@ $(OBJS)
	cp -a $@ ../builtlibs

.c.o:
	$(CC) $(CFLAGS) $(TMPFLAGS) -I$(INCDIR) -c $< -o $*.o > $*.lst

clean:
	$(RM) *.o *.map *.lst core *.dis *.elf

