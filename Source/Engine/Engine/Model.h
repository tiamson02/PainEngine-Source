// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/07 Klaudiusz Zych
//----------------------------------------------------------------------------

// #pragma once
#ifndef _MODEL_
#define _MODEL_


class AnimatedMeshManager;

#include "Render/VertexBuffer.h"
#include "Engine/Entity.h"
#include "Physics/Ragdoll.h"
#include "Engine/AnimatedMesh.h"
#include "Engine/AnimationController.h"



class RagdollJoint
{
public:
	Matrix			Transform;
	bool			Global;		// Global == transformed by Havok, and given in localspace
	BoundingBox		LocalBox;
};

class RagdollSkeleton 
{
public:
	DynamicArray<RagdollJoint>	Joints;
};

class AnimatedJoint
{
public:
	int			Joint;
	Quaternion	Angle;

	AnimatedJoint(int j, const Quaternion &a) : Joint(j), Angle(a) {}
};

class MeshProperties
{
public:
	class GroupProperties
	{
	public:
		Texture* TexPtr;
		Texture* TexEmPtr;
		GroupProperties() : TexPtr(0), TexEmPtr(0)
		{}
	};

	bool IsVisible;
	Material* DefaultShader;
	Texture*  NormalMapPtr;
	DynamicArray<GroupProperties> Groups;
	
	MeshProperties() : IsVisible(true), DefaultShader(NULL), NormalMapPtr(NULL)
	{}
};

class ENGINE_API Model : public Entity
{
public:
	float				LastDrawTime;
	int					LastAnimateFrame;
    int					LastRagdollFrame;
	AnimatedMesh		*Mesh;
	RagdollSkeleton		Skeleton;
	Vector				Center;
	int					CenterJointId;
	Vector				FullBrightColor;

	// shadow
    bool                ShadowEnabled;
	Texture*			ShadowMap;
	Texture*			ShadowMapFilter;
	DWORD				ShadowUpdateFrame;
	Decal*				ShadowDecal; 
	Matrix				ShadowMatrix;
	float				BlendAlpha;

	// impact 
	const static int MAX_IMPACTS = 4;

	struct sImpact {
		Vector	Epicenter;
		float	Amplitude;
		float	Period;
		float	Range;
		float	Speed;
		float	Time;
	} Impact[MAX_IMPACTS];

	// persistant state
	DynamicArray<Matrix>			matricesArr;
	DynamicArray<Matrix>			FrameMatrixArr;
	DynamicArray<MeshProperties>	MeshPropertiesArr;

	// animation stuff
	AnimationController				AnimController;
    bool                            RagDoll;
	Ragdoll*						RagdollPtr;
    
public:
	Model();
	~Model();

	virtual void Draw(RenderDevice* dev, int mode);
	virtual void Tick(const float timeDelta);
	virtual Vector GetCenter() const { return Center;}

	// Load/save
	virtual bool LoadEntity( FileObject* f );
	virtual bool SaveEntity( FileObject* f );

#ifndef TRANSGAMING
    // Demo recording
	virtual bool DemoCalcSize( int &size, DWORD &contents );
	virtual bool DemoReadEntity( FileObject *f );
	virtual bool DemoWriteEntity( FileObject *f );
	virtual bool DemoReadFull( FileObject *f );
	virtual bool DemoWriteFull( FileObject *f );
#endif

	void SetScale( float scale ) {};
	void UpdateTransform(bool updateInWorld = true);
	int   ActivateAnimation(int anim, bool looped=false, float animScale = 1.0f, float blendTime=0.5f, bool startAfterBlend=false);
#ifdef TRANSGAMING
    int   FindAnimation(const char *name) const { return Mesh->FindAnimationByName(name); } 
#else
    int   FindAnimation(const String& name) const { return Mesh->FindAnimationByName(name); } 
#endif
	float GetAnimationFrameTime(int anim) const;
	void SetAnimationFrameTime(int anim, float time);
	float GetAnimationTotalTime(int anim) const;
	void  SetAnimationTimeScale(int anim, float timeScale);
	float GetAnimationTimeScale(int anim);

	void SetWaterImpact(int impact, const Vector& epicenter, float amplitude, float period, float range, float speed);
	void UpdateWaterImpact(int impact, const Vector& epicenter, float amplitude, float period, float range, float speed);
#ifdef TRANSGAMING
    void SetMaterialSpecular(const char *name, const Vector& color, const float power);
	void SetMaterialRefractFresnel(const char *name, const float refract, const float fresnel, const Vector &reflTint, const Vector &refrTint);
	void SetMeshVisibility(const char *name, bool isVisible);
    void SetMeshLighting(const char *name, bool isLighting, const Vector& color);
	void SetTexture(const char *originalTexture, const char *newTexture);
	void SetMaterial(const char *newMaterial);
#else
    void SetMaterialSpecular(const String& name, const Vector& color, const float power);
	void SetMaterialRefractFresnel(const String& name, const float refract, const float fresnel, const Vector &reflTint, const Vector &refrTint);
	void SetMeshVisibility(const String& name, bool isVisible);
    void SetMeshLighting(const String& name, bool isLighting, const Vector& color);
	void SetTexture(const String& originalTexture, const String& newTexture);
	void SetMaterial(const String& newMaterial);
#endif
	void ResetMaterialSpecular();
	void SetBlendAlpha(float alpha);
	void UpdateVolEntity();
	void CreateShadowMap(bool enable);

	int    GetJointIndex(const char* jointName) const;
#ifdef TRANSGAMING
    const char *GetJointName(int joint) const;
#else
    String GetJointName(int joint) const;
	String GetMaterialName() const { return MaterialName; }
#endif
	Matrix GetJointTransform(int joint);
		
	bool  ComputeFrameMatrix(DynamicArray<Matrix>& frameMat, const Matrix &world);
	void  SetupGib( Model* gib );
  
    void  SetRagdoll( bool enable, int collisionGroup );
	bool  IsRagdoll();

#ifdef TRANSGAMING_USE_EDITOR
    void  DrawSkeleton(RenderDevice* dev);
	void  DrawJointNames(DWORD color);
#endif

	BOOL  IsPerPixel() const;

	BOOL  Load(const char *name, float scale, AnimatedMeshManager& meshMgr);
    int   LoadAnimation(const char *name, bool looped, int forceSlot = -1);

#ifdef TRANSGAMING
    BOOL   SetAnimationMovementCurve(const int anim, const char *curvename, DWORD lockAxis);
#else
    BOOL   SetAnimationMovementCurve(const int anim, const String& curvename, DWORD lockAxis);
#endif
    Vector GetAnimationMovement(const int anim, const float deltaTime) const;
	void   ApplyJointRotation(int joint, float heading, float pitch, float yaw);

	void RenderCleanup(RenderDevice* dev, MaterialSystem* matSys);

	void CreateRagdollIfNone( void );

protected:
    String MaterialName;
	friend class SkeletalAnimation;
	DynamicArray<AnimatedJoint>	AnimatedJoints;

	void Setup();

#ifdef TRANSGAMING_USE_EDITOR
    void RenderSkeletonRecursive(int &index, const Matrix& boneMatrix);
    void RenderBone(const Vector& p, DWORD color, const Matrix& parentMatrix, const Matrix& objMatrix);
	void RenderSphere(DWORD color, const Matrix& m, float radius);
	void RenderJoint(DWORD color, const Vector& p0, const Vector& p1, float mag, float mag2);
#endif

#ifndef TRANSGAMING
    // eric-> this function seems to never be defined!?!?  Good thing it isn't called :P
	void UpdateObjectToWOrldMatrix();
#endif
	void ProcessRagdoll(); // compute local boundingboxes on ragdoll limbs
	void RecomputeLocalBB();

	// Last frame state for demo recording
	int mLastMeshAnimSize;
};


#endif //_MODEL_

