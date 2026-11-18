
/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HK_WINDOWS_SYSTEM_CLOCK
#define HK_WINDOWS_SYSTEM_CLOCK

#if defined(HK_PLATFORM_WIN32)
#	include <windows.h>
#elif defined(HK_PLATFORM_XBOX)
#	include <xtl.h>
#else
#	error wrong include
#endif

class hkWindowsSystemClock : public hkSystemClock
{
	public:

		hkUint64 getTickCounter()
		{
			hkUint64 ticks;
			// note: using cpuid as a serializing makes timings more accurate, 
			// at the expense of more overhead. (1.5% without versus 5% with cpuid)
			__asm {
				push ebx
				//cpuid 
				pop ebx
				rdtsc
				mov dword ptr[ticks  ], eax
				mov dword ptr[ticks+4], edx
			}

			return ticks;
		}


		hkUint64 getTicksPerSecond()
		{
			static hkUint64 freq = 0;
			if(freq==0)
			{
				hkUint64 ticks;
				hkUint64 qticks;
				hkUint64 ticks2;
				hkUint64 qticks2;
				double minFactor = 1e6f;

				// Iterate several times
				// We take the minimum value beacuse Sleep() sleeps for at least the specified time
				for (int iter = 0; iter <10; iter++)
				{
					ticks = getTickCounter();
					QueryPerformanceCounter( (LARGE_INTEGER*) &qticks);

					///
					///	Sleep for a little while
					///
					volatile x=1;
					for (int j=0; j< 5000; j++)
					{
						x += x*x;
					}


					ticks2 = getTickCounter();
					QueryPerformanceCounter( (LARGE_INTEGER*) &qticks2);

					// We assume that this is fixed & regular 
					QueryPerformanceFrequency( (LARGE_INTEGER*) &freq);

					// Work our calibration factor
					hkUint64 diff = ticks2 - ticks;
					hkUint64 qdiff = qticks2 - qticks;

					double factor = double(diff)/ double(qdiff);
					
					// Is this smaller?
					if (factor < minFactor)
					{
						minFactor = factor;
					}
				}
				freq = hkUint64(minFactor * freq);
			}
			return freq;
		}

};

#endif  // HK_WINDOWS_SYSTEM_CLOCK


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
