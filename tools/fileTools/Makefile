CC = g++
RM = rm -f
CP = cp

FILECLIP = fileClip
FILEREPLACE = fileReplace
FILECOMPILED = fileCompiled
BUILDPREFIX =

ifneq "$(BUILD)" ""
    BUILDPREFIX = -D
endif

CFLAGS = -O2 -Wall

default: 
	@echo specify a target: 'linux' or 'cygwin'

linux: main.cpp
	$(CC) $(CFLAGS) -o $(FILECLIP) -DBUILD_FILECLIP -DLINUX $(BUILDPREFIX)$(BUILD) main.cpp
	@$(RM) main.o
	$(CC) $(CFLAGS) -o $(FILEREPLACE) -DBUILD_FILEREPLACE -DLINUX $(BUILDPREFIX)$(BUILD) main.cpp
	@$(RM) main.o
	$(CC) $(CFLAGS) -o $(FILECOMPILED) -DBUILD_FILECOMPILED -DLINUX $(BUILDPREFIX)$(BUILD) main.cpp
	@$(RM) main.o
	$(CP) $(FILECLIP) ../bin
	$(CP) $(FILEREPLACE) ../bin
	$(CP) $(FILECOMPILED) ../bin

cygwin: main.cpp
	$(CC) $(CFLAGS) -o $(FILECLIP) -DBUILD_FILECLIP $(BUILDPREFIX)$(BUILD) main.cpp
	@$(RM) main.o
	$(CC) $(CFLAGS) -o $(FILEREPLACE) -DBUILD_FILEREPLACE $(BUILDPREFIX)$(BUILD) main.cpp
	@$(RM) main.o
	$(CC) $(CFLAGS) -o $(FILECOMPILED) -DBUILD_FILECOMPILED $(BUILDPREFIX)$(BUILD) main.cpp
	@$(RM) main.o
	$(CP) $(FILECLIP) ../bin
	$(CP) $(FILEREPLACE) ../bin
	$(CP) $(FILECOMPILED) ../bin
	
clip: main.cpp
	@$(RM) main.o
	$(CC) $(CFLAGS) -o $(FILECLIP) -DBUILD_FILECLIP $(BUILDPREFIX)$(BUILD) main.cpp
	$(CP) $(FILECLIP) ../bin

replace: main.cpp
	@$(RM) main.o
	$(CC) $(CFLAGS) -o $(FILEREPLACE) -DBUILD_FILEREPLACE $(BUILDPREFIX)$(BUILD) main.cpp
	$(CP) $(FILEREPLACE) ../bin

compiled: main.cpp
	@$(RM) main.o
	$(CC) $(CFLAGS) -o $(FILECOMPILED) -DBUILD_FILECOMPILED $(BUILDPREFIX)$(BUILD) main.cpp
	$(CP) $(FILECOMPILED) ../bin
	
clean:
	$(RM) $(FILECLIP).exe
	$(RM) $(FILEREPLACE).exe
	$(RM) $(FILECOMPILED).exe
	$(RM) $(FILECLIP)
	$(RM) $(FILEREPLACE)
	$(RM) $(FILECOMPILED)
	$(RM) main.o