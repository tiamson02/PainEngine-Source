/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkgeometry2/display/hkDisplayCone.h>
#include <hkmath2/linear/hkVector4Util.h>
// hkReal cone_angle, const hkVector4& startPos, const hkVector4& coneAaxis, const hkVector4& perpVector, int numSegments, unsigned color


hkDisplayCone::hkDisplayCone(const hkReal coneAngle, const hkReal coneHeight,
							 const int numSegments, const hkVector4& coneAxis, 
							 const hkVector4& startPos )
:	hkDisplayGeometry(HK_DISPLAY_CONE),
	m_startPos(startPos),
	m_coneAxis(coneAxis),
	m_numSegments(numSegments),
	m_coneAngle(coneAngle), 
	m_coneHeight(coneHeight)
{

}

hkDisplayCone::hkDisplayCone()
:	hkDisplayGeometry(HK_DISPLAY_CONE),
	m_numSegments(0),
	m_coneAngle(0.0f),
	m_coneHeight(0.0f)
{
	m_startPos.setZero4();
	m_coneAxis.setZero4();
}



void hkDisplayCone::setParameters(const hkReal coneAngle, 
								  const hkReal coneHeight,
								  const int numSegments, 
							      const hkVector4& coneAxis, 
							      const hkVector4& startPos)
{
	m_coneAngle = coneAngle;
	m_startPos = startPos;
	m_coneAxis = coneAxis;
	m_coneHeight = coneHeight;
	m_numSegments = numSegments;
}


void hkDisplayCone::generateConeVertices(hkArray<hkVector4>& conePoints)
{
	hkVector4 base;
	hkVector4 previous;
	hkVector4 current;

	// Better way -- member of class??
	conePoints.setSize(m_numSegments);

	hkVector4 perpVector;

	hkVector4Util::calculatePerpendicularVector(m_coneAxis, perpVector);
	perpVector.normalize3();

	hkQuaternion rotationFromNormal(perpVector, m_coneAngle);
	
	hkQuaternion rotationAboutAxis (m_coneAxis, (hkMath::HK_REAL_PI*2.0f) / m_numSegments);

	hkVector4 offsetDirection;
	
	offsetDirection.setRotatedDir(rotationFromNormal, m_coneAxis);

	for(int i = 0; i < m_numSegments; i++)
	{
		conePoints[i] = m_startPos;
		conePoints[i].addMul4(m_coneHeight , offsetDirection);
		offsetDirection.setRotatedDir(rotationAboutAxis, offsetDirection);
	}
}


void hkDisplayCone::buildGeometry()
{
	// build triangle hkGeometry

	m_geometry = new hkGeometry();

	hkArray<hkVector4> conePoints;

	generateConeVertices(conePoints);
	
	conePoints.pushBack(m_startPos);

	m_geometry->addVertices(&conePoints[0], m_numSegments + 1);
	
	hkArray<hkTriangleIndices> triangleIndices;
	triangleIndices.setSize(m_numSegments);

	int i;
	for(i = 0; i < m_numSegments - 1; i++)
	{
		triangleIndices[i].setTriangleIndices(m_numSegments, i + 1, i);
	}
	triangleIndices[i].setTriangleIndices(m_numSegments, 0, i);


	m_geometry->addTriangles(&triangleIndices[0], m_numSegments);
}


void hkDisplayCone::getWireframeGeometry(hkArray<hkVector4>& lines)
{
	hkArray<hkVector4> conePoints;

	generateConeVertices(conePoints);
	
	lines.setSize(4*m_numSegments);
	
	int k = 0;

	for (int j = 0; j < m_numSegments; )
	{
		lines[k] = m_startPos;
		lines[++k] = conePoints[j];
		lines[++k] = conePoints[j];
		int next_point_index = (++j) % m_numSegments;
		lines[++k] = conePoints[next_point_index];
		k++;
	}
}


hkVector4 hkDisplayCone::getPosition()
{
	return m_startPos;
}


hkVector4 hkDisplayCone::getAxis()
{
	return m_coneAxis;
}

hkReal hkDisplayCone::getHeight()
{
	return m_coneHeight;
}

hkReal hkDisplayCone::getAngle()
{
	return m_coneAngle;	
}

int hkDisplayCone::getNumSegments()
{
	return m_numSegments;
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
