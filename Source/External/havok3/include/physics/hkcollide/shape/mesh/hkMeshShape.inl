/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

int hkMeshShape::getNumSubparts() const
{
	return m_subparts.getSize();
}

hkMeshShape::Subpart& hkMeshShape::getSubpartAt( int i )
{
	HK_ASSERT2(0x3031b232,  i < m_subparts.getSize(), "You are trying to access a subpart which is not in the subpart array");
	return m_subparts[i];
}

const hkMeshShape::Subpart& hkMeshShape::getSubpartAt( int i ) const
{
	HK_ASSERT2(0x2bb0d984,  i < m_subparts.getSize(), "You are trying to access a subpart which is not in the subpart array");
	return m_subparts[i];
}


const hkMeshShape::Subpart& hkMeshShape::getSubPart( hkShapeKey key ) const
{
	int subPart = static_cast<int>(key >> ( 32 - m_numBitsForSubpartIndex ));
	return m_subparts[ subPart ];
}


hkInt32 hkMeshShape::getNumBitsForSubpartIndex() const
{
	return m_numBitsForSubpartIndex;
}

hkReal hkMeshShape::getRadius() const
{
	return m_radius;
}

void hkMeshShape::setRadius(hkReal r )
{
	m_radius = r;
}

inline const hkVector4&	hkMeshShape::getScaling() const
{
	return m_scaling;
}

inline const hkMeshMaterial* hkMeshShape::getMeshMaterial( hkShapeKey key ) const
{
	const hkUint32 triangleIndex = key & ( ~0U >> m_numBitsForSubpartIndex );
	const hkUint32 subPartIndex = key >> ( 32 - m_numBitsForSubpartIndex );
	// Grab a handle to the sub-part
	const Subpart& part = m_subparts[ subPartIndex ];

	if (part.m_materialIndexBase)
	{
		int materialId;
		HK_ASSERT2(0xad453fa2, part.m_materialIndexStridingType == MATERIAL_INDICES_INT8 || part.m_materialIndexStridingType == MATERIAL_INDICES_INT16, "Invalid hkMeshShape::SubPart::m_materialIndexStridingType.");
		if (part.m_materialIndexStridingType == MATERIAL_INDICES_INT8)
		{	
			materialId = *static_cast<const hkUint8 *>(hkAddByteOffsetConst( part.m_materialIndexBase, triangleIndex * part.m_materialIndexStriding ));
		}
		else
		{	
			materialId = *static_cast<const hkUint16*>(hkAddByteOffsetConst( part.m_materialIndexBase, triangleIndex * part.m_materialIndexStriding ));
		}
		HK_ASSERT2(0x26d359f1,  materialId < part.m_numMaterials, "Your mesh references a material which does not exist" );

		return hkAddByteOffsetConst<hkMeshMaterial>( part.m_materialBase, part.m_materialStriding * materialId );
	}
	else
	{
		return HK_NULL;
	}
}

inline hkMeshShape::Subpart::Subpart()
{
	// materials (default is fine)
	//m_materialIndexStridingType = MATERIAL_INDICES_INVALID;
	m_materialIndexStridingType = MATERIAL_INDICES_INT8;
	m_materialIndexStriding = 0;
	m_materialStriding = 0;
	m_numMaterials = 1;
	m_materialBase = HK_NULL;
	m_materialIndexBase = HK_NULL;

	// 'must set' values, defaults are error flags effectively for HK_ASSERTS in the cpp.
	#ifdef HK_DEBUG
		m_vertexBase = HK_NULL;
		m_vertexStriding = -1;
		m_numVertices = -1;
		m_indexBase = HK_NULL;
		m_stridingType = INDICES_INVALID;
		m_indexStriding = -1;
		m_numTriangles = -1;
	#endif
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
