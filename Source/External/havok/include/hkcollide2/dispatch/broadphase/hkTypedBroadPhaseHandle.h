/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_TYPED_BROAD_PHASE_HANDLE
#define HK_TYPED_BROAD_PHASE_HANDLE

#include <hkcollide2/broadphase/hkBroadPhaseHandle.h>

///An hkBroadPhaseHandle with a type. hkCollidable is a subclass of hkTypedBroadPhaseHandle
/// with the type HK_BROAD_PHASE_ENTITY.
class hkTypedBroadPhaseHandle : public hkBroadPhaseHandle
{
	public:
		
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_CDINFO, hkTypedBroadPhaseHandle);

			///Creates a new hkTypedBroadPhaseHandle of the specified type
		inline hkTypedBroadPhaseHandle( int type );

			///Gets the type
		inline int getType() const;
		
			///Gets the unique ID.
		inline hkUint32 getUniqueId() const;

			///Gets the collision filter info. This is an identifying value used by collision filters
			/// - for example, if a group collision filter is used, this value would specify a collision group.
		inline hkUint32 getCollisionFilterInfo() const;

			///Sets the collision filter info. This is an identifying value used by collision filters
			/// - for example, if a group collision filter is used, this value would specify a collision group.
		inline void setCollisionFilterInfo( hkUint32 info );

	private:

		hkUint32 m_uniqueId;
		int m_type;
		hkUint32 m_collisionFilterInfo;

	public:

		inline void setUniqueId( hkUint32 id );


};

#include <hkcollide2/dispatch/broadphase/hkTypedBroadPhaseHandle.inl>

#endif // HK_TYPED_BROAD_PHASE_HANDLE

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
