#include "Engine.h"
#include "Render/HUD.h"

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

#include "Render/Decal.h"
#include "Render/View.h"
#include "Render/FogVolume.h"
#include "Physics/Ragdoll.h"

#include "Physics/PhysicsWorld.h"
#include "Net/NetworkDevice2.h"

#include "Engine/ModelAnimation.h"
#include "Engine/AnimatedMeshManager.h"
#include "Render/Video.h"
#include "Engine/Model.h"
#include "Engine/World.h"
#include "Render/Material.h"
#include "Physics/PhysicsObject.h"


/* min and max macros */
#ifndef NOMINMAX
#ifndef max
#define max(a,b)   (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b)   (((a) < (b)) ? (a) : (b))
#endif
#endif  /* NOMINMAX */

Model::Model(void) : Entity(ETypeModel)
{
	Lighting		= ELightingAllLights;
	RagDoll         = false;
	RagdollPtr		= NULL;
	Mesh			= NULL;
	Center			= Vector(0,0,0);
	CenterJointId	= -1;
	BlendAlpha		= 1;
    ShadowEnabled = false;

	for (int i=0; i<MAX_IMPACTS; ++i)
	{
		Impact[i].Epicenter = Vector(0,60,0);
		Impact[i].Amplitude = 0.0f;
		Impact[i].Period	 = 1.0f;
		Impact[i].Range	 = 20.0f;
		Impact[i].Speed	 = 6.0f;
		Impact[i].Time = 0.0f;
	}
	Impact[0].Epicenter = Vector(-10,60, 10);
	Impact[1].Epicenter = Vector(-10,60,-10);
	Impact[2].Epicenter = Vector( 10,60,-10);
	Impact[3].Epicenter = Vector( 10,60, 10);

	ShadowDecal		= NULL;
	ShadowMap		= NULL;
	ShadowMapFilter	= NULL;
	ShadowUpdateFrame = 0;

	LastDrawTime		= 0.0f;
	LastAnimateFrame	= -1;
    LastRagdollFrame    = -1;
	FullBrightColor		= Vector(1,1,1);
}

Model::~Model(void)
{
#ifndef TRANSGAMING_NO_HAVOK
	if( RagdollPtr ) delete RagdollPtr;
#else
	RagdollPtr = 0;
#endif
}

void Model::UpdateTransform(bool updateInWorld)
{
	Entity::UpdateTransform(updateInWorld);

	if (CenterJointId==-1)
		Center = Box.GetCenter();
}

BOOL Model::Load(const char *name, float scale, AnimatedMeshManager& meshMgr)
{
	Mesh = meshMgr.CreateAnimatedMesh(name, scale);

	if (Mesh)
	{
		int size = Mesh->Skeleton.Joints.Size();
		Skeleton.Joints.ResizeArray(size);

		MeshPropertiesArr.ResizeArray(Mesh->Meshes.Size());

		for (int iMesh=0; iMesh<MeshPropertiesArr.Size(); ++iMesh)
		{
			MeshPropertiesArr[iMesh].Groups.ResizeArray(Mesh->Meshes[iMesh].Data->Groups.Size());
		}

		for (int iJoint=0; iJoint<size; iJoint++)
		{
			Skeleton.Joints[iJoint].Transform	= Mesh->Skeleton.Joints[iJoint].Mat;
			Skeleton.Joints[iJoint].Global		= false;
		}

		matricesArr.ResizeArray(size);
		FrameMatrixArr.ResizeArray(size);

		LocalBox = Mesh->Box;
		UpdateTransform();
        
		//GLog.Print("Create ragdoll %s\n", Mesh->Name);
#ifndef TRANSGAMING_NO_HAVOK
		if( RagdollPtr )
			delete RagdollPtr;
#else
		RagdollPtr = 0;
#endif
		CreateRagdollIfNone();

		CenterJointId = GetJointIndex("root");

		Sort = Mesh->Sort;

		for (int i=0; i<Mesh->Meshes.Size(); ++i) {
			const String& MeshName = Mesh->Meshes[i].Name;
			if (strstr(MeshName.AsChar(),"warp")) {
				WarpScreen = true;
				Mesh->Meshes[i].IsWarp = true;

				int num = MeshName.Len() >= 5 ? MeshName[4] - '0' : 0;
				if (num < MAX_WARP_NUM) {
					Mesh->Warps[num] = i;

					if (!GEngine->Video)
						continue;

					String shaderName = "palskinned_warp";
					shaderName += String::Sprintf("%d",num);
					Mesh->WarpShaders[num] = GEngine->Video->MaterialSys->FindMaterial(shaderName.AsChar());
					shaderName += "_fake";
					Mesh->WarpFakeShaders[num] = GEngine->Video->MaterialSys->FindMaterial(shaderName.AsChar());
				}
			}
		}

		return true;
	}

	return false;
}

void Model::RenderCleanup(RenderDevice* dev, MaterialSystem* matSys)
{
#ifdef	TRANSGAMING
	if (ShadowMap)
		GEngine->pcfWorld->AnimMeshManager.ReleaseShadowMap(ShadowMap);
#else
	if (ShadowMap)
		GEngine->World->AnimMeshManager.ReleaseShadowMap(ShadowMap);
#endif	

	for (int iMesh=0; iMesh<Mesh->Meshes.Size(); ++iMesh)
	{
		MeshProperties &meshProp = MeshPropertiesArr[iMesh];

		for (int iGroup=0; iGroup<meshProp.Groups.Size(); ++iGroup)
		{
			MeshProperties::GroupProperties &groupProp = meshProp.Groups[iGroup];
			if (groupProp.TexPtr)
				GEngine->Video->MaterialSys->ReleaseTexture(groupProp.TexPtr);
		}
	}
}

void Model::CreateShadowMap(bool enabled)
{

    ShadowEnabled = enabled;

    if (!GEngine->Video)
		return;

	if (ShadowMap)
#ifdef	TRANSGAMING
		GEngine->pcfWorld->AnimMeshManager.ReleaseShadowMap(ShadowMap);
#else
		GEngine->World->AnimMeshManager.ReleaseShadowMap(ShadowMap);
#endif	

	ShadowMap = NULL;
	ShadowMapFilter = NULL;

	if (Mesh->Meshes.Size()==0)
		return;

	if (!enabled || !GEngine->Video->ShadowsEnabled)
	{
		if (ShadowDecal)
		{
#ifdef	TRANSGAMING
			GEngine->pcfWorld->DeleteEntityDelayed(ShadowDecal);
#else
			GEngine->World->DeleteEntityDelayed(ShadowDecal);
#endif	
			ShadowDecal=NULL;
		}
		return;
	}

	if( !GEngine->Video->ShadowsEnabled )
		return;

#ifdef	TRANSGAMING
	if (!ShadowDecal)
	{
		ShadowDecal = (Decal*)GEngine->pcfWorld->CreateEntity(Entity::ETypeDecal,"shadow","",1);
		GEngine->pcfWorld->AddEntity(ShadowDecal);
		ShadowDecal->EnableDraw(false);
		RegisterChild(ShadowDecal,false);
	}

	ShadowMap = GEngine->pcfWorld->AnimMeshManager.CreateShadowMap(128);
	ShadowMapFilter = GEngine->pcfWorld->AnimMeshManager.ShadowMapFilter;
#else
	if (!ShadowDecal)
	{
		ShadowDecal = (Decal*)GEngine->World->CreateEntity(Entity::ETypeDecal,"shadow","",1);
		GEngine->World->AddEntity(ShadowDecal);
		ShadowDecal->EnableDraw(false);
		RegisterChild(ShadowDecal,false);
	}

	ShadowMap = GEngine->World->AnimMeshManager.CreateShadowMap(128);
	ShadowMapFilter = GEngine->World->AnimMeshManager.ShadowMapFilter;
#endif	
}

void Model::ProcessRagdoll()
{
	DynamicArray<RagdollJoint> &rjnt = Skeleton.Joints;
	DynamicArray<Joint>         &jnt = Mesh->Skeleton.Joints;
	DynamicArray<Matrix>		rjntinv(rjnt.Size());

	for (int i=0; i<rjnt.Size(); ++i)
		rjntinv[i] = jnt[i].Global.Inverse();


	for (int i=0; i<Mesh->Meshes.Size(); ++i) {

		MeshShape &m = Mesh->Meshes[i];

		for (int j=0; j<m.Data->VertList.Size(); ++j) {

			Vector pos = m.Data->VertList.getVertPos(j);
			DynamicArray<Weight> &wgh = m.Influences[j].Weights;

			for (int k=0; k<wgh.Size(); ++k) {

				int b = wgh[k].bone;

				do {
					if (rjnt[b].Global) {					
						rjnt[b].LocalBox += rjntinv[b].TransformCoord(pos);
						break;
					}
					b = jnt[b].Parent;
				}
				while (b != -1);
			}
		}
	}

	for (int i=0; i<rjnt.Size(); ++i)
		if (rjnt[i].Global & !rjnt[i].LocalBox.Valid())
			rjnt[i].LocalBox += Vector(0,0,0);
}

void Model::RecomputeLocalBB()
{
	if( !RagdollPtr )
		return;

	LocalBox.Reset();
	DynamicArray<RagdollJoint> &rjnt = Skeleton.Joints;
	Matrix ObjectToWorldInv = ObjectToWorld.Inverse();

	for (int i=0; i<rjnt.Size(); ++i)
	{
		if (rjnt[i].Global)
		{
			LocalBox += rjnt[i].LocalBox.Transform(FrameMatrixArr[i]*ObjectToWorldInv);
		}
	}
}

int Model::LoadAnimation(const char *name, bool looped, int forceSlot)
{
	// One of few network code injections
	if( GEngine->Net && GEngine->Net->IsServer() && ( SynchroState() & ESynchroEntity ) )
		GEngine->Net->OnLoadAnimation( this, name, looped, forceSlot );

	return Mesh->LoadAnimation(name, looped, forceSlot);
}

void Model::Draw(RenderDevice* dev, int mode)
{
	LastDrawTime = 0;

	if (!(mode & View::EModeShowModels))
		return;
	if ((mode & View::EModeMultiPass) && (Sort!=ESortTrans))
		return;
	if ((mode & View::EModeZPass) && (Sort==ESortTrans))
		return;
	if ((mode & View::EModeShadowMapPass) && (ShadowMap==NULL))
		return;
	if (!(mode & View::EModeShadowMapPass) && Demonic)
		if (!((mode & View::EModeDemonFX) || (mode & View::EModeGhostFX)))
			return;

	if (mode & View::EModeWarpPass)
	{
		Mesh->Render(this, mode);
		return;
	}

#ifdef TRANSGAMING_USE_EDITOR
	if( mode & View::EModeShowJoints )
    {
        DrawSkeleton(dev);

		return;
    }
#endif

	if (AnimController.HasAnyAnimations())
	{
		const DynamicArray<Matrix> &bindMatrixArr = Mesh->Skeleton.InvBindMatrixArr;
		ComputeFrameMatrix(FrameMatrixArr, ObjectToWorld);
        
        // never animate ragdolls on MP clients
        if (GEngine->GameState != PCFSystem::MultiplayerClient)
        {
            if( WorldLink && RagdollPtr && (DWORD)LastRagdollFrame != WorldLink->Frame ) 
            {
#ifndef TRANSGAMING_NO_HAVOK
                RagdollPtr->Animate( FrameMatrixArr );
#endif
                LastRagdollFrame = WorldLink->Frame;
            }            
        }
		
        for (int iJoint=0; iJoint<matricesArr.Size(); iJoint++)
		{
			matricesArr[iJoint] = bindMatrixArr[iJoint]*FrameMatrixArr[iJoint];
		}

		Mesh->AnimateGeometry(matricesArr);
	    dev->SetObjectTransform(GIdentityMatrix);
	}
	else
	{
		Mesh->SetBindPoseFrame(this);
	    dev->SetObjectTransform(GIdentityMatrix);
	}

	Mesh->Render(this, mode);
}

#ifdef TRANSGAMING_USE_EDITOR
void Model::DrawSkeleton(RenderDevice* dev)
{
	const DWORD color = 0x3F003F;
	const float radius = 0.01f;
	const float boneradius = 0.007f;

	ComputeFrameMatrix(FrameMatrixArr, ObjectToWorld);

	GEngine->Video->LineBuf.SetState(dev);

	for (int iJoint=0; iJoint<FrameMatrixArr.Size(); iJoint++)
	{
		const Joint &joint = Mesh->Skeleton.Joints[iJoint];

		if (joint.Parent!=-1)
		{
			RenderJoint(color, 
				FrameMatrixArr[joint.Parent].GetTranslation(), 
				FrameMatrixArr[iJoint].GetTranslation(), 
				boneradius, radius);
		}

		RenderSphere(color, FrameMatrixArr[iJoint], radius);
	}
}

void Model::DrawJointNames(DWORD color)
{
	TextureState st[2];
	st[0].Set(0,TextureState::ETexOpColor,0,TextureState::ETexFilterPoint);
	st[1].Set(1,TextureState::ETexOpDisable);
	RenderDevice *dev = GEngine->GetRenderDevice();
	dev->SetTextureState(st,ARRAY_COUNT(st));

	Viewport &pViewport = GEngine->Video->GetActiveView()->LocalViewport;

	ComputeFrameMatrix(FrameMatrixArr, ObjectToWorld);
	for (int iJoint=0; iJoint<FrameMatrixArr.Size(); iJoint++)
	{
        DWORD col = color;
        if (Skeleton.Joints[iJoint].Global)
#ifdef	TRANSGAMING
            col = Color_TG(1,1,0).dwVal;
#else
            col = Color(1,1,0).dwVal;
#endif

		Vector vscr = pViewport.WorldToScreen.Transform(FrameMatrixArr[iJoint].GetTranslation());
		vscr.X =  (vscr.X + 1) * pViewport.Width /2;
		vscr.Y =  (-vscr.Y + 1) * pViewport.Height /2;

		GEngine->Video->Hud->Print((int)vscr.X, (int)vscr.Y, Mesh->Skeleton.Joints[iJoint].Name.AsChar(), col);
	}
}

void Model::RenderJoint(DWORD color, const Vector& p0, const Vector& p1, float mag, float mag2)
{	
    // do some precalculations
	Vector p    = p1-p0;
    Vector left = p.MinimalAxis()^p;
    Vector up   = left^p;
    Vector fwd  = up^left;
    left.Normalize();   left *= mag;
    up.Normalize();     up   *= mag;
    fwd.Normalize();    fwd  *= mag2;


    // do rendering    
    RenderDevice* dev=GEngine->GetRenderDevice();
    Assert(dev);

	LineRender::LineBuffer &VBuf = GEngine->Video->LineBuf.VBuf;

    RenderState state;
    state.SetConstColor(color);
    dev->SetState(state);

    dev->SetObjectTransform(GIdentityMatrix);

    const int primsb = 8;
	Vector* v = VBuf.Lock(2*primsb);
    if (!v) return;
	// bone
	*v++ = p1-fwd;        *v++ = p0+fwd+left+up;
	*v++ = p1-fwd;        *v++ = p0+fwd+left-up;
	*v++ = p1-fwd;        *v++ = p0+fwd-left-up;
	*v++ = p1-fwd;        *v++ = p0+fwd-left+up;

	*v++ = p0+fwd-left+up;  *v++ = p0+fwd+left+up;
	*v++ = p0+fwd+left+up;  *v++ = p0+fwd+left-up;
	*v++ = p0+fwd+left-up;  *v++ = p0+fwd-left-up;
	*v++ = p0+fwd-left-up;  *v++ = p0+fwd-left+up;

	dev->Draw(RenderDevice::EPrimLineList, VBuf.Unlock(), primsb);
}

void Model::RenderSphere(DWORD color, const Matrix& m, float radius)
{
    RenderDevice* dev=GEngine->GetRenderDevice();
    Assert(dev);
	LineRender::LineBuffer &VBuf = GEngine->Video->LineBuf.VBuf;

	Matrix mnorm = m;
	mnorm.OrthoNormalize();

	RenderState state;
    state.SetConstColor(color);
    dev->SetState(state);

    dev->SetObjectTransform(&mnorm);

    Vector joint[3][12];
    const float sin[12] = {0, .5f, .866f, 1, .866f, .5f, 0, -.5f, -.866f, -1, -.866f, -.5f};
    const float cos[12] = {1, .866f, .5f, 0, -.5f, -.866f, -1, -.866f, -.5f, 0, .5f, .866f};
    for(int i=0; i<12; i++)
    {        
        joint[0][i] = Vector(0, sin[i]*radius, cos[i]*radius);
        joint[1][i] = Vector(sin[i]*radius, 0, cos[i]*radius);
        joint[2][i] = Vector(sin[i]*radius, cos[i]*radius, 0);
    }

	const int primsj = 3*12;
    Vector* v = VBuf.Lock(2*primsj);
    if (!v) return;
    // joint
    for (int a=0; a<3; a++)
        for (int i=0; i<12; i++)
        {
            *v++ = joint[a][i];
            *v++ = joint[a][(i+1)%12];
        }
	dev->Draw(RenderDevice::EPrimLineList, VBuf.Unlock(), primsj);
}
#endif

BOOL Model::IsPerPixel() const
{
	if (!Mesh)
		return FALSE;

	for (int i=0; i<Mesh->Meshes.Size(); i++)
	{
		if (Mesh->Meshes[i].RenderType==MeshShape::EPixelGPUSkin)
			return TRUE;
	}

	return FALSE;
}

void Model::UpdateVolEntity() 
{
	if (!WorldLink)
		return;

	for (int i=0; i<WorldLink->Volumes.Size(); ++i)
	{
		Volume* vol = WorldLink->Volumes[i];
		if (vol->Box.Intersect(Box))
			vol->DEntities.Add(this);
	}
}

int Model::GetJointIndex(const char *jointName) const
{
	return Mesh->Skeleton.JointIndex(jointName);
}

#ifdef TRANSGAMING
const char *Model::GetJointName(int joint) const
{
    if (joint<0 || !Mesh || joint>=Mesh->Skeleton.Joints.Size())
		return "<none>";

	return Mesh->Skeleton.Joints[joint].Name.AsChar();
}
#else
String Model::GetJointName(int joint) const
{
	if (joint<0 || !Mesh || joint>=Mesh->Skeleton.Joints.Size())
		return "<none>";
	return Mesh->Skeleton.Joints[joint].Name;
}
#endif


Matrix Model::GetJointTransform(int joint)
{
	ComputeFrameMatrix(FrameMatrixArr, ObjectToWorld);
    
    if (joint > -1 && joint < FrameMatrixArr.Size())
        return FrameMatrixArr[joint];

	return *GIdentityMatrix;
}

void Model::Tick(float timeDelta)
{
	Entity::Tick(timeDelta);

	for (int i=0; i<MAX_IMPACTS; ++i)
		Impact[i].Time += timeDelta;

	LastDrawTime += timeDelta;

	if (TimeToDelete != -1 && TimeToDelete < FadeTime)
	{
		BlendAlpha = TimeToDelete/FadeTime;
		Sort = ESortTrans;
		SetMaterial("blendpalskinned");
	}

	if (AnimController.HasAnyAnimations())
    {
		AnimController.Tick(timeDelta);
        // always animate ragdolls on server
#ifndef TRANSGAMING_NO_DEDICATED_SERVER 
        if (GEngine->GameState == PCFSystem::DedicatedServer ||  GEngine->GameState == PCFSystem::MultiplayerServer)
#else
        if (GEngine->GameState == PCFSystem::MultiplayerServer)
#endif
        {
            ComputeFrameMatrix(FrameMatrixArr, ObjectToWorld);

            if( RagdollPtr && (DWORD)LastRagdollFrame!=WorldLink->Frame) 
            {
#ifndef TRANSGAMING_NO_HAVOK
                RagdollPtr->Animate( FrameMatrixArr );
#endif
                LastRagdollFrame = WorldLink->Frame;
            }            
        }
    }	

	if (ShadowMap)
	{
		Vector cener = Box.GetCenter();

	    if (!(Flags & Entity::EEntNoDraw))
		{
			/*
			Entity *entity = GEngine->World->GetEntityByIndex(ShadowDecal->SpawnedOnId);
			
			if (GEngine->World->Frame%30==0)
			{
				HitData hitData;
				if (GEngine->World->Physics->LineTraceFixedGeom(cener,cener-Vector(0,5,0),hitData))
					entity = (Entity*)hitData.UserData;
			}			

			if (!entity)
				return;
			*/
			Plane N(0,0,0,0), T(0,0,0,0), B(0,0,0,0);

			N.Normal = WorldLink->Env.DirLight.GetDirection(); 

			if (LightCount>=1) {
				N.Normal.Set(-VSLights[0].Dir.X, -VSLights[0].Dir.Y, -VSLights[0].Dir.Z);
			}

			if (LightCount>=2) {
				Plane n = -VSLights[1].Dir;
				float i = min(1,VSLights[1].Color.W);
				N.X = n.X*i + N.X*(1-i);
				N.Z = n.Z*i + N.Z*(1-i);
			}

			N.Normalize();
			T.Normal = N.Normal^N.Normal.MinimalAxis(); T.Normalize();
			B.Normal = N.Normal^T.Normal;

			BoundingBox box = Box;
			box.Min -= cener;
			box.Max -= cener;

			Vector v;
			v = box.GetCorner(N.Normal.Sign());
			N.Normal *= 2*v.Dot(N.Normal);

			v = box.GetCorner(B.Normal.Sign());
			B.Normal *= 2*v.Dot(B.Normal);

			v = box.GetCorner(T.Normal.Sign());
			T.Normal *= 2*v.Dot(T.Normal);

			Plane P(cener,1);

			Matrix spawn(T,B,N,P);

			ShadowMatrix = spawn;
			/*
			ShadowDecal->SpawnedOnId = entity->GIndex;
			ShadowDecal->Spawn(entity,spawn);
			ShadowDecal->SetTexture(ShadowMap);
			ShadowDecal->Flags &= ~EEntNoDraw;
			*/
		}
		else
		{
			//ShadowDecal->Flags |= EEntNoDraw;
		}
	}
}

void Model::SetRagdoll( bool enable, int collisionGroup )
{
	if (enable==RagDoll)
	{
		return;
	}

    RagDoll = enable;

    if (RagDoll)
    {
		ComputeFrameMatrix(FrameMatrixArr, ObjectToWorld);

		if( RagdollPtr )
        {
			for (int i=0; i<FrameMatrixArr.Size(); i++)
			{
				float a = FrameMatrixArr[i].A00;
				if (a<-1e6f || a>1e6f)
				{
					GLog.Print( "Bad matrix during ragdoll activate, model name: %s, filename: %s\n", Name.AsChar(), FileName.AsChar() );
					break;
				}
			}
#ifndef TRANSGAMING_NO_HAVOK
            RagdollPtr->Animate( FrameMatrixArr );
            RagdollPtr->Activate( FrameMatrixArr, collisionGroup );
#endif
        }

		ActivateAnimation(0); // ragdoll       
        RecomputeLocalBB();
        UpdateTransform();
    }
	else
	{
		ActivateAnimation(1);

        if( RagdollPtr && Mesh)
        {
#ifndef TRANSGAMING_NO_HAVOK
            RagdollPtr->Deactivate();
#endif
    		LocalBox = Mesh->Box;
	    	UpdateTransform();	
        }
	}
}

bool Model::IsRagdoll()
{
    return RagDoll;
}


bool Model::ComputeFrameMatrix(DynamicArray<Matrix>& frameMatrixArr, const Matrix &world)
{
#ifdef	TRANSGAMING
    if ((DWORD)LastAnimateFrame == GEngine->pcfWorld->Frame) // already animated in this tick
		return false;

	LastAnimateFrame = GEngine->pcfWorld->Frame;
#else
    if ((DWORD)LastAnimateFrame == GEngine->World->Frame) // already animated in this tick
		return false;

	LastAnimateFrame = GEngine->World->Frame;
#endif	

    if (AnimController.HasAnyAnimations())
    {
		int size = AnimController.Animations.Size();

		if (size == 1)
		{
			ModelAnimation *anim = AnimController.Animations[0].Animation;
			float frameTime = AnimController.Animations[0].FrameTime;

			anim->ComputeFrameMatrix(frameMatrixArr, frameTime, this, world);
		}
		else if (size == 2)
		{
			const int maxSize = 2;
			DynamicArray<Matrix> blendFrameMatrixArr[maxSize];
			// blend animations
			for (int i=0; i<maxSize; i++)
			{
				float frameTime = AnimController.Animations[i].FrameTime;

                blendFrameMatrixArr[i].ResizeArray(frameMatrixArr.Size());
				AnimController.Animations[i].Animation->ComputeFrameMatrix(blendFrameMatrixArr[i], frameTime, this, world);
			}

			int jointsNum = blendFrameMatrixArr[0].Size();

			float t = 1-AnimController.Animations[0].Weight;

			for (int iJoint=0; iJoint<jointsNum; iJoint++)
			{
				Matrix &mm = frameMatrixArr[iJoint];

				Quaternion q0(blendFrameMatrixArr[0][iJoint]);
				Quaternion q1(blendFrameMatrixArr[1][iJoint]);
				Quaternion qslerp = Quaternion::Slerp(t, q0, q1);
				mm = qslerp.ToMatrix();
				Vector p0 = blendFrameMatrixArr[0][iJoint].GetTranslation();
				Vector p1 = blendFrameMatrixArr[1][iJoint].GetTranslation();
				mm.SetTranslation(p0*(1-t) + p1*t);				
			}
		}
		else 
		{
			int maxSize = AnimController.Animations.Size();
			DynamicArray<DynamicArray<Matrix> > blendFrameMatrixArr;
			blendFrameMatrixArr.ResizeArray(maxSize);

			// blend animations
			for (int i=0; i<maxSize; i++)
			{
				float frameTime = AnimController.Animations[i].FrameTime;

				blendFrameMatrixArr[i].ResizeArray(frameMatrixArr.Size());
				AnimController.Animations[i].Animation->ComputeFrameMatrix(blendFrameMatrixArr[i], frameTime, this, world);
			}

			int jointsNum = blendFrameMatrixArr[0].Size();

			for (int iJoint=0; iJoint<jointsNum; iJoint++)
			{
				Quaternion q0(blendFrameMatrixArr[0][iJoint]);
				Vector p0 = blendFrameMatrixArr[0][iJoint].GetTranslation();

				float accumWeight = AnimController.Animations[0].Weight;

				for (int i=1; i<maxSize; i++)
				{
					float weight = AnimController.Animations[i].Weight;

					// Added by Krystian to get rid of ragdoll disappearing by #INF in factor
					if( weight == 0.0f ) continue;

					accumWeight += weight;
					float factor = weight / accumWeight;

					Quaternion q1(blendFrameMatrixArr[i][iJoint]);
					q0 = Quaternion::Slerp(factor, q0, q1);

					Vector p1 = blendFrameMatrixArr[i][iJoint].GetTranslation();
					p0 = p0*(1-factor) + p1*factor;
				}

				Matrix &mm = frameMatrixArr[iJoint];
				mm = q0.ToMatrix();
				mm.SetTranslation(p0);
			}
		}
    }
    else
    {
        frameMatrixArr = Mesh->ComputeBindMatrix(world);
    }

	if (CenterJointId!=-1)
		Center = frameMatrixArr[CenterJointId].GetTranslation();
	else
		Center = Box.GetCenter();

	RecomputeLocalBB();
	Box = LocalBox.Transform(ObjectToWorld);
	UpdateTransform();
/*
	if (IsRagdoll() || CenterJointId!=-1)
		UpdateTransform();
	else
		Box = LocalBox.Transform(ObjectToWorld);
*/  
    return true;
}

float Model::GetAnimationFrameTime(int anim) const
{
	if (anim >= 0)
		return AnimController.GetAnimationFrameTime(Mesh->Animations[anim]);
	return 0;
}

void Model::SetAnimationFrameTime(int anim, float time)
{
	if (anim >= 0)
		AnimController.SetAnimationFrameTime(Mesh->Animations[anim], time);
}


void Model::SetAnimationTimeScale(int anim, float timeScale)
{
	if (anim >= 0)
		AnimController.SetAnimationTimeScale(Mesh->Animations[anim], timeScale);
}

float Model::GetAnimationTimeScale(int anim)
{
	return (anim<0)?1.0f:AnimController.GetAnimationTimeScale(Mesh->Animations[anim]);
}

float Model::GetAnimationTotalTime(int anim) const
{
	if (anim<0 || anim>Mesh->Animations.Size())
	{
		return 0.0f;
	}
	else
	{
		return Mesh->Animations[anim]->Time;
	}
}

int Model::ActivateAnimation(int anim, bool looped, float animScale, float blendTime, bool startAfterBlend)
{
	// One of few network code injections
	if( GEngine->Net && GEngine->Net->IsServer() && ( SynchroState() & ESynchroEntity ) )
	{
		bool mc = false;
		if( anim>=0 && anim<Mesh->Animations.Size() )
			mc = ((SkeletalAnimation*)Mesh->Animations[anim])->HasMovementCurve()!=0;
		GEngine->Net->OnActivateAnimation( this, anim, looped, blendTime, animScale, mc );
	}

//	GLog.Print("Activate animation - object: %d, number: %d\n", GetSynchroNumber(), anim );
	if (anim>=0 && anim<Mesh->Animations.Size())
	{
//		GLog.Print( "Animation %d = '%s', %s\n", anim, Mesh->Animations[anim]->Name.AsChar(), Mesh->Animations[anim]->Looped?"looped":"not looped" );
		AnimController.ActivateAnimation(Mesh->Animations[anim], looped, startAfterBlend, animScale, anim>0 ? blendTime : 0.0f);

//		GLog.Print("Activate animation time scale: %f\n", GetAnimationTimeScale(anim) );
		return anim;
	}
	else
	{
		AnimController.ActivateAnimation(NULL, true, false, 1, 1);
	}
	return -1;
}

#ifdef TRANSGAMING
BOOL Model::SetAnimationMovementCurve(const int anim, const char *curvename, DWORD lockAxis)
{
	if (anim>=0 && anim<Mesh->Animations.Size())
	{
		Mesh->Animations[anim]->SetMovementCurve(curvename, lockAxis);
		return TRUE;
	}

	return FALSE;
}
#else
BOOL Model::SetAnimationMovementCurve(const int anim, const String& curvename, DWORD lockAxis)
{
	if (anim>=0 && anim<Mesh->Animations.Size())
	{
		Mesh->Animations[anim]->SetMovementCurve(curvename, lockAxis);
		return TRUE;
	}

	return FALSE;
}
#endif

Vector Model::GetAnimationMovement(const int anim, const float deltaTime) const
{
	if (anim>=0 && anim<Mesh->Animations.Size())
	{
		return AnimController.GetAnimationMovement(Mesh->Animations[anim], deltaTime);
	}

	return Vector(0,0,0);
}

void Model::ApplyJointRotation(int joint, float heading, float pitch, float yaw)
{
	Quaternion qrot = Quaternion(heading, Vector(1,0,0))*Quaternion(pitch, Vector(0,0,1))*Quaternion(yaw, Vector(0,1,0));

	bool nullRot = (heading==0 && pitch==0 && yaw==0);

	for (int i=0; i<AnimatedJoints.Size(); i++)
	{
		if (AnimatedJoints[i].Joint==joint) 
		{
			if (nullRot)
				AnimatedJoints.FastRemove(i);
			else
				AnimatedJoints[i].Angle=qrot;

			return;
		}
	}

	if (!nullRot)
		AnimatedJoints.Add(AnimatedJoint(joint,qrot));
}

void Model::SetWaterImpact(int impact, const Vector& epicenter, float amplitude, float period, float range, float speed)
{
	if (impact<0 || impact>=MAX_IMPACTS)
		return;

	Impact[impact].Time = 0;

	UpdateWaterImpact(impact, epicenter, amplitude, period, range, speed);    
}

void Model::UpdateWaterImpact(int impact, const Vector& epicenter, float amplitude, float period, float range, float speed)
{
	if (impact<0 || impact>=MAX_IMPACTS)
		return;

	Impact[impact].Epicenter = epicenter;
	Impact[impact].Amplitude = amplitude;
	Impact[impact].Period	 = period;
	Impact[impact].Range	 = range;
	Impact[impact].Speed	 = speed;
}

void Model::ResetMaterialSpecular()
{
	if (!Mesh)
	{
		return;
	}

	for (int i=0; i<Mesh->Meshes.Size(); i++)
	{
		MeshShape &m = Mesh->Meshes[i];
		m.MatSpecColor = Vector(0.5f,0.5f,0.5f);
		m.MatSpecPower = 20.0f;
		m.MatRefract   = 1.1f;
		m.MatFresnel   = 0.3f;
		m.MatReflTint  = Vector(1.0f,1.0f,1.0f);
		m.MatRefrTint  = Vector(1.0f,1.0f,1.0f);
	}
}

void Model::SetBlendAlpha(float alpha)
{
	if (!Mesh) 
	{
		return;
	}

	BlendAlpha = alpha;
}

#ifdef TRANSGAMING
void Model::SetMeshVisibility(const char *name, bool isVisible)
{
	if (!Mesh)
	{
        return;
	}

    if (!strcmp(name, "-all-"))
    {
        for (int i=0; i<Mesh->Meshes.Size(); i++)
            MeshPropertiesArr[i].IsVisible = isVisible;
        return;
    }

    if (!strcmp(name, "-invert-"))
    {
        for (int i=0; i<Mesh->Meshes.Size(); i++)
            MeshPropertiesArr[i].IsVisible = !MeshPropertiesArr[i].IsVisible;
        return;
    }

	for (int i=0; i<Mesh->Meshes.Size(); i++)
	{
        MeshShape &m = Mesh->Meshes[i];
        if (!strcmp(m.Name.AsChar(), name))
		{
			MeshPropertiesArr[i].IsVisible = isVisible;
			return;
		}
	}
}

void Model::SetMeshLighting(const char *name, bool isLighting, const Vector& color)
{
    if (!Mesh)
    {
        return;
    }

	FullBrightColor = color;

    for (int i=0; i<Mesh->Meshes.Size(); i++)
    {
        MeshShape &m = Mesh->Meshes[i];

        if (!strcmp(name, "*") || !strcmp(m.Name.AsChar(), name))
        {
            m.IsLighting = isLighting;
        }
    }
}

void Model::SetMaterialSpecular(const char *name, const Vector& color, const float power)
{
	if (!Mesh)
	{
		return;
	}

	for (int i=0; i<Mesh->Meshes.Size(); i++)
	{
		MeshShape &m = Mesh->Meshes[i];

 		if (!strcmp(m.Name.AsChar(), name))
		{
			m.MatSpecColor = color;
			m.MatSpecPower = power;
			return;
		}
	}
}

void Model::SetMaterialRefractFresnel(const char *name, const float refract, const float fresnel, const Vector &reflTint, const Vector &refrTint)
{
	if (!Mesh)
	{
		return;
	}

	for (int i=0; i<Mesh->Meshes.Size(); i++)
	{
		MeshShape &m = Mesh->Meshes[i];

 		if (!strcmp(m.Name.AsChar(), name))
		{
			m.MatRefract = refract;
			m.MatFresnel = fresnel;
			m.MatReflTint = reflTint;
			m.MatRefrTint = refrTint;
			return;
		}
	}
}

void Model::SetTexture(const char *originalTexture, const char *newTexture)
{
    char tmpName[MAX_PATH];


    if (!GEngine->Video)
        return;

	for (int iMesh=0; iMesh<Mesh->Meshes.Size(); ++iMesh)
	{
		MeshShape &mesh = Mesh->Meshes[iMesh];
		MeshProperties &meshProp = MeshPropertiesArr[iMesh];

#ifndef TRANSGAMING
		if (mesh.NormalMapName.EqualsNoCase(originalTexture))
		{
			if (meshProp.NormalMapPtr)
				GEngine->Video->MaterialSys->ReleaseTexture(meshProp.NormalMapPtr);

			meshProp.NormalMapPtr = Mesh->CreateTexture(newTexture, 0, RenderDevice::ETexLODWeapons);
		}
#endif

		for (int iGroup=0; iGroup<mesh.Data->Groups.Size(); ++iGroup)
		{
			SimpleMesh::Group &group = mesh.Data->Groups[iGroup];

			if (group.TexName.EqualsNoCase(originalTexture))
			{
				MeshProperties::GroupProperties &groupProp = MeshPropertiesArr[iMesh].Groups[iGroup];

				if (groupProp.TexPtr)
					GEngine->Video->MaterialSys->ReleaseTexture(groupProp.TexPtr);

				groupProp.TexPtr = Mesh->CreateTexture(newTexture, 0, RenderDevice::ETexLODModels);
			}


            snprintf(tmpName, MAX_PATH, "%s_add", group.TexName.AsChar());

			if (!strcasecmp(tmpName, originalTexture))
			{
				MeshProperties::GroupProperties &groupProp = MeshPropertiesArr[iMesh].Groups[iGroup];

				if (groupProp.TexEmPtr)
					GEngine->Video->MaterialSys->ReleaseTexture(groupProp.TexEmPtr);

				groupProp.TexEmPtr = Mesh->CreateTexture(newTexture, 0, RenderDevice::ETexLODModels);
			}

		}
	}
}

void Model::SetMaterial(const char *newMaterial){
	if (!strcmp(MaterialName.AsChar(), newMaterial))
		return;

    if (!GEngine->Video) return;
	MaterialSystem *matSys = GEngine->Video->MaterialSys;

	Material* DefaultShader = matSys->FindMaterial(newMaterial);
    
    MaterialName = newMaterial;

	for (int iMesh=0; iMesh<MeshPropertiesArr.Size(); ++iMesh)
	{
		MeshProperties &meshProps = MeshPropertiesArr[iMesh];
		meshProps.DefaultShader = DefaultShader;
	}
}
#else
void Model::SetMeshVisibility(const String& name, bool isVisible)
{
	if (!Mesh)
	{
        return;
	}

    if (name == "-all-")
    {
        for (int i=0; i<Mesh->Meshes.Size(); i++)
            MeshPropertiesArr[i].IsVisible = isVisible;
        return;
    }

    if (name == "-invert-")
    {
        for (int i=0; i<Mesh->Meshes.Size(); i++)
            MeshPropertiesArr[i].IsVisible = !MeshPropertiesArr[i].IsVisible;
        return;
    }

	for (int i=0; i<Mesh->Meshes.Size(); i++)
	{
        MeshShape &m = Mesh->Meshes[i];
        if (m.Name == name)
		{
			MeshPropertiesArr[i].IsVisible = isVisible;
			return;
		}
	}
}

void Model::SetMeshLighting(const String& name, bool isLighting, const Vector& color)
{
    if (!Mesh)
    {
        return;
    }

	FullBrightColor = color;

    for (int i=0; i<Mesh->Meshes.Size(); i++)
    {
        MeshShape &m = Mesh->Meshes[i];

        if (name.EqualsNoCase("*") || m.Name == name)
        {
            m.IsLighting = isLighting;
        }
    }
}

void Model::SetMaterialSpecular(const String& name, const Vector& color, const float power)
{
	if (!Mesh)
	{
		return;
	}

	for (int i=0; i<Mesh->Meshes.Size(); i++)
	{
		MeshShape &m = Mesh->Meshes[i];

 		if (m.Name == name)
		{
			m.MatSpecColor = color;
			m.MatSpecPower = power;
			return;
		}
	}
}

void Model::SetMaterialRefractFresnel(const String& name, const float refract, const float fresnel, const Vector &reflTint, const Vector &refrTint)
{
	if (!Mesh)
	{
		return;
	}

	for (int i=0; i<Mesh->Meshes.Size(); i++)
	{
		MeshShape &m = Mesh->Meshes[i];

 		if (m.Name == name)
		{
			m.MatRefract = refract;
			m.MatFresnel = fresnel;
			m.MatReflTint = reflTint;
			m.MatRefrTint = refrTint;
			return;
		}
	}
}

void Model::SetTexture(const String& originalTexture, const String& newTexture)
{
    if (!GEngine->Video)
        return;

	for (int iMesh=0; iMesh<Mesh->Meshes.Size(); ++iMesh)
	{
		MeshShape &mesh = Mesh->Meshes[iMesh];
		MeshProperties &meshProp = MeshPropertiesArr[iMesh];

		if (mesh.NormalMapName.EqualsNoCase(originalTexture))
		{
			if (meshProp.NormalMapPtr)
				GEngine->Video->MaterialSys->ReleaseTexture(meshProp.NormalMapPtr);

			meshProp.NormalMapPtr = Mesh->CreateTexture(newTexture.AsChar(), 0, RenderDevice::ETexLODWeapons);
		}

		for (int iGroup=0; iGroup<mesh.Data->Groups.Size(); ++iGroup)
		{
			SimpleMesh::Group &group = mesh.Data->Groups[iGroup];

			if (group.TexName.EqualsNoCase(originalTexture))
			{
				MeshProperties::GroupProperties &groupProp = MeshPropertiesArr[iMesh].Data->Groups[iGroup];

				if (groupProp.TexPtr)
					GEngine->Video->MaterialSys->ReleaseTexture(groupProp.TexPtr);

				groupProp.TexPtr = Mesh->CreateTexture(newTexture.AsChar(), 0, RenderDevice::ETexLODModels);
			}

			String TexNameAdd = group.TexName+"_add";

			if (TexNameAdd.EqualsNoCase(originalTexture))
			{
				MeshProperties::GroupProperties &groupProp = MeshPropertiesArr[iMesh].Data->Groups[iGroup];

				if (groupProp.TexEmPtr)
					GEngine->Video->MaterialSys->ReleaseTexture(groupProp.TexEmPtr);

				groupProp.TexEmPtr = Mesh->CreateTexture(newTexture.AsChar(), 0, RenderDevice::ETexLODModels);
			}

		}
	}
}

void Model::SetMaterial(const String& newMaterial)
{
	if (MaterialName == newMaterial)
		return;

    if (!GEngine->Video) return;
	MaterialSystem *matSys = GEngine->Video->MaterialSys;

	Material* DefaultShader = matSys->FindMaterial(newMaterial.AsChar());
    
    MaterialName = newMaterial;

	for (int iMesh=0; iMesh<MeshPropertiesArr.Size(); ++iMesh)
	{
		MeshProperties &meshProps = MeshPropertiesArr[iMesh];
		meshProps.DefaultShader = DefaultShader;
	}
}
#endif

void Model::SetupGib( Model* gib )
{
	ComputeFrameMatrix(FrameMatrixArr, ObjectToWorld);
	gib->ComputeFrameMatrix(gib->FrameMatrixArr, gib->ObjectToWorld);

	for( int i = 0; i < Mesh->Skeleton.Joints.Size(); ++i )
	{
		for( int j = 0; j < gib->Mesh->Skeleton.Joints.Size(); ++j )
		{
			if( Mesh->Skeleton.Joints[i].Name == gib->Mesh->Skeleton.Joints[j].Name )
			{
				gib->FrameMatrixArr[j] = FrameMatrixArr[i];
				break;
			}
		}
	}

#ifndef TRANSGAMING_NO_HAVOK
	if( gib->RagdollPtr ) gib->RagdollPtr->Animate( gib->FrameMatrixArr );
#endif
}

#define MODEL_LOAD_SAVE_TOKEN 0x5e55

bool Model::LoadEntity( FileObject* f )
{
	if( !f->TestToken( MODEL_LOAD_SAVE_TOKEN ) )
	{
		GLog.Print( "Token test failure: Model::LoadEntity()\n" );
		return false;
	}

	Vector pos;
	Quaternion rot;
	String animName;
	int animSize;
    int activeAnimSize;
    char ok;
	char looped;
	DWORD setMovementCurve;
    char anim;
	int index;

	Entity::LoadEntity( f );

	*f >> pos >> rot;
	SetPositionAndRotation( pos, rot );

    // materials
    *f >> BlendAlpha;
    *f >> MaterialName;
    
    if (MaterialName.Size())
#ifdef TRANSGAMING
        SetMaterial(MaterialName.AsChar());
#else
        SetMaterial(MaterialName);
#endif

    // shadow
    *f >> ShadowEnabled;
    if (ShadowEnabled)
        CreateShadowMap(true);

    // load mesh visibility
    for (int i=0; i<Mesh->Meshes.Size(); i++)
        *f >> MeshPropertiesArr[i].IsVisible;

	// Read animations state for entity and load animations

	*f >> animSize;
	for( int i = 1; i < animSize; ++i )
	{
		*f >> ok;
		if( ok )
		{
			*f >> animName >> looped >> setMovementCurve;
            index = LoadAnimation( animName.AsChar(), looped?true:false , i );
			if( setMovementCurve )
				SetAnimationMovementCurve( index, "ROOOT", setMovementCurve );
		}
	}

    
	// Synchronize animation controller
	AnimController.Animations.ClearArray();
	*f >> activeAnimSize;
	for( int i = 0; i < activeAnimSize; ++i )
	{

		AnimController.Animations.GrowArray(1);
		AnimationParams& a = AnimController.Animations[i];

        *f >> anim;
        *f >> a;        

		//*f >> anim >> a.Weight >> a.TimeScale >> looped >> a.FrameBlendTime >> a.BlendTime;
        //a.Looped = looped?true:false;	// bool field might be 1-bit long32, and then what?

		a.Animation = Mesh->Animations[ anim ];        
	}
    

	// Load PO?
	if( PO )
		RemovePhysicsObject();

	char hasPO;
	*f >>  hasPO;
	if( hasPO )
	{
		DWORD bodyType;
		float bodyScale;
		*f >> bodyType >> bodyScale;	// this data was saved by PO
		CreatePhysicsObject( bodyType, bodyScale, -1, true );
#ifndef TRANSGAMING_NO_HAVOK
		if( !PO->LoadPO( f ) )
#endif
			return false;
	}

    
	// Load Ragdoll
	char isRagdoll;
	*f >> isRagdoll;
	if( isRagdoll )
	{
		if( RagDoll ) SetRagdoll( false, 6 );
		RagDoll = true;
		ActivateAnimation(0); // ragdoll
#ifndef TRANSGAMING_NO_HAVOK
		if( !RagdollPtr->LoadRagdoll( f ) )	return false;
#else
		return false;
#endif
		ComputeFrameMatrix(FrameMatrixArr, ObjectToWorld);
	}
	else
	{
#ifndef TRANSGAMING_NO_HAVOK
		char hasRagdoll;
		*f >> hasRagdoll;
		if( !hasRagdoll )
		{
			SetRagdoll( false, 6 );
			if( RagdollPtr )
			{
				delete RagdollPtr;
				RagdollPtr = 0;
			}
		}
		else if( !RagdollPtr->LoadRagdollCollisionGroups( f ) )
			return false;
#endif
	}

	return true;
}

bool Model::SaveEntity( FileObject* f )
{
	// Data for World to recreate us
	*f << (int)ETypeModel << FileName << Name << Mesh->Scale  << (char) (Flags&EEntCenteredGeometry);

	if( !f->PutToken( MODEL_LOAD_SAVE_TOKEN ) ) return false;

	Entity::SaveEntity( f );

	// Data for us
	Vector pos = GetPosition();
	Quaternion rot = GetRotation();
	*f << pos << rot;

    // materials
    *f << BlendAlpha;
    *f << MaterialName;

    // shadow
    *f << ShadowEnabled;

	// save mesh visibility
    for (int i=0; i<Mesh->Meshes.Size(); i++)
        *f << MeshPropertiesArr[i].IsVisible;

	// Write animations state for entity
	char looped;
	DWORD hasMovementCurve;

	*f << Mesh->Animations.Size();
	for( int i = 1; i < Mesh->Animations.Size(); ++i )
		if( Mesh->Animations[i] )
		{
			*f << (char)1;
			looped = Mesh->Animations[i]->Looped;
			hasMovementCurve = ((SkeletalAnimation*)(Mesh->Animations[i]))->HasMovementCurve();
			*f << Mesh->Animations[i]->Name << looped << hasMovementCurve;
		}
		else
			*f << (char)0;

    
	// Synchronize animation controller
	*f << AnimController.Animations.Size();
	for( int i = 0; i < AnimController.Animations.Size(); ++i )
	{
		char index = -1;
		for( int j = 0; j < Mesh->Animations.Size(); ++j )
		{
			if( Mesh->Animations[j] == AnimController.Animations[i].Animation )
			{
				index = j;
				break;
			}
		}
		AnimationParams& a = AnimController.Animations[i];
        *f << index;
        *f << a;
		//*f << index << a.Weight << a.TimeScale << (char)a.Looped << a.FrameBlendTime << a.BlendTime;
	}
    

#ifndef TRANSGAMING_NO_HAVOK
	// Save PO
	char hasPO = ( PO != 0 );
	*f << hasPO;
	if( hasPO && !PO->SavePO( f ) )
		return false;
    
	// Save Ragdoll
	char isRagdoll = IsRagdoll();
	*f << isRagdoll;
	if( isRagdoll )
	{
		if( !RagdollPtr->SaveRagdoll( f ) )
			return false;
	}
	else
	{
		char hasRagdoll = (RagdollPtr != 0 );
		*f << hasRagdoll;
		if( hasRagdoll && !RagdollPtr->SaveRagdollCollisionGroups( f ) )
			return false;
	}

	return true;
#else
	return false;
#endif

}

#ifndef TRANSGAMING
bool Model::DemoReadEntity( FileObject *f )
{
	DWORD contents;
	*f >> contents;

	if( contents & EDemoWriteFlags )
		*f >> Flags;

	Vector pos = Translation;
	Quaternion rot = Rotation;
	if( contents & EDemoWriteTranslation )
		*f >> pos;
	if( contents & EDemoWriteRotation )
		*f >> rot;

	SetPositionAndRotation( pos, rot );

	if( contents & EDemoWriteScale )
	{
		float scale = 1.0f;
		*f >> scale;
		SetScale( scale );
	}

	if( contents & EDemoWriteDemonic )
	{
		char demonic;
		*f >> demonic;
		Demonic = (demonic!=0);
	}

	if( contents & EDemoWriteTimeToDelete )
		*f >> TimeToDelete;
	if( contents & EDemoWriteFadeTime )
		*f >> FadeTime;
	if( contents & EDemoWriteSort )
		*f >> Sort;

	char parent = ( Parent == NULL ) ? 0 : 1;
	if( contents & EDemoWriteParent )
		*f >> parent;

	if( parent )
	{
		unsigned int pind = 0;
		if( Parent )
			pind = Parent->GIndex;
		char pjoint = ParentJoint;
		char transByParent = ( char )TransformByParent;
		if( contents & EDemoWriteParentGIndex )
			*f >> pind;
		if( contents & EDemoWriteParentJoint )
			*f >> pjoint;
		if( contents & EDemoWriteTransformByParent )
			*f >> transByParent;

#ifdef	TRANSGAMING
		Entity *pe = GEngine->pcfWorld->EntityGIndexes[pind];
#else
		Entity *pe = GEngine->World->EntityGIndexes[pind];
#endif	
		if( pe && !Parent )
		{
			pe->RegisterChild( this, transByParent != 0, pjoint );
//			GLog.Print( "Entity (%d, %s) added to parent (%d, %s)\n", GIndex, Name.AsChar(), pe->GIndex, pe->Name );
		}
	}

	UpdateTransform(true);

    // load mesh visibility
    for (int i=0; i<Mesh->Meshes.Size(); i++)
	{
        *f >> MeshPropertiesArr[i].IsVisible;
		if( Flags & Entity::EEntNoDraw )
			MeshPropertiesArr[i].IsVisible = false;
	}

	int activeAnimSize;
	char anim;

	if( contents & EDemoWriteMeshAnims )
	{
		String animName;
		int animSize;
		char ok;
		char looped;
		DWORD setMovementCurve;
		int index;

		// Read animations state for entity and load animations
		*f >> animSize;
		for( int i = 1; i < animSize; ++i )
		{
			*f >> ok;
			if( ok )
			{
				*f >> animName >> looped >> setMovementCurve;
				index = LoadAnimation( animName.AsChar(), looped?true:false , i );
				if( setMovementCurve )
					SetAnimationMovementCurve( index, "ROOOT", setMovementCurve );
			}
		}
	}

	// Synchronize animation controller
	AnimController.Animations.ClearArray();
	*f >> activeAnimSize;
	for( int i = 0; i < activeAnimSize; ++i )
	{

		AnimController.Animations.GrowArray(1);
		AnimationParams& a = AnimController.Animations[i];

        *f >> anim;
        *f >> a;        

		//*f >> anim >> a.Weight >> a.TimeScale >> looped >> a.FrameBlendTime >> a.BlendTime;
        //a.Looped = looped?true:false;	// bool field might be 1-bit long32, and then what?

		a.Animation = Mesh->Animations[ anim ];        
	}

	char isRagdoll;
	*f >> isRagdoll;
	if( isRagdoll )
	{
		int numJoints;
		*f >> numJoints;
		for( int i = 0; i < numJoints; i++ )
		{
			if( i < Skeleton.Joints.Size() )
			{
				*f >> Skeleton.Joints[i].Transform;
			}
			else
			{
//				GLog.Print( "Wrong joint index: %d\n", i );
				f->Seek( 16 * sizeof( float ), SEEK_CUR );
			}
		}
	}

	char b;
	*f >> b;
	Env.Initiated = ( b != 0 );
	*f >> b;
	Env.OverwriteAmbient = ( b != 0 );

	*f >> Env.LightFadeTime;
	*f >> Env.LightProgress;
	*f >> Env.LightProgressDelta;

	*f >> Env.Ambient;
	*f >> Env.DirColor;

	return true;
}

bool Model::DemoCalcSize( int &size, DWORD &contents )
{
	contents = 0;
	size = sizeof( DWORD );

	if( Flags != mLastFlags )
	{
		size += sizeof( DWORD );
		contents |= EDemoWriteFlags;
	}

	if( Translation != mLastTranslation )
	{
		size += sizeof( Vector );
		contents |= EDemoWriteTranslation;
	}

	if( !( Rotation == mLastRotation ) )
	{
		size += sizeof( Quaternion );
		contents |= EDemoWriteRotation;
	}

	if( Scale != mLastScale )
	{
		size += sizeof( float );
		contents |= EDemoWriteScale;
	}

	if( Demonic != mLastDemonic )
	{
		size++;
		contents |= EDemoWriteDemonic;
	}

	if( TimeToDelete != mLastTimeToDelete )
	{
		size += sizeof( float );
		contents |= EDemoWriteTimeToDelete;
	}

	if( FadeTime != mLastFadeTime )
	{
		size += sizeof( float );
		contents |= EDemoWriteFadeTime;
	}

	if( Sort != mLastSort )
	{
		size += sizeof( int );
		contents |= EDemoWriteSort;
	}

	if( ( Parent != NULL ) != mLastParent )
	{
		size++;
		contents |= EDemoWriteParent;
	}

	if( Parent != NULL )
	{
		if( Parent->GIndex != mLastParentGIndex )
		{
			size += sizeof( unsigned int );
			contents |= EDemoWriteParentGIndex;
		}

		if( ParentJoint != mLastParentJoint )
		{
			size++;
			contents |= EDemoWriteParentJoint;
		}

		if( TransformByParent != mLastTransformByParent )
		{
			size++;
			contents |= EDemoWriteTransformByParent;
		}
	}

	int animSize = 0;
	for( int i = 1; i < Mesh->Animations.Size(); ++i )
		if( Mesh->Animations[i] )
			animSize++;

	if( animSize != mLastMeshAnimSize )
	{
		size += sizeof( int ); // Mesh->Animations.Size();
		for( int i = 1; i < Mesh->Animations.Size(); ++i )
			if( Mesh->Animations[i] )
			{
				size += sizeof( char ); // 1
				size += sizeof( char ); // looped
				size += sizeof( DWORD ); // hasMovementCurve
				size += sizeof( int ); // name len
				size += Mesh->Animations[i]->Name.Len() + sizeof( char );
			}
			else
				size += sizeof( char );
		contents |= EDemoWriteMeshAnims;
	}

	return true; // Model always writes more
}

bool Model::DemoWriteEntity( FileObject *f )
{	
	int size = 0;
	DWORD contents = 0;
	if( !DemoCalcSize( size, contents ) )
		return true;

	char parent = ( Parent == NULL ) ? 0 : 1;

	size += Mesh->Meshes.Size(); // visibility (char for each mesh)

	size += sizeof( int ); // AnimController.Animations.Size();
	for( int i = 0; i < AnimController.Animations.Size(); ++i )
	{
		size += sizeof( char );	// index
		size += 5 * sizeof( float ) + 2 * sizeof( char ); // a
	}

	char isRagdoll = IsRagdoll();
	size += sizeof( char ); // isRagdoll
	if( isRagdoll )
	{
		size += sizeof( int ); // joints.size
		for( int i = 0; i < Skeleton.Joints.Size(); i++ )
			size += 16 * sizeof( float ); // matrix :(
	}

	size += 38; // Env

	*f << size;
	*f << contents;

	if( contents & EDemoWriteFlags )
	{
		*f << Flags;
		mLastFlags = Flags;
	}
	if( contents & EDemoWriteTranslation )
	{
		*f << Translation;
		mLastTranslation = Translation;
	}
	if( contents & EDemoWriteRotation )
	{
		*f << Rotation;
		mLastRotation = Rotation;
	}
	if( contents & EDemoWriteScale )
	{
		*f << Scale;
		mLastScale = Scale;
	}

	char demonic = Demonic ? 1 : 0;
	if( contents & EDemoWriteDemonic )
	{
		*f << demonic;
		mLastDemonic = Demonic;
	}

	if( contents & EDemoWriteTimeToDelete )
	{
		*f << TimeToDelete;
		mLastTimeToDelete = TimeToDelete;
	}
	if( contents & EDemoWriteFadeTime )
	{
		*f << FadeTime;
		mLastFadeTime = FadeTime;
	}
	if( contents & EDemoWriteSort )
	{
		*f << Sort;
		mLastSort = Sort;
	}

	if( contents & EDemoWriteParent )
	{
		*f << parent;
		mLastParent = ( Parent != NULL );
	}
	if( parent )
	{
		if( contents & EDemoWriteParentGIndex )
		{
			*f << Parent->GIndex;
			mLastParentGIndex = Parent->GIndex;
		}
		if( contents & EDemoWriteParentJoint )
		{
			*f << ParentJoint;
			mLastParentJoint = ParentJoint;
		}
		if( contents & EDemoWriteTransformByParent )
		{
			*f << ( char )TransformByParent;
			mLastTransformByParent = TransformByParent;
		}
	}

	// save mesh visibility
    for (int i=0; i<Mesh->Meshes.Size(); i++)
        *f << MeshPropertiesArr[i].IsVisible;

	if( contents & EDemoWriteMeshAnims )
	{
		// Write animations state for entity
		char looped;
		DWORD hasMovementCurve;

		mLastMeshAnimSize = 0;

		*f << Mesh->Animations.Size();
		for( int i = 1; i < Mesh->Animations.Size(); ++i )
			if( Mesh->Animations[i] )
			{
				*f << (char)1;
				looped = Mesh->Animations[i]->Looped;
				hasMovementCurve = ((SkeletalAnimation*)(Mesh->Animations[i]))->HasMovementCurve();
	/*			*f << Mesh->Animations[i]->Name.Len();
				for( int cc = 0; cc < Mesh->Animations[i]->Name.Len(); cc++ )
					*f << Mesh->Animations[i]->Name[cc];*/
				*f << Mesh->Animations[i]->Name << looped << hasMovementCurve;
				mLastMeshAnimSize++;
			}
			else
				*f << (char)0;
	}

	// Synchronize animation controller
	*f << AnimController.Animations.Size();
	for( int i = 0; i < AnimController.Animations.Size(); ++i )
	{
		char index = -1;
		for( int j = 0; j < Mesh->Animations.Size(); ++j )
		{
			if( Mesh->Animations[j] == AnimController.Animations[i].Animation )
			{
				index = j;
				break;
			}
		}
		AnimationParams& a = AnimController.Animations[i];
        *f << index;
        *f << a;
		//*f << index << a.Weight << a.TimeScale << (char)a.Looped << a.FrameBlendTime << a.BlendTime;
	}

	*f << isRagdoll;
	if( isRagdoll )
	{
		*f << Skeleton.Joints.Size();
		for( int i = 0; i < Skeleton.Joints.Size(); i++ )
		{
			*f << Skeleton.Joints[i].Transform;
		}
	}

	*f << ( char )Env.Initiated;
	*f << ( char )Env.OverwriteAmbient;
	*f << Env.LightFadeTime;
	*f << Env.LightProgress;
	*f << Env.LightProgressDelta;
	*f << Env.Ambient;
	*f << Env.DirColor;

//	int size2 = f->Tell() - 4;
//	if( ( size2 - size1 ) != size )
//	{
//		GLog.Print( "WARNING (Model): Wrong skipping size %d, should be %d\n", size, size2 - size1 );
//	}

	return true;
}

bool Model::DemoReadFull( FileObject *f )
{
	Entity::DemoReadFull( f );

	// materials
    *f >> BlendAlpha;
    *f >> MaterialName;
    
    if (MaterialName.Size())
#ifdef TRANSGAMING
        SetMaterial(MaterialName.AsChar());
#else
        SetMaterial(MaterialName);
#endif

    // shadow
    *f >> ShadowEnabled;
    if (ShadowEnabled)
        CreateShadowMap(true);

	return true;
}

bool Model::DemoWriteFull( FileObject *f )
{
	*f << (char)FileName.Len();
	f->Write( FileName.AsChar(), FileName.Len() );
	*f << (char)Name.Len();
	f->Write( Name.AsChar(), Name.Len() );
	*f << Mesh->Scale;
	*f << Flags;
	*f << TimeToDelete << FadeTime;
	*f << Sort;

	// materials
    *f << BlendAlpha;
    *f << MaterialName;

    // shadow
    *f << ShadowEnabled;

	mLastFlags = 0xFFFFFFFF;
	mLastTranslation = Vector( 65535.0f, 65535.0f, 65535.0f );
	mLastRotation = Quaternion( 65535.0f, 65535.0f, 65535.0f, 65535.0f );
	mLastScale = 65535.0f;
	mLastDemonic = !Demonic;
	mLastTimeToDelete = 65535.0f;
	mLastFadeTime = 65535.0f;
	mLastSort = Sort - 1;
	mLastParent = ( Parent == NULL );
	mLastParentGIndex = 65535;
	mLastParentJoint = -127;
	mLastTransformByParent = !TransformByParent;
	mLastMeshAnimSize = 0;

	return true;
}
#endif

void Model::CreateRagdollIfNone( void )
{
#ifndef TRANSGAMING_NO_HAVOK
	if( RagdollPtr ) return;

	String modelsDir = String("../Data/Models/")+Mesh->Name+".hke";

    m_setTag("Mesh");
	Ragdoll* r = new Ragdoll;
	bool hasRagDoll = r->Init( this, &Skeleton, modelsDir.AsChar(), Mesh->Scale );
	if( hasRagDoll )
	{
		ProcessRagdoll();
		RagdollPtr = r;
	}
	else
	{
		delete r;
		RagdollPtr = 0;
	}
#else
	RagdollPtr = 0;
#endif
}
