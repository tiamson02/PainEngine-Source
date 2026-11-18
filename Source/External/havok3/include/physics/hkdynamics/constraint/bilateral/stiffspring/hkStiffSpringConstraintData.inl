/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


inline void hkStiffSpringConstraintData::setInWorldSpace(const hkTransform& bodyATransform,
													 const hkTransform& bodyBTransform,
													 const hkVector4& pivotAW,
													 const hkVector4& pivotBW) 
{
	m_pivotInA.setTransformedInversePos(bodyATransform,pivotAW);
	m_pivotInB.setTransformedInversePos(bodyBTransform,pivotBW);

	hkVector4 sepDist;
	sepDist.setSub4( pivotAW, pivotBW );
	m_springLength = sepDist.length3();
}


inline void hkStiffSpringConstraintData::setInBodySpace( const hkTransform& bodyATransform,
													 const hkTransform& bodyBTransform,
													 const hkVector4& pivotA,const hkVector4& pivotB )
{
	m_pivotInA = pivotA;
	m_pivotInB = pivotB;	

	hkVector4 pivotAW, pivotBW; 
	pivotAW.setTransformedPos(bodyATransform,pivotA); 
	pivotBW.setTransformedPos(bodyBTransform,pivotB); 

	hkVector4 sepDist; sepDist.setSub4( pivotAW, pivotBW ); 

	m_springLength = sepDist.length3();
}


inline hkReal hkStiffSpringConstraintData::getSpringLength()
{
	return m_springLength;
}


inline void hkStiffSpringConstraintData::setSpringLength(hkReal length)
{
	m_springLength = length;
}


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
