/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_CONSTRAINT_H
#define HK_DYNAMICS2_CONSTRAINT_H

#include <hkdynamics2/entity/hkEntityListener.h>

#include <hkconstraintsolver2/solve/hkSolverResults.h>

class hkConstraintQueryIn;
class hkConstraintQueryOut;
class hkConstraintOwner;
struct hkConstraintInfo;

// Types for canned contraints
enum hkConstraintType 
{
	HK_CONSTRAINT_TYPE_BALLANDSOCKET = 0,
	HK_CONSTRAINT_TYPE_HINGE = 1,
	HK_CONSTRAINT_TYPE_LIMITEDHINGE = 2,
	HK_CONSTRAINT_TYPE_POINTTOPATH = 3,
	HK_CONSTRAINT_TYPE_POWEREDHINGE = 4,
	HK_CONSTRAINT_TYPE_POWEREDRAGDOLL = 5,
	HK_CONSTRAINT_TYPE_PRISMATIC = 6,
	HK_CONSTRAINT_TYPE_RAGDOLL = 7,
	HK_CONSTRAINT_TYPE_STIFFSPRING = 8,
	HK_CONSTRAINT_TYPE_WHEEL = 9,
	HK_CONSTRAINT_TYPE_GENERIC = 10,
	HK_CONSTRAINT_TYPE_CONTACT = 11,
	HK_CONSTRAINT_TYPE_BREAKABLE = 12,
	HK_CONSTRAINT_TYPE_MALLEABLE = 13
};

/// The base class for constraints.
class hkConstraint: public hkReferencedObject, protected hkEntityListener
{
	public:

        HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_CONSTRAINT);

			/// Default constructor. You cannot instanciate this class directly - see derived classes.
		inline hkConstraint();

			/// Destructor removes references from entities A and B if set.
		virtual ~hkConstraint();

			///Gets the first constrained entity.
		inline hkEntity* getEntityA() const;

			///Sets the first constrained entity.
		void setEntityA(hkEntity* entity );

			///Gets the second constrained entity.
		inline hkEntity* getEntityB() const;

			///Sets the second constrained entity.
		void setEntityB(hkEntity* entity );

			/// Provides access to solver results.
	    virtual hkSolverResults* getSolverResults() = 0;
		
			/// Provides number of solver results
		virtual int getNumSolverResults() const = 0;

			/// Get the user data for the constraint (initialized to 0).
		inline void* getUserData() const;

			/// Set the user data of the constraint.
		inline void setUserData( void* data );

			/// Checks member variables of constraint for consistency
			/// this method will be empty for some constraints
			/// This method is automatically called by the constraint constructors
		virtual bool isValid() const = 0;

	private:


		hkEntity* m_bodies[2];

		hkConstraintOwner* m_owner;
		
		void* m_userData;

	protected:	// exported data for the solver setup

		friend class hkConstraintSolverSetup;
		hkUint32    m_padding;

	public:

		//
		// Internal functions
		//

			/// Get the type of constraint
		virtual int getType() const = 0;

		inline hkConstraintOwner* getOwner();

			//Sets up the constraint for the solver.
		virtual void buildJacobian( const hkConstraintQueryIn &in, hkConstraintQueryOut &out ) = 0;

		virtual void getConstraintInfo( hkConstraintInfo& infoOut ) const = 0;


			// hkEntityListener interface implementation
		virtual void entityAddedCallback(hkEntity* entity);

			// hkEntityListener interface implementation
		virtual void entityRemovedCallback(hkEntity* entity);

			// The constraint should never recieve this callback, as the constraint keeps a reference to any entities it operates on. 
			// The implementation asserts if called.
		virtual void entityDeletedCallback( hkEntity* entity );

		inline void setOwner( hkConstraintOwner* world );

};

struct hkConstraintInfo
{
	int m_maxSizeOfJacobians;
	int m_sizeOfJacobians;
	int m_sizeOfSchemas;
	int m_numSolverResults;

	void clear( ) { m_maxSizeOfJacobians = 0; m_sizeOfJacobians = 0; m_sizeOfSchemas = 0; m_numSolverResults = 0; }
	void addHeader( ) { m_sizeOfSchemas += 24; }
	void add( int schemaSize, int jacSize) { m_sizeOfSchemas += schemaSize; m_sizeOfJacobians += jacSize; m_numSolverResults++; }
};



#include <hkdynamics2/constraint/hkConstraint.inl>

#endif // HK_DYNAMICS2_CONSTRAINT_H

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
