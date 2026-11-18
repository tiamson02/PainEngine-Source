/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_CONSTRAINT_DATA_H
#define HK_DYNAMICS2_CONSTRAINT_DATA_H

#include <hkconstraintsolver/solve/hkSolverResults.h>
#include <hkconstraintsolver/solve/hkSolve.h>

class hkConstraintQueryIn;
class hkConstraintQueryOut;
class hkConstraintOwner;
class hkConstraintInstance;
struct hkConstraintInfo;

extern const hkClass hkConstraintDataClass;
extern const hkClass hkConstraintInfoClass;

	// All runtime information of the constraints must start with an array of solverresults
typedef void hkConstraintRuntime;

class hkConstraintData;

/// The base class for constraints.
/// This class holds all information about the data of a constraint. 
/// As this data is constant over the lifetime of a constraint, 
class hkConstraintData : public hkReferencedObject
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_CONSTRAINT);
		HK_DECLARE_REFLECTION();

            /// Types for canned contraints
        enum ConstraintType
        {
            CONSTRAINT_TYPE_BALLANDSOCKET = 0,
            CONSTRAINT_TYPE_HINGE = 1,
            CONSTRAINT_TYPE_LIMITEDHINGE = 2,
            CONSTRAINT_TYPE_POINTTOPATH = 3,
            CONSTRAINT_TYPE_POWEREDHINGE = 4,
            CONSTRAINT_TYPE_POWEREDRAGDOLL = 5,
            CONSTRAINT_TYPE_PRISMATIC = 6,
            CONSTRAINT_TYPE_RAGDOLL = 7,
            CONSTRAINT_TYPE_STIFFSPRING = 8,
            CONSTRAINT_TYPE_WHEEL = 9,
            CONSTRAINT_TYPE_GENERIC = 10,
            CONSTRAINT_TYPE_CONTACT = 11,
            CONSTRAINT_TYPE_BREAKABLE = 12,
            CONSTRAINT_TYPE_MALLEABLE = 13,
            CONSTRAINT_TYPE_POINTTOPLANE = 14,

	        CONSTRAINT_TYPE_PULLEY = 15,
	        CONSTRAINT_TYPE_RELATIVE_ORIENTATION = 16,
			CONSTRAINT_TYPE_HINGE_LIMITS = 18,
			CONSTRAINT_TYPE_RAGDOLL_LIMITS = 19,
        
	        // Constraint Chains
	        BEGIN_CONSTRAINT_CHAIN_TYPES = 100,        
	        CONSTRAINT_TYPE_STIFF_SPRING_CHAIN = 100,
	        CONSTRAINT_TYPE_BALL_SOCKET_CHAIN = 101,
	        CONSTRAINT_TYPE_POWERED_CHAIN = 102
        };

			/// Default constructor. You cannot instanciate this class directly - see derived classes.
			/// This constructor just does some debug checks for alignment.
		inline hkConstraintData();

			/// Destructor.
		inline virtual ~hkConstraintData();

			/// Get the user data for the constraint (initialized to 0).
		inline hkUint32 getUserData() const;

			/// Set the user data of the constraint.
		inline void setUserData( hkUint32 data );

			/// Checks member variables of constraint for consistency
			/// this method will be empty for some constraints
			/// This method is automatically called by the constraint constructors
		virtual hkBool isValid() const = 0;

			/// Get the type of constraint
		virtual int getType() const = 0;

		//
		//	Interface for solver runtime data 
		//

			/// Return runtime information
		struct RuntimeInfo
		{
			int m_sizeOfExternalRuntime;
			int m_numSolverResults;
		};

			/// Get information about the size of the runtime data needed
			/// Notes:
			/// - if wantRuntime is set, you ask for a runtime information
			/// - Some constraint have their private runtime, so no need to externally allocate runtime info
			/// - Some constraint might not want to use any runtime information
			/// - All Runtimes of the constraints should start with an array of hkSolverResults
		virtual void getRuntimeInfo( hkBool wantRuntime, RuntimeInfo& infoOut ) const = 0;

			/// Gives you access to the solver results in a generic way, use getRuntimeInfo to get the number of solver results
		virtual hkSolverResults* getSolverResults( hkConstraintRuntime* runtime );

			/// Initialize the runtime data. The default implementation simply zeros all values
		virtual void addInstance( hkConstraintInstance* constraint, hkConstraintRuntime* runtime, int sizeOfRuntime ) const;

			/// Drop an instance reference to this data
		inline void removeInstance( hkConstraintInstance* constraint, hkConstraintRuntime* runtime, int sizeOfRuntime ) const;
	public:
			
			/// For any use you want. Not used by the Havok system.
		hkUint32 m_userData;


	protected:	// exported data for the solver setup

		friend class hkConstraintSolverSetup;

	public:

		//
		// Internal functions
		//

			//Sets up the constraint for the solver.
		virtual void buildJacobian( const hkConstraintQueryIn &in, hkConstraintQueryOut &out ) = 0;

		virtual void getConstraintInfo( hkConstraintInfo& infoOut ) const = 0;

	public:
	
		// Serialization ctor
		hkConstraintData(hkFinishLoadedObjectFlag f) {}

};

//! Keep the members of this synced with the generic constraint cinfo
//! as it has to save all these members.
struct hkConstraintInfo
{
	// We need this reflected as it is serialized in the generic constraint.
	HK_DECLARE_REFLECTION();

		// This vairable only has a meaning in an island (it is equal to sizeOfJacobians for constraints).
		// It realates to splitting buffers on playstation
	int m_maxSizeOfJacobians;
	int m_sizeOfJacobians;
	int m_sizeOfSchemas;
	int m_numSolverResults;

	void clear( ) { m_maxSizeOfJacobians = 0; m_sizeOfJacobians = 0; m_sizeOfSchemas = 0; m_numSolverResults = 0; }
	void addHeader( ) { m_sizeOfSchemas += HK_SIZE_OF_JACOBIAN_HEADER_SCHEMA; }
	void add( int schemaSize, int jacSize) { m_sizeOfSchemas += schemaSize; m_sizeOfJacobians += jacSize; m_numSolverResults++; }
};



#include <hkdynamics/constraint/hkConstraintData.inl>

#endif // HK_DYNAMICS2_CONSTRAINT_DATA_H

/*
* Havok SDK - PUBLIC RELEASE, BUILD(#20060114)
*
* Confidential Information of Havok.  (C) Copyright 1999-2006 
* Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
* Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
* rights, and intellectual property rights in the Havok software remain in
* Havok and/or its suppliers.
*
* Use of this software for evaluation purposes is subject to and indicates 
* acceptance of the End User licence Agreement for this product. A copy of 
* the license is included with this software and is also available from salesteam@havok.com.
*
*/
