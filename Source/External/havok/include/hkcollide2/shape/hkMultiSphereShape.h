/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_MULTI_SPHERE_SHAPE_H
#define HK_COLLIDE2_MULTI_SPHERE_SHAPE_H

#include <hkcollide2/shape/hkShapeCollection.h>


///A compound shape made up of a number of spheres. This is useful as a fast approximation for complex surfaces,
/// as collision detection for spheres is very fast.
class hkMultiSphereShape : public hkShapeCollection
{
	public:
		enum { MAX_SPHERES = 8 };

			/// The w component of each vector3 is the sphere radius.
		hkMultiSphereShape(const hkVector4* spheres, int numSpheres);

			/// Gets this hkShape's primary type. For hkMultiSphereShapes, this is HK_SHAPE_MULTI_SPHERE.
		virtual hkShapeType getPrimaryType() const { return HK_SHAPE_MULTI_SPHERE; }

			/// Gets the AABB for a given transform and an extra tolerance.
 		virtual void getAabb( const hkTransform& localToWorld, hkReal tolerance, hkAabb& out ) const;

		///Returns the shape's hkVector4 array that defines its spheres.
		const hkVector4* getSpheres() const { return m_spheres; }
		const int	getNumSpheres() const { return m_numSpheres; }


		// hkShapeCollection interface implementation
		virtual int getNumChildShapes() const;

			// hkShapeCollection interface implementation
		virtual hkShapeCollection::hkShapeKey getKeyAt( int i ) const;

			// hkShapeCollection interface implementation
		virtual const hkShape* getChildShape(const hkShapeCollection::hkShapeKey& key, AllocBuffer& buffer ) const;


	private:

			/// Be careful to match size of inplace arrays in getAABB and hkMultiSphereTriangleAgent
		int		m_numSpheres;
		hkVector4 m_spheres[MAX_SPHERES];
};

#endif // HK_COLLIDE2_MULTI_SPHERE_SHAPE_H

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
