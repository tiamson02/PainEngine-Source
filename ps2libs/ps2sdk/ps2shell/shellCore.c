//-----------------------------------------------------------------------------
//                           PS2 Shell System Core
//
// $Workfile: shellCore.c $
// $Author: Mdean $
// $Date: 24/09/03 16:22 $
// $Revision: 4 $
//
// Copyright (c) 2002-3 Sony Computer Entertainment Europe.
// All Rights Reserved.
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// I N C L U D E S
//-----------------------------------------------------------------------------

#include "shellCore.h"


//-----------------------------------------------------------------------------
// S T A T I C   V A R I A B L E S
//-----------------------------------------------------------------------------

static void(*_idle)(int);
static ShellCoreIdleFn *_idle = NULL;
static int _data = 0;

static int _intcCounts[16] = {0};
static int _dmacCounts[16] = {0};


//-----------------------------------------------------------------------------
// G L O B A L   F U N C T I O N S
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// shellCoreSetIdleRoutine()
// Description: Sets a routine which may be called during idle periods
// Parameters:  routine		- routine to call
//				data		- data passed to this routine when called
// Returns:     N/A
// Notes:		This function is not guaranteed to be called at all, but may
//				be called during certain long operations if waiting for
//				external synchronisation.
//-----------------------------------------------------------------------------
void shellCoreSetIdleRoutine(ShellCoreIdleFn *routine,int data)
{
	_idle=routine;
	_data=data;
}

//-----------------------------------------------------------------------------
// shellCoreCallIdleRoutine()
// Description: Calls the previously defined idle routine
// Parameters:  N/A
// Returns:     N/A
//-----------------------------------------------------------------------------
void shellCoreCallIdleRoutine()
{
	if(_idle)
	{
		_idle(_data);
	}
}

int shellCoreAddIntcHandler(int cause,ShellCoreIntcHandlerFn *handler,int next,void* arg)
{
int id=AddIntcHandler2(cause,handler,next,arg);
    ASSERT( -1<id, "Could not register interrupt handler" );
    _intcCounts[cause]++;
    if(_intcCounts[cause]==1) {
     	EnableIntc(cause);
    }
    return id;
}

int shellCoreAddDmacHandler(int cause,ShellCoreDmacHandlerFn *handler,int next,void* arg)
{
int id=AddDmacHandler2(cause,handler,next,arg);
    ASSERT( -1<id, "Could not register DMA handler" );
    _dmacCounts[cause]++;
    if(_dmacCounts[cause]==1) {
     	EnableDmac(cause);
    }
    return id;
}

void shellCoreRemoveIntcHandler(int cause,int id)
{
	_intcCounts[cause]--;
	ASSERT(_intcCounts[cause]>=0,"Too many interrupt handlers removed");
	if(_intcCounts[cause]==0)
		DisableIntc(cause);
	RemoveIntcHandler(cause,id);
}

void shellCoreRemoveDmacHandler(int cause,int id)
{
	_dmacCounts[cause]--;
	ASSERT(_dmacCounts[cause]>=0,"Too many DMAC handlers removed");
	if(_dmacCounts[cause]==0)
		DisableDmac(cause);
	RemoveDmacHandler(cause,id);
}
