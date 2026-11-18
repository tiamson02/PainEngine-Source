/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DASHPOT_H
#define HK_DASHPOT_H

#include <hkdynamics2/action/hkBinaryAction.h>
#include <hkdynamics2/entity/hkRigidBody.h>

/// The information needed to create an hkDashpotAction

class hkDashpotActionConstructionInfo
{
	public:

		hkDashpotActionConstructionInfo();

		~hkDashpotActionConstructionInfo();

		// Methods to initialize info wrt world and body space

		///Sets the dashpot information in world space.
		void setInWorldSpace(hkRigidBody* bodyA,hkRigidBody* bodyB,
						   const hkVector4& pivotA,const hkVector4& pivotB);

		///Sets the dashpot information in body space.
		void setInBodySpace(hkRigidBody* bodyA,hkRigidBody* bodyB,
						  const hkVector4& pivotA,const hkVector4& pivotB);

		///Sets the strength value for the dashpot.
		void setStrength(hkReal str);

		///Sets the damping value for the dashpot.
		void setDamping(hkReal damp);

	public:

		hkRigidBody* m_bodyA;
		hkRigidBody* m_bodyB;

		hkVector4 m_pivotA;
		hkVector4 m_pivotB;
		hkReal	m_strength;
		hkReal	m_damping;
};


/// A dashpot is a simple version of a constraint that works like a heavily damped spring. 
/// You can use a dashpot to attach two bodies together, or a body to a point in world space.
/// It is similar to a point-to-point constraint, although it is an hkAction rather
/// an hkConstraint.
class hkDashpotAction : public hkBinaryAction
{
	public:


		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_ACTION);

			/// Creates a dashpot with the specified construction info.
		hkDashpotAction(const hkDashpotActionConstructionInfo& info);

		virtual ~hkDashpotAction();

 		  /// Apply the hkDashpotAction to the two bodies.
		void applyAction(const hkStepInfo& stepInfo);


	 	  /// Dynamically changes the action's body A.
		inline void setBodyA(hkRigidBody* ra);

			///Dynamically changes the action's body B.
		inline void setBodyB(hkRigidBody* rb);

			/// Gets the point of attachment to body A, in body A space.
		inline const hkVector4& getPointA() const;

			///Gets the point of attachment to body B, in body B space
		inline const hkVector4& getPointB() const;

			/// Sets the point of attachment to body A, in body A space.
		inline void setPointA(const hkVector4& pa);

			///Sets the point of attachment to body B, in body B space
		inline void setPointB(const hkVector4& pb);

			/// Return positions in local space on bodies A and B to which the hkDashpotAction is attached
		inline void getPoints(hkVector4& pa, hkVector4& pb);

			/// Dynamically change points (in local space)  on bodies A and B to which the hkDashpotAction is attached
		inline void setPoints(const hkVector4& pa, const hkVector4& pb);

			/// Returns the strength of the hkDashpotAction
		inline hkReal getStrength() const;


			/// Dynamically sets the strength of the hkDashpotAction
		inline void setStrength(hkReal s);


			/// Returns the damping coefficient of the hkDashpotAction
		inline hkReal getDamping() const;


			/// Dynamically sets the damping coefficient of the hkDashpotAction
		inline void setDamping(hkReal d);

			/// Returns the impulse that is applied.
		inline const hkVector4 & getImpulse();


	/*************************************************************************************************
	**
	** THE REMAINDER OF THIS FILE IS FOR INTERNAL USE ONLY
	**
	*************************************************************************************************/

	protected:


		hkVector4	m_point[2];

		hkReal		m_strength;

		hkReal		m_damping;

		hkVector4	m_impulse;
};

#include <hkutilities2/actions/dashpot/hkDashpotAction.inl>


#endif // HAVOK_DASHPO


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
