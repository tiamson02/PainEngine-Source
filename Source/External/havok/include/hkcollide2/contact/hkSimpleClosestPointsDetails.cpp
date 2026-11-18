/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkcollide2/hkCollide2.h>

#include <hkcollide2/contact/hkSimpleClosestPointsDetails.h>
#include <hkcollide2/dispatch/hkCollisionDispatcher.h>

void hkSimpleClosestPointsDetails::addClosestPointCallback( const ClosestPointEvent& event )
{
	//todo: check out recursive structures and flipping thoroughly. might need more info in this case !
	
	m_contactPoints.expandBy(1);
	const int curindex = m_contactPoints.getSize() - 1;
	hkContactPoint& cp = m_contactPoints[curindex];
	cp.setPosition( event.m_pointInWorld );
	cp.setDistance( event.m_distance );

	
	//flip normal if a and b has been flipped
	if (&m_collidableB == event.m_collidableB) 
	{
		cp.setNormal( event.m_normalInWorld );
	}	
	else
	{
		hkVector4 reversedNormal;
		reversedNormal.setNeg4( event.m_normalInWorld );
		cp.setNormal( reversedNormal );
	}

	if (event.m_distance < m_closestDistance)
	{
		m_closestDistance = event.m_distance;
		m_closestIndex  = curindex;
	}
	
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
