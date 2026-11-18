// ---------------------------------------------------------------------------------------------------------------------------------
//                                                      
//                                                      
//  _ __ ___  _ __ ___   __ _ _ __      ___ _ __  _ __  
// | '_ ` _ \| '_ ` _ \ / _` | '__|    / __| '_ \| '_ \ 
// | | | | | | | | | | | (_| | |    _ | (__| |_) | |_) |
// |_| |_| |_|_| |_| |_|\__, |_|   (_) \___| .__/| .__/ 
//                       __/ |             | |   | |    
//                      |___/              |_|   |_|    
//
// Memory manager & tracking software
//
// Best viewed with 8-character tabs and (at least) 132 columns
//
// ---------------------------------------------------------------------------------------------------------------------------------
//
// Restrictions & freedoms pertaining to usage and redistribution of this software:
//
//  * This software is 100% free
//  * If you use this software (in part or in whole) you must credit the author.
//  * This software may not be re-distributed (in part or in whole) in a modified
//    form without clear documentation on how to obtain a copy of the original work.
//  * You may not use this software to directly or indirectly cause harm to others.
//  * This software is provided as-is and without warrantee. Use at your own risk.
//
// For more information, visit HTTP://www.FluidStudios.com
//
// ---------------------------------------------------------------------------------------------------------------------------------
// Originally created on 12/22/2000 by Paul Nettle
//
// Copyright 2000, Fluid Studios, Inc., all rights reserved.
// ---------------------------------------------------------------------------------------------------------------------------------
//
// !!IMPORTANT!!
//
// This software is self-documented with periodic comments. Before you start using this software, perform a search for the string
// "-DOC-" to locate pertinent information about how to use this software.
//
// You are also encouraged to read the comment blocks throughout this source file. They will help you understand how this memory
// tracking software works, so you can better utilize it within your applications.
//
// NOTES:
//
// 1. If you get compiler errors having to do with set_new_handler, then go through this source and search/replace
//    "std::set_new_handler" with "set_new_handler".
//
// 2. This code purposely uses no external routines that allocate RAM (other than the raw allocation routines, such as malloc). We
//    do this because we want this to be as self-contained as possible. As an example, we don't use assert, because when running
//    under WIN32, the assert brings up a dialog box, which allocates RAM. Doing this in the middle of an allocation would be bad.
//
// 3. When trying to override new/delete under MFC (which has its own version of global new/delete) the linker will complain. In
//    order to fix this error, use the compiler option: /FORCE, which will force it to build an executable even with linker errors.
//    Be sure to check those errors each time you compile, otherwise, you may miss a valid linker error.
//
// 4. If you see something that looks odd to you or seems like a strange way of going about doing something, then consider that this
//    code was carefully thought out. If something looks odd, then just assume I've got a good reason for doing it that way (an
//    example is the use of the class MemStaticTimeTracker.)
//
// 5. With MFC applications, you will need to comment out any occurance of "#define new DEBUG_NEW" from all source files.
//
// 6. Include file dependencies are _very_important_ for getting the MMGR to integrate nicely into your application. Be careful if
//    you're including standard includes from within your own project inclues; that will break this very specific dependency order. 
//    It should look like this:
//
//		#include <stdio.h>   // Standard includes MUST come first
//		#include <stdlib.h>  //
//		#include <streamio>  //
//
//		#include "mmgr.h"    // mmgr.h MUST come next
//
//		#include "myfile1.h" // Project includes MUST come last
//		#include "myfile2.h" //
//		#include "myfile3.h" //
//
// ---------------------------------------------------------------------------------------------------------------------------------
#include "Engine.h"

#if defined( TRANSGAMING_PS2 )
#include <unistd.h>
#include <tg_pc2ps2.h>
#include <TNG_stdio_PS2.h>
#include <malloc.h>
#elif defined( TRANSGAMING_PSP )
#include <kernel.h>
#include <unistd.h>
#include <tg_pc2psp.h>
#include "TNG_stdio_PSP.h"
#elif defined( TRANSGAMING_WINDOWS )
#include "../TNG_WIN/TNG_stdio_WIN.h"
#endif

#include <malloc.h>

#ifdef MEMORY_TEST

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <new>

#define UTTYPE_UKN		0x00
#define UTTYPE_HAVOK	0x01
#define UTTYPE_PACKET	0x02
#define UTTYPE_LUA		0x03
#define UTTYPE_MAX		0x08

//#define TRACK_UNTRACKEDMEMORY		//PSP crashes...

static bool AllocatorBusy = false;
#ifdef TRACK_UNTRACKEDMEMORY

typedef struct {
	const char		*Name;
	int				Size;
}UNTMEM;

UNTMEM	UntrackedMemory[UTTYPE_MAX]	= 
{
		{"Unknown",				0},		//UTTYPE_UKN
		{"Havok",				0},		//UTTYPE_HAVOK
		{"PS2GL DMA packet's",	0},		//UTTYPE_PACKET
		{"LUA",					0},		//UTTYPE_LUA
		{"",					0},		//spare
		{"",					0},		//spare
		{"",					0},		//spare
		{"",					0},		//spare
};

int		UTType				= UTTYPE_UKN;

void GetUTType(const char *Tag)
{
	UTType	= UTTYPE_UKN;

	if (Tag)
	{
		if (strcmp(Tag, "Packet")==0)		UTType	= UTTYPE_PACKET;
		if (strcmp(Tag, "Physics")==0)		UTType	= UTTYPE_HAVOK;
		if (strcmp(Tag, "LUA")==0)			UTType	= UTTYPE_LUA;
	}
}
#endif

//joe
/*
#ifdef TRANSGAMING_PS2
#include <unistd.h>
#include <tg_pc2ps2.h>
#include <TNG_stdio_PS2.h>
#endif

#ifdef TRANSGAMING_PSP
#include <unistd.h>
#include <tg_pc2psp.h>
#include "TNG_stdio_PSP.h"
#endif
*/


// ---------------------------------------------------------------------------------------------------------------------------------
// -DOC- If you're like me, it's hard to gain trust in foreign code. This memory manager will try to INDUCE your code to crash (for
// very good reasons... like making bugs obvious as early as possible.) Some people may be inclined to remove this memory tracking
// software if it causes crashes that didn't exist previously. In reality, these new crashes are the BEST reason for using this
// software!
//
// Whether this software causes your application to crash, or if it reports errors, you need to be able to TRUST this software. To
// this end, you are given some very simple debugging tools.
// 
// The quickest way to locate problems is to enable the STRESS_TEST macro (below.) This should catch 95% of the crashes before they
// occur by validating every allocation each time this memory manager performs an allocation function. If that doesn't work, keep
// reading...
//
// If you enable the TEST_MEMORY_MANAGER #define (below), this memory manager will log an entry in the memory.log file each time it
// enters and exits one of its primary allocation handling routines. Each call that succeeds should place an "ENTER" and an "EXIT"
// into the log. If the program crashes within the memory manager, it will log an "ENTER", but not an "EXIT". The log will also
// report the name of the routine.
//
// Just because this memory manager crashes does not mean that there is a bug here! First, an application could inadvertantly damage
// the heap, causing malloc(), realloc() or free() to crash. Also, an application could inadvertantly damage some of the memory used
// by this memory tracking software, causing it to crash in much the same way that a damaged heap would affect the standard
// allocation routines.
//
// In the event of a crash within this code, the first thing you'll want to do is to locate the actual line of code that is
// crashing. You can do this by adding log() entries throughout the routine that crashes, repeating this process until you narrow
// in on the offending line of code. If the crash happens in a standard C allocation routine (i.e. malloc, realloc or free) don't
// bother contacting me, your application has damaged the heap. You can help find the culprit in your code by enabling the
// STRESS_TEST macro (below.)
//
// If you truely suspect a bug in this memory manager (and you had better be sure about it! :) you can contact me at
// midnight@FluidStudios.com. Before you do, however, check for a newer version at:
//
//	http://www.FluidStudios.com/publications.html
//
// When using this debugging aid, make sure that you are NOT setting the alwaysLogAll variable on, otherwise the log could be
// cluttered and hard to read.
// ---------------------------------------------------------------------------------------------------------------------------------

//#define	TEST_MEMORY_MANAGER

// ---------------------------------------------------------------------------------------------------------------------------------
// -DOC- Enable this sucker if you really want to stress-test your app's memory usage, or to help find hard-to-find bugs
// ---------------------------------------------------------------------------------------------------------------------------------

//#define	STRESS_TEST

// ---------------------------------------------------------------------------------------------------------------------------------
// -DOC- Enable this sucker if you want to stress-test your app's error-handling. Set RANDOM_FAIL to the percentage of failures you
//       want to test with (0 = none, >100 = all failures).
// ---------------------------------------------------------------------------------------------------------------------------------

//#define	RANDOM_FAILURE 10.0

// ---------------------------------------------------------------------------------------------------------------------------------
// -DOC- Locals -- modify these flags to suit your needs
// ---------------------------------------------------------------------------------------------------------------------------------
#define USE_PERSISTENT_MEMORY_LOG

static  MemorySection       m_isInGame             = MEM_SYSTEM;

#ifdef	STRESS_TEST
static  const unsigned int  hashBits               = 24;
static  bool                randomWipe             = true;
static  bool                alwaysValidateAll      = true;
static  bool                alwaysLogAll           = true;
static  bool                alwaysWipeAll          = true;
static  bool                cleanupLogOnFirstRun   = true;
static  const unsigned int	paddingSize            = 1024; // An extra 8K per allocation!
#else

# ifdef TRANSGAMING
/* 
BLAIR - This hashBits value is an accomplice to a huge memory allocation below, in the variable *hashTable[hashSize], where hashSize = 1 << hashBits.  This made our PSP .prx file implode at load time the memory was getting allocated, and the app could not fit on the devkit.  Now, 8 ** WILL NOT ** be a final value for this variable, for either we'll find a way to allocate the space later, or we might not use this memory management at all.  For now ( 01/11/06 ), we will set it here, so that we may load our beloved program, and start debugging.
*/ 
static  const unsigned int      hashBits               = 8;
# else
static const unsigned int       hashBits               = 24;
# endif // TRANSGAMING
static bool                     cleanupLogOnFirstRun   = true;
static const int                reservoirBufferGrow    = 4;
# ifdef ENABLE_VALIDATION
static bool                     randomWipe             = false;
static bool                     alwaysValidateAll      = true;
static bool                     alwaysWipeAll          = true;
static bool                     alwaysLogAll           = true;
static const unsigned int       paddingSize            = 4;
# else
static bool                     randomWipe             = false;
static bool                     alwaysValidateAll      = false;
static bool                     alwaysWipeAll          = false;
static bool                     alwaysLogAll           = false;
static const unsigned int       paddingSize            = 4;
# endif
#endif

// ---------------------------------------------------------------------------------------------------------------------------------
// We define our own assert, because we don't want to bring up an assertion dialog, since that allocates RAM. Our new assert
// simply declares a forced breakpoint.
//
// The BEOS assert added by Arvid Norberg <arvid@iname.com>.
// ---------------------------------------------------------------------------------------------------------------------------------

#ifdef	WIN32
# ifdef	_DEBUG
#  define	m_assert(x) if ((x) == false) __asm { int 3 }
# else
static char tmp_buf[255];
#  define	m_assert(x) if ((x) == false) {itoa(__LINE__,tmp_buf,10); MessageBox(NULL,tmp_buf,"Memory Assert Failed!",MB_TOPMOST); __asm { int 3 }}
# endif
#elif defined(__BEOS__)
# ifdef DEBUG
extern void debugger(const char *message);
#  define	m_assert(x) if ((x) == false) debugger("mmgr: assert failed")
# else
#  define m_assert(x) {}
# endif
#else	// Linux uses assert, which we can use safely, since it doesn't bring up a dialog within the program.
// eric-> the PSP debugger doesn't properly break on assert calls (call stack usually can't be found), so we'll
//   cause a deliberate seg fault here instead of calling assert().
# define	m_assert(cond) { \
                if (!(cond)){ \
                    printf("%s (%d)::%s(): ASSERT-> '%s' failed!\n", __FILE__, __LINE__, __FUNCTION__, #cond); \
                    char *x = NULL; \
                    x[3] = 14; \
                    } \
                } 
                //assert(cond)
#endif

// ---------------------------------------------------------------------------------------------------------------------------------
// Here, we turn off our macros because any place in this source file where the word 'new' or the word 'delete' (etc.)
// appear will be expanded by the macro. So to avoid problems using them within this source file, we'll just #undef them.
// ---------------------------------------------------------------------------------------------------------------------------------

#undef	new
#undef	delete
#undef	malloc
#undef	calloc
#undef	realloc
#undef	free
#undef  memalign

// ---------------------------------------------------------------------------------------------------------------------------------
// Defaults for the constants & statics in the MemoryManager class
// ---------------------------------------------------------------------------------------------------------------------------------

const		unsigned int	m_alloc_unknown        = 0;
const		unsigned int	m_alloc_new            = 1;
const		unsigned int	m_alloc_new_array      = 2;
const		unsigned int	m_alloc_malloc         = 3;
const		unsigned int	m_alloc_calloc         = 4;
const		unsigned int	m_alloc_realloc        = 5;
const		unsigned int	m_alloc_delete         = 6;
const		unsigned int	m_alloc_delete_array   = 7;
const		unsigned int	m_alloc_free           = 8;
const	    unsigned int	m_alloc_memalign       = 9;

// ---------------------------------------------------------------------------------------------------------------------------------
// -DOC- Get to know these values. They represent the values that will be used to fill unused and deallocated RAM.
// ---------------------------------------------------------------------------------------------------------------------------------

static		unsigned int	prefixPattern          = 0xbaadf00d; // Fill pattern for bytes preceeding allocated blocks
static		unsigned int	postfixPattern         = 0xdeadc0de; // Fill pattern for bytes following allocated blocks
static		unsigned int	unusedPattern          = 0xfeedface; // Fill pattern for freshly allocated blocks
static		unsigned int	releasedPattern        = 0xdeadbeef; // Fill pattern for deallocated blocks

// ---------------------------------------------------------------------------------------------------------------------------------
// Other locals
// ---------------------------------------------------------------------------------------------------------------------------------
static const unsigned int hashSize               = 1 << hashBits;

static const char *     m_allocPositionNames[MEM_COUNT + 1] = {
    "System Memory:",
    "Menu Memory:  ",
    "Level Memory: ",
    "Unknown:      "
};
static const char *     allocationTypes[] = {"Unknown",
                                             "new",
                                             "new[]",
                                             "malloc",
                                             "calloc",
                                             "realloc",
                                             "delete",
                                             "delete[]",
                                             "free",
                                             "memalign"};
static sAllocUnit *     hashTable[hashSize]     = {0};
static sAllocUnit *     reservoir               = NULL;
static unsigned int	    currentAllocationCount  = 0;
static unsigned int     currentBlockCount       = 0;
static unsigned int	    breakOnAllocationCount  = 0;
static sMStats		    stats                   = {0};
static const char *     m_unknownTag            = "??";
static const char *     m_uninitializedTag      = "??@!#$^&^!@";
static const char *     sourceTag               = "";
static const char *     sourceFile              = m_uninitializedTag;
static const char *     sourceFunc              = m_uninitializedTag;
static unsigned int	    sourceLine              = 0;
static bool             staticDeinitTime        = false;
static sAllocUnit **    reservoirBuffer         = NULL;
static unsigned int     reservoirBufferSize     = 0;
static unsigned int     reservoirBufferMaxSize  = 0;
static const char *     memoryLogFile           = "memory.log";
static const char *     memoryLeakLogFile       = "memleaks.log";
#ifdef USE_PERSISTENT_MEMORY_LOG
static FILE *           g_memoryLogFile         = NULL;
#endif

static void		        doCleanupLogOnFirstRun();


// ---------------------------------------------------------------------------------------------------------------------------------
// Tag Grouping functions
// ---------------------------------------------------------------------------------------------------------------------------------
typedef struct{
    const char *    name;
    int             totalCount;
    int             accumulatedCount;
    int             peakCount;
    size_t          totalSize;
    size_t          accumulatedSize;
    size_t          peakSize;
} TaggedStat;

static const int        MM_STARTING_TAG_COUNT = 8;
static const int        MM_TAG_TABLE_INCREMENT = 16;
static int              g_sourceTagCount = 0;
static int              g_tagTableMaxSize = 0;
static TaggedStat *     g_tagTable = NULL;

extern "C" void mmgr_SetInGame(MemorySection section)
{
	m_isInGame = section;
}

extern "C" void mmgr_setTag(const char *tag)
{
    if (tag == NULL)
        sourceTag = "";
    
    else
        sourceTag = tag;
}

TaggedStat *findTag(const char *name){

    //no table has been set up yet => fail
    if (g_tagTable == NULL)
        return NULL;


    for (int i = 0; i < g_sourceTagCount; i++){
        if (!strcasecmp(g_tagTable[i].name, name))
            return &g_tagTable[i];
    }

    return NULL;
}

TaggedStat *addTag(const char *name){
    TaggedStat *ts = findTag(name);


    if (ts)
        return ts;


    //tag table doesn't exist => create it with a few blocks to start
    if (g_tagTable == NULL){
        g_tagTable = (TaggedStat *)malloc(sizeof(TaggedStat) * MM_STARTING_TAG_COUNT);

        memset(g_tagTable, 0, sizeof(TaggedStat) * MM_STARTING_TAG_COUNT);
        
        g_tagTableMaxSize = MM_STARTING_TAG_COUNT;
        g_sourceTagCount = 0;
    }

    //tag table is full => expand it.
    else if (g_sourceTagCount >= g_tagTableMaxSize){
        g_tagTable = (TaggedStat *)realloc(g_tagTable, sizeof(TaggedStat) * (g_tagTableMaxSize + MM_TAG_TABLE_INCREMENT));

        g_tagTableMaxSize += MM_TAG_TABLE_INCREMENT;
    }


    //add in the new tag and reset all its info
    g_tagTable[g_sourceTagCount].name =             name;
    g_tagTable[g_sourceTagCount].totalCount =       0;
    g_tagTable[g_sourceTagCount].accumulatedCount = 0;
    g_tagTable[g_sourceTagCount].peakCount =        0;
    g_tagTable[g_sourceTagCount].accumulatedSize =  0;
    g_tagTable[g_sourceTagCount].totalSize =        0;
    g_tagTable[g_sourceTagCount].peakSize =         0;

    g_sourceTagCount++;

    return &g_tagTable[g_sourceTagCount - 1];
}

void cleanupTagTable(){
    if (g_tagTable)
        free(g_tagTable);

    g_tagTable = NULL;
    g_sourceTagCount = 0;
    g_tagTableMaxSize = 0;
}

// ---------------------------------------------------------------------------------------------------------------------------------
// Local functions only
// ---------------------------------------------------------------------------------------------------------------------------------
static void logf(const char *fmt, va_list &ap){
#ifdef TRANSGAMING
# ifdef USE_PERSISTENT_MEMORY_LOG
    if (g_memoryLogFile == NULL){
        //printf("logf(): opening log file '%s'\n", memoryLogFile);
	    g_memoryLogFile = TNG_fopen(memoryLogFile, "wb");

        if (g_memoryLogFile == NULL){
# else
        //printf("logf(): opening log file '%s'\n", memoryLogFile);
        FILE *fp = TNG_fopen(memoryLogFile, "ab");

        if (fp == NULL){
# endif

            //printf("logf(): log open failed\n");
            static bool msgShown = false;

            if (!msgShown){
                printf("!!!! [MMGR]::logf(): ERROR-> could not open memory log file '%s'!\n", memoryLogFile);   

                msgShown = true;
            }

            return;
        }

        /*else
            printf("logf(): log opened\n");//*/
# ifdef USE_PERSISTENT_MEMORY_LOG
    }
# endif

#else
	FILE	*fp = fopen(memoryLogFile, "ab");

	// If you hit this assert, then the memory logger is unable to log information to a file (can't open the file for some
	// reason.) You can interrogate the variable 'buffer' to see what was supposed to be logged (but won't be.)
	m_assert(fp);

	if (!fp)
        return;
#endif


    const int   BUFSIZE = 1024;    
    static char buffer[BUFSIZE];
    int         len;


    //printf("logf(): formatting text from '%s'\n", fmt);
    len = vsnprintf(buffer, BUFSIZE, fmt, ap);

	// Spit out the data to the log

#ifdef TRANSGAMING
# ifdef USE_PERSISTENT_MEMORY_LOG
    //printf("logf(): printing text '%s'\n", buffer);
	TNG_fwrite(buffer, 1, len, g_memoryLogFile);
    //printf("logf(): flushing log\n");
    TNG_fflush(g_memoryLogFile);
    //printf("logf(): done log print\n");
# else
    //printf("logf(): printing text '%s'\n", buffer);
	TNG_fwrite(buffer, 1, len, fp);
    //printf("logf(): closing log\n");
	TNG_fclose(fp);
    //printf("logf(): done log print\n");
# endif
#else
	fwrite(buffer, 1, len, fp);
	fclose(fp);
#endif
}

void m_logMsg(const char *fmt, ...){
	// Cleanup the log?

	if (cleanupLogOnFirstRun) 
        doCleanupLogOnFirstRun();

	// Build the buffer

	va_list	ap;
	va_start(ap, fmt);
	
	
	// Open the log file
    logf(fmt, ap);

    va_end(ap);    
}

#define log m_logMsg
/*static void log(const char *format, ...)
{
	// Cleanup the log?

	if (cleanupLogOnFirstRun) doCleanupLogOnFirstRun();

	// Build the buffer

	va_list	ap;
	va_start(ap, format);
	
	
	// Open the log file
    logf(format, ap);

    va_end(ap);
}//*/

// ---------------------------------------------------------------------------------------------------------------------------------

static	void	doCleanupLogOnFirstRun()
{
    //printf("doCleanupLogOnFirstRun(): opening the log file in '%s'\n", memoryLogFile);
	if (cleanupLogOnFirstRun)
	{
#ifdef TRANSGAMING
		unlink(memoryLogFile);
#else
		unlink(memoryLogFile);
#endif
		cleanupLogOnFirstRun = false;

		// Print a header for the log

		time_t	t = time(NULL);
		m_setTag("PHYSFS");
		log("--------------------------------------------------------------------------------\r\n");
		log("\r\n");
		log("      %s - Memory logging file created on %s\r\n", memoryLogFile, asctime(localtime(&t)));
		log("--------------------------------------------------------------------------------\r\n");
		log("\r\n");
		log("This file contains a log of all memory operations performed during the last run.\r\n");
		log("\r\n");
		log("Interrogate this file to track errors or to help track down memory-related\r\n");
		log("issues. You can do this by tracing the allocations performed by a specific owner\r\n");
		log("or by tracking a specific address through a series of allocations and\r\n");
		log("reallocations.\r\n");
		log("\r\n");
		log("There is a lot of useful information here which, when used creatively, can be\r\n");
		log("extremely helpful.\r\n");
		log("\r\n");
		log("Note that the following guides are used throughout this file:\r\n");
		log("\r\n");
		log("   [!] - Error\r\n");
		log("   [+] - Allocation\r\n");
		log("   [~] - Reallocation\r\n");
		log("   [-] - Deallocation\r\n");
		log("   [I] - Generic information\r\n");
		log("   [F] - Failure induced for the purpose of stress-testing your application\r\n");
		log("   [D] - Information used for debugging this memory manager\r\n");
		log("\r\n");
		log("...so, to find all errors in the file, search for \"[!]\"\r\n");
		log("\r\n");
		log("--------------------------------------------------------------------------------\r\n");
	}
    //printf("doCleanupLogOnFirstRun(): done opening log\n");
}

// ---------------------------------------------------------------------------------------------------------------------------------

static	const char	*sourceFileStripper(const char *sourceFile)
{
	char	*ptr = strrchr(sourceFile, '\\');
	if (ptr) return ptr + 1;
	ptr = strrchr(sourceFile, '/');
	if (ptr) return ptr + 1;
	return sourceFile;
}

// ---------------------------------------------------------------------------------------------------------------------------------

static	const char	*ownerString(const char *sourceFile, const unsigned int sourceLine, const char *sourceFunc, const char *sourceTag)
{
    const int   BUFSIZE = 256;
	static char str[BUFSIZE];


	//memset(str, 0, sizeof(str));

    if (sourceTag)
        snprintf(str, BUFSIZE, "[%s (%05d)]::%s() (%s)", sourceFileStripper(sourceFile), sourceLine, sourceFunc, sourceTag);
    else
	    snprintf(str, BUFSIZE, "[%s (%05d)]::%s()", sourceFileStripper(sourceFile), sourceLine, sourceFunc);

	return str;
}

// ---------------------------------------------------------------------------------------------------------------------------------

static	const char	*insertCommas(unsigned int value)
{
    const int   BUFSIZE = 30;
	static char str[BUFSIZE];
	
    
    memset(str, 0, sizeof(str));

	size_t len = snprintf(str, BUFSIZE, "%u", value);


    if (len < 0){
#ifdef DEBUG
        printf("insertCommas(): ERROR-> buffer overflow!  That's one freakin' huge number: '%d'\n", value);
        m_assert(0);
#endif
        str[0] = 0;

        return str;
    }


	if (len > 3){
		memmove(&str[len-3], &str[len-4], 4);
        len++;
		str[len - 4] = ',';
	}
	if (len > 7){
		memmove(&str[len-7], &str[len-8], 8);
        len++;
		str[len - 8] = ',';
	}
	if (len > 11){
		memmove(&str[len-11], &str[len-12], 12);
        len++;
		str[len - 12] = ',';
	}

    //make sure the string is properly terminated
    str[len] = 0;

	return str;
}

// ---------------------------------------------------------------------------------------------------------------------------------

static	const char	*memorySizeString(ulong32 size)
{
    const int   BUFSIZE = 48;
	static char str[BUFSIZE];


    if (size > (1024*1024)) snprintf(str, BUFSIZE, "%10s (%7.2fM)",      insertCommas(size), static_cast<float>(size) / (1024.0f * 1024.0f));
    else if (size > 1024)   snprintf(str, BUFSIZE, "%10s (%7.2fK)",      insertCommas(size), static_cast<float>(size) / 1024.0f);
    else                    snprintf(str, BUFSIZE, "%10s bytes     ",    insertCommas(size));
	
    return str;
}

// ---------------------------------------------------------------------------------------------------------------------------------

static	sAllocUnit	*findAllocUnit(const void *reportedAddress)
{
	// Just in case...
	m_assert(reportedAddress != NULL);

	// Use the address to locate the hash index. Note that we shift off the lower four bits. This is because most allocated
	// addresses will be on four-, eight- or even sixteen-byte boundaries. If we didn't do this, the hash index would not have
	// very good coverage.

	unsigned int	hashIndex = (reinterpret_cast<unsigned int>(const_cast<void *>(reportedAddress)) >> 4) & (hashSize - 1);
	sAllocUnit	*ptr = hashTable[hashIndex];
	while(ptr)
	{
		if (ptr->reportedAddress == reportedAddress) return ptr;
		ptr = ptr->next;
	}

	return NULL;
}

// ---------------------------------------------------------------------------------------------------------------------------------

static	size_t	calculateActualSize(const size_t reportedSize, const size_t align = 0)
{
	// We use DWORDS as our padding, and a long is guaranteed to be 4 bytes, but an int is not (ANSI defines an int as
	// being the standard word size for a processor; on a 32-bit machine, that's 4 bytes, but on a 64-bit machine, it's
	// 8 bytes, which means an int can actually be larger than a long32.)

    //one padding block is added before the block, and one after.  The one before the block must respect the current
    //alignment setting.

    if (align){
        size_t size = paddingSize * sizeof(DWORD);
        size_t a = size & (align - 1);

        if (a)
            size += align - a;


	    return size + reportedSize + paddingSize * sizeof(DWORD);
    }

	return reportedSize + paddingSize * sizeof(DWORD) * 2;
}

// ---------------------------------------------------------------------------------------------------------------------------------

static	size_t	calculateReportedSize(const size_t actualSize)
{
    // NOTE: this must only be called for reallocations since it doesn't take into account the alignment padding size

    // We use DWORDS as our padding, and a long is guaranteed to be 4 bytes, but an int is not (ANSI defines an int as
	// being the standard word size for a processor; on a 32-bit machine, that's 4 bytes, but on a 64-bit machine, it's
	// 8 bytes, which means an int can actually be larger than a long.)

	return actualSize - paddingSize * sizeof(DWORD) * 2;
}

// ---------------------------------------------------------------------------------------------------------------------------------

static	void	*calculateReportedAddress(const void *actualAddress, const size_t align = 0)
{
	// We allow this...

	if (!actualAddress) return NULL;

    if (align){
        size_t size = paddingSize * sizeof(DWORD);
        size_t a = size & (align - 1);

        if (a)
            size += align - a;


	    return reinterpret_cast<void *>(const_cast<char *>(reinterpret_cast<const char *>(actualAddress) + size));
    }

	// JUst account for the padding

	return reinterpret_cast<void *>(const_cast<char *>(reinterpret_cast<const char *>(actualAddress) + sizeof(DWORD) * paddingSize));
}

// ---------------------------------------------------------------------------------------------------------------------------------

static	void	wipeWithPattern(sAllocUnit *allocUnit, DWORD pattern, const unsigned int originalReportedSize = 0)
{
// BLAIR - This memory test ended up blowing our memory usage, and the app would crash.  We can review soon enough, whether we'll need this around for the console projects.
#ifdef ENABLE_VALIDATION
	// For a serious test run, we use wipes of random a random value. However, if this causes a crash, we don't want it to
	// crash in a differnt place each time, so we specifically DO NOT call srand. If, by chance your program calls srand(),
	// you may wish to disable that when running with a random wipe test. This will make any crashes more consistent so they
	// can be tracked down easier.

	/*if (randomWipe)
	{
		pattern = ((rand() & 0xff) << 24) | ((rand() & 0xff) << 16) | ((rand() & 0xff) << 8) | (rand() & 0xff);
	}//*/

	// -DOC- We should wipe with 0's if we're not in debug mode, so we can help hide bugs if possible when we release the
	// product. So uncomment the following line for releases.
	//
	// Note that the "alwaysWipeAll" should be turned on for this to have effect, otherwise it won't do much good. But we'll
	// leave it this way (as an option) because this does slow things down.
//	pattern = 0;

	// This part of the operation is optional

	if (alwaysWipeAll && allocUnit->reportedSize > originalReportedSize){
		// Fill the bulk

		DWORD	*lptr = reinterpret_cast<DWORD *>(reinterpret_cast<char *>(allocUnit->reportedAddress) + originalReportedSize);
		int	length = static_cast<int>(allocUnit->reportedSize - originalReportedSize);
		int	i;
		for (i = 0; i < (length >> 2); i++, lptr++)
		{
			*lptr = pattern;
		}

		// Fill the remainder

		unsigned int	shiftCount = 0;
		char		*cptr = reinterpret_cast<char *>(lptr);
		for (i = 0; i < (length & 0x3); i++, cptr++, shiftCount += 8)
		{
			*cptr = static_cast<char>((pattern & (0xff << shiftCount)) >> shiftCount);
		}
	}

	// Write in the prefix/postfix bytes

    DWORD		*pre;
	DWORD		*post;
	

    //if this block was allocated using memalign(), ignore the alignment padding and make the block padding begin
    //<paddingSize> DWORDS before the reported address
    if (allocUnit->allocationType == m_alloc_memalign){
        pre = reinterpret_cast<DWORD *>(allocUnit->reportedAddress) - paddingSize;
        post = reinterpret_cast<DWORD *>(reinterpret_cast<char *>(allocUnit->reportedAddress) + allocUnit->reportedSize);
    }
    
    else{
        pre = reinterpret_cast<DWORD *>(allocUnit->actualAddress);
        post = reinterpret_cast<DWORD *>(reinterpret_cast<char *>(allocUnit->actualAddress) + allocUnit->actualSize - paddingSize * sizeof(DWORD));
    }
    

    for (unsigned int i = 0; i < paddingSize; i++, pre++, post++)
	{
		*pre = prefixPattern;
		*post = postfixPattern;
	}
#endif // ENABLE_VALIDATION
}

// ---------------------------------------------------------------------------------------------------------------------------------
static const char *insertRegExTags(const char *str){
    const int   BUFSIZE = 128;
    static char buf[BUFSIZE];


    for (int i = 0, j = 0; str[i] && j < BUFSIZE; i++){
        if (strchr("[]\\?.*<>:;()&^%$#@!|\"'{}", str[i]))
            buf[j++] = '\\';

        buf[j++] = str[i];
    }

    return buf;
}


static	void	dumpAllocations(FILE *fp, bool humanReadable)
{
#ifdef TRANSGAMING
# define fprintf TNG_fprintf
#endif
    if (fp == NULL)
        return;

    if (humanReadable){
	    /*fprintf(fp, "Alloc.    Addr       Size       Addr       Size             BreakOn BreakOn                     \r\n");
	    fprintf(fp, "Number  Reported   Reported    Actual     Actual    Method  Dealloc Realloc InGame Allocated by \r\n");
	    fprintf(fp, "------ ---------- ---------- ---------- ---------- -------- ------- ------- ------ -------------------------------------------- \r\n");//*/
        fprintf(fp, "Alloc.    Addr       Size       Addr       Size                                 \r\n");
	    fprintf(fp, "Number  Reported   Reported    Actual     Actual    Method  InGame Allocated by \r\n");
	    fprintf(fp, "------ ---------- ---------- ---------- ---------- -------- ------ -------------------------------------------- \r\n");
    }
    else{
        fprintf(fp, "\r\n");
	    /*fprintf(fp, "Alloc.,Addr,Size,Addr,Size,,BreakOn,BreakOn\r\n");
	    fprintf(fp, "Number,Reported,Reported,Actual,Actual,Method,Dealloc,Realloc,InGame,Allocated by\r\n");
	    fprintf(fp, "------,----------,----------,----------,----------,--------,-------,-------,------,--------------------------------------------------- \r\n");//*/
        fprintf(fp, "Alloc.,Addr,Size,Addr,Size,\r\n");
	    fprintf(fp, "Number,Reported,Reported,Actual,Actual,Method,InGame,Allocated by\r\n");
	    fprintf(fp, "------,----------,----------,----------,----------,--------,------,--------------------------------------------------- \r\n");
    }


    int         count = 0;
    const float allocFactor = 100.0f / stats.totalAllocUnitCount;
    int         done;
    int         lastDone = 0;
    const int   printRate = 5;
	int			totals[MEM_COUNT + 1] = {0};
    int         counts[MEM_COUNT + 1] = {0};
    char        allocPosition;


    if (humanReadable){
	    for (unsigned int i = 0; i < hashSize; i++)
	    {
		    sAllocUnit *ptr = hashTable[i];
		    while(ptr)
		    {
                switch (ptr->inGame){
                    case MEM_SYSTEM:
                        allocPosition = 'S';
                        break;

                    case MEM_MENU:
                        allocPosition = 'M';
                        break;

                    case MEM_LEVEL:
                        allocPosition = 'L';
                        break;

                    default:
                        allocPosition = 'X';
                        break;
                }

                totals[ptr->inGame] += ptr->actualSize;
                counts[ptr->inGame]++;

			    //fprintf(fp, "%06d 0x%08X %10d 0x%08X %10d %-8s    %c       %c     %c    %s\r\n",
			    fprintf(fp, "%06d 0x%08X %10d 0x%08X %10d %-8s    %c    %s\r\n",
				    ptr->allocationNumber,
				    reinterpret_cast<unsigned int>(ptr->reportedAddress), ptr->reportedSize,
				    reinterpret_cast<unsigned int>(ptr->actualAddress), ptr->actualSize,
				    allocationTypes[ptr->allocationType],
				    /*ptr->breakOnDealloc ? 'Y':'N',
				    ptr->breakOnRealloc ? 'Y':'N',//*/
				    allocPosition,
				    ownerString(ptr->sourceFile, ptr->sourceLine, ptr->sourceFunc, ptr->sourceTag));

			    ptr = ptr->next;

                count++;

                done = (int)(count * allocFactor);
                if (done - lastDone >= printRate){
                    lastDone = done;
                    printf("dumpAllocations(): %d%% done...\n", done);
                }
		    }
	    }
    }

    else{
	    for (unsigned int i = 0; i < hashSize; i++)
	    {
		    sAllocUnit *ptr = hashTable[i];
		    while(ptr)
		    {
                switch (ptr->inGame){
                    case MEM_SYSTEM:
                        allocPosition = 'S';
                        break;

                    case MEM_MENU:
                        allocPosition = 'M';
                        break;

                    case MEM_LEVEL:
                        allocPosition = 'L';
                        break;

                    default:
                        allocPosition = 'X';
                        break;
                }

                totals[ptr->inGame] += ptr->actualSize;
                counts[ptr->inGame]++;

                
                fprintf(fp, "%d,0x%08X,%d,0x%08X,%d,%s,%c,%s\r\n",
				    ptr->allocationNumber,
				    reinterpret_cast<unsigned int>(ptr->reportedAddress), ptr->reportedSize,
				    reinterpret_cast<unsigned int>(ptr->actualAddress), ptr->actualSize,
				    allocationTypes[ptr->allocationType],
				    /*ptr->breakOnDealloc ? 'Y':'N',
				    ptr->breakOnRealloc ? 'Y':'N',//*/
				    allocPosition,
				    ownerString(ptr->sourceFile, ptr->sourceLine, ptr->sourceFunc, ptr->sourceTag));

			    ptr = ptr->next;

                count++;

                done = (int)(count * allocFactor);
                if (done - lastDone >= printRate){
                    lastDone = done;
                    printf("dumpAllocations(): %d%% done...\n", done);
                }
            }
	    }


        /*fprintf(fp, "Tag Counts\r\n");
        fprintf(fp, "------------------\r\n");
        
        for (int i = 0; i < g_sourceTagCount; i++){
            fprintf(fp, "%s,=COUNTIF(I%d:I%d; \".*\\(%s\\).*\")\r\n",
                        g_tagTable[i].name,
                        g_sourceTagCount + 7,
                        g_sourceTagCount + 7 + count,
                        insertRegExTags(g_tagTable[i].name));
        }//*/

    }


	fprintf(fp, "\r\n\r\n");

    for (int i = 0; i < MEM_COUNT; i++)
        fprintf(fp, "    %s %dkb (%d allocations)\r\n", m_allocPositionNames[i], totals[i] / 1024, counts[i]);

	fprintf(fp, "\r\n\r\n");

#ifdef TRANSGAMING
# undef fprintf
#endif
}

// ---------------------------------------------------------------------------------------------------------------------------------

static	void	dumpLeakReport()
{
	// Open the report file

#ifdef TRANSGAMING
	FILE	*fp = TNG_fopen(memoryLeakLogFile, "wb");
# define fprintf TNG_fprintf
#else
	FILE	*fp = fopen(memoryLeakLogFile, "w+b");
#endif

	// If you hit this assert, then the memory report generator is unable to log information to a file (can't open the file for
	// some reason.)
	//m_assert(fp);
	if (!fp) return;

	// Any leaks?

	// Header

	/*static  char    timeString[25];
	memset(timeString, 0, sizeof(timeString));//*/
	time_t  t = time(NULL);
	struct  tm *tme = localtime(&t);
	fprintf(fp, " ---------------------------------------------------------------------------------------------------------------------------------- \r\n");
	fprintf(fp, "|                                          Memory leak report for:  %02d/%02d/%04d %02d:%02d:%02d                                            |\r\n", tme->tm_mon + 1, tme->tm_mday, tme->tm_year + 1900, tme->tm_hour, tme->tm_min, tme->tm_sec);
	fprintf(fp, " ---------------------------------------------------------------------------------------------------------------------------------- \r\n");
	fprintf(fp, "\r\n");
	fprintf(fp, "\r\n");
	if (stats.totalAllocUnitCount)
	{
		fprintf(fp, "%d memory leak%s found:\r\n", stats.totalAllocUnitCount, stats.totalAllocUnitCount == 1 ? "":"s");
	}
	else
	{
		fprintf(fp, "Congratulations! No memory leaks found!\r\n");

		// We can finally free up our own memory allocations

		if (reservoirBuffer)
		{
			for (unsigned int i = 0; i < reservoirBufferSize; i++)
			{
				free(reservoirBuffer[i]);
			}
			free(reservoirBuffer);
			reservoirBuffer = 0;
			reservoirBufferSize = 0;
            reservoirBufferMaxSize = 0;
			reservoir = NULL;
		}
	}
	fprintf(fp, "\r\n");

	if (stats.totalAllocUnitCount)
	{
		dumpAllocations(fp, true);
	}
#ifdef TRANSGAMING
# undef fprintf
	TNG_fclose(fp);
#else
	fclose(fp);
#endif
}


// ---------------------------------------------------------------------------------------------------------------------------------
// We use a static class to let us know when we're in the midst of static deinitialization
// ---------------------------------------------------------------------------------------------------------------------------------

class	MemStaticTimeTracker
{
public:
	MemStaticTimeTracker() {doCleanupLogOnFirstRun();}
	~MemStaticTimeTracker() {
        staticDeinitTime = true; 

# ifdef USE_PERSISTENT_MEMORY_LOG
        if (g_memoryLogFile)
            TNG_fclose(g_memoryLogFile);

        g_memoryLogFile = NULL;
# endif
        /*dumpLeakReport();*/
    }
};

static	MemStaticTimeTracker	mstt;

// ---------------------------------------------------------------------------------------------------------------------------------
// -DOC- Flags & options -- Call these routines to enable/disable the following options
// ---------------------------------------------------------------------------------------------------------------------------------
#ifdef ENABLE_VALIDATION
bool m_getValidateState(){
    return alwaysValidateAll;
}

void m_setValidateState(bool state){
    alwaysValidateAll = state;
}

bool m_getWipeState(){
    return alwaysWipeAll;
}

void m_setWipeState(bool state){
    alwaysWipeAll = state;
}
#endif


/*bool	&m_alwaysValidateAll()
{
	// Force a validation of all allocation units each time we enter this software
	return alwaysValidateAll;
}

// ---------------------------------------------------------------------------------------------------------------------------------

bool	&m_alwaysLogAll()
{
	// Force a log of every allocation & deallocation into memory.log
	return alwaysLogAll;
}

// ---------------------------------------------------------------------------------------------------------------------------------

bool	&m_alwaysWipeAll()
{
	// Force this software to always wipe memory with a pattern when it is being allocated/dallocated
	return alwaysWipeAll;
}

// ---------------------------------------------------------------------------------------------------------------------------------

bool	&m_randomeWipe()
{
	// Force this software to use a random pattern when wiping memory -- good for stress testing
	return randomWipe;
}//*/

// ---------------------------------------------------------------------------------------------------------------------------------
// -DOC- Simply call this routine with the address of an allocated block of RAM, to cause it to force a breakpoint when it is
// reallocated.
// ---------------------------------------------------------------------------------------------------------------------------------

/*bool	&m_breakOnRealloc(void *reportedAddress)
{
	// Locate the existing allocation unit

	sAllocUnit	*au = findAllocUnit(reportedAddress);

	// If you hit this assert, you tried to set a breakpoint on reallocation for an address that doesn't exist. Interrogate the
	// stack frame or the variable 'au' to see which allocation this is.
	m_assert(au != NULL);

	// If you hit this assert, you tried to set a breakpoint on reallocation for an address that wasn't allocated in a way that
	// is compatible with reallocation.
	m_assert(au->allocationType == m_alloc_malloc ||
		 au->allocationType == m_alloc_calloc ||
		 au->allocationType == m_alloc_realloc);

	return au->breakOnRealloc;
}

// ---------------------------------------------------------------------------------------------------------------------------------
// -DOC- Simply call this routine with the address of an allocated block of RAM, to cause it to force a breakpoint when it is
// deallocated.
// ---------------------------------------------------------------------------------------------------------------------------------

bool	&m_breakOnDealloc(void *reportedAddress)
{
	// Locate the existing allocation unit

	sAllocUnit	*au = findAllocUnit(reportedAddress);

	// If you hit this assert, you tried to set a breakpoint on deallocation for an address that doesn't exist. Interrogate the
	// stack frame or the variable 'au' to see which allocation this is.
	m_assert(au != NULL);

	return au->breakOnDealloc;
}

// ---------------------------------------------------------------------------------------------------------------------------------
// -DOC- When tracking down a difficult bug, use this routine to force a breakpoint on a specific allocation count
// ---------------------------------------------------------------------------------------------------------------------------------

void	m_breakOnAllocation(unsigned int count)
{
	breakOnAllocationCount = count;
}//*/

// ---------------------------------------------------------------------------------------------------------------------------------
// Used by the macros
// ---------------------------------------------------------------------------------------------------------------------------------

void	m_setOwner(const char *file, const unsigned int line, const char *func)
{
	// You're probably wondering about this...
	//
	// It's important for this memory manager to primarily work with global new/delete in their original forms (i.e. with
	// no extra parameters.) In order to do this, we use macros that call this function prior to operators new & delete. This
	// is fine... usually. Here's what actually happens when you use this macro to delete an object:
	//
	// m_setOwner(__FILE__, __LINE__, __FUNCTION__) --> object::~object() --> delete
	//
	// Note that the compiler inserts a call to the object's destructor just prior to calling our overridden operator delete.
	// But what happens when we delete an object whose destructor deletes another object, whose desctuctor deletes another
	// object? Here's a diagram (indentation follows stack depth):
	//
	// m_setOwner(...) -> ~obj1()                          // original call to delete obj1
	//     m_setOwner(...) -> ~obj2()                      // obj1's destructor deletes obj2
	//         m_setOwner(...) -> ~obj3()                  // obj2's destructor deletes obj3
	//             ...                                     // obj3's destructor just does some stuff
	//         delete                                      // back in obj2's destructor, we call delete
	//     delete                                          // back in obj1's destructor, we call delete
	// delete                                              // back to our original call, we call delete
	//
	// Because m_setOwner() just sets up some static variables (below) it's important that each call to m_setOwner() and
	// successive calls to new/delete alternate. However, in this case, three calls to m_setOwner() happen in succession
	// followed by three calls to delete in succession (with a few calls to destructors mixed in for fun.) This means that
	// only the final call to delete (in this chain of events) will have the proper reporting, and the first two in the chain
	// will not have ANY owner-reporting information. The deletes will still work fine, we just won't know who called us.
	//
	// "Then build a stack, my friend!" you might think... but it's a very common thing that people will be working with third-
	// party libraries (including MFC under Windows) which is not compiled with this memory manager's macros. In those cases,
	// m_setOwner() is never called, and rightfully should not have the proper trace-back information. So if one of the
	// destructors in the chain ends up being a call to a delete from a non-mmgr-compiled library, the stack will get confused.
	//
	// I've been unable to find a solution to this problem, but at least we can detect it and report the data before we
	// lose it. That's what this is all about. It makes it somewhat confusing to read in the logs, but at least ALL the
	// information is present...
	//
	// There's a caveat here... The compiler is not required to call operator delete if the value being deleted is NULL.
	// In this case, any call to delete with a NULL will sill call m_setOwner(), which will make m_setOwner() think that
	// there is a destructor chain becuase we setup the variables, but nothing gets called to clear them. Because of this
	// we report a "Possible destructor chain".
	//
	// Thanks to J. Woznack (from Kodiak Interactive Software Studios -- www.kodiakgames.com) for pointing this out.

	if (sourceLine && alwaysLogAll)
	{
		log("[I] NOTE! Possible destructor chain: previous owner is %s\r\n", ownerString(sourceFile, sourceLine, sourceFunc, sourceTag));
	}

	// Okay... save this stuff off so we can keep track of the caller

	sourceFile = file;
	sourceLine = line;
	sourceFunc = func;
}


// ---------------------------------------------------------------------------------------------------------------------------------

static	void	resetGlobals()
{
	sourceFile = m_uninitializedTag;
	sourceLine = 0;
	sourceFunc = m_uninitializedTag;
    sourceTag =  "";
}

void m_setLoggingState(bool state){
    alwaysLogAll = state;
}

bool m_getLoggingState(){
    return alwaysLogAll;
}


// ---------------------------------------------------------------------------------------------------------------------------------
// Global new/new[]
//
// These are the standard new/new[] operators. They are merely interface functions that operate like normal new/new[], but use our
// memory tracking routines.
// ---------------------------------------------------------------------------------------------------------------------------------

void	*operator new(size_t reportedSize)
{
#ifdef TEST_MEMORY_MANAGER
    log("[D] new(): allocating a block of size %d\r\n", reportedSize);
#endif

	// Save these off...

	const char *        file = sourceFile;
	const unsigned int  line = sourceLine;
	const char *        func = sourceFunc;
    const char *        tag =  sourceTag;
#ifdef TEST_MEMORY_MANAGER
    log("[D] new(): assigned tags ['%s' (%d)]::%s() (%s)\r\n", reportedSize, file, line, func, tag);
#endif

	// ANSI says: allocation requests of 0 bytes will still return a valid value

	if (reportedSize == 0) reportedSize = 1;
#ifdef TEST_MEMORY_MANAGER
    log("[D] new(): reportedSize = %d\r\n", reportedSize);
#endif

	// ANSI says: loop continuously because the error handler could possibly free up some memory

	for(;;)
	{
#ifdef TEST_MEMORY_MANAGER
        log("[D] new(): attempting allocation\r\n");
#endif
		// Try the allocation

		void	*ptr = m_allocator(file, line, func, m_alloc_new, reportedSize);
		if (ptr)
		{
#ifdef TEST_MEMORY_MANAGER
            log("[D] new(): allocation successful: addr = 0x%X\r\n", ptr);
#endif
			return ptr;
		}

		// There isn't a way to determine the new handler, except through setting it. So we'll just set it to NULL, then
		// set it back again.

//joe 
//		new_handler	nh = std::set_new_handler(0);
		std::new_handler	nh = std::set_new_handler(0);
		std::set_new_handler(nh);

		// If there is an error handler, call it

		if (nh)
		{
			(*nh)();
		}

		// Otherwise, throw the exception

		else
		{
#ifdef TEST_MEMORY_MANAGER
            log("[D] new(): bailing on a bad allocation!\r\n");
#endif
			throw std::bad_alloc();
		}
	}
}


// ---------------------------------------------------------------------------------------------------------------------------------

void	*operator new[](size_t reportedSize)
{
	#ifdef TEST_MEMORY_MANAGER
    log("[D] new[](): allocating a new array of size %d\r\n", reportedSize);
	#endif

	// Save these off...

	const char *        file = sourceFile;
	const unsigned int	line = sourceLine;
	const char *        func = sourceFunc;
    const char *        tag =  sourceTag;

	// The ANSI standard says that allocation requests of 0 bytes will still return a valid value

	if (reportedSize == 0) reportedSize = 1;

	// ANSI says: loop continuously because the error handler could possibly free up some memory

	for(;;)
	{
		// Try the allocation

		void	*ptr = m_allocator(file, line, func, m_alloc_new_array, reportedSize);
		if (ptr)
		{
			#ifdef TEST_MEMORY_MANAGER
            log("[D] new[](): allocation successful: addr = 0x%X\r\n", ptr);
			#endif
			return ptr;
		}

		// There isn't a way to determine the new handler, except through setting it. So we'll just set it to NULL, then
		// set it back again.
		std::new_handler	nh = std::set_new_handler(0);
		std::set_new_handler(nh);

		// If there is an error handler, call it

		if (nh)
		{
			(*nh)();
		}

		// Otherwise, throw the exception

		else
		{
			#ifdef TEST_MEMORY_MANAGER
            log("[D] new[](): bailing on a bad allocation\r\n");
			#endif
			throw std::bad_alloc();
		}
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------
// Other global new/new[]
//
// These are the standard new/new[] operators as used by Microsoft's memory tracker. We don't want them interfering with our memory
// tracking efforts. Like the previous versions, these are merely interface functions that operate like normal new/new[], but use
// our memory tracking routines.
// ---------------------------------------------------------------------------------------------------------------------------------

void	*operator new(size_t reportedSize, const char *sourceFile, int sourceLine)
{
	#ifdef TEST_MEMORY_MANAGER
    log("[D] new(): allocating a new block of size %d (source = '%s' (%d))\r\n", reportedSize, sourceFile, sourceLine);
	#endif

	// The ANSI standard says that allocation requests of 0 bytes will still return a valid value

	if (reportedSize == 0) reportedSize = 1;

	// ANSI says: loop continuously because the error handler could possibly free up some memory

	for(;;)
	{
		// Try the allocation

		void	*ptr = m_allocator(sourceFile, sourceLine, m_unknownTag, m_alloc_new, reportedSize);
		if (ptr)
		{
			#ifdef TEST_MEMORY_MANAGER
            log("[D] new(): allocation successful: addr = 0x%X\r\n", ptr);
			#endif
			return ptr;
		}

		// There isn't a way to determine the new handler, except through setting it. So we'll just set it to NULL, then
		// set it back again.

		std::new_handler	nh = std::set_new_handler(0);
		std::set_new_handler(nh);

		// If there is an error handler, call it

		if (nh)
		{
			(*nh)();
		}

		// Otherwise, throw the exception

		else
		{
			#ifdef TEST_MEMORY_MANAGER
            log("[D] new(): bailing on a bad allocation\r\n");
			#endif
			throw std::bad_alloc();
		}
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	*operator new[](size_t reportedSize, const char *sourceFile, int sourceLine)
{
	#ifdef TEST_MEMORY_MANAGER
    log("[D] new[](): allocating a new block of size %d (source = '%s' (%d))\r\n", reportedSize, sourceFile, sourceLine);
	#endif

	// The ANSI standard says that allocation requests of 0 bytes will still return a valid value

	if (reportedSize == 0) reportedSize = 1;

	// ANSI says: loop continuously because the error handler could possibly free up some memory

	for(;;)
	{
		// Try the allocation

		void	*ptr = m_allocator(sourceFile, sourceLine, m_unknownTag, m_alloc_new_array, reportedSize);
		if (ptr)
		{
			#ifdef TEST_MEMORY_MANAGER
            log("[D] new[](): allocation successful: addr = 0x%X\r\n", ptr);
			#endif
			return ptr;
		}

		// There isn't a way to determine the new handler, except through setting it. So we'll just set it to NULL, then
		// set it back again.

		std::new_handler	nh = std::set_new_handler(0);
		std::set_new_handler(nh);

		// If there is an error handler, call it

		if (nh)
		{
			(*nh)();
		}

		// Otherwise, throw the exception

		else
		{
			#ifdef TEST_MEMORY_MANAGER
            log("[D] new[](): bailing on a bad allocation\r\n");
			#endif
			throw std::bad_alloc();
		}
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------
// Global delete/delete[]
//
// These are the standard delete/delete[] operators. They are merely interface functions that operate like normal delete/delete[],
// but use our memory tracking routines.
// ---------------------------------------------------------------------------------------------------------------------------------

void	operator delete(void *reportedAddress)
{
	#ifdef TEST_MEMORY_MANAGER
    log("[D] delete(): deleting the block 0x%X\r\n", reportedAddress);
	#endif

	// ANSI says: delete & delete[] allow NULL pointers (they do nothing)

	if (reportedAddress) 
        m_deallocator(sourceFile, sourceLine, sourceFunc, m_alloc_delete, reportedAddress);
	
    else if (alwaysLogAll) 
        log("[-] ----- %8s of NULL                      by %s\r\n", allocationTypes[m_alloc_delete], ownerString(sourceFile, sourceLine, sourceFunc, sourceTag));


	// Resetting the globals insures that if at some later time, somebody calls our memory manager from an unknown
	// source (i.e. they didn't include our H file) then we won't think it was the last allocation.

	resetGlobals();

	#ifdef TEST_MEMORY_MANAGER
    log("[D] delete(): delete successful\r\n");
	#endif
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	operator delete[](void *reportedAddress)
{
	#ifdef TEST_MEMORY_MANAGER
    log("[D] delete[](): deleting the block 0x%X\r\n", reportedAddress);
	#endif

	// ANSI says: delete & delete[] allow NULL pointers (they do nothing)

	if (reportedAddress) 
        m_deallocator(sourceFile, sourceLine, sourceFunc, m_alloc_delete_array, reportedAddress);
	
    else if (alwaysLogAll)
		log("[-] ----- %8s of NULL                      by %s\r\n", allocationTypes[m_alloc_delete_array], ownerString(sourceFile, sourceLine, sourceFunc, sourceTag));

	
    // Resetting the globals insures that if at some later time, somebody calls our memory manager from an unknown
	// source (i.e. they didn't include our H file) then we won't think it was the last allocation.

	resetGlobals();

	#ifdef TEST_MEMORY_MANAGER
    log("[D] delete[](): delete successful\r\n");
	#endif
}
// ---------------------------------------------------------------------------------------------------------------------------------
// Allocate memory and track it
// ---------------------------------------------------------------------------------------------------------------------------------

extern "C" void *m_physfsAllocator(const char *sourceFile, const unsigned int sourceLine, const char *sourceFunc, const unsigned int allocationType, const size_t reportedSize){
    if (alwaysLogAll) 
        log("[+] %06d PhysFS-> %8s of size %08d by %s\r\n", currentAllocationCount, allocationTypes[allocationType], reportedSize, ownerString(sourceFile, sourceLine, sourceFunc, "PhysFS"));

    mmgr_setTag("PhysFS");
    return m_allocator(sourceFile, sourceLine, sourceFunc, allocationType, reportedSize);
}

void *m_havokAllocator(size_t reportedSize){
    if (alwaysLogAll) 
        log("[+] %06d Havok-> %8s of size %08d by %s\r\n", currentAllocationCount, allocationTypes[m_alloc_malloc], reportedSize, ownerString(sourceFile, sourceLine, sourceFunc, "Havok"));

    mmgr_setTag("Havok");
    return m_allocator(__FILE__, __LINE__, __FUNCTION__, m_alloc_malloc, reportedSize);
}

extern "C" void *m_gimAllocator(size_t reportedSize){
    if (alwaysLogAll) 
        log("[+] %06d Gim-> %8s of size %08d by %s\r\n", currentAllocationCount, allocationTypes[m_alloc_malloc], reportedSize, ownerString(sourceFile, sourceLine, sourceFunc, "Gim"));

    mmgr_setTag("Gim");
    return m_allocator(__FILE__, __LINE__, __FUNCTION__, m_alloc_malloc, reportedSize);
}




void *m_allocator(const char *sourceFile, const unsigned int sourceLine, const char *sourceFunc, const unsigned int allocationType, const size_t reportedSize, const size_t align)
{
	AllocatorBusy = true;
	try
	{
		#ifdef TEST_MEMORY_MANAGER
		log("[D] ENTER: m_allocator()\r\n");
		#endif

		// Increase our allocation count

		currentAllocationCount++;
        currentBlockCount++;

		// Log the request

		if (alwaysLogAll) 
            log("[+] %06d %8s of size %08d by %s\r\n", currentAllocationCount, allocationTypes[allocationType], reportedSize, ownerString(sourceFile, sourceLine, sourceFunc, sourceTag));

		// If you hit this assert, you requested a breakpoint on a specific allocation count
		m_assert(currentAllocationCount != breakOnAllocationCount);

		// If necessary, grow the reservoir of unused allocation units

		if (!reservoir)
		{
			// Allocate 256 reservoir elements
            //printf("m_allocator(): allocUnit reservoir is empty!  Allocating a new block\n");

			reservoir = (sAllocUnit *) malloc(sizeof(sAllocUnit) * 256);

			// If you hit this assert, then the memory manager failed to allocate internal memory for tracking the
			// allocations
			m_assert(reservoir != NULL);

			// Danger Will Robinson!

			if (reservoir == NULL) throw "Unable to allocate RAM for internal memory tracking data";

			// Build a linked-list of the elements in our reservoir

			//memset(reservoir, 0, sizeof(sAllocUnit) * 256);
            reservoir[255].next = NULL;
            reservoir[255].prev = NULL;

			for (unsigned int i = 0; i < 256 - 1; i++)
			{
				reservoir[i].next = &reservoir[i+1];
                reservoir[i].prev = NULL;
			}

			// Add this address to our reservoirBuffer so we can free it later
            sAllocUnit	**temp;

            if (reservoirBufferSize >= reservoirBufferMaxSize){
                //printf("m_allocator(): allocating a new reservoir buffer to hold %d blocks (allocCount = %d)\n", reservoirBufferSize + 4, currentBlockCount);
                reservoirBufferMaxSize = reservoirBufferSize + reservoirBufferGrow;
			    temp = (sAllocUnit **)realloc(reservoirBuffer, reservoirBufferMaxSize * sizeof(sAllocUnit *));

			    m_assert(temp);


			    if (temp){
				    reservoirBuffer = temp;
				    reservoirBuffer[reservoirBufferSize++] = reservoir;
			    }
            }
            else{
                //printf("m_allocator(): reservoir buffer is large enough... recording new blocks (allocCount = %d)\n", currentBlockCount);
                reservoirBuffer[reservoirBufferSize++] = reservoir;
            }
		}

		// Logical flow says this should never happen...
		m_assert(reservoir != NULL);

		// Grab a new allocaton unit from the front of the reservoir

		sAllocUnit	*au = reservoir;
		reservoir = au->next;

		// Populate it with some real data

		//memset(au, 0, sizeof(sAllocUnit));
        /*au->breakOnDealloc = false;
        au->breakOnRealloc = false;//*/
		au->inGame         = m_isInGame;
		au->actualSize     = calculateActualSize(reportedSize, align);
		#ifdef RANDOM_FAILURE
		double	a = rand();
		double	b = RAND_MAX / 100.0 * RANDOM_FAILURE;
		if (a > b)
		{
			au->actualAddress = malloc(au->actualSize);
		}
		else
		{
			log("[F] Random faiure\r\n");
			au->actualAddress = NULL;
		}
		#else
        if (align > 0)
		    au->actualAddress     = memalign(align, au->actualSize);

        else
		    au->actualAddress     = malloc(au->actualSize);
		#endif
		au->reportedSize      = reportedSize;
		au->reportedAddress   = calculateReportedAddress(au->actualAddress, align);
		au->allocationType    = allocationType;
		au->sourceLine        = sourceLine;
        au->sourceTag =         sourceTag;
		au->allocationNumber  = currentAllocationCount;

        if (sourceFile){
            /*strncpy(au->sourceFile, sourceFileStripper(sourceFile), sizeof(au->sourceFile) - 1);
            au->sourceFile[sizeof(au->sourceFile) - 1] = 0;//*/
            //NOTE: this pointer MUST be constant!
            au->sourceFile = sourceFile;
        }
		else
            //strcpy (au->sourceFile, "??");
            au->sourceFile = m_unknownTag;

        if (sourceFunc){
            /*strncpy(au->sourceFunc, sourceFunc, sizeof(au->sourceFunc) - 1);
            au->sourceFunc[sizeof(au->sourceFunc) - 1] = 0;//*/
            //NOTE: this pointer MUST be constant!
            au->sourceFunc = sourceFunc;
        }
		else
            //strcpy (au->sourceFunc, "??");
            au->sourceFunc = m_unknownTag;


        //accumulate the tag stats
        TaggedStat *ts = addTag(sourceTag);

        //make sure the tag's data was found
        if (ts){
            ts->totalCount++;
            ts->accumulatedCount++;
            ts->accumulatedSize += reportedSize;
            ts->totalSize += reportedSize;
            
            if (ts->totalSize > ts->peakSize)
                ts->peakSize = ts->totalSize;

            if (ts->totalCount > ts->peakCount)
                ts->peakCount = ts->totalCount;
        }


		// We don't want to assert with random failures, because we want the application to deal with them.

		#ifndef RANDOM_FAILURE
		// If you hit this assert, then the requested allocation simply failed (you're out of memory.) Interrogate the
		// variable 'au' or the stack frame to see what you were trying to do.
        if (au->actualAddress == NULL){
            log("m_allocator(): ERROR-> no of memory!  {memUsed = %d}\n", m_getMemUsed());
		    m_assert(au->actualAddress != NULL);
        }
		#endif

		if (au->actualAddress == NULL)
		{
			throw "Request for allocation failed. Out of memory.";
		}

		// If you hit this assert, then this allocation was made from a source that isn't setup to use this memory tracking
		// software, use the stack frame to locate the source and include our H file.
		m_assert(allocationType != m_alloc_unknown);

		// Insert the new allocation into the hash table

		unsigned int	hashIndex = (reinterpret_cast<unsigned int>(au->reportedAddress) >> 4) & (hashSize - 1);
		if (hashTable[hashIndex]) hashTable[hashIndex]->prev = au;
		au->next = hashTable[hashIndex];
		au->prev = NULL;
		hashTable[hashIndex] = au;

		// Account for the new allocatin unit in our stats

		stats.totalReportedMemory += static_cast<unsigned int>(au->reportedSize);
		stats.totalActualMemory   += static_cast<unsigned int>(au->actualSize);
		stats.totalAllocUnitCount++;
		if (stats.totalReportedMemory > stats.peakReportedMemory) stats.peakReportedMemory = stats.totalReportedMemory;
		if (stats.totalActualMemory   > stats.peakActualMemory)   stats.peakActualMemory   = stats.totalActualMemory;
		if (stats.totalAllocUnitCount > stats.peakAllocUnitCount) stats.peakAllocUnitCount = stats.totalAllocUnitCount;
		stats.accumulatedReportedMemory += static_cast<unsigned int>(au->reportedSize);
		stats.accumulatedActualMemory += static_cast<unsigned int>(au->actualSize);
		stats.accumulatedAllocUnitCount++;

		// Prepare the allocation unit for use (wipe it with recognizable garbage)

		wipeWithPattern(au, unusedPattern);

		// calloc() expects the reported memory address range to be filled with 0's

		if (allocationType == m_alloc_calloc)
		{
			memset(au->reportedAddress, 0, au->reportedSize);
		}

#ifdef ENABLE_VALIDATION
		// Validate every single allocated unit in memory
		if (alwaysValidateAll) 
            m_validateAllAllocUnits();
#endif

		// Log the result

		if (alwaysLogAll) 
            log("[+] ---->             addr 0x%08X\r\n", reinterpret_cast<unsigned int>(au->reportedAddress));

		// Resetting the globals insures that if at some later time, somebody calls our memory manager from an unknown
		// source (i.e. they didn't include our H file) then we won't think it was the last allocation.

		resetGlobals();

		// Return the (reported) address of the new allocation unit

		#ifdef TEST_MEMORY_MANAGER
		log("[D] EXIT : m_allocator()\r\n");
		#endif

		AllocatorBusy = false;

		return au->reportedAddress;
	}
	catch(const char *err)
	{
		// Deal with the errors

		log("[!] %s\r\n", err);
		resetGlobals();

		#ifdef TEST_MEMORY_MANAGER
		log("[D] EXIT : m_allocator()\r\n");
		#endif

		AllocatorBusy = false;

		return NULL;
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------
// Reallocate memory and track it
// ---------------------------------------------------------------------------------------------------------------------------------
extern "C" void *m_physfsReallocator(const char *sourceFile, const unsigned int sourceLine, const char *sourceFunc, const unsigned int reallocationType, const size_t reportedSize, void *reportedAddress){
    if (alwaysLogAll) 
        log("[~] %06d PhysFS-> %8s of size %08d (addr = 0x%08X) by %s\r\n", currentAllocationCount, allocationTypes[reallocationType], reportedSize, reportedAddress, ownerString(sourceFile, sourceLine, sourceFunc, "PhysFS"));

  
    mmgr_setTag("PhysFS");
    return m_reallocator(sourceFile, sourceLine, sourceFunc, reallocationType, reportedSize, reportedAddress);    
}

extern "C" void *m_scriptReallocator(const char *sourceFile, const unsigned int sourceLine, const char *sourceFunc, const unsigned int reallocationType, const size_t reportedSize, void *reportedAddress){
    if (alwaysLogAll) 
        log("[~] %06d Script-> %8s of size %08d (addr = 0x%08X) by %s\r\n", currentAllocationCount, allocationTypes[reallocationType], reportedSize, reportedAddress, ownerString(sourceFile, sourceLine, sourceFunc, "Script"));

  
    mmgr_setTag("Script");
    return m_reallocator(sourceFile, sourceLine, sourceFunc, reallocationType, reportedSize, reportedAddress);
}

void *m_havokReallocator(void *reportedAddress, size_t reportedSize){
    if (alwaysLogAll) 
        log("[~] %06d Havok-> %8s of size %08d (addr = 0x%08X) by %s\r\n", currentAllocationCount, allocationTypes[m_alloc_realloc], reportedSize, reportedAddress, ownerString(sourceFile, sourceLine, sourceFunc, "Havok"));


    mmgr_setTag("Havok");
    return m_reallocator(__FILE__, __LINE__, __FUNCTION__, m_alloc_realloc, reportedSize, reportedAddress);
}


void *m_reallocator(const char *sourceFile, const unsigned int sourceLine, const char *sourceFunc, const unsigned int reallocationType, const size_t reportedSize, void *reportedAddress)
{
	AllocatorBusy = true;
	try
	{
		#ifdef TEST_MEMORY_MANAGER
		log("[D] ENTER: m_reallocator()\r\n");
		#endif

		// Calling realloc with a NULL should force same operations as a malloc

		if (!reportedAddress)
		{
			return m_allocator(sourceFile, sourceLine, sourceFunc, reallocationType, reportedSize);
		}

        else if (reportedSize == 0){
            //size == 0 && addr == NULL is a bad case that often results in allocating sizeof(int) bytes.
            m_assert(reportedAddress != NULL);

            m_deallocator(sourceFile, sourceLine, sourceFunc, reallocationType, reportedAddress);

			AllocatorBusy = false;
            return NULL;
        }


		// Increase our allocation count

		currentAllocationCount++;

		// If you hit this assert, you requested a breakpoint on a specific allocation count
		m_assert(currentAllocationCount != breakOnAllocationCount);

		// Log the request

		if (alwaysLogAll) 
            log("[~] %06d %8s of size %08d (addr = 0x%08X) by %s\r\n", currentAllocationCount, allocationTypes[reallocationType], reportedSize, reportedAddress, ownerString(sourceFile, sourceLine, sourceFunc, sourceTag));

		// Locate the existing allocation unit

		sAllocUnit	*au = findAllocUnit(reportedAddress);

		// If you hit this assert, you tried to reallocate RAM that wasn't allocated by this memory manager.
#if defined(TRANSGAMING) && defined(DEBUG)
        if (au == NULL)
            printf("m_reallocator(): %s (%d)::%s()-> attempting to reallocate the block 0x%08X (size %d)\n", sourceFile, sourceLine, sourceFunc, reportedAddress, reportedSize);
#endif
		m_assert(au != NULL);
		if (au == NULL) throw "Request to reallocate RAM that was never allocated";

#ifdef ENABLE_VALIDATION
		// If you hit this assert, then the allocation unit that is about to be reallocated is damaged. But you probably
		// already know that from a previous assert you should have seen in validateAllocUnit() :)
		m_assert(m_validateAllocUnit(au));
#endif

		// If you hit this assert, then this reallocation was made from a source that isn't setup to use this memory
		// tracking software, use the stack frame to locate the source and include our H file.
		m_assert(reallocationType != m_alloc_unknown);


		// If you hit this assert, you were trying to reallocate RAM that was not allocated in a way that is compatible with
		// realloc. In other words, you have a allocation/reallocation mismatch.  Note this does not allow reallocating of
        // blocks that were originally created using memalign()
		m_assert(au->allocationType == m_alloc_malloc ||
			 au->allocationType == m_alloc_calloc ||
			 au->allocationType == m_alloc_realloc);

		// If you hit this assert, then the "break on realloc" flag for this allocation unit is set (and will continue to be
		// set until you specifically shut it off. Interrogate the 'au' variable to determine information about this
		// allocation unit.
		//m_assert(au->breakOnRealloc == false);

		// Keep track of the original size

		unsigned int	originalReportedSize = static_cast<unsigned int>(au->reportedSize);

		if (alwaysLogAll) 
            log("[~] ---->             resize 0x%08X from %08d to %08d\r\n", reportedAddress, originalReportedSize, reportedSize);

		// Do the reallocation

		void	*oldReportedAddress = reportedAddress;
		size_t	newActualSize = calculateActualSize(reportedSize);
		void	*newActualAddress = NULL;
		#ifdef RANDOM_FAILURE
		double	a = rand();
		double	b = RAND_MAX / 100.0 * RANDOM_FAILURE;
		if (a > b)
		{
			newActualAddress = realloc(au->actualAddress, newActualSize);
		}
		else
		{
			log("[F] Random faiure\r\n");
		}
		#else
		newActualAddress = realloc(au->actualAddress, newActualSize);
		#endif

		// We don't want to assert with random failures, because we want the application to deal with them.

		#ifndef RANDOM_FAILURE
		// If you hit this assert, then the requested allocation simply failed (you're out of memory) Interrogate the
		// variable 'au' to see the original allocation. You can also query 'newActualSize' to see the amount of memory
		// trying to be allocated. Finally, you can query 'reportedSize' to see how much memory was requested by the caller.
        if (newActualAddress == NULL){
            log("m_reallocator(): ERROR-> no of memory!  {memUsed = %d}\n", m_getMemUsed());

		    m_assert(newActualAddress);
        }
		#endif

		if (!newActualAddress) throw "Request for reallocation failed. Out of memory.";

		// Remove this allocation from our stats (we'll add the new reallocation again later)

		stats.totalReportedMemory -= static_cast<unsigned int>(au->reportedSize);
		stats.totalActualMemory   -= static_cast<unsigned int>(au->actualSize);

		// Update the allocation with the new information

		au->actualSize        = newActualSize;
		au->actualAddress     = newActualAddress;
		au->reportedSize      = calculateReportedSize(newActualSize);
		au->reportedAddress   = calculateReportedAddress(newActualAddress);
		au->allocationType    = reallocationType;
		au->sourceLine        = sourceLine;
		au->allocationNumber  = currentAllocationCount;
		
        if (sourceFile){
            /*strncpy(au->sourceFile, sourceFileStripper(sourceFile), sizeof(au->sourceFile) - 1);
            au->sourceFile[sizeof(au->sourceFile) - 1] = 0;//*/
            //NOTE: this MUST be a constant pointer!
            au->sourceFile = sourceFile;
        }
		else
            //strcpy (au->sourceFile, "??");
            au->sourceFile = m_unknownTag;

        if (sourceFunc){
            /*strncpy(au->sourceFunc, sourceFunc, sizeof(au->sourceFunc) - 1);
            au->sourceFunc[sizeof(au->sourceFunc) - 1] = 0;//*/
            //NOTE: this MUST be a constant pointer!
            au->sourceFunc = sourceFunc;
        }
        else
            //strcpy (au->sourceFunc, "??");
            au->sourceFunc = m_unknownTag;


        //accumulate the tag stats
        TaggedStat *ts = addTag(au->sourceTag);

        //make sure the tag's data was found
        if (ts){
            ts->accumulatedSize += (reportedSize - originalReportedSize);
            ts->totalSize -= originalReportedSize;
            ts->totalSize += reportedSize;

            
            if (ts->totalSize < 0)
                log("[!] ----> ERROR-> allocation size for the tag '%s' has dropped below 0 during a realloc()!\r\n", au->sourceTag);
            
            else if (ts->totalSize > ts->peakSize)
                ts->peakSize = ts->totalSize;
        }


		// The reallocation may cause the address to change, so we should relocate our allocation unit within the hash table

		unsigned int	hashIndex = static_cast<unsigned int>(-1);
		if (oldReportedAddress != au->reportedAddress)
		{
			// Remove this allocation unit from the hash table

			{
				unsigned int	hashIndex = (reinterpret_cast<unsigned int>(oldReportedAddress) >> 4) & (hashSize - 1);
				if (hashTable[hashIndex] == au)
				{
					hashTable[hashIndex] = hashTable[hashIndex]->next;
				}
				else
				{
					if (au->prev)	au->prev->next = au->next;
					if (au->next)	au->next->prev = au->prev;
				}
			}

			// Re-insert it back into the hash table

			hashIndex = (reinterpret_cast<unsigned int>(au->reportedAddress) >> 4) & (hashSize - 1);
			if (hashTable[hashIndex]) hashTable[hashIndex]->prev = au;
			au->next = hashTable[hashIndex];
			au->prev = NULL;
			hashTable[hashIndex] = au;
		}

		// Account for the new allocatin unit in our stats

		stats.totalReportedMemory += static_cast<unsigned int>(au->reportedSize);
		stats.totalActualMemory   += static_cast<unsigned int>(au->actualSize);
		if (stats.totalReportedMemory > stats.peakReportedMemory) stats.peakReportedMemory = stats.totalReportedMemory;
		if (stats.totalActualMemory   > stats.peakActualMemory)   stats.peakActualMemory   = stats.totalActualMemory;
		int	deltaReportedSize = static_cast<int>(reportedSize - originalReportedSize);
		if (deltaReportedSize > 0)
		{
			stats.accumulatedReportedMemory += deltaReportedSize;
			stats.accumulatedActualMemory += deltaReportedSize;
		}

		// Prepare the allocation unit for use (wipe it with recognizable garbage)

		wipeWithPattern(au, unusedPattern, originalReportedSize);

#ifdef ENABLE_VALIDATION
		// If you hit this assert, then something went wrong, because the allocation unit was properly validated PRIOR to
		// the reallocation. This should not happen.
		m_assert(m_validateAllocUnit(au));

		// Validate every single allocated unit in memory
		if (alwaysValidateAll) 
            m_validateAllAllocUnits();
#endif

		// Log the result

        if (alwaysLogAll){
            char    buf[18];
            int     stop = reportedSize > 16 ? 16 : reportedSize;
            int     i;
            char *  data = reinterpret_cast<char *>(au->reportedAddress);


            for (i = 0; i < stop; i++){
                if (data[i] == 0 || data[i] == '\n' || data[i] == '\r' || data[i] == '\b')
                    buf[i] = '.';
                else
                    buf[i] = data[i];
            }
            buf[i] = 0;

            log("[~] ---->             addr 0x%08X (orig = 0x%08X)\r\n", reinterpret_cast<unsigned int>(au->reportedAddress), reportedAddress);
            log("[~] ====>             block = [%s]\r\n", buf);
        }

		// Resetting the globals insures that if at some later time, somebody calls our memory manager from an unknown
		// source (i.e. they didn't include our H file) then we won't think it was the last allocation.

		resetGlobals();

		// Return the (reported) address of the new allocation unit

		#ifdef TEST_MEMORY_MANAGER
		log("[D] EXIT : m_reallocator()\r\n");
		#endif

		return au->reportedAddress;
	}
	catch(const char *err)
	{
		// Deal with the errors

		log("[!] %s\r\n", err);
		resetGlobals();

		#ifdef TEST_MEMORY_MANAGER
		log("[D] EXIT : m_reallocator()\r\n");
		#endif

		AllocatorBusy = false;
		return NULL;
	}
	AllocatorBusy = false;
}

// ---------------------------------------------------------------------------------------------------------------------------------
// Deallocate memory and track it
// ---------------------------------------------------------------------------------------------------------------------------------
extern "C" void m_physfsDeallocator(const char *sourceFile, const unsigned int sourceLine, const char *sourceFunc, const unsigned int deallocationType, const void *reportedAddress){
    if (alwaysLogAll) 
        log("[-] ----- PhysFS-> %8s of addr 0x%08X           by %s\r\n", allocationTypes[deallocationType], reinterpret_cast<unsigned int>(const_cast<void *>(reportedAddress)), ownerString(sourceFile, sourceLine, sourceFunc, "PhysFS"));
  
    mmgr_setTag("PhysFS");
    return m_deallocator(sourceFile, sourceLine, sourceFunc, deallocationType, reportedAddress);    
}

extern "C" void m_gimDeallocator(void *reportedAddress){
    if (alwaysLogAll)
        log("[-] ----- Gim-> %8s of addr 0x%08X           by %s\r\n", allocationTypes[m_alloc_free], reinterpret_cast<unsigned int>(const_cast<void *>(reportedAddress)), ownerString(__FILE__, __LINE__, __FUNCTION__, "Gim"));
  
    mmgr_setTag("Gim");
    return m_deallocator(__FILE__, __LINE__, __FUNCTION__, m_alloc_free, reportedAddress);    
}


void	m_deallocator(const char *sourceFile, const unsigned int sourceLine, const char *sourceFunc, const unsigned int deallocationType, const void *reportedAddress)
{
	try
	{
		#ifdef TEST_MEMORY_MANAGER
		log("[D] ENTER: m_deallocator()\r\n");
		#endif

		// Log the request

		if (alwaysLogAll) 
            log("[-] ----- %8s of addr 0x%08X           by %s\r\n", allocationTypes[deallocationType], reinterpret_cast<unsigned int>(const_cast<void *>(reportedAddress)), ownerString(sourceFile, sourceLine, sourceFunc, sourceTag));

		// We should only ever get here with a null pointer if they try to do so with a call to free() (delete[] and delete will
		// both bail before they get here.) So, since ANSI allows free(NULL), we'll not bother trying to actually free the allocated
		// memory or track it any further.

		if (reportedAddress)
		{
			// Go get the allocation unit

			sAllocUnit	*au = findAllocUnit(reportedAddress);

			// If you hit this assert, you tried to deallocate RAM that wasn't allocated by this memory manager.
			m_assert(au != NULL);
			if (au == NULL) throw "Request to deallocate RAM that was never allocated";

#ifdef ENABLE_VALIDATION
			// If you hit this assert, then the allocation unit that is about to be deallocated is damaged. But you probably
			// already know that from a previous assert you should have seen in validateAllocUnit() :)
			m_assert(m_validateAllocUnit(au));
#endif

			// If you hit this assert, then this deallocation was made from a source that isn't setup to use this memory
			// tracking software, use the stack frame to locate the source and include our H file.
			m_assert(deallocationType != m_alloc_unknown);

			// If you hit this assert, you were trying to deallocate RAM that was not allocated in a way that is compatible with
			// the deallocation method requested. In other words, you have a allocation/deallocation mismatch.
			m_assert((deallocationType == m_alloc_delete       && au->allocationType == m_alloc_new      ) ||
				(deallocationType == m_alloc_delete_array && au->allocationType == m_alloc_new_array) ||
				(deallocationType == m_alloc_free         && au->allocationType == m_alloc_malloc   ) ||
				(deallocationType == m_alloc_free         && au->allocationType == m_alloc_memalign ) ||
				(deallocationType == m_alloc_free         && au->allocationType == m_alloc_calloc   ) ||
				(deallocationType == m_alloc_free         && au->allocationType == m_alloc_realloc  ) ||
				(deallocationType == m_alloc_unknown                                                ) );

			// If you hit this assert, then the "break on dealloc" flag for this allocation unit is set. Interrogate the 'au'
			// variable to determine information about this allocation unit.
			//m_assert(au->breakOnDealloc == false);

			// Wipe the deallocated RAM with a new pattern. This doen't actually do us much good in debug mode under WIN32,
			// because Microsoft's memory debugging & tracking utilities will wipe it right after we do. Oh well.

			wipeWithPattern(au, releasedPattern);

			// Do the deallocation

			free(au->actualAddress);

			// Remove this allocation unit from the hash table

			unsigned int	hashIndex = (reinterpret_cast<unsigned int>(au->reportedAddress) >> 4) & (hashSize - 1);
			if (hashTable[hashIndex] == au)
			{
				hashTable[hashIndex] = au->next;
			}
			else
			{
				if (au->prev)	au->prev->next = au->next;
				if (au->next)	au->next->prev = au->prev;
			}

			// Remove this allocation from our stats

			stats.totalReportedMemory -= static_cast<unsigned int>(au->reportedSize);
			stats.totalActualMemory   -= static_cast<unsigned int>(au->actualSize);
			stats.totalAllocUnitCount--;


            //accumulate the tag stats
            TaggedStat *ts = addTag(au->sourceTag);

            //make sure the tag's data was found
            if (ts){
                ts->totalCount--;
                ts->totalSize -= au->reportedSize;
                
                if (ts->totalSize < 0)
                    log("[!] ----> ERROR-> allocation size for the tag '%s' has dropped below 0 during a block free!\r\n", au->sourceTag);
            }


			// Add this allocation unit to the front of our reservoir of unused allocation units

			//memset(au, 0, sizeof(sAllocUnit));
			au->next = reservoir;
            au->prev = NULL;
			reservoir = au;

            currentBlockCount--;
		}

		// Resetting the globals insures that if at some later time, somebody calls our memory manager from an unknown
		// source (i.e. they didn't include our H file) then we won't think it was the last allocation.

		resetGlobals();

#ifdef ENABLE_VALIDATION
		// Validate every single allocated unit in memory
		if (alwaysValidateAll) 
            m_validateAllAllocUnits();
#endif

		// If we're in the midst of static deinitialization time, track any pending memory leaks

		if (staticDeinitTime) dumpLeakReport();
	}
	catch(const char *err)
	{
		// Deal with errors

		log("[!] %s\r\n", err);
		resetGlobals();
	}

	#ifdef TEST_MEMORY_MANAGER
	log("[D] EXIT : m_deallocator()\r\n");
	#endif
}

// ---------------------------------------------------------------------------------------------------------------------------------
// -DOC- The following utilitarian allow you to become proactive in tracking your own memory, or help you narrow in on those tough
// bugs.
// ---------------------------------------------------------------------------------------------------------------------------------
#ifdef ENABLE_VALIDATION
bool	m_validateAddress(const void *reportedAddress)
{
	// Just see if the address exists in our allocation routines

	return findAllocUnit(reportedAddress) != NULL;
}
#endif

// ---------------------------------------------------------------------------------------------------------------------------------
#ifdef ENABLE_VALIDATION
bool	m_validateAllocUnit(const sAllocUnit *allocUnit)
{
	// Make sure the padding is untouched

	DWORD	*pre;
	DWORD	*post;
	bool	errorFlag = false;


    if (allocUnit->allocationType == m_alloc_memalign){
        pre = reinterpret_cast<DWORD *>(allocUnit->reportedAddress) - paddingSize;
        post = reinterpret_cast<DWORD *>((char *)allocUnit->reportedAddress + allocUnit->reportedSize);
    }

    else{
        pre = reinterpret_cast<DWORD *>(allocUnit->actualAddress);
        post = reinterpret_cast<DWORD *>((char *)allocUnit->actualAddress + allocUnit->actualSize - paddingSize * sizeof(DWORD));
    }


	for (unsigned int i = 0; i < paddingSize; i++, pre++, post++)
	{
		if (*pre != (DWORD) prefixPattern)
		{
			log("[!] A memory allocation unit was corrupt because of an underrun:\r\n");
			m_dumpAllocUnit(allocUnit, "  ");
			errorFlag = true;
		}

		// If you hit this assert, then you should know that this allocation unit has been damaged. Something (possibly the
		// owner?) has underrun the allocation unit (modified a few bytes prior to the start). You can interrogate the
		// variable 'allocUnit' to see statistics and information about this damaged allocation unit.
		m_assert(*pre == static_cast<DWORD>(prefixPattern));

		if (*post != static_cast<DWORD>(postfixPattern))
		{
			log("[!] A memory allocation unit was corrupt because of an overrun:\r\n");
			m_dumpAllocUnit(allocUnit, "  ");
			errorFlag = true;
		}

		// If you hit this assert, then you should know that this allocation unit has been damaged. Something (possibly the
		// owner?) has overrun the allocation unit (modified a few bytes after the end). You can interrogate the variable
		// 'allocUnit' to see statistics and information about this damaged allocation unit.
		m_assert(*post == static_cast<DWORD>(postfixPattern));
	}

	// Return the error status (we invert it, because a return of 'false' means error)

	return !errorFlag;
}
#endif // ENABLE_VALIDATION

// ---------------------------------------------------------------------------------------------------------------------------------

#ifdef ENABLE_VALIDATION
bool	m_validateAllAllocUnits()
{
	// Just go through each allocation unit in the hash table and count the ones that have errors

	unsigned int	errors = 0;
	unsigned int	allocCount = 0;
	for (unsigned int i = 0; i < hashSize; i++)
	{
		sAllocUnit	*ptr = hashTable[i];
		while(ptr)
		{
			allocCount++;
			if (!m_validateAllocUnit(ptr)) errors++;
			ptr = ptr->next;
		}
	}

	// Test for hash-table correctness

	if (allocCount != stats.totalAllocUnitCount)
	{
		log("[!] Memory tracking hash table corrupt!\r\n");
		errors++;
	}

	// If you hit this assert, then the internal memory (hash table) used by this memory tracking software is damaged! The
	// best way to track this down is to use the alwaysLogAll flag in conjunction with STRESS_TEST macro to narrow in on the
	// offending code. After running the application with these settings (and hitting this assert again), interrogate the
	// memory.log file to find the previous successful operation. The corruption will have occurred between that point and this
	// assertion.
	m_assert(allocCount == stats.totalAllocUnitCount);

	// If you hit this assert, then you've probably already been notified that there was a problem with a allocation unit in a
	// prior call to validateAllocUnit(), but this assert is here just to make sure you know about it. :)
	m_assert(errors == 0);

	// Log any errors

	if (errors) 
        log("[!] While validting all allocation units, %d allocation unit(s) were found to have problems\r\n", errors);

	// Return the error status

	return errors != 0;
}
#endif

// ---------------------------------------------------------------------------------------------------------------------------------
// -DOC- Unused RAM calculation routines. Use these to determine how much of your RAM is unused (in bytes)
// ---------------------------------------------------------------------------------------------------------------------------------

/*unsigned int	m_calcUnused(const sAllocUnit *allocUnit)
{
# ifdef TRANSGAMING
    return 0;

# else
	const DWORD		*ptr = reinterpret_cast<const DWORD *>(allocUnit->reportedAddress);
	unsigned int		count = 0;

	for (unsigned int i = 0; i < allocUnit->reportedSize; i += sizeof(DWORD), ptr++)
	{
		if (*ptr == unusedPattern) count += sizeof(DWORD);
	}

	return count;
# endif
}

// ---------------------------------------------------------------------------------------------------------------------------------

unsigned int	m_calcAllUnused()
{
# ifdef TRANSGAMING
    return 0;

# else
	// Just go through each allocation unit in the hash table and count the unused RAM

	unsigned int	total = 0;
	for (unsigned int i = 0; i < hashSize; i++)
	{
		sAllocUnit	*ptr = hashTable[i];
		while(ptr)
		{
			total += m_calcUnused(ptr);
			ptr = ptr->next;
		}
	}

	return total;
# endif
}//*/

// ---------------------------------------------------------------------------------------------------------------------------------
// -DOC- The following functions are for logging and statistics reporting.
// ---------------------------------------------------------------------------------------------------------------------------------

#ifdef ENABLE_VALIDATION
void m_printBuf(const char *msg, const void *buf, const int size, bool msgOnFirstLine = false){
    const unsigned char *   data = static_cast<const unsigned char *>(buf);
    int                     count = 0;
    int                     count2;
    const int               numToPrint = 64;
    const int               numPerLine = 16;

    

    for (int j = 0; j < numToPrint / numPerLine && count < size; j++){
        if (msgOnFirstLine && j == 0 || j > 0)
            log("%s", msg);

        count2 = count;

        for (int i = 0; i < numPerLine && count < size; i++){
            log("%02X ", data[count]);
            count++;
        }

        //end of the line => print out hex versions of 
        log(" [");
        for (int i = 0; i < numPerLine && count2 + i < size; i++){
            if (data[count2 + i] >= 32 && data[count2 + i] <= 127)
                log("%c", data[count2 + i]);
            else
                log(" ");
        }
        log("]\r\n", msg);
    }
}

void	m_dumpAllocUnit(const sAllocUnit *allocUnit, const char *prefix)
{
	log("[I]   %sAddress (reported): %010p\r\n",                prefix, allocUnit->reportedAddress);
	log("[I]   %sAddress (actual)  : %010p\r\n",                prefix, allocUnit->actualAddress);
	log("[I]   %sSize (reported)   : 0x%08X (%s)\r\n",          prefix, static_cast<unsigned int>(allocUnit->reportedSize), memorySizeString(static_cast<unsigned int>(allocUnit->reportedSize)));
	log("[I]   %sSize (actual)     : 0x%08X (%s)\r\n",          prefix, static_cast<unsigned int>(allocUnit->actualSize), memorySizeString(static_cast<unsigned int>(allocUnit->actualSize)));
	log("[I]   %sOwner             : %s (%d)::%s() (%s)\r\n",   prefix, allocUnit->sourceFile, allocUnit->sourceLine, allocUnit->sourceFunc, allocUnit->sourceTag);
	log("[I]   %sAllocation type   : %s\r\n",                   prefix, allocationTypes[allocUnit->allocationType]);
	log("[I]   %sAllocation number : %d\r\n",                   prefix, allocUnit->allocationNumber);
    log("[I]   %sPadding values    : ",                         prefix);
    
    if (allocUnit->allocationType == m_alloc_memalign)
        m_printBuf("             PreBuffer      : ",            reinterpret_cast<DWORD *>(allocUnit->reportedAddress) - paddingSize, paddingSize * sizeof(DWORD));

    else
        m_printBuf("             PreBuffer      : ",            allocUnit->actualAddress, paddingSize * sizeof(DWORD));
    

    m_printBuf("             PostBuffer     : ",                static_cast<char *>(allocUnit->reportedAddress) + allocUnit->reportedSize, paddingSize * sizeof(DWORD));

    log("[I]   %sContents          : ",                         prefix);
    m_printBuf("                              ",                allocUnit->reportedAddress, allocUnit->reportedSize);
}
#endif

// ---------------------------------------------------------------------------------------------------------------------------------

void	m_dumpMemoryReport(const char *filename, const bool dumpAllocs, const bool humanReadable)
{
#ifndef TRANSGAMING_MASTERING
	// Open the report file

	FILE	*fp = NULL;
	
#ifdef TRANSGAMING
    printf("m_dumpMemoryReport(): dumping memory report to '%s'\n", filename);

	fp = TNG_fopen(filename, "wb");
# define fprintf TNG_fprintf
    /*char buf[MAX_PATH];
    

    snprintf(buf, MAX_PATH, "host0:%s", filename);

	fp = fopen(buf, "wb");//*/
#else
	fp = fopen(filename, "wb");
#endif
	// If you hit this assert, then the memory report generator is unable to log information to a file (can't open the file for
	// some reason.)
	//m_assert(fp);
	if (!fp) return;

    // Header

    /*static  char    timeString[25];
    memset(timeString, 0, sizeof(timeString));//*/
    time_t  t = time(NULL);
    struct  tm *tme = localtime(&t);
	fprintf(fp, " ---------------------------------------------------------------------------------------------------------------------------------- \r\n");
    fprintf(fp, "|                                             Memory report for: %02d/%02d/%04d %02d:%02d:%02d                                               |\r\n", tme->tm_mon + 1, tme->tm_mday, tme->tm_year + 1900, tme->tm_hour, tme->tm_min, tme->tm_sec);
	fprintf(fp, " ---------------------------------------------------------------------------------------------------------------------------------- \r\n");
	fprintf(fp, "\r\n");
	fprintf(fp, "\r\n");

	// Report summary
#ifdef TRANSGAMING_PSP
    SceKernelModuleInfo mi;
    SceUID              id = sceKernelGetModuleId();
    long32              totalMem = 0;

    memset(&mi, 0, sizeof(mi));
    fprintf(fp, " ---------------------------------------------------------------------------------------------------------------------------------- \r\n");
	fprintf(fp, "|                                                           M O D U L E                                                            |\r\n");
	fprintf(fp, " ---------------------------------------------------------------------------------------------------------------------------------- \r\n");

    if (id > 0){
        if (sceKernelQueryModuleInfo(id, &mi) == SCE_KERNEL_ERROR_OK){
            fprintf(fp, "                        module name: '%s' (0x%X)\r\n", mi.modname, mi.modname);
            fprintf(fp, "                  module attributes: 0x%08X\r\n",      mi.modattribute);
            fprintf(fp, "                     module version: %d.%d\r\n",       mi.modversion[0], mi.modversion[1]);
            fprintf(fp, "                entry point address: 0x%X\r\n",        mi.entry_addr);
            fprintf(fp, "                           gp value: %d\r\n",          mi.gp_value);
            fprintf(fp, "                       text address: 0x%X\r\n",        mi.text_addr);
            fprintf(fp, "                          text size: %s\r\n",          insertCommas(mi.text_size));
            fprintf(fp, "                          data size: %s\r\n",          insertCommas(mi.data_size));
            fprintf(fp, "                           bss size: %s\r\n",          insertCommas(mi.bss_size));
            fprintf(fp, "                      segment count: %d\r\n",          mi.nsegment);

            for (int i = 0; i < mi.nsegment; i++){
                fprintf(fp, "                     Segment number: %d\r\n", i);
                fprintf(fp, "                       address:      0x%X\r\n"
                            "                       size:         %s\r\n",
                            mi.segmentaddr[i],
                            insertCommas(mi.segmentsize[i]));

                totalMem += mi.segmentsize[i];
            }

            fprintf(fp,     "                 Segment size total: %s\r\n",  insertCommas(totalMem));
            fprintf(fp,     "\r\n");
            fprintf(fp,     "                  Total Free Memory: %s\r\n",  insertCommas(sceKernelTotalFreeMemSize()));
            fprintf(fp,     "                    Max Free Memory: %s\r\n",  insertCommas(sceKernelMaxFreeMemSize()));


        }

        else
            fprintf(fp, "ERROR-> could not get module info!\n");
    }

    else
        fprintf(fp, "ERROR-> could not get module handle!\n");

    fprintf(fp, "\r\n");
	fprintf(fp, "\r\n");

#endif

	fprintf(fp, " ---------------------------------------------------------------------------------------------------------------------------------- \r\n");
	fprintf(fp, "|                                                           T O T A L S                                                            |\r\n");
	fprintf(fp, " ---------------------------------------------------------------------------------------------------------------------------------- \r\n");
	fprintf(fp, "              Allocation unit count: %10s\r\n", insertCommas(stats.totalAllocUnitCount));
	fprintf(fp, "            Reported to application: %s\r\n", memorySizeString(stats.totalReportedMemory));
	fprintf(fp, "         Actual total memory in use: %s\r\n", memorySizeString(stats.totalActualMemory));
	fprintf(fp, "           Memory tracking overhead: %s\r\n", memorySizeString(stats.totalActualMemory - stats.totalReportedMemory));
    fprintf(fp, "\r\n");
    fprintf(fp, "                  total memory used: %s\r\n", memorySizeString(m_getMemUsed()));
#ifdef TRACK_UNTRACKEDMEMORY
	int UntrackedTotal = 0;
	for (int i=0; i<UTTYPE_MAX; i++)
		UntrackedTotal += UntrackedMemory[i].Size;

	fprintf(fp, "                          Untracked: %s\r\n", memorySizeString(UntrackedTotal));
#endif
	fprintf(fp, "\r\n");

	fprintf(fp, " ---------------------------------------------------------------------------------------------------------------------------------- \r\n");
	fprintf(fp, "|                                                            P E A K S                                                             |\r\n");
	fprintf(fp, " ---------------------------------------------------------------------------------------------------------------------------------- \r\n");
	fprintf(fp, "              Allocation unit count: %10s\r\n", insertCommas(stats.peakAllocUnitCount));
	fprintf(fp, "            Reported to application: %s\r\n", memorySizeString(stats.peakReportedMemory));
	fprintf(fp, "                             Actual: %s\r\n", memorySizeString(stats.peakActualMemory));
	fprintf(fp, "           Memory tracking overhead: %s\r\n", memorySizeString(stats.peakActualMemory - stats.peakReportedMemory));
	fprintf(fp, "\r\n");

	fprintf(fp, " ---------------------------------------------------------------------------------------------------------------------------------- \r\n");
	fprintf(fp, "|                                                      A C C U M U L A T E D                                                       |\r\n");
	fprintf(fp, " ---------------------------------------------------------------------------------------------------------------------------------- \r\n");
	fprintf(fp, "              Allocation unit count: %s\r\n", memorySizeString(stats.accumulatedAllocUnitCount));
	fprintf(fp, "            Reported to application: %s\r\n", memorySizeString(stats.accumulatedReportedMemory));
	fprintf(fp, "                             Actual: %s\r\n", memorySizeString(stats.accumulatedActualMemory));
	fprintf(fp, "\r\n");


	fprintf(fp, " ---------------------------------------------------------------------------------------------------------------------------------- \r\n");
	fprintf(fp, "|                                                           B Y  T A G S                                                           |\r\n");
	fprintf(fp, " ---------------------------------------------------------------------------------------------------------------------------------- \r\n");
    
    for (int i = 0; i < g_sourceTagCount; i++){
        fprintf(fp, "                           Tag name: '%s'\r\n", g_tagTable[i].name);
        fprintf(fp, "                  Total Alloc Count: %10d\r\n", g_tagTable[i].totalCount);
        fprintf(fp, "            Accumulated Alloc Count: %10d\r\n", g_tagTable[i].accumulatedCount);
        fprintf(fp, "                   Peak Alloc Count: %10d\r\n", g_tagTable[i].peakCount);
        fprintf(fp, "                   Total Memory Use: %s\r\n", memorySizeString(g_tagTable[i].totalSize));
        fprintf(fp, "             Accumulated Memory Use: %s\r\n", memorySizeString(g_tagTable[i].accumulatedSize));
        fprintf(fp, "                    Peak Memory Use: %s\r\n", memorySizeString(g_tagTable[i].peakSize));
	    fprintf(fp, "\r\n");
    }


	/*fprintf(fp, " ---------------------------------------------------------------------------------------------------------------------------------- \r\n");
	fprintf(fp, "|                                                           U N U S E D                                                            |\r\n");
	fprintf(fp, " ---------------------------------------------------------------------------------------------------------------------------------- \r\n");
	fprintf(fp, "    Memory allocated but not in use: %s\r\n", memorySizeString(m_calcAllUnused()));
	fprintf(fp, "\r\n");//*/

	fprintf(fp, " ---------------------------------------------------------------------------------------------------------------------------------- \r\n");
	fprintf(fp, "|                                                        U N T R A C K E D                                                         |\r\n");
	fprintf(fp, " ---------------------------------------------------------------------------------------------------------------------------------- \r\n");

#ifdef TRACK_UNTRACKEDMEMORY
	for (int i=0; i<UTTYPE_MAX; i++)
	{
		if (UntrackedMemory[i].Size)
		{
			fprintf(fp, "                           Tag name: '%s'\r\n", UntrackedMemory[i].Name);
			fprintf(fp, "                   Total Memory Use: %s\r\n", memorySizeString(UntrackedMemory[i].Size));
			fprintf(fp, "\r\n");
		}
	}
#endif

    printf("m_dumpMemoryReport(): Finished dumping memory report");
    if (dumpAllocs){
        printf("...  dumping allocations\n");
	    dumpAllocations(fp, humanReadable);
        printf("m_dumpMemoryReport(): done dumping allocations\n");
    }
    else
        printf("\n");

#ifdef TRANSGAMING
# undef fprintf
	TNG_fclose(fp);
    printf("m_dumpMemoryReport(): file closed\n");
#else
	fclose(fp);
#endif
#endif
}

// ---------------------------------------------------------------------------------------------------------------------------------

/*sMStats	m_getMemoryStatistics()
{
	return stats;
}//*/

/*void* m_malloc(size_t size)
{
	return malloc(size);
}

void* m_calloc(size_t size)
{
	return calloc(1,size);
}

void* m_realloc(void *ptr, size_t size)
{
	return realloc(ptr,size);
}
void m_free(void *ptr)
{
	free(ptr);
}

#ifndef _CRTDBG_MAP_ALLOC
	#include <malloc.h>
#endif

ENGINE_API void* m_malloc_a16(size_t size)
{
#if defined( TRANSGAMING_PS2 )
	return PS2_aligned_malloc(size,16);
#elif defined (TRANSGAMING_PSP )
	return PSP_aligned_malloc(size,16);
#else
	return _aligned_malloc(size,16);
#endif
}

ENGINE_API void* m_realloc_a16(void *ptr, size_t size)
{
#if defined( TRANSGAMING_PS2 )
	return PS2_aligned_realloc(ptr,size,16);
#elif defined (TRANSGAMING_PSP )
	return PSP_aligned_realloc(ptr,size,16);
#else
	return _aligned_realloc(ptr,size,16);
#endif
}
ENGINE_API void m_free_a16(void *ptr)
{
#if defined( TRANSGAMING_PS2 )
	PS2_aligned_free(ptr);
#elif defined (TRANSGAMING_PSP )
	return PSP_aligned_free(ptr);
#else
	_aligned_free(ptr);
#endif
}//*/

#endif  //ifdef MEMORY_TEST

#ifdef TRANSGAMING_PSP
float m_getModuleSize(){
    SceKernelModuleInfo mi;
    SceUID              id = sceKernelGetModuleId();
    long32              totalMem = 0;


    if (id > 0){
        if (sceKernelQueryModuleInfo(id, &mi) == SCE_KERNEL_ERROR_OK){
            for (int i = 0; i < mi.nsegment; i++)
                totalMem += mi.segmentsize[i];

            return totalMem;
        }
    }

    return 0;
}
#elif defined(TRANSGAMING_PS2)
float m_getModuleSize(){
    extern unsigned int _end;
    extern unsigned int _stack_size;
    return (float)((unsigned int)&_end + (unsigned int)&_stack_size);
}
#endif

size_t m_getMemUsed(){
    return mallinfo().uordblks;
}

// ---------------------------------------------------------------------------------------------------------------------------------
// mmgr.cpp - End of file
// ---------------------------------------------------------------------------------------------------------------------------------

#ifdef TRACK_UNTRACKEDMEMORY

extern "C"
{
	typedef struct
	{
		unsigned int	Magic;
		unsigned int	Size;
		bool			Tracked;
		int				Type;
		int				UTType;
	}MEMBLOCK;


	void *malloc(size_t Size)
	{
		_reent	re;

		GetUTType(sourceTag);

		void		*Memory	= _malloc_r(&re, Size+64);
		MEMBLOCK	*MB		= (MEMBLOCK*)Memory;
		MB->Magic			= 0x0c0dface;
		MB->Size			= Size;
		MB->Tracked			= AllocatorBusy;
		MB->UTType			= UTType;

		if (!AllocatorBusy)
			UntrackedMemory[UTType].Size+=Size;

		return((void*)((int)Memory+64));

	}

	void *calloc(size_t _NumOfElements, size_t _SizeOfElements)
	{
		_reent	re;

		GetUTType(sourceTag);

		void	*Memory = _malloc_r(&re, (_NumOfElements*_SizeOfElements)+64);
		MEMBLOCK	*MB		= (MEMBLOCK*)Memory;
		MB->Magic			= 0x0c0dface;
		MB->Size			= (_NumOfElements*_SizeOfElements);
		MB->Tracked			= AllocatorBusy;
		MB->UTType			= UTType;

		if (!AllocatorBusy)
			UntrackedMemory[UTType].Size += (_NumOfElements*_SizeOfElements);

		return((void*)((int)Memory+64));
	}

	void *memalign(size_t Align, size_t Size)
	{
		_reent	re;

		GetUTType(sourceTag);

		void	*Memory = _memalign_r(&re, Align, Size+64);
		MEMBLOCK	*MB		= (MEMBLOCK*)Memory;
		MB->Magic			= 0x0c0dface;
		MB->Size			= Size;
		MB->Tracked			= AllocatorBusy;
		MB->UTType			= UTType;

		if (!AllocatorBusy)
			UntrackedMemory[UTType].Size+=Size;

		return((void*)((int)Memory+64));
	}

	void *realloc(void *ptr, size_t Size)
	{
		_reent	re;

		if (ptr == NULL)
			return(malloc(Size));

		GetUTType(sourceTag);

		void	*Memory = _realloc_r(&re, (void*)((int)ptr-64), Size+64);
		MEMBLOCK	*MB		= (MEMBLOCK*)Memory;
		MB->Magic			= 0x0c0dface;
		MB->Size			= Size;
		MB->Tracked			= AllocatorBusy;
		MB->UTType			= UTType;

		if (!AllocatorBusy)
			UntrackedMemory[UTType].Size+=Size;

		return((void*)((int)Memory+64));
	}

	void free(void *ptr)
	{
		_reent	re;

		if (ptr)
		{
			MEMBLOCK	*MB		= (MEMBLOCK*)((int)ptr-64);

			if (!MB->Tracked)
				UntrackedMemory[MB->UTType].Size -= MB->Size;

			_free_r(&re, (void*)((int)ptr-64));
		}
	}

	char *strdup (const char *s)
	{
		size_t len = strlen (s);
		char *mem = (char *)malloc (len + 1);
		strcpy (mem, s);
		mem[len] = 0;
		return mem;
	}


};

#endif


