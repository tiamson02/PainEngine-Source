/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_UTILITIES2_SMOOTH_OPERATOR_H
#define HK_UTILITIES2_SMOOTH_OPERATOR_H

#include <hkdynamics2/collide/hkCollisionListener.h>

/// You can add this class either as collision listener to either the world, or a specific entity
/// It implements the contactPointAddedCallback to reject points which can cause unexpected "bumps" as
/// objects slide across each other (for example a box sliding across a triangular mesh).
/// It works by rejecting new contact points which are at new angles, and are below the existing contact 
/// manifold.
class hkSmoothOperator : public hkCollisionListener
{
	public:
		virtual void contactPointAddedCallback( hkContactPointAddedEvent& event );

		virtual void contactPointRemovedCallback( hkContactPointRemovedEvent& event );


};

#endif // HK_UTILITIES2_SMOOTH_OPERATOR_H

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
