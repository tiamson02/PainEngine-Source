/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


#ifndef HK_MALLEABLE_CONSTRAINT_H
#define HK_MALLEABLE_CONSTRAINT_H

#include <hkdynamics/constraint/hkConstraintData.h>
#include <hkdynamics/world/hkWorld.h>

extern const hkClass hkMalleableConstraintDataClass;

/// This is a wrapper class around constraints intended to allow
/// the user to make a constraint softer.. (by scaling
/// the tau and damping values) used by the constraint solver.
/// Important: This malleable constraint only affects constraints, which do not allow for
/// setting tau and damping. So it does not affect angular limits, angular motors etc
class hkMalleableConstraintData : public hkConstraintData
{
	public:

		HK_DECLARE_REFLECTION();

			/// The constraint to be 'softened' should be passed in - its ref count will be incremented.
			///
			/// The malleable constraint essentially wraps another constraint.  A reference 
			/// is added to the constraint passed in (and removed upon destruction).
			/// NOTE: Instead of adding the original constraint to the world, you should 
			/// add this malleable constraint. Do not add both constraints.
		hkMalleableConstraintData(hkConstraintData* constraintData);	

			/// The reference to the original constraint is removed.
		~hkMalleableConstraintData();
	
			/// Set the solver tau value
		void setStrength(const hkReal tau);
		
			/// Get the tau value for this constraint
        hkReal getStrength() const;
		
			/// Checks consistency of constraint members			
		virtual hkBool isValid() const;
		
			// hkConstraintData interface implementation
		virtual int getType() const;
		
			// hkConstraintData interface implementation
		virtual void getConstraintInfo( hkConstraintInfo& infoOut ) const;
	
			/// Gets the wrapped constraint
		hkConstraintData* getWrappedConstraintData();
			/// Gets the wrapped constraint
		const hkConstraintData* getWrappedConstraintData() const;

	protected:

			/// The wrapped constraint (referenced, so protected)
		hkConstraintData* m_constraintData;

	public:

		hkReal m_strength;

			// Internal functions
		virtual void buildJacobian( const hkConstraintQueryIn &in, hkConstraintQueryOut &out );

			// hkConstraintData interface implementation
		virtual void getRuntimeInfo( hkBool wantRuntime, hkConstraintData::RuntimeInfo& infoOut ) const;

	public:

		hkMalleableConstraintData(hkFinishLoadedObjectFlag f) : hkConstraintData(f) {}
};
#endif 

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
