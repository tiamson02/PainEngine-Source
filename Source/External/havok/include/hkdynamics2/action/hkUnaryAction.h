/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_UNARY_ACTION_H
#define HK_DYNAMICS2_UNARY_ACTION_H

#include <hkdynamics2/action/hkAction.h>

class hkRigidBody;

	/// You can use this as a base class for hkActions that operates on a single
	/// hkRigidBody. In addition to the hkAction interface, this class provides
	/// some useful basic functionality for actions, such as a callback that
	/// removes the action from the simulation if its hkRigidBody is removed.
class hkUnaryAction : public hkAction
{
	public:
			/// Constructor creates a hkUnaryAction that operates on the specified
			/// hkRigidBody.
		hkUnaryAction(hkRigidBody* body = HK_NULL);

			/// Constructor.
		~hkUnaryAction();

			/// hkAction interface implementation.
		virtual void getEntities( hkArray<hkEntity*>& entitiesOut );

			/// Removes the action from the hkWorld if the hkRigidBody is removed from
			/// the simulation.
		virtual void entityRemovedCallback(hkEntity* entity);

			/// Sets m_body, adds a reference to it and adds the action as a listener.
			/// NB: Only intended to be called pre-simulation i.e. before hkUnaryAction
			/// is added to an hkWorld.
		void setRigidBody(hkRigidBody* rigidBody);

			/// Gets m_body.
		inline hkRigidBody* getRigidBody();

	protected:

			/// The hkRigidBody.
		hkRigidBody* m_body;
};

inline hkRigidBody* hkUnaryAction::getRigidBody()
{
	return m_body;
}

#endif // HK_DYNAMICS2_UNARY_ACTION_H

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
