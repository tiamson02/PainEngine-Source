
#include "Core/FileSystem/ConfigFile.h"

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

#include "Core/SimpleProfiler.h"
extern ENGINE_API SimpleProfiler GProfiler;

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

#include "Physics/Havok/HavokWorld.h"
#include "Physics/Havok/HavokPhysics.h"
#include "Physics/Havok/POHavokPart.h"
#include "Physics/Havok/HavokRagdoll.h"
#include "Engine/Mesh.h"
#include "Physics/PhysicsEngine.h"
#include "Net/NetworkDevice2.h"
#include "Physics/PhysicsWorld.h"	// for HitData

#include "Engine/Model.h"
#include "Engine/World.h"

#include "Engine/PCFSystem.h"
extern ENGINE_API PCFSystem* GEngine;

#include "Core/FileSystem/FileManagerStandard.h"
extern ENGINE_API FileManagerStd GFileMan;

#include "Physics/Havok/HavokCharacterController.h"
#include "Physics/Havok/HavokMissileController.h"

#define FIXED_BONES_SUBSTRING "fixed"
#define MASS_FROM_HKE_SUBSTRING "getmass"
#define ACTIVE_MESHES_RECURSIVE_ACTIVATION_EPSILON 0.005f

#define RAY_BODY
#define HAVOK_WORLD_SIZE 3000.0f

#define RAGDOLL_SWEEP_SPEED 10
#define SPHERE_SWEEP_SPEED 6

#ifndef FINAL_BUILD
#define GENERATE_RDE_FILES
#endif

#ifdef TRANSGAMING_PS2
#include "tg_pc2ps2.h"
#endif

#ifdef TRANSGAMING_PSP
#include "tg_pc2psp.h"
#endif

#ifdef TRANSGAMING_WINDOWS
#include "../TNG_WIN/TNG_stdio_WIN.h"
#endif

inline HavokWorldConvexEntry* HavokWorldConvexCache::GetEntry( String& filename, String& meshname, bool centered )
{
	String key = filename;
	key += ":";
	key += meshname;
	if( centered )
		key += ":C";
	int* index = EntryCache.Find( key );
	return index?(&(Entries[*index])):0;
}

inline void HavokWorldConvexCache::PutEntry( String& filename, String& meshname, hkArray<hkVector4>& planeEquations, hkArray<hkVector4>& vertsOut, float scale, bool centered )
{
	String key = filename;
	key += ":";
	key += meshname;
	if( centered )
		key += ":C";
#ifdef _DEBUG
	int* index = EntryCache.Find( key );
	Assert( index == 0 );
#endif
	int ind = Entries.GrowArray(1);
	EntryCache.Insert( key, ind );
    m_setTag("Physics");
	HavokWorldConvexEntry* entry = new (&Entries[ind]) HavokWorldConvexEntry;
	entry->planeEquations = planeEquations;
	entry->vertsOut = vertsOut;
	entry->scale = scale;
}


unsigned short *generateDummyIndex(int count, unsigned short base = 0){
    m_setTag("Physics");
    unsigned short *index = new unsigned short[count];


    for (int i = 0; i < count; i++)
        index[i] = i + base;


    return index;
}

unsigned short *destroyDummyIndex(unsigned short *index){
    //if (index)
    //    delete []index;

    return NULL;
}
// -----------------------------------------------------------------

//static int secondTime = 1;

HavokWorld::HavokWorld( float deactDelay, float deactMaxPosDiff )
:	mIsUnderwater( false ), mLastTimeDelta( 0.1f ), mTickPart(0.0f), mPhysicsFrame(0)
#if defined ( USE_HAVOK_DEBUGGER ) && !defined( TRANSGAMING )
,	mVisualDebugger( 0 )
#endif
,	MaxRecursiveActivationDistance( 3000.0f ), PinnedActiveMeshesInitialSize(0)
,	mRagdollTickCounter(0)
{
#ifdef HAVOK3
	hkWorldCinfo info;
	info.setupSolverInfo( hkWorldCinfo::SOLVER_TYPE_4ITERS_MEDIUM );
	// BLAIR
	info.m_broadPhaseBorderBehaviour = info.BROADPHASE_BORDER_DO_NOTHING;
    //info.m_simulationType = info.SIMULATION_TYPE_HALFSTEP;
#else
	hkWorldConstructionInfo info;
	info.setupSolverInfo( HK_SOLVER_TYPE_4ITERS_MEDIUM );
#endif
	info.setBroadPhaseWorldSize( HAVOK_WORLD_SIZE );
	info.m_collisionTolerance = 0.1f;
    m_setTag("Physics");
	World = new hkWorld( info );

#if defined( HAVOK3 ) && defined ( USE_HAVOK_DEBUGGER ) && !defined( TRANSGAMING )
	// ** SETUP THE VISUAL DEBUGGER **
    m_setTag("Physics");
	hkPhysicsContext* context = new hkPhysicsContext;
	hkPhysicsContext::registerAllPhysicsProcesses(); // all the physics viewers
	context->addWorld(World); // add the physics world so the viewers can see it
	mVisualDebugger = setupVisualDebugger(context);
#endif // USE_HAVOK_DEBUGGER
	// Agent registration

	//
	//	Warning: order is important, entries at the bottom have higher priority
	//

	hkCollisionDispatcher* dis = World->getCollisionDispatcher();

	//
	//	Unary agents 
	//
#ifdef HAVOK3
	// Blair - Let Havok calculate which agents we need.  Missing some agents
	// is what had our player/skeletons falling through the floor.
	hkAgentRegisterUtil::registerAllAgents(dis);
#else
	hkBvTreeShapeAgent::registerAgent(dis);
	hkBvShapeAgent::registerAgent(dis);
	hkListAgent::registerAgent(dis);
	hkShapeCollectionAgent::registerAgent(dis);
	
	hkPhantomAgent::registerAgent(dis);

	// should have highest priority
	hkTransformAgent::registerAgent(dis);	

	//
	//	Terminal agents: secondary type
	//

	hkConvexConvexAgent::registerAgent(dis);
	//
	//	Special agents: primary type
	//
	hkBoxBoxAgent::registerAgent(dis);
	hkSphereSphereAgent::registerAgent(dis);
//	hkSphereTriangleAgent::registerAgent(dis);
	hkUserSphereTriangleAgent::registerAgent(dis);
	hkSphereBoxAgent::registerAgent(dis);
//	hkMultiSphereTriangleAgent::registerAgent(dis);
	hkUserMultiSphereTriangleAgent::registerAgent(dis);
	hkMultiRayConvexAgent::registerAgent(dis);
	hkAabbCastConvexAgent::registerAgent(dis);
	hkAabbCastAabbCastAgent::registerAgent(dis);

	//
	// Convex "welder" agents - used for removing internal collisions in landscapes
	//
	hkConvexWelderConvexAgent::registerAgent(dis);
	hkConvexWelderSphereAgent::registerAgent(dis);

	hkConvexSweepConvexAgent::registerAgent(dis);
#endif
	// Bullet vs paper problem fix
#ifndef HAVOK3 // ================================
	// BLAIR - unsupported...
	World->enableBackstepping();

#ifdef HAVOK212
	hkClipVelocityListener::create( World );
#else
	hkClipVelocityListener::create( World, 15 );
#endif

#endif // ==============================================
/*
	if( !secondTime )
	{
		mOriginal = (void*)dis->m_defaultCollisionAgent;
		dis->m_defaultCollisionAgent = hkDispatchFailureAgent::createDispatchFailureAgent;
	}
*/
	// Group Collision filter
    m_setTag("Physics");
	hkGroupFilter* gf = new hkGroupFilter();
	gf->enableCollisionsUsingBitfield( 0xffffffff,0xffffffff );

	// HCGFixed collisions ( Static Geometry )
	gf->disableCollisionsBetween( HCGFixed , HCGFixed );
	gf->disableCollisionsBetween( HCGFixed , HCGPhantom );
	gf->disableCollisionsBetween( HCGFixed , HCGRagdoll );
	gf->disableCollisionsBetween( HCGFixed , HCGNoncolliding );
	gf->disableCollisionsBetween( HCGFixed , HCGBarrier );
	gf->disableCollisionsBetween( HCGFixed , HCGFixedSpecial );
	gf->disableCollisionsBetween( HCGFixed , HCGOnlyWithFixedSpecial );
	gf->disableCollisionsBetween( HCGFixed , HCGMonsterBarrier );

	// HCGPhantom collisions ( Regions and Player step check )
	gf->disableCollisionsBetween( HCGPhantom , HCGFixed );
	gf->disableCollisionsBetween( HCGPhantom , HCGPhantom );
	gf->disableCollisionsBetween( HCGPhantom , HCGRagdoll );
	gf->disableCollisionsBetween( HCGPhantom , HCGMissile );
	gf->disableCollisionsBetween( HCGPhantom , HCGNormalNCWithSelf );
	gf->disableCollisionsBetween( HCGPhantom , HCGNoncolliding );
	gf->disableCollisionsBetween( HCGPhantom , HCGParticles );
	gf->disableCollisionsBetween( HCGPhantom , HCGBarrier );
	gf->disableCollisionsBetween( HCGPhantom , HCGFixedSpecial );
	gf->disableCollisionsBetween( HCGPhantom , HCGOnlyWithFixedSpecial );
	gf->disableCollisionsBetween( HCGPhantom , HCGMonsterBarrier );
	gf->disableCollisionsBetween( HCGPhantom , HCGClientGrenade );
	gf->disableCollisionsBetween( HCGPhantom , HCGPlayerBodyNoRegions );

	// HCGNormal collisions ( ActiveMeshes, Meshes )
	gf->disableCollisionsBetween( HCGNormal , HCGRagdoll );
	gf->disableCollisionsBetween( HCGNormal , HCGNoncolliding );
	gf->disableCollisionsBetween( HCGNormal , HCGInsideItems );
	gf->disableCollisionsBetween( HCGNormal , HCGBarrier );
	gf->disableCollisionsBetween( HCGNormal , HCGOnlyWithFixedSpecial );
	gf->disableCollisionsBetween( HCGNormal , HCGMonsterBarrier );

	// HCGBody collisions ( Monster spheres, Ravens, rocket & grenade initially )
	gf->disableCollisionsBetween( HCGBody , HCGMissile );
	gf->disableCollisionsBetween( HCGBody , HCGRagdoll );
	gf->disableCollisionsBetween( HCGBody , HCGNoncolliding );
	gf->disableCollisionsBetween( HCGBody , HCGParticles );
	gf->disableCollisionsBetween( HCGBody , HCGInsideItems );
	gf->disableCollisionsBetween( HCGBody , HCGActiveRagdollSpecial );
	gf->disableCollisionsBetween( HCGBody , HCGBarrier );
	gf->disableCollisionsBetween( HCGBody , HCGOnlyWithFixedSpecial );
	gf->disableCollisionsBetween( HCGBody , HCGClientGrenade );

	// HCGRagdoll collisions ( inactive, pinned, keyframed ragdolls )
	gf->disableCollisionsBetween( HCGRagdoll , HCGFixed );
	gf->disableCollisionsBetween( HCGRagdoll , HCGPhantom );
	gf->disableCollisionsBetween( HCGRagdoll , HCGNormal );
	gf->disableCollisionsBetween( HCGRagdoll , HCGBody );
	gf->disableCollisionsBetween( HCGRagdoll , HCGRagdoll );
	gf->disableCollisionsBetween( HCGRagdoll , HCGNoncolliding );
	gf->disableCollisionsBetween( HCGRagdoll , HCGParticles );
	gf->disableCollisionsBetween( HCGRagdoll , HCGInsideItems );
	gf->disableCollisionsBetween( HCGRagdoll , HCGBarrier );
	gf->disableCollisionsBetween( HCGRagdoll , HCGPlayerBody );
	gf->disableCollisionsBetween( HCGRagdoll , HCGFixedSpecial );
	gf->disableCollisionsBetween( HCGRagdoll , HCGOnlyWithFixedSpecial );
	gf->disableCollisionsBetween( HCGRagdoll , HCGMonsterBarrier );
	gf->disableCollisionsBetween( HCGRagdoll , HCGClientGrenade );
	gf->disableCollisionsBetween( HCGRagdoll , HCGPlayerBodyNoRegions );

	// HCGMissile collisions ( rocket & grenade, axe ... )
	gf->disableCollisionsBetween( HCGMissile , HCGPhantom );
	gf->disableCollisionsBetween( HCGMissile , HCGBody );
	gf->disableCollisionsBetween( HCGMissile , HCGMissile );
	gf->disableCollisionsBetween( HCGMissile , HCGNoncolliding );
	gf->disableCollisionsBetween( HCGMissile , HCGParticles );
	gf->disableCollisionsBetween( HCGMissile , HCGInsideItems );
	gf->disableCollisionsBetween( HCGMissile , HCGBarrier );
	gf->disableCollisionsBetween( HCGMissile , HCGPlayerBody );
	gf->disableCollisionsBetween( HCGMissile , HCGOnlyWithFixedSpecial );
	gf->disableCollisionsBetween( HCGMissile , HCGMonsterBarrier );
	gf->disableCollisionsBetween( HCGMissile , HCGClientGrenade );
	gf->disableCollisionsBetween( HCGMissile , HCGPlayerBodyNoRegions );

	// HCGNormalNCWithSelf collisions ( CrossPieces samurajv2) ale nie koliduje z Body
	gf->disableCollisionsBetween( HCGNormalNCWithSelf , HCGPhantom );
	gf->disableCollisionsBetween( HCGNormalNCWithSelf , HCGNoncolliding );
	gf->disableCollisionsBetween( HCGNormalNCWithSelf , HCGBody );
	gf->disableCollisionsBetween( HCGNormalNCWithSelf , HCGParticles );
	gf->disableCollisionsBetween( HCGNormalNCWithSelf , HCGInsideItems );
	gf->disableCollisionsBetween( HCGNormalNCWithSelf , HCGBarrier );
	gf->disableCollisionsBetween( HCGNormalNCWithSelf , HCGOnlyWithFixedSpecial );
	gf->disableCollisionsBetween( HCGNormalNCWithSelf , HCGNormalNCWithSelf );
	gf->disableCollisionsBetween( HCGNormalNCWithSelf , HCGMonsterBarrier );

	// HCGNormalBodyNCWithSelf collisions (tank) tak jak HCGNormalNCWithSelf, ale tez z Body
	gf->disableCollisionsBetween( HCGNormalBodyNCWithSelf , HCGPhantom );
	gf->disableCollisionsBetween( HCGNormalBodyNCWithSelf , HCGNoncolliding );
	//gf->disableCollisionsBetween( HCGNormalBodyNCWithSelf , HCGBody );
	gf->disableCollisionsBetween( HCGNormalBodyNCWithSelf , HCGParticles );
	gf->disableCollisionsBetween( HCGNormalBodyNCWithSelf , HCGInsideItems );
	gf->disableCollisionsBetween( HCGNormalBodyNCWithSelf , HCGBarrier );
	gf->disableCollisionsBetween( HCGNormalBodyNCWithSelf , HCGOnlyWithFixedSpecial );
	gf->disableCollisionsBetween( HCGNormalBodyNCWithSelf , HCGNormalBodyNCWithSelf );
	//gf->disableCollisionsBetween( HCGNormalNCWithSelf , HCGMonsterBarrier );


	// HCGNoncolliding collisions ( collides with nothing )
	gf->disableCollisionsBetween( HCGNoncolliding , HCGFixed );
	gf->disableCollisionsBetween( HCGNoncolliding , HCGPhantom );
	gf->disableCollisionsBetween( HCGNoncolliding , HCGNormal );
	gf->disableCollisionsBetween( HCGNoncolliding , HCGBody );
	gf->disableCollisionsBetween( HCGNoncolliding , HCGMissile );
	gf->disableCollisionsBetween( HCGNoncolliding , HCGRagdoll );
	gf->disableCollisionsBetween( HCGNoncolliding , HCGNoncolliding );
	gf->disableCollisionsBetween( HCGNoncolliding , HCGParticles );
	gf->disableCollisionsBetween( HCGNoncolliding , HCGInsideItems );
	gf->disableCollisionsBetween( HCGNoncolliding , HCGActiveRagdoll );
	gf->disableCollisionsBetween( HCGNoncolliding , HCGActiveRagdoll2 );
	gf->disableCollisionsBetween( HCGNoncolliding , HCGActiveRagdoll3 );
	gf->disableCollisionsBetween( HCGNoncolliding , HCGActiveRagdoll4 );
	gf->disableCollisionsBetween( HCGNoncolliding , HCGActiveRagdoll5 );
	gf->disableCollisionsBetween( HCGNoncolliding , HCGActiveRagdoll6 );
	gf->disableCollisionsBetween( HCGNoncolliding , HCGActiveRagdoll7 );
	gf->disableCollisionsBetween( HCGNoncolliding , HCGActiveRagdoll8 );
	gf->disableCollisionsBetween( HCGNoncolliding , HCGActiveRagdoll9 );
	gf->disableCollisionsBetween( HCGNoncolliding , HCGActiveRagdoll0 );
	gf->disableCollisionsBetween( HCGNoncolliding , HCGActiveRagdollColliding );
	gf->disableCollisionsBetween( HCGNoncolliding , HCGActiveRagdollSpecial );
	gf->disableCollisionsBetween( HCGNoncolliding , HCGBarrier );
	gf->disableCollisionsBetween( HCGNoncolliding , HCGPlayerBody );
	gf->disableCollisionsBetween( HCGNoncolliding , HCGFixedSpecial );
	gf->disableCollisionsBetween( HCGNoncolliding , HCGOnlyWithFixedSpecial );
	gf->disableCollisionsBetween( HCGNoncolliding , HCGNormalNCWithSelf );
	gf->disableCollisionsBetween( HCGNoncolliding , HCGMonsterBarrier );
	gf->disableCollisionsBetween( HCGNoncolliding , HCGClientGrenade );
	gf->disableCollisionsBetween( HCGNoncolliding , HCGPlayerBodyNoRegions );

	// HCGFixedWithNoncollidable collisions ( fixed, ale koliduje z noncollidable)
	gf->disableCollisionsBetween( HCGFixedWithNoncollidable , HCGFixedWithNoncollidable );
	gf->disableCollisionsBetween( HCGFixedWithNoncollidable , HCGFixed );
	gf->disableCollisionsBetween( HCGFixedWithNoncollidable , HCGPhantom );
	gf->disableCollisionsBetween( HCGFixedWithNoncollidable , HCGRagdoll );
	gf->disableCollisionsBetween( HCGFixedWithNoncollidable , HCGBarrier );
	gf->disableCollisionsBetween( HCGFixedWithNoncollidable , HCGFixedSpecial );
	gf->disableCollisionsBetween( HCGFixedWithNoncollidable , HCGOnlyWithFixedSpecial );
	gf->disableCollisionsBetween( HCGFixedWithNoncollidable , HCGMonsterBarrier );

	// HCGParticles collisions ( only with fixed )
	gf->disableCollisionsBetween( HCGParticles , HCGPhantom );
	gf->disableCollisionsBetween( HCGParticles , HCGBody );
	gf->disableCollisionsBetween( HCGParticles , HCGMissile );
	gf->disableCollisionsBetween( HCGParticles , HCGRagdoll );
	 gf->disableCollisionsBetween( HCGParticles , HCGNormal );
	gf->disableCollisionsBetween( HCGParticles , HCGNoncolliding );
	gf->disableCollisionsBetween( HCGParticles , HCGParticles );
	gf->disableCollisionsBetween( HCGParticles , HCGInsideItems );
	gf->disableCollisionsBetween( HCGParticles , HCGActiveRagdoll );
	gf->disableCollisionsBetween( HCGParticles , HCGActiveRagdoll2 );
	gf->disableCollisionsBetween( HCGParticles , HCGActiveRagdoll3 );
	gf->disableCollisionsBetween( HCGParticles , HCGActiveRagdoll4 );
	gf->disableCollisionsBetween( HCGParticles , HCGActiveRagdoll5 );
	gf->disableCollisionsBetween( HCGParticles , HCGActiveRagdoll6 );
	gf->disableCollisionsBetween( HCGParticles , HCGActiveRagdoll7 );
	gf->disableCollisionsBetween( HCGParticles , HCGActiveRagdoll8 );
	gf->disableCollisionsBetween( HCGParticles , HCGActiveRagdoll9 );
	gf->disableCollisionsBetween( HCGParticles , HCGActiveRagdoll0 );
	gf->disableCollisionsBetween( HCGParticles , HCGActiveRagdollColliding );
	gf->disableCollisionsBetween( HCGParticles , HCGActiveRagdollSpecial );
	gf->disableCollisionsBetween( HCGParticles , HCGBarrier );
	gf->disableCollisionsBetween( HCGParticles , HCGPlayerBody );
	gf->disableCollisionsBetween( HCGParticles , HCGOnlyWithFixedSpecial );
	gf->disableCollisionsBetween( HCGParticles , HCGNormalNCWithSelf );
	gf->disableCollisionsBetween( HCGParticles , HCGMonsterBarrier );
	gf->disableCollisionsBetween( HCGParticles , HCGClientGrenade );
	gf->disableCollisionsBetween( HCGParticles , HCGPlayerBodyNoRegions );

	// HCGInsideItems collisions ( items inside the items, jumping out after items' destruction )
	gf->disableCollisionsBetween( HCGInsideItems , HCGNormal );
	gf->disableCollisionsBetween( HCGInsideItems , HCGBody );
	gf->disableCollisionsBetween( HCGInsideItems , HCGNoncolliding );
	gf->disableCollisionsBetween( HCGInsideItems , HCGRagdoll );
	gf->disableCollisionsBetween( HCGInsideItems , HCGMissile );
	gf->disableCollisionsBetween( HCGInsideItems , HCGParticles );
	gf->disableCollisionsBetween( HCGInsideItems , HCGInsideItems );
	gf->disableCollisionsBetween( HCGInsideItems , HCGActiveRagdoll );
	gf->disableCollisionsBetween( HCGInsideItems , HCGActiveRagdoll2 );
	gf->disableCollisionsBetween( HCGInsideItems , HCGActiveRagdoll3 );
	gf->disableCollisionsBetween( HCGInsideItems , HCGActiveRagdoll4 );
	gf->disableCollisionsBetween( HCGInsideItems , HCGActiveRagdoll5 );
	gf->disableCollisionsBetween( HCGInsideItems , HCGActiveRagdoll6 );
	gf->disableCollisionsBetween( HCGInsideItems , HCGActiveRagdoll7 );
	gf->disableCollisionsBetween( HCGInsideItems , HCGActiveRagdoll8 );
	gf->disableCollisionsBetween( HCGInsideItems , HCGActiveRagdoll9 );
	gf->disableCollisionsBetween( HCGInsideItems , HCGActiveRagdoll0 );
	gf->disableCollisionsBetween( HCGInsideItems , HCGActiveRagdollColliding );
	gf->disableCollisionsBetween( HCGInsideItems , HCGActiveRagdollSpecial );
	gf->disableCollisionsBetween( HCGInsideItems , HCGPlayerBody );
	gf->disableCollisionsBetween( HCGInsideItems , HCGBarrier );
	gf->disableCollisionsBetween( HCGInsideItems , HCGOnlyWithFixedSpecial );
	gf->disableCollisionsBetween( HCGInsideItems , HCGNormalNCWithSelf );
	gf->disableCollisionsBetween( HCGInsideItems , HCGClientGrenade );
	gf->disableCollisionsBetween( HCGInsideItems , HCGPlayerBodyNoRegions );

	// Active, non-self-colliding ragdolls collisions
	gf->disableCollisionsBetween( HCGActiveRagdoll , HCGActiveRagdoll );
	gf->disableCollisionsBetween( HCGActiveRagdoll , HCGNoncolliding );
	gf->disableCollisionsBetween( HCGActiveRagdoll , HCGParticles );
	gf->disableCollisionsBetween( HCGActiveRagdoll , HCGInsideItems );
	gf->disableCollisionsBetween( HCGActiveRagdoll , HCGActiveRagdollSpecial );
	gf->disableCollisionsBetween( HCGActiveRagdoll , HCGBarrier );
	gf->disableCollisionsBetween( HCGActiveRagdoll , HCGOnlyWithFixedSpecial );
	gf->disableCollisionsBetween( HCGActiveRagdoll , HCGMonsterBarrier );

	gf->disableCollisionsBetween( HCGActiveRagdoll2 , HCGActiveRagdoll2 );
	gf->disableCollisionsBetween( HCGActiveRagdoll2 , HCGNoncolliding );
	gf->disableCollisionsBetween( HCGActiveRagdoll2 , HCGParticles );
	gf->disableCollisionsBetween( HCGActiveRagdoll2 , HCGInsideItems );
	gf->disableCollisionsBetween( HCGActiveRagdoll2 , HCGActiveRagdollSpecial );
	gf->disableCollisionsBetween( HCGActiveRagdoll2 , HCGBarrier );
	gf->disableCollisionsBetween( HCGActiveRagdoll2 , HCGOnlyWithFixedSpecial );
	gf->disableCollisionsBetween( HCGActiveRagdoll2 , HCGMonsterBarrier );

	gf->disableCollisionsBetween( HCGActiveRagdoll3 , HCGActiveRagdoll3 );
	gf->disableCollisionsBetween( HCGActiveRagdoll3 , HCGNoncolliding );
	gf->disableCollisionsBetween( HCGActiveRagdoll3 , HCGParticles );
	gf->disableCollisionsBetween( HCGActiveRagdoll3 , HCGInsideItems );
	gf->disableCollisionsBetween( HCGActiveRagdoll3 , HCGActiveRagdollSpecial );
	gf->disableCollisionsBetween( HCGActiveRagdoll3 , HCGBarrier );
	gf->disableCollisionsBetween( HCGActiveRagdoll3 , HCGOnlyWithFixedSpecial );
	gf->disableCollisionsBetween( HCGActiveRagdoll3 , HCGMonsterBarrier );

	gf->disableCollisionsBetween( HCGActiveRagdoll4 , HCGActiveRagdoll4 );
	gf->disableCollisionsBetween( HCGActiveRagdoll4 , HCGNoncolliding );
	gf->disableCollisionsBetween( HCGActiveRagdoll4 , HCGParticles );
	gf->disableCollisionsBetween( HCGActiveRagdoll4 , HCGInsideItems );
	gf->disableCollisionsBetween( HCGActiveRagdoll4 , HCGActiveRagdollSpecial );
	gf->disableCollisionsBetween( HCGActiveRagdoll4 , HCGBarrier );
	gf->disableCollisionsBetween( HCGActiveRagdoll4 , HCGOnlyWithFixedSpecial );
	gf->disableCollisionsBetween( HCGActiveRagdoll4 , HCGMonsterBarrier );

	gf->disableCollisionsBetween( HCGActiveRagdoll5 , HCGActiveRagdoll5 );
	gf->disableCollisionsBetween( HCGActiveRagdoll5 , HCGNoncolliding );
	gf->disableCollisionsBetween( HCGActiveRagdoll5 , HCGParticles );
	gf->disableCollisionsBetween( HCGActiveRagdoll5 , HCGInsideItems );
	gf->disableCollisionsBetween( HCGActiveRagdoll5 , HCGActiveRagdollSpecial );
	gf->disableCollisionsBetween( HCGActiveRagdoll5 , HCGBarrier );
	gf->disableCollisionsBetween( HCGActiveRagdoll5 , HCGOnlyWithFixedSpecial );
	gf->disableCollisionsBetween( HCGActiveRagdoll5 , HCGMonsterBarrier );

	gf->disableCollisionsBetween( HCGActiveRagdoll6 , HCGActiveRagdoll6 );
	gf->disableCollisionsBetween( HCGActiveRagdoll6 , HCGNoncolliding );
	gf->disableCollisionsBetween( HCGActiveRagdoll6 , HCGParticles );
	gf->disableCollisionsBetween( HCGActiveRagdoll6 , HCGInsideItems );
	gf->disableCollisionsBetween( HCGActiveRagdoll6 , HCGActiveRagdollSpecial );
	gf->disableCollisionsBetween( HCGActiveRagdoll6 , HCGBarrier );
	gf->disableCollisionsBetween( HCGActiveRagdoll6 , HCGOnlyWithFixedSpecial );
	gf->disableCollisionsBetween( HCGActiveRagdoll6 , HCGMonsterBarrier );

	gf->disableCollisionsBetween( HCGActiveRagdoll7 , HCGActiveRagdoll7 );
	gf->disableCollisionsBetween( HCGActiveRagdoll7 , HCGNoncolliding );
	gf->disableCollisionsBetween( HCGActiveRagdoll7 , HCGParticles );
	gf->disableCollisionsBetween( HCGActiveRagdoll7 , HCGInsideItems );
	gf->disableCollisionsBetween( HCGActiveRagdoll7 , HCGActiveRagdollSpecial );
	gf->disableCollisionsBetween( HCGActiveRagdoll7 , HCGBarrier );
	gf->disableCollisionsBetween( HCGActiveRagdoll7 , HCGOnlyWithFixedSpecial );
	gf->disableCollisionsBetween( HCGActiveRagdoll7 , HCGMonsterBarrier );

	gf->disableCollisionsBetween( HCGActiveRagdoll8 , HCGActiveRagdoll8 );
	gf->disableCollisionsBetween( HCGActiveRagdoll8 , HCGNoncolliding );
	gf->disableCollisionsBetween( HCGActiveRagdoll8 , HCGParticles );
	gf->disableCollisionsBetween( HCGActiveRagdoll8 , HCGInsideItems );
	gf->disableCollisionsBetween( HCGActiveRagdoll8 , HCGActiveRagdollSpecial );
	gf->disableCollisionsBetween( HCGActiveRagdoll8 , HCGBarrier );
	gf->disableCollisionsBetween( HCGActiveRagdoll8 , HCGOnlyWithFixedSpecial );
	gf->disableCollisionsBetween( HCGActiveRagdoll8 , HCGMonsterBarrier );

	gf->disableCollisionsBetween( HCGActiveRagdoll9 , HCGActiveRagdoll9 );
	gf->disableCollisionsBetween( HCGActiveRagdoll9 , HCGNoncolliding );
	gf->disableCollisionsBetween( HCGActiveRagdoll9 , HCGParticles );
	gf->disableCollisionsBetween( HCGActiveRagdoll9 , HCGInsideItems );
	gf->disableCollisionsBetween( HCGActiveRagdoll9 , HCGActiveRagdollSpecial );
	gf->disableCollisionsBetween( HCGActiveRagdoll9 , HCGBarrier );
	gf->disableCollisionsBetween( HCGActiveRagdoll9 , HCGOnlyWithFixedSpecial );
	gf->disableCollisionsBetween( HCGActiveRagdoll9 , HCGMonsterBarrier );

	gf->disableCollisionsBetween( HCGActiveRagdoll0 , HCGActiveRagdoll0 );
	gf->disableCollisionsBetween( HCGActiveRagdoll0 , HCGNoncolliding );
	gf->disableCollisionsBetween( HCGActiveRagdoll0 , HCGParticles );
	gf->disableCollisionsBetween( HCGActiveRagdoll0 , HCGInsideItems );
	gf->disableCollisionsBetween( HCGActiveRagdoll0 , HCGActiveRagdollSpecial );
	gf->disableCollisionsBetween( HCGActiveRagdoll0 , HCGBarrier );
	gf->disableCollisionsBetween( HCGActiveRagdoll0 , HCGOnlyWithFixedSpecial );
	gf->disableCollisionsBetween( HCGActiveRagdoll0 , HCGMonsterBarrier );

	// HCGActiveRagdollColliding collisions
	gf->disableCollisionsBetween( HCGActiveRagdollColliding , HCGNoncolliding );
	gf->disableCollisionsBetween( HCGActiveRagdollColliding , HCGParticles );
	gf->disableCollisionsBetween( HCGActiveRagdollColliding , HCGInsideItems );
	gf->disableCollisionsBetween( HCGActiveRagdollColliding , HCGActiveRagdollSpecial );
	gf->disableCollisionsBetween( HCGActiveRagdollColliding , HCGBarrier );
	gf->disableCollisionsBetween( HCGActiveRagdollColliding , HCGOnlyWithFixedSpecial );
	gf->disableCollisionsBetween( HCGActiveRagdollColliding , HCGMonsterBarrier );

	// HCGActiveRagdollSpecial collisions
	gf->disableCollisionsBetween( HCGActiveRagdollSpecial , HCGBody );
	gf->disableCollisionsBetween( HCGActiveRagdollSpecial , HCGRagdoll );
	gf->disableCollisionsBetween( HCGActiveRagdollSpecial , HCGNoncolliding );
	gf->disableCollisionsBetween( HCGActiveRagdollSpecial , HCGParticles );
	gf->disableCollisionsBetween( HCGActiveRagdollSpecial , HCGInsideItems );
	gf->disableCollisionsBetween( HCGActiveRagdollSpecial , HCGActiveRagdoll );
	gf->disableCollisionsBetween( HCGActiveRagdollSpecial , HCGActiveRagdoll2 );
	gf->disableCollisionsBetween( HCGActiveRagdollSpecial , HCGActiveRagdoll3 );
	gf->disableCollisionsBetween( HCGActiveRagdollSpecial , HCGActiveRagdoll4 );
	gf->disableCollisionsBetween( HCGActiveRagdollSpecial , HCGActiveRagdoll5 );
	gf->disableCollisionsBetween( HCGActiveRagdollSpecial , HCGActiveRagdoll6 );
	gf->disableCollisionsBetween( HCGActiveRagdollSpecial , HCGActiveRagdoll7 );
	gf->disableCollisionsBetween( HCGActiveRagdollSpecial , HCGActiveRagdoll8 );
	gf->disableCollisionsBetween( HCGActiveRagdollSpecial , HCGActiveRagdoll9 );
	gf->disableCollisionsBetween( HCGActiveRagdollSpecial , HCGActiveRagdoll0 );
	gf->disableCollisionsBetween( HCGActiveRagdollSpecial , HCGActiveRagdollColliding );
	gf->disableCollisionsBetween( HCGActiveRagdollSpecial , HCGActiveRagdollSpecial );
	gf->disableCollisionsBetween( HCGActiveRagdollSpecial , HCGBarrier );
	gf->disableCollisionsBetween( HCGActiveRagdollSpecial , HCGPlayerBody );
	gf->disableCollisionsBetween( HCGActiveRagdollSpecial , HCGOnlyWithFixedSpecial );
	gf->disableCollisionsBetween( HCGActiveRagdollSpecial , HCGMonsterBarrier );
	gf->disableCollisionsBetween( HCGActiveRagdollSpecial , HCGPlayerBodyNoRegions );

	// HCGBarrier collisions ( collides only with player body )
	gf->disableCollisionsBetween( HCGBarrier , HCGFixed );
	gf->disableCollisionsBetween( HCGBarrier , HCGPhantom );
	gf->disableCollisionsBetween( HCGBarrier , HCGNormal );
	gf->disableCollisionsBetween( HCGBarrier , HCGBody );
	gf->disableCollisionsBetween( HCGBarrier , HCGMissile );
	gf->disableCollisionsBetween( HCGBarrier , HCGRagdoll );
	gf->disableCollisionsBetween( HCGBarrier , HCGNoncolliding );
	gf->disableCollisionsBetween( HCGBarrier , HCGParticles );
	gf->disableCollisionsBetween( HCGBarrier , HCGActiveRagdoll );
	gf->disableCollisionsBetween( HCGBarrier , HCGActiveRagdoll2 );
	gf->disableCollisionsBetween( HCGBarrier , HCGActiveRagdoll3 );
	gf->disableCollisionsBetween( HCGBarrier , HCGActiveRagdoll4 );
	gf->disableCollisionsBetween( HCGBarrier , HCGActiveRagdoll5 );
	gf->disableCollisionsBetween( HCGBarrier , HCGActiveRagdoll6 );
	gf->disableCollisionsBetween( HCGBarrier , HCGActiveRagdoll7 );
	gf->disableCollisionsBetween( HCGBarrier , HCGActiveRagdoll8 );
	gf->disableCollisionsBetween( HCGBarrier , HCGActiveRagdoll9 );
	gf->disableCollisionsBetween( HCGBarrier , HCGActiveRagdoll0 );
	gf->disableCollisionsBetween( HCGBarrier , HCGActiveRagdollColliding );
	gf->disableCollisionsBetween( HCGBarrier , HCGActiveRagdollSpecial );
	gf->disableCollisionsBetween( HCGBarrier , HCGBarrier );
	gf->disableCollisionsBetween( HCGBarrier , HCGInsideItems );
	gf->disableCollisionsBetween( HCGBarrier , HCGFixedSpecial );
	gf->disableCollisionsBetween( HCGBarrier , HCGOnlyWithFixedSpecial );
	gf->disableCollisionsBetween( HCGBarrier , HCGNormalNCWithSelf );
	gf->disableCollisionsBetween( HCGBarrier , HCGMonsterBarrier );
	gf->disableCollisionsBetween( HCGBarrier , HCGFixedWithNoncollidable );
	gf->disableCollisionsBetween( HCGBarrier , HCGClientGrenade );

	// HCGPlayerBody collisions ( Player spheres )
	gf->disableCollisionsBetween( HCGPlayerBody , HCGMissile );
	gf->disableCollisionsBetween( HCGPlayerBody , HCGRagdoll );
	gf->disableCollisionsBetween( HCGPlayerBody , HCGNoncolliding );
	gf->disableCollisionsBetween( HCGPlayerBody , HCGParticles );
	gf->disableCollisionsBetween( HCGPlayerBody , HCGInsideItems );
	gf->disableCollisionsBetween( HCGPlayerBody , HCGActiveRagdollSpecial );
	gf->disableCollisionsBetween( HCGPlayerBody , HCGOnlyWithFixedSpecial );
	gf->disableCollisionsBetween( HCGPlayerBody , HCGMonsterBarrier );
	gf->disableCollisionsBetween( HCGPlayerBody , HCGClientGrenade );

	// HCGFixedSpecial collisions ( Static Geometry )
	gf->disableCollisionsBetween( HCGFixedSpecial , HCGFixed );
	gf->disableCollisionsBetween( HCGFixedSpecial , HCGPhantom );
	gf->disableCollisionsBetween( HCGFixedSpecial , HCGRagdoll );
	gf->disableCollisionsBetween( HCGFixedSpecial , HCGNoncolliding );
	gf->disableCollisionsBetween( HCGFixedSpecial , HCGBarrier );
	gf->disableCollisionsBetween( HCGFixedSpecial , HCGFixedSpecial );
	gf->disableCollisionsBetween( HCGFixedSpecial , HCGMonsterBarrier );

	// HCGOnlyWithFixedSpecial collisions
	gf->disableCollisionsBetween( HCGOnlyWithFixedSpecial , HCGFixed );
	gf->disableCollisionsBetween( HCGOnlyWithFixedSpecial , HCGPhantom );
	gf->disableCollisionsBetween( HCGOnlyWithFixedSpecial , HCGNormal );
	gf->disableCollisionsBetween( HCGOnlyWithFixedSpecial , HCGBody );
	gf->disableCollisionsBetween( HCGOnlyWithFixedSpecial , HCGMissile );
	gf->disableCollisionsBetween( HCGOnlyWithFixedSpecial , HCGRagdoll );
	gf->disableCollisionsBetween( HCGOnlyWithFixedSpecial , HCGNoncolliding );
	gf->disableCollisionsBetween( HCGOnlyWithFixedSpecial , HCGParticles );
	gf->disableCollisionsBetween( HCGOnlyWithFixedSpecial , HCGActiveRagdoll );
	gf->disableCollisionsBetween( HCGOnlyWithFixedSpecial , HCGActiveRagdoll2 );
	gf->disableCollisionsBetween( HCGOnlyWithFixedSpecial , HCGActiveRagdoll3 );
	gf->disableCollisionsBetween( HCGOnlyWithFixedSpecial , HCGActiveRagdoll4 );
	gf->disableCollisionsBetween( HCGOnlyWithFixedSpecial , HCGActiveRagdoll5 );
	gf->disableCollisionsBetween( HCGOnlyWithFixedSpecial , HCGActiveRagdoll6 );
	gf->disableCollisionsBetween( HCGOnlyWithFixedSpecial , HCGActiveRagdoll7 );
	gf->disableCollisionsBetween( HCGOnlyWithFixedSpecial , HCGActiveRagdoll8 );
	gf->disableCollisionsBetween( HCGOnlyWithFixedSpecial , HCGActiveRagdoll9 );
	gf->disableCollisionsBetween( HCGOnlyWithFixedSpecial , HCGActiveRagdoll0 );
	gf->disableCollisionsBetween( HCGOnlyWithFixedSpecial , HCGActiveRagdollColliding );
	gf->disableCollisionsBetween( HCGOnlyWithFixedSpecial , HCGActiveRagdollSpecial );
	gf->disableCollisionsBetween( HCGOnlyWithFixedSpecial , HCGBarrier );
	gf->disableCollisionsBetween( HCGOnlyWithFixedSpecial , HCGPlayerBody );
	gf->disableCollisionsBetween( HCGOnlyWithFixedSpecial , HCGOnlyWithFixedSpecial );
	gf->disableCollisionsBetween( HCGOnlyWithFixedSpecial , HCGNormalNCWithSelf );
	gf->disableCollisionsBetween( HCGOnlyWithFixedSpecial , HCGMonsterBarrier );
	gf->disableCollisionsBetween( HCGOnlyWithFixedSpecial , HCGClientGrenade );
	gf->disableCollisionsBetween( HCGOnlyWithFixedSpecial , HCGPlayerBodyNoRegions );

	// HCGMonsterBarrier collisions ( collides only with player body )
	gf->disableCollisionsBetween( HCGMonsterBarrier , HCGFixed );
	gf->disableCollisionsBetween( HCGMonsterBarrier , HCGPhantom );
	gf->disableCollisionsBetween( HCGMonsterBarrier , HCGNormal );
	gf->disableCollisionsBetween( HCGMonsterBarrier , HCGMissile );
	gf->disableCollisionsBetween( HCGMonsterBarrier , HCGRagdoll );
	gf->disableCollisionsBetween( HCGMonsterBarrier , HCGNoncolliding );
	gf->disableCollisionsBetween( HCGMonsterBarrier , HCGParticles );
	gf->disableCollisionsBetween( HCGMonsterBarrier , HCGActiveRagdoll );
	gf->disableCollisionsBetween( HCGMonsterBarrier , HCGActiveRagdoll2 );
	gf->disableCollisionsBetween( HCGMonsterBarrier , HCGActiveRagdoll3 );
	gf->disableCollisionsBetween( HCGMonsterBarrier , HCGActiveRagdoll4 );
	gf->disableCollisionsBetween( HCGMonsterBarrier , HCGActiveRagdoll5 );
	gf->disableCollisionsBetween( HCGMonsterBarrier , HCGActiveRagdoll6 );
	gf->disableCollisionsBetween( HCGMonsterBarrier , HCGActiveRagdoll7 );
	gf->disableCollisionsBetween( HCGMonsterBarrier , HCGActiveRagdoll8 );
	gf->disableCollisionsBetween( HCGMonsterBarrier , HCGActiveRagdoll9 );
	gf->disableCollisionsBetween( HCGMonsterBarrier , HCGActiveRagdoll0 );
	gf->disableCollisionsBetween( HCGMonsterBarrier , HCGActiveRagdollColliding );
	gf->disableCollisionsBetween( HCGMonsterBarrier , HCGActiveRagdollSpecial );
	gf->disableCollisionsBetween( HCGMonsterBarrier , HCGBarrier );
	gf->disableCollisionsBetween( HCGMonsterBarrier , HCGPlayerBody );
	gf->disableCollisionsBetween( HCGMonsterBarrier , HCGFixedSpecial );
	gf->disableCollisionsBetween( HCGMonsterBarrier , HCGOnlyWithFixedSpecial );
	gf->disableCollisionsBetween( HCGMonsterBarrier , HCGMonsterBarrier );
	gf->disableCollisionsBetween( HCGMonsterBarrier , HCGClientGrenade );
	gf->disableCollisionsBetween( HCGMonsterBarrier , HCGPlayerBodyNoRegions );
	gf->disableCollisionsBetween( HCGMonsterBarrier , HCGNormalNCWithSelf );

	// HCGClientGrenade collisions
	gf->disableCollisionsBetween( HCGClientGrenade , HCGPhantom );
	gf->disableCollisionsBetween( HCGClientGrenade , HCGBody );
	gf->disableCollisionsBetween( HCGClientGrenade , HCGMissile );
	gf->disableCollisionsBetween( HCGClientGrenade , HCGRagdoll );
	gf->disableCollisionsBetween( HCGClientGrenade , HCGNoncolliding );
	gf->disableCollisionsBetween( HCGClientGrenade , HCGParticles );
	gf->disableCollisionsBetween( HCGClientGrenade , HCGInsideItems );
	gf->disableCollisionsBetween( HCGClientGrenade , HCGBarrier );
	gf->disableCollisionsBetween( HCGClientGrenade , HCGPlayerBody );
	gf->disableCollisionsBetween( HCGClientGrenade , HCGOnlyWithFixedSpecial );
	gf->disableCollisionsBetween( HCGClientGrenade , HCGMonsterBarrier );
	gf->disableCollisionsBetween( HCGClientGrenade , HCGClientGrenade );
	gf->disableCollisionsBetween( HCGClientGrenade , HCGPlayerBodyNoRegions );

	// HCGPlayerBodyNoRegions collisions ( Player aabb cast shape in player movement v3 )
	gf->disableCollisionsBetween( HCGPlayerBodyNoRegions , HCGPhantom );
	gf->disableCollisionsBetween( HCGPlayerBodyNoRegions , HCGMissile );
	gf->disableCollisionsBetween( HCGPlayerBodyNoRegions , HCGRagdoll );
	gf->disableCollisionsBetween( HCGPlayerBodyNoRegions , HCGNoncolliding );
	gf->disableCollisionsBetween( HCGPlayerBodyNoRegions , HCGParticles );
	gf->disableCollisionsBetween( HCGPlayerBodyNoRegions , HCGInsideItems );
	gf->disableCollisionsBetween( HCGPlayerBodyNoRegions , HCGActiveRagdollSpecial );
	gf->disableCollisionsBetween( HCGPlayerBodyNoRegions , HCGOnlyWithFixedSpecial );
	gf->disableCollisionsBetween( HCGPlayerBodyNoRegions , HCGMonsterBarrier );
	gf->disableCollisionsBetween( HCGPlayerBodyNoRegions , HCGClientGrenade );

	World->setCollisionFilter( gf );
	gf->removeReference();

    m_setTag("Physics");
	havokWorldBorder* border = new havokWorldBorder( World, HAVOK_WORLD_SIZE/2 );
	border->removeReference();

    m_setTag("Physics");
	BreakableListener = new havokBreakableListener;

    m_setTag("Physics");
	mDeactivator = new myPhysicsObjectDeactivator( deactDelay, deactMaxPosDiff );

    m_setTag("Physics");
	mDisappearTrigger = new havokDisappearTrigger;

#ifdef USE_HAVOK_DEBUGGER
#ifndef HAVOK3 // ============ Not supported....
	mVisualDebugger = hkVisualDebugger::create( World );
	mVisualDebugger->addViewer("Islands");
	mVisualDebugger->addViewer("Broadphase");
	mVisualDebugger->addViewer("Statistics");
	mVisualDebugger->addViewer("Tensor");
	mVisualDebugger->addViewer("CentreOfMass");
	mVisualDebugger->addViewer("ContactPoints");
	mVisualDebugger->serve();	// port number
#endif // ifndef HAVOK3 ==============
//	mVisualDebugger->capture( "falling.hkm" );
#endif
}

HavokWorld::~HavokWorld()
{
	delete mDeactivator;
	mDeactivator = 0;

	PinnedActiveMeshes.ClearArray();
	PhysIndices.Clear();
	PhysArray.ClearArray();

	for( int i = 0; i < RagdollTemplates.Size(); ++i )
		delete RagdollTemplates[i];
	RagdollTemplates.ClearArray();

	Assert( !Ragdolls.Size() );

	while( PhysicsObjects.Size() )
		delete PhysicsObjects[0];

	while( World->getPhantoms().getSize() > 0 )
		World->removePhantom( World->getPhantoms()[0] );

	delete BreakableListener;
	BreakableListener = 0;

	delete mDisappearTrigger;
	mDisappearTrigger = 0;

#if defined ( USE_HAVOK_DEBUGGER ) && !defined( TRANSGAMING )
	mVisualDebugger->removeReference();
#endif

	World->removeReference();
	World = 0;
}

hkRigidBody* HavokWorld::CreateStaticMesh( WorldMesh* mesh, float friction, float restitution, int collisionGroup )
{
	const int   vertexCount = mesh->Data->VertList.Size();
    int         stride;
    FileManagerStd fm;


	if(!vertexCount) return 0;
	
#ifdef TRANSGAMING_DEINDEX
    if( !mesh->Data->Triangles.Size() && mesh->isIndexed ){
        TRACE("HavokWorld::CreateStaticMesh(): ERROR-> no index array in an indexed mesh!\n");

        return 0;
    }
#else
# ifdef TRANSGAMING
	if(!mesh->Data->Triangles.Size()) return 0;
# else
	if(!mesh->Elems.Size() && !mesh->Data->Triangles.Size()) return 0;
# endif
#endif

/*
	static bool firstMesh = true;
	if( !firstMesh ) return;
	firstMesh = false;

	String nm = mesh->Name;
	nm += ".info";

	LogBuffer MeshLog;
	MeshLog.Open( nm.AsChar() );
	MeshLog.Print( "Number of vertices: %d\n", mesh->Data->VertList.Size() );
	MeshLog.Print( "Vertex data ( Y axis up ):\n" );
	for( int i = 0; i < vertexCount; ++i ){
        Vector pos = mesh->Data->VertList.getVertPos(i);

		MeshLog.Print( "%f,%f,%f\n", pos.X, pos.Y, pos.Z );
    }
	MeshLog.Print( "Number of triangles: %d\n", triangleCount );
	MeshLog.Print( "Triangle indices ( 0-based ):\n" );
	for( int i = 0; i < triangleCount; ++i )
		MeshLog.Print( "%d,%d,%d\n", mesh->Data->Triangles[3*i],mesh->Data->Triangles[3*i+1],mesh->Data->Triangles[3*i+2] );
	MeshLog.Flush();
	MeshLog.Close();
*/
/*
	// hkLandscapeShape creation - warning: it uses our (mesh) vertices & triangles
    m_setTag("Physics");
	hkLandscapeShape* landscapeShape = new hkLandscapeShape;
	{
		hkLandscapeShape::Subpart part;
        float *positions = (float *)mesh->Data->VertList.getVertPos(0, &stride);
        Assert(positions && stride);


		part.m_vertexBase = positions;
		//part.m_vertexStriding = (int)(((char*)&(mesh->Verts[1].Pos)) - ((char*)&(mesh->Verts[0].Pos)));
        part.m_vertexStriding = stride;
        part.m_numVertices = vertexCount;
		part.m_index16Base = (hkUint16*)&(mesh->Data->Triangles[0]);
		part.m_indexStriding = 3*sizeof(WORD);
		part.m_numTriangles = triangleCount;
		part.m_stridingType = hkLandscapeShape::HK_INT16_INDICES;
		landscapeShape->addSubpart( part );
	}
	landscapeShape->setRadius( 0.05f );
*/

	// hkMeshShape creation - warning: it uses our (mesh) vertices & triangles
    m_setTag("Physics");
	hkMeshShape*        meshShape = new hkMeshShape;
    DynamicArray<unsigned short *> dummyIndices;


	meshShape->setRadius( 0.05f );


	// *** STRIPS *** ================================================================
	// 1.) Indexed Strips
	// 2.) De-Indexed Strips

	// 1.)Indexed Strips
#ifdef TRANSGAMING_DEINDEX
	if ( mesh->Data->Triangles.Size() && mesh->isIndexed && mesh->PrimType == RenderDevice::EPrimTriStrip )
#else
	if ( mesh->Data->Triangles.Size() && mesh->PrimType == RenderDevice::EPrimTriStrip )
#endif
	{
#ifdef TRANSGAMING_MESH_WARNINGS
        //TRACE("HavokWorld::CreateStaticMesh(): creating tristripped indexed geometry\n");
#endif
		// with each group being its own tristrip,
		// we'll have to add them all separately
		for (int gi=0; gi<mesh->Data->Groups.Size(); gi++) 
		{
			unsigned gbase = mesh->Data->Groups[gi].RenderBase; // index base
			unsigned gcount = mesh->Data->Groups[gi].RenderCount; // num triangles
			hkMeshShape::Subpart part;
            float *positions = (float *)mesh->Data->VertList.getVertPos(0, &stride);
            Assert(positions && stride);

			part.m_vertexBase = positions;
			//part.m_vertexStriding = (int)(((char*)&(mesh->Verts[1].Pos)) - ((char*)&(mesh->Verts[0].Pos)));
            part.m_vertexStriding = stride;
            part.m_numVertices = mesh->Data->Groups[gi].VertexCount;
#ifdef HAVOK3
			part.m_indexBase = (hkUint16*)&(mesh->Data->Triangles[gbase]);
#else
			part.m_index16Base = (hkUint16*)&(mesh->Data->Triangles[gbase]);
#endif
			part.m_indexStriding = sizeof(WORD);
			part.m_numTriangles = mesh->Data->Groups[gi].RenderCount;
#ifdef HAVOK3
			part.m_stridingType = hkMeshShape::INDICES_INT16;
#else
			part.m_stridingType = hkMeshShape::HK_INT16_INDICES;
#endif
			meshShape->addSubpart( part );
		}
	}

#ifdef TRANSGAMING_DEINDEX
    // 2.) De-Indexed Strips
	else if ( !mesh->isIndexed && mesh->PrimType == RenderDevice::EPrimTriStrip )
    {
#ifdef TRANSGAMING_MESH_WARNINGS
        //TRACE("HavokWorld::CreateStaticMesh(): creating tristripped deindexed geometry\n");
#endif

		// with each group being its own tristrip,
		// we'll have to add them all separately
		for (int gi=0; gi<mesh->Data->Groups.Size(); gi++) 
		{
			unsigned gbase = mesh->Data->Groups[gi].RenderBase; // index base
			unsigned gcount = mesh->Data->Groups[gi].RenderCount; // num triangles
			hkMeshShape::Subpart part;
            float *positions = (float *)mesh->Data->VertList.getVertPos(0, &stride);
            Assert(positions && stride);

			part.m_vertexBase = positions;
			//part.m_vertexStriding = (int)(((char*)&(mesh->Verts[1].Pos)) - ((char*)&(mesh->Verts[0].Pos)));
            part.m_vertexStriding = stride;
			part.m_numVertices = mesh->Data->Groups[gi].VertexCount;
# ifdef HAVOK3
			part.m_indexBase = generateDummyIndex(gcount + 2, gbase);
            dummyIndices.Add((unsigned short *)part.m_indexBase);
# else
			part.m_index16Base = (hkUint16*)&(mesh->Data->Triangles[gbase]);
# endif
			part.m_indexStriding = sizeof(WORD);
			part.m_numTriangles = mesh->Data->Groups[gi].RenderCount;
# ifdef HAVOK3
            part.m_stridingType = hkMeshShape::INDICES_INT16;
# else
			part.m_stridingType = hkMeshShape::HK_INT16_INDICES;
# endif
			meshShape->addSubpart( part );
		}
	}
#endif


	// *** LISTS *** ================================================================
	// 1.) Indexed Lists
	// 2.) De-indexed Lists

	// 1.) Indexed Lists
#ifdef TRANSGAMING_DEINDEX
	else if ( mesh->Data->Triangles.Size() && mesh->isIndexed && mesh->PrimType == RenderDevice::EPrimTriList )
#else
	else if ( mesh->Data->Triangles.Size() && mesh->PrimType == RenderDevice::EPrimTriList )
#endif
	{

		unsigned tcount = mesh->Data->Triangles.Size()/3;
		hkMeshShape::Subpart part;
        float *positions = (float *)mesh->Data->VertList.getVertPos(0, &stride);
        Assert(positions && stride);
        
#ifdef TRANSGAMING_MESH_WARNINGS
        //TRACE("HavokWorld::CreateStaticMesh(): creating trilist indexed geometry {positions = %p, stride = %d, count = %d (%d), index = %p, indexCount = %d}\n", positions, stride, vertexCount, mesh->Data->VertList.Size(), &mesh->Data->Triangles[0], mesh->Data->Triangles.Size());
#endif

		part.m_vertexBase = positions;
		//part.m_vertexStriding = (int)(((char*)&(mesh->Verts[1].Pos)) - ((char*)&(mesh->Verts[0].Pos)));
        part.m_vertexStriding = stride;
		part.m_numVertices = vertexCount;
#ifdef HAVOK3
		part.m_indexBase = (hkUint16*)&(mesh->Data->Triangles[0]);
#else
		part.m_index16Base = (hkUint16*)&(mesh->Data->Triangles[0]);
#endif
		part.m_indexStriding = 3 * sizeof(WORD);
		part.m_numTriangles = tcount;
#ifdef HAVOK3
		part.m_stridingType = hkMeshShape::INDICES_INT16;
#else
		part.m_stridingType = hkMeshShape::HK_INT16_INDICES;
#endif
		meshShape->addSubpart( part );
	}
	
#ifdef TRANSGAMING_DEINDEX
	// 2.) De-indexed Lists
    else if ( !mesh->isIndexed && mesh->PrimType == RenderDevice::EPrimTriList )
	{
		// Blair - DE-INDEXED
		unsigned tcount = mesh->Data->VertList.Size()/3;
		hkMeshShape::Subpart part;
        float *positions = (float *)mesh->Data->VertList.getVertPos(0, &stride);
        Assert(positions && stride);

#ifdef TRANSGAMING_MESH_WARNINGS
        //TRACE("HavokWorld::CreateStaticMesh(): creating trilist deindexed geometry {positions = %p, stride = %d}\n", positions, stride);
#endif

		part.m_vertexBase = positions;
		//part.m_vertexStriding = (int)(((char*)&(mesh->Verts[1].Pos)) - ((char*)&(mesh->Verts[0].Pos)));
        part.m_vertexStriding = stride;
		part.m_numVertices = vertexCount;
# ifdef HAVOK3
		part.m_indexBase = generateDummyIndex(mesh->Data->VertList.Size());
        dummyIndices.Add((unsigned short *)part.m_indexBase);
# else
		part.m_index16Base = (hkUint16*)&(mesh->Data->Triangles[0]);
# endif
		part.m_indexStriding = 3 * sizeof(WORD);
		part.m_numTriangles = tcount;
# ifdef HAVOK3
		part.m_stridingType = hkMeshShape::INDICES_INT16;
# else
		part.m_stridingType = hkMeshShape::HK_INT16_INDICES;
# endif
		meshShape->addSubpart( part );
	}
#endif

    else
    {
#ifdef TRANSGAMING_DEINDEX
        TRACE("HavokWorld::CreateStaticMesh(): ERROR-> unsupported vertex format! (indexed = %d, format = %s, triangles = %d, verts = %d)\n", mesh->isIndexed, mesh->PrimType == RenderDevice::EPrimTriList ? "list" : "strip", mesh->Data->Triangles.Size(), mesh->Data->VertList.Size());
#else
        TRACE("HavokWorld::CreateStaticMesh(): ERROR-> unsupported vertex format! (indexed = %d, format = %s, triangles = %d, verts = %d)\n", mesh->Data->Triangles.Size() == 0, mesh->PrimType == RenderDevice::EPrimTriList ? "list" : "strip", mesh->Data->Triangles.Size(), mesh->Data->VertList.Size());
#endif

        Assert(!"Unknown mesh format!");
    }

//	PrintMeshStatistics( mesh );

/*
	// hkStorageMeshShape creation
    m_setTag("Physics");
	hkStorageMeshShape* meshShape = new hkStorageMeshShape;
	meshShape->reserveMemory( vertexCount, triangleCount );

	for( int i = 0; i < vertexCount; ++i )
	{
		Vector v = mesh->Data->VertList.getVertPos(i);
		hkVector4 temp( v.X, v.Y, v.Z );

		meshShape->addVertex( temp );
	}

	for( int i = 0; i < triangleCount; ++i )
		meshShape->addTriangle( mesh->Data->Triangles[3*i], mesh->Data->Triangles[3*i+1], mesh->Data->Triangles[3*i+2] );
	meshShape->setRadius( 0.05f );
*/

//	MOPP From here
	String fileTime = "";
	String moppPath = "../Data/Maps/MOPPCode/";

	if( mesh->Package )
	{
		String mapName = "../Data/Maps/";
		moppPath += mesh->Package->GetName();
		mapName += mesh->Package->GetName();
#ifndef TRANSGAMING
		fileTime = fm.FileTime( mapName.AsChar() );
		GFileMan.CreateDirectory( moppPath.AsChar() );
#endif
		moppPath += '/';
	}
	else if( mesh->MPKIndex != INVALID_MPKINDEX )
	{
		String mapName = "../Data/Maps/";
#ifdef	TRANSGAMING
		moppPath += GEngine->pcfWorld->MPKFilesLoaded[mesh->MPKIndex];
		mapName += GEngine->pcfWorld->MPKFilesLoaded[mesh->MPKIndex];
#else
		moppPath += GEngine->World->MPKFilesLoaded[mesh->MPKIndex];
		mapName += GEngine->World->MPKFilesLoaded[mesh->MPKIndex];
		fileTime = fm.FileTime( mapName.AsChar() );
		GFileMan.CreateDirectory( moppPath.AsChar() );
#endif
		moppPath += '/';
	}
//	else do nothing; rare cases
	String dirPath = moppPath;
	moppPath += mesh->Name;
	char buffer[10];
	sprintf( buffer, "%d", mesh->Data->VertList.Size() );
	moppPath += buffer;
	moppPath += ".mopp";

#ifndef TRANSGAMING
	if( fileTime.Len() )
	{
		String moppTime = fm.FileTime( moppPath.AsChar() );
		if( moppTime.Len() && ( strcmp( fileTime.AsChar(), moppTime.AsChar() ) > 0 ) )
		{
			GLog.Print( "MOPP file older than map file, removing all MOPPs for this map.\n" );
			fm.RemoveFilesInDirectory( dirPath.AsChar() );
		}
	}
#endif

	hkMoppCode* code = LoadMOPPCode( moppPath );
	if( !code )
	{
		m_setTag("Physics");
		hkMoppFitToleranceRequirements mft;
		mft.setAbsoluteFitToleranceOfTriangles(0.05f);
//		code = hkMoppUtility::buildCode( landscapeShape ,mft );
		code = hkMoppUtility::buildCode( meshShape ,mft );

        // OK, I've fixed up the SaveMOPPCode and LoadMOPPCode paths so that now, we can
		// just load the MOPP codes once they've been generated once ( the first run of a level
		// will generate them and call SaveMOPPCode...everytime after that, we just load
		// the files in the code above.. )  This will give us a nice boost on the MOPP loading until the .hkx
		// loading replaces this.
		SaveMOPPCode( moppPath, code );
	}

//    m_setTag("Physics");
//	hkMoppBvTreeShape* moppShape = new hkMoppBvTreeShape( landscapeShape, code );
    m_setTag("Physics");
	hkMoppBvTreeShape* moppShape = new hkMoppBvTreeShape( meshShape, code );
	code->removeReference();
//	landscapeShape->removeReference();
	meshShape->removeReference();

//	MOPP To here
#ifdef HAVOK3
	hkRigidBodyCinfo rigidBodyInfo;
#else
	hkRigidBodyConstructionInfo rigidBodyInfo;
#endif
	rigidBodyInfo.m_shape = moppShape;
#ifdef HAVOK3
	rigidBodyInfo.m_motionType = hkMotion::MOTION_FIXED;
#else
	rigidBodyInfo.m_motionType = HK_FIXED_MOTION;
#endif
	rigidBodyInfo.m_collisionFilterInfo = hkGroupFilter::calcFilterInfo( (collisionGroup==-1)?HCGFixed:collisionGroup );
	rigidBodyInfo.m_mass = 10.0f;	// just in case
    m_setTag("Physics");
	hkRigidBody* rigidbody = new hkRigidBody( rigidBodyInfo );
	rigidbody->getMaterial().setRestitution( restitution );
	rigidbody->getMaterial().setFriction( friction );
	hkPropertyValue pv;
#ifdef HAVOK3
	pv.setPtr(mesh);
#else
	pv.asPtr = mesh;
#endif
	rigidbody->addProperty( HPEntityPointer, pv );
	World->addEntity( rigidbody );
	moppShape->removeReference();
	// don't remove reference to rigid body, the system will destroy it then
	StaticMeshes.Add( rigidbody );

	mesh->Flags |= Entity::EEntDontDeleteOnTimer;


    for (int i = 0; i < dummyIndices.Size(); i++)
        destroyDummyIndex(dummyIndices[i]);
    

#ifdef TRANSGAMING_MESH_WARNINGS
    //TRACE("HavokWorld::CreateStaticMesh(): done creating a static mesh for '%s'\n", mesh->Name.AsChar());
#endif
	return rigidbody;
}

void HavokWorld::RemoveStaticMesh( hkRigidBody* rb )
{
	if( rb->getSimulationIsland() )
		World->removeEntity( rb );
}

void* HavokWorld::GetStaticMesh( Entity* entity , bool activeOnly )
{
	for( int i = 0; i < StaticMeshes.Size(); ++i )
		if( (!activeOnly || StaticMeshes[i]->getSimulationIsland()) && StaticMeshes[i]->hasProperty( HPEntityPointer ) )
		{
			hkPropertyValue p = StaticMeshes[i]->getProperty( HPEntityPointer );
#ifdef HAVOK3
			Entity* ent = (Entity*)p.getPtr();
#else
			Entity* ent = (Entity*)p.asPtr;
#endif
			if( ent == entity )
				return StaticMeshes[i];
		}
	return 0;
}

bool HavokWorld::SetEntityCollisionGroup(Entity* entity, bool activeOnly, int newGroup)
{
	for( int i = 0; i < StaticMeshes.Size(); ++i ) {
		if( (!activeOnly || StaticMeshes[i]->getSimulationIsland()) && StaticMeshes[i]->hasProperty( HPEntityPointer ) )
		{
			hkPropertyValue p = StaticMeshes[i]->getProperty( HPEntityPointer );
#ifdef HAVOK3
			Entity* ent = (Entity*)p.getPtr();
#else
			Entity* ent = (Entity*)p.asPtr;
#endif
			if( ent == entity ) {
				hkRigidBody* rigidBody = (hkRigidBody*) StaticMeshes[i];
#ifdef HAVOK3
				rigidBody->getCollidable()->setCollisionFilterInfo( hkGroupFilter::calcFilterInfo( newGroup ) );
#else
				rigidBody->getCollidable().setCollisionFilterInfo( hkGroupFilter::calcFilterInfo( newGroup ) );
#endif
				if (rigidBody->getSimulationIsland()) {
#ifdef HAVOK3
					// BLAIR - not convinced about this...
					World->updateCollisionFilterOnEntity( rigidBody, HK_UPDATE_FILTER_ON_ENTITY_DISABLE_ENTITY_ENTITY_COLLISIONS_ONLY, HK_UPDATE_COLLECTION_FILTER_IGNORE_SHAPE_COLLECTIONS );
#else
					World->updateCollisionFilter(rigidBody);
#endif
					return 1;
				}
			}
		}
	}

	return 0;
}

hkRigidBody* HavokWorld::CreatePawnBody( Vector& pivotOffset, float scale, DWORD pawnBody, int collisionGroup )
{
	const float scale2 = scale*OLD_PAWN_SCALE;
	hkReal mass = 10000*scale2*scale2*scale2;	// Player mass will be 80 that way

	float realDampingLinear = 0.0f;
	float realDampingAngular = 0.0f;
	if( mIsUnderwater )
	{
		realDampingLinear = 1.0f - ((1.0f-realDampingLinear)/GEngine->Physics->TWUnderwaterDampingRaiseLinear);
		realDampingAngular = 1.0f - ((1.0f-realDampingAngular)/GEngine->Physics->TWUnderwaterDampingRaiseAngular);
	}

	hkRigidBody* rigidbody;

	switch( pawnBody )
	{
	default:
	case EPawnBodyPlayer:	// default player body, 4 spheres
	case EPawnBodyDefault:
	case EPawnBodyDefaultSweep:
		{

//#ifndef HAVOK3 // ============================================

			// Sphere data
			float spheresData[16] = {
				0, (delta-3.15f)*scale2, 0, 1.65f*scale2,
				0, (delta-0.5f)*scale2, 0, 2.0f*scale2,
				0, (delta+2.5f)*scale2, 0, 2.0f*scale2,
				0, (delta+4.5f)*scale2, 0, scale2
			};

			// Load hkArray with sphere data
			hkVector4 spheres[4];
			int index = 0;
			for( int i = 0; i < 4; i++ )
			{
				spheres[i].set( spheresData[index], spheresData[index + 1], spheresData[index + 2], spheresData[index + 3] );
				index += 4;
			}

            m_setTag("Physics");
#ifdef HAVOK3
			hkVector4 capsuleTop( 0, (delta-3.15f)*scale2, 0, 0 );
			hkVector4 capsuleBottom( 0, (delta+4.5f)*scale2, 0, 0 );
			hkCapsuleShape* capsuleShape = new hkCapsuleShape( capsuleTop, capsuleBottom, 2.0f*scale2 );
#else
			hkMultiSphereShape* spheresShape = new hkMultiSphereShape( spheres, 4 );
#endif

			// Havok fix for jumping on triangle edges :)
			hkMultiRayShape::Ray rays[1];
			rays[0].m_start.set( 0, (delta-4.8f)*scale2, 0 );
			rays[0].m_end.set( 0, (delta-5.5f)*scale2, 0 );
            m_setTag("Physics");
			hkMultiRayShape *rayShape = new hkMultiRayShape(rays, 1);

			hkArray<hkShape*> shapeArray;
			shapeArray.pushBack( rayShape );
#ifdef HAVOK3
			shapeArray.pushBack( capsuleShape );
#else
			shapeArray.pushBack( spheresShape );
#endif

            m_setTag("Physics");
			hkListShape* listShape = new hkListShape( &shapeArray[0], shapeArray.getSize() );
#ifdef HAVOK3
			capsuleShape->removeReference();
#else
			spheresShape->removeReference();
#endif
			rayShape->removeReference();

			hkShape* sweepShape = 0;
#ifndef HAVOK3
			if( pawnBody != EPawnBodyDefault )
			{
                m_setTag("Physics");
				sweepShape = new hkSweepBvTreeShape( listShape );
				listShape->removeReference();
			}
#endif

#ifdef HAVOK3
			hkRigidBodyCinfo rigidBodyInfo;
			rigidBodyInfo.m_shape = (hkShape*)listShape; // THERE's NO SWEEP SHAPE IN HAVOK3!
#else
			hkRigidBodyConstructionInfo rigidBodyInfo;
			rigidBodyInfo.m_shape = (sweepShape != 0)?(hkShape*)sweepShape:(hkShape*)listShape;
#endif
			rigidBodyInfo.m_position.set(0.0f, 0.0f, 0.0f);
			rigidBodyInfo.m_restitution = 0;
			rigidBodyInfo.m_friction = 1.0f;
			rigidBodyInfo.m_angularDamping = realDampingAngular;
			rigidBodyInfo.m_linearDamping = realDampingLinear;
#ifdef HAVOK3
			rigidBodyInfo.m_motionType = hkMotion::MOTION_SPHERE_INERTIA;
#else
			rigidBodyInfo.m_motionType = HK_SPHERE_INERTIA_MOTION;
#endif
			rigidBodyInfo.m_collisionFilterInfo = hkGroupFilter::calcFilterInfo( (collisionGroup == -1)?
				( ( pawnBody == EPawnBodyPlayer )?HCGPlayerBody:HCGBody ):
				collisionGroup );

			// Calculate inertia tensor

			hkMassProperties massProperties;
			hkInertiaTensorComputer::computeVertexCloudMassProperties( spheresData, 4*sizeof(float), 4, mass, massProperties );
			rigidBodyInfo.m_inertiaTensor = massProperties.m_inertiaTensor;
			rigidBodyInfo.m_centerOfMass = massProperties.m_centerOfMass;
			rigidBodyInfo.m_mass = massProperties.m_mass;
#ifdef HAVOK_POSITION_BUG_TRACKING
			Assert( massProperties.m_mass >= 0.0f );
#endif

            m_setTag("Physics");
			rigidbody = new hkRigidBody( rigidBodyInfo );

#ifndef HAVOK3 // NO SWEEP SHAPE IN HAVOK3!
			if( sweepShape )
				sweepShape->removeReference();
			else
#endif
				listShape->removeReference();

			hkPropertyValue pv;
#ifdef HAVOK3
			pv.setInt(1);
#else
			pv.asInt = 1;
#endif
			rigidbody->addProperty( HPRayCastDontCollide, pv );

			pivotOffset.X = 0.0f; pivotOffset.Y = -5.5f*scale2; pivotOffset.Z = 0.0f;
//			float m = rigidbody->getMass();
//			GLog.Print( "Player Mass: %f\n" , m );	// not only player has such a body; lucy_sword too
//#else
	//printf("HavokWorld::CreatePawnBody() - FIXME!!!!!!!!!!!\n");
//			GLog.Print("HavokWorld::CreatePawnBody -> NO SWEEPSHAPE\n");
			//return HK_NULL;
//#endif // ifndef HAVOK3 ==================================
		}
		break;
	case EPawnBodySphere:		// simple sphere, like, for instance, it would be for raven
	case EPawnBodySphereSweep:	// simple sphere, like, for instance, it would be for raven
		{
            m_setTag("Physics");
			hkSphereShape* sphereShape = new hkSphereShape( 5.5f*scale2 );

#ifdef HAVOK3
			hkRigidBodyCinfo sphereInfo;
#else
			hkRigidBodyConstructionInfo sphereInfo;
#endif
			sphereInfo.m_shape = sphereShape;
			sphereInfo.m_position.set( 0.0f, 0.0f, 0.0f );
			sphereInfo.m_restitution = 0.9f;
			sphereInfo.m_angularDamping = realDampingAngular;
			sphereInfo.m_linearDamping = realDampingLinear;
#ifdef HAVOK3
			sphereInfo.m_motionType = hkMotion::MOTION_SPHERE_INERTIA;
#else
			sphereInfo.m_motionType = HK_SPHERE_INERTIA_MOTION;
#endif
			sphereInfo.m_collisionFilterInfo = hkGroupFilter::calcFilterInfo( (collisionGroup == -1)? HCGBody:collisionGroup );

			sphereInfo.m_mass = mass;
			hkMassProperties massProperties;
			hkInertiaTensorComputer::computeSphereVolumeMassProperties( 5.5f*scale2, sphereInfo.m_mass, massProperties );
			sphereInfo.m_inertiaTensor = massProperties.m_inertiaTensor;
			sphereInfo.m_centerOfMass = massProperties.m_centerOfMass;
			sphereInfo.m_mass = massProperties.m_mass;

#ifdef HAVOK_POSITION_BUG_TRACKING
			Assert( massProperties.m_mass >= 0.0f );
#endif
			
            m_setTag("Physics");
			rigidbody = new hkRigidBody( sphereInfo );

			sphereShape->removeReference();
/*
			if( pawnBody == EPawnBodySphereSweep )
			{
                m_setTag("Physics");
				hkConvexSweepShape* css = new hkConvexSweepShape( static_cast<const hkConvexShape*>(sphereShape), rigidbody, 15 );//SPHERE_SWEEP_SPEED );
				rigidbody->getCollidable().getShape()->removeReference();
				rigidbody->getCollidable().setShape( css );
			}
*/
			hkPropertyValue pv;
#ifdef HAVOK3
			pv.setInt(1);
#else
			pv.asInt = 1;
#endif
			rigidbody->addProperty( HPRayCastDontCollide, pv );

			pivotOffset.X = 0.0f; pivotOffset.Y = 0.0f; pivotOffset.Z = 0.0f;
		}
		break;
	case EPawnBodyFatter:	// three spheres, two big, one smaller, more fat than with default spheres
	case EPawnBodyFatterSweep:
		{
			// Sphere data
			float spheresData[12] = {
#ifdef RAY_BODY
				0, (delta-2.2f)*scale2, 0, 2.6f*scale2,
#else
				0, (delta-3.0f)*scale2, 0, 2.5f*scale2,
#endif
				0, (delta+1.0f)*scale2, 0, 3.0f*scale2,
				0, (delta+4.0f)*scale2, 0, 1.5f*scale2,
			};

			// Load hkArray with sphere data
			hkVector4 spheres[3];
			int index = 0;
			for( int i = 0; i < 3; i++ )
			{
				spheres[i].set( spheresData[index], spheresData[index + 1], spheresData[index + 2], spheresData[index + 3] );
				index += 4;
			}

            m_setTag("Physics");
#ifdef HAVOK3
			hkVector4 capsuleTop( 0, (delta-2.2f)*scale2, 0, 0 );
			hkVector4 capsuleBottom( 0, (delta+4.0f)*scale2, 0, 0 );
			hkCapsuleShape* capsuleShape = new hkCapsuleShape( capsuleTop, capsuleBottom, 2.5f*scale2 );
#else
			hkMultiSphereShape* spheresShape = new hkMultiSphereShape( spheres, 3 );
#endif

#ifdef RAY_BODY
			// Havok fix for jumping on triangle edges :)
			hkMultiRayShape::Ray rays[1];
			rays[0].m_start.set( 0, (delta-4.8f)*scale2, 0 );
			rays[0].m_end.set( 0, (delta-5.5f)*scale2, 0 );
            m_setTag("Physics");
			hkMultiRayShape *rayShape = new hkMultiRayShape(rays, 1);

			hkArray<hkShape*> shapeArray;
#ifdef HAVOK3
			shapeArray.pushBack( capsuleShape );
#else
			shapeArray.pushBack( spheresShape );
#endif
			shapeArray.pushBack( rayShape );

            m_setTag("Physics");
			hkListShape* listShape = new hkListShape( &shapeArray[0], shapeArray.getSize() );

#ifdef HAVOK3
			capsuleShape->removeReference();
#else
			spheresShape->removeReference();
#endif
			rayShape->removeReference();
#endif
			hkShape* sweepShape = 0;
			if( pawnBody != EPawnBodyFatter )
			{
#ifndef HAVOK3 // No Sweep Shape!

#ifdef RAY_BODY
                m_setTag("Physics");
				sweepShape = new hkSweepBvTreeShape( listShape );
				listShape->removeReference();
#else
                m_setTag("Physics");
				sweepShape = new hkSweepBvTreeShape( spheresShape );
				spheresShape->removeReference();
#endif
#endif // HAVOK3
			}

#ifdef HAVOK3
			hkRigidBodyCinfo rigidBodyInfo;
#else
			hkRigidBodyConstructionInfo rigidBodyInfo;
#endif

#ifdef HAVOK3
#ifdef RAY_BODY
			rigidBodyInfo.m_shape = (hkShape*)listShape;
#else
			// blair - the multi-sphere shape...no longer all the rage...replace it with
			// the sleeker, faster hkCapsuleShape...
			rigidBodyInfo.m_shape = (hkShape*)capsuleShape; // (hkShape*)spheresShape;
#endif
#else // HAVOK3

#ifdef RAY_BODY
			rigidBodyInfo.m_shape = (sweepShape != 0)?(hkShape*)sweepShape:(hkShape*)listShape;
#else
			rigidBodyInfo.m_shape = (sweepShape != 0)?(hkShape*)sweepShape:(hkShape*)spheresShape;
#endif

#endif // HAVOK3

			rigidBodyInfo.m_position.set(0.0f, 0.0f, 0.0f);
			rigidBodyInfo.m_angularDamping = realDampingAngular;
			rigidBodyInfo.m_linearDamping = realDampingLinear;
#ifdef HAVOK3
			rigidBodyInfo.m_motionType = hkMotion::MOTION_SPHERE_INERTIA;
#else
			rigidBodyInfo.m_motionType = HK_SPHERE_INERTIA_MOTION;
#endif
			rigidBodyInfo.m_collisionFilterInfo = hkGroupFilter::calcFilterInfo( (collisionGroup == -1)?HCGBody:collisionGroup );

			// Calculate inertia tensor

			hkMassProperties massProperties;
			hkInertiaTensorComputer::computeVertexCloudMassProperties( spheresData, 4*sizeof(float), 3, mass, massProperties );
			rigidBodyInfo.m_inertiaTensor = massProperties.m_inertiaTensor;
			rigidBodyInfo.m_centerOfMass = massProperties.m_centerOfMass;
			rigidBodyInfo.m_mass = massProperties.m_mass;

#ifdef HAVOK_POSITION_BUG_TRACKING
			Assert( massProperties.m_mass >= 0.0f );
#endif

            m_setTag("Physics");
			rigidbody = new hkRigidBody( rigidBodyInfo );
			if( sweepShape )
				sweepShape->removeReference();
			else
#ifdef RAY_BODY
				listShape->removeReference();
#else
				spheresShape->removeReference();
#endif

#ifndef HAVOK3 // BLAIR - this makes us not hit enemies...but we want to, 'cause it's fun.
			hkPropertyValue pv;
#ifdef HAVOK3
			pv.setInt(1);
#else
			pv.asInt = 1;
#endif
			rigidbody->addProperty( HPRayCastDontCollide, pv );
#endif // HAVOK3

			pivotOffset.X = 0.0f; pivotOffset.Y = -5.5f*scale2; pivotOffset.Z = 0.0f;
		}
		break;
	}

	//float m = rigidbody->getMass();
	//GLog.Print( "Mass: %f\n" , m );

    return rigidbody;
}

hkRigidBody* HavokWorld::CreatePawnBodyFromMesh( WorldMesh* mesh, float& volumeMass, DWORD pawnBodyType, int collisionGroup )
{
	Assert(mesh);
	Assert(mesh->Data->VertList.Size()>=0);
	GLog.Print("CreatePhysicsObjectFromMesh : %s\n", mesh->Name.AsChar());
	if(!mesh->Data->VertList.Size())     return NULL;
    DynamicArray<unsigned short *> dummyIndices;


	// BLAIR
#ifdef TRANSGAMING_DEINDEX
	if( (!mesh->Data->Triangles.Size() && mesh->isIndexed) &&
		( ( pawnBodyType == EPawnFromMeshGlass ) || ( pawnBodyType == EPawnFromMeshGlassSweep ) ||
		  ( pawnBodyType == EPawnFromMeshNonCenteredNotConvex ) || ( pawnBodyType == EPawnFromMeshNotConvex ) )
		) return NULL;
#else
	if( !mesh->Data->Triangles.Size() &&
		( ( pawnBodyType == EPawnFromMeshGlass ) || ( pawnBodyType == EPawnFromMeshGlassSweep ) ||
		  ( pawnBodyType == EPawnFromMeshNonCenteredNotConvex ) || ( pawnBodyType == EPawnFromMeshNotConvex ) )
		) return NULL;
#endif
	float scale = mesh->GetScale();
/*
	if( mesh->Name == "physdest_k2_ciezkakolumnicashape5" )
	{
		GLog.Print( "Mesh data:\n" );
		GLog.Print( "Vertices:\n" );
		for( int i = 0; i < mesh->Data->VertList.Size(); ++i ){
            Vector pos = mesh->Data->VertList.getVertPos(i);

			GLog.Print( "%d: (%f,%f,%f)\n", i, pos.X * scale, pos.Y * scale, pos.Z * scale );
        }
//		for( int i = 0; i < mesh->Data->Triangles.Size(); ++i )
//			GLog.Print( "%d: (%d,%d,%d)\n", i, mesh->Data->Triangles[3*i], mesh->Data->Triangles[3*i+1], mesh->Data->Triangles[3*i+2] );
		GLog.Flush();
	}
*/
	float realDampingLinear = 0.05f;
	float realDampingAngular = 0.0f;
	if( mIsUnderwater )
	{
		realDampingLinear = 1.0f - ((1.0f-realDampingLinear)/GEngine->Physics->TWUnderwaterDampingRaiseLinear);
		realDampingAngular = 1.0f - ((1.0f-realDampingAngular)/GEngine->Physics->TWUnderwaterDampingRaiseAngular);
	}

	hkShape* ourShape = 0;
	hkMassProperties massProperties;
	bool makeSweep = false;

	if( ( pawnBodyType == EPawnFromMeshGlass ) || ( pawnBodyType == EPawnFromMeshGlassSweep ) )
	{
		const float scaleModif = (0.005/scale)*(0.005/scale);

#ifdef LOG_CONVEX_CREATION
# ifdef TRANSGAMING
		float  firstTime = GEngine->SysDriver.GetCurrentTime();
# else
		double firstTime = GEngine->SysDriver.GetCurrentTime();
# endif
		GLog.Print( "Creating glass from mesh '%s'", mesh->Name.AsChar() );
		GLog.Flush();
#endif

		// 1. get highest distance between verts && get lowest distance
		float highest = -1.0f;
		int highestInd1 = -1;
		int highestInd2 = -1;
		float lowest = 100000.0f;

        for( int i = 0; i < mesh->Data->VertList.Size(); ++i ){
            Vector pos = mesh->Data->VertList.getVertPos(i);

			for( int j = i+1; j < mesh->Data->VertList.Size(); ++j )
			{
                Vector pos2 = mesh->Data->VertList.getVertPos(j);
				float   dist = (pos - pos2).MagnitudeSquared();


				if( dist > highest )
				{
					highest = dist;
					highestInd1 = i;
					highestInd2 = j;
				}
				if( dist < lowest )
					lowest = dist;
			}
        }

		// 1.5. if highest is lower than 0.005/scale, return NULL
		if( highest < scaleModif )
			return NULL;

        Vector highestInd1Pos = mesh->Data->VertList.getVertPos(highestInd1);
        Vector highestInd2Pos = mesh->Data->VertList.getVertPos(highestInd2);
        Vector  one = highestInd1Pos - highestInd2Pos;


		one.Normalize();

		// 2. get the second distance ( one of those two, and a different one ) so the field of triangle would be greatest
		float secondHighest = -1.0f;
		int highestInd3 = -1;
		for( int i = 0; i < mesh->Data->VertList.Size(); ++i )
		{
			if( ( i == highestInd1 ) || ( i == highestInd2 ) ) continue;

            Vector      pos =   mesh->Data->VertList.getVertPos(i);
			Vector      t =     pos - highestInd2Pos;
			Vector      point = highestInd2Pos + one * one.Dot(t);
			float       val =   (pos - point).MagnitudeSquared();

			if( val > secondHighest )
			{
				secondHighest = val;
				highestInd3 = i;
			}
		}

		// 2.5. if the field of triangle is lower than 0.005/scale, return NULL
		if( 0.5*secondHighest < (0.005/scale) )
			return NULL;

        Vector      highestInd3Pos = mesh->Data->VertList.getVertPos(highestInd3);
		hkVector4   pt1( highestInd1Pos.X, highestInd1Pos.Y, highestInd1Pos.Z );
		hkVector4   pt2( highestInd2Pos.X, highestInd2Pos.Y, highestInd2Pos.Z );
		hkVector4   pt3( highestInd3Pos.X, highestInd3Pos.Y, highestInd3Pos.Z );

		// Let's make it a bit smaller
		hkVector4 center( pt1 );
		center.add4( pt2 );
		center.add4( pt3 );
		center.mul4( 0.333333333f );

		hkVector4 temp;
		temp = pt1; temp.sub4( center ); temp.mul4( 0.8f ); pt1 = center; pt1.add4( temp );
		temp = pt2; temp.sub4( center ); temp.mul4( 0.8f ); pt2 = center; pt2.add4( temp );
		temp = pt3; temp.sub4( center ); temp.mul4( 0.8f ); pt3 = center; pt3.add4( temp );

        m_setTag("Physics");
		hkTriangleShape* triShape = new hkTriangleShape( pt1, pt2, pt3 );
		triShape->setRadius( 0.01f );

		if( pawnBodyType == EPawnFromMeshGlassSweep )
			makeSweep = true;

		float mass = 10;

		hkInertiaTensorComputer::computeTriangleSurfaceMassProperties( pt1, pt2, pt3, mass, 0.1f, massProperties );
		volumeMass = 10;
		ourShape = (hkConvexShape*)triShape;

#ifdef LOG_CONVEX_CREATION
		GLog.Print( ", duration: %f\n", GEngine->SysDriver.GetCurrentTime()-firstTime );
		GLog.Flush();
#endif

	}
	else
	{
		switch( pawnBodyType )
		{
		case EPawnFromMeshNonCentered:
		case EPawnFromMeshNonCenteredSweep:
		case EPawnFromMeshConvex:
		case EPawnFromMeshConvexSweep:
			{
				hkConvexVerticesShape* vertShape = 0;

				hkArray<hkVector4> verts;
				hkArray<hkVector4> planeEquations;
#ifdef HAVOK3
				hkStridedVertices stridedVerts;
				hkGeometry geom;
#else
				hkArray<hkVector4> vertsOut;
#endif
				bool centered = ( pawnBodyType == EPawnFromMeshConvex ) || ( pawnBodyType == EPawnFromMeshConvexSweep );

				// Check if we have planes for this shape in cache
				if( mesh->MPKIndex == INVALID_MPKINDEX )
				{
					HavokWorldConvexCache* cache = (HavokWorldConvexCache*)GEngine->Physics->mConvexCache;
					HavokWorldConvexEntry* entry = cache->GetEntry( mesh->FileName, mesh->Name, centered );
					if( entry )
					{
#ifdef LOG_CONVEX_CREATION
						GLog.Print( "Mesh '%s', shape '%s', scale %f taken from cache\n", mesh->FileName.AsChar(), mesh->Name.AsChar(), scale );
						GLog.Flush();
#endif
						planeEquations = entry->planeEquations;
#ifdef HAVOK3
						geom.m_vertices = entry->vertsOut;
#else
						vertsOut = entry->vertsOut;
#endif
						int planeEqSize = planeEquations.getSize();
						if( ( planeEqSize >= 4 ) && ( entry->scale != scale ) )
						{
#ifdef HAVOK3
							int geomVertSize = geom.m_vertices.getSize();
#else
							int vertsOutSize = vertsOut.getSize();
#endif
							Assert(entry->scale);
#ifdef TRANSGAMING
							float  multiplier = scale/entry->scale;
#else
							double multiplier = scale/entry->scale;
#endif
							for( int i = 0; i < planeEqSize; ++i )
								planeEquations[i](3) *= multiplier;
#ifdef HAVOK3
							for( int i = 0; i < geomVertSize; ++i )
								geom.m_vertices[i].mul4( multiplier );
#else
							for( int i = 0; i < vertsOutSize; ++i )
								vertsOut[i].mul4( multiplier );
#endif
						}
#ifdef HAVOK3
						stridedVerts.m_numVertices = geom.m_vertices.getSize();
						stridedVerts.m_striding = sizeof(hkVector4);
						stridedVerts.m_vertices = &(geom.m_vertices[0](0));
#endif
					}
					else
					{
//#ifndef HAVOK3
						//GLog.Print("HavokWorld::CreatePawnBodyFromMesh -> NO SWEEPSHAPE\n");
						for( int i = 0; i < mesh->Data->VertList.Size(); ++i )
						{
                            Vector      pos = mesh->Data->VertList.getVertPos(i);
							hkVector4   vec( pos.X * scale, pos.Y * scale, pos.Z * scale, 0 );


							verts.pushBack( vec );
						}

#ifndef HAVOK3 // for now we'll stick with the verts we have...
						// Check if we can make a smaller set for all subsequent operations
						hkArray<hkVector4> verts2;
						verts2 = verts;
						hkGeometryFilterer::removeCloseVertices( verts2, 0.0001f );
						if( ( verts2.getSize() > 3 ) && !hkGeometryChecker::isCoplanarSet( verts2, 0.00000000001f ) ) verts = verts2;
						verts2.clearAndDeallocate();
#endif // HAVOK3

#ifdef LOG_CONVEX_CREATION
# ifdef TRANSGAMING
						float  firstTime = GEngine->SysDriver.GetCurrentTime();
# else
						double firstTime = GEngine->SysDriver.GetCurrentTime();
# endif
						GLog.Print( "Creating convex from mesh '%s'", mesh->Name.AsChar() );
						GLog.Flush();
#endif

#ifdef HAVOK3
						stridedVerts.m_numVertices = verts.getSize();
						stridedVerts.m_striding = sizeof(hkVector4);
						stridedVerts.m_vertices = &(verts[0](0));
						// By far the COOLEST part of the game...where we create the convex geometry...
						hkGeometryUtility::createConvexGeometry( stridedVerts, geom, planeEquations );
						{
							stridedVerts.m_numVertices = geom.m_vertices.getSize();
							stridedVerts.m_striding = sizeof(hkVector4);
							stridedVerts.m_vertices = &(geom.m_vertices[0](0));					
						}

						// BLAIR - We're having problems with shapes that have only 3 vertices...it seems
						// that you can't really make a convex hull with only 3 vertices.
						GLog.Print("#### %s numVertices = %d\n", mesh->Name.AsChar(), stridedVerts.m_numVertices);
						if ( stridedVerts.m_numVertices < 4 )
						{
							GLog.Print("HavokWorld::CreatePawnBodyFromMesh() - %s : Insufficient data to create a hkConvexVerticesShape.  Failure on hkInertiaTensorComputer::computeVertexHull().\n", mesh->Name.AsChar());
							return HK_NULL; // Bad Object
						}
#else
						hkGeometryUtil::computePlanesFromVertices( &verts[0](0), verts.getSize(), sizeof(hkVector4), planeEquations, vertsOut );
#endif

#ifdef LOG_CONVEX_CREATION
						GLog.Print( ", duration: %f\n", GEngine->SysDriver.GetCurrentTime()-firstTime );
						GLog.Flush();
#endif

#ifdef HAVOK3
						cache->PutEntry( mesh->FileName, mesh->Name, planeEquations, geom.m_vertices, scale, centered );
#else
						cache->PutEntry( mesh->FileName, mesh->Name, planeEquations, vertsOut, scale, centered );
#endif
#ifdef LOG_CONVEX_CREATION
						GLog.Print( "Mesh '%s', shape '%s', scale %f entered into cache\n", mesh->FileName.AsChar(), mesh->Name.AsChar(), scale );
						GLog.Flush();
#endif

//#else
//				GLog.Print("HavokWorld::CreatePawnBodyFromMesh -> NO SWEEPSHAPE\n");
//				return HK_NULL;
//#endif
					}
				}
				else
				{
					m_setTag("Physics");
					for( int i = 0; i < mesh->Data->VertList.Size(); ++i )
					{
                        Vector      pos = mesh->Data->VertList.getVertPos(i);
						hkVector4   vec( pos.X * scale, pos.Y * scale, pos.Z * scale, 0 );

						verts.pushBack( vec );
					}

#ifndef HAVOK3 // we'll worry about trimming verts later...
					// Check if we can make a smaller set for all subsequent operations
					hkArray<hkVector4> verts2;
					verts2 = verts;
					hkGeometryFilterer::removeCloseVertices( verts2, 0.0001f );
					if( ( verts2.getSize() > 3 ) && !hkGeometryChecker::isCoplanarSet( verts2, 0.00000000001f ) )
						verts = verts2;
					verts2.clearAndDeallocate();
#endif

#ifdef LOG_CONVEX_CREATION
# ifdef TRANSGAMING
					float  firstTime = GEngine->SysDriver.GetCurrentTime();
# else
					double firstTime = GEngine->SysDriver.GetCurrentTime();
# endif
					GLog.Print( "Creating MPK convex from mesh '%s'", mesh->Name.AsChar() );
					GLog.Flush();
#endif

#ifdef HAVOK3
					stridedVerts.m_numVertices = verts.getSize();
					stridedVerts.m_striding = sizeof(hkVector4);
					stridedVerts.m_vertices = &(verts[0](0));
					// By far the COOLEST part of the game...where we create the convex geometry...
					hkGeometryUtility::createConvexGeometry( stridedVerts, geom, planeEquations );
					{
						stridedVerts.m_numVertices = geom.m_vertices.getSize();
						stridedVerts.m_striding = sizeof(hkVector4);
						stridedVerts.m_vertices = &(geom.m_vertices[0](0));					
					}
#else
					hkGeometryUtil::computePlanesFromVertices( &verts[0](0), verts.getSize(), sizeof(hkVector4), planeEquations, vertsOut );
#endif

#ifdef LOG_CONVEX_CREATION
					GLog.Print( ", duration: %f\n", GEngine->SysDriver.GetCurrentTime()-firstTime );
					GLog.Flush();
#endif
				}

                if( planeEquations.getSize() >= 4 ){
                    m_setTag("Physics");
#ifdef HAVOK3
					vertShape = new hkConvexVerticesShape( stridedVerts, planeEquations );
#else
					vertShape = new hkConvexVerticesShape(&(vertsOut[0](0)), sizeof(hkVector4), vertsOut.getSize(), planeEquations );
#endif
                }
				else
					return NULL;

#ifdef HAVOK3
				vertShape->setRadius(0.0f);
#else
				vertShape->setRadius(0.0f);
#endif
				if( ( pawnBodyType == EPawnFromMeshNonCenteredSweep ) ||
					( pawnBodyType == EPawnFromMeshConvexSweep ) )
					makeSweep = true;

				// Get arbitrary mass
				// Calculate inertia tensor
				float mass = 10;
				// Compute properties to get volume
#ifdef HAVOK3
				int res = hkInertiaTensorComputer::computeVertexHullVolumeMassProperties( stridedVerts.m_vertices, sizeof(hkVector4), stridedVerts.m_numVertices, mass, massProperties );
#else
				int res = hkInertiaTensorComputer::computeVertexHullVolumeMassProperties( &(vertsOut[0](0)), sizeof(hkVector4), vertsOut.getSize(), mass, massProperties );
#endif
				Assert( res == HK_SUCCESS );
				// Adjust mass, based on volume
				mass = 100*fabs(massProperties.m_volume);
				volumeMass = mass;

				// Make sure that mesh mass is minimum 30
				mass = (mass < 30.0f)?30.0f:mass;
				// Compute properties again to get final inertia tensor, mass, and center of mass ( just in case )
#ifdef HAVOK3
				res = hkInertiaTensorComputer::computeVertexHullVolumeMassProperties( stridedVerts.m_vertices, sizeof(hkVector4), stridedVerts.m_numVertices, mass, massProperties );
#else
				res = hkInertiaTensorComputer::computeVertexHullVolumeMassProperties( &(vertsOut[0](0)), sizeof(hkVector4), vertsOut.getSize(), mass, massProperties );
#endif
				Assert( res == HK_SUCCESS );

				ourShape = (hkConvexShape*)vertShape;
			}
			break;
			case EPawnFromMeshNonCenteredNotConvex:
			case EPawnFromMeshNotConvex:
			{
#ifdef TRANSGAMING // BLAIR
                int             triangleCount;
			    hkUint16*       indices = NULL;
                m_setTag("Physics");
				hkMeshShape*    meshShape = new hkMeshShape;
				hkVector4       scale3( scale, scale, scale );

                
                meshShape->setRadius( 0.05f );
				meshShape->setScaling( scale3 );


                if ( mesh->PrimType == RenderDevice::EPrimTriStrip )
                {
                    for (int gi=0; gi<mesh->Data->Groups.Size(); gi++) 
                    {
                        //numTris += mesh->Data->Groups[gi].RenderCount; // num triangles
#ifdef TRANSGAMING_DEINDEX
                        if (mesh->isIndexed)
#endif
                             indices = (hkUint16 *)&mesh->Data->Triangles[mesh->Data->Groups[gi].RenderBase];

#ifdef TRANSGAMING_DEINDEX
                        else{
                            indices = generateDummyIndex(mesh->Data->Groups[gi].RenderCount + 2, mesh->Data->Groups[gi].RenderBase);
                            dummyIndices.Add(indices);
                        }
#endif

                        hkMeshShape::Subpart    part;
                        int                     stride;
                        float *                 positions = (float *)mesh->Data->VertList.getVertPos(0, &stride);
                        Assert(positions && stride);


                        part.m_vertexBase = positions;
                        //part.m_vertexStriding = (int)(((char*)&(mesh->Verts[1].Pos)) - ((char*)&(mesh->Verts[0].Pos)));
                        part.m_vertexStriding = stride;
                        part.m_numVertices = mesh->Data->Groups[gi].VertexCount;
#ifdef HAVOK3
                        part.m_indexBase = indices;
#else
                        part.m_index16Base = (hkUint16*)&(mesh->Data->Triangles[0]);
#endif
                        part.m_indexStriding = sizeof(WORD);
                        part.m_numTriangles = mesh->Data->Groups[gi].RenderCount;
#ifdef HAVOK3
                        part.m_stridingType = hkMeshShape::INDICES_INT16;
#else
                        part.m_stridingType = hkMeshShape::HK_INT16_INDICES;
#endif
                        meshShape->addSubpart( part );
                    }
                }
                else
                {
#ifdef TRANSGAMING_DEINDEX
                    // De-indexed
                    if ( !mesh->isIndexed )
                    {
                        Assert(mesh->Data->Triangles.Size() == 0);

                        triangleCount = mesh->Data->VertList.Size()/3;
                        indices = generateDummyIndex(mesh->Data->VertList.Size());
                        dummyIndices.Add(indices);
                    }
                    // Indexed
                    else
                    {
#endif
                        triangleCount = mesh->Data->Triangles.Size()/3;
                        indices = (hkUint16*)&mesh->Data->Triangles[0];
#ifdef TRANSGAMING_DEINDEX
                    }
#endif


                    {
                        hkMeshShape::Subpart    part;
                        int                     stride;
                        float *                 positions = (float *)mesh->Data->VertList.getVertPos(0, &stride);
                        Assert(positions && stride);


                        part.m_vertexBase = positions;
                        //part.m_vertexStriding = (int)(((char*)&(mesh->Verts[1].Pos)) - ((char*)&(mesh->Verts[0].Pos)));
                        part.m_vertexStriding = stride;
                        part.m_numVertices = mesh->Data->VertList.Size();
#ifdef HAVOK3
                        part.m_indexBase = indices;
#else
                        part.m_index16Base = (hkUint16*)&(mesh->Data->Triangles[0]);
#endif
                        part.m_indexStriding = 3 * sizeof(WORD);
                        part.m_numTriangles = triangleCount;
#ifdef HAVOK3
                        part.m_stridingType = hkMeshShape::INDICES_INT16;
#else
                        part.m_stridingType = hkMeshShape::HK_INT16_INDICES;
#endif
                        meshShape->addSubpart( part );
                    }

                }

#else
				const int vertexCount = mesh->Data->VertList.Size();
				const int triangleCount = mesh->Data->Triangles.Size()/3;
	
                {
					hkMeshShape::Subpart    part;
                    int                     stride;
                    float *                 positions = (float *)mesh->Data->VertList.getVertPos(0, stride);
                    Assert(positions && stride);


					part.m_vertexBase = positions;
					//part.m_vertexStriding = (int)(((char*)&(mesh->Verts[1].Pos)) - ((char*)&(mesh->Verts[0].Pos)));
                    part.m_vertexStriding = stride;
					part.m_numVertices = vertexCount;
#ifdef HAVOK3
					part.m_indexBase = (hkUint16*)&(mesh->Data->Triangles[0]);
#else
					part.m_index16Base = (hkUint16*)&(mesh->Data->Triangles[0]);
#endif
					part.m_indexStriding = 3*sizeof(WORD);
					part.m_numTriangles = triangleCount;
#ifdef HAVOK3
                    part.m_stridingType = hkMeshShape::INDICES_INT16;
#else
					part.m_stridingType = hkMeshShape::HK_INT16_INDICES;
#endif
					meshShape->addSubpart( part );
				}
#endif



#ifdef HAVOK3 // NO SWEEP SHAPE!
				ourShape = meshShape;
#else
				m_setTag("Physics");
				ourShape = new hkSweepBvTreeShape( meshShape );
				meshShape->removeReference();
#endif
				// Compute mass properties
				float mass = 10;
				volumeMass = mass;
				hkGeometry geom;
#ifdef HAVOK3
				hkGeometry::Triangle ind;
#else
				hkTriangleIndices ind;
#endif
				hkArray<hkVector4> vertsTemp;
				for( int i = 0; i < mesh->Data->VertList.Size(); ++i )
				{
                    Vector      pos = mesh->Data->VertList.getVertPos(i);
					hkVector4   vec( pos.X * scale, pos.Y * scale, pos.Z * scale, 0 );


					vertsTemp.pushBack( vec );
				}

#ifdef HAVOK3
				geom.m_vertices = vertsTemp;
#else
				geom.addVertices( vertsTemp.begin(),vertsTemp.getSize() );
#endif
				for( int i = 0; i < mesh->Data->Triangles.Size()/3; ++i )
				{
#ifdef HAVOK3
					ind.set(mesh->Data->Triangles[3*i],mesh->Data->Triangles[3*i+1],mesh->Data->Triangles[3*i+2]);
					geom.m_triangles.pushBack( ind );
#else
					ind.setTriangleIndices(mesh->Data->Triangles[3*i],mesh->Data->Triangles[3*i+1],mesh->Data->Triangles[3*i+2]);
					geom.addTriangles( &ind, 1 );
#endif
				}

				hkInertiaTensorComputer::computeGeometryVolumeMassProperties( &geom, mass, massProperties );

				// Adjust mass, based on volume
				mass = 100*fabs(massProperties.m_volume);
				volumeMass = mass;
#ifdef HAVOK3
				HavokWorld::scaleMassProperties( mass/10.0f, massProperties );
#else
				hkInertiaTensorComputer::scaleMassProperties( mass/10.0f, massProperties );
#endif
			}
			break;
		}
	}

#ifdef HAVOK3
	hkRigidBodyCinfo rigidBodyInfo;
#else
	hkRigidBodyConstructionInfo rigidBodyInfo;
#endif
	rigidBodyInfo.m_shape = ourShape;
	rigidBodyInfo.m_position.set(0.0f, 0.0f, 0.0f);
	rigidBodyInfo.m_angularDamping = realDampingAngular;
	rigidBodyInfo.m_linearDamping = realDampingLinear;
#ifdef HAVOK3
	rigidBodyInfo.m_motionType = hkMotion::MOTION_STABILIZED_BOX_INERTIA;
#else
	rigidBodyInfo.m_motionType = HK_STABILIZED_BOX_INERTIA_MOTION;
#endif
	rigidBodyInfo.m_collisionFilterInfo = hkGroupFilter::calcFilterInfo( (collisionGroup == -1)?HCGNormal:collisionGroup );
	rigidBodyInfo.m_inertiaTensor = massProperties.m_inertiaTensor;
	rigidBodyInfo.m_centerOfMass = massProperties.m_centerOfMass;
	rigidBodyInfo.m_mass = massProperties.m_mass;

#ifdef HAVOK_POSITION_BUG_TRACKING
	Assert( massProperties.m_mass >= 0.0f );
#endif

    m_setTag("Physics");
	hkRigidBody* rigidbody = new hkRigidBody( rigidBodyInfo );
/*
	if( makeSweep )
	{
        m_setTag("Physics");
		hkConvexSweepShape* css = new hkConvexSweepShape( static_cast<const hkConvexShape*>(rigidbody->getCollidable().getShape()), rigidbody, 15 );
		rigidbody->getCollidable().getShape()->removeReference();
		rigidbody->getCollidable().setShape( css );
	}
*/

	ourShape->removeReference();


    for (int i = 0; i < dummyIndices.Size(); i++)
        destroyDummyIndex(dummyIndices[i]);


	return rigidbody;
}
/*
hkRigidBody* HavokWorld::CreatePawnBodyFromHKE( Entity* entity, const char* fileName, float scale )
{
	hkMatrix3 scale3;
	scale3.setDiagonal(scale,scale,scale);

	// setup toolkit
	hkWorldConstructionInfo info;
	info.setupSolverInfo( HK_SOLVER_TYPE_4ITERS_MEDIUM );
	info.setBroadPhaseWorldSize( 3000.0f );
	info.m_collisionTolerance = 0.1f;
    m_setTag("Physics");
	hkWorld* tempWorld = new hkWorld( info );
    m_setTag("Physics");
	ExportWorld* exportWorld = new ExportWorld( tempWorld );
	hkIfstream is( fileName );
	if( !is.isOk() ) return NULL;

	DynamicArray<hkRigidBody*> oldRB;
	DynamicArray<hkRigidBody*> newRB;

	CheckLog log;
	
	NS_ExportStream::InExportStream instream( is );
	exportWorld->Import( instream, log );

	float massSum = 0;
	hkArray<hkShape*> shapeArray;

	for( int i = 0; i < exportWorld->GetNumRigidBodies(); ++i )
	{
		ExportRigidBody* exportBody = exportWorld->GetRigidBody(i);
		hkRigidBody* exportRigidBody = exportBody->GetHKRigidBody();

		const char *name = exportBody->GetName();

		bool isFixed = false;
		if( exportRigidBody->isFixed() || strstr( name, FIXED_BONES_SUBSTRING ) )
			isFixed = true;

		Assert( exportBody->GetNumPrimitives() == 1 );
		ExportPrimitive* primitive = exportBody->GetPrimitive(0);

		// My own addititon!
		hkStorageMeshShape* shape = primitive->GetHKSourceShape();

		const hkArray<hkVector4>& verts = shape->getVertices();
		const int vertNum = verts.getSize();

        m_setTag("Physics");
		float* table = new float[3*vertNum];
		for( int j = 0; j < vertNum; ++j )
		{
			table[3*j+0] = verts[j](0)*scale;
			table[3*j+1] = verts[j](1)*scale;
			table[3*j+2] = verts[j](2)*scale;
		}

		// Create internal shape
        m_setTag("Physics");
		hkConvexVerticesShape* vertShape = new hkConvexVerticesShape( table, 3*sizeof(float), vertNum );

		// Calculate mass
		// Take arbitrary mass
		float mass = 10;
		hkMassProperties massProperties;
		// Compute properties to get volume
		hkInertiaTensorComputer::computeVertexHullVolumeMassProperties( table, 3*sizeof(float), vertNum, mass, massProperties );
		massSum += 600*fabs(massProperties.m_volume);

		shapeArray.pushBack( vertShape );
	}

    m_setTag("Physics");
	hkListShape* shape = new hkListShape( shapeArray.begin(), shapeArray.getSize() );

	for( int i = 0; i < shapeArray.getSize(); ++i )
		shapeArray[i]->removeReference();

	hkRigidBodyConstructionInfo rigidBodyInfo;
	rigidBodyInfo.m_shape = shape;
	//rigidBodyInfo.m_restitution = 0.000001f;
	rigidBodyInfo.m_position.set(0.0f, 0.0f, 0.0f);
	rigidBodyInfo.m_angularDamping = 0.05f;
	rigidBodyInfo.m_linearDamping = 0.05f;
	rigidBodyInfo.m_friction = 2.0f;
	rigidBodyInfo.m_motionType = HK_BOX_INERTIA_MOTION;
	rigidBodyInfo.m_collisionFilterInfo = hkGroupFilter::calcFilterInfo( HCGNormal );

	hkReal mass = massSum;
	hkMassProperties massProperties;
	hkInertiaTensorComputer::computeShapeVolumeMassProperties( shape, mass, massProperties );
	rigidBodyInfo.m_inertiaTensor = massProperties.m_inertiaTensor;
	rigidBodyInfo.m_centerOfMass = massProperties.m_centerOfMass;
	rigidBodyInfo.m_mass = massProperties.m_mass;

#ifdef HAVOK_POSITION_BUG_TRACKING
	Assert( massProperties.m_mass >= 0.0f );
#endif

    m_setTag("Physics");
	hkRigidBody* rigidbody = new hkRigidBody( rigidBodyInfo );
	
	shape->removeReference();

	delete exportWorld;
	tempWorld->removeReference();

	return rigidbody;
}
*/

hkMoppCode* HavokWorld::LoadMOPPCode( String path )
{
#ifndef HAVOK3 // ===================================================
/*
	// It is up to you to ensure any "streamed" MOPP data
	// files are in sync with the hkMoppBvTreeShape you assign them to, otherwise collision detection
	// will fail. It may be advisable to "hash" the shape into a checksum, and write this as additional
	// data into the file, to avoid any such conflicts.
	hkString path2 = path.AsChar();
	hkStreambuf* sb = hkStreambufFactory::getInstance().open( path2.cString(), hkStreambuf::INPUT );
	if( sb->isOk() )
	{
		// Create an archive from the streambuf
		hkIArchive inputArchive(sb);
		
		// Read mopp data in. This method allocates the hkMoppCode too. N.B. The MOPP actually holds its
		// data *immediately* after the class itself, so the number of bytes actually allocated is NOT
		// sizeof(hkMoppCode), but rather sizeof(hkMoppCode) + [number bytes required for byte data]
		// which is stored in the class as m_totalSize. You can query this using the getSize() method of
		// hkMoppCode. The code initially has reference count of 1.

		hkMoppCode* code = hkMoppCodeStreamer::readMoppCodeFromArchive( inputArchive );
		sb->removeReference();
		return code;
	}
	else
	{
		sb->removeReference();
		return HK_NULL;
	}
*/

	FileObject* file = GFileMan.CreateFileReader( path.AsChar() );
	if( !file ) return 0;

	// Read in the header info
	float temp;
	hkVector4 header(1e10f, 1e10f, 1e10f);
	*file >> temp; header(0) = temp;
	*file >> temp; header(1) = temp;
	*file >> temp; header(2) = temp;
	*file >> temp; header(3) = temp;

	// Read in the MOPP byte code size
	int byteCodeSize;
	*file >> byteCodeSize;

	// Put in some very basic "safety" checks here to detect file corruption. It is "unlikely" that
	// the offset values will be very large in magnitude.
	Assert( (header(0) > -1e9f) && (header(0) < 1e9f) );
	Assert( (header(1) > -1e9f) && (header(1) < 1e9f) );
	Assert( (header(2) > -1e9f) && (header(2) < 1e9f) );

	// Also, assume size of data less than ~100 meg! This may be overly conservative!
	Assert( byteCodeSize < 10000000 );
	
	// Calculate the overall MOPP code size (header + byte code). The MOPP actually holds its
	// data *immediately* after the class itself, hence when allocating a hkMoppCode structure
	// it is also necessary to allocate enough space for the code at the same time. The total space
	// required is hence sizeof(hkMoppCode) + [number bytes required for byte data]
	int size = hkSizeOf(hkMoppCode) + byteCodeSize;	

	// Allocate a new MOPP code
	hkMoppCode* code = reinterpret_cast<hkMoppCode*>( hkAllocate<char>(size, HK_MEMORY_CLASS_TOOLS) );
	
	// Fill in MoppCode structure data
#ifndef HAVOK3
	code->initializeExternally(size);	// You MUST call this on a hkMoppCode object if you're not using the hkMoppUtility/hkMoppCompiler
#endif

#ifdef HAVOK3
	code->m_info.m_offset(0) = header(0);
	code->m_info.m_offset(1) = header(1);
	code->m_info.m_offset(2) = header(2);
	code->m_info.m_offset(3) = header(3);
#else
	code->m_info.offset(0) = header(0);
	code->m_info.offset(1) = header(1);
	code->m_info.offset(2) = header(2);
	code->m_info.offset(3) = header(3);
#endif

	// Read in the byte code
#ifndef HAVOK3 // PROBLEM HERE.........THIS FUNC WON'T WORK without below...
#ifdef _DEBUG
	Assert( file->Read( (void*)code->getProgramStart(), byteCodeSize ) == byteCodeSize );
#else
	file->Read( (void*)code->getProgramStart(), byteCodeSize );
#endif
#endif // HAVOK3
	GFileMan.Close(file);

	return code;
#else // HAVOK3

	// Load up the MOPPS if you got 'em.  If not, we'll end up generating them as a backup.
	// Eventually, we'll just load up .hkx's, so this won't be used later.
	hkIArchive inputArchive(path.AsChar());
	if ( inputArchive.isOk() )
	{
		hkMoppCode* code = hkMoppCodeStreamer::readMoppCodeFromArchive(inputArchive);
		HK_ASSERT2(0x15d0655f, code != HK_NULL, "There was a problem loading the hkMoppCode");
		return code;
	}
	else
	{
		return HK_NULL;
	}

#endif
}

void HavokWorld::SaveMOPPCode( String path, hkMoppCode* code )
{
#ifndef HAVOK3 // ============================================
/*
	// Now write out to file
	hkString path2 = path.AsChar();
	hkStreambuf* sb = hkStreambufFactory::getInstance().open( path2.cString(), hkStreambuf::OUTPUT );
	if( sb->isOk() )
	{
		// Create an archive
		hkOArchive outputArchive(sb);

		// Write mopp data out.
		hkMoppCodeStreamer::writeMoppCodeToArchive( code, outputArchive );
	}
	sb->removeReference();
*/
	FileObject* file = GFileMan.CreateFileWriter( path.AsChar() );
	if( !file ) return;

	// Write out the header info
	float temp;
#ifdef HAVOK3
	temp = code->m_info.m_offset(0); *file << temp;
	temp = code->m_info.m_offset(1); *file << temp;
	temp = code->m_info.m_offset(2); *file << temp;
	temp = code->m_info.m_offset(3); *file << temp;
#else
	temp = code->m_info.offset(0); *file << temp;
	temp = code->m_info.offset(1); *file << temp;
	temp = code->m_info.offset(2); *file << temp;
	temp = code->m_info.offset(3); *file << temp;
#endif
	// Write out the MOPP byte code size
	int byteCodeSize = code->getCodeSize(); *file << byteCodeSize;

	// Write out the byte code
	file->Write( code->getProgramStart(), byteCodeSize );
	GFileMan.Close(file);
#else

	hkOArchive outputArchive( path.AsChar() );
	if ( outputArchive.isOk() )
	{
		hkMoppCodeStreamer::writeMoppCodeToArchive(code, outputArchive);
	}

#endif // ifndef HAVOK3 =========================================
}

void HavokWorld::EnableUnderwaterWorld( bool enable )
{
	if( enable && !mIsUnderwater )
	{
		// Basic world parameters
		hkVector4 gravity(0.0f, -2*GEngine->Physics->TWUnderwaterGravity, 0.0f);
		World->setGravity( gravity );
//		World->setLowFrequencyDeactivationPeriod( 2.0f );

		// Increase damping by arbitrary factor

		//	- for Physics Objects

		float raiseLinear = GEngine->Physics->TWUnderwaterDampingRaiseLinear;
		float raiseAngular = GEngine->Physics->TWUnderwaterDampingRaiseAngular;

		for( int i = 0; i < PhysicsObjects.Size(); ++i )
		{
			hkRigidBody* rb = PhysicsObjects[i]->mHavokPart->Body;
			if( rb )
			{
				Assert( rb->getSimulationIsland() );
				hkReal linDamping = rb->getLinearDamping();
				hkReal diff = 1.0 - linDamping;
				diff /= raiseLinear;
				rb->setLinearDamping( 1.0 - diff );

				hkReal angDamping = rb->getAngularDamping();
				diff = 1.0 - angDamping;
				diff /= raiseAngular;
				rb->setAngularDamping( 1.0 - diff );
			}
			hkPropertyValue p = rb->getProperty( HPEntityPointer );
#ifdef HAVOK3
			Entity* entity = (Entity*)p.getPtr();
#else
			Entity* entity = (Entity*)p.asPtr;
#endif
			if( entity && entity->PO && ( entity->PO->IsPlayer() ) )
				entity->PO->EnableGravity( false );
		}

		//	- for Ragdolls

		for( int i = 0; i < Ragdolls.Size(); ++i )
			Ragdolls[i]->EnableUnderwaterWorld( enable, raiseLinear, raiseAngular );

		mIsUnderwater = true;
	}
	else if ( !enable && mIsUnderwater )
	{
		// Basic world parameters
		hkVector4 gravity(0.0f,-GEngine->Physics->TWGravity, 0.0f);
		World->setGravity( gravity );
//		World->setLowFrequencyDeactivationPeriod( 10.0f );

		float raiseLinear = GEngine->Physics->TWUnderwaterDampingRaiseLinear;
		float raiseAngular = GEngine->Physics->TWUnderwaterDampingRaiseAngular;

		// Decrease damping by arbitrary factor

		//	- for Physics Objects

		for( int i = 0; i < PhysicsObjects.Size(); ++i )
		{
			hkRigidBody* rb = PhysicsObjects[i]->mHavokPart->Body;
			if( rb )
			{
				Assert( rb->getSimulationIsland() );
				hkReal linDamping = rb->getLinearDamping();
				hkReal diff = 1.0 - linDamping;
				diff *= raiseLinear;
				rb->setLinearDamping( 1.0 - diff );

				hkReal angDamping = rb->getAngularDamping();
				diff = 1.0 - angDamping;
				diff *= raiseAngular;
				rb->setAngularDamping( 1.0 - diff );
			}
			hkPropertyValue p = rb->getProperty( HPEntityPointer );
#ifdef HAVOK3
			Entity* entity = (Entity*)p.getPtr();
#else
			Entity* entity = (Entity*)p.asPtr;
#endif
			if( entity && entity->PO && ( entity->PO->IsPlayer() ) )
				entity->PO->EnableGravity( true );
		}

		//	- for Ragdolls

		for( int i = 0; i < Ragdolls.Size(); ++i )
			Ragdolls[i]->EnableUnderwaterWorld( enable, raiseLinear, raiseAngular );

		mIsUnderwater = false;
	}
}

void HavokWorld::PrecacheRagdoll( Model* model, const char* fileName )
{
	HavokRagdollTemplate* templ = FindRagdollTemplate( model, fileName );
	if( !templ )
		templ = CreateRagdollTemplate( model, fileName );
}

HavokRagdoll* HavokWorld::CreateRagdoll( Model *model, RagdollSkeleton* ragdollSkeleton, const char* fileName, float scale, int collisionGroup )
{
	HavokRagdollTemplate* templ = FindRagdollTemplate( model, fileName );
	if( !templ )
		templ = CreateRagdollTemplate( model, fileName );

	HavokRagdoll* r = 0;
	if( templ )
		r = CreateRagdollFromTemplate( templ, model, ragdollSkeleton, scale, collisionGroup );
	return r;
}

hkRigidBody* HavokWorld::CreatePawnBodyFromRagdoll( Model *model, float& mVolumeMass, float scale, int collisionGroup )
{
	String modelsDir = String("../Data/Models/")+model->Mesh->Name+".hke";

	HavokRagdollTemplate* templ = FindRagdollTemplate( model, modelsDir.AsChar() );
	if( !templ )
		templ = CreateRagdollTemplate( model, modelsDir.AsChar() );

	hkRigidBody* r = 0;
	if( templ )
		r = CreatePawnBodyFromRagdollTemplate( templ, scale, mVolumeMass, collisionGroup );
	return r;
}

HavokRagdollTemplate* HavokWorld::FindRagdollTemplate( Model *model, String fileName )
{
	for( int i = 0; i < RagdollTemplates.Size(); ++i )
		if( ( model->FileName == RagdollTemplates[i]->ModelFile ) &&
			( RagdollTemplates[i]->RagdollFile == fileName ) )
			return RagdollTemplates[i];
	return 0;
}



HavokRagdollTemplate* HavokWorld::CreateRagdollTemplate( Model *model, const char* fileName )
{
#ifndef HAVOK3 // ============================================

	char str1[10];	// 'mp-model-'
	char str2[8];	// 'gib.hke'

	str1[1] = 'p';
	str1[7] = 'l';
	str1[9] = 0;
	str1[3] = 'm';
	str1[8] = '-';
	str1[2] = '-';
	str1[4] = 'o';
	str1[0] = 'm';
	str1[6] = 'e';
	str1[5] = 'd';

	str2[7] = 0;
	str2[3] = '.';
	str2[6] = 'e';
	str2[2] = 'b';
	str2[4] = 'h';
	str2[0] = 'g';
	str2[1] = 'i';
	str2[5] = 'k';

	// check crc for special mp ragdolls
	if( ( strstr( fileName, str1 ) != 0 ) &&
		( strstr( fileName, str2 ) == 0 ) )
	{
		FileObject* file = GFileMan.CreateFileReader( fileName );

		if( !file )
			return 0;

		long32 size = file->GetSize();
        m_setTag("Physics");
		char* buffer = new char[size];
		file->Read( buffer, size );
		DWORD crc = Crc32( buffer, size );
		GFileMan.Close(file);
		delete[] buffer;

		if( crc != 1852776096 )
		{
#ifndef TRANSGAMING_NO_DEDICATED_SERVER 
			if( GEngine->GameState == PCFSystem::DedicatedServer )
			{
				MessageBox( NULL, "Critical error: error in ragdoll hke!!!", NULL, MB_OK );
				GScript.DoString( "Exit()" );
			}
			else
#endif
				GScript.DoString( "PMENU.ShowMenu(); PainMenu:ShowInfo( 'Critical error: error in ragdoll hke!!!', 'Exit()' )" );

			return 0;
		}
	}

	// setup toolkit
#ifdef HAVOK3
	hkWorldCinfo info;
	info.setupSolverInfo( hkWorldCinfo::SolverType::SOLVER_TYPE_4ITERS_MEDIUM );
#else
	hkWorldConstructionInfo info;
	info.setupSolverInfo( HK_SOLVER_TYPE_4ITERS_MEDIUM );
#endif
	info.setBroadPhaseWorldSize( 3000.0f );
	info.m_collisionTolerance = 0.1f;
    m_setTag("Physics");
	hkWorld* tempWorld = new hkWorld( info );
    m_setTag("Physics");
	ExportWorld* exportWorld = new ExportWorld( tempWorld );
	hkIfstream is( fileName );
	bool hasRagdoll = is.isOk();
	HavokRagdollTemplate* rgdTempl = 0;
	if( hasRagdoll )
	{
		DynamicArray<hkRigidBody*> oldRB;

        m_setTag("Physics");
		rgdTempl = new HavokRagdollTemplate( model->FileName, String(fileName) );

		BindSkeleton &bindSkeleton = model->Mesh->Skeleton;

		//
		// Import the world
		//

		CheckLog log;
	
		NS_ExportStream::InExportStream instream( is );
		exportWorld->Import( instream, log );

		bool notCreatedSome = false;

		if( !exportWorld->GetNumRigidBodies() )
//		{
			GLog.Print( "Warning: ragdoll '%s' invalid or has no rigid bodies\n", fileName );
//			for( int i = 0; i < log.NumberOfLines(); ++i )
//				GLog.Print( log.GetLine(i) );
//		}

		for( int i = 0; i < exportWorld->GetNumRigidBodies(); ++i )
		{
			HavokRagdollRigidBodyInfo rbInfo;

			ExportRigidBody* exportBody = exportWorld->GetRigidBody(i);
			hkRigidBody* exportRigidBody = exportBody->GetHKRigidBody();

			const char *name = exportBody->GetName();
			rbInfo.Name = name;
			rbInfo.AngularDamping = 0.05f;
			rbInfo.LinearDamping = 0.05f;
			rbInfo.Friction = 2.5f;
			rbInfo.Restitution = 0.4f;
			rbInfo.MassFromDef = false;

			rbInfo.Fixed = false;
			if( exportRigidBody->isFixed() || strstr( name, FIXED_BONES_SUBSTRING ) )
				rbInfo.Fixed = true;

			rbInfo.MassFromHKE = false;
			if( strstr( name, MASS_FROM_HKE_SUBSTRING ) )
				rbInfo.MassFromHKE = true;

			int joint = bindSkeleton.JointIndex( name );
			if( joint >= 0 )
			{
				Assert( exportBody->GetNumPrimitives() == 1 );
				ExportPrimitive* primitive = exportBody->GetPrimitive(0);

				// My own addititon!
				hkStorageMeshShape* shape = primitive->GetHKSourceShape();

				const hkArray<hkVector4>& verts = shape->getVertices();
				const int vertNum = verts.getSize();

				for( int j = 0; j < vertNum; ++j )
					rbInfo.Verts.Add( Vector( verts[j](0), verts[j](1), verts[j](2) ) );

				rbInfo.Mass = exportRigidBody->getMass();
#ifdef HAVOK_POSITION_BUG_TRACKING
				Assert( rbInfo.Mass >= 0.0f );
#endif

				hkVector4 pos;
				pos = exportRigidBody->getPosition();
				rbInfo.Position.X = pos(0);
				rbInfo.Position.Y = pos(1);
				rbInfo.Position.Z = pos(2);

				const hkTransform& T = exportRigidBody->getTransform();
				T.get4x4ColumnMajor( &rbInfo.Transform.mArray[0] );

				oldRB.Add( exportRigidBody );
				rgdTempl->RigidBodies.Add( rbInfo );
			}
			else
			{
				GLog.Print("CreateRagdollTemplate: %s joint found in %s.hke but not in %s%s\n", 
					name, model->Mesh->Name.AsChar(), model->Mesh->Name.AsChar(), AnimatedMesh::DefaultExt);
				notCreatedSome = true;
			}
		}

		if( notCreatedSome )
		{
			delete rgdTempl;
			return 0;
		}

		//
		// Extract constraints from collection
		//
//		GLog.Print("Ragdoll constraint types: ");

		for( int i = 0; i < exportWorld->GetNumConstraints(); ++i )
		{
			HavokRagdollConstraintData data;
			data.Gen = 0;
			data.Type = 255;

			ExportConstraint* c = exportWorld->GetConstraint(i);
#ifdef HAVOK3
			hkConstraintInstance* constraint = c->GetHKConstraint();
#else
			hkConstraint* constraint = c->GetHKConstraint();
#endif
			data.Breakable = c->GetIsBreakable();

			// Find out bodies' translation
			hkRigidBody* b1 = static_cast<hkRigidBody*>(constraint->getEntityA());
			data.BodyA = -1;

			int j;
			for( j = 0; j < oldRB.Size(); ++j )
			{
				if( oldRB[j] == b1 )
				{
					data.BodyA = j;
					break;
				}
			}
			Assert( data.BodyA != -1 );

			hkRigidBody* b2 = static_cast<hkRigidBody*>(constraint->getEntityB());
			data.BodyB = -1;

			for( j = 0; j < oldRB.Size(); ++j )
			{
				if( oldRB[j] == b2 )
				{
					data.BodyB = j;
					break;
				}
			}
			Assert( data.BodyB != -1 );

			// Create new constraint, scaled appropriately
			String constraintName = c->GetName();

			if( c->ClassNameIs("Ragdoll") )
			{
//				GLog.Print("R");
#ifdef HAVOK3
                //m_setTag("Physics");
				//hkRagdollCInfo* info = new hkRagdollConstructionInfo;
				//ragdollCons->getCInfo( *info );
				hkRagdollConstraintData* info = (hkRagdollConstraintData*)constraint->getData();
#else
				hkRagdollConstraint* ragdollCons = static_cast<hkRagdollConstraint*>(constraint);
                m_setTag("Physics");
				hkRagdollConstructionInfo* info = new hkRagdollConstructionInfo;
				ragdollCons->getConstructionInfo( *info );
#endif
				data.ConstraintInfo = (void*)info;
				data.Type = 0;
			}
			else if( c->ClassNameIs("Hinge") )
			{
				ExportHingeConstraint* exportHinge = static_cast<ExportHingeConstraint*>(c);
				bool limited = exportHinge->GetIsLimited();

				if( limited )
				{
//					GLog.Print("Hl");
#ifdef HAVOK3
					hkLimitedHingeConstraintData* info = (hkLimitedHingeConstraintData*)constraint->getData();
#else
					hkLimitedHingeConstraint* hingeCons = exportHinge->GetHKLimitedHinge();
                    m_setTag("Physics");
					hkLimitedHingeConstructionInfo* info = new hkLimitedHingeConstructionInfo;
					hingeCons->getConstructionInfo( *info );
#endif
					data.ConstraintInfo = (void*)info;
					data.Type = 1;
				}
				else
				{
//					GLog.Print("Hu");
#ifdef HAVOK3
					hkHingeConstraintData* info = (hkHingeConstraintData*)constraint->getData();
#else
					hkHingeConstraint* hingeCons = exportHinge->GetHKHinge();
                    m_setTag("Physics");
					hkHingeConstructionInfo* info = new hkHingeConstructionInfo;
					hingeCons->getConstructionInfo( *info );
#endif
					data.ConstraintInfo = (void*)info;
					data.Type = 2;
				}
			}
			else if( c->ClassNameIs("PointToPoint") )
			{
#ifndef HAVOK3 // ===============================================
				if( c->IsLimitedPointToPoint )
				{
					GLog.Print("HavokWorld::CreateRagdollTemplate - PointToPoint() - FIXME!!!!!!!!!!!\n");
                    m_setTag("Physics");
					data.Gen = new HavokRagdollGenericConstraintData;
					GLog.Print("Pl");
					// Limited
					ExportLimitedPointToPointConstraint* exportLPTP = static_cast<ExportLimitedPointToPointConstraint*>(c);
					hkGenericConstraint* oldLPTP = exportLPTP->GetHKPointToPoint();
					hkVector4 temp;
					hkMatrix3 tempM;

					temp = *oldLPTP->getParameters( exportLPTP->GetPivotAParameterIndex() );
					data.Gen->PivotA.X = temp(0); data.Gen->PivotA.Y = temp(1); data.Gen->PivotA.Z = temp(2);
					temp = *oldLPTP->getParameters( exportLPTP->GetPivotBParameterIndex() );
					data.Gen->PivotB.X = temp(0); data.Gen->PivotB.Y = temp(1); data.Gen->PivotB.Z = temp(2);

					hkVector4 mat1, mat2, mat3;
					// Set the angular basis of each body
					tempM = *(hkMatrix3*)oldLPTP->getParameters( exportLPTP->GetRotationAParameterIndex() );
					tempM.getRows( mat1, mat2, mat3 );
					data.Gen->AngBasisA1.X = mat1(0); data.Gen->AngBasisA1.Y = mat1(1); data.Gen->AngBasisA1.Z = mat1(2);
					data.Gen->AngBasisA2.X = mat2(0); data.Gen->AngBasisA2.Y = mat2(1); data.Gen->AngBasisA2.Z = mat2(2);
					data.Gen->AngBasisA3.X = mat3(0); data.Gen->AngBasisA3.Y = mat3(1); data.Gen->AngBasisA3.Z = mat3(2);
					tempM = *(hkMatrix3*)oldLPTP->getParameters( exportLPTP->GetRotationBParameterIndex() );
					tempM.getRows( mat1, mat2, mat3 );
					data.Gen->AngBasisB1.X = mat1(0); data.Gen->AngBasisB1.Y = mat1(1); data.Gen->AngBasisB1.Z = mat1(2);
					data.Gen->AngBasisB2.X = mat2(0); data.Gen->AngBasisB2.Y = mat2(1); data.Gen->AngBasisB2.Z = mat2(2);
					data.Gen->AngBasisB3.X = mat3(0); data.Gen->AngBasisB3.Y = mat3(1); data.Gen->AngBasisB3.Z = mat3(2);

					// Set the angular limits wrt to each axis
					temp = *oldLPTP->getParameters( exportLPTP->GetXRotationLimitParameterIndex() );
					data.Gen->TempX0 = temp(0); data.Gen->TempX1 = temp(1);
					temp = *oldLPTP->getParameters( exportLPTP->GetYRotationLimitParameterIndex() );
					data.Gen->TempY0 = temp(0); data.Gen->TempY1 = temp(1);
					temp = *oldLPTP->getParameters( exportLPTP->GetZRotationLimitParameterIndex() );
					data.Gen->TempZ0 = temp(0); data.Gen->TempZ1 = temp(1);

					data.ConstraintInfo = 0;
					data.Type = 3;
				}
				else
				{
//					GLog.Print("Pu");
					// Simple
					ExportPointToPointConstraint* exportPTP = static_cast<ExportPointToPointConstraint*>(c);
#ifdef HAVOK3
					hkBallAndSocketConstraintData* info = (hkBallAndSocketConstraintData*)constraint->getData();
#else
					hkBallAndSocketConstraint* bsCons = exportPTP->GetHKPointToPoint();
                    m_setTag("Physics");
					hkBallAndSocketConstructionInfo* info = new hkBallAndSocketConstructionInfo;
					bsCons->getConstructionInfo( *info );
#endif
					data.ConstraintInfo = (void*)info;
					data.Type = 4;
				}
#endif // ==========================================================
			}
			else if( c->ClassNameIs("StiffSpring") )
			{
//				GLog.Print("S");
#ifdef HAVOK3
				hkStiffSpringConstraintData* info = (hkStiffSpringConstraintData*)constraint->getData();
#else
				hkStiffSpringConstraint* ssCons = static_cast<hkStiffSpringConstraint*>(c->GetHKConstraint());
                m_setTag("Physics");
				hkStiffSpringConstructionInfo* info = new hkStiffSpringConstructionInfo;
				ssCons->getConstructionInfo( *info );
#endif
				data.ConstraintInfo = (void*)info;
				data.Type = 5;
			}
			else if( c->ClassNameIs("Prismatic") )
			{
//				GLog.Print("Pr");
#ifdef HAVOK3
				hkPrismaticConstraintData* info = (hkPrismaticConstraintData*)constraint->getData();
#else
				hkPrismaticConstraint* prismatic = static_cast<hkPrismaticConstraint*>(c->GetHKConstraint());
                m_setTag("Physics");
				hkPrismaticConstructionInfo* info = new hkPrismaticConstructionInfo;
				prismatic->getConstructionInfo( *info );
#endif
				data.ConstraintInfo = (void*)info;
				data.Type = 6;
			}
			else if( c->ClassNameIs("Wheel") )
			{
//				GLog.Print("W");
#ifdef HAVOK3
				hkWheelConstraintData* info = (hkWheelConstraintData*)constraint->getData();
#else
				hkWheelConstraint* wheel = static_cast<hkWheelConstraint*>(c->GetHKConstraint());
                m_setTag("Physics");
				hkWheelConstructionInfo* info = new hkWheelConstructionInfo;
				wheel->getConstructionInfo( *info );
#endif
				data.ConstraintInfo = (void*)info;
				data.Type = 7;
			}

			if( data.Type != 255 )
			{
				if( data.Breakable )
				{
#ifdef HAVOK3
					hkBreakableConstraintData* breaking = (hkBreakableConstraintData*)constraint->getData();
#else
					hkBreakableConstraint* breaking = (hkBreakableConstraint*)c->GetHKBreakableConstraint();
#endif
					data.Threshold = breaking->getThreshold();
				}
				rgdTempl->Constraints.Add( data );
			}
		}
//		GLog.Print("\n");

		//
		// Extract actions from collection
		//
//		GLog.Print("Ragdoll action types: ");
		for( int i = 0; i < exportWorld->GetNumActions(); ++i )
		{
			HavokRagdollActionData data;
			data.Type = 255;

			ExportAction* a = exportWorld->GetAction(i);
			hkAction* action = a->GetHKAction();

			if( a->ClassNameIs("FastConstraintSolver") || a->ClassNameIs("Drag") )
				continue;

			// Find out bodies' translation
			hkArray<hkEntity*> entities;
			action->getEntities( entities );

			hkRigidBody* b1 = (hkRigidBody*)entities[0];
			hkRigidBody* b2 = (entities.getSize() > 1)?(hkRigidBody*)entities[1]:0;

			data.BodyA = -1;

			int j;
			for( j = 0; j < oldRB.Size(); ++j )
			{
				if( oldRB[j] == b1 )
				{
					data.BodyA = j;
					break;
				}
			}
			Assert( data.BodyA != -1 );

			data.BodyB = -1;

			for( j = 0; j < oldRB.Size(); ++j )
			{
				if( oldRB[j] == b2 )
				{
					data.BodyB = j;
					break;
				}
			}

			// Create new action, scaled appropriately
			data.Name = a->GetName();

			if( a->ClassNameIs("Dashpot") )
			{
//				GLog.Print("D");
				hkDashpotAction* dashpotAct = static_cast<hkDashpotAction*>(action);
				hkVector4 pivotA = dashpotAct->getPointA();
				data.PivotA.X = pivotA(0); data.PivotA.Y = pivotA(1); data.PivotA.Z = pivotA(2);
				hkVector4 pivotB(0,0,0);
				if( entities.getSize() > 1 ) pivotB = dashpotAct->getPointB();
				data.PivotB.X = pivotB(0); data.PivotB.Y = pivotB(1); data.PivotB.Z = pivotB(2);
				data.Strength = dashpotAct->getStrength();
				data.Damping = dashpotAct->getDamping();
				data.Type = 0;
				rgdTempl->Actions.Add( data );
			}
			else if( a->ClassNameIs("AngularDashpot") )
			{
//				GLog.Print("Ad");
				hkAngularDashpotAction* angDashpotAct = static_cast<hkAngularDashpotAction*>(action);
				hkReal r = angDashpotAct->getRotation().getReal();
				hkVector4 q = angDashpotAct->getRotation().getImag();
				data.Rotation = Quaternion( -r, q(0), q(1), q(2) );
				data.Strength = angDashpotAct->getStrength();
				data.Damping = angDashpotAct->getDamping();
				data.Type = 1;
				rgdTempl->Actions.Add( data );
			}
			else if( a->ClassNameIs("Spring") )
			{
//				GLog.Print("S");
				hkSpringAction* springAct = static_cast<hkSpringAction*>(action);
#ifndef HAVOK3
				hkSpringActionConstructionInfo info;
#endif
				hkVector4 pivotA = springAct->getPositionAinA();
				data.PivotA.X = pivotA(0); data.PivotA.Y = pivotA(1); data.PivotA.Z = pivotA(2);
				hkVector4 pivotB;
				if( entities.getSize() > 1 ) pivotB = springAct->getPositionBinB();
				data.PivotB.X = pivotB(0); data.PivotB.Y = pivotB(1); data.PivotB.Z = pivotB(2);
				data.Strength = springAct->getStrength();
				data.Damping = springAct->getDamping();
				data.RestLength = springAct->getRestLength();
				data.OnCompression = springAct->getOnCompression();
				data.OnExtension = springAct->getOnExtension();
				data.Type = 2;
				rgdTempl->Actions.Add( data );
			}
		}
//		GLog.Print("\n");

//		GLog.Print("Created ragdoll %d\n", iRagdoll );
	}
	delete exportWorld;
	tempWorld->removeReference();

	// Find .rde file, get information from it and add it to the Template
	if( rgdTempl )
	{
		ConfigFile ragdollDef;
		String ragdollDefFile = String("../Data/Models/")+model->Mesh->Name+".rde";
		if( ragdollDef.Load( ragdollDefFile ) )
		{
			const char* str;
			for( int i = 0; i < rgdTempl->RigidBodies.Size(); ++i )
			{
				HavokRagdollRigidBodyInfo& rbInfo = rgdTempl->RigidBodies[i];

				str = ragdollDef.GetString( rbInfo.Name.AsChar(), "Mass" );
				if( str )
				{
#ifdef TRANSGAMING
					float  mass = atof( str );
#else
					double mass = atof( str );
#endif
					if( mass != -1.0 )
					{
						rbInfo.MassFromDef = true;
						rbInfo.Mass = mass;
					}
				}
				str = ragdollDef.GetString( rbInfo.Name.AsChar(), "LinearDamping" );
				if( str ) { rbInfo.LinearDamping = atof( str ); }
				str = ragdollDef.GetString( rbInfo.Name.AsChar(), "AngularDamping" );
				if( str ) { rbInfo.AngularDamping = atof( str ); }
				str = ragdollDef.GetString( rbInfo.Name.AsChar(), "Friction" );
				if( str ) { rbInfo.Friction = atof( str ); }
				str = ragdollDef.GetString( rbInfo.Name.AsChar(), "Restitution" );
				if( str ) { rbInfo.Restitution = atof( str ); }
			}
		}
#ifdef GENERATE_RDE_FILES
		else	// If .rde file not found, create it and fill with information from Template
		{
			String str;

			for( int i = 0; i < rgdTempl->RigidBodies.Size(); ++i )
			{
				HavokRagdollRigidBodyInfo& rbInfo = rgdTempl->RigidBodies[i];

				ragdollDef.AddOption( rbInfo.Name.AsChar(), "Mass", "-1.0" );
				str = String::Sprintf( "%f", rbInfo.LinearDamping );
				ragdollDef.AddOption( rbInfo.Name.AsChar(), "LinearDamping", str.AsChar() );
				str = String::Sprintf( "%f", rbInfo.AngularDamping );
				ragdollDef.AddOption( rbInfo.Name.AsChar(), "AngularDamping", str.AsChar() );
				str = String::Sprintf( "%f", rbInfo.Friction );
				ragdollDef.AddOption( rbInfo.Name.AsChar(), "Friction", str.AsChar() );
				str = String::Sprintf( "%f", rbInfo.Restitution );
				ragdollDef.AddOption( rbInfo.Name.AsChar(), "Restitution", str.AsChar() );
			}
			ragdollDef.Save( ragdollDefFile );
		}
#endif
		RagdollTemplates.Add( rgdTempl );
	}
	return rgdTempl;
#else
	GLog.Print("CreateRagdollTemplate() - FIXME!!!!!!!!!!!\n");
	return HK_NULL;
#endif // ifndef HAVOK3 =========================================
}

HavokRagdoll* HavokWorld::CreateRagdollFromTemplate( HavokRagdollTemplate* ragdollTemplate, Model *model, RagdollSkeleton* ragdollSkeleton, float scale, int collisionGroup )
{
//#define MAKE_RAGDOLL_VERBOSE

#ifndef HAVOK3 // =================================================================

#ifdef MAKE_RAGDOLL_VERBOSE
		GLog.Print( "Scaling: %f\n", scale );
#endif

	DynamicArray<int> oldIndex;
	DynamicArray<hkRigidBody*> newRB;

    m_setTag("Physics");
	HavokRagdoll* rgd = new HavokRagdoll( model, ragdollSkeleton, this );
	Ragdolls.Add( rgd );
	DynamicArray<Limb>& limbs = rgd->Limbs;
	BindSkeleton &bindSkeleton = model->Mesh->Skeleton;

	float massSum = 0;

#ifdef MAKE_RAGDOLL_VERBOSE
	GLog.Print( "Rigid Bodies: %d\n", ragdollTemplate->RigidBodies.Size() );
#endif

	for( int i = 0; i < ragdollTemplate->RigidBodies.Size(); ++i )
	{
		HavokRagdollRigidBodyInfo& rbInfo = ragdollTemplate->RigidBodies[i];

		int joint = bindSkeleton.JointIndex( rbInfo.Name.AsChar() );
		Assert( joint >= 0 );

		// Create internal shape
		hkRigidBody* rigidbody;
		hkArray<hkVector4> planeEquations;
		hkArray<hkVector4> vertsOut;

		// Check if we have planes for this shape in cache
		HavokWorldConvexCache* cache = (HavokWorldConvexCache*)GEngine->Physics->mConvexCache;
		HavokWorldConvexEntry* entry = cache->GetEntry( model->FileName, rbInfo.Name, false );
		if( entry )
		{
#ifdef LOG_CONVEX_CREATION
			GLog.Print( "Model '%s', joint '%s', scale %f taken from cache\n", model->FileName.AsChar(), rbInfo.Name.AsChar(), scale );
			GLog.Flush();
#endif
			planeEquations = entry->planeEquations;
			vertsOut = entry->vertsOut;
			int planeEqSize = planeEquations.getSize();
			if( ( planeEqSize >= 4 ) && ( entry->scale != scale ) )
			{
				int vertsOutSize = vertsOut.getSize();
				Assert(entry->scale);
#ifdef TRANSGAMING
				float  multiplier = scale/entry->scale;
#else
				double multiplier = scale/entry->scale;
#endif
				for( int j = 0; j < planeEqSize; ++j )
					planeEquations[j](3) *= multiplier;
				for( int j = 0; j < vertsOutSize; ++j )
					vertsOut[j].mul4( multiplier );
			}
		}
		else
		{
			const int vertNum = rbInfo.Verts.Size();
			hkArray<hkVector4> verts;
			for( int j = 0; j < vertNum; ++j )
			{
				hkVector4 vec( rbInfo.Verts[j].X*scale, rbInfo.Verts[j].Y*scale, rbInfo.Verts[j].Z*scale, 0 );
				verts.pushBack( vec );
			}

			// Check if we can make a smaller set for all subsequent operations
			hkArray<hkVector4> verts2;
			verts2 = verts;
			hkGeometryFilterer::removeCloseVertices( verts2, 0.0001f );
			if( ( verts2.getSize() > 3 ) && !hkGeometryChecker::isCoplanarSet( verts2, 0.00000000001f ) ) verts = verts2;
			verts2.clearAndDeallocate();

#ifdef LOG_CONVEX_CREATION
# ifdef TRANSGAMING
			float  firstTime = GEngine->SysDriver.GetCurrentTime();
# else
			double firstTime = GEngine->SysDriver.GetCurrentTime();
# endif
			GLog.Print( "Creating convex for ragdoll pawn '%s'", rbInfo.Name.AsChar() );
			GLog.Flush();
#endif

			hkGeometryUtil::computePlanesFromVertices( &verts[0](0), verts.getSize(), sizeof(hkVector4), planeEquations, vertsOut );

#ifdef LOG_CONVEX_CREATION
			GLog.Print( ", duration: %f\n", GEngine->SysDriver.GetCurrentTime()-firstTime );
			GLog.Flush();
#endif

			cache->PutEntry( model->FileName, rbInfo.Name, planeEquations, vertsOut, scale, false );
#ifdef LOG_CONVEX_CREATION
			GLog.Print( "Ragdoll '%s', joint '%s', scale %f entered into cache\n", model->FileName.AsChar(), rbInfo.Name.AsChar(), scale );
			GLog.Flush();
#endif
		}

		if( planeEquations.getSize() >= 4 )
		{
            m_setTag("Physics");
			hkConvexVerticesShape* vertShape = new hkConvexVerticesShape( &vertsOut[0](0), sizeof(hkVector4), vertsOut.getSize(), planeEquations );

			// Calculate inertia tensor
			hkMassProperties massProperties;

			if( rbInfo.MassFromDef )
			{
				float mass = rbInfo.Mass;
				if( mass == 0.0 ) mass = 10.0f;

				// Compute properties to get volume
				hkInertiaTensorComputer::computeVertexHullVolumeMassProperties( &vertsOut[0](0), sizeof(hkVector4), vertsOut.getSize(), mass, massProperties );
			}
			else if( rbInfo.MassFromHKE )
			{
				float mass = rbInfo.Mass;
				mass *= scale*scale*scale;

				if( mass == 0.0 ) mass = 10.0f;

				// Compute properties to get volume
				hkInertiaTensorComputer::computeVertexHullVolumeMassProperties( &vertsOut[0](0), sizeof(hkVector4), vertsOut.getSize(), mass, massProperties );
			}
			else
			{
				// Take arbitrary mass
				float mass = 10;
				// Compute properties to get volume
				hkInertiaTensorComputer::computeVertexHullVolumeMassProperties( &vertsOut[0](0), sizeof(hkVector4), vertsOut.getSize(), mass, massProperties );
				// Adjust mass, based on volume
				mass = 600*fabs(massProperties.m_volume);
				// Make sure that mesh mass is minimum 10
				mass = (mass < 10.0f)?10.0f:mass;
				// Compute properties again to get final inertia tensor, mass, and center of mass ( just in case )
				hkInertiaTensorComputer::computeVertexHullVolumeMassProperties( &vertsOut[0](0), sizeof(hkVector4), vertsOut.getSize(), mass, massProperties );
			}

			// Set position
			hkVector4 pos( rbInfo.Position.X, rbInfo.Position.Y, rbInfo.Position.Z );
			pos.mul4( scale );
#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
			if( !pos.isOk4() || ( fabs(pos(0)) > 10000.0f ) || ( fabs(pos(1)) > 10000.0f ) || ( fabs(pos(2)) > 10000.0f ) )
#else
			if( !pos.isOk() || ( fabs(pos(0)) > 10000.0f ) || ( fabs(pos(1)) > 10000.0f ) || ( fabs(pos(2)) > 10000.0f ) )
#endif
				GLog.Print( "Position > 10000 in CreateRagdollFromTemplate\n" );
#endif
#ifdef HAVOK3
			hkRigidBodyCinfo rigidBodyInfo;
			rigidBodyInfo.m_shape = (hkConvexVerticesShape*)vertShape;
#else
			hkRigidBodyConstructionInfo rigidBodyInfo;
			rigidBodyInfo.m_shape = vertShape;
#endif
			rigidBodyInfo.m_position = pos;
			rigidBodyInfo.m_angularDamping = rbInfo.AngularDamping;
			rigidBodyInfo.m_linearDamping = rbInfo.LinearDamping;
			rigidBodyInfo.m_friction = rbInfo.Friction;
			rigidBodyInfo.m_restitution = rbInfo.Restitution;
#ifdef HAVOK3
			rigidBodyInfo.m_motionType = hkMotion::MOTION_STABILIZED_BOX_INERTIA;
#else
			rigidBodyInfo.m_motionType = HK_STABILIZED_BOX_INERTIA_MOTION;
#endif
			rigidBodyInfo.m_collisionFilterInfo = hkGroupFilter::calcFilterInfo( ( collisionGroup == -1 )?HCGRagdoll:collisionGroup );
			rigidBodyInfo.m_inertiaTensor = massProperties.m_inertiaTensor;
			rigidBodyInfo.m_centerOfMass = massProperties.m_centerOfMass;
			rigidBodyInfo.m_mass = massProperties.m_mass;
			massSum += massProperties.m_mass;

#ifdef MAKE_RAGDOLL_VERBOSE
			GLog.Print( "Joint index: %d\n", joint );
			GLog.Print( "Linear damping: %f\n", rbInfo.LinearDamping );
			GLog.Print( "Angular damping: %f\n", rbInfo.AngularDamping );
			GLog.Print( "Friction: %f\n", rbInfo.Friction );
			GLog.Print( "Restitution: %f\n", rbInfo.Restitution );
			GLog.Print( "Inertia Tensor: (%f,%f,%f,%f,%f,%f,%f,%f,%f)\n", 
				massProperties.m_inertiaTensor(0,0), massProperties.m_inertiaTensor(0,1), massProperties.m_inertiaTensor(0,2),
				massProperties.m_inertiaTensor(1,0), massProperties.m_inertiaTensor(1,1), massProperties.m_inertiaTensor(1,2),
				massProperties.m_inertiaTensor(2,0), massProperties.m_inertiaTensor(2,1), massProperties.m_inertiaTensor(2,2) );
			GLog.Print( "Center of mass: (%f,%f,%f)\n", pos(0)+massProperties.m_centerOfMass(0), pos(1)+massProperties.m_centerOfMass(1), pos(2)+massProperties.m_centerOfMass(2) );
			GLog.Print( "Mass: %f%s%s%s\n", massProperties.m_mass, rbInfo.MassFromDef?", fromDEF":"", rbInfo.MassFromHKE?", fromHKE":"", rbInfo.Fixed?", fixed":"" );
#endif

#ifdef HAVOK_POSITION_BUG_TRACKING
			Assert( massProperties.m_mass >= 0.0f );
#endif

            m_setTag("Physics");
			rigidbody = new hkRigidBody( rigidBodyInfo );
			vertShape->removeReference();

			// Set sweep
            m_setTag("Physics");
#ifdef HAVOK212
			hkConvexSweepShape* css = new hkConvexSweepShape( static_cast<const hkConvexShape*>(rigidbody->getCollidable().getShape()), rigidbody, RAGDOLL_SWEEP_SPEED );
			rigidbody->getCollidable()->getShape()->removeReference();
			rigidbody->getCollidable()->setShape( css );
#else
			hkConvexSweepShape* css = new hkConvexSweepShape( static_cast<const hkConvexShape*>(rigidbody->getCollidable().getShape()) );
			rigidbody->getCollidable().getShape()->removeReference();
			rigidbody->getCollidable().setShape( css );
#endif
		}
		else
		{
			GLog.Print( "Ragdoll creation error!!! Unable to create convex body '%s'\n", rbInfo.Name.AsChar() );
			const int vertNum = rbInfo.Verts.Size();

			int index1 = 0, index2 = 0;
			float dist = 0.0f;
			for( int j = 0; j < vertNum; ++j )
				for( int k = j+1; k < vertNum; ++k )
				{
					hkVector4 temp( rbInfo.Verts[j].X, rbInfo.Verts[j].Y, rbInfo.Verts[j].Z );
					hkVector4 temp2( rbInfo.Verts[k].X, rbInfo.Verts[k].Y, rbInfo.Verts[k].Z );
					temp.sub4( temp2 );
					if( temp.lengthSquared3() > dist )
					{
						dist = temp.lengthSquared3();
						index1 = j;
						index2 = k;
					}
				}

			dist = sqrt( dist );
			hkVector4 center( rbInfo.Verts[index1].X, rbInfo.Verts[index1].Y, rbInfo.Verts[index1].Z );
			hkVector4 center2( rbInfo.Verts[index2].X, rbInfo.Verts[index2].Y, rbInfo.Verts[index2].Z );
			center.add4( center2 );
			center.mul4(0.5f);

			// Make a ball
            m_setTag("Physics");
			hkSphereShape* sphereShape = new hkSphereShape( dist*scale );

#ifdef HAVOK3
			hkRigidBodyCinfo sphereInfo;
#else
			hkRigidBodyConstructionInfo sphereInfo;
#endif

			hkMassProperties massProperties;
			if( rbInfo.MassFromDef )
			{
				float mass = rbInfo.Mass;
				if( mass == 0.0 ) mass = 10.0f;

				// Compute properties to get volume
				hkInertiaTensorComputer::computeVertexHullVolumeMassProperties( &vertsOut[0](0), sizeof(hkVector4), vertsOut.getSize(), mass, massProperties );
			}
			else if( rbInfo.MassFromHKE )
			{
				float mass = rbInfo.Mass;
				mass *= scale*scale*scale;

				if( mass == 0.0 ) mass = 10.0f;
				hkInertiaTensorComputer::computeSphereVolumeMassProperties( dist*scale, mass, massProperties );
			}
			else
			{
				// Take arbitrary mass
				float mass = 10;
				// Compute properties to get volume
				hkInertiaTensorComputer::computeSphereVolumeMassProperties( dist*scale, mass, massProperties );
				// Adjust mass, based on volume
				mass = 600*fabs(massProperties.m_volume);
				// Make sure that mesh mass is minimum 10
				mass = (mass < 10.0f)?10.0f:mass;
				// Compute properties again to get final inertia tensor, mass, and center of mass ( just in case )
				hkInertiaTensorComputer::computeSphereVolumeMassProperties( dist*scale, mass, massProperties );
			}

			sphereInfo.m_shape = sphereShape;
			sphereInfo.m_position = center;
			sphereInfo.m_angularDamping = 0.05f;
			sphereInfo.m_linearDamping = 0.05f;
			sphereInfo.m_friction = 2.5f;
#ifdef HAVOK3
			sphereInfo.m_motionType = hkMotion::MOTION_SPHERE_INERTIA;
#else
			sphereInfo.m_motionType = HK_SPHERE_INERTIA_MOTION;
#endif
			sphereInfo.m_collisionFilterInfo = hkGroupFilter::calcFilterInfo( ( collisionGroup == -1 )?HCGRagdoll:collisionGroup );
			sphereInfo.m_inertiaTensor = massProperties.m_inertiaTensor;
			sphereInfo.m_centerOfMass = massProperties.m_centerOfMass;
			sphereInfo.m_mass = massProperties.m_mass;
//			GLog.Print("Ragdoll limb mass: %f\n", massProperties.m_mass );
			massSum += massProperties.m_mass;

#ifdef HAVOK_POSITION_BUG_TRACKING
			Assert( massProperties.m_mass >= 0.0f );
#endif
			
            m_setTag("Physics");
			rigidbody = new hkRigidBody( sphereInfo );
			sphereShape->removeReference();

			// Set sweep
            m_setTag("Physics");
#ifdef HAVOK212
			hkConvexSweepShape* css = new hkConvexSweepShape( static_cast<const hkConvexShape*>(rigidbody->getCollidable().getShape()), rigidbody, RAGDOLL_SWEEP_SPEED );
			rigidbody->getCollidable()->getShape()->removeReference();
			rigidbody->getCollidable()->setShape( css );
#else
			hkConvexSweepShape* css = new hkConvexSweepShape( static_cast<const hkConvexShape*>(rigidbody->getCollidable().getShape()) );
			rigidbody->getCollidable().getShape()->removeReference();
			rigidbody->getCollidable().setShape( css );
#endif

			// Set position
			hkVector4 pos( rbInfo.Position.X, rbInfo.Position.Y, rbInfo.Position.Z );
			pos.mul4( scale );
#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
			if( !pos.isOk4() || ( fabs(pos(0)) > 10000.0f ) || ( fabs(pos(1)) > 10000.0f ) || ( fabs(pos(2)) > 10000.0f ) )
#else
			if( !pos.isOk() || ( fabs(pos(0)) > 10000.0f ) || ( fabs(pos(1)) > 10000.0f ) || ( fabs(pos(2)) > 10000.0f ) )
#endif
				GLog.Print( "Position > 10000 in CreateRagdollFromTemplate\n" );
#endif
			rigidbody->setPosition( pos );
		}

		// For MP prediction only :(
		hkPropertyValue pv;
		pv.asInt = 1;
		rigidbody->addProperty( HPRagdollLimb, pv );

		// Set user info
#ifdef HAVOK3
		pv.setPtr(model);
#else
		pv.asPtr = model;
#endif
		rigidbody->addProperty( HPEntityPointer, pv );

		hkTransform T2;
		T2.set4x4ColumnMajor( &rbInfo.Transform.mArray[0] );
		hkVector4 transl = T2.getTranslation();
		transl.mul4(scale);
		T2.setTranslation( transl );

		Matrix rigidTransform;
		T2.get4x4ColumnMajor( &rigidTransform.mArray[0] );

		Matrix offset;
		offset = bindSkeleton.Joints[joint].Global * rigidTransform.Inverse();

#ifdef HAVOK_POSITION_BUG_TRACKING
		Vector posTEST = offset.GetTranslation();
		hkVector4 p( posTEST.X, posTEST.Y, posTEST.Z );
#ifdef HAVOK3
		if( !p.isOk4() )
#else
		if( !p.isOk() )
#endif
			GLog.Print( "Bad ragdoll limb pos in animate HW\n" );
		else if( p.lengthSquared3() > 10000.0f*10000.0f )
			GLog.Print( "Ragdoll limb pos > 10000 in animate HW: (%f,%f,%f)\n", p(0), p(1), p(2) );
#endif


		limbs.Add( Limb( rigidbody, joint, offset, rbInfo.Fixed ) );
		ragdollSkeleton->Joints[joint].Global = true;

		oldIndex.Add( i );
		newRB.Add( rigidbody );

		World->addEntity( rigidbody );
		rigidbody->setKeyframed( true );
#ifdef HAVOK3
		rigidbody->activate();
#else
		rigidbody->forceActivate();
#endif
	}

	for( int i = 0; i < ragdollTemplate->Constraints.Size(); ++i )
	{
		HavokRagdollConstraintData& c = ragdollTemplate->Constraints[i];

		// Find out bodies' translation
		hkRigidBody* newB1 = newRB[c.BodyA];
		hkRigidBody* newB2 = newRB[c.BodyB];

		// Create new constraint, scaled appropriately
		ConstraintData cond;
		cond.Constraint = 0;
		cond.Type = c.Type;

		switch( c.Type )
		{
		case 0:
			{

				hkRagdollConstructionInfo info = *(hkRagdollConstructionInfo*)c.ConstraintInfo;
				info.m_pivotInA.mul4(scale);
				info.m_pivotInB.mul4(scale);
				info.m_bodyA = newB1;
				info.m_bodyB = newB2;
#ifdef MAKE_RAGDOLL_VERBOSE
				GLog.Print( "Ragdoll pivots: (%f,%f,%f),(%f,%f,%f)\n", info.m_pivotInA(0), info.m_pivotInA(1), info.m_pivotInA(2), info.m_pivotInB(0), info.m_pivotInB(1), info.m_pivotInB(2) );
#endif
                m_setTag("Physics");
				hkRagdollConstraint* newRagdoll = new hkRagdollConstraint( info );
				newRagdoll->setUserData( model );
				cond.Constraint = newRagdoll;
				cond.Type = 0;
			}
			break;
		case 1:
			{
				hkLimitedHingeConstructionInfo info = *(hkLimitedHingeConstructionInfo*)c.ConstraintInfo;
				info.m_pivotInA.mul4(scale);
				info.m_pivotInB.mul4(scale);
				info.m_bodyA = newB1;
				info.m_bodyB = newB2;
#ifdef MAKE_RAGDOLL_VERBOSE
				GLog.Print( "Limited hinge pivots: (%f,%f,%f),(%f,%f,%f)\n", info.m_pivotInA(0), info.m_pivotInA(1), info.m_pivotInA(2), info.m_pivotInB(0), info.m_pivotInB(1), info.m_pivotInB(2) );
#endif
                m_setTag("Physics");
				hkLimitedHingeConstraint* newHinge = new hkLimitedHingeConstraint( info );
				newHinge->setUserData( model );
				cond.Constraint = newHinge;
			}
			break;
		case 2:
			{
				hkHingeConstructionInfo info = *(hkHingeConstructionInfo*)c.ConstraintInfo;
				info.m_pivotInA.mul4(scale);
				info.m_pivotInB.mul4(scale);
				info.m_bodyA = newB1;
				info.m_bodyB = newB2;
#ifdef MAKE_RAGDOLL_VERBOSE
				GLog.Print( "Hinge pivots: (%f,%f,%f),(%f,%f,%f)\n", info.m_pivotInA(0), info.m_pivotInA(1), info.m_pivotInA(2), info.m_pivotInB(0), info.m_pivotInB(1), info.m_pivotInB(2) );
#endif
                m_setTag("Physics");
				hkHingeConstraint* newHinge = new hkHingeConstraint( info );
				newHinge->setUserData( model );
				cond.Constraint = newHinge;
			}
			break;
		case 3:
			{
                m_setTag("Physics");
				hkGenericConstraint* newLPTP = new hkGenericConstraint( newB1, newB2 );
				hkConstraintConstructionKit kit;
				kit.begin( newLPTP );
				hkVector4 temp;
				hkMatrix3 tempM;

				temp.set( c.Gen->PivotA.X, c.Gen->PivotA.Y, c.Gen->PivotA.Z );
				temp.mul4( scale );
				kit.setPivotA( temp );

				temp.set( c.Gen->PivotB.X, c.Gen->PivotB.Y, c.Gen->PivotB.Z );
				temp.mul4( scale );
				kit.setPivotB( temp );

#ifdef MAKE_RAGDOLL_VERBOSE
				GLog.Print( "LPTP pivots: (%f,%f,%f),(%f,%f,%f)\n", c.Gen->PivotA.X, c.Gen->PivotA.Y, c.Gen->PivotA.Z, c.Gen->PivotB.X, c.Gen->PivotB.Y, c.Gen->PivotB.Z );
#endif

				// Since we want the pivots in body space to remain together
				kit.constrainAllLinearDof();

				// Set the angular basis of each body
				hkVector4 mat1( c.Gen->AngBasisA1.X, c.Gen->AngBasisA1.Y, c.Gen->AngBasisA1.Z );
				hkVector4 mat2( c.Gen->AngBasisA2.X, c.Gen->AngBasisA2.Y, c.Gen->AngBasisA2.Z );
				hkVector4 mat3( c.Gen->AngBasisA3.X, c.Gen->AngBasisA3.Y, c.Gen->AngBasisA3.Z );
				hkMatrix3 mat;
				mat.setRows( mat1, mat2, mat3 );
				kit.setAngularBasisA( mat );
				mat1.set( c.Gen->AngBasisB1.X, c.Gen->AngBasisB1.Y, c.Gen->AngBasisB1.Z );
				mat2.set( c.Gen->AngBasisB2.X, c.Gen->AngBasisB2.Y, c.Gen->AngBasisB2.Z );
				mat3.set( c.Gen->AngBasisB3.X, c.Gen->AngBasisB3.Y, c.Gen->AngBasisB3.Z );
				mat.setRows( mat1, mat2, mat3 );
				kit.setAngularBasisB( mat );

				// Set the angular limits wrt to each axis
				kit.setAngularLimit( 0, c.Gen->TempX0, c.Gen->TempX1 );
				kit.setAngularLimit( 1, c.Gen->TempY0, c.Gen->TempY1 );
				kit.setAngularLimit( 2, c.Gen->TempZ0, c.Gen->TempZ1 );

				kit.end();

				newLPTP->setUserData( model );
				cond.Constraint = newLPTP;
			}
			break;
		case 4:
			{
				hkBallAndSocketConstructionInfo info = *(hkBallAndSocketConstructionInfo*)c.ConstraintInfo;
				info.m_pivotInA.mul4(scale);
				info.m_pivotInB.mul4(scale);
				info.m_bodyA = newB1;
				info.m_bodyB = newB2;
#ifdef MAKE_RAGDOLL_VERBOSE
				GLog.Print( "BallAndSocket pivots: (%f,%f,%f),(%f,%f,%f)\n", info.m_pivotInA(0), info.m_pivotInA(1), info.m_pivotInA(2), info.m_pivotInB(0), info.m_pivotInB(1), info.m_pivotInB(2) );
#endif
                m_setTag("Physics");
				hkBallAndSocketConstraint* newBS = new hkBallAndSocketConstraint( info );
				newBS->setUserData( model );
				cond.Constraint = newBS;
			}
			break;
		case 5:
			{
				hkStiffSpringConstructionInfo info = *(hkStiffSpringConstructionInfo*)c.ConstraintInfo;
				info.m_pivotInA.mul4(scale);
				info.m_pivotInB.mul4(scale);
				info.m_bodyA = newB1;
				info.m_bodyB = newB2;
				info.m_springLength *= scale;
#ifdef MAKE_RAGDOLL_VERBOSE
				GLog.Print( "Spring pivots: (%f,%f,%f),(%f,%f,%f)\n", info.m_pivotInA(0), info.m_pivotInA(1), info.m_pivotInA(2), info.m_pivotInB(0), info.m_pivotInB(1), info.m_pivotInB(2) );
				GLog.Print( "Spring length: %f\n", info.m_springLength );
#endif
                m_setTag("Physics");
				hkStiffSpringConstraint* newSS = new hkStiffSpringConstraint( info );
				newSS->setUserData( model );
				cond.Constraint = newSS;
			}
			break;
		case 6:
			{
				hkPrismaticConstructionInfo info = *(hkPrismaticConstructionInfo*)c.ConstraintInfo;
				info.m_pivotInA.mul4(scale);
				info.m_pivotInB.mul4(scale);
				info.m_bodyA = newB1;
				info.m_bodyB = newB2;
				info.m_minLimit *= scale;
				info.m_maxLimit *= scale;
				info.m_maxFrictionForce *= (scale*scale*scale);	// not sure about that though...
#ifdef MAKE_RAGDOLL_VERBOSE
				GLog.Print( "Prismatic pivots: (%f,%f,%f),(%f,%f,%f)\n", info.m_pivotInA(0), info.m_pivotInA(1), info.m_pivotInA(2), info.m_pivotInB(0), info.m_pivotInB(1), info.m_pivotInB(2) );
				GLog.Print( "Prismatic limits: (%f,%f,)\n", info.m_minLimit, info.m_maxLimit );
				GLog.Print( "Prismatic max friction force: %f\n", info.m_maxFrictionForce );
#endif
                m_setTag("Physics");
				hkPrismaticConstraint* newPrismatic = new hkPrismaticConstraint( info );
				newPrismatic->setUserData( model );
				cond.Constraint = newPrismatic;
			}
			break;
		case 7:
			{
				hkWheelConstructionInfo info = *(hkWheelConstructionInfo*)c.ConstraintInfo;
				info.m_pivotInA.mul4(scale);
				info.m_pivotInB.mul4(scale);
				info.m_bodyA = newB1;
				info.m_bodyB = newB2;
				info.m_suspensionMinLimit *= scale;
				info.m_suspensionMaxLimit *= scale;
#ifdef MAKE_RAGDOLL_VERBOSE
				GLog.Print( "Wheel pivots: (%f,%f,%f),(%f,%f,%f)\n", info.m_pivotInA(0), info.m_pivotInA(1), info.m_pivotInA(2), info.m_pivotInB(0), info.m_pivotInB(1), info.m_pivotInB(2) );
				GLog.Print( "Wheel max limits: (%f,%f)\n", info.m_suspensionMinLimit, info.m_suspensionMaxLimit );
#endif
                m_setTag("Physics");
				hkWheelConstraint* newWheel = new hkWheelConstraint( info );
				newWheel->setUserData( model );
				cond.Constraint = newWheel;
			}
			break;
		};

		if( c.Breakable )
		{
            m_setTag("Physics");
			hkBreakableConstraint* newBreak = new hkBreakableConstraint(cond.Constraint,0);
			newBreak->setThreshold( c.Threshold );
			newBreak->setRemoveWhenBroken(false);
			newBreak->addBreakableListener( BreakableListener );
			newBreak->setUserData( model );
			cond.Constraint = newBreak;
			cond.Type += 10;
		}
		rgd->Constraints.Add( cond );
	}

	for( int i = 0; i < ragdollTemplate->Actions.Size(); ++i )
	{
		HavokRagdollActionData& a = ragdollTemplate->Actions[i];

		hkRigidBody* newB1 = newRB[a.BodyA];
		hkRigidBody* newB2 = (a.BodyB != -1)?newRB[a.BodyB]:0;

		ActionData ad;
		ad.Type = a.Type;
		switch( a.Type )
		{
		case 0:
			{
				hkDashpotActionConstructionInfo info;
				hkVector4 pivotA( a.PivotA.X, a.PivotA.Y, a.PivotA.Z );
				hkVector4 pivotB( a.PivotB.X, a.PivotB.Y, a.PivotB.Z );
				pivotA.mul4(scale);
				pivotB.mul4(scale);
#ifdef MAKE_RAGDOLL_VERBOSE
				GLog.Print( "Action pivots: (%f,%f,%f),(%f,%f,%f)\n", pivotA(0), pivotA(1), pivotA(2), pivotB(0), pivotB(1), pivotB(2) );
#endif
				info.setInBodySpace( newB1, newB2, pivotA, pivotB );
				info.setStrength( a.Strength );
				info.setDamping( a.Damping );
                m_setTag("Physics");
				hkDashpotAction* newAction = new hkDashpotAction( info );
				ad.Action = newAction;
			}
			break;
		case 1:
			{
				hkAngularDashpotActionConstructionInfo info;
				hkQuaternion q( a.Rotation.X, a.Rotation.Y, a.Rotation.Z, -a.Rotation.W );
				info.set( newB1, newB2, q );
				info.setStrength( a.Strength );
				info.setDamping( a.Damping );
                m_setTag("Physics");
				hkAngularDashpotAction* newAction = new hkAngularDashpotAction( info );
				ad.Action = newAction;
			}
		case 2:
			{
				hkSpringActionConstructionInfo info;
				hkVector4 pivotA( a.PivotA.X, a.PivotA.Y, a.PivotA.Z );
				hkVector4 pivotB( a.PivotB.X, a.PivotB.Y, a.PivotB.Z );
				pivotA.mul4(scale);
				pivotB.mul4(scale);
#ifdef MAKE_RAGDOLL_VERBOSE
				GLog.Print( "Action pivots: (%f,%f,%f),(%f,%f,%f)\n", pivotA(0), pivotA(1), pivotA(2), pivotB(0), pivotB(1), pivotB(2) );
				GLog.Print( "Action rest length: %f\n", a.RestLength );
#endif
				info.setInBodySpace( newB1, newB2, pivotA, pivotB );
				info.setStrength( a.Strength );
				info.setDamping( a.Damping );
				info.setRestLength( a.RestLength );
				info.setOnCompression( a.OnCompression );
				info.setOnExtension( a.OnExtension );
                m_setTag("Physics");
				hkSpringAction* newAction = new hkSpringAction( info );
				ad.Action = newAction;
			}
			break;
		}
		rgd->Actions.Add( ad );
	}

	return rgd;
#else
	GLog.Print("HavokWorld::CreateRagdollFromTemplate() - FIXME!!!!!!!!!!!\n");
	return HK_NULL;
#endif // ifndef HAVOK3 // =================================================================
}

hkRigidBody* HavokWorld::CreatePawnBodyFromRagdollTemplate( HavokRagdollTemplate* ragdollTemplate, float scale, float& mVolumeMass, int collisionGroup )
{
	float realDampingLinear = 0.0f;
	float realDampingAngular = 0.0f;
	if( mIsUnderwater )
	{
		realDampingLinear = 1.0f - ((1.0f-realDampingLinear)/GEngine->Physics->TWUnderwaterDampingRaiseLinear);
		realDampingAngular = 1.0f - ((1.0f-realDampingAngular)/GEngine->Physics->TWUnderwaterDampingRaiseAngular);
	}

	hkArray<hkShape*> shapeArray;
	hkArray<hkMassElement> massArray;
	mVolumeMass = 0;

	for( int i = 0; i < ragdollTemplate->RigidBodies.Size(); ++i )
	{
		HavokRagdollRigidBodyInfo& rbInfo = ragdollTemplate->RigidBodies[i];

		// Create internal shape
		hkArray<hkVector4> planeEquations;
#ifdef HAVOK3
		hkStridedVertices stridedVerts;
		hkGeometry geom;
#else
		hkArray<hkVector4> vertsOut;
#endif

		// Check if we have planes for this shape in cache
		HavokWorldConvexCache* cache = (HavokWorldConvexCache*)GEngine->Physics->mConvexCache;
		HavokWorldConvexEntry* entry = cache->GetEntry( ragdollTemplate->ModelFile, rbInfo.Name, false );
		if( entry )
		{
#ifdef LOG_CONVEX_CREATION
			GLog.Print( "Model '%s', joint '%s', scale %f taken from cache\n", ragdollTemplate->ModelFile.AsChar(), rbInfo.Name.AsChar(), scale );
			GLog.Flush();
#endif
			planeEquations = entry->planeEquations;
#ifdef HAVOK3
			geom.m_vertices = entry->vertsOut;
#else
			vertsOut = entry->vertsOut;
#endif
			int planeEqSize = planeEquations.getSize();
			if( ( planeEqSize >= 4 ) && ( entry->scale != scale ) )
			{
#ifdef HAVOK3
				int geomVertSize = geom.m_vertices.getSize();
#else
				int vertsOutSize = vertsOut.getSize();
#endif
				Assert(entry->scale);
#ifdef TRANSGAMING
				float  multiplier = scale/entry->scale;
#else
				double multiplier = scale/entry->scale;
#endif
				for( int j = 0; j < planeEqSize; ++j )
					planeEquations[j](3) *= multiplier;
#ifdef HAVOK3
				for( int j = 0; i < geomVertSize; ++j )
					geom.m_vertices[j].mul4( multiplier );
#else
				for( int j = 0; j < vertsOutSize; ++j )
					vertsOut[j].mul4( multiplier );
#endif
			}
#ifdef HAVOK3
			stridedVerts.m_numVertices = geom.m_vertices.getSize();
			stridedVerts.m_striding = sizeof(hkVector4);
			stridedVerts.m_vertices = &(geom.m_vertices[0](0));
#endif
		}
		else
		{
			const int vertNum = rbInfo.Verts.Size();
			hkArray<hkVector4> verts;
			for( int j = 0; j < vertNum; ++j )
			{
				hkVector4 vec( rbInfo.Verts[j].X*scale, rbInfo.Verts[j].Y*scale, rbInfo.Verts[j].Z*scale, 0 );
				verts.pushBack( vec );
			}

			// Check if we can make a smaller set for all subsequent operations
#ifndef HAVOK3 // BLAIR - this doesn't seem vital, and is depricated.  Might fix later.
			hkArray<hkVector4> verts2;
			verts2 = verts;
			hkGeometryFilterer::removeCloseVertices( verts2, 0.0001f );
			if( ( verts2.getSize() > 3 ) && !hkGeometryChecker::isCoplanarSet( verts2, 0.00000000001f ) ) verts = verts2;
			verts2.clearAndDeallocate();
#endif

#ifdef LOG_CONVEX_CREATION
# ifdef TRANSGAMING
			float  firstTime = GEngine->SysDriver.GetCurrentTime();
# else
			double firstTime = GEngine->SysDriver.GetCurrentTime();
# endif
			GLog.Print( "Creating convex for ragdoll pawn (pawn body ) '%s'", rbInfo.Name.AsChar() );
			GLog.Flush();
#endif

#ifdef HAVOK3
			stridedVerts.m_numVertices = verts.getSize();
			stridedVerts.m_striding = sizeof(hkVector4);
			stridedVerts.m_vertices = &(verts[0](0));
			// By far the COOLEST part of the game...where we create the convex geometry...
			hkGeometryUtility::createConvexGeometry( stridedVerts, geom, planeEquations );
			{
				stridedVerts.m_numVertices = geom.m_vertices.getSize();
				stridedVerts.m_striding = sizeof(hkVector4);
				stridedVerts.m_vertices = &(geom.m_vertices[0](0));					
			}
#else
			hkGeometryUtil::computePlanesFromVertices( &verts[0](0), verts.getSize(), sizeof(hkVector4), planeEquations, vertsOut );
#endif

#ifdef LOG_CONVEX_CREATION
			GLog.Print( ", duration: %f\n", GEngine->SysDriver.GetCurrentTime()-firstTime );
			GLog.Flush();
#endif

#ifdef HAVOK3
			cache->PutEntry( ragdollTemplate->ModelFile, rbInfo.Name, planeEquations, geom.m_vertices, scale, false );
#else
			cache->PutEntry( ragdollTemplate->ModelFile, rbInfo.Name, planeEquations, vertsOut, scale, false );
#endif

#ifdef LOG_CONVEX_CREATION
			GLog.Print( "Ragdoll '%s', joint '%s', scale %f entered into cache\n", ragdollTemplate->ModelFile.AsChar(), rbInfo.Name.AsChar(), scale );
			GLog.Flush();
#endif
		}

		if( planeEquations.getSize() >= 4 )
		{
#ifdef HAVOK3
			m_setTag("Physics");
			hkConvexVerticesShape* vertShape = new hkConvexVerticesShape( stridedVerts, planeEquations );
#else
			hkConvexVerticesShape* vertShape = new hkConvexVerticesShape( &vertsOut[0](0), sizeof(hkVector4), vertsOut.getSize(), planeEquations );
#endif
			// Calculate inertia tensor
			hkMassElement new_mass_info;

			float mass = 10;
#ifdef HAVOK3
			hkInertiaTensorComputer::computeVertexHullVolumeMassProperties( stridedVerts.m_vertices, sizeof(hkVector4), stridedVerts.m_numVertices, mass, new_mass_info.m_properties );
#else
			hkInertiaTensorComputer::computeVertexHullVolumeMassProperties( &vertsOut[0](0), sizeof(hkVector4), vertsOut.getSize(), mass, new_mass_info.m_properties );
#endif
			mVolumeMass += 600*fabs(new_mass_info.m_properties.m_volume);

			if( rbInfo.MassFromHKE )
			{
				float mass = rbInfo.Mass;
				mass *= scale*scale*scale;
				if( mass == 0.0 ) mass = 10.0f;
#ifdef HAVOK3
				hkInertiaTensorComputer::computeVertexHullVolumeMassProperties( stridedVerts.m_vertices, sizeof(hkVector4), stridedVerts.m_numVertices, mass, new_mass_info.m_properties );
#else
				hkInertiaTensorComputer::computeVertexHullVolumeMassProperties( &vertsOut[0](0), sizeof(hkVector4), vertsOut.getSize(), mass, new_mass_info.m_properties );
#endif
			}
			else
			{
				mass = 600*fabs(new_mass_info.m_properties.m_volume);
				mass = (mass < 10.0f)?10.0f:mass;
#ifdef HAVOK3
				hkInertiaTensorComputer::computeVertexHullVolumeMassProperties( stridedVerts.m_vertices, sizeof(hkVector4), stridedVerts.m_numVertices, mass, new_mass_info.m_properties );
#else
				hkInertiaTensorComputer::computeVertexHullVolumeMassProperties( &vertsOut[0](0), sizeof(hkVector4), vertsOut.getSize(), mass, new_mass_info.m_properties );
#endif
			}
			shapeArray.pushBack( vertShape );
			new_mass_info.m_transform.setIdentity();
			massArray.pushBack( new_mass_info );
		}
		else
			GLog.Print( "Physics Object From Ragdoll creation error!!! Unable to create convex body '%s'\n", rbInfo.Name.AsChar() );
	}

	if( !shapeArray.getSize() ) return 0;

#ifdef HAVOK3
	hkRigidBodyCinfo rigidBodyInfo;
#else
	hkRigidBodyConstructionInfo rigidBodyInfo;
#endif

	hkMassProperties overallMassProperties;
    m_setTag("Physics");
	hkListShape* listShape = new hkListShape( &shapeArray[0], shapeArray.getSize() );

	rigidBodyInfo.m_inertiaTensor.setIdentity();
	hkInertiaTensorComputer::combineMassProperties( massArray, overallMassProperties );
	rigidBodyInfo.m_inertiaTensor = overallMassProperties.m_inertiaTensor;
	rigidBodyInfo.m_centerOfMass = overallMassProperties.m_centerOfMass;

	for( int i = 0; i < shapeArray.getSize(); ++i )
		shapeArray[i]->removeReference();
	massArray.clearAndDeallocate();

#ifdef HAVOK3
	hkShape* ourShape = listShape;
#else
	hkShape* ourShape = new hkSweepBvTreeShape( listShape );
	listShape->removeReference();
#endif

	rigidBodyInfo.m_shape = ourShape;
	rigidBodyInfo.m_position.set(0.0f, 0.0f, 0.0f);
	rigidBodyInfo.m_angularDamping = realDampingAngular;
	rigidBodyInfo.m_linearDamping = realDampingLinear;
#ifdef HAVOK3
	rigidBodyInfo.m_motionType = hkMotion::MOTION_STABILIZED_BOX_INERTIA;
#else
	rigidBodyInfo.m_motionType = HK_STABILIZED_BOX_INERTIA_MOTION;
#endif
	rigidBodyInfo.m_collisionFilterInfo = hkGroupFilter::calcFilterInfo( (collisionGroup==-1)?HCGNormal:collisionGroup );
	rigidBodyInfo.m_mass = 10.0f;	// just in case

    m_setTag("Physics");
	hkRigidBody* rigidbody = new hkRigidBody( rigidBodyInfo );

	ourShape->removeReference();

	return rigidbody;
}

void HavokWorld::RemoveRagdoll( HavokRagdoll* ragdoll )
{
	for( int iAction = 0; iAction < ragdoll->Actions.Size(); ++iAction )
		ragdoll->Actions[iAction].Action->removeReference();

	for( int iConstraint = 0; iConstraint < ragdoll->Constraints.Size(); ++iConstraint )
		ragdoll->Constraints[iConstraint].Constraint->removeReference();

	for( int iLimb = 0; iLimb < ragdoll->Limbs.Size(); iLimb++ )
	{
		hkRigidBody *rb = ragdoll->Limbs[iLimb].Body;
		if( rb && rb->getSimulationIsland() )
		{
#ifdef HAVOK3
		if( rb->getMotionType() == hkMotion::MOTION_KEYFRAMED )
			rb->setMotionType( hkMotion::MOTION_FIXED );
#else
			if( rb->isKeyframed() ) rb->setKeyframed( false );	// to disable Havok memory leak
#endif
			World->removeEntity( rb );
			rb->removeReference();
		}
	}

	for( int i = 0; i < Ragdolls.Size(); ++i )
		if( Ragdolls[i] == ragdoll )
		{
			Ragdolls.FastRemove(i);
			break;
		}
}

void HavokWorld::UpdateEntities( DWORD msToAdd )
{
    const float tickPart = mTickPart + msToAdd * 0.001f * GEngine->Physics->PhysicsFPS;


    //TRACE("HavokWorld::UpdateEntities(): updating %d entities (tickPart = %g)\n", PhysicsObjects.Size(), tickPart);


    for( int i = 0; i < PhysicsObjects.Size(); ++i )
		PhysicsObjects[i]->UpdateEntity( tickPart );
}

bool HavokWorld::TickWillMovePhysics( float timeDelta )
{
	return ( timeDelta * GEngine->Physics->PhysicsFPS + mTickPart >= 1.0f );
}

bool HavokWorld::Tick( float timeDelta )
{
	int substeps;
	float substepTimeDelta;

	if( timeDelta == 0.0f ) return false;

	#ifdef ENGINE_LOG
	GLog.Print("^* HWT1\n");
	#endif

	#ifdef ENGINE_LOG
	GLog.Print("^* HWT2\n");
	#endif
	substepTimeDelta = timeDelta * GEngine->Physics->PhysicsFPS + mTickPart;
	substeps =  (int)(floor(substepTimeDelta));
	mTickPart = substepTimeDelta - substeps;
	substepTimeDelta = 1.0f / GEngine->Physics->PhysicsFPS; //constant
	DWORD integerSubstepTimeDelta = 1000/GEngine->Physics->PhysicsFPS;

	if( ( substeps == 0 ) || ( substeps > 15 ) )
	{
		if( substeps > 15 )
			GLog.Print( "Physics substeps skipped: %d\n", substeps );
		// update world matrices for ragdolls
		for( int i = 0; i < Ragdolls.Size(); ++i )
			if( !( Ragdolls[i]->Flags & HavokRagdoll::EternallyFrozen ) && ( Ragdolls[i]->Flags & HavokRagdoll::Active ) )
				Ragdolls[i]->TickInterpolate( mTickPart );
		#ifdef ENGINE_LOG
		GLog.Print("^* HWT3\n");
		#endif
		return false;
	}

	#ifdef ENGINE_LOG
	GLog.Print("^* HWT4\n");
	#endif
	DynamicArray<PhysicsObject*> players;
	DynamicArray<PhysicsObject*> missiles;
	for( int i = 0; i < PhysicsObjects.Size(); ++i )
	{
		if( PhysicsObjects[i]->IsPlayer() )
			players.Add( PhysicsObjects[i] );
		else if( PhysicsObjects[i]->mMissileController )
			missiles.Add( PhysicsObjects[i] );
	}

	#ifdef ENGINE_LOG
	GLog.Print("^* HWT5\n");
	#endif
	// STEP DELTA TIME
	for( int ss = 0; ss < substeps; ++ss )
	{
		mPhysicsFrame++;
		for( int i = 0; i < Ragdolls.Size(); ++i )
			if( !( Ragdolls[i]->Flags & HavokRagdoll::EternallyFrozen ) && ( Ragdolls[i]->Flags & HavokRagdoll::Active ) )
				Ragdolls[i]->AdvanceInterpolationFrame();
		#ifdef ENGINE_LOG
		GLog.Print("^* HWT6\n");
		#endif
		GProfiler.SetState(PHYSICS_TICK_STEP_DELTA_TIME);
		World->stepDeltaTime( substepTimeDelta );
#if defined( HAVOK3 ) && defined ( USE_HAVOK_DEBUGGER ) && !defined( TRANSGAMING )
		stepVisualDebugger();
#endif 
		#ifdef ENGINE_LOG
		GLog.Print("^* HWT7\n");
		#endif
		for( int i = 0; i < players.Size(); ++i )
			players[i]->Tick( substepTimeDelta );

		ulong32 fpucontrol = _controlfp( 0, 0 );
#if defined( TRANSGAMING_PS2 )
		_controlfp( _PS2_64, _MCW_PS2 );
#elif defined( TRANSGAMING_PSP )
		_controlfp( _PSP_64, _MCW_PSP );
#else
		_controlfp( _PC_64, _MCW_PC );
#endif

		for( int i = 0; i < missiles.Size(); ++i )
			missiles[i]->mMissileController->missileMove( integerSubstepTimeDelta );

#if defined( TRANSGAMING_PS2 )
		_controlfp( fpucontrol, _MCW_PS2 );
#elif defined( TRANSGAMING_PSP )
		_controlfp( fpucontrol, _MCW_PSP );
#else
		_controlfp( fpucontrol, _MCW_PC );
#endif

		#ifdef ENGINE_LOG
		GLog.Print("^* HWT8\n");
		#endif
	}

	if( GEngine->GameState != PCFSystem::MultiplayerClient )
		RemoveFarawayBodies();
	else
		RigidBodiesPosExceeded.ClearArray();

	#ifdef ENGINE_LOG
	GLog.Print("^* HWT9\n");
	#endif
	GProfiler.SetState(PHYSICS_TICK_TICK_PO);
	for( int i = 0; i < PhysicsObjects.Size(); ++i )
		if( !PhysicsObjects[i]->IsPlayer()
			&& !PhysicsObjects[i]->mMissileController
			)
			PhysicsObjects[i]->Tick( timeDelta );

	#ifdef ENGINE_LOG
	GLog.Print("^* HWT10\n");
	#endif
	GProfiler.SetState(PHYSICS_TICK_TICK_RAGDOLL);

	// update world matrices for ragdolls
	for( int i = 0; i < Ragdolls.Size(); ++i )
		if( !( Ragdolls[i]->Flags & HavokRagdoll::EternallyFrozen ) && ( Ragdolls[i]->Flags & HavokRagdoll::Active ) )
			Ragdolls[i]->TickInterpolate( mTickPart );
	#ifdef ENGINE_LOG
	GLog.Print("^* HWT11\n");
	#endif

	// animate those ragdolls that need it
	for( int i = 0; i < Ragdolls.Size(); ++i )
	{
		HavokRagdoll* r = Ragdolls[i];
		Model* model = r->mModel;
		if( model->AnimController.HasAnyAnimations() && ( model->LastDrawTime > 1.0f ) &&
			( !GEngine->Net || !GEngine->Net->IsServer() ) && !(r->Flags&(HavokRagdoll::Frozen|HavokRagdoll::Active|HavokRagdoll::EternallyFrozen)) )
			r->Freeze();
	}

	#ifdef ENGINE_LOG
	GLog.Print("^* HWT12\n");
	#endif
	mDeactivator->DeactivateBodies( timeDelta );
#ifndef HAVOK3
	hkVisualDebugger::step();
#endif
	#ifdef ENGINE_LOG
	GLog.Print("^* HWT13\n");
	#endif

	static float timeDeltaSinceLastDeletedMeshes = 0.0f;

	if( timeDeltaSinceLastDeletedMeshes > 1.0f )
	{
		// Remove pinned active meshed whose time to delete has come
		for( int i = 0; i < PinnedActiveMeshes.Size(); ++i )
			if( PinnedActiveMeshes[i].TimeToDelete >= 0 )
			{
				PinnedActiveMeshes[i].TimeToDelete -= timeDelta;
				if( PinnedActiveMeshes[i].TimeToDelete <= 0 )
				{
					bool dontDelete = (( PinnedActiveMeshes[i].StaticMesh->Flags & Entity::EEntDontDeleteOnTimer ) != 0);
					RemovePinnedActiveMesh( i, dontDelete );
					i--;
				}
			}
		timeDeltaSinceLastDeletedMeshes = 0.0f;
	}
	else
		timeDeltaSinceLastDeletedMeshes += timeDelta;

	GProfiler.SetState(PHYSICS_TICK);

	#ifdef ENGINE_LOG
	GLog.Print("^* HWT14\n");
	#endif
	return true;
}

void HavokWorld::SingleTick( float timeDelta )
{
	int substeps;
	float substepTimeDelta;

	if( timeDelta == 0.0f ) return;

	// Step differences fix
	if( GEngine->UsePhysicsSubsteps )
	{
		if( timeDelta > 0.034f )
		{
			substeps = (int)(ceil(timeDelta/0.03f));
			substepTimeDelta = timeDelta/((float)substeps);
			//GLog.Print("delta: %f : havok substeps: %d\n",timeDelta,substeps);
		}
		else
		{
			substeps = 1;
			substepTimeDelta = timeDelta;
		}
	}
	else
	{
		if( timeDelta > 3.5f*mLastTimeDelta ) timeDelta = 3.5f*mLastTimeDelta;
		if( timeDelta < mLastTimeDelta/3.5f ) timeDelta = mLastTimeDelta/3.5f;

		substeps = 1;
		substepTimeDelta = timeDelta;
	}
	mLastTimeDelta = substepTimeDelta;	

	DynamicArray<PhysicsObject*> players;
	for( int i = 0; i < PhysicsObjects.Size(); ++i )
		if( PhysicsObjects[i]->IsPlayer() )
			players.Add( PhysicsObjects[i] );

	// STEP DELTA TIME
	GProfiler.SetState(PHYSICS_TICK_STEP_DELTA_TIME);
	for( int ss = 0; ss < substeps; ++ss )
	{
		World->stepDeltaTime( substepTimeDelta );
#if defined( HAVOK3 ) && defined ( USE_HAVOK_DEBUGGER ) && !defined( TRANSGAMING )
		stepVisualDebugger();
#endif 
		for( int i = 0; i < players.Size(); ++i ) players[i]->Tick( substepTimeDelta );
	}

	if( GEngine->GameState != PCFSystem::MultiplayerClient )
		RemoveFarawayBodies();
	else
		RigidBodiesPosExceeded.ClearArray();

	GProfiler.SetState(PHYSICS_TICK_TICK_PO);
	for( int i = 0; i < PhysicsObjects.Size(); ++i )
		if( !PhysicsObjects[i]->IsPlayer() )
			PhysicsObjects[i]->Tick( timeDelta );

	GProfiler.SetState(PHYSICS_TICK_TICK_RAGDOLL);

	// update world matrices for ragdolls
	for( int i = 0; i < Ragdolls.Size(); ++i )
		if( !( Ragdolls[i]->Flags & HavokRagdoll::EternallyFrozen ) && ( Ragdolls[i]->Flags & HavokRagdoll::Active ) )
			Ragdolls[i]->Tick();

	// animate those ragdolls that need it
	for( int i = 0; i < Ragdolls.Size(); ++i )
	{
		HavokRagdoll* r = Ragdolls[i];
		Model* model = r->mModel;
		if( model->AnimController.HasAnyAnimations() && ( model->LastDrawTime > 1.0f ) &&
			( !GEngine->Net || !GEngine->Net->IsServer() ) && !(r->Flags&(HavokRagdoll::Frozen|HavokRagdoll::Active|HavokRagdoll::EternallyFrozen)) )
			r->Freeze();
	}

	mDeactivator->DeactivateBodies( timeDelta );
#ifndef HAVOK3
	hkVisualDebugger::step();
#endif
	static float timeDeltaSinceLastDeletedMeshes = 0.0f;

	if( timeDeltaSinceLastDeletedMeshes > 1.0f )
	{
		// Remove pinned active meshed whose time to delete has come
		for( int i = 0; i < PinnedActiveMeshes.Size(); ++i )
			if( PinnedActiveMeshes[i].TimeToDelete >= 0 )
			{
				PinnedActiveMeshes[i].TimeToDelete -= timeDelta;
				if( PinnedActiveMeshes[i].TimeToDelete <= 0 )
				{
					bool dontDelete = (( PinnedActiveMeshes[i].StaticMesh->Flags & Entity::EEntDontDeleteOnTimer ) != 0);
					RemovePinnedActiveMesh( i, dontDelete );
					i--;
				}
			}
		timeDeltaSinceLastDeletedMeshes = 0.0f;
	}
	else
		timeDeltaSinceLastDeletedMeshes += timeDelta;

	GProfiler.SetState(PHYSICS_TICK);
}

void HavokWorld::UpdatePredictionTick( float timeDelta )
{
	int substeps = 1;
	float substepTimeDelta = timeDelta;

	DynamicArray<PhysicsObject*> players;

	// STEP DELTA TIME
	GProfiler.SetState(PHYSICS_TICK_STEP_DELTA_TIME);
	for( int ss = 0; ss < substeps; ++ss )
	{
		World->stepDeltaTime( substepTimeDelta );
#if defined( HAVOK3 ) && defined ( USE_HAVOK_DEBUGGER ) && !defined( TRANSGAMING )
		stepVisualDebugger();
#endif 
	}

	RigidBodiesPosExceeded.ClearArray();

	GProfiler.SetState(PHYSICS_TICK_TICK_PO);
	for( int i = 0; i < PhysicsObjects.Size(); ++i )
		PhysicsObjects[i]->Tick( timeDelta );

	GProfiler.SetState(PHYSICS_TICK_TICK_RAGDOLL);

	// update world matrices for ragdolls
	for( int i = 0; i < Ragdolls.Size(); ++i )
		if( !( Ragdolls[i]->Flags & HavokRagdoll::EternallyFrozen ) && ( Ragdolls[i]->Flags & HavokRagdoll::Active ) )
			Ragdolls[i]->Tick();

	mDeactivator->DeactivateBodies( timeDelta );
#ifndef HAVOK3
	hkVisualDebugger::step();
#endif
}

void HavokWorld::RemoveFarawayBodies( void )
{
	hkVector4 neutralPos(0,-HAVOK_WORLD_SIZE*0.4f,0);
	hkUint32 collFilterInfo = hkGroupFilter::calcFilterInfo( HCGNoncolliding );
	for( int i = 0; i < RigidBodiesPosExceeded.Size(); ++i )
	{
		hkRigidBody* rb = (hkRigidBody*)RigidBodiesPosExceeded[i];

		Entity* entity = 0;
		if( rb->hasProperty( HPEntityPointer ) )
		{
			hkPropertyValue p = rb->getProperty( HPEntityPointer );
#ifdef HAVOK3
			entity = (Entity*)p.getPtr();
#else
			entity = (Entity*)p.asPtr;
#endif
			entity->Flags |= Entity::EEntNoDraw;
		}
#ifdef HAVOK3
		rb->getCollidable()->setCollisionFilterInfo( collFilterInfo );	// not to worry; non-colliding
#else
		rb->getCollidable().setCollisionFilterInfo( collFilterInfo );	// not to worry; non-colliding
#endif

		if( rb->getSimulationIsland() )
		{
			if( entity && entity->PO && ( entity->PO->mHavokPart->Body == rb ) )
				entity->PO->Enable( false );
			else
				World->removeEntity( rb );
		}
	}

	RigidBodiesPosExceeded.ClearArray();
}

struct ExplosionMessage
{
	Entity* entity;
	float damage;
};

void HavokWorld::Explosion( Vector center, float strength, float range, int ownerID, int weapon_type, float weapon_damage )
{
	static int explosionCount=0;
	explosionCount++;

    //GScript.DoString("Game:Print('Explosion2: %f,%f,%f')",center.X,center.Y,center.Z);

	hkVector4 havokCenter( center.X, center.Y, center.Z );

	static DynamicArray<ExplosionMessage> ExplosionMessages;
	ExplosionMessages.ClearArray();

	// Affect physics objects
	for( int i = 0; i < PhysicsObjects.Size(); i++ )
	{
		PhysicsObject* po = PhysicsObjects[i];
		if( !po->mHavokPart->Body->getSimulationIsland() )
			continue;

		Entity* entity = 0;
		hkVector4 pawnCenter;
		if( ( po->BodyType == EPawnFromMeshNonCentered ) || ( po->BodyType == EPawnFromMeshNonCenteredSweep ) )
		{
			entity = po->GetEntity();
			if( !entity )
				continue;
			Vector pawnC = entity->Box.GetCenter();

			pawnCenter.set( pawnC.X, pawnC.Y, pawnC.Z );
		}
		else
			pawnCenter = po->mHavokPart->Body->getCenterOfMassInWorld();

		hkVector4 distanceV( pawnCenter );
		distanceV.sub4( havokCenter );
		float length = distanceV.length3() ;
		hkVector4 newPawnCenter( pawnCenter );

		if( length < 3 * range )
		{
			if( !entity )
				entity = po->GetEntity();

			if( entity->Type == Entity::ETypeMesh )
			{
				WorldMesh* wm = (WorldMesh*)entity;
				Vector closestPoint;
				float closestDistance;
				wm->GetClosestPoint( center, closestDistance, closestPoint );
				if( closestDistance < length )
				{
					length = closestDistance;
					newPawnCenter.set( closestPoint.X, closestPoint.Y, closestPoint.Z );
					distanceV.set( closestPoint.X, closestPoint.Y, closestPoint.Z );
					distanceV.sub4( havokCenter );
				}
			}
		}

		if( length < range )
		{
			float factor = 1-(length/range);
			float pow = strength * factor;
			float damage = weapon_damage * sinf(factor*F_PI/2.0f);

			// Rocket jumps
			if( po->mPlayerVariables )
			{
				if( po->mPlayerVariables->Controller )
					po->mPlayerVariables->Controller->setPlayerShocked( (short)((GEngine->Physics->TWPlayerShockedTime - GEngine->Physics->TWPlayerExplosionShockedTime)*1000.f) );
				else
					po->mPlayerVariables->TimeSinceExplosion = GEngine->Physics->TWPlayerShockedTime - GEngine->Physics->TWPlayerExplosionShockedTime;
			}

#ifdef HAVOK3
			if( (po->mHavokPart->Body->getMotionType() != hkMotion::MOTION_KEYFRAMED) && ( length > 0.0001 ) )
#else
			if( !po->mHavokPart->Body->isKeyframed() && ( length > 0.0001 ) )
#endif
			{
				distanceV.normalize3();
				hkVector4 imp( distanceV );
				imp.mul4(pow);

                // player rocketjump
				if( po->mPlayerVariables )
				{
					if( ( po->mPlayerVariables->Controller && po->mPlayerVariables->Controller->isFalling() ) ||
						( po->mPlayerVariables->Falling ) )
					{
						Vector v = po->GetVel();
						if (v.Y > 1)
						    imp.mul4(v.Y * 0.28f);
					}
                }
                
                if (!(po->State & EPOExplosionsDontWorkOnIt))
                {
                    if (po->mHavokPart->Body->getSimulationIsland())
#ifdef HAVOK3
                        po->mHavokPart->Body->activate();
#else
                        po->mHavokPart->Body->forceActivate();
#endif
					Vector force( imp(0), imp(1), imp(2) );
					if( !newPawnCenter.equals3(pawnCenter) )
					{
						Vector pCenter( newPawnCenter(0), newPawnCenter(1), newPawnCenter(2) );
						po->EffectForce( force, &pCenter );
					}
					else
						po->EffectForce( force );
                }
			}
			else if( po->mPlayerVariables && po->mPlayerVariables->Controller )
			{
				distanceV.normalize3();
				hkVector4 imp( distanceV );
				imp.mul4(pow/160);

                // player rocketjump
				if( po->mPlayerVariables->Controller->isFalling() )
				{
					hkVector4 v = po->mPlayerVariables->Controller->getLinearVelocity();
					if( v(1) > 1 )
						imp.mul4(v(1) * 0.28f);
				}
                
				po->mPlayerVariables->Controller->addVelocity( imp );
			}

			if( !entity )
				entity = po->GetEntity();

			if( entity )
			{
				ExplosionMessage em;
				em.entity = entity;
				em.damage = damage;
				ExplosionMessages.Add( em );
			}
		}
	}

	// Send explosion messages
	for( int i = 0; i < ExplosionMessages.Size(); ++i )
		GScript.QueueMessage("EXPLOSION","%e,%f,%f,%f,%d,%d,%d,%f",ExplosionMessages[i].entity,
			center.X,center.Y,center.Z,explosionCount,ownerID,weapon_type,ExplosionMessages[i].damage);

	// Explode glass
#ifdef	TRANSGAMING
	for( int i = 0; i < GEngine->pcfWorld->Glasses.Size(); ++i )
	{
		WorldMesh* wm = GEngine->pcfWorld->Glasses[i]->GetMainMesh();
#else
	for( int i = 0; i < GEngine->World->Glasses.Size(); ++i )
	{
		WorldMesh* wm = GEngine->World->Glasses[i]->GetMainMesh();
#endif
		Vector wmCenter = wm->GetCenter();
		hkVector4 pawnCenter;
		pawnCenter.set( wmCenter.X, wmCenter.Y, wmCenter.Z );
		hkVector4 distanceV( pawnCenter );
		distanceV.sub4( havokCenter );
		float length = distanceV.length3();

		if( length >= 3 * range * Glass::ExplosionRangeModifier ) continue;

		Vector closestPoint;
		float closestDistance;
		wm->GetClosestPoint( center, closestDistance, closestPoint );
		if( closestDistance >= length )
		{
			closestDistance = length;
			closestPoint = wmCenter;
		}

		length = closestDistance;

		if( length >= range * Glass::ExplosionRangeModifier ) continue;
		Vector vel = closestPoint-center;
		vel.Normalize();

#ifdef	TRANSGAMING
		GEngine->pcfWorld->Glasses[i]->StartGlass( center, range*Glass::ExplosionRangeModifier, vel*50.0f*((range-length)/range) );
#else
		GEngine->World->Glasses[i]->StartGlass( center, range*Glass::ExplosionRangeModifier, vel*50.0f*((range-length)/range) );
#endif
	}

	// Affect pinned active meshes
	DynamicArray<BoundingBox> explodingMeshesBoxes;
	int oldHowManyBoxes = 0;

	for( int i = 0; i < PinnedActiveMeshes.Size(); i++ )
	{
		if( PinnedActiveMeshes[i].ActivationDisabled )
			continue;

		hkVector4 pawnCenter;
		pawnCenter.set(
			PinnedActiveMeshes[i].ActivationPoint.X,
			PinnedActiveMeshes[i].ActivationPoint.Y,
			PinnedActiveMeshes[i].ActivationPoint.Z );
		
		hkVector4 distanceV( pawnCenter );
		distanceV.sub4( havokCenter );
		float length = distanceV.length3();

		if( length < range + PinnedActiveMeshes[i].ActivationExtent )
		{
			if( !PinnedActiveMeshes[i].RecurrentActivationDisabled )
				explodingMeshesBoxes.Add( PinnedActiveMeshes[i].StaticMesh->Box );
			ActivatePinnedMesh( i, center, range, strength, ownerID, weapon_type, weapon_damage, explosionCount );
			i--;
		}
	}

	// Affect meshes, boxes of which intersect with boxes of exploded meshes
	while( explodingMeshesBoxes.Size() > oldHowManyBoxes )
	{
		oldHowManyBoxes = explodingMeshesBoxes.Size();
		for( int i = 0; i < PinnedActiveMeshes.Size(); ++i )
		{
			if( PinnedActiveMeshes[i].ActivationDisabled ) continue;

			hkVector4 pawnCenter;
			pawnCenter.set(
				PinnedActiveMeshes[i].ActivationPoint.X,
				PinnedActiveMeshes[i].ActivationPoint.Y,
				PinnedActiveMeshes[i].ActivationPoint.Z );
		
			hkVector4 distanceV( pawnCenter );
			distanceV.sub4( havokCenter );
			distanceV(1) = 0;
			float length = distanceV.length3();
			if( length > MaxRecursiveActivationDistance )
				continue;

			for( int j = 0; j < explodingMeshesBoxes.Size(); ++j )
				if( PinnedActiveMeshes[i].StaticMesh->Box.Intersect( explodingMeshesBoxes[j], ACTIVE_MESHES_RECURSIVE_ACTIVATION_EPSILON ) )
				{
					if( !PinnedActiveMeshes[i].RecurrentActivationDisabled )
						explodingMeshesBoxes.Add( PinnedActiveMeshes[i].StaticMesh->Box );
					ActivatePinnedMesh( i, center, range, strength, ownerID, weapon_type, weapon_damage, explosionCount );
					i--;
					break;
				}
		}
	}

	// Affect ragdolls

	// Make a zero-radius sphere
	hkRigidBody* explosionBody;
	{
        m_setTag("Physics");
		hkSphereShape* sphereShape = new hkSphereShape( 0.01f );
#ifdef HAVOK3
		hkRigidBodyCinfo sphereInfo;
#else
		hkRigidBodyConstructionInfo sphereInfo;
#endif
		sphereInfo.m_shape = sphereShape;
		sphereInfo.m_position = havokCenter;
#ifdef HAVOK3
		sphereInfo.m_motionType = hkMotion::MOTION_FIXED;
#else
		sphereInfo.m_motionType = HK_FIXED_MOTION;
#endif
		sphereInfo.m_collisionFilterInfo = hkGroupFilter::calcFilterInfo( HCGNoncolliding );
		sphereInfo.m_mass = 10.0f;	// just in case
        m_setTag("Physics");
		explosionBody = new hkRigidBody( sphereInfo );
		sphereShape->removeReference();
		World->addEntity( explosionBody );
	}

	for( int i = 0; i < Ragdolls.Size(); i++ )
		Ragdolls[i]->SelfExplosion( center, strength, range, ownerID, weapon_type, weapon_damage, explosionCount, explosionBody );

	World->removeEntity( explosionBody );
	explosionBody->removeReference();
}

void HavokWorld::MultiplayerExplosion( Vector center, float strength, float range, int ownerID, int weapon_type, float weapon_damage, float factorY )
{
#ifndef HAVOK3 // ============================================

	static int explosionCount=0;
	explosionCount++;

    //GScript.DoString("Game:Print('Explosion2: %f,%f,%f')",center.X,center.Y,center.Z);

	hkVector4 havokCenter( center.X, center.Y, center.Z );

	static DynamicArray<ExplosionMessage> ExplosionMessages;
	ExplosionMessages.ClearArray();

	// Affect physics objects
	for( int i = 0; i < PhysicsObjects.Size(); i++ )
	{
		PhysicsObject* po = PhysicsObjects[i];
		Assert( po->mHavokPart->Body->getSimulationIsland() );

		Entity* entity = 0;
		hkVector4 pawnCenter;
		if( ( po->BodyType == EPawnFromMeshNonCentered ) || ( po->BodyType == EPawnFromMeshNonCenteredSweep ) )
		{
			entity = po->GetEntity();
			Vector pawnC = entity->Box.GetCenter();
			pawnCenter.set( pawnC.X, pawnC.Y, pawnC.Z );
		}
		else
			pawnCenter = po->mHavokPart->Body->getCenterOfMassInWorld();
		hkVector4 distanceV( pawnCenter );
		distanceV.sub4( havokCenter );
		float length = distanceV.length3();
		hkVector4 newPawnCenter = pawnCenter;

		if( length < 3 * range )
		{
			if( !entity )
				entity = po->GetEntity();

			if( entity->Type == Entity::ETypeMesh )
			{
				WorldMesh* wm = (WorldMesh*)entity;
				Vector closestPoint;
				float closestDistance;
				wm->GetClosestPoint( center, closestDistance, closestPoint );
				if( closestDistance < length )
				{
					length = closestDistance;
					newPawnCenter.set( closestPoint.X, closestPoint.Y, closestPoint.Z );
					distanceV.set( closestPoint.X, closestPoint.Y, closestPoint.Z );
					distanceV.sub4( havokCenter );
				}
			}
		}

		if( length < range )
		{
			float factor = 1.0f-( (length/range)*(1.0f-GEngine->Physics->TWMPMinimalExplosionImpulse) );
			float pow = strength * factor;
			float damage = weapon_damage * factor;

			// Multiplayer explosion effect correction, when the hit body is the player
			bool isHit = true;
			if( po->GetType() == EPawnBodyPlayer )
			{
				// Trace from explosion to points on the player ( joint positions ) and pronounce him not hit, if
				// none of the points was hit

				if( !entity )
					entity = po->GetEntity();

				if( entity && entity->AsModel() )
				{
					bool allTracesBlocked = true;

					HitData hitData;
					hkVector4 start( center.X, center.Y, center.Z );
					hkVector4 end;

					hkBroadPhase* broadPhase = World->getBroadPhase();

					// Disable players' own body for tracing
					bool canCollide = po->CanLineTraceCollision();
					po->EnableLineTraceCollision( false );
					Ragdoll* r = ((Model*)entity)->RagdollPtr;
					bool canCollideRagdoll = false;
					if( r )
					{
						canCollideRagdoll = r->CanLineTraceCollision();
						r->EnableLineTraceCollision( false );
					}

					const char* jointNames[] = { "k_head", "r_l_lok", "r_p_lok", "K_ogo", "n_p_kol", "n_l_kol" };	// for player model
					const int jointNamesCount = 6;

					for( int j = 0; j < jointNamesCount; ++j )
					{
						int ji = entity->AsModel()->GetJointIndex(jointNames[j]);
						if( ji )
						{
							Vector pos = entity->AsModel()->GetJointTransform( ji ).GetTranslation();
							end.set( pos.X, pos.Y, pos.Z );

							hkVector4 startModified(start);
							hkVector4 temp(end);
							temp.sub4(start);
							float l = temp.length3();
							if( l < 0.001f )
							{
								allTracesBlocked = false;
								break;
							}
							temp.mul4( 0.1f/l );
							startModified.add4( temp );

							hkShape::hkRayResults initResults;
							havokFilteredRaycastCallback rayCallBack( startModified, end, initResults );
							broadPhase->castPrimitive( startModified, end, rayCallBack );
							initResults = rayCallBack.getRayResults();
							if( !rayCallBack.getHitObject() )
							{
								allTracesBlocked = false;
								break;
							}
                            else
                            {
                                /*
                                hkRigidBody* rb = (hkRigidBody*)rayCallBack.getHitObject()->getOwner();
                                if( !rb->hasProperty( HPEntityPointer ) )
                                    GLog.Print( "Multiplayer Explosion collided with rigid body which has no entity!\n" );
                                else
                                {
                                    hkPropertyValue p = rb->getProperty( HPEntityPointer );
                                    Entity* rigidEntity = (Entity*)p.asPtr;
                                    if( !rigidEntity )
                                        GLog.Print( "Multiplayer Explosion collided with rigid body which has no entity!\n" );
                                    else
                                        GLog.Print( "Multiplayer Explosion collided with entity '%s', filename '%s'\n",
                                            rigidEntity->Name.AsChar(), rigidEntity->FileName.AsChar() );
                                }
                                */
                            }
						}
					}

					// Return players' own body to normal state after tracing
					if( canCollide ) po->EnableLineTraceCollision( true );
					if( r && canCollideRagdoll ) r->EnableLineTraceCollision( true );

					isHit = !allTracesBlocked;
					//if( !isHit ) 
					//	GLog.Print( "All traces blocked!!\n" );
				}

				// Rocket jumps
				if( isHit )
				{
					if( po->mPlayerVariables->Controller )
						po->mPlayerVariables->Controller->setPlayerShocked( (GEngine->Physics->TWMPPlayerShockedTime - GEngine->Physics->TWMPPlayerExplosionShockedTime)*1000.f );
					else
						po->mPlayerVariables->TimeSinceExplosion = GEngine->Physics->TWMPPlayerShockedTime - GEngine->Physics->TWMPPlayerExplosionShockedTime;
				}
			}

			if( isHit )
			{
				if( !po->mHavokPart->Body->isKeyframed() && ( length > 0.0001 ) )
				{
					distanceV.normalize3();
					hkVector4 imp( distanceV );
					imp.mul4(pow);
                
	                if (!(po->State & EPOExplosionsDontWorkOnIt))
	                {
						if( imp(1) > 0 )	// for PCF MP Mode
							imp(1) *= factorY;

						if( po->mMissileController )
							po->mMissileController->addVelocity( imp );
						else
						{
							if (po->mHavokPart->Body->getSimulationIsland())
#ifdef HAVOK3
							    po->mHavokPart->Body->activate();
#else
							    po->mHavokPart->Body->forceActivate();
#endif

							Vector force( imp(0), imp(1), imp(2) );

							if( !newPawnCenter.equals3(pawnCenter) )
							{
								Vector pCenter( newPawnCenter(0), newPawnCenter(1), newPawnCenter(2) );
								po->EffectForce( force, &pCenter );
							}
							else
								po->EffectForce( force );
						}
	                }
				}
				else if( po->mPlayerVariables && po->mPlayerVariables->Controller )
				{
					distanceV.normalize3();
					hkVector4 imp( distanceV );
					imp.mul4(pow/160);

					if( imp(1) > 0 )	// for PCF MP Mode
						imp(1) *= factorY;

					if( GEngine->Physics->TWMPAlternateRJ )
					{
						if( !entity )
							entity = po->GetEntity();

						if( !GEngine->Physics->TWMPAlternateRJOnlyMyRockets || ( GEngine->Net->GetPlayerByIndex( ownerID ) == entity ) )
						{
							if( pow/160 > GEngine->Physics->TWMPAlternateRJWorkAboveThisStrength )
							{
								hkVector4 v = po->mPlayerVariables->Controller->getLinearVelocity();

								hkVector4 vHoriz( po->mPlayerVariables->Controller->getLinearVelocity() );
								vHoriz(1) = 0;
								hkVector4 impHoriz( imp );
								impHoriz(1) = 0;

								float lenV = vHoriz.length3();
								float lenI = impHoriz.length3();

								if( ( lenV > 0.001f ) && ( lenI > 0.001f ) )
								{
									hkVector4 impForward(vHoriz);
#ifdef	TRANSGAMING_PS2
									float multiplier = hkReal(vHoriz.dot3( impHoriz ))/lenV/lenI;
#else
									float multiplier = vHoriz.dot3( impHoriz )/lenV/lenI;
#endif
									if( multiplier < 0 )
									{
										impForward.mul4( multiplier );
										imp.sub4(impForward);
									}
								}
//								if( ( ( imp(0) < 0 ) && ( v(0) > 0 ) ) || ( ( imp(0) > 0 ) && ( v(0) < 0 ) ) ) imp(0) = 0;
//								if( ( ( imp(2) < 0 ) && ( v(2) > 0 ) ) || ( ( imp(2) > 0 ) && ( v(2) < 0 ) ) ) imp(2) = 0;
							}
						}
					}
                
					po->mPlayerVariables->Controller->addVelocity( imp );
				}

				if( !entity )
					entity = po->GetEntity();

				if( entity )
				{
					ExplosionMessage em;
					em.entity = entity;
					em.damage = damage;
					ExplosionMessages.Add( em );
				}
			}
		}
	}

	// Send explosion messages
	for( int i = 0; i < ExplosionMessages.Size(); ++i )
		GScript.QueueMessage("EXPLOSION","%e,%f,%f,%f,%d,%d,%d,%f",ExplosionMessages[i].entity,
			center.X,center.Y,center.Z,explosionCount,ownerID,weapon_type,ExplosionMessages[i].damage);

	// Explode glass
#ifdef	TRANSGAMING
	for( int i = 0; i < GEngine->pcfWorld->Glasses.Size(); ++i )
	{
		WorldMesh* wm = GEngine->pcfWorld->Glasses[i]->GetMainMesh();
#else
	for( int i = 0; i < GEngine->World->Glasses.Size(); ++i )
	{
		WorldMesh* wm = GEngine->World->Glasses[i]->GetMainMesh();
#endif
		Vector wmCenter = wm->GetCenter();
		hkVector4 pawnCenter;
		pawnCenter.set( wmCenter.X, wmCenter.Y, wmCenter.Z );
		hkVector4 distanceV( pawnCenter );
		distanceV.sub4( havokCenter );
		float length = distanceV.length3();

		if( length >= 3 * range * Glass::ExplosionRangeModifier ) continue;

		Vector closestPoint;
		float closestDistance;
		wm->GetClosestPoint( center, closestDistance, closestPoint );
		if( closestDistance >= length )
		{
			closestDistance = length;
			closestPoint = wmCenter;
		}

		length = closestDistance;

		if( length >= range * Glass::ExplosionRangeModifier ) continue;
		Vector vel = closestPoint-center;
		vel.Normalize();

#ifdef	TRANSGAMING
		GEngine->pcfWorld->Glasses[i]->StartGlass( center, range*Glass::ExplosionRangeModifier, vel*50.0f*((range-length)/range) );
#else
		GEngine->World->Glasses[i]->StartGlass( center, range*Glass::ExplosionRangeModifier, vel*50.0f*((range-length)/range) );
#endif
	}

	// Affect pinned active meshes
	DynamicArray<BoundingBox> explodingMeshesBoxes;
	int oldHowManyBoxes = 0;

	for( int i = 0; i < PinnedActiveMeshes.Size(); i++ )
	{
		if( PinnedActiveMeshes[i].ActivationDisabled )
			continue;

		hkVector4 pawnCenter;
		pawnCenter.set(
			PinnedActiveMeshes[i].ActivationPoint.X,
			PinnedActiveMeshes[i].ActivationPoint.Y,
			PinnedActiveMeshes[i].ActivationPoint.Z );
		
		hkVector4 distanceV( pawnCenter );
		distanceV.sub4( havokCenter );
		float length = distanceV.length3();

		if( length < range + PinnedActiveMeshes[i].ActivationExtent )
		{
			if( !PinnedActiveMeshes[i].RecurrentActivationDisabled )
				explodingMeshesBoxes.Add( PinnedActiveMeshes[i].StaticMesh->Box );
			ActivatePinnedMesh( i, center, range, strength, ownerID, weapon_type, weapon_damage, explosionCount );
			i--;
		}
	}

	// Affect meshes, boxes of which intersect with boxes of exploded meshes
	while( explodingMeshesBoxes.Size() > oldHowManyBoxes )
	{
		oldHowManyBoxes = explodingMeshesBoxes.Size();
		for( int i = 0; i < PinnedActiveMeshes.Size(); ++i )
		{
			if( PinnedActiveMeshes[i].ActivationDisabled ) continue;

			hkVector4 pawnCenter;
			pawnCenter.set(
				PinnedActiveMeshes[i].ActivationPoint.X,
				PinnedActiveMeshes[i].ActivationPoint.Y,
				PinnedActiveMeshes[i].ActivationPoint.Z );
		
			hkVector4 distanceV( pawnCenter );
			distanceV.sub4( havokCenter );
			distanceV(1) = 0;
			float length = distanceV.length3();
			if( length > MaxRecursiveActivationDistance )
				continue;

			for( int j = 0; j < explodingMeshesBoxes.Size(); ++j )
				if( PinnedActiveMeshes[i].StaticMesh->Box.Intersect( explodingMeshesBoxes[j], ACTIVE_MESHES_RECURSIVE_ACTIVATION_EPSILON ) )
				{
					if( !PinnedActiveMeshes[i].RecurrentActivationDisabled )
						explodingMeshesBoxes.Add( PinnedActiveMeshes[i].StaticMesh->Box );
					ActivatePinnedMesh( i, center, range, strength, ownerID, weapon_type, weapon_damage, explosionCount );
					i--;
					break;
				}
		}
	}

	// Affect ragdolls

	// Make a zero-radius sphere
	hkRigidBody* explosionBody;
	{
        m_setTag("Physics");
		hkSphereShape* sphereShape = new hkSphereShape( 0.01f );
#ifdef HAVOK3
		hkRigidBodyCinfo sphereInfo;
#else
		hkRigidBodyConstructionInfo sphereInfo;
#endif
		sphereInfo.m_shape = sphereShape;
		sphereInfo.m_position = havokCenter;
#ifdef HAVOK3
		sphereInfo.m_motionType = hkMotion::MOTION_FIXED;
#else
		sphereInfo.m_motionType = HK_FIXED_MOTION;
#endif
		sphereInfo.m_collisionFilterInfo = hkGroupFilter::calcFilterInfo( HCGNoncolliding );
		sphereInfo.m_mass = 10.0f;	// just in case
        m_setTag("Physics");
		explosionBody = new hkRigidBody( sphereInfo );
		sphereShape->removeReference();
		World->addEntity( explosionBody );
	}

	for( int i = 0; i < Ragdolls.Size(); i++ )
		Ragdolls[i]->SelfExplosion( center, strength, range, ownerID, weapon_type, weapon_damage, explosionCount, explosionBody );

	World->removeEntity( explosionBody );
	explosionBody->removeReference();

#else
	GLog.Print("HavokWorld::MultiplayerExplosion() - FIXME!!!!!!!!!!!\n");
#endif // ifndef HAVOK3 =========================================
}

void HavokWorld::PinnedMeshGroupEnable( unsigned char group, bool enable )
{
	for( int i = 0; i < PinnedActiveMeshes.Size(); i++ )
	{
		if( PinnedActiveMeshes[i].StaticMesh->ActiveGroup != group )
			continue;
		PinnedActiveMeshes[i].ActivationDisabled = !enable;
	}
}

void HavokWorld::PinnedMeshGroupStaticMeshEnable( unsigned char group, bool enable )
{
	for( int i = 0; i < PinnedActiveMeshes.Size(); i++ )
	{
		if( PinnedActiveMeshes[i].StaticMesh->ActiveGroup != group )
			continue;
		if( enable )
		{
			if( !PinnedActiveMeshes[i].StaticMeshBody->getSimulationIsland() )
				World->addEntity( PinnedActiveMeshes[i].StaticMeshBody );
		}
		else
		{
			if( PinnedActiveMeshes[i].StaticMeshBody->getSimulationIsland() )
				World->removeEntity( PinnedActiveMeshes[i].StaticMeshBody );
		}
	}
}

void HavokWorld::PinnedMeshGroupRecurrentActivationEnable( unsigned char group, bool enable )
{
	for( int i = 0; i < PinnedActiveMeshes.Size(); i++ )
	{
		if( PinnedActiveMeshes[i].StaticMesh->ActiveGroup != group )
			continue;
		PinnedActiveMeshes[i].RecurrentActivationDisabled = !enable;
	}
}

void HavokWorld::PinnedMeshGroupActivate( unsigned char group )
{
	PinnedMeshGroupEnable( group, true );

	// Affect pinned active meshes
	DynamicArray<BoundingBox> explodingMeshesBoxes;
	int oldHowManyBoxes = 0;

	for( int i = 0; i < PinnedActiveMeshes.Size(); i++ )
	{
		if( PinnedActiveMeshes[i].StaticMesh->ActiveGroup != group )
			continue;

		if( !PinnedActiveMeshes[i].RecurrentActivationDisabled )
			explodingMeshesBoxes.Add( PinnedActiveMeshes[i].StaticMesh->Box );
		ActivatePinnedMesh( i, PinnedActiveMeshes[i].ActivationPoint, 6, 0, 0, 0, 0, 0 );
		i--;
	}

	// Affect meshes, boxes of which intersect with boxes of exploded meshes
	while( explodingMeshesBoxes.Size() > oldHowManyBoxes )
	{
		oldHowManyBoxes = explodingMeshesBoxes.Size();
		for( int i = 0; i < PinnedActiveMeshes.Size(); ++i )
		{
			for( int j = 0; j < explodingMeshesBoxes.Size(); ++j )
				if( PinnedActiveMeshes[i].StaticMesh->Box.Intersect( explodingMeshesBoxes[j], ACTIVE_MESHES_RECURSIVE_ACTIVATION_EPSILON ) &&
					!PinnedActiveMeshes[i].ActivationDisabled )
				{
					if( !PinnedActiveMeshes[i].RecurrentActivationDisabled )
						explodingMeshesBoxes.Add( PinnedActiveMeshes[i].StaticMesh->Box );
					ActivatePinnedMesh( i, PinnedActiveMeshes[i].ActivationPoint, 6, 0, 0, 0, 0, 0 );
					i--;
					break;
				}
		}
	}
}

bool HavokWorld::CheckStartGlass( hkRigidBody* rb, Vector hitPoint, float activationRadius, Vector velocityToSet )
{
	if( rb->hasProperty( HPGlass ) )
	{
		hkPropertyValue p = rb->getProperty( HPGlass );
#ifdef HAVOK3
		Glass* ourGlass = (Glass*)p.getPtr();
#else
		Glass* ourGlass = (Glass*)p.asPtr;
#endif
		ourGlass->StartGlass( hitPoint, activationRadius, velocityToSet );
		if( !rb->isFixed() )
			ourGlass->ActivateFragment( rb, hitPoint, velocityToSet );
		return true;
	}
	return false;
}

void HavokWorld::ActiveMeshGroupSetTimeToRemove( unsigned char group, float time, float randomizer )
{
	if( !PinnedActiveMeshes.Size() ) return;

	if( time == 0.0f )
	{
		for( int i = PinnedActiveMeshes.Size()-1; i >= 0; --i )
		{
			PinnedActiveMesh& pam = PinnedActiveMeshes[i];
			if( pam.StaticMesh->ActiveGroup == group )
				RemovePinnedActiveMesh( i , true );
		}
		return;
	}

	for( int i = 0; i < PinnedActiveMeshes.Size(); ++i )
	{
		PinnedActiveMesh& pam = PinnedActiveMeshes[i];
		if( pam.StaticMesh->ActiveGroup == group )
		{
            float r = ((float)rand()/(float)RAND_MAX) * randomizer;
            if( rand()%2 == 0 ) r = -r;
			float ttd = time + r;
			if( ttd < 0.0f ) ttd = 0.00000001f;
			if( ttd == 0.0f )
			{
				RemovePinnedActiveMesh( i, true );
				i--;
			}
			else
				pam.TimeToDelete = ttd;
		}
	}
}

void HavokWorld::RemovePinnedActiveMesh( int meshIndex, bool dontDeleteEntity )
{
	PinnedActiveMesh& pam = PinnedActiveMeshes[meshIndex];
	RemoveStaticMesh( pam.StaticMeshBody );

	if( pam.ActiveMesh )
		delete pam.ActiveMesh;
	else						// find and remove parts
	{
		for( int j = 0; j < pam.MeshParts.Size(); ++j )
#ifdef	TRANSGAMING
			GEngine->pcfWorld->DeleteEntity( pam.MeshParts[j] );
#else
			GEngine->World->DeleteEntity( pam.MeshParts[j] );
#endif
		pam.MeshParts.ClearArray();
	}

	pam.StaticMesh->PO = 0;

//	if( dontDeleteEntity )
//	{
		pam.StaticMesh->Flags |= Entity::EEntNoDraw;
		for( int i = 0; i < pam.StaticMesh->Children.Size(); ++i )
			pam.StaticMesh->Children[i]->Flags |= Entity::EEntNoDraw;
//	}
//	else
//		GEngine->World->DeleteEntity( pam.StaticMesh );

	PinnedActiveMeshes.FastRemove(meshIndex);
}

void HavokWorld::ActivatePinnedMesh( int meshNumber, Vector activationCenter, float explosionRange, float strength, int ownerID, int weapon_type, float weapon_damage, int explosionCount )
{
	PinnedActiveMesh& pam = PinnedActiveMeshes[meshNumber];

	ActiveMeshGroupActivationParams* activationParams = AMGActivationParams.Find( pam.StaticMesh->ActiveGroup );

	hkVector4 havokCenter( activationCenter.X, activationCenter.Y, activationCenter.Z );
	hkVector4 pawnCenter;
	pawnCenter.set( pam.ActivationPoint.X, pam.ActivationPoint.Y, pam.ActivationPoint.Z );
	hkVector4 distanceV( pawnCenter );
	distanceV.sub4( havokCenter );
	float length = distanceV.length3();

	// Remove static mesh body
	RemoveStaticMesh( pam.StaticMeshBody );
	//GScript.DoString( "Game.PlayerDestroyedItems = Game.PlayerDestroyedItems + 1" );

	if( pam.ActiveMesh )	// it will just activate, into one part
	{
		Entity* entity = pam.StaticMesh;
		PhysicsObject* po = pam.ActiveMesh;
		Assert( po->mHavokPart->Body );

		PinnedActiveMeshes.FastRemove(meshNumber);

		// Add active mesh to world
		po->Enable( true );
		po->Activate( true );

		// Set collision callbacks?
		if( activationParams && activationParams->EnableCollisions )
		{
			float mass = po->mHavokPart->Body->getMass();
#ifdef HAVOK_POSITION_BUG_TRACKING
			Assert( mass >= 0.0f );
#endif
			if( ( mass < activationParams->MaximalMass ) && ( mass > activationParams->MinimalMass ) &&
				( rand()%100 < (activationParams->MinProbability + mass / activationParams->MaximalMass * activationParams->MinProbability ) ) )
				po->SetCollisionCallbacks( activationParams->EnableCollisions, activationParams->MinimalDelay, activationParams->MinimalStrength );
		}

		if( strstr( entity->Name.AsChar(),"autodelete" ) )
		{
			float myTimeToDelete;

			if( activationParams )
				myTimeToDelete = activationParams->TTDBase + ((rand()*activationParams->TTDAddition)/RAND_MAX);
			else
				myTimeToDelete = 10 + rand()%10;

			if( ( entity->TimeToDelete == -1 ) || ( entity->TimeToDelete > myTimeToDelete ) )
			{
#ifdef HAVOK3
				po->mHavokPart->Body->setDeactivator( hkRigidBodyDeactivator::DEACTIVATOR_NEVER );
#else
				po->mHavokPart->Body->setDeactivator( HK_RIGIDBODY_DEACTIVATOR_NEVER );
#endif
				entity->TimeToDelete = myTimeToDelete;
			}
		}
		entity->Flags |= Entity::EEntDontDeleteOnTimer;

		WorldMesh* wm = (WorldMesh*)entity;
		Vector closestPoint;
		float closestDistance;
		wm->GetClosestPoint( activationCenter, closestDistance, closestPoint );
		hkVector4 havokClosestPoint = po->mHavokPart->Body->getCenterOfMassInWorld();

		if( closestDistance < length )
		{
			length = closestDistance;
			havokClosestPoint.set( closestPoint.X, closestPoint.Y, closestPoint.Z );
			distanceV = havokClosestPoint;
			distanceV.sub4( havokCenter );
		}

		float factor = 1-(length/explosionRange);
		if( factor < 0 ) factor = 0;
		float pow = strength * factor;
		float damage = weapon_damage * sinf(factor*F_PI/2.0f);
		if(length > 0.0001)
		{
			distanceV.normalize3();
			hkVector4 imp( distanceV );
			imp.mul4(pow);

			Vector force( imp(0), imp(1), imp(2) );
			Vector hClosestPoint( havokClosestPoint(0), havokClosestPoint(1), havokClosestPoint(2) );
			po->EffectForce( force, &hClosestPoint );
		}

		if( entity )
			GScript.QueueMessage("EXPLOSION","%e,%f,%f,%f,%d,%d,%d,%f",entity,
				activationCenter.X,activationCenter.Y,activationCenter.Z,explosionCount,ownerID,weapon_type,damage);
	}
	else
	{
		// this one has more than one active mesh - we will have to search in our list for its parts
		if( activationParams && activationParams->EnableCollisions ) {
			Entity* entity = pam.StaticMesh;
			if (entity)
				GScript.QueueMessage("EXPLODEMESH","%d,%f,%f,%f",entity->AsMesh()->ActiveGroup,
					activationCenter.X,activationCenter.Y,activationCenter.Z);
		}

		// find parts
		DynamicArray<Entity*> parts = pam.MeshParts;

		for( int i = 0; i < pam.StaticMesh->Children.Size(); ++i )
			pam.StaticMesh->Children[i]->Flags |= Entity::EEntNoDraw;
#ifdef	TRANSGAMING
		GEngine->pcfWorld->RemoveEntity( pam.StaticMesh );
#else
		GEngine->World->RemoveEntity( pam.StaticMesh );
#endif
		PinnedActiveMeshes.FastRemove( meshNumber );

		// activate parts
		for( int j = 0; j < parts.Size(); ++j )
		{
#ifdef	TRANSGAMING
			GEngine->pcfWorld->AddEntity( parts[j] );
			GEngine->pcfWorld->MeshesTableToSlotAdd( (WorldMesh*)parts[j] );
#else
			GEngine->World->AddEntity( parts[j] );
			GEngine->World->MeshesTableToSlotAdd( (WorldMesh*)parts[j] );
#endif

			parts[j]->TimeToDelete = ( activationParams )?(activationParams->TTDBase + ((rand()*activationParams->TTDAddition)/RAND_MAX)):(10 + rand()%10);;
			parts[j]->PO->SetVel( Vector(0,0,0) );
			parts[j]->PO->Enable( true );
			parts[j]->PO->Activate( true );

			// Set collision callbacks?
			if( activationParams && activationParams->EnableCollisions )
			{
				float mass = parts[j]->PO->mHavokPart->Body->getMass();
#ifdef HAVOK_POSITION_BUG_TRACKING
				Assert( mass >= 0.0f );
#endif
				if( ( mass < activationParams->MaximalMass ) && ( mass > activationParams->MinimalMass ) &&
					( rand()%100 < (activationParams->MinProbability + mass / activationParams->MaximalMass * activationParams->MinProbability) ) )
					parts[j]->PO->SetCollisionCallbacks( activationParams->EnableCollisions, activationParams->MinimalDelay, activationParams->MinimalStrength );
			}

			// apply force to part
			Vector v = parts[j]->GetCenter();
			hkVector4 distanceVtemp;
			distanceVtemp.set(v.X,v.Y,v.Z);
			distanceV.sub4( havokCenter );
			float partLength = distanceVtemp.length3();

			Vector closestPoint;
			float closestDistance;
			parts[j]->AsMesh()->GetClosestPoint( activationCenter, closestDistance, closestPoint );
			hkVector4 havokClosestPoint = parts[j]->PO->mHavokPart->Body->getCenterOfMassInWorld();
			if( closestDistance < length )
			{
				partLength = closestDistance;
				havokClosestPoint.set( closestPoint.X, closestPoint.Y, closestPoint.Z );
				distanceVtemp = havokClosestPoint;
				distanceVtemp.sub4( havokCenter );
			}

			if( partLength < explosionRange )
			{
				float factor = 1-(partLength/explosionRange);
				if( factor < 0.0f ) factor = 0.0f;
				float pow = strength * factor / 10;
				if( partLength > 0.0001 )
				{
					distanceVtemp.normalize3();
					hkVector4 imp( distanceVtemp );
					imp.mul4( pow );
                
					Vector force( imp(0), imp(1), imp(2) );
					Vector hClosestPoint( havokClosestPoint(0), havokClosestPoint(1), havokClosestPoint(2) );
					parts[j]->PO->EffectForce( force, &hClosestPoint );
				}
			}
		}
	}
}

void HavokWorld::PrintMeshStatistics( WorldMesh* mesh )
{
#ifndef HAVOK3 // ============================================

#ifdef _DEBUG
	if(!mesh->Data->VertList.Size())     return;
	if(!mesh->Data->Triangles.Size()) return;

	const int vertexCount = mesh->Data->VertList.Size();
	const int triangleCount = mesh->Data->Triangles.Size()/3;

	// Geometry tests
    m_setTag("Physics");
	hkVector4* verts = new hkVector4[vertexCount];
    for( int i = 0; i < vertexCount; ++i ){
        Vector pos = mesh->Data->VertList.getVertPos(i);


		verts[i].set( pos.X, pos.Y, pos.Z );
    }

    m_setTag("Physics");
	hkTriangleIndices* ti = new hkTriangleIndices[triangleCount];
	for( int i = 0; i < triangleCount; ++i )
		ti[i].setTriangleIndices( mesh->Data->Triangles[3*i], mesh->Data->Triangles[3*i+1], mesh->Data->Triangles[3*i+2] );

	hkGeometry geom;
	geom.setVertices( verts, vertexCount );
	geom.setTriangles( ti, triangleCount );
	hkGeometryCheckerDetails details;
	details.init();

	bool isValid = true;

	// at least four vertices?
	if( !hkGeometryChecker::atLeastFourVertices( &geom, details, false ) )
	{
		GLog.Print( "Number of vertices is LESS than 4!\n" );
		isValid = false;
	}
/*
	// all vertices used correctly? (at least 3 times)
	if( !hkGeometryChecker::allVerticesUsedCorrectly( &geom, details, false ) )
	{
		GLog.Print( "Some vertices are used less than three times!\n" );
		isValid = false;

//		if( details.m_vertsUsedThreeTimesTest == hkGeometryChecker::FAILED )	GLog.Print( "The following vertices are used less than three times:\n" );

//		for( int i = 0; i < details.m_numTimesVertexUsed.getSize(); ++i )
//			if( details.m_numTimesVertexUsed[i] < 3 )
//				GLog.Print( "\t%d is used %d time(s)\n", i, details.m_numTimesVertexUsed[i] );

	}
*/
	// edges used exactly twice?
	if( !hkGeometryChecker::edgesUsedExactlyTwice( &geom, details, false ) )
	{
		bool written = false;

		for( int i = 0; i < details.m_edges.getSize(); ++i )
			if( details.m_facesUsingEdge[i].getSize() > 2 )
			{
				if( !written )
				{
					GLog.Print( "Some edges are are not shared exactly twice with correct winding!\n" );
					GLog.Print( "Edge used more than twice\n" );
					written = true;
				}
				GLog.Print( "\tEdge is: %d %d\n", details.m_edges[i].getVertex(0), details.m_edges[i].getVertex(1) );
				GLog.Print( "\tEdge used %d times by face(s): \n", details.m_facesUsingEdge[i].getSize() );
				int j;
				for( j = 0; j < details.m_facesUsingEdge[i].getSize(); ++j )
					GLog.Print( "\t%d\n", details.m_facesUsingEdge[i][j] );
				isValid = false;
			}
	}

	// faces well defined and unique?
	if( !hkGeometryChecker::facesWellDefinedAndUnique( &geom, details, false ) )
	{
		GLog.Print( "Some faces are invalid!\n" );
		isValid = false;

		if( details.m_illdefinedTriangles.getSize() )
		{
			GLog.Print( "The following faces are ill-defined:\n" );
			for( int i = 0; i < details.m_illdefinedTriangles.getSize(); ++i )
				GLog.Print( "\t%d\n", details.m_illdefinedTriangles[i] );
		}

		if( details.m_duplicatedTriangles.getSize() )
		{
			GLog.Print( "The following faces are duplicated triangles:\n" );
			for( int i = 0; i < details.m_duplicatedTriangles.getSize(); ++i )
				GLog.Print( "\t%d\n", details.m_duplicatedTriangles[i] );
		}
	}
/*
	// how many unconnected parts?
	if( !hkGeometryChecker::isConnected( &geom, details, false ) )
	{
		GLog.Print( "More than one connected part!\n" );
		isValid = false;
		GLog.Print( "Number of connected components: %d\n", details.m_numConnectedComponenents );
	}
*/
	// has zero-area triangles?
	if( !hkGeometryChecker::noZeroAreaTriangles( &geom, details, false ) )
	{
		GLog.Print( "Some triangles have zero (or very small) area!\n" );
		isValid = false;

		GLog.Print( "The following triangles have small area:\n" );
		for( int i = 0; i < details.m_zeroAreaTriangles.getSize(); ++i )
			GLog.Print( "\t%d\n", details.m_zeroAreaTriangles[i] );
	}

	// has large edges?
	if( !hkGeometryChecker::noLargeEdges( &geom, details, false ) )
	{
		GLog.Print( "Some edges are too large!\n" );
		isValid = false;

		GLog.Print( "The following edges have large length:\n" );
		for( int i = 0; i < details.m_largeEdges.getSize(); ++i )
			GLog.Print( "\tEdge is: %d %d\n", details.m_largeEdges[i].getVertex(0), details.m_largeEdges[i].getVertex(1) );
	}

	// are some vertices coplanar?
	if( !hkGeometryChecker::notCoplanar( &geom, details, false ) )
	{
		GLog.Print( "Vertices are coplanar!\n" );
		isValid = false;
	}

	if( !isValid )
	{
		GLog.Print( "Invalid mesh name: %s\n", mesh->Name );
		// The following outputs the mesh in Havok .tk format
/*
		if( mesh->Name == "polysurfaceshape196" )
		{
			GLog.Print( "%d\n", vertexCount );
			for( int i = 0; i < vertexCount; ++i ){
                Vector pos = mesh.VertList.getVertPos(i);

				GLog.Print( "%f %f %f\n", pos.X, pos.Y, pos.Z );
            }
			GLog.Print( "%d\n", triangleCount );
			for( int i = 0; i < triangleCount; ++i )
				GLog.Print( "3 %d %d %d\n", mesh->Data->Triangles[3*i], mesh->Data->Triangles[3*i+1], mesh->Data->Triangles[3*i+2] );
		}
		else
*/
			GLog.Print( "\n" );
	}
#endif

#else
	GLog.Print("HavokWorld::PrintMeshStatistics() - FIXME!!!!!!!!!!!\n");
#endif // ifndef HAVOK3 =========================================
}

Model* HavokWorld::GetModelByConstraint( void* constraint )
{
	for( int i = 0; i < Ragdolls.Size(); ++i )
#ifdef HAVOK3
		if( Ragdolls[i]->ConstraintInRagdoll( (hkConstraintInstance*)constraint ) != -1 )
#else
		if( Ragdolls[i]->ConstraintInRagdoll( (hkConstraint*)constraint ) != -1 )
#endif
			return Ragdolls[i]->mModel;
	return 0;
}

bool HavokWorld::LineTraceStaticMesh( WorldMesh* mesh, Vector& start, Vector& end, HitData& hitData )
{
	for( int i = 0; i < StaticMeshes.Size(); ++i )
	{
		hkPropertyValue p = StaticMeshes[i]->getProperty( HPEntityPointer );
#ifdef HAVOK3
		WorldMesh* wm = (WorldMesh*)p.getPtr();
#else
		WorldMesh* wm = (WorldMesh*)p.asPtr;
#endif
		if( wm == mesh )
		{
			hkVector4 startWorld( start.X, start.Y, start.Z );
			hkVector4 endWorld( end.X, end.Y, end.Z );
#ifdef HAVOK3
			hkShapeRayCastInput input;
			hkShapeRayCastOutput output;

			input.m_from.setTransformedInversePos(StaticMeshes[i]->getTransform(), startWorld);
			input.m_to.setTransformedInversePos(StaticMeshes[i]->getTransform(), endWorld);

			if( StaticMeshes[i]->getCollidable()->getShape()->castRay( input, output ) )
			{
				hkVector4 intersectionPointWorld;
				intersectionPointWorld.setInterpolate4( startWorld, endWorld, output.m_hitFraction );
				// For debugging purposes...
#ifndef TRANSGAMING_WINDOWS // we're getting a few linking errors on these...
				HK_DISPLAY_LINE(startWorld, intersectionPointWorld, hkColor::CYAN);
				HK_SET_OBJECT_COLOR( hkUlong(StaticMeshes[i]->getCollidable()), hkColor::YELLOW);
#endif

#else
			hkShape::hkRayResults initResults;

//			const hkTransform& trans = StaticMeshes[i]->getTransform();
//			hkVector4 shapefrom,shapeto;
//			shapefrom.setTransformedInversePos( trans, startWorld );
//			shapeto.setTransformedInversePos( trans, endWorld );

			if( StaticMeshes[i]->getCollidable().getShape()->castRay( startWorld, endWorld, initResults ) )
//			if( StaticMeshes[i]->getCollidable().getShape()->castRay( shapefrom, shapeto, initResults ) )
			{
				hkVector4 intersectionPointWorld;
				intersectionPointWorld.setInterpolate4( startWorld, endWorld, initResults.m_mindist );
#endif
				hitData.HitPoint = Vector( intersectionPointWorld(0), intersectionPointWorld(1), intersectionPointWorld(2) );

				hkVector4 temp;
				temp = startWorld;
				temp.sub4( intersectionPointWorld );
				hitData.Dist = temp.length3();

				hkVector4 hitNormal;
//				hitNormal.setRotatedDir( StaticMeshes[i]->getRotation(), initResults.m_unnormalizedHitNormal );
#ifdef HAVOK3
				hitNormal = output.m_normal;
#else
				hitNormal = initResults.m_unnormalizedHitNormal;
				hitNormal.normalize3();
#endif
				hitData.Normal = Vector( hitNormal(0), hitNormal(1), hitNormal(2) );
				hitData.HavokEntity = StaticMeshes[i];
				hitData.UserData = mesh;

				return true;
			}
			return false;
		}
	}
	return false;
}

void HavokWorld::ActiveMeshGroupSetActivationSettings( unsigned char group, bool enable, float minimalDelay, float minimalStrength, float minimalMass, float maximalMass, float chance, float TTDBase, float TTDAddition )
{
	AMGActivationParams.Remove( group );
	ActiveMeshGroupActivationParams params;
	params.EnableCollisions = enable;
	params.MinimalDelay = minimalDelay;
	params.MinimalStrength = minimalStrength;
	params.MinimalMass = minimalMass;
	params.MaximalMass = maximalMass;
	params.MinProbability = chance;
	params.TTDBase = TTDBase;
	params.TTDAddition = TTDAddition;
	AMGActivationParams.Insert( group, params );
}

void HavokWorld::PinnedMeshGroupSetCollisionGroup( unsigned char group, int collGroup )
{
	for( int i = 0; i < PinnedActiveMeshes.Size(); ++i )
		if( PinnedActiveMeshes[i].StaticMesh->ActiveGroup == group )
		{
#ifdef HAVOK3
			PinnedActiveMeshes[i].StaticMeshBody->getCollidable()->setCollisionFilterInfo( hkGroupFilter::calcFilterInfo( collGroup ) );
			World->updateCollisionFilterOnEntity( PinnedActiveMeshes[i].StaticMeshBody, HK_UPDATE_FILTER_ON_ENTITY_DISABLE_ENTITY_ENTITY_COLLISIONS_ONLY, HK_UPDATE_COLLECTION_FILTER_IGNORE_SHAPE_COLLECTIONS );
#else
			PinnedActiveMeshes[i].StaticMeshBody->getCollidable().setCollisionFilterInfo( hkGroupFilter::calcFilterInfo( collGroup ) );
			World->updateCollisionFilter( PinnedActiveMeshes[i].StaticMeshBody );
#endif
		}
}

#define HAVOKWORLD_LOAD_SAVE_TOKEN 0x5e50

// Physics load/save - for savegames
bool HavokWorld::LoadWorld( FileObject* f )
{
	if( !f->TestToken( HAVOKWORLD_LOAD_SAVE_TOKEN ) )
	{
		GLog.Print( "Token test failure: HavokWorld::LoadWorld()\n" );
		return false;
	}

	// Load AMGActivationParams
	int count;
	*f >> count;

	AMGActivationParams.Clear();

	int index;
	ActiveMeshGroupActivationParams a;
	for( int i = 0; i < count; ++i )
	{
		*f >> index;
		*f >> (char&)a.EnableCollisions >> a.MaximalMass >> a.MinimalDelay >> a.MinimalMass >> a.MinimalStrength;
		*f >> a.MinProbability >> a.TTDAddition >> a.TTDBase;
		AMGActivationParams.Insert( index, a );
	}

	// Load static mesh visibility state, collision group and noDraw flag
	*f >> count;

	unsigned char collGroup;
	char enabled;
	char meshInWorld;

	Assert( count == StaticMeshes.Size() );
	for( int i = 0; i < count; ++i )
	{
        hkPropertyValue p = StaticMeshes[i]->getProperty( HPEntityPointer );
#ifdef HAVOK3
        Entity* entity = (Entity*)p.getPtr();
#else
        Entity* entity = (Entity*)p.asPtr;
#endif

//		Assert( strstr( entity->Name.AsChar(), "actgrp16_phys_pinned_autodelete_1shape" ) == 0 );

		*f >> collGroup >> entity->Flags >> enabled >> meshInWorld >> entity->TimeToDelete;

		if( !enabled ) World->removeEntity( StaticMeshes[i] );

#ifdef HAVOK3
		int myCollGroup = StaticMeshes[i]->getCollidable()->getCollisionFilterInfo();
		if( myCollGroup != collGroup )
		{
			StaticMeshes[i]->getCollidable()->setCollisionFilterInfo( hkGroupFilter::calcFilterInfo( collGroup ) );
			if( StaticMeshes[i]->getSimulationIsland() )
				World->updateCollisionFilterOnEntity( StaticMeshes[i], HK_UPDATE_FILTER_ON_ENTITY_DISABLE_ENTITY_ENTITY_COLLISIONS_ONLY, HK_UPDATE_COLLECTION_FILTER_IGNORE_SHAPE_COLLECTIONS );
		}
#else
		int myCollGroup = StaticMeshes[i]->getCollidable().getCollisionFilterInfo();
		if( myCollGroup != collGroup )
		{
			StaticMeshes[i]->getCollidable().setCollisionFilterInfo( hkGroupFilter::calcFilterInfo( collGroup ) );
			if( StaticMeshes[i]->getSimulationIsland() )
				World->updateCollisionFilter( StaticMeshes[i] );
		}
#endif

#ifdef	TRANSGAMING
		if( !meshInWorld )
			GEngine->pcfWorld->RemoveEntity( entity );
#else
		if( !meshInWorld )
			GEngine->World->RemoveEntity( entity );
#endif

		entity->Flags |= Entity::EEntDontDeleteOnTimer;
	}

//	GLog.Print( "Load\n" );

	// Load phys meshes
	char ok;
	*f >> count;
	Assert( count == PhysArray.Size() );
	for( int i = 0; i < count; ++i )
	{
		*f >> ok;
		Entity* ent = PhysArray[i]->GetEntity();

//		Assert( strstr( ent->Name.AsChar(), "actgrp16_phys_pinned_autodelete_1shape" ) == 0 );

		if( !ok )	// delete phys mesh
			ent->TimeToDelete = 0;
		else
		{
			Vector pos;
			Quaternion rot;
			*f >> pos >> rot;
			hkVector4 posH( pos.X, pos.Y, pos.Z );

//			GLog.Print( "Position: (%f,%f,%f)\n", posH(0), posH(1), posH(2) );

			PhysArray[i]->mHavokPart->Body->setPosition( posH );
			PhysArray[i]->SetRotQuaternion( rot );
			// pass entity info, unneeded here
			DWORD bt;
			float bs;
			*f >> bt >> bs;
			if( !PhysArray[i]->LoadPO( f ) ) return false;
			*f >> ent->Flags >> ent->TimeToDelete;
		}
	}

	// Load PinnedActiveMeshes
	Assert( PinnedActiveMeshes.Size() == PinnedActiveMeshesInitialSize );
	*f >> count;
	Assert( count == PinnedActiveMeshesInitialSize );
	DynamicArray<int> toDelete;

	for( int i = 0; i < count; ++i )
	{
		char ok;
		*f >> ok;
		if( ok )
		{
			*f >> index;
			PinnedActiveMesh& pam = PinnedActiveMeshes[index];
			bool temp;
			*f >> (char&)temp;
			pam.ActivationDisabled = temp;
			*f >> (char&)temp;
			pam.RecurrentActivationDisabled = temp;
			*f >> pam.TimeToDelete;
		}
		else
			toDelete.Add( i );
	}

	// Remove obsolete PinnedActiveMesh structures
	while( toDelete.Size() )
	{
		index = toDelete[toDelete.Size()-1];
		PinnedActiveMeshes.FastRemove(index);
		toDelete.RemoveLast();
	}

	for( int i = 1; i <50; ++i )
	{
		World->stepDeltaTime( 0.00001f );
#if defined( HAVOK3 ) && defined ( USE_HAVOK_DEBUGGER ) && !defined( TRANSGAMING )
		stepVisualDebugger();
#endif
	}

	return true;
}

void HavokWorld::StaticMeshEntityCheck( void )
{
	for( int i = 0; i < StaticMeshes.Size(); ++i )
	{
		hkPropertyValue p = StaticMeshes[i]->getProperty( HPEntityPointer );
#ifdef HAVOK3
		Entity* entity = (Entity*)p.getPtr();
#else
		Entity* entity = (Entity*)p.asPtr;
#endif
		char meshInWorld = entity->WorldLink?1:0;
		meshInWorld = 0;
	}
}

bool HavokWorld::SaveWorld( FileObject* f )
{
	if( !f->PutToken( HAVOKWORLD_LOAD_SAVE_TOKEN ) ) return false;

	// Save AMGActivationParams
	int count = 0;
	for( int i = 0; i < 256; ++i )
		if( AMGActivationParams.Find( i ) )
			count++;

	*f << count;
	ActiveMeshGroupActivationParams* a;
	for( int i = 0; i < 256; ++i )
		if( ( a = AMGActivationParams.Find( i ) ) != 0 )
		{
			*f << i;
			*f << a->EnableCollisions << a->MaximalMass << a->MinimalDelay << a->MinimalMass << a->MinimalStrength;
			*f << a->MinProbability << a->TTDAddition << a->TTDBase;
		}

	// Save static mesh visibility state, collision group and noDraw flag
	*f << StaticMeshes.Size();

	for( int i = 0; i < StaticMeshes.Size(); ++i )
	{
		char enabled = ( StaticMeshes[i]->getSimulationIsland() != 0 );
#ifdef HAVOK3
		unsigned char collGroup = StaticMeshes[i]->getCollidable()->getCollisionFilterInfo();
#else
		unsigned char collGroup = StaticMeshes[i]->getCollidable().getCollisionFilterInfo();
#endif
		hkPropertyValue p = StaticMeshes[i]->getProperty( HPEntityPointer );
#ifdef HAVOK3
		Entity* entity = (Entity*)p.getPtr();
#else
		Entity* entity = (Entity*)p.asPtr;
#endif
		char meshInWorld = entity->WorldLink?1:0;

		*f << collGroup << entity->Flags << enabled << meshInWorld << entity->TimeToDelete;
	}

//	GLog.Print( "Save\n" );

	// Save phys meshes
	*f << PhysArray.Size();
	for( int i = 0; i < PhysArray.Size(); ++i )
		if( PhysArray[i] )
		{
			*f << (char)1;

			const hkVector4& posH = PhysArray[i]->mHavokPart->Body->getPosition();
			Vector pos( posH(0), posH(1), posH(2) );
//			GLog.Print( "Position: (%f,%f,%f)\n", posH(0), posH(1), posH(2) );
			Quaternion rot = PhysArray[i]->GetRotQuaternion();
			*f << pos << rot;
			PhysArray[i]->SavePO( f );
			Entity* ent = PhysArray[i]->GetEntity();
			*f << ent->Flags << ent->TimeToDelete;
		}
		else
			*f << (char)0;

	// Save PinnedActiveMeshes ( first segregate them )
	int size = PinnedActiveMeshes.Size();
    m_setTag("Physics");
	int* transform = new int[PinnedActiveMeshesInitialSize];
	::memset( transform, 0, PinnedActiveMeshesInitialSize*sizeof(int) );

	for( int i = 0; i < size; ++i )
		transform[PinnedActiveMeshes[i].Index] = i+1;

	*f << PinnedActiveMeshesInitialSize;
	for( int i = 0; i < PinnedActiveMeshesInitialSize; ++i )
		if( transform[i] )
		{
			*f << (char)1;
			PinnedActiveMesh& pam = PinnedActiveMeshes[transform[i]-1];
			*f << pam.Index << pam.ActivationDisabled << pam.RecurrentActivationDisabled << pam.TimeToDelete;
		}
		else
			*f << (char)0;

	delete[] transform;

	return true;
}

void HavokWorld::ReorganizePinnedActiveMeshTable( void )
{
	GLog.Print( "Reorganizing pinned active mesh table...\n" );

#ifdef TRANSGAMING
    static const int    BUFSIZE = 256;
	static char         meshName[BUFSIZE];
#else
	static char meshName[256];
#endif
	int size = PinnedActiveMeshes.Size();

	DynamicArray<String> meshNames;
	DynamicArray<int> meshIndices;

	for( int i = 0; i < size; ++i )
	{
		PinnedActiveMesh& pam = PinnedActiveMeshes[i];
		pam.Index = i;
		if( pam.ActiveMesh )
			continue;


#ifdef TRANSGAMING
		strncpy(meshName, pam.StaticMesh->Name.AsChar(), BUFSIZE);
#else
		strcpy(meshName, pam.StaticMesh->Name.AsChar());
#endif

        char* indPhys = strstr( meshName, "statdest" );
		int meshNameSize = strlen( meshName );
		memcpy( indPhys, "phys", 4*sizeof(char) );
		memmove( meshName, indPhys, meshNameSize-(indPhys-meshName)+1 );
		indPhys = strstr( meshName, "Shape" );
		if( indPhys )
			memmove( indPhys, indPhys+5, strlen(indPhys)-5+1 );
		else
		{
			indPhys = strstr( meshName, "shape" );
			if( indPhys ) memmove( indPhys, indPhys+5, strlen(indPhys)-5+1 );
		}

#define CHECK_STATDEST_CONSISTENCY
#ifdef CHECK_STATDEST_CONSISTENCY
		for( int j = 0; j < meshNames.Size(); ++j )
		{
			if( strstr( meshName, meshNames[j].AsChar() ) )
				GLog.Print( "Statdest consistency fault: Statdest '%s' name part contained in '%s'!!!\n",
					PinnedActiveMeshes[meshIndices[j]].StaticMesh->Name.AsChar(), pam.StaticMesh->Name.AsChar() );
			if( strstr( meshNames[j].AsChar(), meshName ) )
				GLog.Print( "Statdest consistency fault: Statdest '%s' name part contained in '%s'!!!\n",
					pam.StaticMesh->Name.AsChar(), PinnedActiveMeshes[meshIndices[j]].StaticMesh->Name.AsChar() );
		}
#endif
		meshNames.Add( meshName );
		meshIndices.Add( i );
	}

	bool ok = true;
	while( PinnedActiveMeshParts.Size() )
	{
		int i;
		for( i = 0; i < meshNames.Size(); ++i )
			if( strstr( PinnedActiveMeshParts[0]->Name.AsChar(), meshNames[i].AsChar() ) )
			{
				PinnedActiveMeshes[meshIndices[i]].MeshParts.Add( PinnedActiveMeshParts[0] );
				PinnedActiveMeshParts.FastRemove(0);
				break;
			}

		if( i == meshNames.Size() )
		{
			GLog.Print( "Critical error: physdest mesh '%s' has no statdest!!!!\n", PinnedActiveMeshParts[0]->Name.AsChar() );
			PinnedActiveMeshParts.FastRemove(0);
			ok = false;
		}
	}

	if( !ok )
	{
		GLog.Print( "Some physdest meshes had no statdest counterparts!!!\n" );
		GLog.Print( "The engine might crash on this level due to this error!!!\n" );
	}

	PinnedActiveMeshesInitialSize = PinnedActiveMeshes.Size();

	GLog.Print( "Reorganized\n" );
}

void HavokWorld::ForceActivateBodies( void )
{
	for( int i = 0; i < Ragdolls.Size(); ++i )
		Ragdolls[i]->ForceActivate();

//	for( int i = 0; i < PhysicsObjects.Size(); ++i )
//		PhysicsObjects[i]->Activate(true);
}

void HavokWorld::MPPredRemoveAllFromWorld( void )
{
	MPPredictionEntityBatch.clear();
	const hkArray<hkSimulationIsland*>& islands = World->getActiveSimulationIslands();
	for( int i = 0; i < islands.getSize(); ++i )
	{
		const hkArray<hkEntity*>& entities = islands[i]->getEntities();
		for( int j = 0; j < entities.getSize(); ++j )
		{
			hkRigidBody* rb = static_cast<hkRigidBody*>(entities[j]);

			// Don't mess with ragdolls
			if( rb->hasProperty( HPRagdollLimb ) ) continue;

			MPPredictionEntityBatch.pushBack(rb);
		}
	}

	for( int i = 0; i < MPPredictionEntityBatch.getSize(); ++i )
		World->removeEntity( MPPredictionEntityBatch[i] );
}

void HavokWorld::MPPredRemoveFromBatch( hkRigidBody* rb, bool addToWorld )
{
	for( int i = 0; i < MPPredictionEntityBatch.getSize(); ++i )
		if( MPPredictionEntityBatch[i] == rb )
		{
			MPPredictionEntityBatch.removeAt(i);
			if( addToWorld && !rb->getSimulationIsland() )
			{
				World->addEntity( rb );
				hkPropertyValue p = rb->getProperty( HPEntityPointer );
#ifdef HAVOK3
				Entity* e = (Entity*)p.getPtr();
#else
				Entity* e = (Entity*)p.asPtr;
#endif
				if( e )
				{
					PhysicsObject* po = e->PO;
					if( po && ( rb == po->mHavokPart->Body ) )
						po->mHavokPart->ReconnectActionsToWorld();
				}
			}
			return;
		}
}

void HavokWorld::MPPredAddRestToWorld( void )
{
	for( int i = 0; i < MPPredictionEntityBatch.getSize(); ++i )
	{
		if( MPPredictionEntityBatch[i]->getSimulationIsland() ) continue;	// might already have been added

		World->addEntity( MPPredictionEntityBatch[i] );
		hkPropertyValue p = MPPredictionEntityBatch[i]->getProperty( HPEntityPointer );
#ifdef HAVOK3
		Entity* e = (Entity*)p.getPtr();
#else
		Entity* e = (Entity*)p.asPtr;
#endif
		if( e )
		{
			PhysicsObject* po = e->PO;
			if( po && ( MPPredictionEntityBatch[i] == po->mHavokPart->Body ) )
				po->mHavokPart->ReconnectActionsToWorld();
		}
	}
	MPPredictionEntityBatch.clear();
}

void HavokWorldSavedState::Restore( HavokWorld* hw )
{
	const hkArray<hkSimulationIsland*>& islands = hw->World->getActiveSimulationIslands();
	for( int i = 0; i < islands.getSize(); ++i )
	{
		const hkArray<hkEntity*>& entities = islands[i]->getEntities();
		for( int j = 0; j < entities.getSize(); ++j )
		{
			hkRigidBody* rb = static_cast<hkRigidBody*>(entities[j]);
			int* index = EntitiesFind.Find( (void*)rb );
			if( index )
			{
				HavokEntitySavedState& state = Entities[*index];
				rb->setTransform( state.Transform );
				rb->setLinearVelocity( state.Velocity );
				rb->setAngularVelocity( state.AngularVelocity );
			}
		}
	}
}

void HavokWorldSavedState::SaveRagdolls( HavokWorld* hw )
{
	EntitiesFind.Clear();

	const hkArray<hkSimulationIsland*>& islands = hw->World->getActiveSimulationIslands();
	int index = 0;
	for( int i = 0; i < islands.getSize(); ++i )
	{
		const hkArray<hkEntity*>& entities = islands[i]->getEntities();
		for( int j = 0; j < entities.getSize(); ++j )
		{
			hkRigidBody* rb = static_cast<hkRigidBody*>(entities[j]);

			if( !rb->hasProperty( HPRagdollLimb ) )
				continue;

			// Insert its ptr into hash
			HavokEntitySavedState& state = Entities[index];
			state.Velocity = rb->getLinearVelocity();
			state.AngularVelocity = rb->getAngularVelocity();

			if( ( state.Velocity.lengthSquared3() < (0.01f*0.01f) ) &&
				( state.AngularVelocity.lengthSquared3() < (0.1f*0.1f) ) )
				continue;

			state.Transform = rb->getTransform();
			EntitiesFind.Insert( (void*)rb, index );
			index++;
			Assert( index < 1000 );
		}
	}
}

void HavokWorldSavedState::SaveObj( hkRigidBody* rb, hkTransform* tr )
{
	int* index = EntitiesFind.Find( (void*)rb );
	if( index )
		Entities[*index].Transform = *tr;
}


#ifdef HAVOK3
// ================================================
// Blair - I'm reinstating this function for now...
void HK_CALL HavokWorld::scaleMassProperties( hkReal scale, hkMassProperties& massProperties )
{
	massProperties.m_mass = massProperties.m_mass * scale;
	massProperties.m_inertiaTensor(0,0) *= scale;
	massProperties.m_inertiaTensor(0,1) *= scale;
	massProperties.m_inertiaTensor(0,2) *= scale;
	massProperties.m_inertiaTensor(1,0) *= scale;
	massProperties.m_inertiaTensor(1,1) *= scale;
	massProperties.m_inertiaTensor(1,2) *= scale;
	massProperties.m_inertiaTensor(2,0) *= scale;
	massProperties.m_inertiaTensor(2,1) *= scale;
	massProperties.m_inertiaTensor(2,2) *= scale;
}
#endif

#if defined( HAVOK3 ) && defined ( USE_HAVOK_DEBUGGER ) && !defined( TRANSGAMING )
hkVisualDebugger* HavokWorld::setupVisualDebugger( hkPhysicsContext* physicsWorld )
{
	hkArray<hkProcessContext*> contexts;
	contexts.pushBack(physicsWorld);

    m_setTag("Physics");
	hkVisualDebugger* visualDebugger = new hkVisualDebugger(contexts); 
	visualDebugger->serve(); 
	
	// *** FOR HAVOK PROFILING INFO!!!!! ***
	hkMonitorStream::resize( 500 * 1024 ); // BLAIR - Deactivate this for RELEASE BUILDS
	hkMonitorStream::reset();

	// It's hot off the press! Deliver our creation!
	return visualDebugger;
}

void HavokWorld::stepVisualDebugger()
{
	// Step the debugger forward...
	if ( !mVisualDebugger )
		return;

	mVisualDebugger->step();

	// Reset the profiling info...
	hkMonitorStream::reset();
}


void HavokWorld::writeSimpleMemoryStatistics()
{
    hkSimpleStatisticsCollector collector;
    collector.beginSnapshot( collector.MEMORY_ALL );
    collector.addChildObject("All", collector.MEMORY_APPLICATION, this->World );
    collector.commitSnapshot(  );
    collector.beginSnapshot( collector.MEMORY_SHARED );
    collector.addChildObject("SharedOnly", collector.MEMORY_APPLICATION,  this->World );
    collector.commitSnapshot(  );
    collector.beginSnapshot( collector.MEMORY_INSTANCE );
    collector.addChildObject("InstanceOnly", collector.MEMORY_APPLICATION,  this->World );
    collector.commitSnapshot(  );
    collector.beginSnapshot( collector.MEMORY_ENGINE );
    collector.addChildObject("EngineOnly", collector.MEMORY_APPLICATION,  this->World );
    collector.commitSnapshot(  );
    collector.beginSnapshot( collector.MEMORY_RUNTIME );
    collector.addChildObject("RuntimeOnly", collector.MEMORY_APPLICATION,  this->World );
    collector.commitSnapshot(  );
    hkOstream ostr("havokMemoryDump.txt");
    ostr.printf("\n\nDemoName: Morphine\n");
    ostr.printf("============================\n\n");
    collector.writeStatistics( ostr, hkMonitorStreamAnalyzer::REPORT_PERFRAME_TIME | hkMonitorStreamAnalyzer::REPORT_PERFRAME_SUMMARY );
    hkMemory::getInstance().printStatistics( &ostr );
}

void HavokWorld::SaveSnapshot()
{
	hkOstream outfile("snapshot.xml");
	hkHavokSnapshot::save(World, outfile.getStreamWriter(), false);
}

#endif
