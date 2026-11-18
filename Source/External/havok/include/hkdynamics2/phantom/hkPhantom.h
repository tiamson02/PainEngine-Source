/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_PHANTOM_H
#define HK_DYNAMICS2_PHANTOM_H

#include <hkbase/hkBase.h>
#include <hkcollide2/hkCollidable.h>
#include <hkcollide2/algorithms/boundingvolume/hkAabb.h>
#include <hkdynamics2/phantom/hkPhantomConstructionInfo.h>
#include <hkdynamics2/common/hkProperty.h>

class hkAabb;
class hkTypedBroadPhaseHandler;
class hkBroadPhase;
class hkCollidable;
class hkWorld;


/// A phantom is a pinned entity that maintains a list of other entities whose AABBs overlap with it (including other phantoms).
/// The phantom itself is just an AABB and has no physical presence in the simulation. You can use phantoms to create areas where events occur if the area is entered.
/// Phantoms are very fast as they use only broadphase collision detection.
class hkPhantom : public hkReferencedObject
{
	public:
			/// Creates a new hkPhantom using the information in the construction info parameter.
			/// To add a phantom to the world, you use hkWorld::addEntity( myPhantom ).
		hkPhantom( const hkPhantomConstructionInfo& info );

		
			/// Get the aabb of the phantom
		inline const hkAabb& getAabb() const;

			/// Get the phantom's collidable
		inline hkCollidable& getCollidable();

			/// Get the world that owns the phantom. This is HK_NULL if the phantom has not been added to the world
		inline hkWorld* getOwner();

			/// Moves the phantom around. This updates the broad phase.
		virtual void updateAabb( const hkAabb& aabb );


			/// Adds a property to the phantom. Properties must be unique. 
			/// You can use properties to add additional information to a phantom- for instance, for using your own collision filters or backlinks into your game.
		void addProperty( hkUint32 key, hkPropertyValue value );

			/// Removes the property of the specified type. This function returns the property removed, or 0 if there is no property of the given type.
		hkPropertyValue removeProperty( hkUint32 key );

			/// Returns the property specified, or 0 if there is no property of the given type.
		inline hkPropertyValue getProperty( hkUint32 key ) const ;

			/// Returns whether a property of this type has been set for the phantom.
		inline bool hasProperty( hkUint32 key ) const ;


	public:
		
			/// Callback implementation 
		virtual void addOverlappingCollidable( hkCollidable& collidable ) = 0;

			/// Callback implementation 
		virtual void removeOverlappingCollidable( hkCollidable& collidable ) = 0;

			/// This fuction is only called by hkWorld
		inline void setOwner( hkWorld* world );

	protected:

		hkCollidable m_collidable;
		hkAabb m_aabb;
		hkArray<hkProperty> m_properties;
		hkWorld* m_owner;
};

#include <hkdynamics2/phantom/hkPhantom.inl>

#endif	//HK_DYNAMICS2_PHANTOM_H



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
