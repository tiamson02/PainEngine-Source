/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkbase/hkBase.h>
#include <eeregs.h>

class hkPs2SystemClock : public hkSystemClock
{
	public:

		hkPs2SystemClock()
			:	m_whichTimer(0), // possible values are (0,1) for (TIMER0, TIMER1) respectively
				m_ticksPerSecond(1),
				m_timerOverflowHandler(-1)
		{
			if (m_timerOverflowHandler == -1)
			{
				volatile unsigned int* tmode  = 0;
				volatile unsigned int* tcount = 0;
				volatile unsigned int* tcomp  = 0;
				volatile unsigned int* thold  = 0;
				int cause = -1;

				if(m_whichTimer==0)
				{
					tmode  = T0_MODE;
					tcount = T0_COUNT;
					tcomp  = T0_COMP;
					thold  = T0_HOLD;
					cause  = INTC_TIM0;
				}
				else
				{
					tmode  = T1_MODE;
					tcount = T1_COUNT;
					tcomp  = T1_COMP;
					thold  = T1_HOLD;
					cause  = INTC_TIM1;
				}

				s_overflowCount = 0;
				m_timerOverflowHandler = AddIntcHandler(cause, timerOverflowHandler, 0);
				EnableIntc(cause);

				switch (*tmode & 0x03)	// look at the clock selection bits, see eeregs.h:56
				{
					case 0:
						m_ticksPerSecond = 150000000;
						break;
					case 1:
						m_ticksPerSecond = 150000000 / 16;
						break;
					case 2:
						m_ticksPerSecond = 150000000 / 256;
						break;
					case 3:
						m_ticksPerSecond = 1;
						break;
				}

				if ((*tmode & 0x80) == 0)	// reset counts only if counter not already running
				{
					*tcount = 0;
					*tcomp  = 0;
					*thold  = 0;
				}
				// (clear overflow happened flag | enable interrupt on overflow | count up enable)
				*tmode |= (0x200 | 0x80 | 0x20);
			}
		}

		~hkPs2SystemClock()
		{
			if (m_timerOverflowHandler != -1)
			{
				int timer = -1;
				timer = (m_whichTimer == 0) ? INTC_TIM0 : INTC_TIM1;
				DisableIntc(timer);
				RemoveIntcHandler(timer, m_timerOverflowHandler);
				m_timerOverflowHandler = -1;
			}
		}

		static int timerOverflowHandler(int cause)
		{
			switch (cause)
			{
				case INTC_TIM0:
					*T0_MODE |= 0x800;
					break;
				case INTC_TIM1:
					*T1_MODE |= 0x800;
					break;
				default:       
					HK_ASSERT(0 && "unknown overflow event in handler");
					break;
			}
			s_overflowCount++;
			return 0;
		}

		hkUint64 getTicksPerSecond()
		{
			return m_ticksPerSecond;
		}

		hkUint64 getTickCounter()
		{
			HK_ASSERT2(m_timerOverflowHandler != -1, "Cannot use timer, it has not been initialised!");

			unsigned tcount = 0;
			switch (m_whichTimer)
			{
				case 0:
					tcount = *T0_COUNT & 0xffff;
					break;
				case 1:
					tcount = *T1_COUNT & 0xffff;
					break;
				default:
					HK_ASSERT(0 && "Impossible - unknown timer");
					break;
			}

			return ((s_overflowCount << 16) + tcount);
		}

		// possible values are (0,1) for (TIMER0, TIMER1) respectively
		int m_whichTimer;
		hkUint64 m_ticksPerSecond;
		int m_timerOverflowHandler;

		static hkUint64 s_overflowCount;
};

hkUint64 hkPs2SystemClock::s_overflowCount;

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
