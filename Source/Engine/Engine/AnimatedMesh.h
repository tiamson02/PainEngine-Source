// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/06 Klaudiusz Zych
//----------------------------------------------------------------------------

// #pragma once
#ifndef _ANIMATEDMESH_
#define _ANIMATEDMESH_


class ModelAnimation;
class Model;

#include "Core/AbstractFactory.h"
#include "Core/Object.h"
#include "Engine/SimpleMesh.h"

class Joint
{
public:
    String          Name;
    Matrix          Mat;
	int				Parent;
    char            Children;

	Matrix			Global;		//temp shit, move it to MAYA exporter
	Joint() : Parent(-1)
	{
	}

    friend FileObject& operator<<(FileObject& file, const Joint& j)
	{
		return file << j.Name << j.Mat << j.Children;
	}

    friend FileObject& operator>>(FileObject& file, Joint& j)
	{
		return file >> j.Name >> j.Mat >> j.Children;
	}
};

class Weight
{
public:
	WORD		    bone;
	float		    weight;

    friend FileObject& operator<<(FileObject& file, const Weight& w)
    {
        return file << w.bone << w.weight;
    }

    friend FileObject& operator>>(FileObject& file, Weight& w)
    {
        return file >> w.bone >> w.weight;
    }
};

class Influence
{
public:
	DynamicArray<Weight>		Weights;

    friend FileObject& operator<<(FileObject& file, const Influence& infl)
    {
		return file << infl.Weights;
    }

    friend FileObject& operator>>(FileObject& file, Influence& infl)
    {
		return file >> infl.Weights;
    }
};

class ENGINE_API MeshShape : public SimpleMesh
{
public:
	int							MaxInfluences;

    bool						IsLighting;
	bool						IsWarp;

	String						Name;

	DynamicArray<Influence>	    Influences;

	DynamicArray<WORD>			InflictingBones;

    DynamicArray<VertexWeights> Weights;

	int WeightsStream;

	enum
	{
		EVertexCPUSkin	= (1<<0),	// Vertex lighting + CPU skinning
		EPixelCPUSkin	= (1<<1),	// Pixel lighting + CPU skinning
		EVertexGPUSkin	= (1<<2),	// Vertex lighting + GPU skinning
		EPixelGPUSkin	= (1<<3),	// Pixel lighting + GPU skinning
		EWaterGPUSkin	= (1<<4),	// water effect
        EInvalid        = (1<<31),  // cannot be rendered
	};

	int RenderType;


    friend FileObject& operator<<(FileObject& file, const MeshShape& ms)
	{
		return file << ms.Name << (SimpleMesh&)ms << ms.Influences;
	}

    friend FileObject& operator>>(FileObject& file, MeshShape& ms)
	{
		return file >> ms.Name >> (SimpleMesh&)ms >> ms.Influences;
	}

	MeshShape();
	virtual ~MeshShape();

	void SetupVertexWeights();

protected:
	WORD RedirectBone(WORD globalBone);
};

class ENGINE_API BindSkeleton
{
public:
    WORD                        Hierarchies;
    DynamicArray<Joint>         Joints;
	DynamicArray<Matrix>		InvBindMatrixArr;

	BindSkeleton() : Hierarchies(0)
	{}
    int  JointIndex(const char* name) const;
	void ProcessRelations();
	void NormalizeJoints(float scale);


    friend FileObject& operator<<(FileObject& file, const BindSkeleton& bs)
	{
		return file << bs.Hierarchies << bs.Joints;
	}

    friend FileObject& operator>>(FileObject& file, BindSkeleton& bs)
	{
		return file >> bs.Hierarchies >> bs.Joints;
	}

protected:
	void ProcessRelationsRecursive(int &index, int parentId);
};

#define MAX_WARP_NUM	4

class ENGINE_API AnimatedMesh : public Object
{
public:
	static const char *DefaultExt;
    DECLARE_OBJECT_CLASS(AnimatedMesh);

	String							Name;
    BoundingBox                     Box;
	float							Scale;
	
	int								Warps[MAX_WARP_NUM];		// index into Meshes, -1 slot unused
	Material*						WarpShaders[MAX_WARP_NUM];
	Material*						WarpFakeShaders[MAX_WARP_NUM];
    DynamicArray<MeshShape>			Meshes;
    BindSkeleton					Skeleton;
	DynamicArray<ModelAnimation*>	Animations;
	
	int								Sort;

	bool							UseNormalMaps;

	Texture*						EnvCubeMap;


	AnimatedMesh();

    virtual ~AnimatedMesh();

    DynamicArray<Matrix> ComputeBindMatrix(const Matrix &world) const;

	virtual void AnimateGeometry(const DynamicArray<Matrix>& frameMatrixArr);
#ifndef TRANSGAMING
    virtual void AnimateTangentSpace(const DynamicArray<Matrix>& frameMatrixArr);
#endif
	virtual void SetBindPoseFrame(const Model* model) const;

	virtual void RenderInitialize();
	virtual void RenderCleanup();

	virtual void Render(Model* model, int mode) const;
	void LoadPreprocess(float scale, const char* name);

#ifdef TRANSGAMING
    int  LoadAnimation(const char *name, bool looped, int forceSlot);
	int  FindAnimationByName(const char *name) const;

	Texture* CreateTexture(const char *texture, int flags, int lodset) const;
#else
    int  LoadAnimation(const String& name, bool looped, int forceSlot);
	int  FindAnimationByName(const String& name) const;

	Texture* CreateTexture(const String& texture, int flags, int lodset) const;
#endif
protected:	

    void ComputeBindMatrix(Matrix m, const DynamicArray<Matrix>& matrix, int& index) const;

    void ProcessHierarchy(FileObject *f);
#ifdef TRANSGAMING
    MeshShape* FindMeshByName(const char *name) const;
#else
    MeshShape* FindMeshByName(const String& name) const;
#endif

	virtual void RenderDefault(Model* model, int mode) const;
	virtual void RenderVolume(const Model* model) const;
	virtual void RenderShadowmap(const Model* model) const;
	virtual void RenderGhostFX(Model* model) const;
	virtual void RenderDemonFX(Model* model) const;
	virtual void RenderWarpFX(Model* model) const;

private:
	// serialization
    void Load(FileObject* file);
    void Save(FileObject* file);
};


class ENGINE_API AnimatedMeshMatPal : public AnimatedMesh
{
public:
    DECLARE_OBJECT_CLASS(AnimatedMeshMatPal);
	static const int MAX_BONES_PER_MESH = 23;

	void RenderInitialize();

	void AnimateGeometry(const DynamicArray<Matrix>& frameMatrixArr);
	void SetBindPoseFrame(const Model* model) const;

protected:
	virtual void RenderDefault(Model* model, int mode) const;
	void RenderVolume(const Model* model) const;
	void RenderShadowmap(const Model* model) const;
	void RenderGhostFX(Model* model) const;
	void RenderDemonFX(Model* model) const;
	void RenderWarpFX(Model* model) const;
};


#endif //_ANIMATEDMESH_

