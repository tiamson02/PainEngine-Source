/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


#ifndef HK_DYNAMICS2_ENTITY_H
#define HK_DYNAMICS2_ENTITY_H

#include <hkmath2/hkMath.h>
#include <hkdynamics2/common/hkProperty.h>
#include <hkdynamics2/entity/hkEntityDeactivator.h>
#include <hkcollide2/hkCollidable.h>
#include <hkcollide2/shape/hkShape.h>
#include <hkdynamics2/common/hkMaterial.h>
#include <hkdynamics2/entity/hkEntityConstructionInfo.h>
#include <hkdynamics2/world/hkBroadPhaseTypes.h>

class hkEntityListener;
class hkEntityActivationListener;
class hkCollisionListener;
class hkMotion;
class hkSimulationIsland;
class hkWorld;

	/// This class represents the core "physical object" elements in the dynamics system, such
	/// as rigid bodies. 
class hkEntity: public hkReferencedObject
{
	public:

			//
			// PROPERTIES
			//
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_ENTITY)

			/// Adds a property to the entity. Properties must be unique. 
			/// You can use properties to add additional information to an entity - e.g.
			/// for using your own collision filters or backlinks into your game.
		void addProperty( hkUint32 key, hkPropertyValue value );

			/// Removes the property of the specified type. This function returns the property
			/// removed, or 0 if there is no property of the given type.
		hkPropertyValue removeProperty( hkUint32 key );

			/// Returns the property specified, or 0 if there is no property of the given type.
		inline hkPropertyValue getProperty( hkUint32 key ) const ;

			/// Returns whether a property of this type has been set for the entity.
		inline bool hasProperty( hkUint32 key ) const ;

			/// Get construction info from this entity
		inline void getConstructionInfo(hkEntityConstructionInfo& info);	


			//
			// EVENT HANDLING
			//

			/// Adds an entity listener to the entity.
		void addEntityListener( hkEntityListener* el);

			/// Removes an entity listener from the entity.
		void removeEntityListener( hkEntityListener* el);

			/// Adds an entity activation listener to the entity.
		void addEntityActivationListener( hkEntityActivationListener* el);

			/// Removes an entity activation listener from the entity.
		void removeEntityActivationListener( hkEntityActivationListener* el);

			/// Adds a collision listener to the entity.
		void addCollisionListener( hkCollisionListener* cl );

			/// Removes a collision listener from the entity.
		void removeCollisionListener( hkCollisionListener* cl);

			/// Get const access to the array of entity listeners.
		inline const hkArray<hkEntityListener*>& getEntityListeners() const;

			/// Get const access to the array of entity activation listeners.
		inline const hkArray<hkEntityActivationListener*>& getEntityActivationListeners() const;
		
			/// Get const access to the array of entity listeners.
		inline const hkArray<hkCollisionListener*>& getCollisionListeners() const;

			/// Gets the process contact callback delay.
		inline hkUint16 getProcessContactCallbackDelay();

			/// Sets the process contact callback delay.
			/// This value is used to determine how often a callback is raised for the
			/// "process contact" collision event.
			/// A value of 0 means the callback is called every step, whereas a value of 4 means 
			/// that a callback is raised every 5th step.
			/// Process contact callbacks can be used to change properties of contact points, such
			/// as the friction of a sliding contact.
		inline void setProcessContactCallbackDelay( hkUint16 delay );


			//
			// UTILITY FUNCTIONS
			//

			/// Gets the collidable owned by this entity.
			/// If the entity has no collision detection representation, 
			/// the shape in the collidable will be HK_NULL.
		inline hkCollidable& getCollidable();

			/// Gets the material used by this entity.
			/// If the entity has no collision detection representation, 
			/// the material is not used.
		inline hkMaterial& getMaterial();

			/// A utility function to determine if the entity is fixed.
		inline bool isFixed() const;


			//
			// DEACTIVATION
			//

			/// Gets the deactivator for the entity. The setDeactivator calls are in the 
			/// hkEntity derived classes, to ensure type safety.
		inline hkEntityDeactivator* getDeactivator();

			/// Returns whether the entity is active. This method returns false if the entity
			/// has not yet been added to a hkWorld object.
		bool isActive() const;

			/// Forces the entity to deactivate. This has the effect of deactivating all entities
			/// in the same simulation island as this entity.
		void forceDeactivate();

			/// Forces the entity to activate. This has the effect of activating all entities
			/// in the same simulation island as this entity.
		void forceActivate();

			/// Gets the owner of the entity.
		inline hkWorld* getOwner();

	protected:

			/// Set the shape.
			/// NB: This function is NOT intended to be called at runtime i.e. after
			/// the hkEntity has been added to an hkWorld.
		inline bool setShape(hkShape* shape);

			/// Creates a new entity using the provided construction info. Note that the
			/// constructor does not fully initialize the hkCollidable (explain?). 
		inline hkEntity( const hkEntityConstructionInfo& info );

			/// Destructor.
		~hkEntity();

		hkMotion* m_motion;

		inline void setDeactivator( hkEntityDeactivator* deactivator );

	protected:

			///Indicates whether the entity is fixed.
		bool m_fixed:1;

			/// The entity's simulation island.
		hkSimulationIsland* m_simulationIsland;

			/// The entity's collidable, only used if the collision detection is enabled.
		hkCollidable m_collidable;

			/// The entity's material, only used if the collision detection is enabled.
		hkMaterial   m_material;

			/// The entity's deactivator.
		hkEntityDeactivator* m_deactivator;

			/// The world that owns this entity.
		hkWorld*			m_owner;

			// ------------------ 2nd CacheLine64 (rarely accessed data ) -------------------------

	private:

		int m_storageIndex;

		hkUint16 m_processContactCallbackDelay;

		friend class hkEntityCallbackUtil;
		friend class hkWorldCallbackUtil;
		hkInplaceArray<hkCollisionListener*,1> m_collisionListeners;
		hkArray<hkEntityListener*> m_entityListeners;
		hkArray<hkEntityActivationListener*> m_activationListeners;

		hkArray<hkProperty> m_properties;


	public:

			//
			// INTERNAL FUNCTIONS
			//

			/// Simulation units use this interface.
		inline hkMotion* getMotion();

			/// Get the simulation island, is HK_NULL for entities not in simulation.
		inline hkSimulationIsland* getSimulationIsland();

		inline void setSimulationIsland( hkSimulationIsland* island );

		inline int getStorageIndex();

		inline void setStorageIndex(int index);

		inline void setOwner( hkWorld* owner );

		inline void cleanupEntityListeners();
};

//HK_COMPILE_TIME_ASSERT( HK_OFFSET_OF( hkEntity, m_constrainable ) == 0x0c );
//HK_COMPILE_TIME_ASSERT( HK_OFFSET_OF( hkEntity, m_simulationIsland ) == 0x14 );

//HK_COMPILE_TIME_ASSERT( HK_OFFSET_OF( hkEntity, m_collisionFilterInfo ) == 0x40 );

	// check to see if the memory pool gives us a 64 byte aligned position
//HK_COMPILE_TIME_ASSERT( sizeof(hkEntity) >= 97 );
//HK_COMPILE_TIME_ASSERT( sizeof(hkEntity) <= 128 );

#include <hkdynamics2/entity/hkEntity.inl>

#endif // HK_DYNAMICS2_ENTITY_H

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
