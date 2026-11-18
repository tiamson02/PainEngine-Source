// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/07 Klaudiusz Zych
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//   Model Animation - pure virtual base class for animations
//----------------------------------------------------------------------------

// #pragma once
#ifndef _MODELANIMATION_
#define _MODELANIMATION_


class AnimatedMesh;
class Model;

#include "Core/LeString.h"
#include "Core/Math/LeMath.h"

class ModelAnimation
{
public:
	enum 
	{
		ETransX	= (1<<0),
		ETransY = (1<<1),
		ETransZ = (1<<2),
		ERot    = (1<<3),
	};

	String		   	Name;
    float		   	Time;
	bool			Looped;		// for later synchronization with multiplayer only
    BoundingBox     Box;
    AnimatedMesh*   MyAnimatedMesh;

#ifdef TRANSGAMING
	ModelAnimation(const char *name, AnimatedMesh* animatedMesh) : Name(name), MyAnimatedMesh(animatedMesh), Looped(false), Time(0)
	{}
#else
	ModelAnimation(const String& name, AnimatedMesh* animatedMesh) : Name(name), MyAnimatedMesh(animatedMesh), Looped(false), Time(0)
	{}
#endif
	virtual ~ModelAnimation();

    virtual void ComputeFrameMatrix(DynamicArray<Matrix>& matrixArr, float time, const Model* model, const Matrix &world) const;

    virtual void Load(FileObject*, bool loopKeys, float scale) = 0;
#ifdef TRANSGAMING
    virtual void SetMovementCurve(const char *name, DWORD MovementCurves) = 0;
#else
    virtual void SetMovementCurve(const String& name, DWORD MovementCurves) = 0;
#endif
	virtual Vector GetMovement(const float time) const = 0;
	virtual Quaternion GetRotation(const float time) const = 0;
};

//----------------------------------------------------------------------------
//   Skeletal Animation
//----------------------------------------------------------------------------

class Key
{
public:
    float               Frame;
    Matrix              Mat;
	Quaternion			Rot;
	Vector				Pos;
	Matrix				GlobalMat;
};

class AnimationCurve
{
public:
	int					Joint;
    String              Name;
    DynamicArray<Key>   Keys;

	AnimationCurve() : Joint(-1) {}
};

class SkeletalAnimation : public ModelAnimation
{
public:
    DynamicArray<AnimationCurve>    AnimationCurves;

protected:
	AnimationCurve*					MovementCurve;
	DWORD							MovementFlags;
	Vector							MovementDelta;
	Quaternion						RotationDelta;

public:
#ifdef TRANSGAMING
	SkeletalAnimation(const char *name, AnimatedMesh* animatedMesh) 
		: ModelAnimation(name, animatedMesh), MovementCurve(NULL), MovementFlags(0)
	{}
#else
	SkeletalAnimation(const String& name, AnimatedMesh* animatedMesh) 
		: ModelAnimation(name, animatedMesh), MovementCurve(NULL), MovementFlags(0)
	{}
#endif
	virtual ~SkeletalAnimation();

    virtual void Load(FileObject* f, bool loopKeys, float scale);

    void ComputeFrameMatrix(DynamicArray<Matrix>& mat, float time, const Model* model, const Matrix &world) const;
#ifdef TRANSGAMING
	void SetMovementCurve(const char *name, DWORD MovementCurves);
#else
	void SetMovementCurve(const String& name, DWORD MovementCurves);
#endif
    DWORD HasMovementCurve(void) { return ( MovementFlags ); };
	Vector GetMovement(const float time) const;
	Quaternion GetRotation(const float time) const;

protected:
    virtual void ComputeFrameMatrix(Matrix m, const DynamicArray<Matrix>& matrix, int& index, const Model* model, float t, int pKey, int nKey) const;
    
	void ShrinkAnim(void);
	void NormalizeJoints(const float scale);	//temp shit, move it to MAYA exporter
	void ComputeKeyMatrix(int key, Matrix m, const DynamicArray<Matrix>& matrix, int& index) const;
	DynamicArray<Matrix> ComputeKeyMatrix(int key, float scale) const;
	void ComputeKeysTime(float time, float &t, int &pKey, int &nKey) const;

#ifdef TRANSGAMING
    AnimationCurve* FindCurve(const char *name) const;
#else
    AnimationCurve* FindCurve(const String& name) const;
#endif
    AnimationCurve* FindCurve(int joint) const;
};



//----------------------------------------------------------------------------
//   Ragdoll Animation
//----------------------------------------------------------------------------

class RagdollAnimation : public SkeletalAnimation
{
public:

#ifdef TRANSGAMING
	RagdollAnimation(const char *name, AnimatedMesh* animatedMesh) : SkeletalAnimation(name, animatedMesh)
	{}
#else
	RagdollAnimation(const String& name, AnimatedMesh* animatedMesh) : SkeletalAnimation(name, animatedMesh)
	{}
#endif
    void ComputeFrameMatrix(DynamicArray<Matrix>& mat, float time, const Model* model, const Matrix &world) const;

protected:
    void ComputeFrameMatrix(Matrix m, const DynamicArray<Matrix>& matrix, int& index, const Model *model) const;
};

#endif //_MODELANIMATION_

