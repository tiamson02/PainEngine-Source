/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


extern "C"
{
#include <stdio.h>
#include <stdlib.h>
}

class hkLinuxSystemClock : public hkSystemClock
{
	public:

		hkUint64 getTickCounter()
		{
			hkUint64 ticks;
			__asm__ __volatile__ (	"rdtsc\n\t"
									"movl %%eax,  (%0)\n\t"
									"movl %%edx, 4(%0)\n\t"
										: /* no output regs */
										: "D" (&ticks)
										: "%eax", "%edx");
			return ticks;
		}

		hkUint64 getTicksPerSecond()
		{
			static hkUint64 freq = 0;

			if(freq==0)
			{
				FILE* fin = fopen("/proc/cpuinfo", "r");
				if( fin )
				{
					const int bufsize = 1024;
					char buf[bufsize];
					int buflen = fread(buf, 1, bufsize, fin);
					if( buflen > 0 )
					{
						const char marker[] = "cpu MHz\t\t:";
						const char* mhz = hkString::strStr(buf, marker);
						if( mhz != HK_NULL )
						{
							freq = static_cast<hkUint64>( 1000000 * atof(mhz + sizeof(marker)));
						}
					}
				}

				if( freq == 0 )
				{
					// fallback assumes 500 MHz
					HK_WARN("Unable to detect cpu speed. Do you have /proc kernel support?");
					freq = 500000000;
				}
			}
			return freq;
		}
};

/*
* Havok Game Dynamics SDK - PUBLIC RELEASE, BUILD(#20030622)
*
* (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. The Telekinesys
* Logo, Havok and the Havok buzzsaw logo are trademarks of Telekinesys
* Research Limited.  Title, ownership
* rights, and intellectual property rights in the Software remain in
* Telekinesys Research Limited and/or its suppliers.
*
* Use of this software is subject to and indicates acceptance of the End User licence
* Agreement for this product. A copy of the license is included with this software and
* is also available from info@havok.com.
*
*/ 
