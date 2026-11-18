/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_ACTION_H
#define HK_DYNAMICS2_ACTION_H


#include <hkmath2/hkMath.h>
#include <hkdynamics2/entity/hkEntityListener.h>
#include <hkdynamics2/common/hkStepInfo.h>

class hkEntity;
class hkSimulationIsland;
class hkStepInfo;
class hkWorld;

/// This is the base class from which user actions (or controllers) are derived. Actions 
/// are the interface between user controllable behavior of the physical simulation and the Havok core. 
class hkAction : public hkReferencedObject, protected hkEntityListener
{
	public:

		inline hkAction();

		/// The applyAction() method does the actual work of the action, and is called at every simulation step.
		virtual void applyAction( const hkStepInfo& stepInfo ) = 0;


			/// This method is used to maintain simulation islands.
			/// An action that requires that the enties it operates on are simulated together
			/// must return these entities in this function call. See the hkBinaryAction 
			/// implementation for an example.
		virtual void getEntities( hkArray<hkEntity*>& entitiesOut ) = 0;

			/// Returns the hkWorld that owns this action.
		inline hkWorld* getOwner();

			/// Get the user data for the action (initialized to 0).
		inline void* getUserData() const;

			/// Set the user data of the action.
		inline void setUserData( void* data );

	private:

		hkWorld* m_owner;
		hkSimulationIsland* m_island;
		void* m_userData;

	public:
		/// Gets the simulation island to which the action belongs.
		inline hkSimulationIsland* getSimulationIsland();

		/// Sets the simulation island.
		inline void setSimulationIsland(hkSimulationIsland* island);

		/// Sets the hkWorld that owns this action.
		inline void setOwner( hkWorld* owner );

			/// The action should never receive this callback, as the action keeps a reference to any entity it operates on. 
			/// The implementation asserts if called.
		virtual void entityDeletedCallback( hkEntity* entity );

};

#include <hkdynamics2/action/hkAction.inl>

#endif // HK_DYNAMICS2_ACTION_H

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
