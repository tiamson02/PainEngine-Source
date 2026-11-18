/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkmath2/hkMath.h>
#include <hkutilities2/collide/hkSmoothOperator.h>
#include <hkcollide2/contact/hkCollisionContactPoint.h>
#include <hkdynamics2/collide/hkDynamicsContactMgr.h>

void hkSmoothOperator::contactPointAddedCallback( hkContactPointAddedEvent& event )
{

	hkInplaceArrayAligned16<hkCollisionContactPoint,16> contacts;
	event.m_contactMgr.getContactPoints( contacts, 16 );
	
	for( int i = 0; i < contacts.getSize(); i++ )
	{
		hkReal ep = contacts[i].m_normal.dot3(event.m_contactPoint->getNormal());
		if (  ep < .99f ) // 8 degrees...
		{
			hkVector4 diff;
			diff.setSub4( event.m_contactPoint->getPosition(), contacts[i].m_position );	
			hkReal manifoldDist = diff.dot3( contacts[i].m_normal );

			if( manifoldDist < .1f )
			{
				event.m_status = HK_CONTACT_POINT_INVALID;
				return;
			}
		}
	}
}

void hkSmoothOperator::contactPointRemovedCallback( hkContactPointRemovedEvent& event )
{

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
