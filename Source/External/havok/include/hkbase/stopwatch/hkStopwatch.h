/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HKBASE_HKSTOPWATCH_H
#define HKBASE_HKSTOPWATCH_H

/// An hkStopwatch provides high resolution timing.
/// Time is stored internally as a 64 bit integer. 
class hkStopwatch
{
	public:

			/// 
		static hkUint64 HK_CALL getTickCounter();

			/// 
		static hkUint64 HK_CALL getTicksPerSecond();

	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE, hkStopwatch)

			/// Creates a new stopwatch with optional name
		hkStopwatch( const char* name=HK_NULL );

			/// Starts the stopwatch.
		void start();

			/// Stops the stopwatch. The split ticks are also stopped.
		void stop();

			/// Resets the counter and split counter to zero.
		void reset();
	
			/// Checks if the stopwatch is currently running.
		bool isRunning() const;

			/// Returns the name given in the constructor.
		const char* getName() const;

			/// Returns the elapsed time in seconds.
		hkReal getElapsedSeconds() const;

			/// Returns the split time in seconds.
		hkReal getSplitSeconds();

			/// Returns the elapsed time in ticks.
		hkUint64 getElapsedTicks() const;

			/// Returns the split time in ticks. 
		hkUint64 getSplitTicks();

			/// How many times has this stopwatch been stopped?
		int getNumTimings() const;
			
			/// Resumes the stopwatch.
		void resume();

	private:

		static hkReal HK_CALL divide64(hkUint64, hkUint64);

		hkUint64	m_ticks_at_start;
		hkUint64	m_ticks_total;
		hkUint64	m_ticks_at_split;
		hkUint64	m_split_total;
		bool		m_running_flag;
		int			m_num_timings;
		const char*	m_name;
};

#include <hkbase/stopwatch/hkStopwatch.inl>

#endif // HKBASE_HKSTOPWATCH_H

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
