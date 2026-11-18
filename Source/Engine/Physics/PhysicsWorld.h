
// #pragma once
#ifndef _PHYSICSWORLD_
#define _PHYSICSWORLD_


class Entity;
class HavokWorld;
class Model;
class WorldMesh;
class Zone;
class World;

#include "Core/Math/LeMath.h"

struct HitData
{
    float   Dist;
    Vector  HitPoint;
    Vector  Normal;
	void*   HavokEntity;
	void*	UserData;
};

#ifndef TRANSGAMING
class ENGINE_API PhysicsWorld
#else
class PhysicsWorld
#endif
{
	friend class PhysicsEngine;
	friend class PhysicsObject;
	friend class Ragdoll;
	friend class havokWorldBorder;
public:
	~PhysicsWorld();

	void Start( bool loading );
    void WarmUp(float warmTime);
#ifdef TRANSGAMING
	bool Tick( float timeDelta );
#else
	inline bool Tick( float timeDelta );
#endif
	inline bool TickWillMovePhysics( float timeDelta );
#ifndef TRANSGAMING
	inline void UpdatePredictionTick( float timeDelta );
	inline void UpdateEntities( DWORD msToAdd = 0 );
#else
	void UpdatePredictionTick( float timeDelta );
	void UpdateEntities( DWORD msToAdd = 0 );
#endif
	void Pause();

	// Physics load/save - for savegames
	bool LoadPhysicsWorld( FileObject* f );
	bool SavePhysicsWorld( FileObject* f );

	PhysicsObject* CreatePhysicsObject( const Vector& pos, const Quaternion& quat, DWORD poBodyType, Entity* entity = NULL, float scale = 1.0f, int collisionGroup = -1, bool dontAddToWorld = false );
	
	bool CalculatePawnToPointVisibility( PhysicsObject* po, Vector& positionToSee );
	bool CalculatePawnToEntityVisibility( PhysicsObject* po, PhysicsObject* entity );

	Vector GetGravity( void );

	// Region methods
	void* CreateRegionFromPoints( DynamicArray<Vector>& points, void* userInfo, float mass );	// Returns NULL on failure
	void DestroyRegion( void* regionHandle );
	void SetRegionPos( void* regionHandle, const Vector& pos );
	Vector GetRegionPos( void* regionHandle );

	// Ragdoll havok body management
	Quaternion GetHavokBodyRotation( void* body );
	void SetHavokBodyRotation( void* body, Quaternion& quat );
	Vector GetHavokBodyVelocity( void* body );
	void SetHavokBodyVelocity( void* body, Vector vel );
	Vector GetHavokBodyPosition( void* body );
	void SetHavokBodyPosition( void* body, Vector vel );
	void PinHavokBody( void* body, bool pin );
	bool IsHavokBodyPinned( void* body );
	bool IsHavokBodyInWorld( void* body );
	void RemoveHavokBodyFromIS( void* body, bool remove );
	BYTE GetHavokBodyActiveGroup( void* body );
	inline Model* GetModelByConstraint( void* constraint );
	
	// Box tracing
	void GetIntersectingEntities( BoundingBox& box, DynamicArray<Entity*>& entityList, int collisionGroup = -1 );

	// Line tracing
    BOOL LineTrace(const Vector& start, const Vector& end, HitData& hitData );
    BOOL LineTraceHitPlayer(const Vector& start, const Vector& end, HitData& hitData );
    BOOL LineTraceFixedGeom(const Vector& start, const Vector& end, HitData& hitData );
	BOOL LineTraceFirstHit(const Vector& start, const Vector& end );
	void HitEntity(void* entity, const Vector& hitPoint, const Vector& hitVector);

	// Explosion

#ifdef TRANSGAMING
	void Explosion( Vector center, float pow, float range, int ownerID, int weapon_type, float weapon_damage );
    void MultiplayerExplosion( Vector center, float pow, float range, int ownerID, int weapon_type, float weapon_damage, float factorY );
#else
	inline void Explosion( Vector center, float pow, float range, int ownerID, int weapon_type, float weapon_damage );
    inline void MultiplayerExplosion( Vector center, float pow, float range, int ownerID, int weapon_type, float weapon_damage, float factorY );
#endif
	void ExplosionUp( Vector center, float strength, float radius, float maxVelocity, float velocityVariation );
	void ExplosionParabolic( Vector center, float flightTime, float radius, Vector target );
//	void ExplosionBossArena(Vector center, float pow, float range, Entity *owner, int weapon_type, float weapon_damage);
	void SelfExplosion( Vector center, float strength, float radius, DynamicArray<PhysicsObject*>* targetObjects );

	int SetCollisionToAll(bool enable, float minimalDelay, float minimalStrength, float minimalMass = 150, float maximalMass = 5000, float chance = 10, int activeMeshGroup = -1);
	bool EnableCollisionToMesh(bool enable, const char* name, float minimalDelay = 1.0, float minimalStrength = 8.0);
/*
	// Manual constraint management
	void* CreateRigidConstraint( Vector pivot, Model* model, int joint, PhysicsObject* po );
	void* CreateRigidConstraint( Vector pivot, PhysicsObject* po1, PhysicsObject* po2 );
	void* CreateRigidConstraint( Vector pivot, Model* model1, int joint1, Model* model2, int joint2 );
	void* CreateRigidConstraint( Vector pivot, void* hpo1, void* hpo2 );
	inline void RemoveRigidConstraint( void* constraint );
*/

	// Closest points
	float ClosestPointsDistanceBetween( PhysicsObject* po1, PhysicsObject* po2 );
/*
	float ClosestPointsDistanceBetween( PhysicsObject* po1, Model* model2, int joint2 );
	float ClosestPointsDistanceBetween( Model* model1, int joint1, Model* model2, int joint2 );
*/
	// Static meshes
	void ReleaseStaticMeshes( void );
	void RemoveStaticMesh( void* mesh );
	void* GetStaticMesh( Entity* entity);
	bool SetEntityCollisionGroup(Entity* entity, bool activeOnly, int newGroup);
	void StaticMeshesEnableByGroup( BYTE group, bool enable );
	void StaticMeshEnable( Entity *entity, bool enable);

	// Active meshes
#ifndef TRANSGAMING
	inline void ActiveMeshGroupSetTimeToRemove( unsigned char group, float time, float randomizer );
	inline void ActiveMeshGroupEnable( unsigned char group, bool enable );
	inline void ActiveMeshGroupStaticMeshEnable( unsigned char group, bool enable );
	inline void ActiveMeshGroupRecurrentActivationEnable( unsigned char group, bool enable );
	inline void ActiveMeshGroupActivate( unsigned char group );
	inline void SetActiveMeshMaxRecursiveActivationDistance( float distance );
	inline void ActiveMeshGroupSetActivationSettings( unsigned char group, bool enable, float minimalDelay,
		float minimalStrength, float minimalMass, float maximalMass, float chance, float TTDBase, float TTDAddition );
	inline void ActiveMeshGroupSetCollisionGroup( unsigned char group, int collGroup );
#else
	void ActiveMeshGroupSetTimeToRemove( unsigned char group, float time, float randomizer );
	void ActiveMeshGroupEnable( unsigned char group, bool enable );
	void ActiveMeshGroupStaticMeshEnable( unsigned char group, bool enable );
	void ActiveMeshGroupRecurrentActivationEnable( unsigned char group, bool enable );
	void ActiveMeshGroupActivate( unsigned char group );
	void SetActiveMeshMaxRecursiveActivationDistance( float distance );
	void ActiveMeshGroupSetActivationSettings( unsigned char group, bool enable, float minimalDelay,
		float minimalStrength, float minimalMass, float maximalMass, float chance, float TTDBase, float TTDAddition );
	void ActiveMeshGroupSetCollisionGroup( unsigned char group, int collGroup );
#endif
	// Ragdoll precache
	inline void PrecacheRagdoll( Model* model, const char* fileName );

	// Underwater world methods
#ifdef TRANSGAMING
	void EnableUnderwaterWorld( bool enable );
	bool IsUnderwaterWorld( void );
#else
	inline void EnableUnderwaterWorld( bool enable );
	inline bool IsUnderwaterWorld( void );
#endif

	void ReloadWorld( void );
	void GetFromHKE( const char* fileName );

#ifdef TRANSGAMING
	bool CheckStartGlass( void* body, Vector hitPoint, float activationRadius, Vector velocityToSet );
#else
	inline bool CheckStartGlass( void* body, Vector hitPoint, float activationRadius, Vector velocityToSet );
#endif

	float ActiveMeshesMassScale;
	float DefaultMeshFriction;
	float DefaultMeshRestitution;

#ifdef TRANSGAMING
	bool LineTraceStaticMesh( WorldMesh* mesh, Vector& start, Vector& end, HitData& hitData );
#else
	inline bool LineTraceStaticMesh( WorldMesh* mesh, Vector& start, Vector& end, HitData& hitData );
#endif

	// Network support
	Entity* GetEntityByActiveMeshIndex( short index );
	void StaticMeshEntityCheck( void );

	// For multiplayer prediction
	inline void MPPredSaveRagdolls( void);
	inline void MPPredRestore( void );
	inline void MPPredAddRestToWorld( void );
	inline void MPPredRemoveAllFromWorld( void );

	float GetTickPart( void );
	ulong32 GetPhysicsFrame( void );
	void SetPhysicsFrame( ulong32 frame );
	void AdvancePhysicsFrame( void );

private:
	PhysicsWorld( float deactDelay, float deactMaxPosDiff );

	PhysicsObject* CreatePhysicsObjectFromMesh( const Vector& pos, const Quaternion& quat, DWORD poBodyType, WorldMesh* mesh, int collisionGroup, bool dontAddToWorld = false );
	PhysicsObject* CreatePhysicsObjectFromRagdoll( const Vector& pos, const Quaternion& quat, Model* model, float scale, int collisionGroup, bool dontAddToWorld = false );
//	PhysicsObject* CreatePhysicsObjectFromHKE( WorldMesh* mesh, const char* fileName, float scale );

	void AddMesh( WorldMesh* mesh, float friction, float restitution );

	bool VisibilityWalkPortals( RaySegment& ray, Zone* zoneStart, Zone* zoneEnd );
	int ResetWorldPortalsCounter( void );
	void SetWorldPortalsCounter( int counter );

	// Variables
	HavokWorld*	mHavokWorld;
	World* mEngineWorld;
	bool mPaused;
	bool mFirstTime;
};


#endif //_PHYSICSWORLD_

