/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_SKELETON_H
#define HK_SKELETON_H

#include <hkmath2/hkMath.h>
#include <hkutilities2/ragdoll/hkHierarchy.h>

/// A simple bone structure to store position and rotation 
struct hkBone
{
	hkVector4		m_position;
	hkQuaternion	m_rotation;
};


/// This simple skeleton class stores an array of bone transforms (pose) and a hierarchy.
/// Its main function is to transform between a pose in local space and world space.
/// Local space bone transforms are generally used within an animation system, while the
/// world transforms are required by the physics engine.
class hkSkeleton
{
	public:

		/// Construct a skeleton from a hierarchy and bone array.
		/// Pose bone array size should match hierarchy.
		hkSkeleton( const hkHierarchy* hierarchy, const hkArray<hkBone>& pose );

		~hkSkeleton();

		/// Get the skeleton parent hierarchy
		inline const hkHierarchy*	getHierarchy() const;

		/// Set transform of bone at specified index
		inline void				setBone( int i, const hkBone& bone );

		/// Get bone at specified index
		inline const hkBone&	getBone( int i ) const;
				
		/// Set pose to new bone array
		/// New pose must be the same size as original
		inline void						setPose( const hkArray<hkBone>& pose );

		/// Get current pose (bone transforms in local space)
		inline const hkArray<hkBone>&	getPose() const;

		/// Get current pose (bone transforms in local space)
		/// Warning changing pose size is not permitted.
		/// Pose bone array size should match hierarchy.
		inline hkArray<hkBone>&			getPose();

		/// Set current pose from world space bone transforms.
		/// Uses hierarchy to calculate local transforms.
		void	setPoseInWorld( const hkArray<hkBone>& poseInWorld );

		/// Get current pose in world space.
		/// Uses hierarchy to convert stored local space bone transforms to world space.
		void	getPoseInWorld( hkArray<hkBone>& poseInWorld ) const;

	private:

		const hkHierarchy*	m_hierarchy;
		hkArray<hkBone>		m_pose;

};

#include <hkutilities2/ragdoll/hkSkeleton.inl>

#endif //HK_SKELETON_H
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
