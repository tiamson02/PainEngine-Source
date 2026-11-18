/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkcollide2/hkCollide2.h>

#include <hkcollide2/shape/hkAabbCastCallback.h>


hkAabbCastCallback::hkAabbCastCallback( const hkVector4& fromWorld , const hkVector4& toWorld , const hkVector4& halfExtents )
: hkRayCastCallback ( fromWorld , toWorld ),
 m_halfExtents( halfExtents )
{
}


hkReal	hkAabbCastCallback::castPrimitive( const hkBroadPhaseHandle* broadphaseHandle )
{
	hkCollidable* col = (hkCollidable*) broadphaseHandle;
	const hkShape* shape = col->getShape();
	if (shape)
	{

		const hkTransform* trans = col->getTransform();
		hkVector4 shapefrom,shapeto;
		shapefrom.setTransformedInversePos(*trans,m_fromWorld);
		shapeto.setTransformedInversePos(*trans,m_toWorld);
		
		if (shape->castAabb( shapefrom , shapeto , trans->getRotation(), m_halfExtents , m_rayResults ))
		{
			m_hitObject = col;
		}
	}


	return m_rayResults.m_mindist;
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
