/*	  Copyright (C) 2000,2001,2002  Sony Computer Entertainment America
       	  
       	  This file is subject to the terms and conditions of the GNU Lesser
	  General Public License Version 2.1. See the file "COPYING" in the
	  main directory of this archive for more details.                             */

/********************************************
 * includes
 */

#include "ps2s/utils.h"

#include <stdarg.h>

printf_t printf_override = NULL;

// set_printf_func lets client code pass a function of its own to handle printf's
// if not setup, the default printf will be invoked normally. This is helpful
// to let a specialized app console display messages.

extern "C" void set_printf_func(printf_t printf_func)
{
	printf_override = printf_func;
}

extern "C" void switched_printf(const char* fmt,...)
{
	va_list argptr;
	va_start(argptr, fmt);
	if (printf_override)
	{
		printf_override(fmt, argptr);
	}
	else
	{
		vprintf(fmt, argptr);
	}
	va_end(argptr);
}

