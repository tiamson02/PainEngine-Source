/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_BINARY_ACTION_H
#define HK_DYNAMICS2_BINARY_ACTION_H

#include <hkdynamics2/action/hkAction.h>

class hkRigidBody;

	/// You can use this as a base class for hkActions that operate on a pair of
	/// hkRigidBodies. In addition to the hkAction interface, this class provides
	/// some useful basic functionality for actions, such as a callback that removes
	/// the action from the simulation if either of its hkRigidBodies is removed.
class hkBinaryAction : public hkAction
{
	public:
			/// Constructor creates a new hkBinaryAction that operates on the specified
			/// hkRigidBodies.
		hkBinaryAction(hkRigidBody* rigidBodyA = HK_NULL, hkRigidBody* rigidBodyB = HK_NULL);
	
			/// hkAction interface implementation.
		virtual void getEntities( hkArray<hkEntity*>& entitiesOut );

			/// Removes the action from the hkWorld if one of its hkRigidBodies is removed.
		virtual void entityRemovedCallback(hkEntity* entity);

			/// Sets m_bodyA, adds a reference to it and adds the action as a listener.
			/// NB: Only intended to be called pre-simulation i.e. before hkBinaryAction
			/// is added to an hkWorld.
		void setRigidBodyA(hkRigidBody* rigidBodyA);

			/// Gets m_bodyA.
		inline hkRigidBody* getRigidBodyA();

			/// Sets m_bodyB, adds a reference to it and adds the action as a listener.
			/// NB: Only intended to be called pre-simulation i.e. before hkBinaryAction
			/// is added to an hkWorld.
		void setRigidBodyB(hkRigidBody* rigidBodyB);

			/// Gets m_bodyB.
		inline hkRigidBody* getRigidBodyB();

	protected:
			/// Destructor.
		virtual ~hkBinaryAction();

			/// The first hkRigidBody.
		hkRigidBody* m_bodyA;

			/// The second hkRigidBody.
		hkRigidBody* m_bodyB;
};

inline hkRigidBody* hkBinaryAction::getRigidBodyA()
{
	return m_bodyA;
}

inline hkRigidBody* hkBinaryAction::getRigidBodyB()
{
	return m_bodyB;
}

#endif // HK_DYNAMICS2_BINARY_ACTION_H

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
