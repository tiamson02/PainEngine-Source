/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_ANGULARDASHPOT_H
#define HK_ANGULARDASHPOT_H

#include <hkdynamics2/action/hkBinaryAction.h>
#include <hkdynamics2/entity/hkRigidBody.h>


/// The information needed to construct an hkAngularDashpotAction.
// class definition of the angular dashpot is below

class hkAngularDashpotActionConstructionInfo
{
	public:

		inline hkAngularDashpotActionConstructionInfo();

		// Methods to initialize info wrt world and body space

		///Sets the bodies and rotation for the angular dashpot.
		inline void set(hkRigidBody* bodyA,hkRigidBody* bodyB,
						   const hkQuaternion& r);

		///Sets the strength for the angular dashpot.
		inline void setStrength(hkReal str);

		///Sets the damping for the angular dashpot.
		inline void setDamping(hkReal damp);

	public:

		hkRigidBody* m_bodyA;
		hkRigidBody* m_bodyB;

		hkReal	m_strength;
		hkReal	m_damping;
		hkQuaternion m_rotation;
};


/// An angular dashpot is an action that can be used to maintain the orientation of a
/// rigid body - either relative to another body or the world. It can be used as a cheap alternative to simple orientation
/// constraints, and is the rotational equivalent of an hkDashpotAction.

class hkAngularDashpotAction : public hkBinaryAction
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_ACTION);
		
			/// Creates an angular dashpot with the specified construction info.
		hkAngularDashpotAction(const hkAngularDashpotActionConstructionInfo& info);

			/// Automatically informs all dependants that this instance is about to be destroyed.
		virtual ~hkAngularDashpotAction();


			/// Applies the angular dashpot to the rigid body/bodies.
		void applyAction(const hkStepInfo& stepInfo);


			/// Returns the rotation of the angular dashpot.
		const hkQuaternion& getRotation() const;


			/// Sets the rotation of the angular dashpot.
		void setRotation(const hkQuaternion& r);


			/// Returns the strength of the dashpot.
		hkReal getStrength() const;


			/// Sets the strength of the dashpot.
		void setStrength(hkReal s);


			/// Returns the damping coefficient of the angular dashpot.
		hkReal getDamping() const;


			/// Sets the damping coefficient of the angular dashpot.
		void setDamping(hkReal s);


			/// Dynamically changes the first body to which the angular dashpot applies.
		void setBodyA(hkRigidBody* ra);


			/// Dynamically changes the second body to which the angular dashpot applies.
		void setBodyB(hkRigidBody* rb);


	/*************************************************************************************************
	**
	** THE REMAINDER OF THIS FILE IS FOR INTERNAL USE ONLY
	**
	*************************************************************************************************/

	private:

		hkQuaternion	m_rotation;
		hkReal		m_strength;
		hkReal		m_damping;
};

#include <hkutilities2/actions/angulardashpot/hkAngularDashpotAction.inl>

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
