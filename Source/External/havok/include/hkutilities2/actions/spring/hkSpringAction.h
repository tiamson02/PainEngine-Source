/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_SPRING_ACTION_H
#define HK_SPRING_ACTION_H

#include <hkdynamics2/action/hkBinaryAction.h>
#include <hkmath2/hkMath.h>

class hkRigidBody;

/// An hkSpringActionConstructionInfo contains all the information needed to construct an hkSpringAction. 
class hkSpringActionConstructionInfo
{
 
	public:

		hkSpringActionConstructionInfo();


			///	Sets the spring information in world space.
			/// By default the rest length is computed in the set methods
			/// as the distance between the pivots in world space.
		inline  void setInWorldSpace(hkRigidBody* bodyA,hkRigidBody* bodyB,
								   const hkVector4& pivotA,const hkVector4& pivotB);

			///	Sets the spring information in body space.
			/// By default the rest length is computed in the set methods
			/// as the distance between the pivots in world space.
		inline  void setInBodySpace(hkRigidBody* bodyA,hkRigidBody* bodyB,
								   const hkVector4& pivotA,const hkVector4& pivotB);
 
			/// Sets the strength value for the spring.	 
		inline void setStrength(hkReal str);

			/// Sets the damping value for the spring.
		inline void setDamping(hkReal damp);

			/// Sets the spring's rest length.
		inline void setRestLength(hkReal restLength);

			/// Sets spring's compression flag
		inline void setOnCompression(bool onCompression);

			/// Sets spring's extension flag
		inline void setOnExtension(bool onExtension);

	public:

		hkRigidBody* m_bodyA;
		hkRigidBody* m_bodyB;

		hkVector4 m_pivotA;
		hkVector4 m_pivotB;

		hkReal	m_restLength;
		hkReal	m_strength;
		hkReal	m_damping;

		bool	m_onCompression;
		bool	m_onExtension;
};


/// A simple spring.
class hkSpringAction: public hkBinaryAction
{
	public:
		
			///Creates a spring with the specified construction info.
		hkSpringAction( hkSpringActionConstructionInfo& info );
  
			///Sets the spring's first rigid body.
		inline void setBodyA( hkRigidBody* ra, const hkVector4& positionAinA );

			///Gets the spring connection position in rigid body A
		inline const hkVector4& getPositionAinA();

			///Sets the spring's second rigid body.
	    inline void setBodyB( hkRigidBody* rb, const hkVector4& positionBinB );

			///Gets the spring connection position in rigid body B
		inline const hkVector4& getPositionBinB();
		
			/// Sets the strength value for the spring.	 
        inline void setStrength( hkReal str );

			/// Gets the strength value for the spring.	 
		inline hkReal getStrength();

			/// Sets the damping value for the spring.
        inline void setDamping( hkReal damp );

			/// Gets the damping value for the spring.
        inline hkReal getDamping();

			/// Sets the rest length value for the spring.
        inline void setRestLength( hkReal restLength );

			/// Gets the rest length value for the spring.
        inline hkReal getRestLength();

			/// Sets the compression flag for the spring.
        inline void setOnCompression( bool onCompression );

			/// Gets the compression flag for the spring.
        inline bool getOnCompression();

			/// Sets the extension flag for the spring.
        inline void setOnExtension( bool onExtension );

			/// Gets the extension flag for the spring.
        inline bool getOnExtension();

			/// Gets the last force applied by the spring.
		inline const hkVector4& getLastForce();

		void applyAction( const hkStepInfo& stepInfo );

	protected:

		hkVector4 m_lastForce;
		hkVector4 m_positionAinA;
		hkVector4 m_positionBinB;
		hkReal	m_restLength;
		hkReal	m_strength;
		hkReal	m_damping;
		bool	m_onCompression;
		bool	m_onExtension;

      
};

#include <hkutilities2/actions/spring/hkSpringAction.inl>

#endif // HK_SPRING_H

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
