/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


#include <hkutilities/constraint/hkConstraintUtils.h>
#include <hkdynamics/entity/hkRigidBody.h>
#include <hkdynamics/constraint/bilateral/ballandsocket/hkBallAndSocketConstraintData.h>
#include <hkdynamics/constraint/bilateral/hinge/hkHingeConstraintData.h>
#include <hkdynamics/constraint/bilateral/limitedhinge/hkLimitedHingeConstraintData.h>
#include <hkdynamics/constraint/bilateral/poweredhinge/hkPoweredHingeConstraintData.h>

#include <hkdynamics/constraint/chain/ballsocket/hkBallSocketChainData.h>
#include <hkdynamics/constraint/chain/powered/hkPoweredChainData.h>

#include <hkinternal/dynamics/constraint/bilateral/poweredragdoll/hkPoweredRagdollConstraintData.h>
#include <hkinternal/dynamics/constraint/bilateral/ragdoll/hkRagdollConstraintData.h>

#include <hkdynamics/constraint/hkConstraintInstance.h>
#include <hkdynamics/constraint/motor/hkConstraintMotor.h>
#include <hkdynamics/constraint/chain/hkConstraintChainInstance.h>
#include <hkdynamics/constraint/chain/ballsocket/hkBallSocketChainData.h>

#include <hkdynamics/constraint/bilateral/ballandsocket/hkBallAndSocketConstraintData.h>
//#include <hkdynamics/constraint/motor/position/hkPositionConstraintMotor.h>

#include <hkdynamics/world/hkSimulationIsland.h>

#include <hkinternal/dynamics/constraint/chain/ragdolllimits/hkRagdollLimitsData.h>
#include <hkdynamics/constraint/chain/hingelimits/hkHingeLimitsData.h>

hkConstraintInstance* hkConstraintUtils::convertToPowered (const hkConstraintInstance* originalConstraint, hkConstraintMotor* constraintMotor)
{
	hkConstraintInstance* newConstraint = HK_NULL;
	hkConstraintData* constraintData = originalConstraint->getData();
	
	hkEntity* entityA = originalConstraint->getEntityA();
	hkEntity* entityB = originalConstraint->getEntityB();

	switch (constraintData->getType())
	{
		case hkConstraintData::CONSTRAINT_TYPE_LIMITEDHINGE:
			{
				hkLimitedHingeConstraintData* oldConstraintData = static_cast<hkLimitedHingeConstraintData*> (constraintData);

				hkPoweredHingeConstraintData* newConstraintData = new hkPoweredHingeConstraintData();

				//*newConstraintData = *oldConstraintData;
				newConstraintData->m_basisA = oldConstraintData->m_basisA;
				newConstraintData->m_basisB = oldConstraintData->m_basisB;
				newConstraintData->m_minAngle = oldConstraintData->m_minAngle;
				newConstraintData->m_maxAngle = oldConstraintData->m_maxAngle;
				newConstraintData->m_maxFrictionTorque = oldConstraintData->m_maxFrictionTorque;

				newConstraintData->setMotor( constraintMotor );
	
				newConstraintData->m_motorActive = false;
				newConstraintData->m_ignoreLimits = false;

				newConstraint = new hkConstraintInstance (entityA, entityB, newConstraintData, originalConstraint->getPriority());
				newConstraintData->removeReference();

				break;
			}

		case hkConstraintData::CONSTRAINT_TYPE_RAGDOLL:
			{
				hkRagdollConstraintData* oldConstraintData = static_cast<hkRagdollConstraintData*> (constraintData);

				hkPoweredRagdollConstraintData* newConstraintData = new hkPoweredRagdollConstraintData();

				//*newConstraintData = *oldConstraintData;
				newConstraintData->m_basisA = oldConstraintData->m_basisA;
				newConstraintData->m_basisB = oldConstraintData->m_basisB;
				newConstraintData->m_coneMinAngle  = oldConstraintData->m_coneMinAngle;
				newConstraintData->m_planeMinAngle = oldConstraintData->m_planeMinAngle;
				newConstraintData->m_planeMaxAngle = oldConstraintData->m_planeMaxAngle;
				newConstraintData->m_twistMinAngle = oldConstraintData->m_twistMinAngle;
				newConstraintData->m_twistMaxAngle = oldConstraintData->m_twistMaxAngle;

				newConstraintData->m_maxFrictionTorque = oldConstraintData->m_maxFrictionTorque;


				newConstraintData->setTwistMotor(constraintMotor);
				newConstraintData->setPlaneMotor(constraintMotor);
				newConstraintData->setConeMotor(constraintMotor);

				newConstraintData->m_motorsActive = false;

				newConstraint = new hkConstraintInstance (entityA, entityB, newConstraintData, originalConstraint->getPriority());
				newConstraintData->removeReference();

				break;

			}

		default:
			{
				HK_WARN_ALWAYS (0xabba1b34, "Cannot convert constraint to powered constraint.");
				return HK_NULL;
			}
	}

	return newConstraint;

}

hkConstraintInstance* HK_CALL hkConstraintUtils::convertToLimits (hkConstraintInstance* originalConstraint)
{
	hkConstraintData* originalData = originalConstraint->getData();

	hkConstraintData* limitData = HK_NULL;

	switch(originalData->getType())
	{
	case hkConstraintData::CONSTRAINT_TYPE_POWEREDRAGDOLL:
	case hkConstraintData::CONSTRAINT_TYPE_RAGDOLL:
		{
			hkRagdollLimitsData* n = new hkRagdollLimitsData();
			hkRagdollConstraintData* o = static_cast<hkRagdollConstraintData*>(originalData);
			limitData = n;

			n->setInBodySpace(o->m_basisA.m_planeAxis, o->m_basisB.m_planeAxis, o->m_basisA.m_twistAxis, o->m_basisB.m_twistAxis);

			n->m_coneMinAngle = o->m_coneMinAngle;
			n->m_planeMinAngle = o->m_planeMinAngle;
			n->m_planeMaxAngle = o->m_planeMaxAngle;
			n->m_twistMinAngle = o->m_twistMinAngle;
			n->m_twistMaxAngle = o->m_twistMaxAngle;
			n->m_maxFrictionTorque = o->m_maxFrictionTorque;
			n->m_angularLimitsTauFactor = o->m_angularLimitsTauFactor;

			break;
		}
	case hkConstraintData::CONSTRAINT_TYPE_LIMITEDHINGE:
	case hkConstraintData::CONSTRAINT_TYPE_POWEREDHINGE:
		{
			hkHingeLimitsData* n = new hkHingeLimitsData();
			hkLimitedHingeConstraintData* o = static_cast<hkLimitedHingeConstraintData*>(originalData);
			limitData = n;

			n->setInBodySpace(o->m_basisA.m_axle, o->m_basisB.m_axle, o->m_basisA.m_perpToAxle2, o->m_basisB.m_perp2FreeAxis);

			n->m_minAngle = o->m_minAngle;
			n->m_maxAngle = o->m_maxAngle;
			n->m_maxFrictionTorque = o->m_maxFrictionTorque;
			n->m_angularLimitsTauFactor = o->m_angularLimitsTauFactor;

			break;
		}
	case hkConstraintData::CONSTRAINT_TYPE_RAGDOLL_LIMITS:
	case hkConstraintData::CONSTRAINT_TYPE_HINGE_LIMITS:
		{
			// Just return the original constriant.
			// Remember to add a reference.
			HK_WARN(0xad67d7db, "The original constraint already is of the 'limits' type.");
			originalConstraint->addReference();
			return originalConstraint;
		}

	default:
		{
			HK_ASSERT2(0xad67d7da, false, "Unsupported constraint type. Cannot generate limits constraints.");
			return HK_NULL;
		}
	}

	if (limitData)
	{
		hkConstraintInstance* limitInstance = new hkConstraintInstance( originalConstraint->getEntityA(), originalConstraint->getEntityB(), limitData, originalConstraint->getPriority() );
		limitData->removeReference();

		return limitInstance;
	}

	return HK_NULL;
}



hkResult hkConstraintUtils::getConstraintPivots (const hkConstraintInstance* constraint, hkVector4& pivotInAOut, hkVector4& pivotInBOut)
{
	hkConstraintData *constraintData = constraint->getData();

	switch (constraintData->getType())
	{
		case hkConstraintData::CONSTRAINT_TYPE_BALLANDSOCKET:
		{
			hkBallAndSocketConstraintData* bsConstraint = static_cast<hkBallAndSocketConstraintData*> (constraintData);
			pivotInAOut = bsConstraint->m_pivotInA;
			pivotInBOut = bsConstraint->m_pivotInB;
		}
		break;

		case hkConstraintData::CONSTRAINT_TYPE_HINGE:
		{
			hkHingeConstraintData* hConstraint = static_cast<hkHingeConstraintData*> (constraintData);
			pivotInAOut = hConstraint->m_basisA.m_pivot;
			pivotInBOut = hConstraint->m_basisB.m_pivot;
		}
		break;

		case hkConstraintData::CONSTRAINT_TYPE_LIMITEDHINGE:
		case hkConstraintData::CONSTRAINT_TYPE_POWEREDHINGE:
		{
			hkLimitedHingeConstraintData* hConstraint = static_cast<hkLimitedHingeConstraintData*> (constraintData);
			pivotInAOut = hConstraint->m_basisA.m_pivot;
			pivotInBOut = hConstraint->m_basisB.m_pivot;
		}
		break;

		case hkConstraintData::CONSTRAINT_TYPE_RAGDOLL:
		case hkConstraintData::CONSTRAINT_TYPE_POWEREDRAGDOLL:
		{
			hkRagdollConstraintData* rConstraint = static_cast<hkRagdollConstraintData*> (constraintData);
			pivotInAOut = rConstraint->m_basisA.m_pivot;
			pivotInBOut = rConstraint->m_basisB.m_pivot;
		}
		break;

		default:
		{
			HK_WARN_ALWAYS (0xabbabf3b, "Unsupported type of constraint in prepareSystemForRagdoll()");
			return HK_FAILURE;
		}
	}

	return HK_SUCCESS;
}

hkResult hkConstraintUtils::getConstraintMotors(const hkConstraintData* constraintData, hkConstraintMotor*& motor0, hkConstraintMotor*& motor1, hkConstraintMotor*& motor2 )
{
	switch (constraintData->getType())
	{
	case hkConstraintData::CONSTRAINT_TYPE_POWEREDHINGE:
		{
			const hkPoweredHingeConstraintData* hConstraint = static_cast<const hkPoweredHingeConstraintData*> (constraintData);
			motor0 = hConstraint->getMotor();
			motor1 = HK_NULL;
			motor2 = HK_NULL;
		}
		break;

	case hkConstraintData::CONSTRAINT_TYPE_POWEREDRAGDOLL:
		{
			const hkPoweredRagdollConstraintData* rConstraint = static_cast<const hkPoweredRagdollConstraintData*> (constraintData);
			// Possibly, those motors should be extracted in a different order.
			motor0 = rConstraint->getTwistMotor();
			motor1 = rConstraint->getConeMotor();
			motor2 = rConstraint->getPlaneMotor();
		}
		break;

	default:
		{
			motor0 = motor1 = motor2 = HK_NULL;
			HK_WARN_ALWAYS (0xabbae233, "This type of constraint does not have motors");
			return HK_FAILURE;
		}
	}

	return HK_SUCCESS;
}


hkBool hkConstraintUtils::checkAndFixConstraint (const hkConstraintInstance* constraint, hkReal maxAllowedError, hkReal relativeFixupOnError)
{
	hkVector4 childPivotInChild;
	hkVector4 parentPivotInParent;

	hkResult res = 	getConstraintPivots(constraint, childPivotInChild, parentPivotInParent);

	// Unsupported constraint type? return false
	if (res!=HK_SUCCESS)
	{
		return false;
	}

	// The pivotInA should be 0,0,0 if the pivots are properly aligned, warn otherwise
	if (childPivotInChild.lengthSquared3()>1e-6f)
	{
		HK_WARN_ALWAYS (0xabba5dff, "Pivot of child rigid body (A) is expected to be aligned with the constraint at setup time.");
	}

	hkRigidBody* parentRigidBody = constraint->getRigidBodyB();
	hkRigidBody* childRigidBody = constraint->getRigidBodyA();

	const hkTransform& worldFromParent = parentRigidBody->getTransform();
	const hkTransform& worldFromChild = childRigidBody->getTransform();

	hkVector4 parentPivotInWorld; parentPivotInWorld.setTransformedPos(worldFromParent, parentPivotInParent);
	hkVector4 childPivotInWorld; childPivotInWorld.setTransformedPos(worldFromChild, childPivotInChild);

	hkVector4 error; error.setSub4(parentPivotInWorld, childPivotInWorld);

	// Are they aligned ?
	if (error.lengthSquared3()>maxAllowedError*maxAllowedError)
	{
		// NO

		// Interpolate the new position between the desired position and the position of the parent
		const hkVector4& parentPositionInWorld = parentRigidBody->getPosition();
		hkVector4 newChildPositionInWorld; newChildPositionInWorld.setInterpolate4(parentPivotInWorld, parentPositionInWorld, relativeFixupOnError);

		childRigidBody->setPosition(newChildPositionInWorld);

		// Set the velocity to match the parent
		childRigidBody->setLinearVelocity(parentRigidBody->getLinearVelocity());
		childRigidBody->setAngularVelocity(parentRigidBody->getAngularVelocity());

		return true; // fix up done
	}
	else
	{
		// YES

		return false; // no fix up done
	}

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
