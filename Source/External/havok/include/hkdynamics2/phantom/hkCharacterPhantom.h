/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef CHARACTER_PHANTOM
#define CHARACTER_PHANTOM

#include <hkdynamics2/phantom/hkAabbPhantom.h>
#include <hkdynamics2/phantom/hkPhantomConstructionInfo.h>
#include <hkcollide2/contact/hkSimpleClosestPointsDetails.h>
//#include <hkmath2/basetypes/hkMotionState.h>

struct hkCollisionEnvironment;
class hkCollisionDispatcher;
class hkCollidable;


struct hkCharacterPhantomConstructionInfo : public hkPhantomConstructionInfo
{
	hkCharacterPhantomConstructionInfo()
		:	m_shape(HK_NULL),
			m_transform( hkTransform::getIdentity() )
	{
	}
	hkShape* m_shape;
	hkTransform m_transform;
};


	/// This class represents a phantom with a shape.
	/// This phantom has no physical presents in the simulation, however when calling updateCollisions
	/// it reports a list of shortest distances (hkSimpleClosestPointsDetails) to the neighbouring objects
class hkCharacterPhantom : public hkAabbPhantom 
{
	public:

			///
		hkCharacterPhantom( const hkCharacterPhantomConstructionInfo& info );

			/// 
		~hkCharacterPhantom();

			/// This must be called in order to update the collision points between the collidable and the objects
			/// overlapping the collidable's aabb
			/// This function returns an index into the closestPointDetails array (the closest point)
			//  or -1 if there is no collision
			/// use getCollisionDetails()[index] to get information like closestDistance and hitNormal
		int updateCollisions( hkCollisionEnvironment& env );

			/// Get the collision details which were calculated in the last call to update collisions.
			/// Note that this array can change in size when the phantom's aabb is updated.
		inline const hkArray<hkSimpleClosestPointsDetails*>& getCollisionDetails() const;

			/// Read access to the transform
		inline const hkTransform& getTransform() const;

			/// Read access to the transform
		inline void setTransform( const hkTransform& transform);

			/// A faster way to move the phantom, setting the translation only
		inline void setTranslation( const hkVector4& translation );

			/// Set the rotation from a quaternion
		inline void setRotation( const hkQuaternion& rotation );



	public:

		virtual void addOverlappingCollidable( hkCollidable& collidable );

		virtual void removeOverlappingCollidable( hkCollidable& collidable );

	private:
		
		hkArray<hkSimpleClosestPointsDetails*> m_collisionDetails;

		hkMotionState m_motionState;

		unsigned int	m_filterInfo;
};

#include <hkdynamics2/phantom/hkCharacterPhantom.inl>


#endif //CHARACTER_PHANTOM


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
