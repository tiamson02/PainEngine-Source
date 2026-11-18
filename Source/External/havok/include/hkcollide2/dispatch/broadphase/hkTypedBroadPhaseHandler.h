/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_BROAD_PHASE_HANDLER
#define HK_BROAD_PHASE_HANDLER

#include <hkcollide2/dispatch/broadphase/hkNullBroadPhaseListener.h>
#include <hkcollide2/dispatch/hkCollisionFilter.h>

class hkTypedBroadPhaseHandlePair;
class hkCollisionFilter;

class hkTypedBroadPhaseHandler
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_CDINFO, hkTypedBroadPhaseHandler);

		hkTypedBroadPhaseHandler();
		
		~hkTypedBroadPhaseHandler();


			/// swap each pair's internal data so A > B, then sort
		static void HK_CALL sortPairs( hkTypedBroadPhaseHandlePair* pairs, int numPairs );

			/// update the dispatacher by merging
		void updateBroadPhaseLists(	hkTypedBroadPhaseHandlePair* newPairs, int numNewPairs, 
									hkTypedBroadPhaseHandlePair* deletedPairs, int numDeletedPairs ) const;


		inline hkBroadPhaseListener* getBroadPhaseListener( int typeA, int typeB );

		inline void setBroadPhaseListener( hkBroadPhaseListener* listener, int typeA, int typeB );

		inline hkBroadPhaseListener* getNullBroadPhaseListener();


	protected:


		int m_numTypes;
		hkInplaceArray<hkBroadPhaseListener*, 4> m_broadPhaseListeners;
		hkNullBroadPhaseListener m_nullBroadPhaseListener;
};

#include <hkcollide2/dispatch/broadphase/hkTypedBroadPhaseHandler.inl>

#endif // HK_BROAD_PHASE_HANDLER

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
