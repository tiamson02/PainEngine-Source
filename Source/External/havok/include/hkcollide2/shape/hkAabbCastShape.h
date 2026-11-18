/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_AABBCAST_SHAPE__H
#define HK_COLLIDE2_AABBCAST_SHAPE__H

#include <hkcollide2/shape/hkConvexShape.h>

	/// A shape that allows you to cast an aabb
class hkAabbCastShape : public hkShape
{
			
	public:

			/// Creates a new hkAabbCastShape representing an aabb cast from 'fromWorld' to 'toWorld'. The aabb size is specified by halfExtents.
		hkAabbCastShape( const hkVector4&	fromWorld, const hkVector4& toWorld, const hkVector4&	halfExtents );

			// hkShape implementation.
 		void getAabb( const hkTransform& localToWorld, hkReal tolerance, hkAabb& out  ) const;

			///Gets this hkShape's secondary type. 
		virtual hkShapeType getPrimaryType() const { return HK_SHAPE_AABBCAST; }

			///Gets this hkShape's secondary type. 
		virtual hkShapeType getSecondaryType() const { return HK_SHAPE_AABBCAST; }
			
			///Set a flag to ignore Penetration cases during an aabb cast
		HK_FORCE_INLINE void	setIgnorePenetration( bool ignorePenetration);
			
			///Get the status flag which tells wether or not to ignore Penetration cases during an aabb cast
		HK_FORCE_INLINE bool	getIgnorePenetration() const;
			
			///Get fromWorld returns the starting point of the aabb cast. This is the center of the aabb.
		HK_FORCE_INLINE const hkVector4&	getFromWorld() const;
			
			///Get toWorld returns the ending point of the aabb cast. This is the center of the aabb.
		HK_FORCE_INLINE const hkVector4&	getToWorld() const;
	
			///Set fromWorld sets the starting point of the aabb cast. This is the center of the aabb.
		HK_FORCE_INLINE void	setFromWorld( const hkVector4& fromWorld );
		
			///Set toWorld sets the end point of the aabb cast. This is the center of the aabb.
		HK_FORCE_INLINE void	setToWorld( const hkVector4& toWorld );

			///Get the halfextents of the aabb.
		HK_FORCE_INLINE const hkVector4&	getHalfExtents() const;

		HK_FORCE_INLINE void setHalfExtents(const hkVector4& extents);

			///Set the size of the aabb.
		HK_FORCE_INLINE void	setAabb(const hkAabb& aabb);

			///Set the size of the aabb.
		HK_FORCE_INLINE hkShape::hkRayResults&	getRayResults() const;

	private:
	

		hkVector4	m_fromWorld;

		hkVector4	m_toWorld;

		hkVector4	m_halfExtents;

		hkAabb		m_aabb;

		bool		m_ignorePenetration;

		mutable hkShape::hkRayResults	m_rayResults;

};

#include <hkcollide2/shape/hkAabbCastShape.inl>

#endif // HK_COLLIDE2_AABBCAST_SHAPE__H

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
