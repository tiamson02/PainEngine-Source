#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#ifdef TRANSGAMING_PSP
# include <kernel.h>
#elif defined(TRANSGAMING_PS2)

#endif
#include "Core/CRC32.h"
#include "Core/Debug.h"
#include "memoryDump.h"


//this value must exist otherwise we only get 64kb of memory to work with
extern int sce_newlib_heap_kb_size;



namespace Memory{

/*
memory state save file format:
----------------------------------------------

  Legend:
    "STRING" :      the string contained in quotes will appear directly in the binary stream
    [] :            designates the contents of a data block or an instance of a data block 
    <name>:size :   names a single sub-block of the file.  The name for the block is "name",
                    and it occupies <size> bytes in the binary stream.
    | :             separation between data blocks or sub-blocks.
    = :             assigns a name to a data block, or a block group to the binary stream.
    STREAM :        the full file format description.

  Blocks:
    Header =    ["STAT" | <moduleCRC>:4 | <entryPoint>:4 | <textAddress>:4 | <textSize>:4 | 
                         <dataSize>:4 | <bssSize>:4 | <codeSegmentAddress>:4 | <codeSize>:4 | 
                         <currentMallocSize>:4]
    TextBlock = ["TEXT" | <textSegmentAddress>:4 | <textSegmentSize>:4 | <textStoredByteCount>:4 |
                         <textSegmentData>:textStoredByteCount]
    HeapBlock = ["HEAP" | <heapAddress>:4 | <heapStoredByteCount>:4 | <heapData>:heapStoredByteCount]

  File Format:
    STREAM = [Header] | [TextBlock] | [HeapBlock]


  Sub-Blocks:
    Header:
    <moduleCRC>: a 32-bit CRC value identifying the current version of the game's module.
        This value must match the 32-bit CRC of the module attempting to load a previously
        saved memory state.  If it does not match the memory state will immediately fail to
        load.
    <entryPoint>: the address of the program's entry point function.  This will be the 
        address of the _start() function found in crt0.c.  This value must match that of 
        any module that is loading a save state or the state load will fail immediately.
    <textAddress>: base address of the text segment of the module.  This must match that
        of any module that is loading a save state or the state load will fail immediately.
    <textSize>: size of the module's text segment in bytes.  This must match that of any 
        module that is loading a save state or the state load will fail immediately.
    <dataSize>: size of the module's data segment in bytes.  This must match that of any 
        module that is loading a save state or the state load will fail immediately.
    <bssSize>: size of the module's BSS segment in bytes.  This must match that of any 
        module that is loading a save state or the state load will fail immediately.
    <codeSegmentAddress>: base address of the module's code segment.  This must match
        that of any module that is loading a save state or the state load will fail
        immediately.
    <codeSize>: sizeof the module's code segment in bytes.  This must match that of any 
        module that is loading a save state or the state load will fail immediately.
    <currentMallocSize>: total amount of memory allocated at the time this memory image
        was saved.  This is mostly used for debugging purposes to make sure the load
        was successful.

    TextBlock:
    <textSegmentAddress>: base address of the module's text segment.  This is the address
        that the first chunk of data in the state save will be written to upon load.  This
        address must match that of the loading module or the state load will fail immediately.
    <textSegmentSize>: size of the module's text memory segment in bytes.  This is the total
        size of all static and global data used within the module.  This value must match 
        that of the loading module or the state load will fail immediately.
    <textStoredByteCount>: total number of bytes stored in the state save stream that will be
        written into the module's memory space.  This value will not be the same as the 
        <textSegmentSize> value since it does not include the 'safe memory' area.
    <textSegmentData>: actual data that will be written into the text data segment of the 
        current module.  The writing will start at <textSegmentAddress> and continue for
        <textStoredByteCount> bytes.  This will not include the entire text data segment, but 
        rather just the portion of it up to, but not including, the 'safe memory' area.  The
        rest of the text data segment memory will be loaded with part of the heap data.

    HeapBlock:
    <heapAddress>: base address that the heap data will be written to.  This address will
        actually be located directly after the 'safe memory' area block, and will continue
        for the rest of the reserved heap space.  No checking or comparison is done with
        this address.
    <heapStoredByteCount>: size of the heap data in bytes.  This is the total number of
        bytes that will be copied into main memory space of the module.
    <heapData>: actual data that will be written into the heap area of the module.  The
        writing starts at the address <heapAddress> and continues for <heapStoredByteCount>
        bytes.  This data chunk actually contains portion of the text data segment that
        was after the 'safe memory' block.  This sub-block should occupy the bulk of the
        binary stream.
*/


#define MAKE_FCC(a, b, c, d) ((((d) & 0xff) << 24) | (((c) & 0xff) << 16) | (((b) & 0xff) << 8) | (((a) & 0xff)))

typedef unsigned int FCC;

static const FCC FCC_STATE = MAKE_FCC('S', 'T', 'A', 'T');
static const FCC FCC_TEXT =  MAKE_FCC('T', 'E', 'X', 'T');
static const FCC FCC_HEAP =  MAKE_FCC('H', 'E', 'A', 'P');

#ifdef TRANSGAMING_PSP
# define FILE_T              SceUID
# define OPEN(fname, mode)   sceIoOpen(fname, (mode), 0644)
# define CLOSE(f)            sceIoClose(f)
# define WRITE(f, blk, size) sceIoWrite(f, (blk), (size))
# define READ(f, blk, size)  sceIoRead(f, (blk), (size))
# define OUTP(f, v)          WRITE(f, &(v), sizeof(v))
# define INP(f, v)           READ(f, &(v), sizeof(v))
# define WRITEFCC(f, fcc)    OUTP(f, fcc)
# define READFCC(f, fcc)     INP(f, fcc)
#elif defined(TRANSGAMING_PS2)

#endif

//StateHeader: main header layout for the state save files
typedef struct{
    FCC     fcc;
    CRC32   moduleCRC;
    size_t  entryPoint;
    size_t  textAddress;
    size_t  textSize;
    size_t  dataSize;
    size_t  bssSize;
    size_t  codeAddress;
    size_t  codeSize;
    size_t  currentMallocSize;
} StateHeader __attribute__((packed));


//normally, having a global variable is an awful thing to do, but we want to guarantee 
//this block always exists in the text segment of the code and will not be clobbered
//when the memory state is loaded or reloaded
SafeMemory safe;


//**************************** SafeMemory Class Implementation **********************************//
SafeMemory::SafeMemory(){
    memset(buffer, 0, SafeMemory::BUFSIZE);

    stateFilename[0] = 0;
    currentStateOp = STATE_OP_NONE;

    calcCRC();
}

SafeMemory::~SafeMemory(){}

bool SafeMemory::registerStateSave(const char *path){
    if (path == NULL || path[0] == 0 || strlen(path) >= FILENAMESIZE)
        return false;

    
    strncpy(stateFilename, path, FILENAMESIZE);
    currentStateOp = STATE_OP_SAVE;

    return true;
}

bool SafeMemory::registerStateLoad(const char *path){
    if (path == NULL || path[0] == 0 || strlen(path) >= FILENAMESIZE)
        return false;

    
    strncpy(stateFilename, path, FILENAMESIZE);
    currentStateOp = STATE_OP_LOAD;

    return true;    
}


void SafeMemory::calcCRC(){
#ifdef TRANSGAMING_PSP
    SceKernelModuleInfo mi;
    SceUID              id = sceKernelGetModuleId();
    int                 error;


    moduleCRC = 0;
    memset(&mi, 0, sizeof(mi));

    if (id <= 0){
        TRACE("Memory::SafeMemory::calcCRC(): ERROR-> could not retrieve the module ID!\n");

        return;
    }

    
    //grab the module information
    error = sceKernelQueryModuleInfo(id, &mi);

    if (error != SCE_KERNEL_ERROR_OK){
        TRACE("Memory::SafeMemory::calcCRC(): ERROR-> could not retrieve the module info to save the program state!\n");

        return;
    }


    moduleCRC = Crc32((const char *)mi.text_addr, mi.text_size);
    TRACE("Memory::SafeMemory::calcCRC(): calculated the code CRC to be 0x%08X {base = 0x%08X, size = %d}\n", moduleCRC, mi.text_addr, mi.text_size);

#ifdef DEBUG
    //dumpModule();
#endif
#elif defined(TRANSGAMING_PS2)

#endif
}

#ifdef DEBUG
void SafeMemory::dumpModule() const{
# ifdef TRANSGAMING_PSP
    FILE_T              fp;
    char                fname[_MAX_PATH];
    SceKernelModuleInfo mi;
    SceUID              id = sceKernelGetModuleId();
    int                 error;


    memset(&mi, 0, sizeof(mi));

    if (id <= 0){
        TRACE("Memory::SafeMemory::dumpModule(): ERROR-> could not retrieve the module ID!\n");

        return;
    }

    
    //grab the module information
    error = sceKernelQueryModuleInfo(id, &mi);

    if (error != SCE_KERNEL_ERROR_OK){
        TRACE("Memory::SafeMemory::dumpModule(): ERROR-> could not retrieve the module info to save the program state!\n");

        return;
    }



    //dump the entire code segment to a file for comparison operations
    for (int i = 0; i < 100; i++){
        snprintf(fname, _MAX_PATH, "host0:States/Modules/module_%02d.bin", i);

        fp = OPEN(fname, SCE_O_RDONLY);

        if (fp >= 0){
            CLOSE(fp);

            continue;
        }


        TRACE("Memory::SafeMemory::dumpModule(): dumping the module's code segment to '%s'\n", fname);
        fp = OPEN(fname, SCE_O_WRONLY | SCE_O_CREAT | SCE_O_TRUNC);

        WRITE(fp, (const void *)mi.segmentaddr[0], mi.segmentsize[0]);

        CLOSE(fp);
        TRACE("Memory::SafeMemory::dumpModule(): done dumping the module\n");
        break;
    }
# elif defined(TRANSGAMING_PS2)

# endif
}
#endif


SafeMemory::StateOpResult SafeMemory::checkForStateRequest(){
    bool result;


    switch (currentStateOp){
        case STATE_OP_NONE:
            return STATE_NONE;

        case STATE_OP_SAVE:
            currentStateOp = STATE_OP_NONE;
            
            if (stateFilename[0] == 0)
                return STATE_NONE;


            result = saveState(stateFilename);
            stateFilename[0] = 0;

            return result ? STATE_SUCCESS : STATE_FAILED;            

        case STATE_OP_LOAD:
            currentStateOp = STATE_OP_NONE;
            
            if (stateFilename[0] == 0)
                return STATE_NONE;


            result = loadState(stateFilename);
            stateFilename[0] = 0;

            return result ? STATE_SUCCESS : STATE_FAILED;
    }

    return STATE_FAILED;
}


//**************************** State dump functions *********************************************//
bool SafeMemory::saveState(const char *filename){
#ifdef TRANSGAMING_PSP
    SceKernelModuleInfo mi;
    SceUID              id = sceKernelGetModuleId();
    int                 error;
    void *              safeSpaceStart;
    void *              safeSpaceEnd;
    void *              textSpaceStart;
    FILE_T              fp;
    size_t              allocatedMemory;
    size_t              textSize;
    size_t              heapSize;
    char                fname[_MAX_PATH];



    memset(&mi, 0, sizeof(mi));

    if (id <= 0){
        TRACE("Memory::SafeMemory::saveState(): ERROR-> could not retrieve the module ID!\n");

        return false;
    }

    
    //grab the module information
    error = sceKernelQueryModuleInfo(id, &mi);

    if (error != SCE_KERNEL_ERROR_OK){
        TRACE("Memory::SafeMemory::saveState(): ERROR-> could not retrieve the module info to save the program state!\n");

        return false;
    }


    //make sure we have a text segment!
    Assert(mi.nsegment >= 2);

    //calculate the important data offsets
    safeSpaceStart = &safe;
    safeSpaceEnd = (&safe + 1);
    textSpaceStart = reinterpret_cast<void *>(mi.segmentaddr[1]);
    Assert(((unsigned int)safeSpaceStart) > ((unsigned int)mi.segmentaddr[1]));

    heapSize = ((mi.segmentaddr[1] + mi.segmentsize[1]) - ((size_t)safeSpaceEnd)) + (sce_newlib_heap_kb_size * 1024);
    textSize = (size_t)(((int)safeSpaceStart) - mi.segmentaddr[1]);
    allocatedMemory = mallinfo().uordblks;



    //apply the correct path name to the file
    snprintf(fname, _MAX_PATH, "host0:States/%s", filename);
    TRACE("Memory::SafeMemory::saveState(): saving game state to '%s' {safeStart = %p, safeEnd = %p, textStart = %p, textSize = %d, heapSize = %d, allocated = %d}\n", fname, safeSpaceStart, safeSpaceEnd, textSpaceStart, textSize, heapSize, allocatedMemory);

    //open the file
    fp = OPEN(fname, SCE_O_WRONLY | SCE_O_CREAT | SCE_O_TRUNC);

    if (fp < 0){
        TRACE("Memory::SafeMemory::saveState(): ERROR-> could not open the file '%s' for writing! (error = %d (0x%08X))\n", fname, fp, fp);

        return false;
    }


    /*
    Header =    ["STAT" | <moduleCRC>:4 | <entryPoint>:4 | <textAddress>:4 | <textSize>:4 | 
                         <dataSize>:4 | <bssSize>:4 | <codeSegmentAddress>:4 | <codeSize>:4 | 
                         <currentMallocSize>:4]
    TextBlock = ["TEXT" | <textSegmentAddress>:4 | <textSegmentSize>:4 | <textStoredByteCount>:4 |
                         <textSegmentData>:textStoredByteCount]
    HeapBlock = ["HEAP" | <heapAddress>:4 | <heapStoredByteCount>:4 | <heapData>:heapStoredByteCount]
    //*/
    
    //calculate the CRC of the code segment
    if (moduleCRC == 0)
        calcCRC();

    //write out the header information
    WRITEFCC(fp, FCC_STATE);
    OUTP(fp, moduleCRC);
    OUTP(fp, mi.entry_addr);
    OUTP(fp, mi.text_addr);
    OUTP(fp, mi.text_size);
    OUTP(fp, mi.data_size);
    OUTP(fp, mi.bss_size);
    OUTP(fp, mi.segmentaddr[0]);
    OUTP(fp, mi.segmentsize[0]);
    OUTP(fp, allocatedMemory);


    //write out the text block header
    WRITEFCC(fp, FCC_TEXT);
    OUTP(fp, textSpaceStart);
    OUTP(fp, mi.segmentsize[1]);
    OUTP(fp, textSize);
    WRITE(fp, textSpaceStart, textSize);


    //write out the heap block
    WRITEFCC(fp, FCC_HEAP);
    OUTP(fp, safeSpaceEnd);
    OUTP(fp, heapSize);
    WRITE(fp, safeSpaceEnd, heapSize);   


    CLOSE(fp);
    TRACE("Memory::SafeMemory::saveState(): done saving the memory state to '%s'\n", fname);
    return true;
#elif defined(TRANSGAMING_PS2)
    return true;
#endif
}

bool SafeMemory::loadState(const char *filename){
#ifdef TRANSGAMING_PSP
    SceKernelModuleInfo mi;
    StateHeader         header;
    SceUID              id = sceKernelGetModuleId();
    int                 error;
    FCC                 fcc;
    void *              safeSpaceStart;
    void *              safeSpaceEnd;
    void *              textSpaceStart;
    FILE_T              fp;
    size_t              allocatedMemory;
    size_t              textSize;
    size_t              heapSize;
    size_t              newPtr;
    size_t              newSize;
    size_t              newTextSize;
    char                fname[_MAX_PATH];
    


    memset(&mi, 0, sizeof(mi));

    if (id <= 0){
        TRACE("Memory::SafeMemory::loadState(): ERROR-> could not retrieve the module ID!\n");

        return false;
    }

    
    //grab the module information
    error = sceKernelQueryModuleInfo(id, &mi);

    if (error != SCE_KERNEL_ERROR_OK){
        TRACE("Memory::SafeMemory::loadState(): ERROR-> could not retrieve the module info to load the program state!\n");

        return false;
    }


    //make sure we have a text segment!
    Assert(mi.nsegment >= 2);

    //calculate the important data offsets
    safeSpaceStart = &safe;
    safeSpaceEnd = (&safe + 1);
    textSpaceStart = reinterpret_cast<void *>(mi.segmentaddr[1]);
    Assert(((unsigned int)safeSpaceStart) > ((unsigned int)mi.segmentaddr[1]));

    heapSize = ((mi.segmentaddr[1] + mi.segmentsize[1]) - ((size_t)safeSpaceEnd)) + (sce_newlib_heap_kb_size * 1024);
    textSize = (size_t)(((int)safeSpaceStart) - mi.segmentaddr[1]);
    allocatedMemory = mallinfo().uordblks;



    //apply the correct path name to the file
    snprintf(fname, _MAX_PATH, "host0:States/%s", filename);
    TRACE("Memory::SafeMemory::loadState(): loading game state from '%s' {safeStart = %p, safeEnd = %p, textStart = %p, textSize = %d, heapSize = %d, allocated = %d}\n", fname, safeSpaceStart, safeSpaceEnd, textSpaceStart, textSize, heapSize, allocatedMemory);


    //open the file
    fp = OPEN(fname, SCE_O_RDONLY);

    if (fp < 0){
        TRACE("Memory::SafeMemory::loadState(): ERROR-> could not open the file '%s' for reading! (error = %d (0x%08X))\n", fname, fp, fp);

        return false;
    }


    /*
    Header =    ["STAT" | <moduleCRC>:4 | <entryPoint>:4 | <textAddress>:4 | <textSize>:4 | 
                         <dataSize>:4 | <bssSize>:4 | <codeSegmentAddress>:4 | <codeSize>:4 | 
                         <currentMallocSize>:4]
    TextBlock = ["TEXT" | <textSegmentAddress>:4 | <textSegmentSize>:4 | <textStoredByteCount>:4 |
                         <textSegmentData>:textStoredByteCount]
    HeapBlock = ["HEAP" | <heapAddress>:4 | <heapStoredByteCount>:4 | <heapData>:heapStoredByteCount]
    //*/

    //read the file header
    INP(fp, header);

    //verify all the information in the file header to make sure this memory image can be loaded
    //******************* tests a single header attribute and fails if different *********************//
#define TEST_ERROR(name, var1, var2, action) \
        if ((var1) != (var2)){ \
            TRACE("Memory::SafeMemory::loadState(): ERROR-> " name " does not match! {" #var1 " = 0x%08X, " #var2 " = 0x%08X}\n", var1, var2); \
            action; \
        }
    //************************************************************************************************//

    TEST_ERROR("file signature",            FCC_STATE,              header.fcc,         return false);
    TEST_ERROR("entry point address",       mi.entry_addr,          header.entryPoint,  return false);
    TEST_ERROR("text segment address",      mi.text_addr,           header.textAddress, return false);
    TEST_ERROR("text segment size",         mi.text_size,           header.textSize,    return false);
    TEST_ERROR("data segment size",         mi.data_size,           header.dataSize,    return false);
    TEST_ERROR("BSS segment size",          mi.bss_size,            header.bssSize,     return false);
    TEST_ERROR("code segment address",      mi.segmentaddr[0],      header.codeAddress, return false);
    TEST_ERROR("code segment size",         mi.segmentsize[0],      header.codeSize,    return false);
    TEST_ERROR("module CRC",                moduleCRC,              header.moduleCRC,   return false);


    //read in the text block header
    READFCC(fp, fcc);
    INP(fp, newPtr);
    INP(fp, newSize);
    INP(fp, newTextSize);

    TEST_ERROR("text block signature",      FCC_TEXT,               fcc,                return false);
    TEST_ERROR("text segment address",      (size_t)textSpaceStart, newPtr,             return false);
    TEST_ERROR("text data segment size",    mi.segmentsize[1],      newSize,            return false);
    TEST_ERROR("text data read size",       textSize,               newTextSize,        return false);

    //start reading the state data with the text data segment
    READ(fp, textSpaceStart, newTextSize);


    //write out the heap block
    READFCC(fp, fcc);
    INP(fp, newPtr);
    INP(fp, newSize);

    //check to make sure the heap values are correct.  If any of them are incorrect, some very very very 
    //very (very very) horrible things will occur after this point because we've already clobbered most
    //of the text data space.  Lots of pointers could already be wrong!  If this signature is not found,
    //exit immediately to prevent anything exceedingly bad from occuring.
    TEST_ERROR("heap block signature",      FCC_HEAP,               fcc,                exit(-1));
    TEST_ERROR("heap data address",         (size_t)safeSpaceEnd,   newPtr,             exit(-1));
    TEST_ERROR("heap data read size",       heapSize,               newSize,            exit(-1));

    READ(fp, safeSpaceEnd, heapSize);   
    //************************************************************************************************//
#undef TEST_ERROR
    //************************************************************************************************//


    CLOSE(fp);
    TRACE("Memory::SafeMemory::loadState(): done loading the memory state from '%s'\n", fname);
    return true;
#elif defined(TRANSGAMING_PS2)
    return true;
#endif
}


#undef OUTP
#undef INP
#undef WRITEFCC
#undef READFCC
#undef READ
#undef WRITE
#undef OPEN
#undef CLOSE
#undef FILE_T

}
