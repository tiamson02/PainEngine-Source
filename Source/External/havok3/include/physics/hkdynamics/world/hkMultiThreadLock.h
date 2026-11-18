/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_MULTI_THREADING_TYPES_H
#define HK_DYNAMICS2_MULTI_THREADING_TYPES_H

#ifdef HK_DEBUG
	/// Enabling multi threading debugging. This is currently only done in debug mode.
	/// If you use a debug build and link against release libraries, everything should work fine except you do not get multithreaded debugging.
	/// If you use a release build and link against debug libraries, you have to enable this define in order to avoid the asserts
#	define HK_DEBUG_MULTI_THREADING
#endif

#ifdef HK_DEBUG_MULTI_THREADING
#	define HK_ON_DEBUG_MULTI_THREADING(x) x
#else
#	define HK_ON_DEBUG_MULTI_THREADING(x)
#endif

	/// A structure used in hkWorld to provide debug access checking.
class hkMultiThreadLock
{
	public:
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR_MT(HK_MEMORY_CLASS_WORLD, hkMultiThreadLock);
		HK_DECLARE_REFLECTION();

		enum AccessType
		{
			HK_ACCESS_IGNORE = 0,
			HK_ACCESS_RO     = 1,
			HK_ACCESS_RW     = 2,
		};

		HK_FORCE_INLINE hkMultiThreadLock();

			/// Lock this class and all child classes for read only access for this thread
			/// Note: This is only for debugging and does not wait to get exclusive access, 
			/// but simply assert if another thread locked the hkWorld. You must read the
			/// user guide about multithreading to use this.
		HK_FORCE_INLINE void markForRead( )
		{
			HK_ON_DEBUG_MULTI_THREADING( markForReadImpl() );
		}

			/// Lock this class and all child classes for read write access for this thread
			/// Note: This is only for debugging and does not wait to get exclusive access, 
			/// but simply assert if another thread locked the hkWorld. You must read the
			/// user guide about multithreading to use this.
		HK_FORCE_INLINE void markForWrite( )
		{
			HK_ON_DEBUG_MULTI_THREADING( markForWriteImpl() );
		}

			/// Undo lockForRead
			/// Note: This is only for debugging and does not wait to get exclusive access, 
			/// but simply assert if another thread locked the hkWorld. You must read the
			/// user guide about multithreading to use this.
		HK_FORCE_INLINE void unmarkForRead( )
		{
			HK_ON_DEBUG_MULTI_THREADING( unmarkForReadImpl() );
		}

			/// Unlock For write
			/// Note: This is only for debugging and does not wait to get exclusive access, 
			/// but simply assert if another thread locked the hkWorld. You must read the
			/// user guide about multithreading to use this.
		HK_FORCE_INLINE void unmarkForWrite()
		{
			HK_ON_DEBUG_MULTI_THREADING( unmarkForWriteImpl() );
		}

			/// Disable checks
		void disableChecks();

			/// Re-enables checks.
		void enableChecks();

			/// Get whether checking is enabled
		bool isCheckingEnabled() const;

		void accessCheck( AccessType type ) const;

		static void HK_CALL accessCheckWithParent( const class hkWorld* world, AccessType parentType, const hkMultiThreadLock& lock, AccessType type );

	public:
		enum
		{
			DISABLED = 0xffffffd1,
			LOCKED_RO = 0xffffffe1,
			UNLOCKED = 0xfffffff1
		};

		// Stored in a 64bit property anyway, and in the sim islands (not serialized, so we can use more than the previous 8 bits)
		// If the threadid is bigger we can catch similar ids better (the id is not an index but the actual platform id)
		hkUint32 m_threadId;
		hkInt32 m_lockCount;
	protected:

		void markForReadImpl( );
		void markForWriteImpl( );
		void unmarkForReadImpl( );
		void unmarkForWriteImpl();
};


#ifdef HK_DEBUG_MULTI_THREADING
#	define HK_ACCESS_CHECK( world, worldAccess, object, objectAccess ) hkMultiThreadLock::accessCheckWithParent( world, hkMultiThreadLock:: worldAccess, object->getMultiThreadLock(), hkMultiThreadLock:: objectAccess )
#	define HK_WORLD_ACCESS_CHECK( world, worldAccess ) if ( world ){ world->m_multiThreadLock.accessCheck( hkMultiThreadLock:: worldAccess ); }

	hkMultiThreadLock::hkMultiThreadLock(): m_threadId( (hkUint32)UNLOCKED ), m_lockCount(0) {}

#else
#	define HK_ACCESS_CHECK( world, worldAccess, object, objectAccess ) 
#	define HK_WORLD_ACCESS_CHECK( world, worldAccess ) 
	hkMultiThreadLock::hkMultiThreadLock(): m_threadId( (hkUint32) DISABLED ), m_lockCount (0){}
#endif


#endif // HK_DYNAMICS2_MULTI_THREADING_TYPES_H


/*
* Havok SDK - PUBLIC RELEASE, BUILD(#20060114)
*
* Confidential Information of Havok.  (C) Copyright 1999-2006 
* Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
* Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
* rights, and intellectual property rights in the Havok software remain in
* Havok and/or its suppliers.
*
* Use of this software for evaluation purposes is subject to and indicates 
* acceptance of the End User licence Agreement for this product. A copy of 
* the license is included with this software and is also available from salesteam@havok.com.
*
*/
