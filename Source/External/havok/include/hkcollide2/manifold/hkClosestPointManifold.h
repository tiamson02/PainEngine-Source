/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_CLOSEST_POINT_MANIFOLD_H
#define HK_COLLIDE2_CLOSEST_POINT_MANIFOLD_H


class hkCollidable;
struct hkCollisionResult;

typedef void* hkContactPointId;

class hkClosestPointManifold
{

	public:
		enum { HK_NUM_MAX_CONTACTS = 5 };

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_CDINFO, hkClosestPointManifold );

		class hkAgentContactPoint
		{
			public:
				hkAgentContactPoint()
				{
					m_contactPointId = 0;
				}

				hkAgentContactPoint( const hkVector4 &pointA, const hkVector4 &pointB, const hkVector4 &normal, hkReal distance ) :
					m_pointA( pointA ), m_pointB( pointB ), m_normal( normal ), m_distance( distance )
				{}

					//
					// members
					//
				hkVector4 m_pointA;			// in object space A
				hkVector4 m_pointB;			// in object space B

				hkVector4 m_normal;			// in world space
				float m_distance;

				hkContactPointId	m_contactPointId;
		};

		inline hkClosestPointManifold( );

		// for 2.0 bridge
		void addPoint( const hkCollidable& ca, const hkCollidable& cb, const hkVector4 &pointA, const hkVector4 &pointB, const hkVector4 &normal, hkReal distance, const hkCollisionEnvironment &env );
		void getPoints( const hkCollidable& ca, const hkCollidable& cb, hkCollisionResult& contactPoints, const hkCollisionEnvironment &env, hkReal dist );

		void cleanup( const hkCollisionEnvironment &env );

		void reduceTo4Points(const hkCollidable& ca, const hkCollidable& cb, const hkCollisionEnvironment& env);

		void makeConvex( const hkCollidable& ca, const hkCollidable& cb, const hkCollisionEnvironment& env );
		void removeLeastUsefulPoint( hkContactPointId freshPoint,  const hkCollidable& ca, const hkCollidable& cb, const hkCollisionEnvironment& env );

		hkAgentContactPoint m_contactPoints[HK_NUM_MAX_CONTACTS];

		int	m_numPoints;
};

hkClosestPointManifold::hkClosestPointManifold()
{
	m_numPoints = 0;
}

#endif // HK_COLLIDE2_CLOSEST_POINT_MANIFOLD_H

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
