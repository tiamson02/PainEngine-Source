/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_RIGID_CONSTRAINT_H
#define HK_DYNAMICS2_RIGID_CONSTRAINT_H

#include <hkdynamics2/constraint/hkConstraint.h>

class hkRigidBody;

	/// A base classs for all constraints between rigid bodies
class hkRigidConstraint: public hkConstraint
{
	public:
			/// Constructor takes two rigid bodies
		inline hkRigidConstraint(hkRigidBody *a, hkRigidBody *b )
		{
			this->setEntityA( reinterpret_cast<hkEntity*>(a) );
			this->setEntityB( reinterpret_cast<hkEntity*>(b) );
		}

			/// Get the first constrained body.
		inline hkRigidBody* getRigidBodyA() const { return reinterpret_cast<hkRigidBody*>( getEntityA() ); }

			/// Get the second constrained body.
		inline hkRigidBody* getRigidBodyB() const { return reinterpret_cast<hkRigidBody*>( getEntityB() );	}
};

#endif // HK_DYNAMICS2_RIGID_CONSTRAINT_H

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
