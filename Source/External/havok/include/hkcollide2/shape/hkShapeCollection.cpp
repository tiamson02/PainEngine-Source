/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkcollide2/hkCollide2.h>
#include <hkcollide2/shape/hkShapeCollection.h>

int hkShapeCollection::castRay( const hkVector4& fromLocal, const hkVector4& toLocal, hkRayResults& results ) const
{
	int hasHit = 0;
	int i;
	hkShapeCollection::AllocBuffer HK_ALIGNED_VARIABLE( shapeBuffer, 16 );
	for ( i = 0; i < getNumChildShapes(); i++)
	{
		const hkShape* shape = getChildShape( getKeyAt( i ), shapeBuffer);
		if (shape != HK_NULL)
		{			
			int hasHitChild = 0;
			hasHitChild = shape->castRay(fromLocal, toLocal,results);
			if (hasHitChild)
			{
				results.m_primary = getKeyAt( i ).m_primary;	
				results.m_secondary = getKeyAt( i ).m_secondary;	
			}

			hasHit |= hasHitChild;
		}
	}
	return hasHit;
}


int hkShapeCollection::castAabb( const hkVector4& fromLocal, const hkVector4& toLocal, const hkRotation& localToWorld, const hkVector4& halfExtents, hkRayResults& results ) const
{
	int hasHit = 0;
	int i;
	hkShapeCollection::AllocBuffer HK_ALIGNED_VARIABLE( shapeBuffer, 16 );
	for (i=0;i< getNumChildShapes( ) ; i++)
	{
		const hkShape* shape = getChildShape( getKeyAt( i ), shapeBuffer);
		if (shape != HK_NULL)
		{
			hasHit |= shape->castAabb(fromLocal,toLocal,localToWorld,halfExtents,results);
		}
	}
	return hasHit;
}

/// gets the childshape and perform a raycast on it (potentially filter it)
int hkShapeCollection::childShapeCastRay(const hkShapeKey& key , const hkVector4& fromLocal,const hkVector4& toLocal,hkRayResults& results) const
{
	//client can add filtering here

	int hasHit = 0;

	hkShapeCollection::AllocBuffer HK_ALIGNED_VARIABLE( shapeBuffer, 16 );
	
	const hkShape* shape = getChildShape( key, shapeBuffer);

	if ( (shape!=HK_NULL) && (shape->castRay(fromLocal,toLocal,results)))
	{
		hasHit = 1;
		results.m_primary = key.m_primary;	
		results.m_secondary = key.m_secondary;	
	}

	return hasHit;
}


/*! \fn const hkShape* hkShapeCollection::getChildShape(const hkShapeKey& key, AllocBuffer buffer ) const;
* The AllocBuffer can be used by the implementor of this function to store the returned child shape, if necessary.
* 
*/

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
