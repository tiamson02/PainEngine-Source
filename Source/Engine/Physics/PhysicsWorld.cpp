#include "Engine.h"

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

#include "Physics/Havok/HavokPhysics.h"
#include "Physics/Havok/HavokWorld.h"
#include "Physics/Havok/POHavokPart.h"
#include "Physics/Havok/HavokCharacterController.h"
#include "Engine/Portal.h"
#include "Engine/Mesh.h"
#include "Physics/PhysicsEngine.h"
#include "Physics/PhysicsWorld.h"
#include "Net/NetworkDevice2.h"

#include "Engine/Model.h"
#include "Engine/World.h"

#include "Engine/PCFSystem.h"
extern ENGINE_API PCFSystem* GEngine;

PhysicsWorld::PhysicsWorld( float deactDelay, float deactMaxPosDiff ) : mEngineWorld(0), mPaused(true), mFirstTime(true), ActiveMeshesMassScale(1.0f)
,	DefaultMeshFriction( 0.5f ), DefaultMeshRestitution(0.5f)
{
	// Start new Havok world
	GLog.Print( "Starting new Havok world\n" );
    m_setTag("Physics");
	mHavokWorld = new HavokWorld( deactDelay, deactMaxPosDiff );
	hkVector4 gravity(0.0f,-GEngine->Physics->TWGravity, 0.0f);
	mHavokWorld->World->setGravity( gravity );
	mHavokWorld->World->setLowFrequencyDeactivationPeriod( 2.0f );
	float deact = mHavokWorld->World->getLowFrequencyDeactivationPeriod();
	GLog.Print( "Low frequency deactivation period: %f\n", deact );
	deact = mHavokWorld->World->getHighFrequencyDeactivationPeriod();
	GLog.Print( "High frequency deactivation period: %f\n", deact );
}

PhysicsWorld::~PhysicsWorld()
{
	delete mHavokWorld;

	for( int i = 0; i <GEngine->Physics->mPhysicsWorlds.Size(); ++i )
		if( GEngine->Physics->mPhysicsWorlds[i] == this )
		{
			GEngine->Physics->mPhysicsWorlds.Remove(i);
			break;
		}
}

PhysicsObject* PhysicsWorld::CreatePhysicsObject(const Vector& pos, const Quaternion& quat, DWORD pawnBodyType, Entity* entity, float scale, int collisionGroup, bool dontAddToWorld )
{
	if( entity && ( entity->Type == Entity::ETypeMesh ) &&
		( ( pawnBodyType == EPawnFromMeshConvex ) ||
		  ( pawnBodyType == EPawnFromMeshConvexSweep ) ||
		  ( pawnBodyType == EPawnFromMeshNotConvex ) ||
		  ( pawnBodyType == EPawnFromMeshGlass ) ||
		  ( pawnBodyType == EPawnFromMeshGlassSweep ) ||
		  ( pawnBodyType == EPawnFromMeshNonCentered ) ||
		  ( pawnBodyType == EPawnFromMeshNonCenteredSweep ) ||
		  ( pawnBodyType == EPawnFromMeshNonCenteredNotConvex ) ) )
		return CreatePhysicsObjectFromMesh( pos, quat, pawnBodyType, (WorldMesh*)entity, collisionGroup, dontAddToWorld );

	if( pawnBodyType == EPawnRagdoll )
	{
		if( !entity || !( entity->Type == Entity::ETypeModel ) )
			return 0;

		return CreatePhysicsObjectFromRagdoll( pos, quat, (Model*)entity, scale, collisionGroup, dontAddToWorld );
	}

	bool player = ( pawnBodyType == EPawnBodyPlayer );

	Vector pivotOffset;
	hkRigidBody* body = NULL;
	bool noScale = ( scale <= 0 );
	if( scale > 0 || entity->Type == Entity::ETypeNone )	// No information about pawn's bounding box
		body = mHavokWorld->CreatePawnBody( pivotOffset, scale, pawnBodyType, collisionGroup );
	else
	{
		Vector center = entity->LocalBox.GetCenter();
		if (entity->AsModel()) {
			Model* m = entity->AsModel();
			int CenterJointId = m->GetJointIndex("ROOOT");
			if (CenterJointId>=0)
			{
				Vector roootTrans = m->Skeleton.Joints[CenterJointId].Transform.GetTranslation();
				center.X = roootTrans.X;
				center.Z = roootTrans.Z;
			}
		}
		Vector feet = center;
		feet.Y = entity->LocalBox.Min.Y;
		scale = (center.Y-feet.Y)/(5.5f*OLD_PAWN_SCALE);
	    body = mHavokWorld->CreatePawnBody( pivotOffset, scale, pawnBodyType, collisionGroup );
		pivotOffset = -center;
	}
	if( entity )
	{
		hkPropertyValue pv;
#ifdef HAVOK3
		//pv.getPtr() = entity;
		pv.setPtr(entity);
#else
		pv.asPtr = entity;
#endif
		body->addProperty( HPEntityPointer, pv );
	}

	Vector realPos = pos - pivotOffset;

	hkQuaternion qu( quat.X, quat.Y, quat.Z, -quat.W);
	qu.normalize();

	hkVector4 realPos1( realPos.X, realPos.Y, realPos.Z );

#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
		if( !realPos1.isOk4() || ( fabs(realPos1(0)) > 10000.0f ) || ( fabs(realPos1(1)) > 10000.0f ) || ( fabs(realPos1(2)) > 10000.0f ) )
#else
		if( !realPos1.isOk() || ( fabs(realPos1(0)) > 10000.0f ) || ( fabs(realPos1(1)) > 10000.0f ) || ( fabs(realPos1(2)) > 10000.0f ) )
#endif
			GLog.Print( "PO position > 10000 in in CreatePhysicsObject\n" );

		if( !qu.isOk() )
			GLog.Print( "Bad quaternion given to PO in CreatePhysicsObject\n" );
#endif

	body->setPositionAndRotation( realPos1, qu );

	if( !player )	// Settings for monsters
	{
		body->getMaterial().setRestitution(0.001f);
		body->getMaterial().setFriction( 0.1f );
	}

    m_setTag("Physics");
	PhysicsObject* po = new PhysicsObject;
	po->mHavokPart->World = mHavokWorld;
	if( !dontAddToWorld )
	{
		mHavokWorld->World->addEntity( body );
		po->mPOTableIndex = mHavokWorld->PhysicsObjects.Add( po );
	}

    po->mHavokPart->Body     = body;
	po->mModelPivotOffset = pivotOffset;
	po->BodyType  = pawnBodyType;
	po->BodyScale = scale;

	if( player )
	{
        m_setTag("Physics");
		po->mPlayerVariables = new PhysicsObjectPlayerVariables;
		po->mPlayerVariables->LastGoodPosition = po->GetPosition();
		po->State |= EPOIsPlayer;

		// Addition for new player movement testing
#ifdef	TRANSGAMING		
		if( GEngine->GameState != PCFSystem::SingleGame )
#else
		if( GEngine->GameState != PCFSystem::PCFSystemStates::SingleGame )
#endif		
		{
			HavokCharacterConstructionInfo hkinfo;
			hkinfo.m_position.set( po->mPlayerVariables->LastGoodPosition.X, po->mPlayerVariables->LastGoodPosition.Y, po->mPlayerVariables->LastGoodPosition.Z );
			hkinfo.m_halfExtents.set(2.0f,5.5f,2.0f);
			hkinfo.m_halfExtents.mul4(scale*OLD_PAWN_SCALE);
            m_setTag("Physics");
			po->mPlayerVariables->Controller = new HavokCharacterController( hkinfo, mHavokWorld->World, body );
		}
	}

	if( noScale )
		po->State |= EPOCreatedWithNoScale;

	po->SetFreedomOfRotation( ERotYAxis );

	if( player && mHavokWorld->IsUnderwater() )
		po->EnableGravity( false );

    return po;
}

PhysicsObject* PhysicsWorld::CreatePhysicsObjectFromRagdoll( const Vector& pos, const Quaternion& quat, Model* model, float scale, int collisionGroup, bool dontAddToWorld )
{
//	Vector center = model->LocalBox.GetCenter();
//	Vector pivotOffset = -center;
	Vector pivotOffset = Vector(0,0,0);

	float volumeMass = -1.0f;
	hkRigidBody* body = mHavokWorld->CreatePawnBodyFromRagdoll( model, volumeMass, scale/10.0f, collisionGroup );
	if (!body) return NULL;

	hkPropertyValue pv;
#ifdef HAVOK3
	//pv.getPtr() = model;
	pv.setPtr(model);
#else
	pv.asPtr = model;
#endif
	body->addProperty( HPEntityPointer, pv );

	Vector realPos = pos - pivotOffset;

	hkQuaternion qu( quat.X, quat.Y, quat.Z, -quat.W);
	qu.normalize();
	
	hkVector4 realPos1( realPos.X, realPos.Y, realPos.Z );
	body->getMaterial().setRestitution( 0.1f );
	body->getMaterial().setFriction( 0.5f );
	
#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
		if( !realPos1.isOk4() || ( fabs(realPos1(0)) > 10000.0f ) || ( fabs(realPos1(1)) > 10000.0f ) || ( fabs(realPos1(2)) > 10000.0f ) )
#else
		if( !realPos1.isOk() || ( fabs(realPos1(0)) > 10000.0f ) || ( fabs(realPos1(1)) > 10000.0f ) || ( fabs(realPos1(2)) > 10000.0f ) )
#endif
			GLog.Print( "PO position > 10000 in in CreatePhysicsObjectFromRagdoll\n" );

		if( !qu.isOk() )
			GLog.Print( "Bad quaternion given to PO in CreatePhysicsObjectFromRagdoll\n" );

#endif

	body->setPositionAndRotation( realPos1, qu );

    m_setTag("Physics");
	PhysicsObject* po = new PhysicsObject;
	po->mVolumeMass = volumeMass;

	if( !dontAddToWorld )
	{
		mHavokWorld->World->addEntity( body );
		po->mPOTableIndex = mHavokWorld->PhysicsObjects.Add( po );
	}

	po->mHavokPart->World = mHavokWorld;
    po->mHavokPart->Body      = body;
	po->mModelPivotOffset = pivotOffset;
	po->BodyType  = EPawnRagdoll;
	po->BodyScale =	scale;

	po->SetHardDeactivator();

//	po->State |= EPOIsActiveMesh;
	if( model->PO )
		delete model->PO;
	model->PO = po;

    return po;
}

PhysicsObject* PhysicsWorld::CreatePhysicsObjectFromMesh(const Vector& pos, const Quaternion& quat, DWORD pawnBodyType, WorldMesh* mesh, int collisionGroup, bool dontAddToWorld )
{
	bool dontCenter = ( pawnBodyType == EPawnFromMeshNonCentered ) || ( pawnBodyType == EPawnFromMeshGlass ) || ( pawnBodyType == EPawnFromMeshNonCenteredNotConvex ) ||
		( pawnBodyType == EPawnFromMeshNonCenteredSweep ) || ( pawnBodyType == EPawnFromMeshGlassSweep );

	Vector center = mesh->LocalBox.GetCenter();
	Vector pivotOffset = -center;

	if( dontCenter )
		pivotOffset = Vector(0,0,0);

	float volumeMass = -1.0f;
	hkRigidBody* body = mHavokWorld->CreatePawnBodyFromMesh( mesh, volumeMass, pawnBodyType, collisionGroup );

	if (!body) return NULL;

	hkPropertyValue pv;
#ifdef HAVOK3
	pv.setPtr(mesh);
#else
	pv.asPtr = mesh;
#endif
	body->addProperty( HPEntityPointer, pv );

	Vector realPos = pos - pivotOffset;

	hkQuaternion qu( quat.X, quat.Y, quat.Z, -quat.W);
	qu.normalize();
	
	hkVector4 realPos1( realPos.X, realPos.Y, realPos.Z );
	//mesh->ObjectToWorld.SetIdentity();    // = IdentityMatrix();
	body->getMaterial().setRestitution( 0.1f );
	body->getMaterial().setFriction( 0.5f );

#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
		if( !realPos1.isOk4() || ( fabs(realPos1(0)) > 10000.0f ) || ( fabs(realPos1(1)) > 10000.0f ) || ( fabs(realPos1(2)) > 10000.0f ) )
#else
		if( !realPos1.isOk() || ( fabs(realPos1(0)) > 10000.0f ) || ( fabs(realPos1(1)) > 10000.0f ) || ( fabs(realPos1(2)) > 10000.0f ) )
#endif
			GLog.Print( "PO position > 10000 in in CreatePhysicsObjectFromMesh\n" );

		if( !qu.isOk() )
			GLog.Print( "Bad quaternion given to PO in CreatePhysicsObjectFromMesh\n" );
#endif
	
	body->setPositionAndRotation( realPos1, qu );

//	GLog.Print( "Active Mesh Mass: %f\n", body->getMass() );

    m_setTag("Physics");
	PhysicsObject* po = new PhysicsObject;
	po->mVolumeMass = volumeMass;

	if( !dontAddToWorld )
	{
		mHavokWorld->World->addEntity( body );
		po->mPOTableIndex = mHavokWorld->PhysicsObjects.Add( po );
	}

	po->mHavokPart->World = mHavokWorld;
    po->mHavokPart->Body  = body;

	// set different deactivator for active meshes
	if( ( pawnBodyType == EPawnFromMeshNonCentered ) || ( pawnBodyType == EPawnFromMeshNonCenteredSweep ) ||
		( pawnBodyType == EPawnFromMeshNonCenteredNotConvex ) )
		po->SetHardDeactivator();

	po->mModelPivotOffset = pivotOffset;
	po->BodyType  = pawnBodyType;
	po->BodyScale =	1.0;
//	po->State |= EPOIsActiveMesh;
	if( mesh->PO )
		delete mesh->PO;
	mesh->PO = po;

    return po;
}

void PhysicsWorld::HitEntity(void* entity, const Vector& hitPoint, const Vector& hitVector)
{
	hkRigidBody* rb = static_cast<hkRigidBody*>( entity );

	Assert(rb);
	// Assert(rb->getSimulationIsland()); XXX Sokol - nie moglem strzelac w szyby w debugu z shotguna

#ifdef HAVOK3
	if(rb->getSimulationIsland() && !rb->isFixed() && rb->getMotionType() != hkMotion::MOTION_KEYFRAMED )
#else
	if(rb->getSimulationIsland() && !rb->isFixed() && !rb->isKeyframed() )
#endif
	{
		hkVector4 hitVector1( hitVector.X, hitVector.Y, hitVector.Z );
		hkVector4 hitPoint1( hitPoint.X, hitPoint.Y, hitPoint.Z );

#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
		if( !hitVector1.isOk4() )
#else
		if( !hitVector1.isOk() )
#endif
			GLog.Print( "Corrupted force in HitEntity\n" );
		else if( hitVector1.length3() > 50000.0f )
			GLog.Print( "Force > 50000 in HitEntity\n" );
#ifdef HAVOK3
		if( !hitPoint1.isOk4() )
#else
		if( !hitPoint1.isOk() )
#endif
			GLog.Print( "Corrupted point in HitEntity\n" );
		else if( hitPoint1.length3() > 10000.0f )
			GLog.Print( "Point pos > 10000 in HitEntity\n" );
		float myMass = rb->getMass();
		if( myMass < 0.000001f )
		{
			Assert( myMass >= 0 );
			Entity* enti = 0;
			if( rb->hasProperty( HPEntityPointer ) )
			{
				hkPropertyValue p = rb->getProperty( HPEntityPointer );
#ifdef HAVOK3
				enti = (Entity*)p.getPtr();
#else
				enti = (Entity*)p.asPtr;
#endif
			}
			GLog.Print( "Zero mass in HitEntity ( entity '%s, filename '%s' )\n", enti?enti->Name.AsChar():"(null)", enti?enti->FileName.AsChar():"(null)" );
		}
#endif

		rb->applyPointImpulse( hitVector1, hitPoint1 );
		if( !rb->isActive() && rb->getSimulationIsland() )
#ifdef HAVOK3
			rb->activate();
#else
			rb->forceActivate();
#endif
	}
}

void PhysicsWorld::AddMesh(WorldMesh* mesh, float friction, float restitution )
{
    BOOL fixed = !(mesh->Flags & WorldMesh::EPhysics);

    if( !fixed )
    {
		if( strstr( mesh->Name.AsChar(),"physdest" ) )
		{
			// destructible mesh - dynamic part
			DWORD bodyType = EPawnFromMeshNonCentered;
			if( strstr( mesh->Name.AsChar(),"concave" ) )
				bodyType = EPawnFromMeshNonCenteredNotConvex;

			PhysicsObject* obj = CreatePhysicsObjectFromMesh( mesh->GetPosition(), mesh->GetRotation(), bodyType, mesh, HCGNormal, true );
			if( obj )
			{
				obj->SetFriction( friction );
				obj->SetRestitution( restitution );

				float massMultiplier = 1.0f;

				if( !GEngine->Net || GEngine->Net->IsServer() )
				{
					Assert( mesh->Name.Len() );
					int t = GScript.GetTop();
					GScript.Call( "Level_GetActiveMeshesData", 1, "%s", mesh->Name.AsChar() );
					massMultiplier = GScript.GetFloat( -1 );
					GScript.SetTop(t);
				}

				if( massMultiplier == 1.0f )
					massMultiplier = ActiveMeshesMassScale;

//				float mass = obj->GetMass();
				obj->ScaleMass( massMultiplier );
//				obj->SetMass( mass*massMultiplier );
				obj->SetAngularDamping( 1.8f );
#ifdef	TRANSGAMING
				GEngine->pcfWorld->RemoveEntity( mesh );
#else
				GEngine->World->RemoveEntity( mesh );
#endif
				mHavokWorld->PinnedActiveMeshParts.Add( mesh );
				int ind = mHavokWorld->PhysArray.Add( obj );
				mHavokWorld->PhysIndices.Insert( obj, ind );
			}
		}
		else
		{
			// destructible mesh - dynamic part
			DWORD bodyType = EPawnFromMeshNonCentered;
			if( strstr( mesh->Name.AsChar(),"concave" ) )
				bodyType = EPawnFromMeshNonCenteredNotConvex;
/*
#ifdef _DEBUG
			if( ( mesh->Name == "actgrp08_phys_pinned_autodelete_34shape" ) ||
				( mesh->Name == "actgrp08_phys_pinned_autodelete_76shape" ) )
				return;
#endif
*/
			PhysicsObject* obj = CreatePhysicsObjectFromMesh( mesh->GetPosition(), mesh->GetRotation(), bodyType, mesh, HCGNormal, true );
			if( obj )
			{
				obj->SetFriction( friction );
				obj->SetRestitution( restitution );

				float massMultiplier = 1.0f;

				if( !GEngine->Net || GEngine->Net->IsServer() )
				{
					Assert( mesh->Name.Len() );
					int t = GScript.GetTop();
					GScript.Call( "Level_GetActiveMeshesData", 1, "%s", mesh->Name.AsChar() );
					massMultiplier = GScript.GetFloat( -1 );
					GScript.SetTop( t );
				}

				if( massMultiplier == 1.0f )
					massMultiplier = ActiveMeshesMassScale;

//				float mass = obj->GetMass();
				obj->ScaleMass( massMultiplier );
//				obj->SetMass( mass*massMultiplier );

				if( strstr( mesh->Name.AsChar(),"pinned" ) )
				{
					//GScript.DoString( "Game.TotalDestroyed = Game.TotalDestroyed + 1" );
					PinnedActiveMesh newMesh;
					newMesh.ActivationPoint = mesh->Box.GetCenter();
					Vector extent = mesh->Box.GetExtent()*0.5f;
					newMesh.ActivationExtent = extent.Magnitude();
					newMesh.ActiveMesh = obj;
					newMesh.StaticMeshBody = mHavokWorld->CreateStaticMesh( mesh, friction, restitution );
					newMesh.StaticMesh = mesh;
					newMesh.ActivationDisabled = false;
					newMesh.TimeToDelete = -1.0f;
					newMesh.RecurrentActivationDisabled = false;
					mHavokWorld->PinnedActiveMeshes.Add( newMesh );
				}
				else
				{
					obj->Enable( true );
					if( strstr( mesh->Name.AsChar(),"autodelete" ) )
						obj->mHavokPart->Body->addEntityActivationListener( mHavokWorld->mDisappearTrigger );
				}
				int ind = mHavokWorld->PhysArray.Add( obj );
				mHavokWorld->PhysIndices.Insert( obj, ind );
			}
		}

		return;
	}

	if( strstr( mesh->Name.AsChar(),"statdest" ) )
	{
		// destructible mesh - static part
		//GScript.DoString( "Game.TotalDestroyed = Game.TotalDestroyed + 1" );
		PinnedActiveMesh newMesh;
		newMesh.ActivationPoint = mesh->Box.GetCenter();
		Vector extent = mesh->Box.GetExtent()*0.5f;
		newMesh.ActivationExtent = extent.Magnitude();
		newMesh.ActiveMesh = 0;
		newMesh.StaticMeshBody = mHavokWorld->CreateStaticMesh( mesh, friction, restitution );
		newMesh.StaticMesh = mesh;
		newMesh.ActivationDisabled = false;
		newMesh.TimeToDelete = -1.0f;
		newMesh.RecurrentActivationDisabled = false;
		mHavokWorld->PinnedActiveMeshes.Add( newMesh );
		return;
	}

#ifdef	TRANSGAMING
	Assert( GEngine->pcfWorld);

    DynamicArray<Glass*>& glasses = GEngine->pcfWorld->Glasses;
#else
	Assert( GEngine->World );

    DynamicArray<Glass*>& glasses = GEngine->World->Glasses;
#endif
	for( int i = 0; i < glasses.Size(); ++i )
		if( glasses[i]->GetMainMesh() == mesh )
		{
			hkRigidBody* body = mHavokWorld->CreateStaticMesh( mesh, friction, restitution );
			if( body )
			{
				//GScript.DoString( "Game.TotalDestroyed = Game.TotalDestroyed + 1" );
				hkPropertyValue pv;
#ifdef HAVOK3
				pv.setPtr(glasses[i]);
#else
				pv.asPtr = glasses[i];
#endif
				body->addProperty( HPGlass, pv );
				body->setProcessContactCallbackDelay(0);
                TRACE("PhysicsWorld::AddMesh(): creating collision listener with a delay of %g\n", 0);
                m_setTag("Physics");
				glasses[i]->SetCL( new havokCollisionListener( body, 0, GEngine->Physics->TWMinimalGlassBreakSpeed, GEngine->Physics->TWMinimalGlassBreakMass ) );
			}
			return;
		}

	int collisionGroup = HCGFixed;
	bool raycastDontCollide = false;
	if( strstr( mesh->Name.AsChar(), "barrier" ) )
	{
		if( strstr( mesh->Name.AsChar(), "monster" ) )
			collisionGroup = HCGMonsterBarrier;
		else
			collisionGroup = HCGBarrier;
		raycastDontCollide = true;
	}
	else if( strstr( mesh->Name.AsChar(), "water" ) )
		collisionGroup = HCGNoncolliding;
	else if( strstr( mesh->Name.AsChar(), "groundmesh" ) )
		collisionGroup = HCGFixedSpecial;

	if( strstr( mesh->Name.AsChar(), "notrace" ) )
		raycastDontCollide = true;

	hkRigidBody* rb = mHavokWorld->CreateStaticMesh( mesh, friction, restitution, collisionGroup );

	if( raycastDontCollide )
	{
		hkPropertyValue pv;
#ifdef HAVOK3
		pv.setInt(1);
#else
		pv.asInt = 1;
#endif
		rb->addProperty( HPRayCastDontCollide, pv );
	}
}

void PhysicsWorld::Start( bool loading )
{
	if( !mPaused ) return;
	if( loading )
	{
		mFirstTime = false;
		mPaused = false;
		return;
	}

	if( mFirstTime )
	{
	    Assert(mHavokWorld);

	    for( int i = 0; i < mHavokWorld->PhysicsObjects.Size(); ++i )
		{
			Assert( mHavokWorld->PhysicsObjects[i]->mHavokPart->Body->getSimulationIsland() );
#ifdef HAVOK3
			mHavokWorld->PhysicsObjects[i]->mHavokPart->Body->deactivate();
#else
			mHavokWorld->PhysicsObjects[i]->mHavokPart->Body->forceDeactivate();
#endif
		}
		mFirstTime = false;
	}
	mPaused = false;
}

void PhysicsWorld::WarmUp(float warmTime)
{
		m_setTag("Physics");
    // Let the rest of the objects fall and achieve appropriate positions
#ifdef HAVOK3
    for( int i = 0; i < warmTime * 60.0f; ++i )
	{
        mHavokWorld->World->stepDeltaTime( hkReal(1.0/60.0) );
#if defined( HAVOK3 ) && defined ( USE_HAVOK_DEBUGGER ) && !defined( TRANSGAMING )
		mHavokWorld->stepVisualDebugger();
#endif 
	}
#else
    for( int i = 0; i < warmTime * 60.0f; ++i )
        mHavokWorld->World->stepDeltaTime( hkReal(1.0/60.0) );
#endif
	mHavokWorld->RigidBodiesPosExceeded.ClearArray();
}

void PhysicsWorld::Pause()
{
	mPaused = true;
}

PhysicsWorld* PhysicsEngine::MakePhysicsWorld( World* world, float deactDelay, float deactMaxPosDiff )
{
    m_setTag("Physics");
	PhysicsWorld* phWorld = new PhysicsWorld(deactDelay,deactMaxPosDiff);
	phWorld->mEngineWorld = world;
	mPhysicsWorlds.Add( phWorld );
	return phWorld;
}

void PhysicsWorld::ReloadWorld( void )
{
	ReleaseStaticMeshes();

	GEngine->LoadingTick(0);

	// check directory for mopp streaming objects
#ifndef TRANSGAMING
	FileManagerStd fm;
	fm.CreateDirectory( "../Data/Maps/MOPPCode" );
#else
    TRACE("PhysicsWorld::ReloadWorld(): generating MOPPCodes...\n");
#endif

	bool multi = ( GEngine->Net != 0 );

	GEngine->LoadingTick(0);

	// add meshes to physics engine
	for( int i = 0; i < mEngineWorld->Meshes.Size(); ++i )
	{
		if( ( mEngineWorld->Meshes[i]->Flags & WorldMesh::ENoClip ) && !( mEngineWorld->Meshes[i]->Flags & WorldMesh::EWater ) )
			continue;
		if( multi && ( mEngineWorld->Meshes[i]->Name.Find( "decal" ) != NULL ) )
			continue;

		AddMesh( mEngineWorld->Meshes[i], DefaultMeshFriction, DefaultMeshRestitution );
	}

	GEngine->LoadingTick(0);

	mHavokWorld->ReorganizePinnedActiveMeshTable();
	mFirstTime = true;
	mPaused = true;
}

PhysicsWorld* PhysicsEngine::MakePhysicsWorldFromHKE( World* world, const char* fileName )
{
    m_setTag("Physics");
	PhysicsWorld* phWorld = new PhysicsWorld(5.0f,0.3f);
	phWorld->mEngineWorld = world;
	mPhysicsWorlds.Add( phWorld );
	phWorld->GetFromHKE( fileName );
	return phWorld;
}

void PhysicsWorld::GetFromHKE( const char* fileName )
{
#ifndef HAVOK3 // =========================================

	ReleaseStaticMeshes();

	hkMatrix3 scale3;
	scale3.setDiagonal(mEngineWorld->Scale,mEngineWorld->Scale,mEngineWorld->Scale);

	// setup toolkit
#ifdef HAVOK3
	hkWorldCinfo info;
	info.setupSolverInfo( hkWorldCinfo::SolverType::SOLVER_TYPE_4ITERS_MEDIUM );
	info.setBroadPhaseWorldSize( 3000.0f );
	info.m_collisionTolerance = 0.1f;
#else
	hkWorldConstructionInfo info;
	info.setupSolverInfo( HK_SOLVER_TYPE_4ITERS_MEDIUM );
	info.setBroadPhaseWorldSize( 3000.0f );
	info.m_collisionTolerance = 0.1f;
#endif
    m_setTag("Physics");
	hkWorld* tempWorld = new hkWorld( info );
    m_setTag("Physics");
	ExportWorld* exportWorld = new ExportWorld( tempWorld );
	hkIfstream is( fileName );

	if( is.isOk() )
	{
		//
		// Import the world
		//

		CheckLog log;
	
		NS_ExportStream::InExportStream instream( is );
		exportWorld->Import( instream, log );

		for( int i = 0; i < exportWorld->GetNumRigidBodies(); ++i )
		{
			ExportRigidBody* exportBody = exportWorld->GetRigidBody(i);
			const char *name = exportBody->GetName();
			ExportPrimitive* primitive = exportBody->GetPrimitive(0);

			// My own addititon!
#ifdef HAVOK3
			hkSimpleMeshShape* shape = primitive->GetHKShape();
			//hkShape* shape = primitive->GetHKShape();
			const hkArray<hkVector4>& verts = shape->m_vertices;
			const hkArray<hkGeometry::Triangle>& tris = shape->m_triangles;
#else
			hkStorageMeshShape* shape = primitive->GetHKSourceShape();

			const hkArray<hkVector4>& verts = shape->getVertices();
			const hkArray<hkStorageMeshShape::Triangle>& tris = shape->getTriangles();
#endif
			const int vertNum = verts.getSize();
			const int triangleNum = tris.getSize();

			// Creation of Convex Vertices Shape - for convex levels
/*
			// Prepare float table to give to ConvexVerticesShape - bug fix try
            m_setTag("Physics");
			float* table = new float[vertNum*3];
			for( int i = 0; i < vertNum; ++i )
			{
				table[3*i]   = verts[i](0)*mEngineWorld->Scale;
				table[3*i+1] = verts[i](1)*mEngineWorld->Scale;
				table[3*i+2] = verts[i](2)*mEngineWorld->Scale;
			}

			// Create internal shape
            m_setTag("Physics");
			hkConvexVerticesShape* vertShape = new hkConvexVerticesShape( table, 3*sizeof(float), vertNum );
			delete[] table;
*/
			// our hkStorageMeshShape creation
            m_setTag("Physics");
            m_setTag("Physics");
			hkStorageMeshShape* meshShape = new hkStorageMeshShape;
			meshShape->reserveMemory( vertNum, triangleNum );

			for( int i = 0; i < vertNum; ++i )
			{
				hkVector4 temp( verts[i] );
				temp.mul4( mEngineWorld->Scale );
				meshShape->addVertex( temp );
			}

			for( int i = 0; i < triangleNum; ++i )
#ifdef HAVOK3
				meshShape->addTriangle( tris[i].m_a, tris[i].m_b, tris[i].m_c );
#else
				meshShape->addTriangle( tris[i].m_a, tris[i].m_b, tris[i].m_c );
#endif
			meshShape->setRadius( 0.05f );

//			MOPP From here
			String moppPath = "../Data/Maps/MOPPCode/";
			String baseName( fileName );
			moppPath += String::BaseName( baseName );
			FileManagerStd fm;
			fm.CreateDirectory( moppPath.AsChar() );
			moppPath += '/';
			moppPath += name;
			char buffer[10];
			sprintf( buffer, "%d", vertNum );
			moppPath += buffer;
			moppPath += ".mopp";

			hkMoppCode* code = mHavokWorld->LoadMOPPCode( moppPath );
			if( !code )
			{
				hkMoppFitToleranceRequirements mft;
				mft.setAbsoluteFitToleranceOfTriangles(0.05f);
				code = hkMoppUtility::buildCode( meshShape ,mft );
				mHavokWorld->SaveMOPPCode( moppPath, code );
			}

            m_setTag("Physics");
			hkMoppBvTreeShape* moppShape = new hkMoppBvTreeShape( meshShape, code );
			code->removeReference();
			meshShape->removeReference();

//			MOPP To here

#ifdef HAVOK3
			hkRigidBodyCinfo rigidBodyInfo;
#else
			hkRigidBodyConstructionInfo rigidBodyInfo;
#endif
//			rigidBodyInfo.m_shape = vertShape;
			rigidBodyInfo.m_shape = moppShape;
#ifdef HAVOK3
			rigidBodyInfo.m_motionType = hkMotion::MOTION_FIXED;
#else
			rigidBodyInfo.m_motionType = HK_FIXED_MOTION;
#endif
			rigidBodyInfo.m_collisionFilterInfo = hkGroupFilter::calcFilterInfo( HCGFixed );
			rigidBodyInfo.m_mass = 10.0f;	// just in case

            m_setTag("Physics");
			hkRigidBody* rigidbody = new hkRigidBody( rigidBodyInfo );
			rigidbody->getMaterial().setRestitution( 0.5f );
			rigidbody->getMaterial().setFriction( 0.5f );
			hkVector4 rbPos = rigidbody->getPosition();
			rbPos.mul4( mEngineWorld->Scale );
#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
			if( !rbPos.isOk4() || ( fabs(rbPos(0)) > 10000.0f ) || ( fabs(rbPos(1)) > 10000.0f ) || ( fabs(rbPos(2)) > 10000.0f ) )
#else
			if( !rbPos.isOk() || ( fabs(rbPos(0)) > 10000.0f ) || ( fabs(rbPos(1)) > 10000.0f ) || ( fabs(rbPos(2)) > 10000.0f ) )
#endif // HAVOK3
				GLog.Print( "Position > 10000 in GetFromHKE\n" );
#endif
			rigidbody->setPosition( rbPos );
			mHavokWorld->World->addEntity( rigidbody );
//			vertShape->removeReference();
			moppShape->removeReference();
			// don't remove reference to rigid body, the system will destroy it then
			mHavokWorld->StaticMeshes.Add( rigidbody );
		}
	}
	mFirstTime = true;
	mPaused = true;

#else
	GLog.Print("PhysicsWorld::GetFromHKE() - FIXME!!!!!!!!!!!\n");
#endif // ifndef HAVOK3 =======================================
}

#ifdef TRANSGAMING
bool PhysicsWorld::Tick( float timeDelta )
#else
inline bool PhysicsWorld::Tick( float timeDelta )
#endif
{
	if( mPaused ) return false;
	Assert( mHavokWorld );
#ifdef	TRANSGAMING
	if( GEngine->GameState == PCFSystem::SingleGame )
#else
	if( GEngine->GameState == PCFSystem::PCFSystemStates::SingleGame )
#endif	
	{
		mHavokWorld->SingleTick( timeDelta );
		return true;
	}
	else
		return mHavokWorld->Tick( timeDelta );
}

#ifdef TRANSGAMING
void PhysicsWorld::UpdatePredictionTick( float timeDelta )
#else
inline void PhysicsWorld::UpdatePredictionTick( float timeDelta )
#endif
{
	Assert( mHavokWorld );
	mHavokWorld->UpdatePredictionTick( timeDelta );
}

#ifdef TRANSGAMING
void PhysicsWorld::UpdateEntities( DWORD msToAdd )
#else
inline void PhysicsWorld::UpdateEntities( DWORD msToAdd )
#endif
{
	Assert( mHavokWorld );
	mHavokWorld->UpdateEntities( msToAdd );
};


BOOL PhysicsWorld::LineTrace(const Vector& start, const Vector& end, HitData& hitData )
{
	GLog.Print("PhysicsWorld::LineTrace() - FIXME!!!!!!!!!!!\n");
	hkVector4 startWorld( start.X, start.Y, start.Z );
	hkVector4 endWorld( end.X, end.Y, end.Z );
#ifdef HAVOK3
	hkWorldRayCastInput ray;
	{
		ray.m_from = startWorld;
		ray.m_to = endWorld;
	}
	hkWorldRayCastOutput output;
	{
		havokFilteredRaycastCallback rayCallback( ray, &output );
		hkBroadPhase* broadPhase = mHavokWorld->World->getBroadPhase();
		hkBroadPhase::hkCastRayInput inputRay;
		inputRay.m_from = ray.m_from;
		inputRay.m_toBase = &ray.m_to;
		//inputRay.m_aabbCacheInfo = HK_NULL;
		broadPhase->castRay( inputRay, &rayCallback, hkSizeOf(havokFilteredRaycastCallback) );
	}
	const hkCollidable* coll = output.m_rootCollidable;

	if( coll != HK_NULL )
#else
	hkBroadPhase* broadPhase = mHavokWorld->World->getBroadPhase();
	hkShape::hkRayResults initResults;

	havokFilteredRaycastCallback rayCallBack( startWorld, endWorld, initResults );
	broadPhase->castPrimitive( startWorld, endWorld, rayCallBack );
	initResults = rayCallBack.getRayResults();
	hkCollidable* coll = rayCallBack.getHitObject();

	if( coll != HK_NULL )
#endif
	{
		hkVector4 intersectionPointWorld;
#ifdef HAVOK3
		intersectionPointWorld.setInterpolate4( startWorld, endWorld, output.m_hitFraction );
#else
		intersectionPointWorld.setInterpolate4( startWorld, endWorld, initResults.m_mindist );
#endif
		hitData.HitPoint = Vector( intersectionPointWorld(0), intersectionPointWorld(1), intersectionPointWorld(2) );

		hkVector4 temp;
		temp = startWorld;
		temp.sub4( intersectionPointWorld );
		hitData.Dist = temp.length3();

		hkVector4 hitNormal;
#ifdef HAVOK3
		hitNormal.setRotatedDir( ((hkRigidBody*)coll->getOwner())->getRotation(), output.m_normal );
#else
		hitNormal.setRotatedDir( ((hkRigidBody*)coll->getOwner())->getRotation(), initResults.m_unnormalizedHitNormal );
		hitNormal.normalize3();
#endif
		hitData.Normal = Vector( hitNormal(0), hitNormal(1), hitNormal(2) );

		hkEntity* ent = (hkEntity*)coll->getOwner();
		hitData.HavokEntity = ent;
		hkPropertyValue p = ent->getProperty( HPEntityPointer );
#ifdef HAVOK3
		Entity* entity = (Entity*)p.getPtr();
#else
		Entity* entity = (Entity*)p.asPtr;
#endif
		hitData.UserData = entity;

		return TRUE;

	}
	return FALSE;
}

BOOL PhysicsWorld::LineTraceHitPlayer(const Vector& start, const Vector& end, HitData& hitData )
{
	hkVector4 startWorld( start.X, start.Y, start.Z );
	hkVector4 endWorld( end.X, end.Y, end.Z );
#ifdef HAVOK3
	hkWorldRayCastInput ray;
	{
		ray.m_from = startWorld;
		ray.m_to = endWorld;
	}
	hkWorldRayCastOutput output;
	{
		havokFilteredRaycastSpecialCallback rayCallback( ray, &output );
		hkBroadPhase* broadPhase = mHavokWorld->World->getBroadPhase();
		hkBroadPhase::hkCastRayInput inputRay;
		inputRay.m_from = ray.m_from;
		inputRay.m_toBase = &ray.m_to;
		//inputRay.m_aabbCacheInfo = HK_NULL;
		broadPhase->castRay( inputRay, &rayCallback, hkSizeOf(havokFilteredRaycastSpecialCallback) );
	}
	const hkCollidable* coll = output.m_rootCollidable;
#else
	hkBroadPhase* broadPhase = mHavokWorld->World->getBroadPhase();
	hkShape::hkRayResults initResults;

	havokFilteredRaycastSpecialCallback rayCallBack( startWorld, endWorld, initResults );
	broadPhase->castPrimitive( startWorld, endWorld, rayCallBack );
	initResults = rayCallBack.getRayResults();
	hkCollidable* coll = rayCallBack.getHitObject();
#endif
	if( coll != HK_NULL )
	{
		hkVector4 intersectionPointWorld;
#ifdef HAVOK3
		intersectionPointWorld.setInterpolate4( startWorld, endWorld, output.m_hitFraction );
#else
		intersectionPointWorld.setInterpolate4( startWorld, endWorld, initResults.m_mindist );
#endif
		hitData.HitPoint = Vector( intersectionPointWorld(0), intersectionPointWorld(1), intersectionPointWorld(2) );

		hkVector4 temp;
		temp = startWorld;
		temp.sub4( intersectionPointWorld );
		hitData.Dist = temp.length3();

		hkVector4 hitNormal;
#ifdef HAVOK3
		hitNormal.setRotatedDir( ((hkRigidBody*)output.m_rootCollidable->getOwner())->getRotation(), output.m_normal );
#else
		hitNormal.setRotatedDir( ((hkRigidBody*)rayCallBack.getHitObject()->getOwner())->getRotation(), initResults.m_unnormalizedHitNormal );
		hitNormal.normalize3();
#endif
		hitData.Normal = Vector( hitNormal(0), hitNormal(1), hitNormal(2) );

		hkEntity* ent = (hkEntity*)coll->getOwner();
		hitData.HavokEntity = ent;
		hkPropertyValue p = ent->getProperty( HPEntityPointer );
#ifdef HAVOK3
		Entity* entity = (Entity*)p.getPtr();
#else
		Entity* entity = (Entity*)p.asPtr;
#endif
		hitData.UserData = entity;

		return TRUE;

	}
	return FALSE;
}

BOOL PhysicsWorld::LineTraceFirstHit(const Vector& start, const Vector& end )
{
	hkVector4 startWorld( start.X, start.Y, start.Z );
	hkVector4 endWorld( end.X, end.Y, end.Z );
	hkBroadPhase* broadPhase = mHavokWorld->World->getBroadPhase();
#ifdef HAVOK3
	hkWorldRayCastInput input;
	{
		input.m_from = endWorld;
		input.m_to = startWorld;
	}	
	hkWorldRayCastOutput output;
	{
		havokFirstHitRaycastCallback rayCallback( input, &output );
		hkBroadPhase* broadPhase = mHavokWorld->World->getBroadPhase();
		hkBroadPhase::hkCastRayInput broadRayInput;
		broadRayInput.m_from = input.m_from;
		broadRayInput.m_toBase = &input.m_to;

		// Cast the ray...
		broadPhase->castRay( broadRayInput, &rayCallback, hkSizeOf(havokFirstHitRaycastCallback) );
		return output.hasHit();
	}
#else
	hkShape::hkRayResults initResults;
	havokFirstHitRaycastCallback rayCallBack( startWorld, endWorld, initResults );
	broadPhase->castPrimitive( startWorld, endWorld, rayCallBack );
	return ( rayCallBack.getHitObject() != HK_NULL );
#endif
}

BOOL PhysicsWorld::LineTraceFixedGeom(const Vector& start, const Vector& end, HitData& hitData )
{
	hkVector4 startWorld( start.X, start.Y, start.Z );
	hkVector4 endWorld( end.X, end.Y, end.Z );
#ifdef HAVOK3
	hkWorldRayCastInput ray;
	{
		ray.m_from = startWorld;
		ray.m_to = endWorld;
	}
	hkWorldRayCastOutput output;
	{
		havokFixedGeometryRaycastCallback rayCallback( ray, &output );
		hkBroadPhase* broadPhase = mHavokWorld->World->getBroadPhase();
		hkBroadPhase::hkCastRayInput inputRay;
		inputRay.m_from = ray.m_from;
		inputRay.m_toBase = &ray.m_to;
		//inputRay.m_aabbCacheInfo = HK_NULL;
		broadPhase->castRay( inputRay, &rayCallback, hkSizeOf(havokFixedGeometryRaycastCallback) );
	}
	const hkCollidable* coll = output.m_rootCollidable;
	if ( coll != HK_NULL )
#else
	hkBroadPhase* broadPhase = mHavokWorld->World->getBroadPhase();
	hkShape::hkRayResults initResults;
	havokFixedGeometryRaycastCallback rayCallBack( startWorld, endWorld, initResults );
	broadPhase->castPrimitive( startWorld, endWorld, rayCallBack );
	initResults = rayCallBack.getRayResults();

	hkCollidable* coll;
	if( ( coll = rayCallBack.getHitObject() ) != HK_NULL )
#endif
	{
		hkVector4 intersectionPointWorld;
#ifdef HAVOK3
		intersectionPointWorld.setInterpolate4( startWorld, endWorld, output.m_hitFraction );
#else
		intersectionPointWorld.setInterpolate4( startWorld, endWorld, initResults.m_mindist );
#endif
		hitData.HitPoint = Vector( intersectionPointWorld(0), intersectionPointWorld(1), intersectionPointWorld(2) );

		hkVector4 temp;
		temp = startWorld;
		temp.sub4( intersectionPointWorld );
		hitData.Dist = temp.length3();

		hkVector4 hitNormal;
#ifdef HAVOK3
		hitNormal.setRotatedDir( ((hkRigidBody*)output.m_rootCollidable->getOwner())->getRotation(), output.m_normal );
#else
		hitNormal.setRotatedDir( ((hkRigidBody*)rayCallBack.getHitObject()->getOwner())->getRotation(), initResults.m_unnormalizedHitNormal );
		hitNormal.normalize3();
#endif
		hitData.Normal = Vector( hitNormal(0), hitNormal(1), hitNormal(2) );

		hkEntity* ent = (hkEntity*)coll->getOwner();
		hitData.HavokEntity = ent;
		hkPropertyValue p = ent->getProperty( HPEntityPointer );
#ifdef HAVOK3
		Entity* entity = (Entity*)p.getPtr();
#else
		Entity* entity = (Entity*)p.asPtr;
#endif
		hitData.UserData = entity;

		return TRUE;

	}
	return FALSE;
}

bool PhysicsWorld::CalculatePawnToPointVisibility( PhysicsObject* po, Vector& positionToSee )
{
	Assert( mEngineWorld );

	// Get the zone the point is in. If it's zero, portal culling won't occur.
	int ourZone = mEngineWorld->FindZone( positionToSee );
	Zone* ourZonePtr = NULL;
	if( ourZone != -1 )
		ourZonePtr = mEngineWorld->Zones[ourZone];

	const float shortenLines = 1.1f*po->BodyScale*OLD_PAWN_SCALE;
	const hkRigidBody* poBody = po->mHavokPart->Body;

	if( !poBody )
		return false;

	Vector endPos = po->GetPawnHeadPos();
	Vector posToSee = positionToSee;
	Vector dir = endPos - posToSee;
	float dist = dir.Magnitude();
	dir.Normalize();

	// Eliminating monsters too far to see point
	if( dist > po->mHowFarICanSee )
		return false;

	// Eliminating monsters with their back to point
   	const hkQuaternion& rotation = poBody->getRotation();
	hkVector4 endRot1( 0,0, -1 );
	hkVector4 endRot2;
	endRot2.setRotatedDir( rotation, endRot1 );
	Vector endRot( endRot2(0), endRot2(1), endRot2(2) );
	//endRot.Normalize();	//niepotrzebne

	if( endRot.Dot( dir ) < cos( F_PI/3 ) )
		return false;

	int zone = mEngineWorld->FindZone( endPos );
	Zone* zonePtr = NULL;
	if( zone != -1 )
		zonePtr = mEngineWorld->Zones[zone];

	// Check if the portals the line gets through lead to the zone the endPos is in
	if( ( ourZone != -1 ) && ( zone != -1 ) && ( zone != ourZone ) )
	{
		RaySegment ray( posToSee, dir );
		int counter = ResetWorldPortalsCounter();
		if( !VisibilityWalkPortals( ray , ourZonePtr, zonePtr ) )
			return false;

		SetWorldPortalsCounter( counter );
	};

	if( dist < 2*shortenLines )
	{
		return true;
	}
	else
	{
		// Shorten lines
		dir *= shortenLines;
		endPos -= dir;
		posToSee += dir;

		// Linetrace the line in Havok to find out.
		return !LineTraceFirstHit( posToSee, endPos );
	}
}

bool PhysicsWorld::CalculatePawnToEntityVisibility( PhysicsObject* po, PhysicsObject* entity )
{
	Vector posToSee = entity->GetPawnHeadPos();

	// Get the zone the point is in. If it's zero, portal culling won't occur.
	int ourZone = mEngineWorld->FindZone( posToSee );
	Zone* ourZonePtr = NULL;
	if( ourZone != -1 )
		ourZonePtr = mEngineWorld->Zones[ourZone];

	hkRigidBody* poBody = po->mHavokPart->Body;
	hkRigidBody* entBody = entity->mHavokPart->Body;

	if( !poBody || !entBody )
		return false;

	Vector endPos = po->GetPawnHeadPos();
	Vector dir = endPos - posToSee;
	float dist = dir.Magnitude();

	// Eliminating monsters too far to see point
	if( dist > po->mHowFarICanSee )
		return false;

	if (po->mViewAnglePitch < F_PI/2) {
		float x = (posToSee.X - endPos.X);
		float z = (posToSee.Z - endPos.Z);
		float pitch = fabs(atan2(posToSee.Y - endPos.Y, sqrt(x*x+z*z)));
		if (pitch > po->mViewAnglePitch)
			return false;
	}

	dir.Normalize();
	// Eliminating monsters with their back to point
   	const hkQuaternion& rotation = poBody->getRotation();
	hkVector4 endRot1( 0,0, -1 );
	hkVector4 endRot2;
	endRot2.setRotatedDir( rotation, endRot1 );
	Vector endRot( endRot2(0), endRot2(1), endRot2(2) );
	//endRot.Normalize();	//niepotrzebne

	if( po->mViewAngleYaw < F_PI/2 && dist > po->mHowFarICanSee360 ) 
		if( endRot.Dot( dir ) < cos(po->mViewAngleYaw) )
			return false;

	int zone = mEngineWorld->FindZone( endPos );
	Zone* zonePtr = NULL;
	if( zone != -1 )
		zonePtr = mEngineWorld->Zones[zone];

	// Check if the portals the line gets through lead to the zone the endPos is in
	if( ( ourZone != -1 ) && ( zone != -1 ) && ( zone != ourZone ) )
	{
		RaySegment ray( posToSee, dir );
		int counter = ResetWorldPortalsCounter();
		if( !VisibilityWalkPortals( ray , ourZonePtr, zonePtr ) )
			return false;

		SetWorldPortalsCounter( counter );
	};

	// Linetrace the line in Havok to find out.
	bool ok = !LineTraceFirstHit( posToSee, endPos );

	return ok;
}

bool PhysicsWorld::VisibilityWalkPortals( RaySegment& ray, Zone* zoneStart, Zone* zoneEnd )
{
	float dist;

	if( zoneStart == zoneEnd )
		return true;

	for( int i = 0; i < zoneStart->Portals.Size(); ++i )
	{
		if( !zoneStart->Portals[i]->VisCounter )	// portal not visited yet
		{
			zoneStart->Portals[i]->VisCounter = 1;
			Portal* por = zoneStart->Portals[i];
			ConvexPolygon& p = por->Poly;
			if( ray.IntersectConvexPolygon( p, dist, 0 ) )
			{
				for( int j = 0; j < por->Zones.Size(); ++j )
				{
					Zone* otherZone = por->Zones[j];
					if( ( otherZone != zoneStart ) && ( VisibilityWalkPortals( ray, otherZone, zoneEnd ) ) )
					{
						return true;
					};
				};
			};
		};
	};
	return false;
}

int PhysicsWorld::ResetWorldPortalsCounter( void )
{
	if( !mEngineWorld->Portals.Size() )
		return 0;

	int c = mEngineWorld->Portals[0]->VisCounter;
	for( int i = 0; i < mEngineWorld->Portals.Size(); ++i )
		mEngineWorld->Portals[i]->VisCounter = 0;
	return c;
}

void PhysicsWorld::SetWorldPortalsCounter( int counter )
{
	for( int i = 0; i < mEngineWorld->Portals.Size(); ++i )
		mEngineWorld->Portals[i]->VisCounter = counter;
}

// Region methods

void* PhysicsWorld::CreateRegionFromPoints( DynamicArray<Vector>& points, void* userInfo, float mass )
{
#ifndef HAVOK3 // =============================================================
	if( !userInfo || ( points.Size() < 4 ) )
		return NULL;

	int size = points.Size();

	// Geometry tests
    m_setTag("Physics");
	hkVector4* verts = new hkVector4[size];
	for( int i = 0; i < size; ++i )
		verts[i].set( points[i].X, points[i].Y, points[i].Z );

#ifndef HAVOK3
	hkGeometry geom;
	geom.setVertices( verts, size );

	// BLAIR - Looks like they took this out...
	hkGeometryCheckerDetails dets;
	dets.init();

	if( !hkGeometryChecker::atLeastFourVertices( &geom, dets, true ) )
		return NULL;
	if( !hkGeometryChecker::notCoplanar( &geom, dets, true ) )
		return NULL;
#endif

	// Prepare float table to give to ConvexVerticesShape - bug fix try
    m_setTag("Physics");
	float* table = new float[size*3];
	for( int i = 0; i < size; ++i )
	{
		table[3*i]   = points[i].X;
		table[3*i+1] = points[i].Y;
		table[3*i+2] = points[i].Z;
	}

	// Create internal shape

	hkConvexVerticesShape* vertShape;
	hkArray<hkVector4> planeEquations;
	hkArray<hkVector4> vertsOut;

#ifdef LOG_CONVEX_CREATION
# ifdef TRANSGAMING
	float  firstTime = GEngine->SysDriver.GetCurrentTime();
# else
	double firstTime = GEngine->SysDriver.GetCurrentTime();
# endif
	GLog.Print( "Creating region from vertices" );
	GLog.Flush();
#endif

	hkGeometryUtil::computePlanesFromVertices( table, size, 3*sizeof(float), planeEquations, vertsOut );

#ifdef LOG_CONVEX_CREATION
	GLog.Print( ", duration: %f\n", GEngine->SysDriver.GetCurrentTime()-firstTime );
	GLog.Flush();
#endif

    if( planeEquations.getSize() >= 4 ){
        m_setTag("Physics");
		vertShape = new hkConvexVerticesShape( table, 3*sizeof(float), size, planeEquations );
    }
	else
	{
		delete[] table;
		return NULL;
	}

	delete[] table;

	// Create child shape
    m_setTag("Physics");
	havokRegion* phantom = new havokRegion( (Entity*)userInfo );

	// Create bv shape
    m_setTag("Physics");
	hkBvShape* bvShape = new hkBvShape( vertShape, phantom );
	vertShape->removeReference();
	phantom->removeReference();

#ifdef HAVOK3
	hkRigidBodyCinfo rigidBodyInfo;
#else
	hkRigidBodyConstructionInfo rigidBodyInfo;
#endif
	rigidBodyInfo.m_shape = bvShape;
	rigidBodyInfo.m_position.set(0.0f, 0.0f, 0.0f);
#ifdef HAVOK3 
	rigidBodyInfo.m_motionType = hkMotion::MOTION_BOX_INERTIA;
#else
	rigidBodyInfo.m_motionType = HK_BOX_INERTIA_MOTION;
#endif
	rigidBodyInfo.m_collisionFilterInfo = hkGroupFilter::calcFilterInfo( HCGPhantom );

	// Calculate inertia tensor
	hkMassProperties massProperties;
	hkInertiaTensorComputer::computeVertexHullVolumeMassProperties( (float*)&points[0], 12, points.Size(), mass, massProperties );
	rigidBodyInfo.m_inertiaTensor = massProperties.m_inertiaTensor;
	rigidBodyInfo.m_centerOfMass = massProperties.m_centerOfMass;
	rigidBodyInfo.m_mass = mass;
#ifdef HAVOK_POSITION_BUG_TRACKING
	Assert( mass >= 0.0f );
#endif

    m_setTag("Physics");
	hkRigidBody* rigidbody = new hkRigidBody( rigidBodyInfo );
	bvShape->removeReference();

	// Set params
	rigidbody->getMaterial().setRestitution(0.001f);
	rigidbody->getMaterial().setFriction(0);

	hkPropertyValue pv;
#ifdef HAVOK3
	pv.setPtr(userInfo);
#else
	pv.asPtr = userInfo;
#endif
	rigidbody->addProperty( HPEntityPointer, pv );

#ifdef HAVOK3
	pv.setInt(1);
#else
	pv.asInt = 1;
#endif
	rigidbody->addProperty( HPRayCastDontCollide, pv );

#ifdef HAVOK3
	rigidbody->getMaterial().m_primaryResponseType = hkMaterial::RESPONSE_NONE;
#else
	rigidbody->getMaterial().m_primaryResponseType = hkMaterial::hkResponseType::HK_NO_RESPONSE;
#endif

	mHavokWorld->World->addEntity( rigidbody );
#ifdef HAVOK3
	rigidbody->setMotionType(hkMotion::MOTION_KEYFRAMED);
#else
	rigidbody->setKeyframed(true);
#endif
	return rigidbody;

#else
	GLog.Print("PhysicsWorld::CreateRegionFromPoints() - FIXME!!!!!!!!!!!\n");
	return HK_NULL;
#endif // ifndef HAVOK3 ============================================
}

void PhysicsWorld::DestroyRegion( void* regionHandle )
{
	hkRigidBody* phantom = (hkRigidBody*)regionHandle;
	if( phantom->getSimulationIsland() )
		mHavokWorld->World->removeEntity( phantom );
	else
		GLog.Print( "Warning: tried to remove phantom that wasn't in the world\n" );
	phantom->removeReference();
}

void PhysicsWorld::SetRegionPos( void* regionHandle, const Vector& pos )
{
	hkRigidBody* phantom = (hkRigidBody*)regionHandle;
	hkVector4 pos1( pos.X, pos.Y, pos.Z );
#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
	if( !pos1.isOk4() || ( fabs(pos1(0)) > 10000.0f ) || ( fabs(pos1(1)) > 10000.0f ) || ( fabs(pos1(2)) > 10000.0f ) )
#else
	if( !pos1.isOk() || ( fabs(pos1(0)) > 10000.0f ) || ( fabs(pos1(1)) > 10000.0f ) || ( fabs(pos1(2)) > 10000.0f ) )
#endif // HAVOK3
		GLog.Print( "Position > 10000 in SetRegionPos\n" );
#endif
	phantom->setPosition( pos1 );
}

Vector PhysicsWorld::GetRegionPos( void* regionHandle )
{
	hkRigidBody* phantom = (hkRigidBody*)regionHandle;
	hkVector4 pos = phantom->getPosition();
	return Vector( pos(0), pos(1), pos(2) );
}

void PhysicsWorld::SelfExplosion( Vector center, float strength, float radius, DynamicArray<PhysicsObject*>* targetObjects )
{
	hkVector4 havokCenter( center.X, center.Y, center.Z );
	for( int i = 0; i < targetObjects->Size(); i++ )
	{
		PhysicsObject* po = (*targetObjects)[i];
		hkVector4 pawnCenter = po->mHavokPart->Body->getCenterOfMassInWorld();
		hkVector4 distanceV( pawnCenter );
		distanceV.sub4( havokCenter );
		if( distanceV.length3() < radius )
		{
			const float gain = 1.0f;	// scaling factor
#ifdef	TRANSGAMING_PS2
			float str = strength * ( 1 - hkReal( distanceV.length3() ) / radius ) * gain;
#else
			//float str = strength * ( 1 - distanceV.length3() / radius ) * gain;
			float str = strength * ( 1 - hkReal( distanceV.length3() ) / radius ) * gain;
#endif
			if( distanceV.length3() > 0.001 )
			{
				distanceV.normalize3();
				hkVector4 imp( distanceV );
				imp.mul4( str );

				Vector force( imp(0), imp(1), imp(2) );
				Vector pCenter( pawnCenter(0), pawnCenter(1), pawnCenter(2) );
				po->EffectForce( force, &pCenter );
			}
		}
	}
}

#ifdef TRANSGAMING
void PhysicsWorld::Explosion( Vector center, float strength, float range, int ownerID, int weapon_type, float weapon_damage )
#else
inline void PhysicsWorld::Explosion( Vector center, float strength, float range, int ownerID, int weapon_type, float weapon_damage )
#endif
{
	mHavokWorld->Explosion( center, strength, range, ownerID, weapon_type, weapon_damage );
}

#ifdef TRANSGAMING
void PhysicsWorld::MultiplayerExplosion( Vector center, float strength, float range, int ownerID, int weapon_type, float weapon_damage, float factorY )
#else
inline void PhysicsWorld::MultiplayerExplosion( Vector center, float strength, float range, int ownerID, int weapon_type, float weapon_damage, float factorY )
#endif
{
	mHavokWorld->MultiplayerExplosion( center, strength, range, ownerID, weapon_type, weapon_damage, factorY );
}

void PhysicsWorld::ExplosionUp( Vector center, float strength, float radius, float maxVelocity, float velocityVariation )
{
	hkVector4 havokCenter( center.X, center.Y, center.Z );

	for( int i = 0; i < mHavokWorld->PhysicsObjects.Size(); i++ )
	{
		PhysicsObject* po = mHavokWorld->PhysicsObjects[i];
		if( ( !po->mHavokPart->Body ) || ( po->State & EPOExplosionsDontWorkOnIt ) || !po->mHavokPart->Body->getSimulationIsland() )
			continue;

		hkVector4 pawnCenter = po->mHavokPart->Body->getCenterOfMassInWorld();
		hkVector4 distanceV( pawnCenter );
		distanceV.sub4( havokCenter );
		if( distanceV.length3() < radius )
		{
			hkVector4 v = po->mHavokPart->Body->getLinearVelocity();
			if (v(1) < 0.5f && v(1) > -0.5f && !po->IsPlayer()) {
#ifdef	TRANSGAMING_PS2
				float str = strength * ( 1 - hkReal( distanceV.length3() )/ radius );
#else
				float str = strength * ( 1 - hkReal( distanceV.length3() )/ radius );
				//float str = strength * ( 1 - distanceV.length3() / radius );
#endif

				if( str > maxVelocity )
					str = maxVelocity;

				float velVar = velocityVariation*(2*(rand()/(float)RAND_MAX)-1);

				str *= (1+velVar);

				hkVector4 up( 0, 1, 0 );

				hkVector4 upVar( 0.05f*(2*(rand()/(float)RAND_MAX)-1), 0, 0.05f*(2*(rand()/(float)RAND_MAX)-1) );
				
				up.add4( upVar );

				up.mul4( str );
				if( !po->mHavokPart->Body->isActive() )
#ifdef HAVOK3
					po->mHavokPart->Body->activate();
#else
					po->mHavokPart->Body->forceActivate();
#endif

#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
				if( !up.isOk4() || ( fabs(up(0)) > 1000.0f ) || ( fabs(up(1)) > 1000.0f ) || ( fabs(up(2)) > 1000.0f ) )
#else
				if( !up.isOk() || ( fabs(up(0)) > 1000.0f ) || ( fabs(up(1)) > 1000.0f ) || ( fabs(up(2)) > 1000.0f ) )
#endif
					GLog.Print( "Velocity > 1000 in ExplosionUp\n" );
#endif
				po->mHavokPart->Body->setLinearVelocity( up );

				hkVector4 rot;

				rot.set( 2*(rand()/(float)RAND_MAX)-1, 2*(rand()/(float)RAND_MAX)-1, 2*(rand()/(float)RAND_MAX)-1 );

#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
				if( !rot.isOk4() || ( fabs(rot(0)) > 100.0f ) || ( fabs(rot(1)) > 100.0f ) || ( fabs(rot(2)) > 100.0f ) )
#else
				if( !rot.isOk() || ( fabs(rot(0)) > 100.0f ) || ( fabs(rot(1)) > 100.0f ) || ( fabs(rot(2)) > 100.0f ) )
#endif
					GLog.Print( "Angular Velocity > 100 in ExplosionUp\n" );
#endif

				po->mHavokPart->Body->setAngularVelocity( rot );
			}
		}
	}
}


int PhysicsWorld::SetCollisionToAll(bool enable, float minimalDelay, float minimalStrength, float minimalMass, float maximalMass, float minProbability, int activeMeshGroup )
{
    //TRACE("PhysicsWorld::SetCollisionToAll(): this = %p, mHavokWorld = %p, count = %d\n", this, mHavokWorld, mHavokWorld->PhysicsObjects.Size());
	int count = 0;
	for( int i = 0; i < mHavokWorld->PhysicsObjects.Size(); i++ )
	{
		PhysicsObject* po = mHavokWorld->PhysicsObjects[i];
		if( !po->mHavokPart->Body->getSimulationIsland() ) continue;

		if( activeMeshGroup != -1 )
		{
			Entity* ent = po->GetEntity();
			if( ent && ent->AsMesh() && ( ent->AsMesh()->ActiveGroup != activeMeshGroup ) )
				continue;
		}

		float mass = po->mHavokPart->Body->getMass();
#ifdef HAVOK_POSITION_BUG_TRACKING
		Assert( mass >= 0.0f );
#endif
		if( mass < maximalMass && mass > minimalMass )
		{
			if (rand()%100 < (minProbability + mass / maximalMass * minProbability))
			{
				count++;
				po->SetCollisionCallbacks(enable, minimalDelay, minimalStrength );
			}
		}
	}

	for( int i = 0; i < mHavokWorld->PinnedActiveMeshes.Size(); ++i )
	{
		PinnedActiveMesh& pam = mHavokWorld->PinnedActiveMeshes[i];
		Assert( pam.StaticMesh );

		if( activeMeshGroup != -1 )
			if( pam.StaticMesh->ActiveGroup != activeMeshGroup )
				continue;

		if( pam.ActiveMesh )
		{
			float mass = pam.ActiveMesh->mHavokPart->Body->getMass();
#ifdef HAVOK_POSITION_BUG_TRACKING
			Assert( mass >= 0.0f );
#endif
			if( mass < maximalMass && mass > minimalMass )
			{
				if( rand()%100 < ( minProbability + mass / maximalMass * minProbability ) )
				{
					count++;
					pam.ActiveMesh->SetCollisionCallbacks(enable, minimalDelay, minimalStrength );
				}
			}
		}
		else
		{
			for( int j = 0; j < pam.MeshParts.Size(); ++j )
			{
				if( !pam.MeshParts[j]->PO ) continue;

				float mass = pam.MeshParts[j]->PO->mHavokPart->Body->getMass();
#ifdef HAVOK_POSITION_BUG_TRACKING
				Assert( mass >= 0.0f );
#endif
				if( mass < maximalMass && mass > minimalMass )
				{
					if( rand()%100 < ( minProbability + mass / maximalMass * minProbability ) )
					{
						count++;
						pam.MeshParts[j]->PO->SetCollisionCallbacks(enable, minimalDelay, minimalStrength );
					}
				}
			}
		}
	}
	return count;
}

bool PhysicsWorld::EnableCollisionToMesh(bool enable, const char* name, float minimalDelay, float minimalStrength)
{
	for( int i = 0; i < mHavokWorld->PhysicsObjects.Size(); ++i )
	{
		PhysicsObject* po = mHavokWorld->PhysicsObjects[i];
		Assert( po->mHavokPart->Body->getSimulationIsland() );
		Entity* ent = po->GetEntity();

		if(ent && ent->Name == name)
		{
			po->SetCollisionCallbacks(enable, minimalDelay, minimalStrength );
			return true;
		}
	}

	for( int i = 0; i < mHavokWorld->PinnedActiveMeshes.Size(); ++i )
	{
		PinnedActiveMesh& pam = mHavokWorld->PinnedActiveMeshes[i];
		Assert( pam.StaticMesh );
		if( pam.StaticMesh->Name == name )
		{
			if( pam.ActiveMesh )
				pam.ActiveMesh->SetCollisionCallbacks( enable, minimalDelay, minimalStrength );
			else
				for( int j = 0; j < pam.MeshParts.Size(); ++j )
					if( pam.MeshParts[j]->PO )
						pam.MeshParts[j]->PO->SetCollisionCallbacks( enable, minimalDelay, minimalStrength );
			return true;
		}
	}

	return false;
}


void PhysicsWorld::ExplosionParabolic( Vector center, float flightTime, float radius, Vector target )
{
	hkVector4 havokCenter( center.X, center.Y, center.Z );

	for( int i = 0; i < mHavokWorld->PhysicsObjects.Size(); i++ )
	{
		PhysicsObject* po = mHavokWorld->PhysicsObjects[i];
		if( ( !po->mHavokPart->Body ) || ( po->State & EPOExplosionsDontWorkOnIt ) || !po->mHavokPart->Body->getSimulationIsland() )
			continue;

		hkVector4 pawnCenter = po->mHavokPart->Body->getCenterOfMassInWorld();
		hkVector4 distanceV( pawnCenter );
		distanceV.sub4( havokCenter );
		if( distanceV.length3() < radius )
		{
			hkVector4 v = po->mHavokPart->Body->getLinearVelocity();
			if (v(1) < 0.5f && v(1) > -0.5f && !po->IsPlayer()) {

				hkVector4 targetVel;

				hkVector4 gravity = mHavokWorld->World->getGravity();

				targetVel(0) = ( target.X - pawnCenter(0) )*1.25f/flightTime;
				targetVel(1) = ( target.Y - pawnCenter(1) )/flightTime - (gravity(1)/2)*flightTime;
				targetVel(2) = ( target.Z - pawnCenter(2) )*1.25f/flightTime;

				if( !po->mHavokPart->Body->isActive() )
#ifdef HAVOK3
					po->mHavokPart->Body->activate();
#else
					po->mHavokPart->Body->forceActivate();
#endif
				float rnd = (rand()/(float)RAND_MAX * 0.15f);
				targetVel(1) *= (1.0f + rnd);
#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
				if( !targetVel.isOk4() || ( fabs(targetVel(0)) > 1000.0f ) || ( fabs(targetVel(1)) > 1000.0f ) || ( fabs(targetVel(2)) > 1000.0f ) )
#else
				if( !targetVel.isOk() || ( fabs(targetVel(0)) > 1000.0f ) || ( fabs(targetVel(1)) > 1000.0f ) || ( fabs(targetVel(2)) > 1000.0f ) )
#endif
					GLog.Print( "Velocity > 1000 in ExplosionParabolic\n" );
#endif
				po->mHavokPart->Body->setLinearVelocity( targetVel );

				hkVector4 rot;
				rot.set( 2*(rand()/(float)RAND_MAX)-1, 2*(rand()/(float)RAND_MAX)-1, 2*(rand()/(float)RAND_MAX)-1 );

#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
				if( !rot.isOk4() || ( fabs(rot(0)) > 100.0f ) || ( fabs(rot(1)) > 100.0f ) || ( fabs(rot(2)) > 100.0f ) )
#else
				if( !rot.isOk() || ( fabs(rot(0)) > 100.0f ) || ( fabs(rot(1)) > 100.0f ) || ( fabs(rot(2)) > 100.0f ) )
#endif
					GLog.Print( "Angular Velocity > 100 in ExplosionParabolic\n" );
#endif

				po->mHavokPart->Body->setAngularVelocity( rot );
			}
		}
	}
}

void PhysicsWorld::ReleaseStaticMeshes( void )
{
	// Release static meshes
	for( int i = 0; i < mHavokWorld->StaticMeshes.Size(); ++i )
	{
		if( mHavokWorld->StaticMeshes[i]->getSimulationIsland() )
			mHavokWorld->World->removeEntity( mHavokWorld->StaticMeshes[i] );
		mHavokWorld->StaticMeshes[i]->removeReference();
	}
	mHavokWorld->StaticMeshes.ClearArray();

	// Release active meshes
	mHavokWorld->PhysArray.ClearArray();
	mHavokWorld->PhysIndices.Clear();
}

void PhysicsWorld::StaticMeshesEnableByGroup( BYTE group, bool enable )
{
	if( enable )
	{
		for( int i = 0; i < mHavokWorld->StaticMeshes.Size(); ++i )
		{
			if( mHavokWorld->StaticMeshes[i]->getSimulationIsland() ) continue;
			Assert( mHavokWorld->StaticMeshes[i]->hasProperty( HPEntityPointer ) );
			Assert( mHavokWorld->StaticMeshes[i]->isFixed() );
#ifdef HAVOK3
			WorldMesh* wm = (WorldMesh*)mHavokWorld->StaticMeshes[i]->getProperty( HPEntityPointer ).getPtr();
#else
			WorldMesh* wm = (WorldMesh*)mHavokWorld->StaticMeshes[i]->getProperty( HPEntityPointer ).asPtr;
#endif
			if( wm->ActiveGroup == group )
			{
				mHavokWorld->World->addEntity( mHavokWorld->StaticMeshes[i] );
				wm->Flags &= ~Entity::EEntNoDraw;
			}
		}
	}
	else
	{
		for( int i = 0; i < mHavokWorld->StaticMeshes.Size(); ++i )
		{
			if( !mHavokWorld->StaticMeshes[i]->getSimulationIsland() ) continue;
			Assert( mHavokWorld->StaticMeshes[i]->hasProperty( HPEntityPointer ) );
			Assert( mHavokWorld->StaticMeshes[i]->isFixed() );
#ifdef HAVOK3
			WorldMesh* wm = (WorldMesh*)mHavokWorld->StaticMeshes[i]->getProperty( HPEntityPointer ).getPtr();
#else
			WorldMesh* wm = (WorldMesh*)mHavokWorld->StaticMeshes[i]->getProperty( HPEntityPointer ).asPtr;
#endif
			if( wm->ActiveGroup == group )
			{
				mHavokWorld->World->removeEntity( mHavokWorld->StaticMeshes[i] );
				wm->Flags |= Entity::EEntNoDraw;
			}
		}
	}
}


void PhysicsWorld::StaticMeshEnable( Entity *entity, bool enable)
{
	Assert( mHavokWorld );

	hkRigidBody* hmesh = (hkRigidBody*) mHavokWorld->GetStaticMesh( entity, false );
	if ( hmesh )
	{
		if ( enable )
		{
			if ( !hmesh->getSimulationIsland() )
				mHavokWorld->World->addEntity( hmesh );
		}
		else
		{
			if ( hmesh->getSimulationIsland() )
				mHavokWorld->World->removeEntity( hmesh );
		};
	}
}

#ifdef TRANSGAMING
void PhysicsWorld::EnableUnderwaterWorld( bool enable )
#else
inline void PhysicsWorld::EnableUnderwaterWorld( bool enable )
#endif
{
	Assert( mHavokWorld );
	mHavokWorld->EnableUnderwaterWorld( enable );
}

#ifdef TRANSGAMING
bool PhysicsWorld::IsUnderwaterWorld( void )
#else
inline bool PhysicsWorld::IsUnderwaterWorld( void )
#endif
{
	Assert( mHavokWorld );
	return mHavokWorld->IsUnderwater();
}
/*
void* PhysicsWorld::CreateRigidConstraint( Vector pivot, Model* model, int joint, PhysicsObject* po )
{
	Ragdoll* ragdoll = model->RagdollPtr;
	if( !ragdoll ) return 0;
	return CreateRigidConstraint( pivot, ragdoll->havokRagdoll->Limbs[joint].Body, po->mHavokPart->Body );
}

void* PhysicsWorld::CreateRigidConstraint( Vector pivot, PhysicsObject* po1, PhysicsObject* po2 )
{
	return CreateRigidConstraint( pivot, po1->mHavokPart->Body, po2->mHavokPart->Body );
}

void* PhysicsWorld::CreateRigidConstraint( Vector pivot, Model* model1, int joint1, Model* model2, int joint2 )
{
	Ragdoll* ragdoll1 = model1->RagdollPtr;
	if( !ragdoll1 ) return 0;
	Ragdoll* ragdoll2 = model2->RagdollPtr;
	if( !ragdoll2 ) return 0;
	return CreateRigidConstraint( pivot, ragdoll1->havokRagdoll->Limbs[joint1].Body, ragdoll2->havokRagdoll->Limbs[joint2].Body );
}

void* PhysicsWorld::CreateRigidConstraint( Vector pivot, void* hpo1, void* hpo2 )
{
	hkRigidBody* rb1 = (hkRigidBody*)hpo1;
	hkRigidBody* rb2 = hpo2?(hkRigidBody*)hpo2:mHavokWorld->World->getFixedRigidBody();
	Assert( rb1 && rb2 );

	// Set the pivot
	hkVector4 myPivot;
	myPivot.set( pivot.X, pivot.Y, pivot.Z );

	// Create constraint
	hkReal tau = 1.0f;
	hkReal damping = 1.0f;

	hkMatrix3 matrix;
	matrix.setIdentity();

    m_setTag("Physics");
	hkGenericConstraint* gen = new hkGenericConstraint( rb1, rb2 );
	hkConstraintConstructionKit kit;
	kit.begin( gen );
	kit.setStiffnessAndDamping( tau, damping );
	kit.setPivotsHelper( myPivot );
	kit.constrainAllLinearDof();
	kit.setAngularBasisA( matrix );
	kit.setAngularBasisB( matrix );
	kit.constrainAllAngularDof();
	kit.restoreStiffnessAndDamping();
	kit.end();

	mHavokWorld->World->addConstraint( gen );
	return gen;
}

void PhysicsWorld::RemoveRigidConstraint( void* constraint )
{
	mHavokWorld->World->removeConstraint( (hkConstraint*)constraint );
	((hkConstraint*)constraint)->removeReference();
}
*/

float PhysicsWorld::ClosestPointsDistanceBetween( PhysicsObject* po1, PhysicsObject* po2 )
{
	hkRigidBody* rb1 = po1->mHavokPart->Body;
	hkRigidBody* rb2 = po2->mHavokPart->Body;
	Assert( rb1 && rb2 );

#ifndef HAVOK3 // =================================================
	hkCollidable& coll1 = rb1->getCollidable();
	hkCollidable& coll2 = rb2->getCollidable();
	hkCollisionEnvironment* env = mHavokWorld->World->getCollisionEnvironment();
    m_setTag("Physics");
	hkSimpleClosestPointsDetails* details = new hkSimpleClosestPointsDetails( coll1, coll2, *env );
	float dist = details->getClosestDistance();
	details->destroy( *env );
	return dist;
#else
	GLog.Print("PhysicsWorld::ClosestPointsDistanceBetween() - FIXME!!!!!!!!!!!\n");
	return 0.0f;
#endif // ifndef HAVOK3 =============================================
}
/*
float PhysicsWorld::ClosestPointsDistanceBetween( PhysicsObject* po1, Model* model2, int joint2 )
{
	Ragdoll* ragdoll = model2->RagdollPtr;
	if( !ragdoll ) return 0;
	hkRigidBody* rb2 = ragdoll->havokRagdoll->Limbs[joint2].Body;

	hkRigidBody* rb1 = po1->mHavokPart->Body;
	Assert( rb1 && rb2 );

	hkCollidable& coll1 = rb1->getCollidable();
	hkCollidable& coll2 = rb2->getCollidable();
	hkCollisionEnvironment* env = mHavokWorld->World->getCollisionEnvironment();
    m_setTag("Physics");
	hkSimpleClosestPointsDetails* details = new hkSimpleClosestPointsDetails( coll1, coll2, *env );
	float dist = details->getClosestDistance();
	details->destroy( *env );
	return dist;
}

float PhysicsWorld::ClosestPointsDistanceBetween( Model* model1, int joint1, Model* model2, int joint2 )
{
	Ragdoll* ragdoll1 = model1->RagdollPtr;
	if( !ragdoll1 ) return 0;
	hkRigidBody* rb1 = ragdoll1->havokRagdoll->Limbs[joint1].Body;

	Ragdoll* ragdoll2 = model2->RagdollPtr;
	if( !ragdoll2 ) return 0;
	hkRigidBody* rb2 = ragdoll2->havokRagdoll->Limbs[joint2].Body;
	Assert( rb1 && rb2 );

	hkCollidable& coll1 = rb1->getCollidable();
	hkCollidable& coll2 = rb2->getCollidable();
	hkCollisionEnvironment* env = mHavokWorld->World->getCollisionEnvironment();
    m_setTag("Physics");
	hkSimpleClosestPointsDetails* details = new hkSimpleClosestPointsDetails( coll1, coll2, *env );
	float dist = details->getClosestDistance();
	details->destroy( *env );
	return dist;
}
*/
#ifdef TRANSGAMING
bool PhysicsWorld::CheckStartGlass( void* body, Vector hitPoint, float activationRadius, Vector velocityToSet )
#else
inline bool PhysicsWorld::CheckStartGlass( void* body, Vector hitPoint, float activationRadius, Vector velocityToSet )
#endif
{
	return mHavokWorld->CheckStartGlass( (hkRigidBody*)body, hitPoint, activationRadius, velocityToSet );
}

void PhysicsWorld::GetIntersectingEntities( BoundingBox& box, DynamicArray<Entity*>& entityList, int collisionGroup )
{
	hkBroadPhase* broadPhase = mHavokWorld->World->getBroadPhase();

	hkVector4 minP( box.Min.X, box.Min.Y, box.Min.Z );
	hkVector4 maxP( box.Max.X, box.Max.Y, box.Max.Z );

	hkAabb boundingBox( minP, maxP );
	hkArray<hkBroadPhaseHandlePair> pairs;

	hkUint32 ourCollisionFilterInfo = 0;
	if( ourCollisionFilterInfo != -1 )
		ourCollisionFilterInfo = hkGroupFilter::calcFilterInfo( collisionGroup );

	broadPhase->querySingleAabb( boundingBox, pairs );

	for( int count = 0; count < pairs.getSize(); ++count )
	{
		// NOTE: hkBroadPhaseHandlePair.m_a will always be NULL
#ifdef HAVOK3
		hkCollidable *coll = (hkCollidable*)( pairs[count].m_b );
#else
		hkCollidable *coll = static_cast<hkCollidable*>( pairs[count].m_b );
#endif
		if( ( collisionGroup != -1 ) && ( coll->getCollisionFilterInfo() != ourCollisionFilterInfo ) )
			continue;

		// it could be a HK_BROAD_PHASE_PHANTOM...
#ifdef HAVOK3
		if( coll->getType() == hkWorldObject::BROAD_PHASE_ENTITY )
#else
		if( coll->getType() == HK_BROAD_PHASE_ENTITY )
#endif
		{
#ifdef HAVOK3
			hkEntity* entity = reinterpret_cast<hkEntity*>( coll->getOwner() );
#else
			hkEntity* entity = reinterpret_cast<hkEntity*>( coll->m_owner );
#endif
			if( entity->hasProperty( HPEntityPointer ) )
			{
				hkPropertyValue p = entity->getProperty( HPEntityPointer );
#ifdef HAVOK3
				entityList.Add( (Entity*)p.getPtr() );
#else
				entityList.Add( (Entity*)p.asPtr );
#endif
			}
		}
	}
}

Entity* PhysicsWorld::GetEntityByActiveMeshIndex( short index )
{
	Assert( mHavokWorld->PhysArray.Size() > index );
	return mHavokWorld->PhysArray[index]->GetEntity();
}

void PhysicsWorld::RemoveStaticMesh( void* mesh )
{
	mHavokWorld->RemoveStaticMesh( (hkRigidBody*)mesh );
}

PhysicsWorld* PhysicsEngine::GetWorldFromRigidBody( void* rb )
{
	hkRigidBody* rigidbody = (hkRigidBody*)rb;
#ifdef HAVOK3
	hkWorld* hkworld = (hkWorld*)rigidbody->getWorld();
#else
	hkWorld* hkworld = rigidbody->getOwner();
#endif
	if( !hkworld ) return 0;
	for( int i = 0; i < mPhysicsWorlds.Size(); ++i )
		if( mPhysicsWorlds[i]->mHavokWorld->World == hkworld )
			return mPhysicsWorlds[i];
	return 0;
}

Quaternion PhysicsWorld::GetHavokBodyRotation( void* body )
{
	hkRigidBody* havokBody = (hkRigidBody*)body;
	if( havokBody )
	{
		hkReal r = havokBody->getRotation().getReal();
		hkVector4 q = havokBody->getRotation().getImag();
		return Quaternion( -r, q(0), q(1), q(2) );
    }
	return Quaternion(1,0,0,0);
}

void PhysicsWorld::SetHavokBodyRotation( void* body, Quaternion& quat )
{
	hkRigidBody* havokBody = (hkRigidBody*)body;
	if( havokBody )
	{
		hkQuaternion q( quat.X, quat.Y, quat.Z, -quat.W);
#ifdef HAVOK_POSITION_BUG_TRACKING
		if( !q.isOk() )
			GLog.Print( "Bad quaternion in SetHavokBodyRotation\n" );
#endif
		havokBody->setRotation( q );
	}
}

Vector PhysicsWorld::GetHavokBodyVelocity( void* body )
{
	hkRigidBody* havokBody = (hkRigidBody*)body;
	if( havokBody )
	{
		const hkVector4& vel = havokBody->getLinearVelocity();
		return Vector( vel(0), vel(1), vel(2) );
    }

    return Vector(0,0,0);
}

void PhysicsWorld::SetHavokBodyVelocity( void* body, Vector vel )
{
	hkRigidBody* havokBody = (hkRigidBody*)body;
#ifdef HAVOK3
	if( havokBody && havokBody->getMotionType() != hkMotion::MOTION_KEYFRAMED )
#else
	if( havokBody && !havokBody->isKeyframed() )
#endif
	{
		hkVector4 temp( vel.X, vel.Y, vel.Z );
#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
				if( !temp.isOk4() || ( fabs(temp(0)) > 1000.0f ) || ( fabs(temp(1)) > 1000.0f ) || ( fabs(temp(2)) > 1000.0f ) )
#else
				if( !temp.isOk() || ( fabs(temp(0)) > 1000.0f ) || ( fabs(temp(1)) > 1000.0f ) || ( fabs(temp(2)) > 1000.0f ) )
#endif
					GLog.Print( "Velocity > 1000 in SetHavokBodyVelocity\n" );
#endif
		havokBody->setLinearVelocity( temp );
		if( havokBody->getSimulationIsland() )
#ifdef HAVOK3
			havokBody->activate();
#else
			havokBody->forceActivate();
#endif
    }
}

Vector PhysicsWorld::GetHavokBodyPosition( void* body )
{
	hkRigidBody* havokBody = (hkRigidBody*)body;
	if( havokBody )
	{
		const hkVector4& vel = havokBody->getPosition();
		return Vector( vel(0), vel(1), vel(2) );
    }

    return Vector(0,0,0);
}

void PhysicsWorld::SetHavokBodyPosition( void* body, Vector vel )
{
	hkRigidBody* havokBody = (hkRigidBody*)body;
	if( havokBody )
	{
		hkVector4 temp( vel.X, vel.Y, vel.Z );
#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
		if( !temp.isOk4() || ( fabs(temp(0)) > 10000.0f ) || ( fabs(temp(1)) > 10000.0f ) || ( fabs(temp(2)) > 10000.0f ) )
#else
		if( !temp.isOk() || ( fabs(temp(0)) > 10000.0f ) || ( fabs(temp(1)) > 10000.0f ) || ( fabs(temp(2)) > 10000.0f ) )
#endif
			GLog.Print( "Position > 10000 in SetHavokBodyPosition\n" );
#endif
		havokBody->setPosition( temp );
		if( havokBody->getSimulationIsland() )
#ifdef HAVOK3
			havokBody->activate();
#else
			havokBody->forceActivate();
#endif
    }
}

void PhysicsWorld::PinHavokBody( void* body, bool pin )
{
	hkRigidBody* havokBody = (hkRigidBody*)body;
	if( havokBody )
#ifdef HAVOK3
	{
		if ( pin )
			havokBody->setMotionType( hkMotion::MOTION_KEYFRAMED );
		else
			havokBody->setMotionType( hkMotion::MOTION_FIXED );
	}
#else
		havokBody->setKeyframed( pin );
#endif
}

bool PhysicsWorld::IsHavokBodyPinned( void* body )
{
	hkRigidBody* havokBody = (hkRigidBody*)body;
#ifdef HAVOK3
	if( havokBody )
		return ( havokBody->getMotionType() == hkMotion::MOTION_KEYFRAMED );
	else
		return true;
#else
	return havokBody?havokBody->isKeyframed():true;
#endif
}

bool PhysicsWorld::IsHavokBodyInWorld( void* body )
{
	hkRigidBody* havokBody = (hkRigidBody*)body;
	return havokBody?( havokBody->getSimulationIsland() != 0 ):false;
}

void PhysicsWorld::RemoveHavokBodyFromIS( void* body, bool remove )
{
	hkRigidBody* havokBody = (hkRigidBody*)body;
	if( remove && !havokBody->hasProperty( HPRayCastDontCollide ) )
	{
		hkPropertyValue pv;
#ifdef HAVOK3
		pv.setInt(1);
#else
		pv.asInt = 1;
#endif
		havokBody->addProperty( HPRayCastDontCollide, pv );
	}
	else if( !remove && havokBody->hasProperty( HPRayCastDontCollide ) )
		havokBody->removeProperty( HPRayCastDontCollide );
}

BYTE PhysicsWorld::GetHavokBodyActiveGroup( void* body )
{
	hkRigidBody* havokBody = (hkRigidBody*)body;
	if( !havokBody->hasProperty( HPEntityPointer ) ) return 0;
	hkPropertyValue pv = havokBody->getProperty( HPEntityPointer );
#ifdef HAVOK3
	Entity* entity = (Entity*)pv.getPtr();
#else
	Entity* entity = (Entity*)pv.asPtr;
#endif
	return ( entity && ( entity->Type == Entity::ETypeMesh ) )?((WorldMesh*)entity)->ActiveGroup:0;
}

Vector PhysicsWorld::GetGravity( void )
{
	hkVector4 grav = mHavokWorld->World->getGravity();
	return Vector( grav(0), grav(1), grav(2) );
}

inline void PhysicsWorld::PrecacheRagdoll( Model* model, const char* fileName )
{
	Assert( mHavokWorld );
	mHavokWorld->PrecacheRagdoll( model, fileName );
}

#ifdef TRANSGAMING
void PhysicsWorld::ActiveMeshGroupEnable( unsigned char group, bool enable )
#else
inline void PhysicsWorld::ActiveMeshGroupEnable( unsigned char group, bool enable )
#endif
{
	Assert( mHavokWorld );
	mHavokWorld->PinnedMeshGroupEnable( group, enable );
}

#ifdef TRANSGAMING
void PhysicsWorld::ActiveMeshGroupStaticMeshEnable( unsigned char group, bool enable )
#else
inline void PhysicsWorld::ActiveMeshGroupStaticMeshEnable( unsigned char group, bool enable )
#endif
{
	Assert( mHavokWorld );
	mHavokWorld->PinnedMeshGroupStaticMeshEnable( group, enable );
}

#ifdef TRANSGAMING
void PhysicsWorld::ActiveMeshGroupRecurrentActivationEnable( unsigned char group, bool enable )
#else
inline void PhysicsWorld::ActiveMeshGroupRecurrentActivationEnable( unsigned char group, bool enable )
#endif
{
	Assert( mHavokWorld );
	mHavokWorld->PinnedMeshGroupRecurrentActivationEnable( group, enable );
}

#ifdef TRANSGAMING
void PhysicsWorld::ActiveMeshGroupActivate( unsigned char group )
#else
inline void PhysicsWorld::ActiveMeshGroupActivate( unsigned char group )
#endif
{
	Assert( mHavokWorld );
	mHavokWorld->PinnedMeshGroupActivate(group);
}

inline Model* PhysicsWorld::GetModelByConstraint( void* constraint )
{
	Assert( mHavokWorld );
	return mHavokWorld->GetModelByConstraint( constraint );
}

#ifdef TRANSGAMING
void PhysicsWorld::ActiveMeshGroupSetTimeToRemove( unsigned char group, float time, float randomizer )
#else
inline void PhysicsWorld::ActiveMeshGroupSetTimeToRemove( unsigned char group, float time, float randomizer )
#endif
{
	Assert( mHavokWorld );
	mHavokWorld->ActiveMeshGroupSetTimeToRemove( group, time, randomizer );
}

#ifdef TRANSGAMING
void PhysicsWorld::SetActiveMeshMaxRecursiveActivationDistance( float distance )
#else
inline void PhysicsWorld::SetActiveMeshMaxRecursiveActivationDistance( float distance )
#endif
{
	Assert( mHavokWorld );
	if( distance < 0.0f ) distance = 0.0f;
	mHavokWorld->MaxRecursiveActivationDistance = distance;
}

#ifdef TRANSGAMING
bool PhysicsWorld::LineTraceStaticMesh( WorldMesh* mesh, Vector& start, Vector& end, HitData& hitData )
#else
inline bool PhysicsWorld::LineTraceStaticMesh( WorldMesh* mesh, Vector& start, Vector& end, HitData& hitData )
#endif
{
	Assert( mHavokWorld );
	return mHavokWorld->LineTraceStaticMesh( mesh, start, end, hitData );
}

#ifdef TRANSGAMING
void PhysicsWorld::ActiveMeshGroupSetActivationSettings( unsigned char group, bool enable, float minimalDelay, float minimalStrength, float minimalMass, float maximalMass, float chance, float TTDBase, float TTDAddition )
#else
inline void PhysicsWorld::ActiveMeshGroupSetActivationSettings( unsigned char group, bool enable, float minimalDelay, float minimalStrength, float minimalMass, float maximalMass, float chance, float TTDBase, float TTDAddition )
#endif
{
	Assert( mHavokWorld );
	return mHavokWorld->ActiveMeshGroupSetActivationSettings( group, enable, minimalDelay, minimalStrength, minimalMass, maximalMass, chance, TTDBase, TTDAddition );
}

#ifdef TRANSGAMING
void PhysicsWorld::ActiveMeshGroupSetCollisionGroup( unsigned char group, int collGroup )
#else
inline void PhysicsWorld::ActiveMeshGroupSetCollisionGroup( unsigned char group, int collGroup )
#endif
{
	Assert( mHavokWorld );
	return mHavokWorld->PinnedMeshGroupSetCollisionGroup( group, collGroup );
}

void PhysicsWorld::StaticMeshEntityCheck( void )
{
	Assert( mHavokWorld );
	mHavokWorld->StaticMeshEntityCheck();
}

bool PhysicsWorld::LoadPhysicsWorld( FileObject* f )
{
	Assert( mHavokWorld );
	return mHavokWorld->LoadWorld( f );
}

bool PhysicsWorld::SavePhysicsWorld( FileObject* f )
{
	Assert( mHavokWorld );
	return mHavokWorld->SaveWorld( f );
}

void* PhysicsWorld::GetStaticMesh( Entity* entity )
{
	Assert( mHavokWorld );
	return mHavokWorld->GetStaticMesh( entity );
}

bool PhysicsWorld::SetEntityCollisionGroup(Entity* entity, bool activeOnly, int newGroup)
{
	Assert( mHavokWorld );
	return mHavokWorld->SetEntityCollisionGroup(entity, activeOnly, newGroup);
}

void PhysicsWorld::MPPredSaveRagdolls( void )
{
	Assert( mHavokWorld );
	mHavokWorld->SavedState.SaveRagdolls( mHavokWorld );
}

void PhysicsWorld::MPPredRestore( void )
{
	Assert( mHavokWorld );
	mHavokWorld->SavedState.Restore( mHavokWorld );
}

void PhysicsWorld::MPPredRemoveAllFromWorld( void )
{
	Assert( mHavokWorld );
	mHavokWorld->MPPredRemoveAllFromWorld();
}

void PhysicsWorld::MPPredAddRestToWorld( void )
{
	Assert( mHavokWorld );
	mHavokWorld->MPPredAddRestToWorld();
}

float PhysicsWorld::GetTickPart( void )
{
	Assert( mHavokWorld );
	return mHavokWorld->mTickPart/GEngine->Physics->PhysicsFPS;
}

bool PhysicsWorld::TickWillMovePhysics( float timeDelta )
{
	Assert( mHavokWorld );
	return mHavokWorld->TickWillMovePhysics( timeDelta );
}

ulong32 PhysicsWorld::GetPhysicsFrame( void )
{
	Assert( mHavokWorld );
	return mHavokWorld->mPhysicsFrame;
}

void PhysicsWorld::AdvancePhysicsFrame( void )
{
	Assert( mHavokWorld );
	++mHavokWorld->mPhysicsFrame;
}

void PhysicsWorld::SetPhysicsFrame( ulong32 frame )
{
	Assert( mHavokWorld );
	mHavokWorld->mPhysicsFrame = frame;
}
