/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_COLLIDABLE_H
#define HK_COLLIDE2_COLLIDABLE_H

#include <hkcollide2/dispatch/broadphase/hkTypedBroadPhaseHandle.h>
#include <hkmath2/basetypes/hkMotionState.h>

class hkShape;
class hkCollisionResponse;

	/// An hkCollidable can collide with other hkCollidables. Each hkEntity has an hkCollidable
	/// member that allows it to work with the collision detection system. It has an hkShape and
	/// a transform for that shape. This transform matches the position of the corresponding
	/// hkEntity, thus linking the collision detection system to the dynamics system.
class hkCollidable : public hkTypedBroadPhaseHandle
{
	public:
			
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_CDINFO, hkCollidable);

			/// Creates a new hkCollidable.
		inline hkCollidable(const hkShape* shape,
							const hkMotionState* transform,
							const hkCollisionResponse* response,
							void* owner,
							int type);

		~hkCollidable();

			/// Gets the transform.
		inline const hkTransform* getTransform() const;

			/// Gets the motion state.
		inline const hkMotionState* getMotionState() const;

			/// Sets the transform.
		inline void setMotionState( const hkMotionState* t );

			/// Sets the hkShape.
		inline void setShape(hkShape* shape);
		
			/// Gets the hkShape.
		inline const hkShape* getShape() const;

			/// Gets the collision response.
		inline const hkCollisionResponse* getResponse() const;

			/// Sets the collision response.
		inline void setResponse( const hkCollisionResponse* response );

			/// Sets the entity that owns this hkCollidable.
		inline void setOwner( void* owner );

			/// Gets the entity that owns this hkCollidable.
		inline void* getOwner() const;

		void checkPerformance();

	public:

			/// The shape is the representation of the colliding geometry, the shape type is
			/// turned into an appropriate agent by the hkCollisionDispatcher.
		const hkShape* m_shape;

			/// A pointer to the transform.
		const hkMotionState* m_motionState;

			/// The response type is turned into an appropiate hkContactMgrFactory by the
			/// hkCollisionDispatcher.
		const hkCollisionResponse* m_response;

			/// A user pointer. This is used by hkDynamics to point to the hkRigidBody.
		void* m_owner;
};

#include <hkcollide2/hkCollidable.inl>

#endif // HK_COLLIDE2_COLLIDABLE_H

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
