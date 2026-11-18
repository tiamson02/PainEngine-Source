/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkdynamics/hkDynamics.h>

#include <hkutilities/collide/filter/constrainedsystem/hkConstrainedSystemFilter.h>

#include <hkdynamics/entity/hkRigidBody.h>
#include <hkdynamics/constraint/hkConstraintInstance.h>
#include <hkdynamics/constraint/hkConstraintData.h>
#include <hkbase/class/hkTypeInfo.h>

HK_REFLECTION_DEFINE_VIRTUAL(hkConstrainedSystemFilter);

hkConstrainedSystemFilter::hkConstrainedSystemFilter (const hkCollisionFilter* otherFilter) : m_otherFilter (otherFilter) 
{ 
	if (m_otherFilter) 
	{
		m_otherFilter->addReference();
	}
}

hkConstrainedSystemFilter::~hkConstrainedSystemFilter()
{
	if (m_otherFilter) 
	{
		m_otherFilter->removeReference();
	}
}

hkBool hkConstrainedSystemFilter::isCollisionEnabled( const hkCollidable& a, const hkCollidable& b ) const
{
	hkRigidBody* rigidBodyA = hkGetRigidBody(&a);
	hkRigidBody* rigidBodyB = hkGetRigidBody(&b);

	if (m_otherFilter && !m_otherFilter->isCollisionEnabled(a,b))
	{
		return false;
	}

	if (! rigidBodyA || ! rigidBodyB)
	{
		return true;
	}

	// Look for the rigid body (thisRigidBody) with fewer constraints
	// (that way the loop below is faster)
	hkRigidBody* thisRigidBody;
	hkRigidBody* otherRigidBody;
	{
		if (rigidBodyA->getNumConstraints() > rigidBodyB->getNumConstraints())
		{
			thisRigidBody = rigidBodyB;
			otherRigidBody = rigidBodyA;
		}
		else
		{
			thisRigidBody = rigidBodyA;
			otherRigidBody = rigidBodyB;
		}
	}

	const int numConstraints = thisRigidBody->getNumConstraints();

	for (int c = 0; c < numConstraints; c++)
	{
		const hkConstraintInstance* constraint = thisRigidBody->getConstraint(c);

		if ((constraint) &&
			(constraint->getData()->getType() != hkConstraintData::CONSTRAINT_TYPE_CONTACT) && 
			(
				(constraint->getEntityA()==otherRigidBody)  || 
				(constraint->getEntityB()==otherRigidBody) 
			))
		{
			return false;
		}
	}

	return true;
}

hkBool hkConstrainedSystemFilter::isCollisionEnabled( const hkCollisionInput& input, const hkCdBody& a, const hkCdBody& b, const hkShapeCollection& bCollection, hkShapeKey bKey  ) const  
{	
	return !m_otherFilter || m_otherFilter->isCollisionEnabled (input, a, b, bCollection, bKey);
}

hkBool hkConstrainedSystemFilter::isCollisionEnabled( const hkShapeRayCastInput& aInput, const hkShapeCollection& bCollection, hkShapeKey bKey ) const  
{	
	return !m_otherFilter || m_otherFilter->isCollisionEnabled (aInput, bCollection, bKey);
}

hkBool hkConstrainedSystemFilter::isCollisionEnabled( const hkWorldRayCastInput& a, const hkCollidable& collidableB ) const  
{	
	return !m_otherFilter || m_otherFilter->isCollisionEnabled (a, collidableB);
}

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
