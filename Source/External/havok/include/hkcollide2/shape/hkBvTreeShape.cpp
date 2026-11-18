/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkcollide2/hkCollide2.h>

#include <hkcollide2/shape/hkBvTreeShape.h>

int hkBvTreeShape::castAabb(const hkVector4& fromLocal,const hkVector4& toLocal, const hkRotation& localToWorld, const hkVector4& halfExtents2, hkRayResults& results) const
{
		
	// create an extruded obb that contains the box in both its start and end position
	int hasHit=0;

	hkVector4 fromBox,toBox;
	fromBox.setRotatedInverseDir(localToWorld,fromLocal);
	toBox.setRotatedInverseDir(localToWorld,toLocal);

	hkVector4 minCoord,maxCoord;
	minCoord.setMin4(fromBox,toBox);
	maxCoord.setMax4(fromBox,toBox);

	hkVector4	obbHalfExtents;
	obbHalfExtents.setSub4(maxCoord,minCoord);
	obbHalfExtents.mul4(0.5f);
	obbHalfExtents.addMul4(2.f,halfExtents2);

	hkVector4 center(fromLocal);
	center.add4(toLocal);
	center.mul4(0.5f);

	hkTransform obbTransform;
	obbTransform.setTranslation(center);
	obbTransform.setRotation(localToWorld);


	//later on add a long-cast aabb version, this is for a 'short' aabb cast
	//long diagonal casts are very inefficiently culled this way
	if (1)
	{
	
		hkReal tolerance = 0.f;
		hkInplaceArray<struct hkShapeCollection::hkShapeKey,64> hitList;

		queryObb( obbTransform , obbHalfExtents , tolerance , hitList );

		hkShapeCollection::AllocBuffer HK_ALIGNED_VARIABLE( shapeBuffer, 16 );
	
		for (int i=0;i<hitList.getSize();i++)
		{
			//hkprintf("num hits %d\n",hitList.getSize());
			const hkShapeCollection::hkShapeKey key = hitList[i];
			const hkShape* shape = getShapeCollection()->getChildShape( key, shapeBuffer);
			
			if (shape != HK_NULL)
			{
				hasHit |= shape->castAabb(fromLocal, toLocal,localToWorld,halfExtents2, results);
			}
		}
		
	}

	return hasHit;

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
