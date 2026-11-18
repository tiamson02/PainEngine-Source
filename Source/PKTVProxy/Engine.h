#pragma once

// core stuff
#include "nommgr.h"
#include <new>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "VisualWin32.h"
#include "mmgr.h"

#define PKTV_CONNECTION_DEBUG

class LogBuffer;
class ProxySystem;

extern LogBuffer*		GLog;
extern ProxySystem*		GSystem;
#ifdef PKTV_CONNECTION_DEBUG
extern LogBuffer*		PKTVDLog;
#endif

#pragma warning(disable:4127) //conditional expression is constant

