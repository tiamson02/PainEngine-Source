/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

inline void hkStopwatch::reset()
{
	m_ticks_at_start = 0;
	m_ticks_total = 0;
	m_ticks_at_split = 0;
	m_split_total = 0;
	m_running_flag = 0;
	m_num_timings = 0;
}

inline hkStopwatch::hkStopwatch(const char* name)
	: m_name(name)
{
	reset();
}

inline void hkStopwatch::start()
{
	HK_ASSERT(! m_running_flag);
	m_running_flag = true;
	m_ticks_at_start = getTickCounter();
	m_ticks_at_split = m_ticks_at_start;
}

inline void hkStopwatch::stop()
{
	HK_ASSERT(m_running_flag);

	m_running_flag = false;
	hkUint64 ticks_now = getTickCounter();
	m_ticks_total += ticks_now - m_ticks_at_start;
	m_split_total += ticks_now - m_ticks_at_split;
	++m_num_timings;
}

inline void hkStopwatch::resume()
{
	m_running_flag = true;
}

inline bool hkStopwatch::isRunning() const
{
	return m_running_flag;
}

inline const char* hkStopwatch::getName() const
{
	return m_name;
}

inline int hkStopwatch::getNumTimings() const
{
	return m_num_timings;
}

/// The split time is the amount of time elapsed since the last
/// call to a getSplit method.
inline hkUint64 hkStopwatch::getSplitTicks()
{
	hkUint64 t = m_split_total;
	if(m_running_flag) 
	{
		hkUint64 now = getTickCounter();
		t += now - m_ticks_at_split;
		m_ticks_at_split = now;
	}
	
	m_split_total = 0;
	return t;
}

/// The split time is the amount of time elapsed since the last
/// call to a getSplit method.  Internally this calls getSplitTicks()
/// and converts the value to seconds.
inline hkReal hkStopwatch::getSplitSeconds()
{
	return divide64(getSplitTicks(), getTicksPerSecond());
}

/// Ticks are a machine dependent quantity and may or may not
/// map exactly to hardware cycles.
inline hkUint64 hkStopwatch::getElapsedTicks() const
{
	hkUint64 ticks = m_ticks_total;
	if( m_running_flag )
	{
		ticks += getTickCounter() - m_ticks_at_start;
	}
	return ticks;
}

inline hkReal hkStopwatch::getElapsedSeconds() const
{
	return divide64(getElapsedTicks(), getTicksPerSecond());
}

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
