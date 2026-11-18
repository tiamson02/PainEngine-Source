/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


#include <hkdynamics2/hkDynamics2.h>
#include <hkutilities2/actions/dashpot/hkDashpotAction.h>
#include <hkdynamics2/entity/hkRigidBody.h>

////////////////////////////////////////////////////////////////////////

hkDashpotActionConstructionInfo::hkDashpotActionConstructionInfo()
{
  m_bodyA = HK_NULL;
  m_bodyB = HK_NULL;
  // set default values
  m_strength = 1;
  m_damping = .1f;
}

////////////////////////////////////////////////////////////////////////

hkDashpotActionConstructionInfo::~hkDashpotActionConstructionInfo()
{

}

////////////////////////////////////////////////////////////////////////


/// \class hkDashpotAction 
///	A dashpot works by applying forces to its object or objects in an attempt 
/// to keep the attachment points at the same position in world space. 
/// These attachment points are specified in the
/// local space of the rigid bodies.
/// Strong forces (such as a Mouse Spring) or impulses may momentarily break this constraint, but the bodies will 
/// automatically snap back to reinstate it. 
/// \n
/// In general, dashpots are faster than constraints,
/// but have less stability.
/// For example, high strength values can add instability to the system, and can cause the system
/// to gain energy and even explode. It is advisable to have a strength that is equal to the heaviest
/// rigid body - for instance, if that body has a mass of 10 kg, a good value for strength
/// is 10. It is also advisable to have a damping that is 1/10 the value of the strength.
/// Values of much higher than this can make the system explode.\n
/// \n
/// The stability of the system is also linked to
/// the size of the timesteps taken during simulation - a bigger timestep can make
/// the system more unstable.  A timestep of .01666 (60 Hz) with two or three
/// substeps is generally required.

/// \param str The strength of the dashpot range[see notes] default==1
/// \param damp The damping coeffecient of the dashpot - this determines the rate of decay of oscillations of the dashpot. A good value for damping is 1/10 the value of the strength. range[see notes] default==0.1

hkDashpotAction::hkDashpotAction(const hkDashpotActionConstructionInfo& info) : 
                 hkBinaryAction( info.m_bodyA, info.m_bodyB )
{
   HK_ASSERT(info.m_bodyA != HK_NULL  && "Initialize construction info before passing to constructor!");
   HK_ASSERT(info.m_bodyB != HK_NULL  && "Initialize construction info before passing to constructor!");
   
   m_point[0] = info.m_pivotA;
   m_point[1] = info.m_pivotB;
   m_strength = info.m_strength;
   m_damping = info.m_damping; 
}

////////////////////////////////////////////////////////////////////////////////////

/// This function returns the last impulse has been applied to the bodies by the
/// dashpot.
const hkVector4 & hkDashpotAction::getImpulse ()
{
	return m_impulse;
}


/// A dashpot is an action and so it has an apply method . This method is called
/// every timestep and applies the necessary impulses to ra and rb to achieve the
/// dashpot motion.
void hkDashpotAction::applyAction(const hkStepInfo& stepInfo)
{
	m_impulse.setZero4();

	HK_TIMER_BEGIN("Actions/Dashpots", HK_NULL);

	const hkReal dtscale = 151; // to keep constants sensible around 1
	hkReal dt = dtscale * (stepInfo.m_deltaTime);
	
	
	hkVector4 pa;

	pa.setTransformedPos(m_bodyA->getTransform(),m_point[0]);
	const hkVector4& va = m_bodyA->getLinearVelocity();

	hkVector4 pb;

	pb.setTransformedPos(m_bodyB->getTransform(),m_point[1]);
	const hkVector4& vb = m_bodyB->getLinearVelocity();

	//m_impulse = (dt * m_strength) * ( pa - pb ) +  * ( va - vb ) Below
    {
      hkVector4 pab;
	  hkVector4 vab;
	
	  pab.setSub4(pa,pb);
	  vab.setSub4(va,vb);

	  m_impulse.setMul4(dt * m_strength, pab);
	  vab.mul4(dt * m_damping);
	  m_impulse.add4(vab);
	}

	/*
	DISPLAY2(showPoint,	pa, 0xffffffff );
	DISPLAY2(showPoint,	pb, 0xffffffff );
	DISPLAY3(showLine,	pb, pb+m_impulse,	0xffff0000 );
    */

	{ 
		hkVector4 negImpulse;
		negImpulse.setMul4(-1,m_impulse);
		m_bodyA->applyPointImpulse(negImpulse, pa); 
		hkVector4 velocity;

        velocity = m_bodyA->getLinearVelocity(); 
		velocity.mul4(1.f-.001f);
		m_bodyA->setLinearVelocity(velocity);

        velocity = m_bodyA->getAngularVelocity(); 
		velocity.mul4(1.f-.001f);
		m_bodyA->setAngularVelocity(velocity);
	}
	
	{ 
		m_bodyB->applyPointImpulse( m_impulse, pb); 

		hkVector4 velocity;

        velocity = m_bodyB->getLinearVelocity(); 
		velocity.mul4(1.f-.001f);
		m_bodyB->setLinearVelocity(velocity);

        velocity = m_bodyB->getAngularVelocity(); 
		velocity.mul4(1.f-.001f);
		m_bodyB->setAngularVelocity(velocity);
	}

	HK_TIMER_END();
}




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
