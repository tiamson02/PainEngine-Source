/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_POINTTOPATH_CONSTRUCION_INFO_H
#define HK_DYNAMICS2_POINTTOPATH_CONSTRUCION_INFO_H


class hkParametricCurve;
#include <hkdynamics2/entity/hkRigidBody.h>

///The information needed to construct a point to path constraint.
class hkPointToPathConstructionInfo
{
   public:

		inline hkPointToPathConstructionInfo();
	   
		enum OrientationConstraintType
		{ 
			HK_DO_NOT_CONSTRAIN_ORIENTATION = 0, 
			HK_CONSTRAIN_ORIENTATION_ALLOW_SPIN = 2, 
			HK_CONSTRAIN_ORIENTATION_TO_PATH = 3
		}; 
		
	
		/// Setup the constraint info using data specified in World space
		inline void setInWorldSpace(hkRigidBody* bodyA, hkRigidBody* bodyB, const hkVector4& pivot, hkParametricCurve *path);
	
		/// Setup the constraint info using data specified in the two Body spaces
		inline void setInBodySpace(hkRigidBody* bodyA, hkRigidBody* bodyB, const hkVector4& pivotA, const hkVector4& pivotB, 
								   hkParametricCurve *path);
		
		/// Set friction magnitude along path
		inline void setMaxFrictionForce(hkReal fmag);

		/// Sets

		/// Set the transformation of the attached body's pivot, in the attached body's Local space.
		/// This is the "Constraint" space for the attached body.
		/// This both sets the pivot position, and pivot orientation in this space. See 
		/// setAttachedBodyPivotOrientation() for how the orientation may be used.
		/// The default is the Identity.
		inline void setAttachedBodyPivotTransform(const hkTransform& t);

		/// Set the orientation of the attached body's pivot, in the attched body's Local space.
		/// This is the "Constraint" space for the attached body.
		/// The does not affect the "linear"component of the constrint (keeping the pivot of the
		/// attached boy on the path), but affects the "angular" component of the constrint 
		/// (which aligns the attached body with the path, if required).
		/// If the OrientationType is set to HK_CONSTRAIN_ORIENTATION_ALLOW_SPIN or HK_CONSTRAIN_ORIENTATION_TO_PATH
		/// the the 'X' axis of the Pivot Transform will align itself "along" the path.
		/// If OrientationType is set to HK_CONSTRAIN_ORIENTATION_TO_PATH then, additionally,
		/// the 'Z' axis of the Pivot Transform will align itself with the "binormal" of the path.
		/// The default is the Identity.
		inline void setAttachedBodyPivotOrientation(const hkRotation& r);

		/// Set the "Constraint" space for the reference body, , in the attched body's Local space.
		/// This is the space in which the path is stored (ie. a "path Space to Local space" Transform.
		/// The default is the Identity ( implying the path should be specified in the reference bodies 
		/// Local space).
		inline void setReferenceBodyPathTransform(const hkTransform& t);

		/// Set orientation constraint type. This must be one of:
		/// HK_DO_NOT_CONSTRAIN_ORIENTATION : No angular constraints
		/// HK_CONSTRAIN_ORIENTATION_ALLOW_SPIN : Atttached body constrained so that 'X' axis of pivot Transform (constraint space) is aligned "along" the path.
		/// HK_CONSTRAIN_ORIENTATION_TO_PATH : Attached body completely "aligned" with path using path direction and binormal, with 
		/// 'X' axis of pivot Transform (constraint space) is aligned "along" the path and
		/// 'Z' axis of the Pivot Transform aligned with the "binormal" of the path.
		inline void setOrientationType(hkPointToPathConstructionInfo::OrientationConstraintType);


		OrientationConstraintType m_angularConstrainedDOF:8;
		hkParametricCurve* m_path;
		hkRigidBody* m_bodyA;
		hkRigidBody* m_bodyB;
		hkReal m_maxFrictionForce;

		hkTransform m_transform_OS_KS[2];
};

#include <hkdynamics2/constraint/bilateral/pointtopath/hkPointToPathConstructionInfo.inl>

#endif

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
