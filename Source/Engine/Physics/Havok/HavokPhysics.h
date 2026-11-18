
// #pragma once
#ifndef _HAVOKPHYSICS_
#define _HAVOKPHYSICS_


#include "Engine.h"

//#define HAVOK212

#ifndef FINAL_BUILD
#define HAVOK_POSITION_BUG_TRACKING
#endif


class Entity;
#include "Core/DynArray.h"
#include "Core/Math/LeMath.h"
#include "Core/HashMap.h"

// havok stuff
#pragma warning (push,3)
#pragma warning (disable : 4267) // conversion from 'size_t' to 'int', possible loss of data
#pragma warning (disable : 4018) // '<' : signed/unsigned mismatch

#include "Core/nommgr.h"
#if defined(TRANSGAMING) && defined(DEBUG)
/*# define DISABLE_STRING_CHECK
# include "Core/noStringCheck.h"//*/
#endif

// Havok2 includes
#ifdef HAVOK3
#include <hkbase/hkBase.h>
#include <hkbase/basesystem/hkBaseSystem.h>
//#include <hkbase/memory/hkThreadMemory.h>
#ifdef TRANSGAMING_WINDOWS
#include <hkbase/memory/impl/hkMallocMemory.h>
#else
#include <hkbase/memory/impl/hkPoolMemory.h>
#endif
//#include <hkbase/monitor/hkMonitorStream.h>
//#include <hkbase/thread/hkCriticalSection.h>
#include <hkbase/stream/hkOArchive.h>
#include <hkbase/stream/hkIArchive.h>
#include "Physics/Havok/TNGPhysFSStream/TNGPhysFSStreambufFactory.h"
#include <hkdynamics/hkDynamics.h>
#include <hkbase/debugutil/hkSimpleStatisticsCollector.h>
#else
#include <hkbase/hkbase.h>
#include <hkdynamics2/hkDynamics2.h>
#include <hkutilities2/hkHavok2Common.h>
#endif

#ifdef HAVOK3
#include <hkdynamics/world/hkSimulationIsland.h>
#include <hkcollide/hkCollide.h>
#include <hkcollide/shape/multisphere/hkMultiSphereShape.h>
#include <hkcollide/shape/multiray/hkMultiRayShape.h>
#include <hkcollide/shape/storagemesh/hkStorageMeshShape.h>
#include <hkcollide/shape/hkRayHitCollector.h>
#include <hkcollide/shape/sphere/hkSphereShape.h>
#include <hkcollide/shape/convexvertices/hkConvexVerticesShape.h>
#else
// Simulation Islands
#include <hkdynamics2/world/hkSimulationIsland.h>
#include <hkcollide2/hkCollide2.h>
#include <hkcollide2/shape/hkMultiSphereShape.h>
#include <hkcollide2/shape/hkMultiRayShape.h>
#include <hkcollide2/shape/hkStorageMeshShape.h>
#endif

// Anti-gravity action
#ifdef HAVOK3
#include <hkdynamics/action/hkUnaryAction.h>
#else
#include <hkdynamics2/action/hkUnaryAction.h>
#endif

// Collision listeners
#ifdef HAVOK3
#include <hkdynamics/collide/hkCollisionListener.h>
#else
#include <hkdynamics2/collide/hkCollisionListener.h>
#endif

// Phantom shapes
#ifdef HAVOK3
// BLAIR - changed shape/hkPlantomShape.h ---> shape/phantomcallback/hkPhantomCallbackShape.h
#include <hkcollide/shape/hkShape.h>
#include <hkcollide/shape/phantomcallback/hkPhantomCallbackShape.h>
#include <hkcollide/shape/bv/hkBvShape.h>

// Other Shapes
#include <hkcollide/shape/list/hkListShape.h>

// Collisions
// BLAIR - changed dispatch/hkCollisionFilterSet.h ---> ?? :(
// ###### MISSING HEADER #######
// #include <hkcollide/dispatch/hkCollisionFilterSet.h>

// Raycast callback
// BLAIR - changed hkcollide/broadphase/hkBroadPhase.h ---> hkinternal/collide/broadphase/hkBroadPhase.h
#include <hkinternal/collide/broadphase/hkBroadPhase.h>
#include <hkcollide/dispatch/broadphase/hkTypedBroadPhaseHandle.h>

// BLAIR - changed hkcollide/shape/hkRayCastCallback.h ---> ?? :(
// ###### MISSING HEADER #######
//#include <hkcollide/shape/hkRayCastCallback.h>

// StepCheck phantom
#include <hkdynamics/phantom/hkAabbPhantom.h>

// Character includes
#include <hkutilities/charactercontrol/characterproxy/hkCharacterProxy.h>
#include <hkutilities/charactercontrol/characterproxy/hkCharacterProxyCinfo.h>
#include <hkutilities/charactercontrol/statemachine/hkCharacterState.h>
#include <hkutilities/charactercontrol/statemachine/hkCharacterStateManager.h>
#include <hkutilities/charactercontrol/statemachine/hkCharacterContext.h>

// ADDED BY TNG
#include<hkdynamics/phantom/hkShapePhantom.h>
#else // HAVOK3
#include <hkcollide2/shape/hkPhantomShape.h>
#include <hkcollide2/shape/hkBvShape.h>

// Collisions
#include <hkcollide2/dispatch/hkCollisionFilterSet.h>

// Raycast callback
#include <hkcollide2/broadphase/hkBroadPhase.h>
#include <hkcollide2/shape/hkRayCastCallback.h>

// StepCheck phantom
#include <hkdynamics2/phantom/hkAabbPhantom.h>
#endif // HAVOK3

// Polygons test in regions
#ifdef HAVOK3
// BLAIR - changed hkgeometry2/hkGeometry.h ---> hkmath/basetypes/hkGeometry.h
#include <hkmath/basetypes/hkGeometry.h>

// ** BLAIR - *** NEW HAVOK3 INCLUDES *****
// BLAIR - just added this...because no hkGeometryChecker, hkGeometryFilterer
#include <hkinternal/preprocess/convexhull/hkGeometryUtility.h>
// BLAIR - added this because we reference hkGroupFilter
#include <hkcollide/filter/group/hkGroupFilter.h>
#include <hkdynamics/entity/hkRigidBodyCinfo.h>
#include <hkutilities/inertia/hkInertiaTensorComputer.h>
#include <hkinternal/dynamics/constraint/bilateral/ragdoll/hkRagdollConstraintData.h>
#include <hkcollide/shape/simplemesh/hkSimpleMeshShape.h>
#include <hkmath/linear/hkVector4.h>
//#include <hkdynamics/constraint/hkConstraintInstance.h>
#include <hkdynamics/constraint/bilateral/stiffspring/hkStiffSpringConstraintData.h>
#include <hkdynamics/constraint/bilateral/prismatic/hkPrismaticConstraintData.h>
#include <hkdynamics/constraint/bilateral/wheel/hkWheelConstraintData.h>
// ************** NEW HAVOK3 INCLUDES *****
#else
#include <hkgeometry2/hkGeometry.h>
#include <hkgeometryutil2/hkGeometryChecker.h>
#include <hkgeometryutil2/hkGeometryFilterer.h>
#endif

#ifdef HAVOK3
// MOPPs
// BLAIR - changed hkcollide2/shape/hkMoppBvTreeShape.h ---> hkcollide/shape/mopp/hkMoppBvTreeShape.h
#include <hkcollide/shape/mopp/hkMoppBvTreeShape.h>
// BLAIR - changed hkcollide/algorithms/mopp/utility/hkMoppUtility.h ---> hkcollide/shape/mopp/hkMoppUtility.h
#include <hkcollide/shape/mopp/hkMoppUtility.h>
#include <hkcollide/filter/null/hkNullCollisionFilter.h>

// BLAIR - changed hkutilities2/collide/hkMoppCodeStreamer.h ---> hkutilities/collide/hkMoppCodeStreamer.h
#include <hkutilities/collide/hkMoppCodeStreamer.h>
#include <hkbase/stream/hkStreambufFactory.h>

// Visual debugger
// BLAIR - changed hkUtilities2/visualdebugger/hkVisualDebugger.h ---> hkvisualize/hkVisualDebugger.h
#include <hkvisualize/hkVisualDebugger.h>

// Constraint info
#include <hkdynamics/constraint/hkConstraintData.h>
#include <hkdynamics/constraint/breakable/hkBreakableConstraintData.h>
#include <hkdynamics/constraint/bilateral/ballandsocket/hkBallAndSocketConstraintData.h>
#include <hkdynamics/constraint/bilateral/hinge/hkHingeConstraintData.h>
#include <hkdynamics/constraint/bilateral/limitedhinge/hkLimitedHingeConstraintData.h>

// Demo info
#include <hkutilities/serialize/hkHavokSnapshot.h>

#else
// Exporter
#include <hkexport2/hkexport.h>
#include <hkexport2/exportclass.h>
#include <hkexport2/exportworld.h>
#include <hkexport2/exportconstraint.h>
#include <hkexport2/exporthingeconstraint.h>
#include <hkexport2/exportlimitedptopconstraint.h>
#include <hkexport2/exportptopconstraint.h>
#include <hkbase/stream/hkIstream.h>

// MOPPs
#include <hkcollide2/shape/hkMoppBvTreeShape.h>
#include <hkcollide2/algorithms/mopp/utility/hkMoppUtility.h>
#include <hkbase/stream/hkOArchive.h>
#include <hkbase/stream/hkIArchive.h>
#include <hkutilities2/collide/hkMoppCodeStreamer.h>
#include <hkbase/stream/hkStreambufFactory.h>

// Visual debugger
#include <hkutilities2/visualdebugger/hkVisualDebugger.h>

// Exporter
#include <hkexport2/hkexport.h>
#include <hkexport2/exportclass.h>
#include <hkexport2/exportworld.h>
#include <hkexport2/exportconstraint.h>
#include <hkexport2/exporthingeconstraint.h>
#include <hkexport2/exportlimitedptopconstraint.h>
#include <hkexport2/exportptopconstraint.h>
#include <hkbase/stream/hkIstream.h>

#endif

#ifdef HAVOK3
// BLAIR - changed hkdynamics2/constraint/constraintkit/hkGenericConstraint.h ---> hkdynamics/constraint/constraintkit/hkGenericConstraintData.h
// - NOTE - hkGenericConstraint has changed to hkGenericConstraintData
#include <hkdynamics/constraint/constraintkit/hkGenericConstraintData.h>
#else
#include <hkdynamics2/constraint/constraintkit/hkGenericConstraint.h>
#endif

#ifdef HAVOK3
// Custom collision agents
// BLAIR - changed hkcollide/agent/hkBoxBoxAgent.h ---> ?? :(
// ADDING :	hkGskConvexConvexAgent.h
//			hkConvexListAgent.h
//#include <hkcollide/agent/hkConvexConvexAgent.h>
//#include <hkcollide/agent/gjk/hkGskConvexConvexAgent.h>
#include <hkcollide/dispatch/hkAgentRegisterUtil.h>
#include <hkcollide/agent/convexlist/hkConvexListAgent.h>
#include <hkcollide/agent/hkCollisionAgent.h>
// BLAIR - changed hkcollide/agent/hkBoxBoxAgent.h ---> hkcollide/agent/boxbox/hkBoxBoxAgent.h
#include <hkcollide/agent/boxbox/hkBoxBoxAgent.h>
// BLAIR - changed hkcollide/agent/hkBvShapeAgent.h ---> hkcollide/agent/bv/hkBvAgent.h
#include <hkcollide/agent/bv/hkBvAgent.h>
// BLAIR - changed hkcollide/agent/hkBvTreeShapeAgent.h ---> hkcollide/agent/bvtree/hkBvTreeAgent.h
#include <hkcollide/agent/bvtree/hkBvTreeAgent.h>
#include <hkcollide/agent/bvtreestream/hkBvTreeStreamAgent.h>
#include <hkcollide/agent/convexlist/hkConvexListAgent.h>
#include <hkcollide/agent/bvtree/hkMoppAgent.h>
#include <hkcollide/agent/bvtreestream/hkMoppBvTreeStreamAgent.h>
// BLAIR - changed hkcollide/agent/hkPhantomAgent.h ---> hkcollide/agent/phantom/hkPhantomAgent.h
#include <hkcollide/agent/phantom/hkPhantomAgent.h>
// BLAIR - changed hkcollide/agent/hkSphereSphereAgent.h ---> hkcollide/agent/spheresphere/hkSphereSphereAgent.h
#include <hkcollide/agent/spheresphere/hkSphereSphereAgent.h>
// BLAIR - changed hkcollide/agent/hkSphereTriangleAgent.h ---> hkcollide/agent/spheretriangle/hkSphereTriangleAgent.h
#include <hkcollide/agent/spheretriangle/hkSphereTriangleAgent.h>
// BLAIR - changed hkcollide/agent/hkSphereBoxAgent.h ---> hkcollide/agent/spherebox/hkSphereBoxAgent.h
#include <hkcollide/agent/spherebox/hkSphereBoxAgent.h>
// BLAIR - changed hkcollide/agent/hkMultiSphereTriangleAgent.h ---> hkcollide/agent/multispheretriangle/hkMultiSphereTriangleAgent.h
#include <hkcollide/agent/multispheretriangle/hkMultiSphereTriangleAgent.h>
// BLAIR - changed hkcollide/agent/hkMultiRayConvexAgent.h ---> hkcollide/agent/multirayconvexagent/hkMultiRayConvexAgent.h
#include <hkcollide/agent/multirayconvex/hkMultiRayConvexAgent.h>
// BLAIR - changed hkcollide/agent/hkTransformAgent.h ---> hkcollide/agent/transform/hkTransformAgent.h
#include <hkcollide/agent/transform/hkTransformAgent.h>
// BLAIR - changed hkcollide/agent/hkListagent.h ---> hkcollide/agent/list/hkListagent.h
#include <hkcollide/agent/list/hkListAgent.h>
// BLAIR - changed hkcollide/agent/hkShapeCollectionAgent.h ---> hkcollide/agent/shapecollection/hkShapeCollectionAgent.h
#include <hkcollide/agent/shapecollection/hkShapeCollectionAgent.h>

// BLAIR - changed hkcollide/agent/hkAabbCastConvexAgent.h ---> :(
// ###### MISSING HEADER #######
//#include <hkcollide/agent/hkAabbCastConvexAgent.h>
// BLAIR - changed hkcollide/agent/hkAabbCastAabbCastAgent.h ---> :(
// ###### MISSING HEADER #######
//#include <hkcollide/agent/hkAabbCastAabbCastAgent.h>
// BLAIR - changed hkcollide/agent/hkConvexWelderConvexAgent.h ---> ?? :(
// ###### MISSING HEADER #######
//#include <hkcollide/agent/hkConvexWelderConvexAgent.h>
// BLAIR - changed hkcollide/agent/hkConvexWelderSphereAgent.h ---> ?? :(
// ###### MISSING HEADER #######
//#include <hkcollide/agent/hkConvexWelderSphereAgent.h>
// BLAIR - changed hkutilities2/simulation/hkConvexSweepConvexAgent.h ---> ?? :(
// ###### MISSING HEADER #######
//#include <hkutilities2/simulation/hkConvexSweepConvexAgent.h>
// BLAIR - changed hkcollide/agent/hkNullAgent.h ---> hkcollide/agent/null/hkNullAgent.h
#include <hkcollide/agent/null/hkNullAgent.h>

// BLAIR - changed hkcollide/algorithms/util/hkCollideTriangleUtil.h ---> hkinternal/collide/util/hkCollideTriangleUtil.h
#include <hkinternal/collide/util/hkCollideTriangleUtil.h>

// Raycast Input/Output
#include <hkcollide/shape/hkShapeRayCastInput.h>
#include <hkcollide/shape/hkShapeRayCastOutput.h>
#include <hkcollide/castutil/hkWorldRayCastInput.h>
#include <hkcollide/castutil/hkWorldRayCastOutput.h>
#include <hkdynamics/world/broadphaseborder/hkBroadPhaseBorder.h>
#include <hkinternal/collide/broadphase/hkBroadPhaseCastCollector.h>

// Phantoms
#include <hkdynamics/phantom/hkSimpleShapePhantom.h>

// Capsule Shape
#include <hkcollide/shape/capsule/hkCapsuleShape.h>

// BLAIR - hkcollide/contact/hkClosestPointsDetails.h ---> ?? :(
// ###### MISSING HEADER #######
//#include <hkcollide/contact/hkClosestPointsDetails.h>

// BLAIR - hkcollide/contact/hkSimpleClosestPointsDetails.h ---> ?? :(
// ###### MISSING HEADER #######
//#include <hkcollide/contact/hkSimpleClosestPointsDetails.h>

// Keyframing utitities
// BLAIR - changed hkutilities2/keyframe/hkKeyFrameUtility.h ---> hkutilities/keyframe/hkKeyFrameUtility.h
#include <hkutilities/keyframe/hkKeyFrameUtility.h>

// Havok world border
// BLAIR - changed hkutilities2/collide/hkBroadPhaseBorder.h ---> hkdynamics/world/broadphaseborder/hkBroadPhaseBorder.h 
#include <hkdynamics/world/broadphaseborder/hkBroadPhaseBorder.h>

// BLAIR - struct for Broadphase queries...
#include <hkinternal/collide/broadphase/hkBroadPhaseHandlePair.h>

// Actions
// BLAIR - changed hkutilities2/actions/dashpot/hkDashpotAction.h ---> hkutilities/actions/dashpot/hkDashpotAction.h
#include <hkutilities/actions/dashpot/hkDashpotAction.h>
// BLAIR - changed hkutilities2/actions/angulardashpot/hkAngularDashpotAction.h ---> hkutilities/actions/angulardashpot/hkAngularDashpotAction.h
#include <hkutilities/actions/angulardashpot/hkAngularDashpotAction.h>
// BLAIR - changed hkutilities2/actions/spring/hkSpringAction.h ---> hkutilities/actions/spring/hkSpringAction.h
#include <hkutilities/actions/spring/hkSpringAction.h>

// Planes from vertices
// BLAIR - changed hkgeometryutil2/hkGeometryUtil.h ---> hkinternal/preprocess/convexhull/hkGeometryUtility.h
#include <hkinternal/preprocess/convexhull/hkGeometryUtility.h>

#include <hkutilities/visualdebugger/hkPhysicsContext.h>

#else
// Custom collision agents
#include <hkcollide2/agent/hkConvexConvexAgent.h>
#include <hkcollide2/agent/hkBoxBoxAgent.h>
#include <hkcollide2/agent/hkBvShapeAgent.h>
#include <hkcollide2/agent/hkBvTreeShapeAgent.h>
#include <hkcollide2/agent/hkPhantomAgent.h>
#include <hkcollide2/agent/hkSphereSphereAgent.h>
#include <hkcollide2/agent/hkSphereTriangleAgent.h>
#include <hkcollide2/agent/hkSphereBoxAgent.h>
#include <hkcollide2/agent/hkMultiSphereTriangleAgent.h>
#include <hkcollide2/agent/hkMultiRayConvexAgent.h>

#include <hkcollide2/agent/hkTransformAgent.h>
#include <hkcollide2/agent/hkListagent.h>
#include <hkcollide2/agent/hkShapeCollectionAgent.h>
#include <hkcollide2/agent/hkAabbCastConvexAgent.h>
#include <hkcollide2/agent/hkAabbCastAabbCastAgent.h>
#include <hkcollide2/agent/hkConvexWelderConvexAgent.h>
#include <hkcollide2/agent/hkConvexWelderSphereAgent.h>
#include <hkutilities2/simulation/hkConvexSweepConvexAgent.h>
#include <hkcollide2/agent/hkNullAgent.h>

#include <hkcollide2/algorithms/util/hkCollideTriangleUtil.h>
#include <hkcollide2/contact/hkClosestPointsDetails.h>
#include <hkcollide2/contact/hkSimpleClosestPointsDetails.h>

// Keyframing utitities
#include <hkutilities2/keyframe/hkKeyFrameUtility.h>

// Havok world border
#include <hkutilities2/collide/hkBroadPhaseBorder.h>

// Actions
#include <hkutilities2/actions/dashpot/hkDashpotAction.h>
#include <hkutilities2/actions/angulardashpot/hkAngularDashpotAction.h>
#include <hkutilities2/actions/spring/hkSpringAction.h>

// Planes from vertices
#include <hkgeometryutil2/hkGeometryUtil.h>
#endif // HAVOK3

// Activation listener
#ifdef HAVOK3
#include <hkdynamics/entity/hkEntityActivationListener.h>
#else
#include <hkdynamics2/entity/hkEntityActivationListener.h>
#endif

#ifndef HAVOK3
// Convex sweep shape
#include <hkutilities2/simulation/hkConvexSweepShape.h>
#include <hkutilities2/simulation/hkClipVelocityListener.h>
#include <hkcollide2/shape/hkSweepBvTreeShape.h>
#endif

// Breakable listener module
#ifdef HAVOK3
#include <hkdynamics/constraint/breakable/hkBreakableListener.h>
#else
#include <hkdynamics2/constraint/breakable/hkBreakableListener.h>
#endif

// Deactivator
#ifdef HAVOK3
#include <hkdynamics/entity/hkSpatialRigidBodyDeactivator.h>
#else
#include <hkdynamics2/entity/hkSpatialRigidBodyDeactivator.h>
#endif


#if defined(TRANSGAMING) && defined(DEBUG)
/*# define ENABLE_STRING_CHECK
# define ENABLE_CHECK_ALL
# include "Core/noStringCheck.h"//*/
#endif
#include "Core/mmgr.h"
/*
// Vehicle module
#include <hkvehicle2/hkVehicle.h>
#include <hkvehicle2/hkDefaultVehicle.h>
*/
#pragma warning (pop)

//#define delta -1.0
#define delta 0

enum HavokProperties
{
	HPEntityPointer = 1,
	HPRayCastDontCollide = 2,
	HPGlass = 3,
	HPDontSaveThis = 4,
	HPSynchronized = 5,
	HPRagdollLimb = 6,
};

class havokAntiGravityAction;
class havokSpeedDampingAction;
class havokCollisionListener;
class havokStepCheckPhantom;

// =============================================================================

class havokAction: public hkUnaryAction
{
public:
	havokAction( hkRigidBody* body );
	virtual int getType( void ) = 0;
	virtual hkAction* clone( const hkArray<hkEntity*>& newEntities, const hkArray<hkPhantom*>& newPhantoms ) const;
};

// Additional Havok actions

/// This action applies a force to a body opposite to a specified "gravity" force
/// and will thus cancel out the current gravity in the world for this body.
/// The force is modulated over time, so sometimes is stronger then gravity, sometimes is less strong.
class havokAntiGravityAction: public havokAction
{
public:
	havokAntiGravityAction( hkRigidBody* body, const hkVector4& gravity );
	hkVector4& getGravity( void );
	void setGravity( hkVector4& newGrav );
	int getType( void );
#ifdef HAVOK3
	//virtual hkAction* clone( const hkArray<hkEntity*>& newEntities, const hkArray<hkPhantom*>& newPhantoms ) const;
#endif

private:
	virtual void applyAction( const hkStepInfo& stepInfo );

	hkVector4	m_gravity;
};

// This action applies a stopping force to a body, when its velocity exceeds normal velocity. The stopping force is
// directly proportional to the body velocity.

class havokSpeedDampingAction: public havokAction
{
public:
	havokSpeedDampingAction( hkRigidBody* body, float maxSpeed, float dampingSpeed, float dampingStrength );
	int getType( void );
	void getInfo( float& maxSpeed, float& dampingSpeed, float& dampingStrength );
	void setInfo( float maxSpeed, float dampingSpeed, float dampingStrength );
#ifdef HAVOK3
	//virtual hkAction* clone( const hkArray<hkEntity*>& newEntities, const hkArray<hkPhantom*>& newPhantoms ) const;
#endif

private:
	virtual void applyAction( const hkStepInfo& stepInfo );

	float m_maxSpeed;
	float m_dampingSpeed;
	float m_dampingStrength;
};

// This action gives to body one degree of freedom - movement along one axis

class havokMaintainLinearMovementAction : public havokAction
{
public:
	havokMaintainLinearMovementAction( hkRigidBody* body, hkVector4& axis, bool soft = false );
	void setAxis( hkVector4& axis );
	void getAxis( hkVector4& axis );
	void setInfo( hkVector4& axis, hkVector4& position, bool soft = false );
	void getInfo( hkVector4& axis, hkVector4& position, bool& soft );
	int getType( void );
#ifdef HAVOK3
	//virtual hkAction* clone( const hkArray<hkEntity*>& newEntities, const hkArray<hkPhantom*>& newPhantoms ) const;
#endif

private:
	virtual void applyAction( const hkStepInfo& stepInfo );

	hkVector4		m_axis;
	hkVector4		m_position;
	bool			m_soft;
};

// This action tries to maintain given position on given body

class havokMaintainPositionAction : public havokAction
{
public:
	havokMaintainPositionAction( hkRigidBody* body, hkVector4& desiredPosition, float strength );
	void setPosition( hkVector4& position );
	void getPosition( hkVector4& position );
	void setInfo( hkVector4& position, float& strength );
	void getInfo( hkVector4& position, float& strength );
	int getType( void );
#ifdef HAVOK3
	//virtual hkAction* clone( const hkArray<hkEntity*>& newEntities, const hkArray<hkPhantom*>& newPhantoms ) const;
#endif

private:
	virtual void applyAction( const hkStepInfo& stepInfo );

	hkVector4		m_position;
	float			m_strength;
};

// This action tries to maintain given velocity on given body

class havokMaintainVelocityAction : public havokAction
{
public:
	havokMaintainVelocityAction( hkRigidBody* body, hkVector4& desiredVelocity, float strength );
	void setVelocity( hkVector4& velocity );
	void getVelocity( hkVector4& velocity );
	void setInfo( hkVector4& velocity, float& strength );
	void getInfo( hkVector4& velocity, float& strength );
	int getType( void );
#ifdef HAVOK3
	//virtual hkAction* clone( const hkArray<hkEntity*>& newEntities, const hkArray<hkPhantom*>& newPhantoms ) const;
#endif

private:
	virtual void applyAction( const hkStepInfo& stepInfo );

	hkVector4		m_velocity;
	float			m_strength;
};

// This action tries to maintain given angular velocity on given body

class havokMaintainAngularVelocityAction : public havokAction
{
public:
	havokMaintainAngularVelocityAction( hkRigidBody* body, hkVector4& desiredAngularVelocity, float strength );
	void setAngularVelocity( hkVector4& angularVelocity );
	void getAngularVelocity( hkVector4& angularVelocity );
	void setInfo( hkVector4& angVelocity, float& strength );
	void getInfo( hkVector4& angVelocity, float& strength );
	int getType( void );
#ifdef HAVOK3
	//virtual hkAction* clone( const hkArray<hkEntity*>& newEntities, const hkArray<hkPhantom*>& newPhantoms ) const;
#endif

private:
	virtual void applyAction( const hkStepInfo& stepInfo );

	hkVector4		m_angularVelocity;
	float			m_strength;
};

// This action is solely for transporters; it makes player, items and monsters act like they were on a moving floor

class havokTransporterAction : public havokAction
{
public:
	havokTransporterAction( hkRigidBody* body, hkVector4& appliedVelocity, float strength );
	void setInfo( hkVector4& velocity, float& strength );
	void getInfo( hkVector4& velocity, float& strength );
	int getType( void );
#ifdef HAVOK3
	//virtual hkAction* clone( const hkArray<hkEntity*>& newEntities, const hkArray<hkPhantom*>& newPhantoms ) const;
#endif

private:
	virtual void applyAction( const hkStepInfo& stepInfo );

	hkVector4	m_velocity;
	float		m_strength;
};

// This listener makes physics objects disappear some time after when they've been activated

class havokDisappearTrigger: public hkEntityActivationListener
{
	virtual void entityDeactivatedCallback( hkEntity* entity );
	virtual void entityActivatedCallback( hkEntity* entity );
};

// Event callback classes

class havokCollisionListener : public hkCollisionListener, public hkEntityListener
{
public:
	havokCollisionListener( hkRigidBody* rigidBody, float minimalDelay, float minimalStrength, float minimalMass );
	~havokCollisionListener();

	void contactPointAddedCallback(	hkContactPointAddedEvent& event );
#ifdef HAVOK3
	void contactPointConfirmedCallback( hkContactPointConfirmedEvent& event);
#endif
	void contactPointRemovedCallback( hkContactPointRemovedEvent& event );
	void contactProcessCallback( hkContactProcessEvent& event );
	void entityDeletedCallback( hkEntity* entity );
	void entityRemovedCallback( hkEntity* entity );
	void entityAddedCallback( hkEntity* entity );

	void getInfo( float& minimalDelay, float& minimalStrength, float& minimalMass );
#ifdef TRANSGAMING
	void getInternalInfo( float& lastTimeCalled ) { lastTimeCalled = mLastTimeCalled; };
	void setInternalInfo( float lastTimeCalled ) { mLastTimeCalled = lastTimeCalled; };
#else
	void getInternalInfo( double& lastTimeCalled ) { lastTimeCalled = mLastTimeCalled; };
	void setInternalInfo( double lastTimeCalled ) { mLastTimeCalled = lastTimeCalled; };
#endif

private:
	hkRigidBody* mRigidBody;
#ifdef TRANSGAMING
	float mLastTimeCalled;
	float mMinimalDelay;
	float mMinimalStrength;
	float mMinimalMass;
#else
	double mLastTimeCalled;
	double mMinimalDelay;
	double mMinimalStrength;
	double mMinimalMass;
#endif

	bool mWereGlass;
	bool mTooManyContactPoints;	// Havok bugfix
};

// Region Havok class
#ifdef HAVOK3
class havokRegion : public hkPhantomCallbackShape 
#else
class havokRegion : public hkPhantomShape
#endif
{
public:
	havokRegion( Entity* ourRegion );

	virtual void phantomEnterEvent( const hkCollidable& collidableA, const hkCollidable& collidableB, const hkCollisionEnvironment& env );
	virtual void phantomLeaveEvent( const hkCollidable& collidableA, const hkCollidable& collidableB, const hkCollisionEnvironment& env );

private:
	Entity* mRegion;
};

#ifdef HAVOK3
// ####### BLAIR - hkRayCastCallback doesn't exist anymore...therefore havokFilteredRaycastCallback ***WILL***
// need a new implementation!!! #######
class havokFilteredRaycastCallback : public hkBroadPhaseCastCollector
{
public:
	havokFilteredRaycastCallback( const hkWorldRayCastInput& input, hkWorldRayCastOutput* output );

	virtual hkReal addBroadPhaseHandle( const hkBroadPhaseHandle* broadPhaseHandle, int castIndex );
	virtual	hkReal castPrimitive(const hkBroadPhaseHandle* broadphaseHandle );

private:
	hkWorldRayCastInput		m_input;
	hkWorldRayCastOutput*	m_output;
};
/*
class havokFilteredRaycastCallback : public hkRayHitCollector
{
public:
	//havokFilteredRaycastCallback( const hkVector4& fromWorld, const hkVector4& toWorld, const hkShape::hkRayResults& initResults );
	havokFilteredRaycastCallback( const hkWorldRayCastInput& input, const hkWorldRayCastOutput& output );
	hkReal castPrimitive(const hkBroadPhaseHandle* broadphaseHandle );
};
*/
#else
// Filtered Raycast Callback class
class havokFilteredRaycastCallback : public hkRayCastCallback
{
public:
	havokFilteredRaycastCallback( const hkVector4& fromWorld, const hkVector4& toWorld, const hkShape::hkRayResults& initResults );
	virtual	hkReal castPrimitive(const hkBroadPhaseHandle* broadphaseHandle );
};
#endif


#ifdef HAVOK3

// Grenade fix callback class
class havokGrenadeFixCallback : public hkBroadPhaseCastCollector
{
public:
	havokGrenadeFixCallback( const hkWorldRayCastInput& input, hkWorldRayCastOutput* output );
	virtual	hkReal castPrimitive(const hkBroadPhaseHandle* broadphaseHandle );
	virtual hkReal addBroadPhaseHandle( const hkBroadPhaseHandle* broadPhaseHandle, int castIndex );
private:
	bool mClient;	// are we a grenade on client or not?
	hkWorldRayCastInput		m_input;
	hkWorldRayCastOutput*	m_output;
};

// Filtered Raycast Callback Special class - collides with player
class havokFilteredRaycastSpecialCallback : public hkBroadPhaseCastCollector
{
public:
	havokFilteredRaycastSpecialCallback( const hkWorldRayCastInput& input, hkWorldRayCastOutput* output );

	virtual hkReal addBroadPhaseHandle( const hkBroadPhaseHandle* broadPhaseHandle, int castIndex );
	virtual	hkReal castPrimitive(const hkBroadPhaseHandle* broadphaseHandle );

private:
	hkWorldRayCastInput		m_input;
	hkWorldRayCastOutput*	m_output;
};

/*
class havokFilteredRaycastSpecialCallback : public hkRayHitCollector
{
public:
	havokFilteredRaycastSpecialCallback();

	virtual	hkReal castPrimitive(const hkBroadPhaseHandle* broadphaseHandle );
};
*/
// First Hit Raycast Callback class
class havokFirstHitRaycastCallback : public hkBroadPhaseCastCollector
{
public:
	havokFirstHitRaycastCallback( const hkWorldRayCastInput& input, hkWorldRayCastOutput* output );

	virtual hkReal addBroadPhaseHandle( const hkBroadPhaseHandle* broadPhaseHandle, int castIndex );
	virtual	hkReal castPrimitive(const hkBroadPhaseHandle* broadphaseHandle );

private:
	hkWorldRayCastInput		m_input;
	hkWorldRayCastOutput*	m_output;
};
/*
class havokFirstHitRaycastCallback : public hkRayHitCollector
{
public:
	havokFirstHitRaycastCallback();
	virtual	hkReal castPrimitive(const hkBroadPhaseHandle* broadphaseHandle );
};
*/

// Fixed geometry only Raycast Callback class
class havokFixedGeometryRaycastCallback : public hkBroadPhaseCastCollector
{
public:
	havokFixedGeometryRaycastCallback( const hkWorldRayCastInput& input, hkWorldRayCastOutput* output );

	virtual hkReal addBroadPhaseHandle( const hkBroadPhaseHandle* broadPhaseHandle, int castIndex );
	virtual	hkReal castPrimitive(const hkBroadPhaseHandle* broadphaseHandle );

private:
	hkWorldRayCastInput		m_input;
	hkWorldRayCastOutput*	m_output;
};
/*
// Fixed geometry only Raycast Callback class
class havokFixedGeometryRaycastCallback : public hkRayHitCollector
{
public:
	havokFixedGeometryRaycastCallback();
	virtual	hkReal castPrimitive(const hkBroadPhaseHandle* broadphaseHandle );
};
*/

// Player movement fix Raycast Callback class

class havokMovementFixRaycastCallback : public hkBroadPhaseCastCollector
{
public:
	havokMovementFixRaycastCallback( const hkWorldRayCastInput& input, hkWorldRayCastOutput* output );

	virtual hkReal addBroadPhaseHandle( const hkBroadPhaseHandle* broadPhaseHandle, int castIndex );
	virtual	hkReal castPrimitive(const hkBroadPhaseHandle* broadphaseHandle );

private:
	hkWorldRayCastInput		m_input;
	hkWorldRayCastOutput*	m_output;
};
/*
class havokMovementFixRaycastCallback : public hkRayHitCollector
{
public:
	havokMovementFixRaycastCallback();
	virtual	hkReal castPrimitive(const hkBroadPhaseHandle* broadphaseHandle );
};*/

// Floor Check Raycast Callback class
class havokFloorCheckRaycastCallback : public hkBroadPhaseCastCollector
{
public:
	havokFloorCheckRaycastCallback( const hkWorldRayCastInput& input, hkWorldRayCastOutput* output );
	virtual	hkReal castPrimitive(const hkBroadPhaseHandle* broadphaseHandle );
	bool fixedGeometryOnly;
protected:
	virtual hkReal addBroadPhaseHandle( const hkBroadPhaseHandle* broadPhaseHandle, int castIndex );
private:
	hkWorldRayCastInput		m_input;
	hkWorldRayCastOutput*	m_output;
};

class havokMonsterFloorCheckRaycastCallback : public hkBroadPhaseCastCollector
{
public:
	havokMonsterFloorCheckRaycastCallback( const hkWorldRayCastInput& input, hkWorldRayCastOutput* output );
	virtual	hkReal castPrimitive(const hkBroadPhaseHandle* broadphaseHandle );
	virtual hkReal addBroadPhaseHandle( const hkBroadPhaseHandle* broadPhaseHandle, int castIndex );
	bool fixedGeometryOnly;
	hkWorldRayCastInput		m_input;
	hkWorldRayCastOutput*	m_output;
};

#else // HAVOK3

// Grenade fix callback class
class havokGrenadeFixCallback : public hkRayCastCallback
{
public:
	havokGrenadeFixCallback( const hkVector4& fromWorld, const hkVector4& toWorld, const hkShape::hkRayResults& initResults, bool client );
	virtual	hkReal castPrimitive(const hkBroadPhaseHandle* broadphaseHandle );
private:
	bool mClient;	// are we a grenade on client or not?
};

// Filtered Raycast Callback Special class - collides with player
class havokFilteredRaycastSpecialCallback : public hkRayCastCallback
{
public:
	havokFilteredRaycastSpecialCallback( const hkVector4& fromWorld, const hkVector4& toWorld, const hkShape::hkRayResults& initResults );

	virtual	hkReal castPrimitive(const hkBroadPhaseHandle* broadphaseHandle );
};

// First Hit Raycast Callback class
class havokFirstHitRaycastCallback : public hkRayCastCallback
{
public:
	havokFirstHitRaycastCallback( const hkVector4& fromWorld, const hkVector4& toWorld, const hkShape::hkRayResults& initResults );
	virtual	hkReal castPrimitive(const hkBroadPhaseHandle* broadphaseHandle );
};

// Fixed geometry only Raycast Callback class
class havokFixedGeometryRaycastCallback : public hkRayCastCallback
{
public:
	havokFixedGeometryRaycastCallback( const hkVector4& fromWorld, const hkVector4& toWorld, const hkShape::hkRayResults& initResults );
	virtual	hkReal castPrimitive(const hkBroadPhaseHandle* broadphaseHandle );
};

// Player movement fix Raycast Callback class
class havokMovementFixRaycastCallback : public hkRayCastCallback
{
public:
	havokMovementFixRaycastCallback( const hkVector4& fromWorld, const hkVector4& toWorld, const hkShape::hkRayResults& initResults );
	virtual	hkReal castPrimitive(const hkBroadPhaseHandle* broadphaseHandle );
};

// Floor Check Raycast Callback class
class havokFloorCheckRaycastCallback : public hkRayCastCallback
{
public:
	havokFloorCheckRaycastCallback( const hkVector4& fromWorld, const hkVector4& toWorld, const hkShape::hkRayResults& initResults );
	virtual	hkReal castPrimitive(const hkBroadPhaseHandle* broadphaseHandle );
	bool fixedGeometryOnly;
};

class havokMonsterFloorCheckRaycastCallback : public hkRayCastCallback
{
public:
	havokMonsterFloorCheckRaycastCallback( const hkVector4& fromWorld, const hkVector4& toWorld, const hkShape::hkRayResults& initResults );
	virtual	hkReal castPrimitive(const hkBroadPhaseHandle* broadphaseHandle );
	bool fixedGeometryOnly;
};
#endif

// Filtered AABB Phantom class
#pragma warning (disable : 4511)

class havokStepCheckPhantom : public hkAabbPhantom
{
public:
#ifdef HAVOK3
	havokStepCheckPhantom( hkAabb& info, float minimalMass );
	//hkPhantomType getType() const;
	//void calcAabb( hkAabb& aabb );
	//void addOverlappingCollidable( hkCollidable* collidable );
	//hkBool isOverlappingCollidableAdded( hkCollidable* collidable );
	//void removeOverlappingCollidable( hkCollidable* collidable );
	//hkPhantom* clone() const; 
	//void deallocateInternalArrays();
#else
	havokStepCheckPhantom( hkPhantomConstructionInfo& info, float minimalMass );
#endif
	virtual void addOverlappingCollidable( hkCollidable& c );
	virtual void removeOverlappingCollidable( hkCollidable& c );

#ifdef HAVOK3
protected:
	hkMotionState* getMotionState(){ return HK_NULL; }
#endif

private:
	float mMinimalMass;
};
#pragma warning (default : 4511)

// This agent handles collisions between hkSpheres and hkTriangles.
class hkCollisionDispatcher;
class hkUserSphereTriangleAgent : public hkCollisionAgent
{
public:
	//Registers this agent with the collision dispatcher.
	static void registerAgent(hkCollisionDispatcher* dispatcher );
	// hkCollisionAgent interface implementation.
#ifdef HAVOK3
	virtual void getPenetrations( const hkCdBody& bodyA, const hkCdBody& bodyB, const hkCollisionInput& input, hkCdBodyPairCollector& collector );
	virtual void linearCast( const hkCdBody& bodyA, const hkCdBody& bodyB, const hkLinearCastCollisionInput& input, hkCdPointCollector& castCollector, hkCdPointCollector* startCollector );
	virtual void updateShapeCollectionFilter( const hkCdBody& bodyA, const hkCdBody& bodyB, const hkCollisionInput& input ) {}
	virtual void invalidateTim(hkCollisionInput& input) {}
	virtual void warpTime(hkTime oldTime, hkTime newTime, hkCollisionInput& input) {}
	virtual void removePoint( hkContactPointId idToRemove ) {}
	virtual void commitPotential( hkContactPointId newId ) {}
	virtual void createZombie( hkContactPointId idTobecomeZombie ) {}

	virtual inline void processCollision(const hkCollidable& collA, const hkCollidable& collB, const hkProcessCollisionInput& env,  hkProcessCollisionOutput& result);
#else
	virtual inline void processCollision(const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env, hkCollisionResult& result);
#endif
	// hkCollisionAgent interface implementation.
#ifdef HAVOK3
	virtual void getClosestPoints( const hkCollidable& collidableA, const hkCollidable& collidableB, const hkCollisionInput& env,  hkCdBodyPairCollector& pointDetails);
#else
	virtual void getClosestPoints( const hkCollidable& collidableA, const hkCollidable& collidableB, const hkCollisionEnvironment& env, hkClosestPointsDetails& pointDetails);
#endif
	// hkCollisionAgent interface implementation.
	virtual bool arePenetrating(const  hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env );
	// hkCollisionAgent interface implementation.
#ifdef HAVOK3
	virtual void cleanup( const hkCollisionInput& env );
#else
	virtual void cleanup( const hkCollisionEnvironment& env );
#endif

protected:
	// Constructor, called by the agent creation functions.
#ifdef HAVOK3
	hkUserSphereTriangleAgent( const hkCollidable& collA, const hkCollidable& collB, const hkCollisionInput& env );
#else
	hkUserSphereTriangleAgent( const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env );
#endif
	// Agent creation function used by the hkCollisionDispatcher. 
	static hkCollisionAgent* createSphereTriangleAgent(const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env);
        
	// Agent creation function used by the hkCollisionDispatcher. 
	static hkCollisionAgent* createTriangleSphereAgent(	const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env);

private:
	HK_FORCE_INLINE bool getClosestPoint( const hkCollidable& collidableA, const hkCollidable& collidableB, hkReal&	dist, hkVector4& pointInWorld, hkVector4& normalInWorld, const hkCollisionEnvironment& environment );

	hkContactPointId m_contactPointId;
	hkCollideTriangleUtil::ClosestPointTriangleCache m_closestPointTriangleCache;
	hkReal m_invTriangleNormalLength;
};

// This agent deals with collisions between hkMultiSphereShapes and hkTriangles.
class hkUserMultiSphereTriangleAgent : public hkCollisionAgent
{
public:
	// Registers the agent with the collision dispatcher.
	static void registerAgent(hkCollisionDispatcher* dispatcher);
	// hkCollisionAgent interface implementation.
#ifdef HAVOK3
	virtual void getPenetrations( const hkCdBody& bodyA, const hkCdBody& bodyB, const hkCollisionInput& input, hkCdBodyPairCollector& collector );
	virtual void linearCast( const hkCdBody& bodyA, const hkCdBody& bodyB, const hkLinearCastCollisionInput& input, hkCdPointCollector& castCollector, hkCdPointCollector* startCollector );
	virtual inline void processCollision(const hkCollidable& collA, const hkCollidable& collB, const hkProcessCollisionInput& env, hkProcessCollisionOutput& result);
#else
	virtual inline void processCollision(const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env, hkCollisionResult& result);
#endif
	// hkCollisionAgent interface implementation.
	virtual bool arePenetrating( const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env );
	// hkCollisionAgent interface implementation.
#ifdef HAVOK3
	virtual void getClosestPoints( const hkCollidable& collidableA, const hkCollidable& collidableB, const hkCollisionInput& env,  hkCdBodyPairCollector& pointDetails);
#else
	virtual void getClosestPoints( const hkCollidable& collidableA, const hkCollidable& collidableB, const hkCollisionEnvironment& env, hkClosestPointsDetails& pointDetails);
#endif
#ifdef HAVOK3
	// hkCollisionAgent interface implementation.
	virtual void cleanup( const hkCollisionInput& env);
#else
	// hkCollisionAgent interface implementation.
	virtual void cleanup( const hkCollisionEnvironment& env);
#endif
protected:

	// Constructor, called by the agent creation functions.
#ifdef HAVOK3
	hkUserMultiSphereTriangleAgent( const hkCollidable& collA, const hkCollidable& collB, const hkCollisionInput& env );
#else
	hkUserMultiSphereTriangleAgent( const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env );
#endif
	// Agent creation function used by the hkCollisionDispatcher. 
	static hkCollisionAgent* createMultiSphereTriangleAgent( const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env);
	// Agent creation function used by the hkCollisionDispatcher. 
	static hkCollisionAgent* createTriangleMultiSphereAgent( const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env);

private:
	hkReal			m_invTriangleNormalLength;
	hkContactPointId m_contactPointId[hkMultiSphereShape::MAX_SPHERES];
	hkCollideTriangleUtil::ClosestPointTriangleCache m_closestPointTriangleCache;
};

// Custom error handler
#ifdef HAVOK3
class havokErrorHandler : public hkError
{
public:
    virtual void message(Message m, int id, const char* description, const char* file, int line);
    virtual void setEnabled( int id, hkBool enabled );
    virtual hkBool isEnabled( int id );
    virtual void enableAll();
	static hkReferencedObject* create();
	virtual ~havokErrorHandler( void );

protected:
	hkArray<int> m_disabledAsserts;
};
#else
class havokErrorHandler : public hkError
{
	struct hkAssertPoint
	{
		char* m_file;
		int m_line;
	};

public:
	virtual void assertMessage(const char* what, const char* description, const char* file, int line);
	virtual void errorMessage(const char* what, const char* description, const char* file, int line);
	virtual void warningMessage(const char* what, const char* description, const char* file, int line);
	virtual void disableAssert( const char *file, int line );
	static hkReferencedObject* create();
	virtual ~havokErrorHandler( void );

protected:

	bool isDisabled(const char* file, int line);
	virtual const char* hkGetFileBaseName( const char* path );

	hkArray<hkAssertPoint> m_disabledAsserts;
};
#endif

// Deactivator for physic objects
class havokPhysicsObjectDeactivator : public hkRigidBodyDeactivator
{
public:
	havokPhysicsObjectDeactivator( void );
#ifdef HAVOK3
	virtual hkBool shouldDeactivateHighFrequency( const hkEntity* entity );
	virtual hkBool shouldDeactivateLowFrequency( const hkEntity* entity );
#else
	virtual bool shouldDeactivateHighFrequency( const hkEntity* entity );
	virtual bool shouldDeactivateLowFrequency( const hkEntity* entity );
#endif

public:
	hkReal mMinHighFrequencyLinearVelocitySquared;
	hkReal mMinHighFrequencyAngularVelocitySquared;
	hkReal mMinLowFrequencyLinearVelocitySquared;
	hkReal mMinLowFrequencyAngularVelocitySquared;
};

// Deactivator for physic objects

#define DEACTIV_TIMES_TO_FAIL 3

class myPhysicsObjectDeactivator
{
public:
	myPhysicsObjectDeactivator( float deactDelay, float deactMaxPosDiff );
	void AddRigidBody( hkRigidBody* body );
	void RemoveRigidBody( hkRigidBody* body );
	void DeactivateBodies( float deltaTime );

protected:
	float mTimeFromLastTest;
	float mTimerTime;
	float mTimeFromLastPosChange;
	float mTimerPosChangeTime;

	DynamicArray<hkRigidBody*> mBodiesToTest;
	DynamicArray<char> mBodyFailedLastTime;
	DynamicArray<Vector> mBodyPosLastTime;

public:
	hkReal mMinHighFrequencyLinearVelocitySquared;
	hkReal mMinHighFrequencyAngularVelocitySquared;
	hkReal mMinMovementSquared;
};

class hkDispatchFailureAgent : public hkNullAgent
{
public:
	/*
	virtual void processCollision(const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env, hkCollisionResult& result);
	virtual bool arePenetrating( const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env );
	virtual void getClosestPoints( const hkCollidable& collidableA, const hkCollidable& collidableB, const hkCollisionEnvironment& env, hkClosestPointsDetails& pointDetails);
	virtual void cleanup(const hkCollisionEnvironment& env);
	*/
	static hkCollisionAgent* HK_CALL createDispatchFailureAgent( const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env );
	static char* getNameFromShapeType(hkShapeType type);
};

#undef DEACTIV_TIMES_TO_FAIL

class havokWorldBorder : public hkBroadPhaseBorder
{
public:
	havokWorldBorder( hkWorld* world, hkReal broadPhaseSize = hkMath::HK_REAL_MAX );

	// If a body moves outside the world size (broadphase), report it, and delete it
	virtual void maxPositionExceededCallback( hkEntity* entity );
};
/*
class havokVehicleAction : public hkUnaryAction
{
	public:
		havokVehicleAction( hkRigidBody* chassis, HavokVehicle* vehicle, int turnNo = 0, int numTurns = 1 );
		virtual ~havokVehicleAction();
		virtual void applyAction( const hkStepInfo& stepInfo );
	private:
		// This system allows for reducing the frequency to the vehicle. 
		// 1/numTurns defines the frequency the vehicle is called (1 = every frame).
		// To avoid that all vehicles are updated at the same time, turnNo can be
		// set to a different value between [0,numTurns[.
		int mTurnNo;
		int mNumTurns;
		HavokVehicle *mHavokVehicle;
};
*/

class havokBreakableListener : public hkBreakableListener
{
public:
#ifdef HAVOK3
	// This change should carry over fine...the old hkBreakableData and the new hkBreakableConstraintEvent
	// are almost identical...the new one has one more member.
	virtual void constraintBrokenCallback(hkBreakableConstraintEvent& event);
#else
	virtual void constraintBrokenCallback(hkBreakableData& event);
#endif
};

// -----------------------------------------------------------------
// Cache classes
// -----------------------------------------------------------------

class HavokWorldConvexEntry
{
public:
	HavokWorldConvexEntry() {};
	~HavokWorldConvexEntry() {};

	hkArray<hkVector4> planeEquations;
	hkArray<hkVector4> vertsOut;
	float scale;
};

class HavokWorldConvexCache
{
public:
	HavokWorldConvexCache() {};
	~HavokWorldConvexCache() {};

	inline HavokWorldConvexEntry* GetEntry( String& filename, String& meshname, bool centered );
	inline void PutEntry( String& filename, String& meshname, hkArray<hkVector4>& planeEquations, hkArray<hkVector4>& vertsOut, float scale, bool centered );

protected:
	DynamicArray<HavokWorldConvexEntry> Entries;
	HashMap<String,int,32> EntryCache;
};


#define OLD_PAWN_SCALE (0.2f)


#endif //_HAVOKPHYSICS_

