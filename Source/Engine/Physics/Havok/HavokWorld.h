
#include "Engine.h"

#include "Core/HashMap.h"
#include "Physics/PhysicsObject.h"
#include "Physics/Havok/HavokPhysics.h"

//#define LOG_CONVEX_CREATION

#ifndef FINAL_BUILD
#define USE_HAVOK_DEBUGGER
#endif

class hkWorld;
class myPhysicsObjectDeactivator;
class hkStiffSpringConstraint;
class hkMoppCode;
class hkVisualDebugger;
class hkRigidBody;
class Ragdoll;
class havokBreakableListener;
class HavokRagdoll;
class RagdollSkeleton;
class WorldMesh;
class Model;

class PinnedActiveMesh
{
public:
	int Index;
	PhysicsObject* ActiveMesh;	// if 0, it's exploding item - use active meshes from PinnedActiveMeshParts table.
	WorldMesh* StaticMesh;
	hkRigidBody* StaticMeshBody;
	Vector ActivationPoint;
	float ActivationExtent;
	float TimeToDelete;
	bool ActivationDisabled:1;
	bool RecurrentActivationDisabled:1;
	DynamicArray<Entity*> MeshParts;

	PinnedActiveMesh() {};
	~PinnedActiveMesh() {};
};

struct ActiveMeshGroupActivationParams
{
	bool EnableCollisions;
	float MinimalDelay;
	float MinimalStrength;
	float MinimalMass;
	float MaximalMass;
	float MinProbability;
	float TTDBase;
	float TTDAddition;
};

struct HavokEntitySavedState
{
	hkTransform Transform;
	hkVector4 Velocity;
	hkVector4 AngularVelocity;
};

class HavokWorldSavedState
{
public:
	HavokWorldSavedState() {};
	~HavokWorldSavedState() { EntitiesFind.Clear(); };

	void SaveRagdolls( HavokWorld* hw );
	void Restore( HavokWorld* hw );
	void SaveObj( hkRigidBody* rb, hkTransform* tr );

private:
	HashMap<void*,int> EntitiesFind;
	HavokEntitySavedState Entities[1000];
};

class HavokRagdollTemplate;

class HavokWorld
{
	friend class PhysicsWorld;
	friend class havokWorldBorder;
	friend class PhysicsObject;
	friend class HavokWorldSavedState;
public:

    HavokWorld( float deactDelay = 5.0f, float deactMaxPosDiff = 0.3f );
	~HavokWorld();

	// Physics load/save - for savegames
	bool LoadWorld( FileObject* f );
	bool SaveWorld( FileObject* f );

	bool Tick( float timeDelta );
	void SingleTick( float timeDelta );
	bool TickWillMovePhysics( float timeDelta );
	void UpdatePredictionTick( float timeDelta );
	void UpdateEntities( DWORD msToAdd );
    void Explosion(Vector center, float strength, float range, int ownerID, int weapon_type, float weapon_damage);
    void MultiplayerExplosion(Vector center, float strength, float range, int ownerID, int weapon_type, float weapon_damage, float factorY  ); // factorY fot PCF MP
	void ActiveMeshGroupSetTimeToRemove( unsigned char group, float time, float randomizer );
	void ActiveMeshGroupSetActivationSettings( unsigned char group, bool enable, float minimalDelay, float minimalStrength,
		float minimalMass, float maximalMass, float chance, float TTDBase, float TTDAddition );

	void ReorganizePinnedActiveMeshTable( void );	// to be used once, after world load
	void ActivatePinnedMesh( int meshNumber, Vector activationCenter, float explosionRange, float strength, int ownerID, int weapon_type, float weapon_damage, int explosionCount );
	void PinnedMeshGroupActivate( unsigned char group );
	void PinnedMeshGroupEnable( unsigned char group, bool enable );
	void PinnedMeshGroupStaticMeshEnable( unsigned char group, bool enable );
	void PinnedMeshGroupRecurrentActivationEnable( unsigned char group, bool enable );
	void PinnedMeshGroupSetCollisionGroup( unsigned char group, int collGroup );

	bool CheckStartGlass( hkRigidBody* rb, Vector hitPoint, float activationRadius, Vector velocityToSet );

	bool LineTraceStaticMesh( WorldMesh* mesh, Vector& start, Vector& end, HitData& hitData );
	void PrintMeshStatistics( WorldMesh* mesh );
	hkRigidBody* CreateStaticMesh( WorldMesh* mesh, float friction, float restitution, int collisionGroup = -1 );
    hkRigidBody* CreatePawnBody(  Vector& pivotOffset, float scale = 1.0f, DWORD pawnBody = EPawnBodyDefault, int collisionGroup = -1 );
	hkRigidBody* CreatePawnBodyFromMesh( WorldMesh* mesh, float& mVolumeMass, DWORD pawnBodyType, int collisionGroup = -1 );
	hkRigidBody* CreatePawnBodyFromRagdoll( Model *model, float& mVolumeMass, float scale = 1.0f, int collisionGroup = -1 );
//	hkRigidBody* CreatePawnBodyFromHKE( Entity* entity, const char* fileName, float scale );
	void RemoveStaticMesh( hkRigidBody* rb );
	void* GetStaticMesh( Entity* entity, bool activeOnly = true );
	bool SetEntityCollisionGroup(Entity* entity, bool activeOnly, int newGroup);

	void EnableUnderwaterWorld( bool enable );
	bool IsUnderwater( void ) { return mIsUnderwater; };

	// Ragdoll methods - usable only by Ragdoll object!
	void PrecacheRagdoll( Model* model, const char* fileName );
	HavokRagdoll* CreateRagdoll( Model* model, RagdollSkeleton* ragdollSkeleton, const char* fileName, float scale, int collisionGroup = -1 );
	HavokRagdollTemplate* FindRagdollTemplate( Model *model, String fileName );
	HavokRagdollTemplate* CreateRagdollTemplate( Model *model, const char* fileName );
	HavokRagdoll* CreateRagdollFromTemplate( HavokRagdollTemplate* ragdollTemplate, Model *model, RagdollSkeleton* ragdollSkeleton, float scale, int collisionGroup = -1 );
	void RemoveRagdoll( HavokRagdoll* ragdoll );

	Model* GetModelByConstraint( void* constraint );

	// Havok-imported methods for loading and saving MOPP codes
	hkMoppCode* LoadMOPPCode( String path );
	void SaveMOPPCode( String path, hkMoppCode* code );

	void StaticMeshEntityCheck( void );

#ifdef HAVOK3
	static void HK_CALL scaleMassProperties( hkReal scale, hkMassProperties& massProperties );
#if defined( HAVOK3 ) && defined ( USE_HAVOK_DEBUGGER ) && !defined( TRANSGAMING )
	hkVisualDebugger* setupVisualDebugger( hkPhysicsContext* physicsWorld );
	void stepVisualDebugger();
	void SaveSnapshot();
	void writeSimpleMemoryStatistics();
#endif // USE_HAVOK_DEBUGGER
#endif

protected:
	void ForceActivateBodies( void );
	void RemovePinnedActiveMesh( int meshIndex, bool dontDeleteEntity = false);
	void RemoveFarawayBodies( void );
	hkRigidBody* CreatePawnBodyFromRagdollTemplate( HavokRagdollTemplate* ragdollTemplate, float scale, float& mVolumeMass, int collisionGroup );

#if defined( USE_HAVOK_DEBUGGER ) && !defined( TRANSGAMING )
	hkVisualDebugger*					mVisualDebugger;
#endif
	havokBreakableListener*				BreakableListener;
	bool								mIsUnderwater;
	float								mLastTimeDelta;
	int									mRagdollTickCounter;

	// Static Meshes
	DynamicArray<hkRigidBody*>			StaticMeshes;				//	List of static meshes to be destroyed when releasing
	// For load/save to work, those are not added after load, not removed until release.

//	void* mOriginal;					// temp

	// Activation params
	int									PinnedActiveMeshesInitialSize;
	DynamicArray<PinnedActiveMesh>		PinnedActiveMeshes;
	DynamicArray<WorldMesh*>			PinnedActiveMeshParts;
	float								MaxRecursiveActivationDistance;

	HashMap<unsigned char,ActiveMeshGroupActivationParams> AMGActivationParams;

	// Ragdoll data
	DynamicArray<HavokRagdollTemplate*>	RagdollTemplates;
	DynamicArray<HavokRagdoll*>			Ragdolls;

	// Rigid bodies to pin in world because they've fallen off it
	DynamicArray<hkRigidBody*>			RigidBodiesPosExceeded;

	// For multiplayer prediction
	hkArray<hkEntity*>					MPPredictionEntityBatch;

	// For tick
	float								mTickPart;
	ulong32						mPhysicsFrame;
public:
	// Physics Objects
	DynamicArray<PhysicsObject*>		PhysicsObjects;

	// Active Meshes identifiers
	HashMap<PhysicsObject*,int>			PhysIndices;
	DynamicArray<PhysicsObject*>		PhysArray;

	// Havok world
	hkWorld*							World;

	// My deactivator
	myPhysicsObjectDeactivator*			mDeactivator;
	havokDisappearTrigger*				mDisappearTrigger;

	// For multiplayer prediction
	HavokWorldSavedState				SavedState;

	void MPPredAddRestToWorld( void );
	void MPPredRemoveAllFromWorld( void );
	void MPPredRemoveFromBatch( hkRigidBody* rb, bool addToWorld );

	ulong32 GetPhysicsFrame( void ) { return mPhysicsFrame; };
};
