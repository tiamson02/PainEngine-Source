// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
//----------------------------------------------------------------------------

// #pragma once
#ifndef _GLASSSYSTEM_
#define _GLASSSYSTEM_


#include "Core/DynArray.h"
#include "Core/Math/LeMath.h"

class WorldMesh;
class havokCollisionListener;

struct GlassFragment
{
public:
	WorldMesh*			FragmentMesh;
	float				ActivationTime;
};

class Glass
{
private:
	// Methods
	void Breakup(const Vector &hitPoint, float activationRadius, Vector velocityToGive );
	bool BreakupAfterStart(const Vector &hitPoint, float activationRadius, Vector velocityToGive );

	// Input
	WorldMesh* mGlassMesh;
	void* mMainMeshBody;
	void* mCollisionListener;

	// Management
	bool mStarted;
	bool mFinished;
	float mActivationState;
	DynamicArray<GlassFragment*> mGlassFragments;

	// Delayed glass breakup
	bool BreakupDelayed;
	Vector DelayedHitPoint;
	float DelayedActivateRadius;
	Vector DelayedVelocity;

	// New breakup method - during level load
	DynamicArray<GlassFragment*> mPreloadedGlassFragments;

	// Save/load remembering structure
	DynamicArray<Vector> CreationNormals;
	DynamicArray<int> CreationMeshIndices;
	DynamicArray<WorldMesh*> mGlassMeshes;

	// Static
	static float ActivationDelay;
	static float ActivationSpeed;
	static float NonColliderPerc;
	static float TimeToDelete;
	static float TimeToDeleteRnd;

	void Clear( void );
	bool BreakupFromFile( FileObject* f );
public:
	static float ExplosionRangeModifier;

	Glass( WorldMesh* glassMesh );
	~Glass();

	void PreloadBreakup( void );

	WorldMesh* GetMainMesh( void ) { return mGlassMesh; };
	void SetCL( void* cl ) { mCollisionListener = cl; };
	void StartGlass( Vector hitPoint, float activationRadius, Vector velocityToGive );
	void StartGlassDelayed( Vector hitPoint, float activateRadius, Vector velocityToGive );
	bool Started( void ) { return mStarted; };
	void ActivateFragment( void* rigidBody, Vector hitPoint, Vector velocityToSet );
	void ForgetFragment( WorldMesh* fragment );

	// Load/save
	bool LoadGlass( FileObject* f );
	bool SaveGlass( FileObject* f );

	void Tick( const float timeDelta );
};


#endif //_GLASSSYSTEM_

