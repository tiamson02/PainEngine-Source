/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_MOUSE_SPRING_ACTION_H
#define HK_MOUSE_SPRING_ACTION_H

#include <hkdynamics2/action/hkUnaryAction.h>

class hkRigidBody;

	/// An action that allows users to drag a rigid body using the mouse. The mouse spring applies
	/// an impulse to the rigid body to move it around. You can see this action being
	/// used in the visual debugger and the Havok 2 demo framework.
class hkMouseSpringAction: public hkUnaryAction
{
	public:

			/// Creates a new hkMouseSpringAction for the specified rigid body.
			/// The mouse spring is created between the specified point in the rigid body's space and the specified
			/// mouse position in world space.
		hkMouseSpringAction(
			const hkVector4& positionInRbLocal, const hkVector4& mousePositionInWorld,
			const hkReal springDamping, const hkReal springElasticity,
			const hkReal objectDamping, hkRigidBody* rb );

			///	Sets the mouse position in world space
		void setMousePosition( const hkVector4& mousePositionInWorld );

			///	Sets the maximum relative force applied to the body
		void setMaxRelativeForce(hkReal newMax);

	public:

			/// Mouse spring end-point in the rigid body's local space
		hkVector4 m_positionInRbLocal;

			/// Mouse position in world space
		hkVector4 m_mousePositionInWorld;

			/// Damping for the mouse spring
		hkReal	m_springDamping;

			/// Elasticity for the mouse spring
		hkReal	m_springElasticity;

			/// Allows you to clip the impulse applied to the body (this gets multiplied by the mass of the object)
		hkReal  m_maxRelativeForce;

			/// Damping applied to the linear and angular velocities of the picked body
		hkReal	m_objectDamping;

		void applyAction( const hkStepInfo& stepInfo );
};

#endif // HK_MOUSE_SPRING_H


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
