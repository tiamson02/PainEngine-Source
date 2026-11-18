#include "Engine.h"
#include "Render/HUD.h"
#include "Core/FileSystem/LogBuffer.h"
#include "Render/Material.h"
#include "Render/Decal.h"
#include "Render/View.h"
#include "Engine/ModelAnimation.h"
#include "Render/Video.h"
#include "Engine/AnimatedMesh.h"
#include "Engine/Model.h"
#include "Render/Light.h"
#include "Engine/World.h"

#include "FileSystem/GFileManager.h"
extern ENGINE_API GFileManager  	GFileMan;

#ifndef TRANSGAMING
extern LogBuffer MLog;
#endif

/* min and max macros */
#ifndef NOMINMAX
#ifndef max
#define max(a,b)   (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b)   (((a) < (b)) ? (a) : (b))
#endif
#endif  /* NOMINMAX */

IMPLEMENT_OBJECT_CLASS(AnimatedMesh);
IMPLEMENT_OBJECT_CLASS(AnimatedMeshMatPal);

#ifdef TRANSGAMING_PS2
# ifdef TRANSGAMING_DEINDEX
#  error CANNOT USE CHEAPDEINDEX and TRANSGAMING_DEINDEX
# else
#  define CHEAPDEINDEX
# endif
#endif

#ifdef TRANSGAMING_SHOW_FPS
extern int g_AnimatedMeshes;
extern int g_AnimatedVerts[4];
#endif

bool DDP_SkipAnimatedMeshes = false;

//---------------------------------------------------------------------------

int BindSkeleton::JointIndex(const char* name) const
{
    for (WORD i=0; i<Joints.Size(); i++)
    {
        if (Joints[i].Name == name)
            return int(i);
    }

    return -1;
}

void BindSkeleton::ProcessRelations()
{
	int index = 0;
    for (int i=0; i<Hierarchies; i++)
	{
		ProcessRelationsRecursive(index, -1);
	}
}

void BindSkeleton::ProcessRelationsRecursive(int &index, int parentId)
{
    Joint &joint = Joints[index];

	joint.Parent = parentId;

	int myId = index;
	index ++;

    for (int i=0; i<joint.Children; i++)
    {
        ProcessRelationsRecursive(index, myId);
    }
}

void BindSkeleton::NormalizeJoints(float scale)
{
	for (int i=0; i<Joints.Size(); i++)
	{
		Joint &joint = Joints[i];
		int p = joint.Parent;

		Matrix m = joint.Mat;

		while (p>=0)
		{
			m = m * Joints[p].Mat;
			p = Joints[p].Parent;
		}
		Joints[i].Global = m * ScaleMatrix(scale);

		Joints[i].Global.OrthoNormalize();
	}

	for (int i=0; i<Joints.Size(); i++)
	{
		Joint &joint = Joints[i];
		int p = joint.Parent;

		Matrix originalMat = joint.Mat;

		if (p==-1)
		{
			joint.Mat = joint.Global;
		}
		else
		{
			joint.Mat = joint.Global*Joints[p].Global.Inverse();
		}
	}
}

//---------------------------------------------------------------------------

MeshShape::MeshShape() : SimpleMesh(), WeightsStream(EInvalidIndex), 
RenderType(EInvalid), IsLighting(true), IsWarp(false), MaxInfluences(0)
{
}

MeshShape::~MeshShape()
{

}

void MeshShape::SetupVertexWeights()
{
    TRACE("\nMeshShape::SetupVertexWeights(): generating a weight stream for '%s'\n", Name.AsChar());
	MaxInfluences = 0;
	int s = Data->VertList.Size();
	Weights.ResizeArray(s);

	const int maxWeights = Entity::MaxInfluences;

	for (int j=0; j<s; j++)
	{
		DynamicArray<Weight> w = Influences[j].Weights;
		int inflNum = w.Size();

		for (int i=0; i<inflNum-1; i++)
		{
			for (int j=i+1; j<inflNum; j++)
			{
				if (w[i].weight<w[j].weight)
				{
					Weight tmp = w[i];
					w[i] = w[j];
					w[j] = tmp;
				}
			}
		}
/*		MLog.Print("%d influences: ", inflNum, Name.AsChar());

		float sum = 0;

		for (int i=0; i<inflNum; i++)
		{
			MLog.Print("%f ", w[i].weight);
			sum += w[i].weight;
		}
		MLog.Print(" sum = %f\n",sum);
*/
		if (inflNum > maxWeights)
		{

			inflNum = maxWeights;

			float sum = 0;
			for (int i=0; i<maxWeights; i++)
			{
				sum += w[i].weight;
			}

			sum = 1/sum;
			for (int i=0; i<maxWeights; i++)
			{
				w[i].weight *= sum;
			}
		}

		for (int i=0; i<inflNum; i++)
		{
			w[i].bone = RedirectBone(w[i].bone);
		}

		switch (inflNum)
		{
		case 1:
			Weights[j] = VertexWeights(w[0].weight,0,0,w[0].bone,0,0,0);
			break;

		case 2:
			Weights[j] = VertexWeights(w[0].weight,w[1].weight,0,w[0].bone,w[1].bone,0,0);
			break;

		case 3:
			Weights[j] = VertexWeights(w[0].weight,w[1].weight,w[2].weight,w[0].bone,w[1].bone,w[2].bone,0);
			break;

		case 4:
			Weights[j] = VertexWeights(w[0].weight,w[1].weight,w[2].weight,w[0].bone,w[1].bone,w[2].bone,w[3].bone);
			break;

		default:
#ifdef TRANSGAMING
            TRACE("[]::MeshShape::SetupVertexWeights(): ERROR-> invalid bone influence count: %d\n", inflNum);
            Assert(0);
#else
            char buf[MAX_PATH];
            sprintf(buf,"Bad skining in mesh: %s",Name.AsChar());

            MessageBox(NULL,buf,"Model Error",MB_TOPMOST);
#endif
            
			//Assert(false); 
			break;
		}

		if (MaxInfluences < inflNum)
			MaxInfluences = inflNum;
	}

#if 0
	MLog.Print("%30s %4d bone(s), %4d bones/vert\n", Name.AsChar(), InflictingBones.Size(), MaxInfluences);
#endif

}

WORD MeshShape::RedirectBone(WORD globalBone)
{
	Assert(globalBone!=WORD(-1));
	int iBonesNum = InflictingBones.Size();
	for (WORD i=0; i<iBonesNum; i++)
	{
		if (globalBone==InflictingBones[i])
			return i;
	}

	return (WORD)InflictingBones.Add(globalBone);	
}

//---------------------------------------------------------------------------

const char *AnimatedMesh::DefaultExt= ".pkmdl";

#ifdef TRANSGAMING
AnimatedMesh::AnimatedMesh() : Scale(0), Sort(Model::ESortOpaque), UseNormalMaps(false), EnvCubeMap(NULL)
#else
AnimatedMesh::AnimatedMesh() : Scale(0), Sort(Model::ESortOpaque), UseNormalMaps(true), EnvCubeMap(NULL)
#endif
{
	for (int i=0; i<4; ++i) {
		Warps[i] = -1;
		WarpShaders[i] = 0;
		WarpFakeShaders[i] = 0;
	}
}

AnimatedMesh::~AnimatedMesh()
{
    for (int i=0; i<Animations.Size(); i++)
    {
        delete Animations[i];
    }
    RenderCleanup();
}

#ifdef TRANSGAMING
Texture* AnimatedMesh::CreateTexture(const char *texture, int flags, int lodset) const
{
	MaterialSystem* matSys = GEngine->Video->MaterialSys;
    char            name[MAX_PATH];


    snprintf(name, MAX_PATH, "Models/%s/%s", Name.AsChar(), texture);

	return matSys->CreateTexture(name, flags, lodset);
}
#else
Texture* AnimatedMesh::CreateTexture(const String& texture, int flags, int lodset) const
{
	MaterialSystem* matSys = GEngine->Video->MaterialSys;

	return matSys->CreateTexture((String("Models/")+Name+"/"+texture).AsChar(), flags, lodset);
}
#endif

void AnimatedMesh::LoadPreprocess(const float scale, const char* name)
{
	Scale = scale;
	Name = name;

    Box.Reset();
	for (int iMesh=0; iMesh<Meshes.Size(); iMesh++)
	{
        Meshes[iMesh].Data->VertList.scaleVerts(scale, &Box);

		//DynamicArray<MeshVertex>  &verts = Meshes[iMesh].Verts;
        /*VertexList &verts = Meshes[iMesh].Data->VertList;

		for (int i=0; i<verts.Size(); i++){
            Vector &pos = verts.getVertPos(i);

            pos *= scale;
            Box += pos;
        }//*/
	}

	// in case mesh doesnt have any verts
	if (!Box.Valid())
		Box += Vector(0,0,0);

	Skeleton.ProcessRelations();
	Skeleton.NormalizeJoints(Scale);
	Skeleton.InvBindMatrixArr = ComputeBindMatrix(*GIdentityMatrix);

	for (int iMat=0; iMat<Skeleton.InvBindMatrixArr.Size(); iMat++)
	{
		Skeleton.InvBindMatrixArr[iMat] = Skeleton.InvBindMatrixArr[iMat].Inverse();
	}

	RenderInitialize();

	for (int iMesh=0; iMesh<Meshes.Size(); iMesh++)
	{
		if (Meshes[iMesh].DefaultShader 
		 && Meshes[iMesh].DefaultShader->PassCount
		 && Meshes[iMesh].DefaultShader->Passes[0].State.GetBlendMode() != RenderState::EBlendNone)
			Sort = Entity::ESortTrans;
	}

    m_setTag("Mesh");
	Animations.Add(new RagdollAnimation("$ragdoll", this));
}

#ifdef TRANSGAMING
int AnimatedMesh::LoadAnimation(const char *name, bool looped, int forceSlot)
#else
int AnimatedMesh::LoadAnimation(const String& name, bool looped, int forceSlot)
#endif
{
	int anim = FindAnimationByName(name);
	
	if (anim != -1)
		return anim;

#ifdef TRANSGAMING
    char    filename[MAX_PATH];
    GFile * f;


    snprintf(filename, MAX_PATH, "../Data/Models/%s.%s.ani", Name.AsChar(), name);
    

    f = GFileMan.CreateFileReader(filename);
#else
	String modelsDir("../Data/Models/");
	GFile *f = GFileMan.CreateFileReader((modelsDir + Name + "." + name + ".ani").AsChar());
#endif

	if (f)
	{
        ModelAnimation* ani = NULL;

        char magic[4];
        f->Read(magic, sizeof(char)* 4);

        if (!strncmp(magic, "skel", 4))
        {
            m_setTag("Mesh");
            ani = new SkeletalAnimation(name, this);
	        try
	        {
                ani->Load(f, looped, Scale);
            }
            catch(...)
            {
                delete ani;
                ani = NULL;
            }
        }
		delete f;

		if (ani)
        {
			if (forceSlot!=-1)    
			{
				for (int i=Animations.Size(); i<=forceSlot; i++)
					Animations.Add(0);

				Assert(Animations[forceSlot]==0);

				Animations[forceSlot] = ani;

				return forceSlot;
			}
			else 
				return Animations.Add(ani);
        }
	}

	return -1;
}

void AnimatedMesh::RenderInitialize()
{
	RenderDevice* dev=GEngine->GetRenderDevice();
    if (!dev) return;

	MaterialSystem* matSys=GEngine->Video->MaterialSys;

	for (int iMesh=0; iMesh<Meshes.Size(); iMesh++)
	{
		MeshShape &mesh = Meshes[iMesh];

		mesh.VolShader		= matSys->CreateMaterial("volskin");

#ifdef TRANSGAMING_NON_TNL
		if (UseNormalMaps && !mesh.NormalMapName.IsNull() && dev->HWClass>=RenderDevice::EHWClassNV20)
		{
#ifdef TRANSGAMING
			Texture* NormalMapTex = CreateTexture(mesh.NormalMapName.AsChar(), 0, RenderDevice::ETexLODWeapons);
#else
			Texture* NormalMapTex = CreateTexture(mesh.NormalMapName, 0, RenderDevice::ETexLODWeapons);
#endif

			if (!(NormalMapTex->Flags & Texture::ETexInvalid))
			{
				mesh.NormalMapTex	= NormalMapTex;

				mesh.DefaultShader	= matSys->CreateMaterial("skinnedPerPixel");

				mesh.RenderType     = MeshShape::EPixelCPUSkin;

				//Sort = Entity::ESortGun;

				continue;
			}
			else
			{
				GEngine->Video->MaterialSys->ReleaseTexture(NormalMapTex);
			}
		}
#endif //TRANSGAMING_NON_TNL

		mesh.RenderType		= MeshShape::EVertexCPUSkin;
#ifndef TRANSGAMING
        mesh.NormalMapTex	= matSys->Textures.NoTex;
#endif
		mesh.DefaultShader	= matSys->CreateMaterial("skinned");
	}

    for(int i=0; i<Meshes.Size(); ++i) 
	{
        MeshShape& mesh=Meshes[i];

		for (int j=0; j<mesh.Data->Groups.Size(); j++)
		{
#ifdef TRANSGAMING
			if (mesh.RenderType==MeshShape::EPixelGPUSkin)
				mesh.Data->Groups[j].TexPtr = CreateTexture(mesh.Data->Groups[j].TexName.AsChar(), 0, RenderDevice::ETexLODModels);
			else
				mesh.Data->Groups[j].TexPtr = CreateTexture(mesh.Data->Groups[j].TexName.AsChar(), 0, RenderDevice::ETexLODModels);
#else
			if (mesh.RenderType==MeshShape::EPixelGPUSkin)
				mesh.Data->Groups[j].TexPtr = CreateTexture(mesh.Data->Groups[j].TexName, 0, RenderDevice::ETexLODModels);
			else
				mesh.Data->Groups[j].TexPtr = CreateTexture(mesh.Data->Groups[j].TexName, 0, RenderDevice::ETexLODModels);
#endif
        }

        // dynamic vertex buffer
#ifdef TRANSGAMING
        void *  vtxPtr;
        int     vtxStride;


        Assert(mesh.Data->VertStreamCount == 0);
# ifdef TRANSGAMING_PS2
        // FIXME: the PSP can do skinning in hardware
        //TRACE("%p Initializing AnimatedMesh %d\n", this, i);

        //Assert(!"This shouldn't be called");

        //!!// for now, convert the vertices to NTU format and create the vertex arrays accordingly.  Later we'll have to
        //!!// add the weight stream and make a new shader to support that.  The animated meshes should have to shader
        //!!// dedicated to them for both the PSP and PS2.  This new shader should expect bone weights and should not
        //!!// be used by any other system.
        Assert(mesh.Data->VertList.getFormat() == VertexList::VTXFMT_MESH);

        //position buffer
        vtxPtr = mesh.Data->VertList.getPositions(&vtxStride);
        Assert(vtxPtr && vtxStride);
        TRACE("AnimatedMesh::RenderInitialize(%d): allocating a position stream for %d verticies (size = %d, type = '%s')\n", __LINE__, mesh.Data->VertList.Size(), vtxStride, mesh.Data->VertList.getFormatName());
		mesh.Data->VertStream[mesh.Data->VertStreamCount++] = dev->CreateStream(NULL, 0, mesh.Data->VertList.Size(), vtxStride);

        //normal buffer
        vtxPtr = mesh.Data->VertList.getNormals(&vtxStride);
        Assert(vtxPtr && vtxStride);
        TRACE("AnimatedMesh::RenderInitialize(%d): allocating a normal stream for %d verticies (size = %d, type = '%s')\n", __LINE__, mesh.Data->VertList.Size(), vtxStride, mesh.Data->VertList.getFormatName());
		mesh.Data->VertStream[mesh.Data->VertStreamCount++] = dev->CreateStream(NULL, 0, mesh.Data->VertList.Size(), vtxStride);

        //tex coord buffer
        vtxPtr = mesh.Data->VertList.getTexCoord1(&vtxStride);
        Assert(vtxPtr && vtxStride);
        TRACE("AnimatedMesh::RenderInitialize(%d): allocating a tex coord stream for %d verticies (size = %d, type = '%s')\n", __LINE__, mesh.Data->VertList.Size(), vtxStride, mesh.Data->VertList.getFormatName());
		mesh.Data->VertStream[mesh.Data->VertStreamCount++] = dev->CreateStream(NULL, 0, mesh.Data->VertList.Size(), vtxStride);
# else
        // FIXME: the PSP can do skinning in hardware
        //TRACE("%p Initializing AnimatedMesh %d\n", this, i);
        mesh.Data->VertList.getVerts(&vtxStride);
        TRACE("AnimatedMesh::RenderInitialize(%d): allocating a vertex stream for %d verticies (size = %d, type = '%s')\n", __LINE__, mesh.Data->VertList.Size(), vtxStride, mesh.Data->VertList.getFormatName());
		mesh.Data->VertStream[mesh.Data->VertStreamCount++] = dev->CreateStream(NULL, 0, mesh.Data->VertList.Size(), vtxStride);
# endif
#else
        mesh.Data->VertStream=dev->CreateStream(NULL, RenderDevice::EMeshVertexNTU, mesh.Data->VertList.Size(), sizeof(MeshVertex));
#endif

        // static index buffer
#ifndef TRANSGAMING
        if (mesh.Elems.Size())
		{
# ifdef TRANSGAMING_MESH_WARNINGS
            TRACE("AnimatedMesh::RenderInitialize(): WARNING-> using indexed tristripped vertex data for the mesh '%s'\n", mesh.Name.AsChar());
# endif
			mesh.PrimType=RenderDevice::EPrimTriStrip;
            mesh.Data->ElemStream=dev->CreateStream(mesh.Elems.Elements(),RenderDevice::EIndex16,mesh.Elems.Size(),sizeof(WORD));
		}
		else
#endif
        if (mesh.Data->Triangles.Size())
		{
#ifdef TRANSGAMING_MESH_WARNINGS
            TRACE("AnimatedMesh::RenderInitialize(): WARNING-> using indexed trilist vertex data for the mesh '%s'\n", mesh.Name.AsChar());
#endif
#ifndef TRANSGAMING_DEINDEX
			mesh.PrimType=RenderDevice::EPrimTriList;
#endif
			mesh.Data->ElemStream=dev->CreateStream(mesh.Data->Triangles.Elements(),RenderDevice::EIndex16,mesh.Data->Triangles.Size(),sizeof(WORD));
		}
#ifdef TRANSGAMING_DEINDEX
        else if (!mesh.isIndexed){
# ifdef TRANSGAMING_MESH_WARNINGS
            TRACE("AnimatedMesh::RenderInitialize(): using deindexed %s vertex data for the mesh '%s'\n", mesh.PrimType == RenderDevice::EPrimTriList ? "trilist" : "tristripped", mesh.Name.AsChar());
# endif
            mesh.Data->ElemStream = EInvalidIndex;
        }
#endif
		else 
		{
			Assert(!"Invalid PrimType");
		}

        if(mesh.Data->VertStream[0] == EInvalidIndex || mesh.Data->VertStreamCount == 0) {
            TRACE("AnimatedMesh::RenderInitialize(): ERROR-> could not create vertex streams for mesh: %s, skipping\n",
                        mesh.Name.AsChar());
            continue;
        }
        
#ifdef TRANSGAMING_DEINDEX
        if (mesh.isIndexed && EInvalidIndex == mesh.Data->ElemStream) {
#else
        if (EInvalidIndex == mesh.Data->ElemStream) {
#endif
            TRACE("AnimatedMesh::RenderInitialize(): ERROR-> could not create index stream for mesh: %s, skipping\n",
                        mesh.Name.AsChar());
            continue;
        }

    }
}

void AnimatedMesh::RenderCleanup()
{
	RenderDevice* dev=GEngine->GetRenderDevice();
    if (!dev) return;

	MaterialSystem* matSys=GEngine->Video->MaterialSys;

	for (int iMesh=0; iMesh<Meshes.Size(); iMesh++)
	{
        MeshShape& mesh=Meshes[iMesh];

        // dynamic vertex buffer
	    int ids[] = { mesh.Data->ElemStream, mesh.WeightsStream };
	    dev->ReleaseResource(RenderDevice::EResBuffer, mesh.Data->VertStream, mesh.Data->VertStreamCount);
	    dev->ReleaseResource(RenderDevice::EResBuffer, ids, ARRAY_COUNT(ids));

        for (int i = 0; i < VertexList::MaxStreamCount; i++)
            mesh.Data->VertStream[i] = EInvalidIndex;
        mesh.Data->ElemStream = EInvalidIndex;
        mesh.WeightsStream = EInvalidIndex;
        mesh.Data->VertStreamCount = 0;


		for (int j=0; j<mesh.Data->Groups.Size(); j++)
		{
			matSys->ReleaseTexture(mesh.Data->Groups[j].TexPtr );
			matSys->ReleaseTexture(mesh.Data->Groups[j].TexEmPtr );
		}

		matSys->ReleaseTexture(mesh.LightMapTex);
#ifndef TRANSGAMING
        matSys->ReleaseTexture(mesh.NormalMapTex);
		matSys->ReleaseTexture(mesh.DetailMap.TexPtr);
#endif
		matSys->ReleaseTexture(mesh.CubeMapTex);
		matSys->ReleaseTexture(mesh.AlphaMap.TexPtr);
	}
}

#ifdef CHEAPDEINDEX

static void AnimateMesh(MeshShape &mesh, Matrix *matrices)
{
	Assert(mesh.Data->VertList.getFormat() == VertexList::VTXFMT_MESH);

	if (DDP_SkipAnimatedMeshes)
		return;

	int			 stride;
	int			 size    = mesh.Data->Triangles.Size();
	Vector4		*dpos    = ((Vector4*)mesh.Data->ps2_Verts[mesh.Data->RenderInstance].Lock(mesh.Data->Triangles.Size())  );
	Vector4		*spos    = mesh.Data->VertList.getPositions(&stride);
	TexCoord	*duvs    = ((TexCoord*)mesh.Data->ps2_UVs[mesh.Data->RenderInstance].Lock(mesh.Data->Triangles.Size()));
	TexCoord	*suvs    = mesh.Data->VertList.getTexCoord1(&stride);
	Vector		*dnor    = ((Vector*)mesh.Data->ps2_Normals[mesh.Data->RenderInstance].Lock(mesh.Data->Triangles.Size()));
	Vector		*snor    = mesh.Data->VertList.getNormals(&stride);

	for (int rep=0; rep<size; rep++)
	{
		int         idx = mesh.Data->Triangles[rep];
		Vector4 *   srcPos = &spos[idx];
		Vector *    srcNor = &snor[idx];
		DynamicArray<Weight> &weightsArr = mesh.Influences[idx].Weights;

		int			bones = weightsArr.Size();
		if (bones==0)
		{
			*dpos = *srcPos;
			*dnor = *srcNor;
#ifdef TRANSGAMING_SHOW_FPS
			g_AnimatedVerts[0]++;
#endif
		}
		else if (bones/*==1*/)
		{
			Weight* weights = &weightsArr[0];
			const Matrix &m0 = matrices[weights[0].bone];

			dpos->X  = srcPos->X * m0.A00 + srcPos->Y * m0.A10 + srcPos->Z * m0.A20 + m0.A30;
			dpos->Y  = srcPos->X * m0.A01 + srcPos->Y * m0.A11 + srcPos->Z * m0.A21 + m0.A31;
			dpos->Z  = srcPos->X * m0.A02 + srcPos->Y * m0.A12 + srcPos->Z * m0.A22 + m0.A32;

			dnor->X  = srcNor->X * m0.A00 + srcNor->Y * m0.A10 + srcNor->Z * m0.A20;
			dnor->Y  = srcNor->X * m0.A01 + srcNor->Y * m0.A11 + srcNor->Z * m0.A21;
			dnor->Z  = srcNor->X * m0.A02 + srcNor->Y * m0.A12 + srcNor->Z * m0.A22;

#ifdef TRANSGAMING_SHOW_FPS
			g_AnimatedVerts[1]++;
#endif
		}
/*		else if (bones==2)
		{
			Weight* weights = &weightsArr[0];
			const Matrix &m0 = matrices[weights[0].bone];
			const Matrix &m1 = matrices[weights[1].bone];
			const float w0 = weights[0].weight;
			const float w1 = weights[1].weight;

			dpos->X  = (srcPos->X * m0.A00 + srcPos->Y * m0.A10 + srcPos->Z * m0.A20 + m0.A30) * w0
					 + (srcPos->X * m1.A00 + srcPos->Y * m1.A10 + srcPos->Z * m1.A20 + m1.A30) * w1;
			dpos->Y  = (srcPos->X * m0.A01 + srcPos->Y * m0.A11 + srcPos->Z * m0.A21 + m0.A31) * w0
					 + (srcPos->X * m1.A01 + srcPos->Y * m1.A11 + srcPos->Z * m1.A21 + m1.A31) * w1;
			dpos->Z  = (srcPos->X * m0.A02 + srcPos->Y * m0.A12 + srcPos->Z * m0.A22 + m0.A32) * w0
					 + (srcPos->X * m1.A02 + srcPos->Y * m1.A12 + srcPos->Z * m1.A22 + m1.A32) * w1;
			dpos->W= 1.0f;

			dnor->X  = (srcNor->X * m0.A00 + srcNor->Y * m0.A10 + srcNor->Z * m0.A20) * w0
					 + (srcNor->X * m1.A00 + srcNor->Y * m1.A10 + srcNor->Z * m1.A20) * w1;
			dnor->Y  = (srcNor->X * m0.A01 + srcNor->Y * m0.A11 + srcNor->Z * m0.A21) * w0
					 + (srcNor->X * m1.A01 + srcNor->Y * m1.A11 + srcNor->Z * m1.A21) * w1;
			dnor->Z  = (srcNor->X * m0.A02 + srcNor->Y * m0.A12 + srcNor->Z * m0.A22) * w0
					 + (srcNor->X * m1.A02 + srcNor->Y * m1.A12 + srcNor->Z * m1.A22) * w1;

#ifdef TRANSGAMING_SHOW_FPS
			g_AnimatedVerts[2]++;
#endif
		}
		else
		{
			//TRACE("AnimateMesh using too many bones, only 0/1/2 supported\n");
#ifdef TRANSGAMING_SHOW_FPS
			g_AnimatedVerts[3]++;
#endif

			Weight* weights = &weightsArr[0];
			const Matrix &m0 = matrices[weights[0].bone];

			dpos->X  = srcPos->X * m0.A00 + srcPos->Y * m0.A10 + srcPos->Z * m0.A20 + m0.A30;
			dpos->Y  = srcPos->X * m0.A01 + srcPos->Y * m0.A11 + srcPos->Z * m0.A21 + m0.A31;
			dpos->Z  = srcPos->X * m0.A02 + srcPos->Y * m0.A12 + srcPos->Z * m0.A22 + m0.A32;

			dnor->X  = srcNor->X * m0.A00 + srcNor->Y * m0.A10 + srcNor->Z * m0.A20;
			dnor->Y  = srcNor->X * m0.A01 + srcNor->Y * m0.A11 + srcNor->Z * m0.A21;
			dnor->Z  = srcNor->X * m0.A02 + srcNor->Y * m0.A12 + srcNor->Z * m0.A22;
		}*/

		duvs->U = suvs[idx].U;
		duvs->V = suvs[idx].V;

		dpos++;
		duvs++;
		dnor++;
	}

	mesh.Data->ps2_Verts[mesh.Data->RenderInstance].Unlock();
	mesh.Data->ps2_UVs[mesh.Data->RenderInstance].Unlock();
	mesh.Data->ps2_Normals[mesh.Data->RenderInstance].Unlock();
}
#elif defined(TRANSGAMING_PS2)

static void AnimateMesh(MeshShape &mesh, Matrix *matrices, RenderDevice* dev)
{
	Assert(mesh.Data->VertList.getFormat() == VertexList::VTXFMT_MESH);

	int			 stride;
	int			 size    = mesh.Data->VertList.Size();

	Vector4		*dpos    = (Vector4*)  dev->LockStream(mesh.Data->VertStream[0], 0, size);
	Vector4		*spos    = mesh.Data->VertList.getPositions(&stride);
	TexCoord	*duvs    = (TexCoord*)dev->LockStream(mesh.Data->VertStream[2], 0, size);
	TexCoord	*suvs    = mesh.Data->VertList.getTexCoord1(&stride);
	Vector		*dnor     = (Vector*)dev->LockStream(mesh.Data->VertStream[1], 0, size);
	Vector		*snor    = mesh.Data->VertList.getNormals(&stride);

	for (int rep=0; rep<size; rep++)
	{
		Vector4		*srcPos = &spos[rep];
		Vector		*srcNor = &snor[rep];
		DynamicArray<Weight> &weightsArr = mesh.Influences[rep].Weights;
		int			bones = weightsArr.Size();

		if (bones==0)
		{
			*dpos = *spos;
			*dnor = *snor;
		}
		// do an optimized path for 1,2 influences per bone, for >2 do a generic path
		else if (bones==1)
		{
			Weight* weights = &weightsArr[0];
			const Matrix &m0 = matrices[weights[0].bone];

			dpos->X  = spos->X * m0.A00 + spos->Y * m0.A10 + spos->Z * m0.A20 + m0.A30;
			dpos->Y  = spos->X * m0.A01 + spos->Y * m0.A11 + spos->Z * m0.A21 + m0.A31;
			dpos->Z  = spos->X * m0.A02 + spos->Y * m0.A12 + spos->Z * m0.A22 + m0.A32;
			dpos->W= 1.0f;

			dnor->X  = snor->X * m0.A00 + snor->Y * m0.A10 + snor->Z * m0.A20;
			dnor->Y  = snor->X * m0.A01 + snor->Y * m0.A11 + snor->Z * m0.A21;
			dnor->Z  = snor->X * m0.A02 + snor->Y * m0.A12 + snor->Z * m0.A22;
		}
		else if (bones==2)
		{
			Weight* weights = &weightsArr[0];
			const Matrix &m0 = matrices[weights[0].bone];
			const Matrix &m1 = matrices[weights[1].bone];
			const float w0 = weights[0].weight;
			const float w1 = weights[1].weight;

			dpos->X  = (spos->X * m0.A00 + spos->Y * m0.A10 + spos->Z * m0.A20 + m0.A30) * w0
					 + (spos->X * m1.A00 + spos->Y * m1.A10 + spos->Z * m1.A20 + m1.A30) * w1;
			dpos->Y  = (spos->X * m0.A01 + spos->Y * m0.A11 + spos->Z * m0.A21 + m0.A31) * w0
					 + (spos->X * m1.A01 + spos->Y * m1.A11 + spos->Z * m1.A21 + m1.A31) * w1;
			dpos->Z  = (spos->X * m0.A02 + spos->Y * m0.A12 + spos->Z * m0.A22 + m0.A32) * w0
					 + (spos->X * m1.A02 + spos->Y * m1.A12 + spos->Z * m1.A22 + m1.A32) * w1;
			dpos->W= 1.0f;

			dnor->X  = (snor->X * m0.A00 + snor->Y * m0.A10 + snor->Z * m0.A20) * w0
					 + (snor->X * m1.A00 + snor->Y * m1.A10 + snor->Z * m1.A20) * w1;
			dnor->Y  = (snor->X * m0.A01 + snor->Y * m0.A11 + snor->Z * m0.A21) * w0
					 + (snor->X * m1.A01 + snor->Y * m1.A11 + snor->Z * m1.A21) * w1;
			dnor->Z  = (snor->X * m0.A02 + snor->Y * m0.A12 + snor->Z * m0.A22) * w0
					 + (snor->X * m1.A02 + snor->Y * m1.A12 + snor->Z * m1.A22) * w1;
		}
		else
		{
			TRACE("AnimateMesh using too many bones, only 0/1/2 supported\n");
		}

		*duvs = suvs[rep];

		dpos++;
		duvs++;
		dnor++;
	}

	dev->UnlockStream(mesh.Data->VertStream[0]);
	dev->UnlockStream(mesh.Data->VertStream[1]);
	dev->UnlockStream(mesh.Data->VertStream[2]);
}

#endif

void AnimatedMesh::AnimateGeometry(const DynamicArray<Matrix>& matricesArr)
{
	RenderDevice* dev=GEngine->GetRenderDevice();
    
    Matrix *matrices = (Matrix*)&matricesArr[0];

#ifdef TRANSGAMING_SHOW_FPS
	g_AnimatedMeshes++;
#endif

	for (int m=0; m<Meshes.Size(); m++)
	{
        MeshShape &mesh = Meshes[m];
		//MeshVertex* srcp=&mesh.Verts[0];

#ifdef TRANSGAMING_PS2

		if (totalFrameCount!=mesh.Data->CurrentFrame)
		{
			mesh.Data->CurrentFrame	= totalFrameCount;
			mesh.Data->RenderInstance	= 0;
		}
		else
		{
			if (mesh.Data->RenderInstance < MAXSKINMESHRENDERS-1)
				mesh.Data->RenderInstance++;
			else
			{
				TRACE(">>> AnimatedMesh::AnimateGeometry - OUT OF RenderInstance's\n");
				mesh.Data->RenderInstance = 0;
			}
		}

#endif

#ifdef CHEAPDEINDEX
		AnimateMesh(mesh, matrices);
	}
#elif defined(TRANSGAMING_PS2)
		AnimateMesh(mesh, matrices, dev);
	}
#else
#ifdef TRANSGAMING
		RenderVertexN1 * dstp = (RenderVertexN1 *)dev->LockStream(mesh.Data->VertStream[0], 0, 0);
#else
		MeshVertex* dstp=(MeshVertex*)dev->LockStream(mesh.Data->VertStream,0,0);
#endif

		Assert(dstp);


		int size = mesh.Data->VertList.Size();
		for (int rep=0; rep<size; rep++)
		{
            const RenderVertexN1 &srcmv = mesh.Data->VertList.getRNVert(rep);
			//MeshVertex &srcmv = srcp[rep];
# ifdef TRANSGAMING
			RenderVertexN1 &dstmv = dstp[rep];
# else
			MeshVertex &dstmv = dstp[rep];
# endif

            Vector   *spos = (Vector*)&srcmv.Pos;
            Vector   *snor = (Vector*)&srcmv.Normal;
	        Vector *dpos = (Vector*)&dstmv.Pos;
            Vector *dnor = (Vector*)&dstmv.Normal;
			DynamicArray<Weight> &weightsArr = mesh.Influences[rep].Weights;
            int size = weightsArr.Size();

			if (size==0)
			{
				*dpos = *spos;
				*dnor = *snor;
			}
            // do an optimized path for 1,2 influences per bone, for >2 do a generic path
			else if (size==1)
            {
	            Weight* weights = &weightsArr[0];
				const Matrix &m0 = matrices[weights[0].bone];

    			dpos->X  = spos->X * m0.A00 + spos->Y * m0.A10 + spos->Z * m0.A20 + m0.A30;
				dpos->Y  = spos->X * m0.A01 + spos->Y * m0.A11 + spos->Z * m0.A21 + m0.A31;
				dpos->Z  = spos->X * m0.A02 + spos->Y * m0.A12 + spos->Z * m0.A22 + m0.A32;

    			dnor->X  = snor->X * m0.A00 + snor->Y * m0.A10 + snor->Z * m0.A20;
				dnor->Y  = snor->X * m0.A01 + snor->Y * m0.A11 + snor->Z * m0.A21;
				dnor->Z  = snor->X * m0.A02 + snor->Y * m0.A12 + snor->Z * m0.A22;
            }
            else if (size==2)
            {
	            Weight* weights = &weightsArr[0];
				const Matrix &m0 = matrices[weights[0].bone];
				const Matrix &m1 = matrices[weights[1].bone];
				const float w0 = weights[0].weight;
				const float w1 = weights[1].weight;

    			dpos->X  = (spos->X * m0.A00 + spos->Y * m0.A10 + spos->Z * m0.A20 + m0.A30) * w0
						 + (spos->X * m1.A00 + spos->Y * m1.A10 + spos->Z * m1.A20 + m1.A30) * w1;
				dpos->Y  = (spos->X * m0.A01 + spos->Y * m0.A11 + spos->Z * m0.A21 + m0.A31) * w0
						 + (spos->X * m1.A01 + spos->Y * m1.A11 + spos->Z * m1.A21 + m1.A31) * w1;
				dpos->Z  = (spos->X * m0.A02 + spos->Y * m0.A12 + spos->Z * m0.A22 + m0.A32) * w0
						 + (spos->X * m1.A02 + spos->Y * m1.A12 + spos->Z * m1.A22 + m1.A32) * w1;

    			dnor->X  = (snor->X * m0.A00 + snor->Y * m0.A10 + snor->Z * m0.A20) * w0
						 + (snor->X * m1.A00 + snor->Y * m1.A10 + snor->Z * m1.A20) * w1;
				dnor->Y  = (snor->X * m0.A01 + snor->Y * m0.A11 + snor->Z * m0.A21) * w0
						 + (snor->X * m1.A01 + snor->Y * m1.A11 + snor->Z * m1.A21) * w1;
				dnor->Z  = (snor->X * m0.A02 + snor->Y * m0.A12 + snor->Z * m0.A22) * w0
						 + (snor->X * m1.A02 + snor->Y * m1.A12 + snor->Z * m1.A22) * w1;
            }
            else
            {
	            Weight* weights = &weightsArr[0];
				const Matrix &m = matrices[weights[0].bone];
				float w = weights[0].weight;

                Vector pos;
                Vector nor;

    			pos.X = (spos->X * m.A00 + spos->Y * m.A10 + spos->Z * m.A20 + m.A30) * w;
				pos.Y = (spos->X * m.A01 + spos->Y * m.A11 + spos->Z * m.A21 + m.A31) * w;
				pos.Z = (spos->X * m.A02 + spos->Y * m.A12 + spos->Z * m.A22 + m.A32) * w;

    			nor.X = (snor->X * m.A00 + snor->Y * m.A10 + snor->Z * m.A20) * w;
				nor.Y = (snor->X * m.A01 + snor->Y * m.A11 + snor->Z * m.A21) * w;
				nor.Z = (snor->X * m.A02 + snor->Y * m.A12 + snor->Z * m.A22) * w;

                for (int i=1; i<size; i++)
				{
					const Matrix &m = matrices[weights[i].bone];
					float w = weights[i].weight;

     				pos.X += (spos->X * m.A00 + spos->Y * m.A10 + spos->Z * m.A20 + m.A30) * w;
					pos.Y += (spos->X * m.A01 + spos->Y * m.A11 + spos->Z * m.A21 + m.A31) * w;
					pos.Z += (spos->X * m.A02 + spos->Y * m.A12 + spos->Z * m.A22 + m.A32) * w;

    				nor.X += (snor->X * m.A00 + snor->Y * m.A10 + snor->Z * m.A20) * w;
					nor.Y += (snor->X * m.A01 + snor->Y * m.A11 + snor->Z * m.A21) * w;
					nor.Z += (snor->X * m.A02 + snor->Y * m.A12 + snor->Z * m.A22) * w;
                }
    			*dpos  = pos;
    			*dnor  = nor;
            }
			dstmv.TU.S     = srcmv.TU.S;
			dstmv.TU.T     = srcmv.TU.T;
		}
		dev->UnlockStream(mesh.Data->VertStream[0]);
	}
#endif
}

#ifndef TRANSGAMING
void AnimatedMesh::AnimateTangentSpace(const DynamicArray<Matrix>& matricesArr)
{
	RenderDevice* dev=GEngine->GetRenderDevice();
    
    Matrix *matrices = (Matrix*)&matricesArr[0];

	for (int m=0; m<Meshes.Size(); m++)
	{
        MeshShape &mesh = Meshes[m];
		if (mesh.NormStream == EInvalidIndex)
			return;

		TangentBasis* dst=(TangentBasis*)dev->LockStream(mesh.NormStream,0,0);
		Assert(dst);

		TangentBasis* srcp=&mesh.Tangents[0];

		int size = mesh.Data->VertList.Size();
		for (int rep=0; rep<size; rep++)
		{
	        Vector *dtan = (Vector*)&dst[rep].Tangent;
            Vector *dbin = (Vector*)&dst[rep].Binormal;
            Vector *stan = (Vector*)&srcp[rep].Tangent;
            Vector *sbin = (Vector*)&srcp[rep].Binormal;

			DynamicArray<Weight> &weightsArr = mesh.Influences[rep].Weights;
            int size = weightsArr.Size();


			if (size==0)
			{
				*dtan = *stan;
				*dbin = *sbin;
			}
            // do an optimized path for 1,2 influences per bone, for >2 do a generic path
			else if (size==1)
            {
	            Weight* weights = &weightsArr[0];
				const Matrix &m0 = matrices[weights[0].bone];

    			dtan->X  = stan->X * m0.A00 + stan->Y * m0.A10 + stan->Z * m0.A20;
				dtan->Y  = stan->X * m0.A01 + stan->Y * m0.A11 + stan->Z * m0.A21;
				dtan->Z  = stan->X * m0.A02 + stan->Y * m0.A12 + stan->Z * m0.A22;

    			dbin->X  = sbin->X * m0.A00 + sbin->Y * m0.A10 + sbin->Z * m0.A20;
				dbin->Y  = sbin->X * m0.A01 + sbin->Y * m0.A11 + sbin->Z * m0.A21;
				dbin->Z  = sbin->X * m0.A02 + sbin->Y * m0.A12 + sbin->Z * m0.A22;
            }
            
            else if (size==2)
            {
	            Weight* weights = &weightsArr[0];
				const Matrix &m0 = matrices[weights[0].bone];
				const Matrix &m1 = matrices[weights[1].bone];
				const float w0 = weights[0].weight;
				const float w1 = weights[1].weight;

    			dtan->X  = (stan->X * m0.A00 + stan->Y * m0.A10 + stan->Z * m0.A20) * w0
						 + (stan->X * m1.A00 + stan->Y * m1.A10 + stan->Z * m1.A20) * w1;
				dtan->Y  = (stan->X * m0.A01 + stan->Y * m0.A11 + stan->Z * m0.A21) * w0
						 + (stan->X * m1.A01 + stan->Y * m1.A11 + stan->Z * m1.A21) * w1;
				dtan->Z  = (stan->X * m0.A02 + stan->Y * m0.A12 + stan->Z * m0.A22) * w0
						 + (stan->X * m1.A02 + stan->Y * m1.A12 + stan->Z * m1.A22) * w1;

    			dbin->X  = (sbin->X * m0.A00 + sbin->Y * m0.A10 + sbin->Z * m0.A20) * w0
						 + (sbin->X * m1.A00 + sbin->Y * m1.A10 + sbin->Z * m1.A20) * w1;
				dbin->Y  = (sbin->X * m0.A01 + sbin->Y * m0.A11 + sbin->Z * m0.A21) * w0
						 + (sbin->X * m1.A01 + sbin->Y * m1.A11 + sbin->Z * m1.A21) * w1;
				dbin->Z  = (sbin->X * m0.A02 + sbin->Y * m0.A12 + sbin->Z * m0.A22) * w0
						 + (sbin->X * m1.A02 + sbin->Y * m1.A12 + sbin->Z * m1.A22) * w1;
            }

            else
            {
	            Weight* weights = &weightsArr[0];
				const Matrix &m = matrices[weights[0].bone];
				float w = weights[0].weight;

                Vector tan;
                Vector bin;

    			tan.X = (stan->X * m.A00 + stan->Y * m.A10 + stan->Z * m.A20) * w;
				tan.Y = (stan->X * m.A01 + stan->Y * m.A11 + stan->Z * m.A21) * w;
				tan.Z = (stan->X * m.A02 + stan->Y * m.A12 + stan->Z * m.A22) * w;

    			bin.X = (sbin->X * m.A00 + sbin->Y * m.A10 + sbin->Z * m.A20) * w;
				bin.Y = (sbin->X * m.A01 + sbin->Y * m.A11 + sbin->Z * m.A21) * w;
				bin.Z = (sbin->X * m.A02 + sbin->Y * m.A12 + sbin->Z * m.A22) * w;

                for (int i=1; i<size; i++)
				{
					const Matrix &m = matrices[weights[i].bone];
					float w = weights[i].weight;

    				tan.X += (stan->X * m.A00 + stan->Y * m.A10 + stan->Z * m.A20) * w;
					tan.Y += (stan->X * m.A01 + stan->Y * m.A11 + stan->Z * m.A21) * w;
					tan.Z += (stan->X * m.A02 + stan->Y * m.A12 + stan->Z * m.A22) * w;

    				bin.X += (sbin->X * m.A00 + sbin->Y * m.A10 + sbin->Z * m.A20) * w;
					bin.Y += (sbin->X * m.A01 + sbin->Y * m.A11 + sbin->Z * m.A21) * w;
					bin.Z += (sbin->X * m.A02 + sbin->Y * m.A12 + sbin->Z * m.A22) * w;
				}

    			*dtan  = tan;
    			*dbin  = bin;
            }

			dst[rep].TU.S     = srcp[rep].TU.S;
			dst[rep].TU.T     = srcp[rep].TU.T;
		}

		dev->UnlockStream(mesh.NormStream);
	} 
}
#endif

void AnimatedMesh::SetBindPoseFrame(const Model* model) const
{
    RenderDevice* dev=GEngine->GetRenderDevice();

	for (int m=0; m<Meshes.Size(); m++)
	{
        //!!// for now, until the HW skinning animation system is in, we're gonna assume the
        //!!// mesh is in the native format (RN1)
        Assert(Meshes[m].Data->VertList.getFormat() == VertexList::VTXFMT_MESH);

#ifdef TRANSGAMING_PS2
# ifdef CHEAPDEINDEX
        MeshShape & mesh = Meshes[m];
		Vector4 *   dstp = (Vector4 *)  dev->LockStream(Meshes[m].Data->ps2_Verts[mesh.Data->RenderInstance].GetStream(), 0, 0);
		Vector *    dstn = (Vector *)   dev->LockStream(Meshes[m].Data->ps2_Normals[mesh.Data->RenderInstance].GetStream(), 0, 0);
		TexCoord *  dstt = (TexCoord *) dev->LockStream(Meshes[m].Data->ps2_UVs[mesh.Data->RenderInstance].GetStream(),   0, 0);
# else
		Vector4 *   dstp = (Vector4 *)  dev->LockStream(Meshes[m].Data->VertStream[0], 0, 0);
		Vector *    dstn = (Vector *)   dev->LockStream(Meshes[m].Data->VertStream[1], 0, 0);
		TexCoord *  dstt = (TexCoord *) dev->LockStream(Meshes[m].Data->VertStream[2], 0, 0);
# endif

		Assert(dstp);
		Assert(dstn);
		Assert(dstt);

        Meshes[m].Data->VertList.copyPositions(dstp,  VertexList::VTXFMT_MESH);
        Meshes[m].Data->VertList.copyNormals(dstn,    VertexList::VTXFMT_MESH);
        Meshes[m].Data->VertList.copyTex1(dstt,       VertexList::VTXFMT_MESH);

# ifdef CHEAPDEINDEX
		dev->UnlockStream(Meshes[m].Data->ps2_Verts[mesh.Data->RenderInstance].GetStream());
		dev->UnlockStream(Meshes[m].Data->ps2_Normals[mesh.Data->RenderInstance].GetStream());
		dev->UnlockStream(Meshes[m].Data->ps2_UVs[mesh.Data->RenderInstance].GetStream());
# else
		dev->UnlockStream(Meshes[m].Data->VertStream[0]);
		dev->UnlockStream(Meshes[m].Data->VertStream[1]);
		dev->UnlockStream(Meshes[m].Data->VertStream[2]);
# endif
#else
# ifdef TRANSGAMING
		RenderVertexN1 *    dst = (RenderVertexN1 *)dev->LockStream(Meshes[m].Data->VertStream[0], 0, 0);
# else
		//MeshVertex* dst=(MeshVertex*)dev->LockStream(Meshes[m].Data->VertStream,0,0);
# endif

		Assert(dst);


        Meshes[m].Data->VertList.copy(dst, VertexList::VTXFMT_MESH);
		
        /*MeshVertex* src=&Meshes[m].Data->VertList[0];

		int size = Meshes[m].Data->VertList.Size();

#ifdef TRANSGAMING
        for (unsigned v=0; v<size; v++)
            dst[v] = src[v]; // invoke implicit copy constructor
#else
        memcpy(dst, src, size*sizeof(MeshVertex));
#endif//*/

		dev->UnlockStream(Meshes[m].Data->VertStream[0]);
#endif
	}
}

void AnimatedMesh::Render(Model* model, int mode) const
{
	if (mode & View::EModeVolPass)
	{
		RenderVolume(model);
	}
	else if (mode & View::EModeShadowMapPass)
	{
		if (model->ShadowMap) 
		{
#ifdef	TRANSGAMING
			model->ShadowUpdateFrame = GEngine->pcfWorld->Frame;
#else
			model->ShadowUpdateFrame = GEngine->World->Frame;
#endif
			RenderShadowmap(model);
		}
	}
	else if (model->Demonic) 
	{
		if (mode & View::EModeDemonFX)	{
			RenderDemonFX(model);
		}
		else if (mode & View::EModeGhostFX)	{
			RenderGhostFX(model);
		}
	}
	else if (mode & View::EModeWarpPass)
	{
		RenderWarpFX(model);
	}
	else  	
	{
		RenderDefault(model, mode);
	}
}

void AnimatedMesh::RenderDefault(Model* model, int mode) const
{
	Plane tmp0(0,0,0,0);
    RenderDevice* dev=GEngine->GetRenderDevice();

	Matrix WorldToObject = model->ObjectToWorld.Inverse();

	const Vector& eyeVec = GEngine->Video->GetActiveView()->LocalViewport.EyePosition;
	Vector eyeDir = eyeVec - model->ObjectToWorld.GetTranslation();
	eyeDir = WorldToObject.TransformNormal(eyeDir);
	eyeDir.Normalize();

	Texture* notex = GEngine->Video->MaterialSys->Textures.NoTex;

	for (int i=0; i<Meshes.Size(); i++) 
	{
		MeshShape &mesh = Meshes[i];

		if (!model->MeshPropertiesArr[i].IsVisible)
			continue;

		if (mesh.IsWarp && !(mode & View::EModeWarpPass))
			continue;

#ifdef TRANSGAMING_DEINDEX
        if (!mesh.isIndexed)
            dev->SetStreams(mesh.Data->VertStream, mesh.Data->VertStreamCount, EInvalidIndex);

        else
#endif
            dev->SetStreams(mesh.Data->VertStream, mesh.Data->VertStreamCount, mesh.Data->ElemStream);

		int numPasses = max(1, min(model->LightCount, mesh.DefaultShader->PassCount));
		for (int iPass=0; iPass<numPasses; ++iPass)
		{
			Plane lightDir(0,-1,0,0);
			Plane lightCol(0,0,0,0);
			Plane ambientCol(0,0,0,0);
#ifdef TRANSGAMING
            Plane tmpPlane(0, 0, 0, 0);
#endif

            if (model->LightCount!=0) 
            {
                Light* light = model->Lights[iPass];

                light->GetDirAndAttCol(model, lightDir, lightCol);

                lightDir.Normal = WorldToObject.TransformNormal(lightDir.Normal);
                lightDir.Normal.Normalize();

                Vector halfDir = (eyeDir+lightDir.Normal);
                halfDir.Normalize();

                dev->SetShaderMemory(13,&lightDir,1);

#ifdef TRANSGAMING
                tmpPlane.set(halfDir);

                dev->SetShaderMemory(14,&tmpPlane,1);
#else
                dev->SetShaderMemory(14,&Plane(halfDir),1);
#endif
            }

			// set ambient in pass 0
			dev->SetPixelShaderMemory(1,&lightCol,1);
			if (iPass==0)
			{
#ifdef	TRANSGAMING
			    Color_TG tmpColor = model->GetEnvironmentAmbient();
				COLOR2PLANE(tmpPlane, tmpColor);

				dev->SetPixelShaderMemory(0,&tmpPlane,1);
#else
				dev->SetPixelShaderMemory(0,&Plane(model->GetEnvironmentAmbient()),1);
#endif
			}
			else
			{
//				dev->SetPixelShaderMemory(0,&Plane(0,0,0,0),1);
				dev->SetPixelShaderMemory(0,&tmp0,1);
			}

			RenderPass& pass = mesh.DefaultShader->Passes[iPass];	
			pass.Activate();

			MeshProperties &meshProp=model->MeshPropertiesArr[i];

			for(int sec=0; sec<mesh.Data->Groups.Size(); ++sec) 
			{
				SimpleMesh::Group &group=mesh.Data->Groups[sec];
				MeshProperties::GroupProperties &groupProp=model->MeshPropertiesArr[i].Groups[sec];
		
#ifdef TRANSGAMING
                MaterialInput tmpMaterial(groupProp.TexPtr?groupProp.TexPtr:group.TexPtr, notex, 
					                      meshProp.NormalMapPtr?meshProp.NormalMapPtr: notex);     //mesh.NormalMapTex);

                pass.SetInput(&tmpMaterial);


# ifdef TRANSGAMING_DEINDEX
                if (!mesh.isIndexed)
				    dev->Draw(mesh.PrimType,group.RenderBase,group.RenderCount);

                else
# endif
				    dev->Draw(mesh.PrimType,group.RenderBase,group.RenderCount,group.VertexMin,group.VertexCount);
#else
				pass.SetInput(&MaterialInput(
					groupProp.TexPtr?groupProp.TexPtr:group.TexPtr, notex, 
					meshProp.NormalMapPtr?meshProp.NormalMapPtr:mesh.NormalMapTex));


				dev->Draw(mesh.PrimType,group.RenderBase,group.RenderCount,group.VertexMin,group.VertexCount);
#endif
			}
		}
	}
}

void AnimatedMesh::RenderVolume(const Model* model) const
{
}

void AnimatedMesh::RenderShadowmap(const Model* model) const
{
}

void AnimatedMesh::RenderGhostFX(Model* model) const
{
}

void AnimatedMesh::RenderDemonFX(Model* model) const
{
}

void AnimatedMesh::RenderWarpFX(Model* model) const
{
}

DynamicArray<Matrix> AnimatedMesh::ComputeBindMatrix(const Matrix &world) const
{
    int index = 0;
    DynamicArray<Matrix> bindMatrixArr;
    bindMatrixArr.ResizeArray(Skeleton.Joints.Size());

    for (int i=0; i<Skeleton.Hierarchies; i++)
    {
       ComputeBindMatrix(world, bindMatrixArr, index);
    }

    return bindMatrixArr;
}

void AnimatedMesh::ComputeBindMatrix(Matrix m, const DynamicArray<Matrix>& matrix, int& index) const
{
    Joint& joint = Skeleton.Joints[index];
    m = joint.Mat*m;
    matrix[index++] = m;

    for (int iChild=0; iChild<joint.Children; iChild++)
    {
        ComputeBindMatrix(m, matrix, index);
    }
}

#ifdef TRANSGAMING
MeshShape* AnimatedMesh::FindMeshByName(const char *name) const
{
    for (int iMesh=0; iMesh<Meshes.Size(); iMesh++)
    {
        MeshShape& mesh = Meshes[iMesh];
		if (!strcmp(mesh.Name.AsChar(), name))
			return &mesh;
    }

	return NULL;
}

int AnimatedMesh::FindAnimationByName(const char *name) const
{
	for (int iAnim=0; iAnim<Animations.Size(); iAnim++)
    {
        ModelAnimation* ani = Animations[iAnim];
		if (!stricmp(ani->Name.AsChar(), name))
			return iAnim;
    }

	return -1;
}
#else
MeshShape* AnimatedMesh::FindMeshByName(const String& name) const
{
    for (int iMesh=0; iMesh<Meshes.Size(); iMesh++)
    {
        MeshShape& mesh = Meshes[iMesh];
		if (mesh.Name == name)
			return &mesh;
    }

	return NULL;
}

int AnimatedMesh::FindAnimationByName(const String& name) const
{
	for (int iAnim=0; iAnim<Animations.Size(); iAnim++)
    {
        ModelAnimation* ani = Animations[iAnim];
		if (!stricmp(ani->Name.AsChar(), name.AsChar()))
			return iAnim;
    }

	return -1;
}
#endif


void AnimatedMesh::Load(FileObject *file)
{
	Object::Load(file);
    *file >> Skeleton >> Meshes;
	for (int i=0; i<Meshes.Size(); i++)
	{
		MeshShape &m = Meshes[i];
		for (int g=m.Data->Groups.Size()-1; g>=0; g--)
		{
			if (m.Data->Groups[g].RenderCount==0)
				m.Data->Groups.Remove(g);
		}
		m.Init();
	}

#ifdef ANNOYING_GRAPHICS_LOGS
	int vTotal=0, tTotal=0, sTotal=0;
	MLog.Print("Animated Mesh (%d submeshes)\n", Meshes.Size());
	for (int i=0; i<Meshes.Size(); i++)
	{
		MeshShape &m = Meshes[i];
		int v = m.Data->VertList.Size();
		int t = m.Data->Triangles.Size()/3;
		int s = m.Elems.Size();
		MLog.Print("%30s Verts: %4d Tris: %4d Strip: %4d\n", m.Name.AsChar(), v, t, s);
		vTotal += v;
		tTotal += t;
		sTotal += s;
	}
	MLog.Print("Total Verts: %d Tris: %d Strip: %d\n", vTotal, tTotal, sTotal);
#endif

#if 0
	for (int i=0; i<Meshes.Size(); i++)
	{
		MeshShape &m = Meshes[i];
		MLog.Print("Mesh: %s\n", m.Name);
		const int maxBones = 128;
		WORD bones[maxBones];
		float infl[maxBones];
		for (int j=0; j<maxBones; j++)
		{
			bones[j] = 0;
			infl[j] = 0;
		}
		int bonenum = 0;
		int maxinfl = 0;
		int mininfl = maxBones;

		for (int j=0; j<m.Influences.Size(); j++)
		{
			Influence &n = m.Influences[j];
			int s = n.Weights.Size();

			if (s>maxinfl)
				maxinfl = s;

			if (s<mininfl)
				mininfl = s;

			for(int k=0; k<s; k++)
			{
				int b = n.Weights[k].bone;
				Assert(b>=0 && b<128);
				if (bones[b]==0)
					bonenum++;
				bones[b] ++;
				infl[b] += n.Weights[k].weight;
			}
		}
		MLog.Print("Bonenum %d Max bps %d Min bps %d\n", bonenum, maxinfl, mininfl);

		for (int j=0; j<maxBones; j++)
			if (bones[j])
				MLog.Print("%d: %d - %f", j, bones[j], infl[j]);
		MLog.Print("\n");
	}
#endif
}

void AnimatedMesh::Save(FileObject *file)
{
	Object::Save(file);
    *file << Skeleton << Meshes;
}

void AnimatedMeshMatPal::RenderInitialize()
{
	if( !GEngine->Video ) return;
	RenderDevice* dev=GEngine->GetRenderDevice();
	MaterialSystem* matSys=GEngine->Video->MaterialSys;

#ifdef ANNOYING_GRAPHICS_LOGS
	MLog.Print("Creating Render Data\n");
#endif

	for(int i=0; i<Meshes.Size(); ++i) 
	{
        MeshShape& mesh=Meshes[i];

		// setup shaders

		mesh.VolShader		= matSys->CreateMaterial("volpalskin");
		mesh.ShadowMapShader= matSys->CreateMaterial("palskinnedshadow");
		mesh.BlurShader		= matSys->CreateMaterial("blur");
		mesh.GhostFXShader	= matSys->CreateMaterial("ghostfx");
		mesh.RenderType		= MeshShape::EInvalid;

#ifdef TRANSGAMING_NON_TNL
		if (UseNormalMaps && mesh.NormalMapName.Len() && dev->HWClass>=RenderDevice::EHWClassNV20)
		{
#ifdef TRANSGAMING
			Texture* NormalMapTex = CreateTexture(mesh.NormalMapName.AsChar(), 0, RenderDevice::ETexLODWeapons);
#else
			Texture* NormalMapTex = CreateTexture(mesh.NormalMapName, 0, RenderDevice::ETexLODWeapons);
#endif

			if (!(NormalMapTex->Flags & Texture::ETexInvalid))
			{
				mesh.NormalMapTex	= NormalMapTex;

				mesh.DefaultShader	= matSys->CreateMaterial("palskinnedperpixel");
	
				mesh.RenderType     = MeshShape::EPixelGPUSkin;
			}
			else
			{
				matSys->ReleaseTexture(NormalMapTex);
			}
		}
#endif //TRANSGAMING_NON_TNL
		// setup textures

		bool hasEmissive = false;
		for (int j=0; j<mesh.Data->Groups.Size(); j++)
		{
#ifdef TRANSGAMING
            char tmpName[MAX_PATH];


            snprintf(tmpName, MAX_PATH, "%s_add", mesh.Data->Groups[j].TexName.AsChar());
			Texture* EmissiveTexture = CreateTexture(tmpName, 0, RenderDevice::ETexLODModels); 			
#else
			Texture* EmissiveTexture = CreateTexture(mesh.Data->Groups[j].TexName+"_add", 0, RenderDevice::ETexLODModels); 			
#endif

			if (EmissiveTexture->Flags & Texture::ETexInvalid) {
				matSys->ReleaseTexture(EmissiveTexture);
			}
			else {
				hasEmissive = true;
				mesh.Data->Groups[j].TexEmPtr = EmissiveTexture;
			}
		}

		for (int j=0; j<mesh.Data->Groups.Size(); j++)
		{
#ifdef TRANSGAMING
			if (mesh.RenderType==MeshShape::EPixelGPUSkin)
				mesh.Data->Groups[j].TexPtr = CreateTexture(mesh.Data->Groups[j].TexName.AsChar(), 0, RenderDevice::ETexLODWeapons);
			else
				mesh.Data->Groups[j].TexPtr = CreateTexture(mesh.Data->Groups[j].TexName.AsChar(), 0, RenderDevice::ETexLODModels);
#else
			if (mesh.RenderType==MeshShape::EPixelGPUSkin)
				mesh.Data->Groups[j].TexPtr = CreateTexture(mesh.Data->Groups[j].TexName, 0, RenderDevice::ETexLODWeapons);
			else
				mesh.Data->Groups[j].TexPtr = CreateTexture(mesh.Data->Groups[j].TexName, 0, RenderDevice::ETexLODModels);
#endif
			if (hasEmissive && mesh.Data->Groups[j].TexEmPtr==NULL)
				mesh.Data->Groups[j].TexEmPtr = matSys->Textures.Black;
		}



		if (mesh.RenderType!=MeshShape::EPixelGPUSkin)
		{
			mesh.RenderType     = MeshShape::EVertexGPUSkin;
#ifndef TRANSGAMING
            mesh.NormalMapTex	= matSys->Textures.NoTex;
#endif

#ifdef TRANSGAMING
            char    namelwr[MAX_PATH];
            char    name[MAX_PATH];
            size_t  len;


            strncpy(name, "palskinned", MAX_PATH);
            strncpy(namelwr, mesh.Name.AsChar(), MAX_PATH);
            strlwr(namelwr);
            len = strlen(name);

            mesh.DefaultShader  = matSys->FindMaterial(namelwr);

			if (strstr(namelwr, "water")){			
				mesh.RenderType = MeshShape::EWaterGPUSkin;
                strncpy(&name[len], "_water", MAX_PATH - len);
			}

			if (strstr(namelwr, "add"))
                strncpy(&name[len], "add", MAX_PATH - len);

			//-DDP - HACK!!!! - not supported currentlly
			//-DDP - HACK!!!! - if (hasEmissive)
            //-DDP - HACK!!!! -      strncpy(&name[len], "emissive", MAX_PATH - len);

			if (strstr(namelwr, "2sided"))
                strncpy(&name[len], "2sided", MAX_PATH - len);


			if (!mesh.DefaultShader)
				mesh.DefaultShader	= matSys->CreateMaterial(name);
#else
			mesh.DefaultShader  = matSys->FindMaterial(mesh.Name.LowerCase().AsChar());
			String name = "palskinned";

			if (strstr(mesh.Name.LowerCase().AsChar(),"water")) 
			{			
				mesh.RenderType = MeshShape::EWaterGPUSkin;
				name = "palskinned_water";			
			}

			if (strstr(mesh.Name.LowerCase().AsChar(),"add")) {
				name += "add";
			}

			//-DDP - HACK!!!! - not supported currentlly
			//-DDP - HACK!!!! - if (hasEmissive)
			//-DDP - HACK!!!! - 	name += "emissive";

			if (strstr(mesh.Name.LowerCase().AsChar(),"2sided")) 
				name += "2sided";


			if (!mesh.DefaultShader)
				mesh.DefaultShader	= matSys->CreateMaterial(name.AsChar());
#endif
		}


        // preprocess weights info
		mesh.SetupVertexWeights();

		int usage = 0;
		if (dev->HWClass == RenderDevice::EHWClassGenericTnL)
		{
            void *  vtxPtr;
            int     vtxStride;

            
            usage = RenderDevice::EUsageSWVP;

			// static vertex buffer
#ifdef TRANSGAMING
            Assert(mesh.Data->VertStreamCount == 0);
# if defined(TRANSGAMING_PS2) && !defined(CHEAPDEINDEX)
            // FIXME: the PS2 can do skinning in hardware
            //TRACE("AnimatedMeshMatPal::RenderInitialize(): %p Initializing AnimatedMeshMatPal %d\n", this, i);
            int     vtxCount = mesh.Data->VertList.Size();

            //position stream
            vtxPtr = mesh.Data->VertList.getPositions(&vtxStride);
            Assert(vtxPtr && vtxStride);
            TRACE("AnimatedMeshMatPal::RenderInitialize(%d): allocating a position stream for %d verticies (size = %d, type = '%s')\n", __LINE__, vtxCount, vtxStride, mesh.Data->VertList.getFormatName());
            mesh.Data->VertStream[mesh.Data->VertStreamCount++] = dev->CreateStream(NULL, usage, vtxCount, vtxStride);

            //normal stream
            vtxPtr = mesh.Data->VertList.getNormals(&vtxStride);
            Assert(vtxPtr && vtxStride);
            TRACE("AnimatedMeshMatPal::RenderInitialize(%d): allocating a normal stream for %d verticies (size = %d, type = '%s')\n", __LINE__, vtxCount, vtxStride, mesh.Data->VertList.getFormatName());
            mesh.Data->VertStream[mesh.Data->VertStreamCount++] = dev->CreateStream(NULL, usage, vtxCount, vtxStride);

            //tex coord stream
            vtxPtr = mesh.Data->VertList.getTexCoord1(&vtxStride);
            Assert(vtxPtr && vtxStride);
            TRACE("AnimatedMeshMatPal::RenderInitialize(%d): allocating a tex coord stream for %d verticies (size = %d, type = '%s')\n", __LINE__, vtxCount, vtxStride, mesh.Data->VertList.getFormatName());
            mesh.Data->VertStream[mesh.Data->VertStreamCount++] = dev->CreateStream(NULL, usage, vtxCount, vtxStride);
# elif !defined(CHEAPDEINDEX)
            // FIXME: the PSP can do skinning in hardware
            //TRACE("AnimatedMeshMatPal::RenderInitialize(): %p Initializing AnimatedMeshMatPal %d\n", this, i);
            vtxPtr = mesh.Data->VertList.getVerts(&vtxStride);
            TRACE("AnimatedMeshMatPal::RenderInitialize(%d): allocating a vertex stream for %d verticies (size = %d, type = '%s')\n", __LINE__, mesh.Data->VertList.Size(), vtxStride, mesh.Data->VertList.getFormatName());
            mesh.Data->VertStream[mesh.Data->VertStreamCount++] = dev->CreateStream(NULL, usage, mesh.Data->VertList.Size(), vtxStride);
# endif
#else
            vtxPtr = mesh.Data->VertList.getVerts(&vtxStride);
			mesh.Data->VertStream=dev->CreateStream(NULL, usage|RenderDevice::EMeshVertexNTU, mesh.Data->VertList.Size(), vtxStride);
#endif
		}
#ifdef TRANSGAMING_NON_TNL
		else
		{
            void *  ptr;
            int     stride;

			// static vertex buffer
            ptr = mesh.Data->VertList.getVerts(&stride);
			mesh.Data->VertStream=dev->CreateStream(ptr, usage|RenderDevice::EMeshVertexNTU, mesh.Data->VertList.Size(), stride);

			// static weights buffer
			mesh.WeightsStream=dev->CreateStream(&mesh.Weights[0],usage|RenderDevice::EStreamNone,mesh.Data->VertList.Size(),sizeof(VertexWeights));
		}
#endif //TRANSGAMING_NON_TNL

		// static index buffer
#ifndef TRANSGAMING
        if (mesh.Elems.Size())
		{
# ifdef TRANSGAMING_MESH_WARNINGS
            TRACE("AnimatedMeshMatPal::RenderInitialize(): WARNING-> using indexed tristripped vertex data for the mesh '%s'\n", mesh.Name.AsChar());
# endif
			mesh.PrimType=RenderDevice::EPrimTriStrip;
			mesh.Data->ElemStream=dev->CreateStream(&mesh.Elems[0],usage|RenderDevice::EIndex16,mesh.Elems.Size(),sizeof(WORD));
		}
		else
#endif

#ifndef CHEAPDEINDEX
        if (mesh.Data->Triangles.Size())
		{
# ifdef TRANSGAMING_MESH_WARNINGS
            TRACE("AnimatedMeshMatPal::RenderInitialize(): WARNING-> using indexed trilist vertex data for the mesh '%s'\n", mesh.Name.AsChar());
# endif
# ifndef TRANSGAMING_DEINDEX
			mesh.PrimType=RenderDevice::EPrimTriList;
# endif
			mesh.Data->ElemStream=dev->CreateStream(&mesh.Data->Triangles[0],usage|RenderDevice::EIndex16,mesh.Data->Triangles.Size(),sizeof(WORD));
		}
# ifdef TRANSGAMING_DEINDEX
        else if (!mesh.isIndexed){
#  ifdef TRANSGAMING_MESH_WARNINGS
            TRACE("AnimatedMeshMatPal::RenderInitialize(): using deindexed %s vertex data for the mesh '%s'\n", mesh.PrimType == RenderDevice::EPrimTriList ? "trilist" : "tristripped", mesh.Name.AsChar());
#  endif
            mesh.Data->ElemStream = EInvalidIndex;
        }
# endif
        else
		{
			Assert(!"Invalid PrimType");
		}
#endif


        if(mesh.Data->VertStream[0] == EInvalidIndex || mesh.Data->VertStreamCount == 0) 
            TRACE("AnimatedMeshMatPal::RenderInitialize(): ERROR-> could not create vertex stream for mesh: %s, skipping\n",
                        mesh.Name.AsChar());

#ifdef TRANSGAMING_NON_TNL
        if(EInvalidIndex == mesh.WeightsStream) 
            TRACE("AnimatedMeshMatPal::RenderInitialize(): ERROR-> could not create weights stream for mesh: %s, skipping\n",
                        mesh.Name.AsChar());
#endif

#ifdef TRANSGAMING_DEINDEX
        if(mesh.isIndexed && EInvalidIndex == mesh.Data->ElemStream) 
#elif !defined(CHEAPDEINDEX)
        if(EInvalidIndex == mesh.Data->ElemStream) 
#endif
            TRACE("AnimatedMeshMatPal::RenderInitialize(): ERROR-> could not create index stream for mesh: %s, skipping\n",
                        mesh.Name.AsChar());

	//+DDP - de-index data for PS2
#ifdef CHEAPDEINDEX
		int Count = mesh.Data->Triangles.Size();
		for (int buffers=0; buffers<MAXSKINMESHRENDERS; buffers++)
		{
#ifdef DEBUG
			mesh.Data->ps2_Verts[buffers].Create  (GEngine->GetRenderDevice(), Count, true, __FILE__, __LINE__);
			mesh.Data->ps2_Normals[buffers].Create(GEngine->GetRenderDevice(), Count, true, __FILE__, __LINE__);
			mesh.Data->ps2_UVs[buffers].Create    (GEngine->GetRenderDevice(), Count, true, __FILE__, __LINE__);
#else
			mesh.Data->ps2_Verts[buffers].Create  (GEngine->GetRenderDevice(), Count, true);
			mesh.Data->ps2_Normals[buffers].Create(GEngine->GetRenderDevice(), Count, true);
			mesh.Data->ps2_UVs[buffers].Create    (GEngine->GetRenderDevice(), Count, true);
#endif

            //since the vertices all have 4 components and we always want the W coordinate to be 1.0f,
            //we can set that here in a preprocess step.  All other vertex data will be overwritten
            //on every frame when animating the mesh.  
            int         stride;
			Vector4  *ps2_Verts = mesh.Data->ps2_Verts[buffers].Lock(Count);

			for (int i = 0; i < Count; i++)
			{
				ps2_Verts[i].X = 0.0f;
				ps2_Verts[i].Y = 0.0f;
				ps2_Verts[i].Z = 0.0f;
				ps2_Verts[i].W = 1.0f;
			}

            mesh.Data->ps2_Verts[buffers].Unlock();

			//as streams are double buffered, copy data to other buffer.
			mesh.Data->ps2_Verts[buffers].CloneStream();
		}

        Assert(mesh.Data->VertStreamCount == 0 && mesh.Data->VertStream[0] == EInvalidIndex);
#endif
	}
}

void AnimatedMeshMatPal::AnimateGeometry(const DynamicArray<Matrix>& matricesArr)
{
	RenderDevice* dev=GEngine->GetRenderDevice();
	if (dev->HWClass==RenderDevice::EHWClassGenericTnL)
		AnimatedMesh::AnimateGeometry(matricesArr);
}

void AnimatedMeshMatPal::SetBindPoseFrame(const Model* model) const
{
	for (int iJoint=0; iJoint<model->matricesArr.Size(); iJoint++)
	{
		model->matricesArr[iJoint] = model->ObjectToWorld;
	}
#ifdef TRANSGAMING // mysteriously missing from original code (bug?)
	RenderDevice* dev=GEngine->GetRenderDevice();
	if (dev->HWClass==RenderDevice::EHWClassGenericTnL)
		AnimatedMesh::SetBindPoseFrame(model);
#endif
}

void AnimatedMeshMatPal::RenderDefault(Model* model, int mode) const
{
	Plane tmp1(0,0,0,1);
	
    RenderDevice* dev=GEngine->GetRenderDevice();
    MaterialSystem* matSys = GEngine->Video->MaterialSys;

	// preprocess lighting for mesh
#ifdef	TRANSGAMING
    Color_TG tmpColor = model->GetEnvironmentAmbient();
	Plane ambient;
        
    COLOR2PLANE(ambient, tmpColor);
#else
	Plane ambient = Plane(model->GetEnvironmentAmbient());
#endif

	// set constants
	View *view = GEngine->Video->GetActiveView();
	Viewport &pViewport = view->LocalViewport;
#ifdef TRANSGAMING
    Plane tmpPlane;
#endif

	bool tnl = dev->HWClass==RenderDevice::EHWClassGenericTnL;
	bool sm2 = dev->HWClass>=RenderDevice::EHWClassNV30;
	bool sm3 = dev->HWClass>=RenderDevice::EHWClassNV40;

	dev->SetShaderMemory(0,&pViewport.WorldToScreen.GetTranspose()[0],4);
#ifdef TRANSGAMING
    tmpPlane.set(ambient.Normal);
	dev->SetShaderMemory(11,&tmpPlane,1);
    dev->SetAmbientLight(ambient.Normal);
#else
	dev->SetShaderMemory(11,&Plane(ambient.Normal),1);
#endif

	if (tnl)
	{
#ifdef	TRANSGAMING
		dev->SetFogState(GEngine->pcfWorld->Env.FogParams);
#else
		dev->SetFogState(GEngine->World->Env.FogParams);
#endif
	}

	for (int iPass=0; iPass<Material::MaxPasses; ++iPass)
	{
		for (int i=0; i<Meshes.Size(); i++) 
		{
			MeshShape &mesh = Meshes[i];
			MeshProperties &meshProps = model->MeshPropertiesArr[i];
			Material* shader;
			bool useLOD = false;

			if (mesh.IsWarp && !(mode & View::EModeWarpPass))
				continue;

			if (meshProps.DefaultShader)
				shader = meshProps.DefaultShader;
			else
			{
				shader = mesh.DefaultShader;

				if (mesh.RenderType==MeshShape::EVertexGPUSkin)
					useLOD = true;
				if (mesh.Data->Groups.Size()>0 && mesh.Data->Groups[0].TexEmPtr)
					useLOD = false;
			}

			int numPasses = shader->PassCount;

			if (mesh.RenderType==MeshShape::EPixelGPUSkin)
			{
#ifdef TRANSGAMING_NON_TNL
				if (sm2)
					numPasses = 1;
				else
#endif  //TRANSGAMING_NON_TNL
					numPasses = max(1, min(model->LightCount, shader->PassCount));
			}

			if (iPass>=numPasses)
				continue;

			if (!model->MeshPropertiesArr[i].IsVisible)
				continue;


			// special effect blending (freezer)
			shader->Passes[0].State.SetConstColor((DWORD(255*model->BlendAlpha)&0xFF)<<24);

			int bones = min(mesh.InflictingBones.Size(),MAX_BONES_PER_MESH);

			for (int iBone=0; iBone<bones; iBone++)
			{
				const Matrix &m = model->matricesArr[mesh.InflictingBones[iBone]];

				Matrix mt=m;
				mt.Transpose();
			//	Matrix mt = IdentityMatrix();

				dev->SetShaderMemory(27+iBone*3,&mt,3);
			}

			int lights = 0;

			if (mesh.RenderType==MeshShape::EVertexGPUSkin)
			{
#ifdef TRANSGAMING
                dev->SetLighting(true);
#endif
				if (!mesh.IsLighting)
				{
					Plane ambient = model->FullBrightColor;

//					dev->SetShaderMemory(10,&Plane(0,0,0,1),1);
					dev->SetShaderMemory(10,&tmp1,1);
					dev->SetShaderMemory(11,&ambient,1);
#ifdef TRANSGAMING
					dev->SetSpecular(tmp1.Normal, tmp1.Dist);
					dev->SetAmbientLight(ambient.Normal);
#endif

					for (int iLight=0; iLight<Model::MaxLights; iLight++)
					{
//						dev->SetShaderMemory(12+3*iLight,&Plane(0,0,0,1),1);
//						dev->SetShaderMemory(13+3*iLight,&Plane(0,0,0,1),1);
//						dev->SetShaderMemory(14+3*iLight,&Plane(0,0,0,1),1);
						dev->SetShaderMemory(12+3*iLight,&tmp1,1);
						dev->SetShaderMemory(13+3*iLight,&tmp1,1);
						dev->SetShaderMemory(14+3*iLight,&tmp1,1);
#ifdef TRANSGAMING
                        dev->SetNoLight(iLight);
#endif
					}
				}
				else
				{
					lights = model->ComputeVSLights();

#ifdef TRANSGAMING
                    tmpPlane.set(mesh.MatSpecColor,mesh.MatSpecPower);
					dev->SetShaderMemory(10,&tmpPlane,1);
					dev->SetSpecular(mesh.MatSpecColor, mesh.MatSpecPower);
#else
					dev->SetShaderMemory(10,&Plane(mesh.MatSpecColor,mesh.MatSpecPower),1);
#endif

					for (int iLight=0; iLight<Model::MaxLights; iLight++)
					{
						dev->SetShaderMemory(12+3*iLight,&model->VSLights[iLight].Color,1);
						dev->SetShaderMemory(13+3*iLight,&model->VSLights[iLight].Dir,1);
						dev->SetShaderMemory(14+3*iLight,&model->VSLights[iLight].HalfDir,1);
#ifdef TRANSGAMING
                        dev->SetDirLight(iLight, model->VSLights[iLight].Dir, model->VSLights[iLight].Color.Normal);
#endif
					}
				}
			}
			else if (mesh.RenderType==MeshShape::EPixelGPUSkin)
			{
				if (view->Mode & View::EModeRenderRTCubemap)
					return;
				int numLights = 2;
				model->ComputeVSLights(numLights);


#ifdef TRANSGAMING_NON_TNL
				if (sm2)
				{
					Plane col1 = model->VSLights[0].Color;
					Plane col2 = model->VSLights[1].Color;
#ifdef	TRANSGAMING
					Plane amb(ambient.Normal * GEngine->pcfWorld->PixelLightingMultiplier,1);
#else
					Plane amb(ambient.Normal * GEngine->World->PixelLightingMultiplier,1);
#endif

					dev->SetPixelShaderMemory(0,&amb,1);
					dev->SetPixelShaderMemory(1,&col1,1);
					dev->SetPixelShaderMemory(2,&col2,1);

					if (sm3)
					{
						BOOL ComputeLights[2] = { TRUE, TRUE };
						if (col2.X + col2.Y + col2.Z<F_EPS)
						{
							ComputeLights[1] = FALSE;
							if (col1.X + col1.Y + col1.Z<F_EPS)
								ComputeLights[0] = FALSE;
						}

						dev->SetPixelShaderMemoryB(0,ComputeLights,2);
					}

					dev->SetShaderMemory(13,	&model->VSLights[0].Dir,1);
					dev->SetShaderMemory(14,	&model->VSLights[0].HalfDir,1);				
					dev->SetShaderMemory(16,	&model->VSLights[1].Dir,1);
					dev->SetShaderMemory(17,	&model->VSLights[1].HalfDir,1);				
				}
				else
				{
#endif //TRANSGAMING_NON_TNL
					//0,0,0.167000,-0.042000 // ^3
					//0,0,0.210375,-0.085375 // ^4
					//0,0,0.254125,-0.129125 // ^5
					//0,0,0.298125,-0.173125 // ^6
					//0,0,0.475250,-0.350250 // ^10
					//0,0,0.830625,-0.705625 // ^18
                    if (iPass>0){
#ifdef TRANSGAMING
                        tmpPlane.set(0,0,0,0);
						dev->SetPixelShaderMemory(0,&tmpPlane,1);
#else
						dev->SetPixelShaderMemory(0,&Plane(0,0,0,0),1);
#endif
                    }
					else
					{
						Plane amb = ambient;
#ifdef	TRANSGAMING
						amb *= 0.5f*GEngine->pcfWorld->PixelLightingMultiplier;
#else
						amb *= 0.5f*GEngine->World->PixelLightingMultiplier;
#endif
						dev->SetPixelShaderMemory(0,&amb,1);
					}

					Plane col = model->VSLights[iPass].Color;	col *= 0.5f;
					dev->SetPixelShaderMemory(1,&col,1);
#ifdef TRANSGAMING
                    tmpPlane.set(0,0,0.475250f,-0.350250f);
					dev->SetPixelShaderMemory(2,&tmpPlane,1); // ^10
#else
					dev->SetPixelShaderMemory(2,&Plane(0,0,0.475250f,-0.350250f),1); // ^10
#endif

					dev->SetShaderMemory(13,	&model->VSLights[iPass].Dir,1);
					dev->SetShaderMemory(14,	&model->VSLights[iPass].HalfDir,1);
#ifdef TRANSGAMING_NON_TNL
				}
#endif  //TRANSGAMING_NON_TNL

			} 
			else if (mesh.RenderType==MeshShape::EWaterGPUSkin)
			{
#ifdef TRANSGAMING_NON_TNL
				if (model->WorldLink && !tnl)
					model->WorldLink->RenderToCubemap = true;
#endif  //TRANSGAMING_NON_TNL

				if (view->Mode & View::EModeRenderRTCubemap)
					return;

				// dont draw if cube map hasnt been updated this frame
#ifdef TRANSGAMING_NON_TNL
#ifdef	TRANSGAMING
				if (!tnl && (GEngine->pcfWorld->CubemapFrame != GEngine->pcfWorld->Frame))
#else
				if (!tnl && (GEngine->World->CubemapFrame != GEngine->World->Frame))
#endif
					return;
#endif  //TRANSGAMING_NON_TNL

				const float refract  = mesh.MatRefract;
				const float fresnel  = mesh.MatFresnel;
				const Vector reflTint = mesh.MatReflTint;
				const Vector refrTint = mesh.MatRefrTint;
				const Vector& eyeVec = GEngine->Video->GetActiveView()->LocalViewport.EyePosition;

#ifdef TRANSGAMING
                Plane tmpPlane;

                tmpPlane.set(F_PI, F_TWOPI, 1.0f/F_TWOPI, 0.5f);
				dev->SetShaderMemory(4, &tmpPlane, 1);
                tmpPlane.set(1.0f, -1.0f / 6.0f, 1.0f / 120.0f, -1.0f / 5040.0f);
				dev->SetShaderMemory(5, &tmpPlane, 1);
                tmpPlane.set(1.0f, -1.0f / 2.0f, 1.0f / 24.0f, -1.0f / 720.0f);
				dev->SetShaderMemory(6, &tmpPlane, 1);

                tmpPlane.set(eyeVec,1);
				dev->SetShaderMemory(10, &tmpPlane, 1);			
                tmpPlane.set(refract, refract*refract, fresnel, 2);
				dev->SetShaderMemory(11, &tmpPlane, 1);

				struct Model::sImpact *imp = model->Impact;

                tmpPlane.set(imp[0].Time*imp[0].Speed, imp[1].Time*imp[1].Speed, imp[2].Time*imp[2].Speed, imp[3].Time*imp[3].Speed);
				dev->SetShaderMemory(12, &tmpPlane, 1);
                tmpPlane.set(1.0f/imp[0].Period,	1.0f/imp[1].Period,	1.0f/imp[2].Period,	1.0f/imp[3].Period);
				dev->SetShaderMemory(13, &tmpPlane, 1);
                tmpPlane.set(1.0f/imp[0].Range, 	1.0f/imp[1].Range, 	1.0f/imp[2].Range, 	1.0f/imp[3].Range);
				dev->SetShaderMemory(14, &tmpPlane, 1);
                tmpPlane.set(0.0f,0.0f,0.0f,0.0f);
				dev->SetShaderMemory(15, &tmpPlane, 1);
                tmpPlane.set(imp[0].Amplitude,  	imp[1].Amplitude,  	imp[2].Amplitude,  	imp[3].Amplitude);
				dev->SetShaderMemory(16, &tmpPlane, 1);

				for (int i=0; i<Model::MAX_IMPACTS; ++i)
				{
					const Vector epicenter	= model->Impact[i].Epicenter;
                    tmpPlane.set(epicenter, 1);
					dev->SetShaderMemory(17+i, &tmpPlane, 1);
				}

                tmpPlane.set(reflTint-refrTint,1);
				dev->SetShaderMemory(21, &tmpPlane, 1);
                tmpPlane.set(refrTint,0);
				dev->SetShaderMemory(22, &tmpPlane, 1);
#else

				dev->SetShaderMemory(4, &Plane(F_PI, F_TWOPI, 1.0f/F_TWOPI, 0.5f), 1);
				dev->SetShaderMemory(5, &Plane(1.0f, -1.0f / 6.0f, 1.0f / 120.0f, -1.0f / 5040.0f), 1);
				dev->SetShaderMemory(6, &Plane(1.0f, -1.0f / 2.0f, 1.0f / 24.0f, -1.0f / 720.0f), 1);

				dev->SetShaderMemory(10, &Plane(eyeVec,1), 1);			
				dev->SetShaderMemory(11, &Plane(refract, refract*refract, fresnel, 2), 1);

				struct Model::sImpact *imp = model->Impact;

				dev->SetShaderMemory(12, &Plane(imp[0].Time*imp[0].Speed, imp[1].Time*imp[1].Speed, imp[2].Time*imp[2].Speed, imp[3].Time*imp[3].Speed), 1);
				dev->SetShaderMemory(13, &Plane(1.0f/imp[0].Period,	1.0f/imp[1].Period,	1.0f/imp[2].Period,	1.0f/imp[3].Period), 1);
				dev->SetShaderMemory(14, &Plane(1.0f/imp[0].Range, 	1.0f/imp[1].Range, 	1.0f/imp[2].Range, 	1.0f/imp[3].Range), 1);
				dev->SetShaderMemory(15, &Plane(0.0f,0.0f,0.0f,0.0f), 1);
				dev->SetShaderMemory(16, &Plane(imp[0].Amplitude,  	imp[1].Amplitude,  	imp[2].Amplitude,  	imp[3].Amplitude), 1);

				for (int i=0; i<Model::MAX_IMPACTS; ++i)
				{
					const Vector epicenter	= model->Impact[i].Epicenter;
					dev->SetShaderMemory(17+i, &Plane(epicenter, 1), 1);
				}

				dev->SetShaderMemory(21, &Plane(reflTint-refrTint,1), 1);
				dev->SetShaderMemory(22, &Plane(refrTint,0), 1);
#endif
            } 
			else
			{
				continue;
			}


			if (tnl)
			{
#ifdef CHEAPDEINDEX
                const int s[] = { mesh.Data->ps2_Verts[mesh.Data->RenderInstance].GetStream(), mesh.Data->ps2_Normals[mesh.Data->RenderInstance].GetStream(), mesh.Data->ps2_UVs[mesh.Data->RenderInstance].GetStream() };
                dev->SetStreams(s, ARRAY_COUNT(s), EInvalidIndex);
                //dev->SetShader(GEngine->Video->MaterialSys->Shaders.Fixed_4NTU, RenderDevice::EShaderVertex);
                //dev->SetShader(EInvalidIndex, RenderDevice::EShaderPixel);
#else
				dev->SetStreams(mesh.Data->VertStream, mesh.Data->VertStreamCount, mesh.Data->ElemStream);

#endif
			}
#ifdef TRANSGAMING_NON_TNL
			else
			{
				const int streams[] = { mesh.Data->VertStream, mesh.WeightsStream };
				dev->SetStreams(streams,ARRAY_COUNT(streams),mesh.Data->ElemStream);
			}
#endif  //TRANSGAMING_NON_TNL

			RenderPass& pass = shader->Passes[iPass];	
			pass.Activate();
			MeshProperties &meshProp=model->MeshPropertiesArr[i];

			for(int sec=0; sec<mesh.Data->Groups.Size(); ++sec) 
			{
				SimpleMesh::Group& group=mesh.Data->Groups[sec];
		
				MeshProperties::GroupProperties &groupProp=model->MeshPropertiesArr[i].Groups[sec];

				Texture* envCubeMap = 0;
				if (model && model->WorldLink)
				{
					envCubeMap = tnl ? model->WorldLink->TexStaticCubemap : model->WorldLink->TexSceneCubemap; 
				}

#ifdef TRANSGAMING
				Texture *textures[] = {	
					groupProp.TexPtr ? groupProp.TexPtr : group.TexPtr,
					groupProp.TexEmPtr ? groupProp.TexEmPtr : group.TexEmPtr,
					envCubeMap
				};

				int flags 
					= MaterialInput::EInputColor
					| MaterialInput::EInputEmissive
					| MaterialInput::EInputEnvCubeMap;


                MaterialInput tmpMaterial(flags,textures,0);

				pass.SetInput(&tmpMaterial);
#else
				Texture *textures[] = {	
					groupProp.TexPtr ? groupProp.TexPtr : group.TexPtr,
					meshProp.NormalMapPtr ? meshProp.NormalMapPtr : mesh.NormalMapTex,
					groupProp.TexEmPtr ? groupProp.TexEmPtr : group.TexEmPtr,
					envCubeMap
				};

				int flags 
					= MaterialInput::EInputColor
					| MaterialInput::EInputNormal
					| MaterialInput::EInputEmissive
					| MaterialInput::EInputEnvCubeMap;


				pass.SetInput(&MaterialInput(flags,textures,0));
#endif

				if (useLOD)
				{
					int influences = mesh.MaxInfluences-1;
					if (model->DistanceFromCamera>view->SkinShaderLODDistance)
						influences = 0;

					if (tnl)
					{
						influences = 0;
						lights = 0;
					}
#ifdef TRANSGAMING_NON_TNL
					else
					{
						dev->SetShader(matSys->Shaders.SKIN[influences][lights],RenderDevice::EShaderVertex);					
					}
#endif //TRANSGAMING_NON_TNL
				}

#ifdef CHEAPDEINDEX
				dev->Draw(mesh.PrimType,group.RenderBase,group.RenderCount,0,0);
#else
# ifdef TRANSGAMING_DEINDEX
                if (!mesh.isIndexed)
				    dev->Draw(mesh.PrimType,group.RenderBase,group.RenderCount);
                else
# endif
				    dev->Draw(mesh.PrimType,group.RenderBase,group.RenderCount,group.VertexMin,group.VertexCount);
#endif
			}
#ifdef TRANSGAMING
            dev->SetLighting(false);
#endif
		}
	}

	if (tnl)
	{
#ifdef	TRANSGAMING
		dev->SetFogState(GEngine->pcfWorld->Env.FogParams.UseVSFog());
#else
		dev->SetFogState(GEngine->World->Env.FogParams.UseVSFog());
#endif
	}

	if (Sort == Entity::ESortGun) {
		dev->SetViewport(0,0,pViewport.ViewportWidth,pViewport.ViewportHeight,pViewport.zNear,pViewport.zFar);
	}
}

void AnimatedMeshMatPal::RenderVolume(const Model* model) const
{
    RenderDevice* dev=GEngine->GetRenderDevice();
	Viewport &pViewport = GEngine->Video->GetActiveView()->LocalViewport;
	dev->SetShaderMemory(0,&pViewport.WorldToScreen.GetTranspose()[0],4);
	bool tnl = dev->HWClass==RenderDevice::EHWClassGenericTnL;

	for (int i=0; i<Meshes.Size(); i++) 
	{
		MeshShape &mesh = Meshes[i];
	
		if (!model->MeshPropertiesArr[i].IsVisible)
			continue;

		if (tnl)
			dev->SetStreams(mesh.Data->VertStream, mesh.Data->VertStreamCount, mesh.Data->ElemStream);
#ifdef TRANSGAMING_NON_TNL
		else
		{
			const int streams[] = { mesh.Data->VertStream, mesh.WeightsStream };
			dev->SetStreams(streams,ARRAY_COUNT(streams),mesh.Data->ElemStream);
		}
#endif  //TRANSGAMING_NON_TNL

		for (int iBone=0; iBone<mesh.InflictingBones.Size() && iBone<MAX_BONES_PER_MESH; iBone++)
		{
			const Matrix &m = model->matricesArr[mesh.InflictingBones[iBone]];

			Matrix mt=m;
			mt.Transpose();

			dev->SetShaderMemory(27+iBone*3,&mt,3);
		}

		for (int index=0; index<mesh.VolShader->PassCount; ++index)
		{
			RenderPass& pass = mesh.VolShader->Passes[index];	
			pass.Activate();

			for(int sec=0; sec<mesh.Data->Groups.Size(); ++sec) 
			{
				SimpleMesh::Group& group=mesh.Data->Groups[sec];
		
#ifdef TRANSGAMING_DEINDEX
                if (!mesh.isIndexed)
    				dev->Draw(mesh.PrimType,group.RenderBase,group.RenderCount);

                else
#endif
				    dev->Draw(mesh.PrimType,group.RenderBase,group.RenderCount,group.VertexMin,group.VertexCount);
			}
		}
	}
}

void AnimatedMeshMatPal::RenderGhostFX(Model* model) const
{
	Plane tmp;
	Plane tmp0(0,0,0,0);
	Plane tmp1(0,0,0,1);
	Plane tmp2(1,1,1,1);
	
    RenderDevice* dev=GEngine->GetRenderDevice();
    MaterialSystem* matSys = GEngine->Video->MaterialSys;
	View *view = GEngine->Video->GetActiveView();
	Viewport &pViewport = view->LocalViewport;
	dev->SetShaderMemory(0,&pViewport.WorldToScreen.GetTranspose()[0],4);
	
	tmp = Plane(0.3f,0.59f,0.11f,0);
//	dev->SetPixelShaderMemory(0,&Plane(0.3f,0.59f,0.11f,0),1);
	dev->SetPixelShaderMemory(0,&tmp,1);
	
	Texture* notex = GEngine->Video->MaterialSys->Textures.NoTex;
	bool tnl = dev->HWClass==RenderDevice::EHWClassGenericTnL;

	for (int i=0; i<Meshes.Size(); i++) 
	{
		MeshShape &mesh = Meshes[i];

		bool useLOD = false;
		if (mesh.RenderType==MeshShape::EVertexGPUSkin)
			useLOD = true;
		if (mesh.Data->Groups.Size()>0 && mesh.Data->Groups[0].TexEmPtr)
			useLOD = false;

		int lights = 0;
	
		if (!model->MeshPropertiesArr[i].IsVisible)
			continue;

		if (tnl)
			dev->SetStreams(mesh.Data->VertStream, mesh.Data->VertStreamCount, mesh.Data->ElemStream);
#ifdef TRANSGAMING_NON_TNL
		else
		{
			const int streams[] = { mesh.Data->VertStream, mesh.WeightsStream };
			dev->SetStreams(streams,ARRAY_COUNT(streams),mesh.Data->ElemStream);
		}
#endif  //TRANSGAMING_NON_TNL

		for (int iBone=0; iBone<mesh.InflictingBones.Size() && iBone<MAX_BONES_PER_MESH; iBone++)
		{
			const Matrix &m = model->matricesArr[mesh.InflictingBones[iBone]];

			Matrix mt=m;
			mt.Transpose();

			dev->SetShaderMemory(27+iBone*3,&mt,3);
		}

		if (mesh.RenderType==MeshShape::EVertexGPUSkin)
		{
#ifdef TRANSGAMING
            dev->SetLighting(true);
#endif
            if (!mesh.IsLighting)
            {
                Plane ambient = Plane(1,1,1,1);

//              dev->SetShaderMemory(10,&Plane(0,0,0,1),1);
//              dev->SetShaderMemory(11,&Plane(1,1,1,1),1);
                dev->SetShaderMemory(10,&tmp1,1);
                dev->SetShaderMemory(11,&tmp2,1);
#ifdef TRANSGAMING
                dev->SetSpecular(tmp1.Normal, tmp1.Dist);
                dev->SetAmbientLight(tmp2.Normal);
#endif

                for (int iLight=0; iLight<Model::MaxLights; iLight++)
                {
//                  dev->SetShaderMemory(12+3*iLight,&Plane(0,0,0,1),1);
//                  dev->SetShaderMemory(13+3*iLight,&Plane(0,0,0,1),1);
//                  dev->SetShaderMemory(14+3*iLight,&Plane(0,0,0,1),1);
                    dev->SetShaderMemory(12+3*iLight,&tmp1,1);
                    dev->SetShaderMemory(13+3*iLight,&tmp1,1);
                    dev->SetShaderMemory(14+3*iLight,&tmp1,1);
#ifdef TRANSGAMING
                    dev->SetNoLight(iLight);
#endif
                }
            }
            else
            {
			    lights = model->ComputeVSLights();
#ifdef	TRANSGAMING
				//why do that? it's no use
			    Color_TG tmpColor = model->GetEnvironmentAmbient();
				Plane ambient;
                
                COLOR2PLANE(ambient, tmpColor);
#else
				Plane ambient = Plane(model->GetEnvironmentAmbient());
#endif

//			    dev->SetShaderMemory(10,&Plane(mesh.MatSpecColor,mesh.MatSpecPower),1);
				tmp = Plane(mesh.MatSpecColor,mesh.MatSpecPower);
			    dev->SetShaderMemory(10,&tmp,1);
//				dev->SetShaderMemory(11,&Plane(0,0,0,0),1);
				dev->SetShaderMemory(11,&tmp0,1);
#ifdef TRANSGAMING
                dev->SetSpecular(mesh.MatSpecColor, mesh.MatSpecPower);
                dev->SetAmbientLight(tmp0.Normal);
#endif

			    for (int iLight=0; iLight<Model::MaxLights; iLight++)
			    {
				    dev->SetShaderMemory(12+3*iLight,&model->VSLights[iLight].Color,1);
				    dev->SetShaderMemory(13+3*iLight,&model->VSLights[iLight].Dir,1);
				    dev->SetShaderMemory(14+3*iLight,&model->VSLights[iLight].HalfDir,1);
#ifdef TRANSGAMING
                    dev->SetDirLight(iLight, model->VSLights[iLight].Dir, model->VSLights[iLight].Color.Normal);
#endif
			    }
            }
		}

		MeshProperties &meshProp=model->MeshPropertiesArr[i];

		for (int index=0; index<mesh.GhostFXShader->PassCount; ++index)
		{
			RenderPass& pass = mesh.GhostFXShader->Passes[index];	
			pass.Activate();

			for(int sec=0; sec<mesh.Data->Groups.Size(); ++sec) 
			{
				SimpleMesh::Group& group=mesh.Data->Groups[sec];

				MeshProperties::GroupProperties &groupProp=model->MeshPropertiesArr[i].Groups[sec];

#ifdef TRANSGAMING
				MaterialInput tmpMaterial = MaterialInput(groupProp.TexPtr?groupProp.TexPtr:group.TexPtr, notex, meshProp.NormalMapPtr?meshProp.NormalMapPtr:notex);
#else
				MaterialInput tmpMaterial = MaterialInput(groupProp.TexPtr?groupProp.TexPtr:group.TexPtr, notex, meshProp.NormalMapPtr?meshProp.NormalMapPtr:mesh.NormalMapTex);
#endif
//				pass.SetInput(&MaterialInput(groupProp.TexPtr?groupProp.TexPtr:group.TexPtr, notex, meshProp.NormalMapPtr?meshProp.NormalMapPtr:mesh.NormalMapTex));
				pass.SetInput(&tmpMaterial);

				if (useLOD)
				{
					int influences = mesh.MaxInfluences-1;
					if (model->DistanceFromCamera>view->SkinShaderLODDistance)
						influences = 0;

					if (tnl) {
						influences = 0;
						lights = 1;
					}
					else					
						dev->SetShader(matSys->Shaders.SKIN[influences][lights],RenderDevice::EShaderVertex);					
				}

#ifdef TRANSGAMING_DEINDEX
                if (!mesh.isIndexed)
				    dev->Draw(mesh.PrimType,group.RenderBase,group.RenderCount);

                else
#endif
				    dev->Draw(mesh.PrimType,group.RenderBase,group.RenderCount,group.VertexMin,group.VertexCount);
			}
		}
#ifdef TRANSGAMING
        dev->SetLighting(false);
#endif
	}
}

void AnimatedMeshMatPal::RenderDemonFX(Model* model) const
{
    RenderDevice* dev=GEngine->GetRenderDevice();
	Viewport &pViewport = GEngine->Video->GetActiveView()->LocalViewport;
	dev->SetShaderMemory(0,&pViewport.WorldToScreen.GetTranspose()[0],4);
	Texture* notex = GEngine->Video->MaterialSys->Textures.NoTex;
	Material* fresnelmat = GEngine->Video->MaterialSys->CreateMaterial("palskinned_fresnel");
	bool tnl = dev->HWClass==RenderDevice::EHWClassGenericTnL;
#ifdef TRANSGAMING
    Plane tmpPlane;
#endif

	for (int i=0; i<Meshes.Size(); i++) 
	{
		MeshShape &mesh = Meshes[i];
	
		if (!model->MeshPropertiesArr[i].IsVisible)
			continue;

		if (tnl)
			dev->SetStreams(mesh.Data->VertStream, mesh.Data->VertStreamCount, mesh.Data->ElemStream);
#ifdef TRANSGAMING_NON_TNL
		else
		{
			const int streams[] = { mesh.Data->VertStream, mesh.WeightsStream };
			dev->SetStreams(streams,ARRAY_COUNT(streams),mesh.Data->ElemStream);
		}
#endif  //TRANSGAMING_NON_TNL

		const float refract  = mesh.MatRefract;
		const float fresnel  = mesh.MatFresnel;
		const Vector reflTint = mesh.MatReflTint;
		const Vector refrTint = mesh.MatRefrTint;		

		const Vector& eyeVec = GEngine->Video->GetActiveView()->LocalViewport.EyePosition;

#ifdef TRANSGAMING
        tmpPlane.set(eyeVec,1);
		dev->SetShaderMemory(10, &tmpPlane, 1);			
        tmpPlane.set(refract, refract*refract, fresnel, 2);
		dev->SetShaderMemory(11, &tmpPlane, 1);
#else
		dev->SetShaderMemory(10, &Plane(eyeVec,1), 1);			
		dev->SetShaderMemory(11, &Plane(refract, refract*refract, fresnel, 2), 1);
#endif

		for (int iBone=0; iBone<mesh.InflictingBones.Size() && iBone<MAX_BONES_PER_MESH; iBone++)
		{
			const Matrix &m = model->matricesArr[mesh.InflictingBones[iBone]];

			Matrix mt=m;
			mt.Transpose();

			dev->SetShaderMemory(27+iBone*3,&mt,3);
		}

		MeshProperties &meshProp=model->MeshPropertiesArr[i];

		for (int index=0; index<fresnelmat->PassCount; ++index)
		{
			RenderPass& pass = fresnelmat->Passes[index];	
			pass.Activate();

			for(int sec=0; sec<mesh.Data->Groups.Size(); ++sec) 
			{
				SimpleMesh::Group& group=mesh.Data->Groups[sec];

				MeshProperties::GroupProperties &groupProp=model->MeshPropertiesArr[i].Groups[sec];
		
#ifdef TRANSGAMING
                MaterialInput tmpMaterial(groupProp.TexPtr?groupProp.TexPtr:group.TexPtr, notex, 
                                          meshProp.NormalMapPtr?meshProp.NormalMapPtr:notex);

				pass.SetInput(&tmpMaterial);

# ifdef TRANSGAMING_DEINDEX
                if (!mesh.isIndexed)
				    dev->Draw(mesh.PrimType,group.RenderBase,group.RenderCount);

                else
# endif
				    dev->Draw(mesh.PrimType,group.RenderBase,group.RenderCount,group.VertexMin,group.VertexCount);
#else
				pass.SetInput(&MaterialInput(groupProp.TexPtr?groupProp.TexPtr:group.TexPtr, notex, meshProp.NormalMapPtr?meshProp.NormalMapPtr:mesh.NormalMapTex));

				dev->Draw(mesh.PrimType,group.RenderBase,group.RenderCount,group.VertexMin,group.VertexCount);
#endif
			}
		}
	}
}

void AnimatedMeshMatPal::RenderWarpFX(Model* model) const
{
	Plane tmp1(0,0,0,1);
	Plane tmp11(1,1,1,1);
	
    RenderDevice* dev=GEngine->GetRenderDevice();

	Viewport &pViewport = GEngine->Video->GetActiveView()->LocalViewport;
	dev->SetShaderMemory(0,&pViewport.WorldToScreen.GetTranspose()[0],4);

	const Matrix ViewMat = pViewport.WorldToScreen
						 * ScaleMatrix(Vector(0.5F,-0.5F,1))
						 * TranslationMatrix(Vector(0.5F, 0.5F, 0));

	dev->SetShaderMemory(4, &ViewMat.GetTranspose()[0], 4);	
	
#ifdef	TRANSGAMING
    Color_TG tmpColor = model->GetEnvironmentAmbient();
	Plane tmpPlane;
    
    COLOR2PLANE(tmpPlane, tmpColor);
	dev->SetShaderMemory(11, &tmpPlane, 1);
    dev->SetAmbientLight(tmpPlane.Normal);
#else
	dev->SetShaderMemory(11, &Plane(model->GetEnvironmentAmbient()), 1);
#endif

	Texture* notex = GEngine->Video->MaterialSys->Textures.NoTex;
	//Material* M = GEngine->Video->MaterialSys->CreateMaterial("palskinned_warp");
	bool tnl = dev->HWClass==RenderDevice::EHWClassGenericTnL;

	for (int i=0; i<MAX_WARP_NUM; ++i) 
	{
		if (Warps[i] == -1)
			continue;

		Material* M = GEngine->Video->WarpEffectsEnabled ? WarpShaders[i] : WarpFakeShaders[i];
		Assert(M);

		const int meshIndex = Warps[i];
		Assert(meshIndex>=0&&meshIndex<Meshes.Size());
		MeshShape &mesh = Meshes[meshIndex];

		if (!model->MeshPropertiesArr[meshIndex].IsVisible)
			continue;

		if (!mesh.IsWarp)
			continue;

		if (tnl)
			dev->SetStreams(mesh.Data->VertStream, mesh.Data->VertStreamCount, mesh.Data->ElemStream);
#ifdef TRANSGAMING_NON_TNL
		else
		{
			const int streams[] = { mesh.Data->VertStream, mesh.WeightsStream };
			dev->SetStreams(streams,ARRAY_COUNT(streams),mesh.Data->ElemStream);
		}
#endif  //TRANSGAMING_NON_TNL

		for (int iBone=0; iBone<mesh.InflictingBones.Size() && iBone<MAX_BONES_PER_MESH; ++iBone)
		{
			const Matrix &m = model->matricesArr[mesh.InflictingBones[iBone]];

			Matrix mt=m;
			mt.Transpose();

			dev->SetShaderMemory(27+iBone*3,&mt,3);
		}

		if (mesh.RenderType==MeshShape::EVertexGPUSkin)
		{
#ifdef TRANSGAMING
            dev->SetLighting(true);
#endif
            if (!mesh.IsLighting)
            {
                Plane ambient = Plane(1,1,1,1);

//				dev->SetShaderMemory(10,&Plane(0,0,0,1),1);
//				dev->SetShaderMemory(11,&Plane(1,1,1,1),1);
				dev->SetShaderMemory(10,&tmp1,1);
				dev->SetShaderMemory(11,&tmp11,1);
#ifdef TRANSGAMING
                dev->SetSpecular(tmp1.Normal, tmp1.Dist);
                dev->SetAmbientLight(tmp11.Normal);
#endif

                for (int iLight=0; iLight<Model::MaxLights; iLight++)
                {
//                    dev->SetShaderMemory(12+3*iLight,&Plane(0,0,0,1),1);
//                    dev->SetShaderMemory(13+3*iLight,&Plane(0,0,0,1),1);
//                    dev->SetShaderMemory(14+3*iLight,&Plane(0,0,0,1),1);
					dev->SetShaderMemory(12+3*iLight,&tmp1,1);
					dev->SetShaderMemory(13+3*iLight,&tmp1,1);
					dev->SetShaderMemory(14+3*iLight,&tmp1,1);
#ifdef TRANSGAMING
                    dev->SetNoLight(iLight);
#endif
                }
            }
            else
            {
			    model->ComputeVSLights();
				//Plane ambient = Plane(model->GetEnvironmentAmbient());

#ifdef TRANSGAMING
                Plane tmpPlane(mesh.MatSpecColor,mesh.MatSpecPower);

			    dev->SetShaderMemory(10, &tmpPlane, 1);
                dev->SetSpecular(mesh.MatSpecColor, mesh.MatSpecPower);
#else
			    dev->SetShaderMemory(10,&Plane(mesh.MatSpecColor,mesh.MatSpecPower),1);
#endif

			    for (int iLight=0; iLight<Model::MaxLights; iLight++)
			    {
				    dev->SetShaderMemory(12+3*iLight,&model->VSLights[iLight].Color,1);
				    dev->SetShaderMemory(13+3*iLight,&model->VSLights[iLight].Dir,1);
				    dev->SetShaderMemory(14+3*iLight,&model->VSLights[iLight].HalfDir,1);
#ifdef TRANSGAMING
                    dev->SetDirLight(iLight, model->VSLights[iLight].Dir, model->VSLights[iLight].Color.Normal);
#endif
			    }
            }
		}

		MeshProperties &meshProp=model->MeshPropertiesArr[meshIndex];

		for (int index=0; index<M->PassCount; ++index)
		{
			RenderPass& pass = M->Passes[index];	
			pass.Activate();

			for(int sec=0; sec<mesh.Data->Groups.Size(); ++sec) 
			{
				SimpleMesh::Group& group=mesh.Data->Groups[sec];

				MeshProperties::GroupProperties &groupProp=model->MeshPropertiesArr[meshIndex].Groups[sec];
		
#ifdef TRANSGAMING
                MaterialInput tmpMaterial(groupProp.TexPtr?groupProp.TexPtr:group.TexPtr, notex, 
                                          meshProp.NormalMapPtr?meshProp.NormalMapPtr:notex);

				pass.SetInput(&tmpMaterial);

# ifdef TRANSGAMING_DEINDEX
                if (!mesh.isIndexed)
				    dev->Draw(mesh.PrimType,group.RenderBase,group.RenderCount);

                else
# endif
				    dev->Draw(mesh.PrimType,group.RenderBase,group.RenderCount,group.VertexMin,group.VertexCount);
#else
				pass.SetInput(&MaterialInput(groupProp.TexPtr?groupProp.TexPtr:group.TexPtr, notex, meshProp.NormalMapPtr?meshProp.NormalMapPtr:mesh.NormalMapTex));

				dev->Draw(mesh.PrimType,group.RenderBase,group.RenderCount,group.VertexMin,group.VertexCount);
#endif
			}
		}
#ifdef TRANSGAMING
        dev->SetLighting(false);
#endif
	}
}

void AnimatedMeshMatPal::RenderShadowmap(const Model* model) const
{
    RenderDevice* dev=GEngine->GetRenderDevice();
	bool tnl = dev->HWClass==RenderDevice::EHWClassGenericTnL;
#ifdef TRANSGAMING
    Plane tmpPlane;
#endif

	dev->SetRenderTarget(model->ShadowMap, NULL);
	dev->ClearFB(0xFF000000);
	dev->SetViewport(1,1,model->ShadowMap->Width-2,model->ShadowMap->Width-2);

	// preprocess lighting for mesh
	Plane ambient(1,1,1,1);

	// set constants
	//Matrix spawn = model->ShadowDecal->GetSpawnMatrix().Inverse();
	const Matrix spawn = model->ShadowMatrix.Inverse();
	Matrix o2c = spawn*OrthoMatrix(-0.5f,0.5f,0.5f,-0.5f,1.5f,-0.5f);

	dev->SetShaderMemory(0,&o2c.GetTranspose()[0],4);

#ifdef TRANSGAMING
    tmpPlane.set(ambient.Normal);
    dev->SetShaderMemory(11,&tmpPlane,1);
#else
	dev->SetShaderMemory(11,&Plane(ambient.Normal),1);
#endif

	Texture* notex = GEngine->Video->MaterialSys->Textures.NoTex;

	dev->SetFog( false );

	for (int i=0; i<Meshes.Size(); i++) 
	{
		MeshShape &mesh = Meshes[i];
	
		if (!model->MeshPropertiesArr[i].IsVisible)
			continue;

		if (tnl)
			dev->SetStreams(mesh.Data->VertStream, mesh.Data->VertStreamCount,mesh.Data->ElemStream);
#ifdef TRANSGAMING_NON_TNL
		else
		{
			const int streams[] = { mesh.Data->VertStream, mesh.WeightsStream };
			dev->SetStreams(streams,ARRAY_COUNT(streams),mesh.Data->ElemStream);
		}
#endif  //TRANSGAMING_NON_TNL

		for (int iBone=0; iBone<mesh.InflictingBones.Size() && iBone<MAX_BONES_PER_MESH; iBone++)
		{
			const Matrix &m = model->matricesArr[mesh.InflictingBones[iBone]];

			Matrix mt=m;
			mt.Transpose();

			dev->SetShaderMemory(27+iBone*3,&mt,3);
		}
		
			
		Material* shader = mesh.ShadowMapShader;

		for (int iPass=0; iPass<shader->PassCount; ++iPass)
		{
			RenderPass& pass = shader->Passes[iPass];	
			pass.Activate();

			for(int sec=0; sec<mesh.Data->Groups.Size(); ++sec) 
			{
				SimpleMesh::Group& group=mesh.Data->Groups[sec];
		
				MeshProperties::GroupProperties &groupProp=model->MeshPropertiesArr[i].Groups[sec];
		
#ifdef TRANSGAMING
                MaterialInput tmpMaterial(groupProp.TexPtr?groupProp.TexPtr:group.TexPtr, notex, notex);

                pass.SetInput(&tmpMaterial);

# ifdef TRANSGAMING_DEINDEX
                if (!mesh.isIndexed)
				    dev->Draw(mesh.PrimType,group.RenderBase,group.RenderCount);
				
                else
# endif
                    dev->Draw(mesh.PrimType,group.RenderBase,group.RenderCount,group.VertexMin,group.VertexCount);
#else
				pass.SetInput(&MaterialInput(groupProp.TexPtr?groupProp.TexPtr:group.TexPtr, notex, mesh.NormalMapTex));

				dev->Draw(mesh.PrimType,group.RenderBase,group.RenderCount,group.VertexMin,group.VertexCount);
#endif
			}
		}
	}

	if (model->ShadowMapFilter)
	{
		int w = GEngine->Video->GetActiveView()->LocalViewport.Width;
		int h = GEngine->Video->GetActiveView()->LocalViewport.Height;
		float u=0,v=0,u1=1,v1=1;
		DWORD col = 0xFFFFFFFF;
		const float texWidth = model->ShadowMap->Width;

		int unlock = GEngine->Video->Hud->BuildQuad(	0.0f,h,   u, v1,col,
														0.0f,0.0f,u, v, col,
														w,   h,   u1,v1,col,
														w,   0.0f,u1,v, col,
														w,   h,   u1,v1,col,
														0.0f,0.0f,u, v, col);
		GEngine->Video->Hud->SetStream();

#ifdef TRANSGAMING
        tmpPlane.set(0.25f,0.25f,0.25f,0.25f);
		dev->SetPixelShaderMemory(0,&tmpPlane,1);
		tmpPlane.set(0.75f,0.75f,0.75f,0.75f);
		dev->SetPixelShaderMemory(1,&tmpPlane,1);
		tmpPlane.set(0.75f,0.75f,0.75f,0.75f);
		dev->SetPixelShaderMemory(2,&tmpPlane,1);
		tmpPlane.set(0.25f,0.25f,0.25f,0.25f);
		dev->SetPixelShaderMemory(3,&tmpPlane,1);
#else
		dev->SetPixelShaderMemory(0,&Plane(0.25f,0.25f,0.25f,0.25f),1);
		dev->SetPixelShaderMemory(1,&Plane(0.75f,0.75f,0.75f,0.75f),1);
		dev->SetPixelShaderMemory(2,&Plane(0.75f,0.75f,0.75f,0.75f),1);
		dev->SetPixelShaderMemory(3,&Plane(0.25f,0.25f,0.25f,0.25f),1);
#endif
		// blur horizontaly
		dev->SetRenderTarget(model->ShadowMapFilter, NULL);
		dev->ClearFB(0xFF000000);
		dev->SetViewport(1,1,model->ShadowMap->Width-2,model->ShadowMap->Width-2);

#ifdef TRANSGAMING
        tmpPlane.set(-1.5f/texWidth,0,0,0);
        dev->SetShaderMemory(10,&tmpPlane,1);
        tmpPlane.set(-0.5f/texWidth,0,0,0);
		dev->SetShaderMemory(11,&tmpPlane,1);
        tmpPlane.set(0.5f/texWidth,0,0,0);
		dev->SetShaderMemory(12,&tmpPlane,1);
        tmpPlane.set(1.5f/texWidth,0,0,0);
		dev->SetShaderMemory(13,&tmpPlane,1);
#else
		dev->SetShaderMemory(10,&Plane(-1.5f/texWidth,0,0,0),1);
		dev->SetShaderMemory(11,&Plane(-0.5f/texWidth,0,0,0),1);
		dev->SetShaderMemory(12,&Plane(0.5f/texWidth,0,0,0),1);
		dev->SetShaderMemory(13,&Plane(1.5f/texWidth,0,0,0),1);
#endif

		MeshShape &mesh = Meshes[0];
		for (int iPass=0; iPass<mesh.BlurShader->PassCount; ++iPass)
		{
			RenderPass& pass = mesh.BlurShader->Passes[iPass];	
			pass.Activate();

#ifdef TRANSGAMING
            MaterialInput tmpMaterial(model->ShadowMap, notex, notex);
            pass.SetInput(&tmpMaterial);
#else
			pass.SetInput(&MaterialInput(model->ShadowMap, notex, notex));
#endif
			dev->Draw(RenderDevice::EPrimTriList,unlock,2);
		}

		dev->SetRenderTarget(model->ShadowMap, NULL);
		dev->SetViewport(1,1,model->ShadowMap->Width-2,model->ShadowMap->Width-2);

#ifdef TRANSGAMING
        tmpPlane.set(0,-1.5f/texWidth,0,0);
        dev->SetShaderMemory(10,&tmpPlane,1);
        tmpPlane.set(0,-0.5f/texWidth,0,0);
		dev->SetShaderMemory(11,&tmpPlane,1);
        tmpPlane.set(0,0.5f/texWidth,0,0);
		dev->SetShaderMemory(12,&tmpPlane,1);
        tmpPlane.set(0,1.5f/texWidth,0,0);
		dev->SetShaderMemory(13,&tmpPlane,1);
#else
		dev->SetShaderMemory(10,&Plane(0,-1.5f/texWidth,0,0),1);
		dev->SetShaderMemory(11,&Plane(0,-0.5f/texWidth,0,0),1);
		dev->SetShaderMemory(12,&Plane(0,0.5f/texWidth,0,0),1);
		dev->SetShaderMemory(13,&Plane(0,1.5f/texWidth,0,0),1);
#endif

		for (int iPass=0; iPass<mesh.BlurShader->PassCount; ++iPass)
		{
			RenderPass& pass = mesh.BlurShader->Passes[iPass];	
			pass.Activate();

#ifdef TRANSGAMING
            MaterialInput tmpMaterial(model->ShadowMapFilter, notex, notex);

            pass.SetInput(&tmpMaterial);
#else
			pass.SetInput(&MaterialInput(model->ShadowMapFilter, notex, notex));
#endif
			dev->Draw(RenderDevice::EPrimTriList,unlock,2);
		}
	}

	dev->SetFBRenderTarget();
#ifdef	TRANSGAMING
	dev->SetFog( GEngine->pcfWorld->UseFog );
#else
	dev->SetFog( GEngine->World->UseFog );
#endif
}



