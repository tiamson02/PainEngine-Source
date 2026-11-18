/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_UTILITIES2_LAZY_ADD_TO_WORLD_H
#define HK_UTILITIES2_LAZY_ADD_TO_WORLD_H

#include <hkbase/hkBase.h>

class hkWorld;
class hkEntity;
class hkAction;
class hkConstraint;

	/// A class that may prove useful if using a complex toolchain that does not
	/// guarantee complete construction of Havok objects e.g. hkRigidBody with an 
	/// hkShape left as HK_NULL. It maintains hkArrays of pointers to objects that
	/// can be added to a hkWorld using commit functions. It Asserts if any incomplete
	/// objects are committed.
	/// NB: This class is only intended for use at setup time, before simulation
	/// begins.
class hkLazyAddToWorld : public hkReferencedObject
{
	public:

			//
			// Public functions.
			//

			/// Constructor takes a pointer to an hkWorld that objects will (eventually)
			/// be added to.
		hkLazyAddToWorld(hkWorld* world);

			/// Destructor Asserts if hkArrays are not all empty i.e. all objects committed
			/// to m_world.
		~hkLazyAddToWorld();


			//
			// Functions to commit objects to m_world.
			//
		
			/// Add all the elements of all the hkArrays to m_world. Returns the 
			/// number of objects committed or -1 for failure.
		int commitAll();

			/// Commit all remaining hkEntities. Return the number of hkEntities
			/// committed or -1 for failure.
		int commitAllEntity();

			/// Commit all remaining hkActions. Return the number of hkActions
			/// committed or -1 for failure.
		int commitAllAction();

			/// Commit all remaining hkConstraints. Return the number of hkConstraints
			/// committed or -1 for failure.
		int commitAllConstraint();

			//
			// Functions to add pointers to the commit hkArrays.
			//

			/// Add an hkEntity to the hkArray of hkEntites to be committed. Return 
			/// the size of m_entities after adding or -1 for failure.
		int addEntity(hkEntity* entity);

			/// Add an hkAction to the hkArray of hkActions to be committed. Return 
			/// the size of m_actions after adding or -1 for failure.
		int addAction(hkAction* action);

			/// Add an hkConstraint to the hkArray of hkConstraints to be committed. Return 
			/// the size of m_constraints after adding or -1 for failure.
		int addConstraint( hkConstraint* constraint );

			//
			// Functions to ensure objects are fully constructed (before committing).
			//

			/// Return true if the hkEntity is fully constructed.
		bool isValid(hkEntity* entity);

			/// Return true if the hkAction is fully constructed.
		bool isValid(hkAction* action);

			/// Return true if the hkConstraint is fully constructed.
		bool isValid(hkConstraint* constraint);

	public:
			//
			// Public data.
			//

			/// The hkWorld to which objects will be committed.
		hkWorld* m_world;

	private:	
			//
			// Private data.
			//

			/// hkArray holding pointers to hkEntities waiting to be committed.
		hkArray<hkEntity*> m_entities;

			/// hkArray holding pointers to hkActions waiting to be committed.
		hkArray<hkAction*> m_actions;

			/// hkArray holding pointers to hkConstraints waiting to be committed.
		hkArray<hkConstraint*> m_constraints;
};

#endif // HK_UTILITIES2_LAZY_ADD_TO_WORLD_H

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
