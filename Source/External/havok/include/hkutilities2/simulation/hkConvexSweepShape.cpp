/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkcollide2/hkcollide2.h>
#include <hkutilities2/simulation/hkConvexSweepShape.h>

#include <hkutilities2/visualdebugger/viewer/hkShapeDisplayBuilder.h>

static bool displayFunctionRegistered = false;

void HK_CALL buildConvexSweepShapeDisplay(	const hkShape* shape,
											const hkTransform& transform,
											hkArray<hkDisplayGeometry*>& displayGeometries,
											hkShapeDisplayBuilder* builder )

{
	const hkConvexSweepShape* convexSweepShape = static_cast<const hkConvexSweepShape*>(shape);
	builder->buildShapeDisplay(convexSweepShape->getChildShape(), transform, displayGeometries );
}



hkConvexSweepShape::hkConvexSweepShape( const hkConvexShape* childShape )
{
	m_childShape = childShape;
	m_displacementLocal.setZero4();
	childShape->addReference();
	m_wantReset = false;

	if ( !displayFunctionRegistered )
	{
		displayFunctionRegistered = true;
		hkUserShapeDisplayBuilder::getInstance().registerUserShapeDisplayBuilder( buildConvexSweepShapeDisplay, HK_SHAPE_USER7 );
	}
}

hkConvexSweepShape::~hkConvexSweepShape()
{
	m_childShape->removeReference();
}

void hkConvexSweepShape::getSupportingVertex( const hkVector4& direction, hkVector4& supportingVertexOut, int& vertexIdCache ) const
{
	m_childShape->getSupportingVertex( direction, supportingVertexOut, vertexIdCache );
	if ( m_displacementLocal.dot3( direction ) > 0 )
	{
		supportingVertexOut.add4( m_displacementLocal );
	}

}

hkShapeType hkConvexSweepShape::getPrimaryType() const 
{ 
	return HK_SHAPE_USER7; 
}

void hkConvexSweepShape::getAabb( const hkTransform& localToWorld, hkReal tolerance, hkAabb& out ) const
{
	m_childShape->getAabb( localToWorld, tolerance, out );
	// could set this externally if it is a root shape
	hkVector4 worldDisp; worldDisp._setRotatedDir(localToWorld.getRotation(), m_displacementLocal );
	{
		hkVector4 maxExt; maxExt.setAdd4( out.m_max, worldDisp );
		out.m_max.setMax4(maxExt, out.m_max);
	}
	{
		hkVector4 minExt; minExt.setAdd4( out.m_min, worldDisp );
		out.m_min.setMin4(minExt, out.m_min);
	}
}


void hkConvexSweepShape::getPlaneEquations( const hkRotation& localToWorld, const hkVector4& halfExtents, hkArray<hkVector4>& planeEquations ) const
{
	m_childShape->getPlaneEquations(  localToWorld, halfExtents, planeEquations );
}


int hkConvexSweepShape::castRay( const hkVector4& fromLocal, const hkVector4& toLocal, hkRayResults& results) const
{
	return m_childShape->castRay( fromLocal, toLocal, results);
}

int hkConvexSweepShape::castAabb(const hkVector4& fromLocal,const hkVector4& toLocal, const hkRotation& localToWorld, const hkVector4& halfExtents, hkRayResults& results) const
{
	return m_childShape->castAabb( fromLocal, toLocal, localToWorld, halfExtents, results);

}

void hkConvexSweepShape::getCollisionSpheres( hkArray<hkSphere>& collisionSpheres ) const
{
	m_childShape->getCollisionSpheres( collisionSpheres );
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
