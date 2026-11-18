/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_UTILITIES2_BROAD_PHASE_BORDER_H
#define HK_UTILITIES2_BROAD_PHASE_BORDER_H

#include <hkmath2/hkMath.h>
#include <hkdynamics2/world/listener/hkWorldDeletionListener.h>

class hkWorld;
class hkEntity;

/// This class monitors objects leaving the broadphase.
class hkBroadPhaseBorder : public hkReferencedObject, private hkWorldDeletionListener
{
	public:

			/// Creates an instance and attaches it to the world. It also adds a second reference to it that gets removed when the world gets deleted
		hkBroadPhaseBorder( hkWorld* world, hkReal broadPhaseSize = hkMath::HK_REAL_MAX );
			
			/// This function is called when an object goes beyond the scope of the broadphase.
			/// By default, it removes the entity from the world, you may implement your own version.
		virtual void maxPositionExceededCallback( hkEntity* entity );

	protected:

		virtual void worldDeletedCallback( hkWorld* world );

		hkWorld* m_world;

};

#endif // HK_UTILITIES2_BROAD_PHASE_BORDER_H

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
