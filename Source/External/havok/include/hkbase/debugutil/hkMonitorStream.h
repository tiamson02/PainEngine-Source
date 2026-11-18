/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HKBASE_HKMONITOR_STREAM_H
#define HKBASE_HKMONITOR_STREAM_H

/// This class is used to store monitor information
class hkMonitorStream : public hkSingleton<hkMonitorStream>
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_MONITOR)

		hkMonitorStream();
		~hkMonitorStream();

			/// clear all memory
		static void HK_CALL reset();

			/// sets the size of the stream
		static void HK_CALL resize( int newSize );

	public:
		//
		//	internal use only
		//


			/// get the ptr to the begin of the memory used for timers
		static inline char*& HK_CALL begin() { return m_ptr[0]; } 

			/// get the ptr to the current write ptr
		static inline char*& HK_CALL current() { return m_ptr[1]; } 

			/// get the ptr to the end of the timer memory minus 16
		static inline char*& HK_CALL endMinus16() { return m_ptr[2]; } 

			/// get the ptr to the end of timer memory
		static inline char*& HK_CALL end() { return m_ptr[3]; } 

			/// Check whether at least 16 bytes are free
		static inline bool HK_CALL memoryAvailable( ) { return current() < endMinus16(); }

			/// get a piece of memory from the timer memory 
		static inline void* HK_CALL expandby( int size ) { void* h = current(); current() += size; return h; }


	private:

		static char* m_ptr[4];
};

#endif // HKBASE_HKMONITOR_STREAM_H

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
