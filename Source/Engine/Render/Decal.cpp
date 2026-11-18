
#include "Engine.h"
#include "Decal.h"
#include "Render/View.h"
#include "Engine/Mesh.h"
#include "Render/DecalSystem.h"
#include "Render/Video.h"
#include "Engine/World.h"
#include "Render/Material.h"

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

Decal::Decal(DecalEffect *effect)
:	Entity(Entity::ETypeDecal), TexturePtr(0), ConstColor(0xFFFFFFFF), Effect(effect), TimeToDie(Effect->LifeTime), ScaleFactor(1)
{
	SpawnedOnId = EInvalidIndex;
    NumElems = 0;
	NeverDie = TimeToDie<=0;
	Sort = Entity::ESortDecalDyn;
	LocalBox.Reset();
	LocalBox.Expand(effect->Scale);
	Box = LocalBox.Transform(ObjectToWorld);
	SpawnedOn = NULL;
}

void Decal::SetScale(float scale)
{
	ScaleFactor = scale;
}

Decal::~Decal(void)
{
}

bool Decal::Spawn(Entity* e, const Vector& pos, const Vector& dir)
{
#ifdef TRANSGAMING_HACK
    return false;
#endif
	Matrix m;

	Vector N = -dir;
	if (Effect->ZScale>0)
		N *= Effect->ZScale;

	Vector T = N.MinimalAxis()^N;
	T *= ScaleFactor*Effect->Scale/T.Magnitude();
	Vector B = N^T;
	B *= ScaleFactor*Effect->Scale/B.Magnitude();

	m.X = Plane(T,0);
	m.Y = Plane(B,0);
	m.Z = Plane(N,0);
	m.W = Plane(pos,1);

	if (!NeverDie)
	{
		float angle = FloatRand()*F_TWOPI;
		m = RotationMatrix(angle, 0,0,1)*m;
	}

	return Spawn(e, m);
}

bool Decal::Spawn(Entity* e, const Vector& pos, const Vector& nor, const Vector& tan, const Vector& bin)
{
#ifdef TRANSGAMING_HACK
    return false;
#endif
	Matrix m;

	Vector N = -nor;
	if (Effect->ZScale>0)
		N *= Effect->ZScale;

	Vector T = tan*ScaleFactor*Effect->Scale;
	Vector B = bin*ScaleFactor*Effect->Scale;

	m.X = Plane(T,0);
	m.Y = Plane(B,0);
	m.Z = Plane(N,0);
	m.W = Plane(pos,1);

	return Spawn(e, m);
}

bool Decal::Spawn(Entity* e, const Matrix& projMat)
{
#ifdef TRANSGAMING_HACK
    return false;
#endif
	int i=0;
	
	if (!e->AsMesh() || e->Sort==Entity::ESortDecal)
		return false;

	SpawnMatrix = projMat;
	SpawnedOn = e;
	NumElems = 0;

	if (!Parent)
		e->RegisterChild(this);
	else
		return true;

	Matrix m = e->ObjectToWorld*projMat.Inverse();
	m.Transpose();
	Matrix mt = projMat*e->ObjectToWorld.Inverse();
	Vector mtN = mt.Z.Normal.GetNormal();

    //TRACE("Decal::Spawn(): spawning a decal on entity '%s'\n", e->Name.AsChar());
    //this condition should always be true since we check e->AsMesh() above
#ifdef TRANSGAMING
    Assert(e->Type==Entity::ETypeMesh);
#else
	if (e->Type==Entity::ETypeMesh){
#endif
		const int maxNumPlanes=6;
		Plane clipPlanes[maxNumPlanes];
		int numPlanes = 4;


		// Left clipping plane
		clipPlanes[2] = m.TransformPlane(Plane(1,0,0,-0.5f)); 

		// Right clipping plane
		clipPlanes[3] = m.TransformPlane(Plane(-1,0,0,-0.5f));

		// Top clipping plane
		clipPlanes[0] = m.TransformPlane(Plane(0,1,0,-0.5f));

		// Bottom clipping plane
		clipPlanes[1] = m.TransformPlane(Plane(0,-1,0,-0.5f));

		if (Effect->ZScale > 0)
		{
			numPlanes = 6;
			// Near clipping plane
			clipPlanes[4] = m.TransformPlane(Plane(0,0,1,-0.5f));

			// Far clipping plane
			clipPlanes[5] = m.TransformPlane(Plane(0,0,-1,-0.5f));
		}

		WorldMesh*  mesh = e->AsMesh();
		DynamicArray<WORD> &tris = mesh->Data->Triangles;
		//DynamicArray<MeshVertex> &verts = mesh->Verts;

		if (!Effect->CutTris) {
			for(int iTri=0; iTri<tris.Size(); iTri+=3)
			{
				Vector p0 = mesh->Data->VertList.getVertPos(tris[iTri+0]);
				Vector p1 = mesh->Data->VertList.getVertPos(tris[iTri+1]);
				Vector p2 = mesh->Data->VertList.getVertPos(tris[iTri+2]);

				if (Effect->CullBackFaces>-1)
				{
					Vector N = (p1-p0)^(p2-p0);
					if (mtN.Dot(N.GetNormal())<Effect->CullBackFaces)
						continue;
				}

				if (((clipPlanes[0].Distance(p0)<0) && (clipPlanes[0].Distance(p1)<0) && (clipPlanes[0].Distance(p2)<0))
				||  ((clipPlanes[1].Distance(p0)<0) && (clipPlanes[1].Distance(p1)<0) && (clipPlanes[1].Distance(p2)<0))
				||  ((clipPlanes[2].Distance(p0)<0) && (clipPlanes[2].Distance(p1)<0) && (clipPlanes[2].Distance(p2)<0))
				||  ((clipPlanes[3].Distance(p0)<0) && (clipPlanes[3].Distance(p1)<0) && (clipPlanes[3].Distance(p2)<0)))
					continue;

				if (Effect->ZScale > 0)
					if (((clipPlanes[4].Distance(p0)<0) && (clipPlanes[4].Distance(p1)<0) && (clipPlanes[4].Distance(p2)<0))
					||  ((clipPlanes[5].Distance(p0)<0) && (clipPlanes[5].Distance(p1)<0) && (clipPlanes[5].Distance(p2)<0)))
						continue;

				VertexArray[NumElems++] = SimpleVertex(p0, 0xFFFFFFFF, 
					TexCoord(clipPlanes[0].Distance(p0),clipPlanes[2].Distance(p0)));

				VertexArray[NumElems++] = SimpleVertex(p1, 0xFFFFFFFF, 
					TexCoord(clipPlanes[0].Distance(p1),clipPlanes[2].Distance(p1)));

				VertexArray[NumElems++] = SimpleVertex(p2, 0xFFFFFFFF, 
					TexCoord(clipPlanes[0].Distance(p2),clipPlanes[2].Distance(p2)));



                if (NumElems>MAX_ELEMS-4){
                    TRACE("Decal::Spawn(): vertex buffer overflowed with %d verticies!\n", NumElems);
					break;
                }
			}
		}
		else {
			for(int iTri=0; iTri<tris.Size(); iTri+=3)
			{
				Vector p[3];
				p[0] = mesh->Data->VertList.getVertPos(tris[iTri]);
				p[1] = mesh->Data->VertList.getVertPos(tris[iTri+1]);
				p[2] = mesh->Data->VertList.getVertPos(tris[iTri+2]);

				if (Effect->CullBackFaces>-1)
				{
					Vector N = (p[1]-p[0])^(p[2]-p[0]);
					if (mtN.Dot(N.GetNormal())<Effect->CullBackFaces)
						continue;
				}

				
				for ( i=0; i<numPlanes; i++)
				{
					if (clipPlanes[i].Distance(p[0])<0 &&
						clipPlanes[i].Distance(p[1])<0 &&
						clipPlanes[i].Distance(p[2])<0)
						break;
				}

				if (i<numPlanes)
					continue;
			
				ConvexPolygon convex;
				convex += p[0];
				convex += p[1];
				convex += p[2];

				for ( i=0; i<numPlanes; i++)
				{
					ConvexPolygon keep, discard;
					convex.Split(clipPlanes[i], keep, discard);
					convex = keep;
				}

				for ( i=2; i<convex.VertexCount; i++)
				{
					const Vector &v0 = convex.Verts[0];
					const Vector &v1 = convex.Verts[i-1];
					const Vector &v2 = convex.Verts[i];

					VertexArray[NumElems++] = (SimpleVertex(v0, 0xFFFFFFFF, 
						TexCoord(clipPlanes[0].Distance(v0),clipPlanes[2].Distance(v0))));

					VertexArray[NumElems++] = (SimpleVertex(v1, 0xFFFFFFFF, 
						TexCoord(clipPlanes[0].Distance(v1),clipPlanes[2].Distance(v1))));

					VertexArray[NumElems++] = (SimpleVertex(v2, 0xFFFFFFFF, 
						TexCoord(clipPlanes[0].Distance(v2),clipPlanes[2].Distance(v2))));

					if (NumElems>MAX_ELEMS-4)
						break;
				}

				if (NumElems>MAX_ELEMS-4)
					break;
			}
		}
#ifndef TRANSGAMING
	}	
#endif

	LocalBox.Reset();
	for (int i=0; i<NumElems; i++)
	{
		LocalBox += VertexArray[i].Pos;
	}

    //TRACE("Decal::Spawn(): spawned the decal '%s' on the object '%s' with %d verticies (e has %d triangles)\n", Name.AsChar(), e->Name.AsChar(), NumElems, tris.Size());

	if (NumElems==0)
		LocalBox += Vector(0,0,0);

	UpdateTransform();

	return NumElems>0;
}

void Decal::SetState(RenderDevice* dev)
{
	TextureState st[2];
	st[0].Set(0,TextureState::ETexOpModulate,TextureState::ETexArgColor,TextureState::ETexFilterBilinear);
	st[0].AlphaOp = TextureState::ETexOpModulate;
	st[0].AlphaArg = TextureState::ETexArgColor;
	st[0].AddrMode = TextureState::ETexAddrClamp;
	st[0].Filter = TextureState::ETexFilterBilinearNoMips;

	st[1].Set(1,TextureState::ETexOpDisable);

	dev->SetTextureState(st,ARRAY_COUNT(st));

	RenderState state;
	state.SetDepthWrite(false);
	state.SetConstColor(ConstColor);
	state.SetBlendMode(Effect->BlendMode);

	dev->SetState(state);
	dev->SetStream(GEngine->Video->DecalSys.VBuf.GetStream());
	dev->SetShader(GEngine->Video->MaterialSys->Shaders.FixedSimple,RenderDevice::EShaderVertex);
	dev->SetShader(EInvalidIndex,RenderDevice::EShaderPixel);
}

void Decal::Draw(RenderDevice* dev, int mode)
{
#ifdef TRANSGAMING_HACK
    // eric-> skip drawing the decals since they seem to cause problems with vertex array counts.
    //   Fix this later.  It seems to be a problem in an unsupported vertex format.
    return;
#endif

	if ((mode & View::EModeMultiPass) && (NumElems > 0))
	{
		dev->SetObjectTransform(&ObjectToWorld);

		SimpleVertex* v=GEngine->Video->DecalSys.VBuf.Lock(NumElems);
		Assert(v);
		if (!v) return;

		for (int i=0; i<NumElems; i++)
		{
			v[i]=VertexArray[i];
		}

		Texture *ptr = TexturePtr ? TexturePtr : Effect->TexturePtr;

		int id = ptr->DeviceId;

		if (Effect->FPS>0)
		{
			int f = int((Effect->LifeTime-TimeToDie)*Effect->FPS);
			if (f>=ptr->NumFrames)
				f = ptr->NumFrames-1;
			id = ptr->FrameIds[f];
		}

		dev->SetTexture(0,id);

#ifdef	TRANSGAMING
		dev->SetFogState(GEngine->pcfWorld->Env.FogParams); // ugly hack: switch to fp'ed fog
#else
		dev->SetFogState(GEngine->World->Env.FogParams); // ugly hack: switch to fp'ed fog
#endif

		SetState(dev);

		dev->Draw(RenderDevice::EPrimTriList,GEngine->Video->DecalSys.VBuf.Unlock(),NumElems/3);

#ifdef	TRANSGAMING
		dev->SetFogState(GEngine->pcfWorld->Env.FogParams.UseVSFog());
#else
		dev->SetFogState(GEngine->World->Env.FogParams.UseVSFog());
#endif
	}
}

void Decal::Tick(const float timeDelta)
{
#ifdef TRANSGAMING_HACK
    return;
#endif
	if (NeverDie)
		return;

	if (Effect->FPS>0)
		TimeToDie -= timeDelta; // animated texture!
#ifdef TRANSGAMING
	else
		TimeToDie -= timeDelta*GEngine->Video->DecalSys.DecalsStay;
#else
	else if (!GEngine->Video->DecalSys.DecalsAlwaysStay)
		TimeToDie -= timeDelta*GEngine->Video->DecalSys.DecalsStay;
#endif


	if (TimeToDie<0)
	{
		ConstColor = 0;
#ifdef	TRANSGAMING
		GEngine->pcfWorld->DeleteEntityDelayed(this);
#else
		GEngine->World->DeleteEntityDelayed(this);
#endif
	}
	else if (TimeToDie<Effect->DecayTime)
	{
		DWORD alpha = DWORD(0xFF*TimeToDie/Effect->DecayTime);

		ConstColor = (alpha<<24) + (alpha<<16) + (alpha<<8) + alpha;
	}
}

void Decal::SetTexture(const char *texName)
{
	TextureName = texName;

	if (TexturePtr)
	{
		GEngine->Video->MaterialSys->ReleaseTexture(TexturePtr);
	}


    char buf[MAX_PATH];


    snprintf(buf, MAX_PATH, "Decals/%s", texName);

	TexturePtr = GEngine->Video->MaterialSys->CreateTexture(buf);
}

#define DECAL_LOAD_SAVE_TOKEN 0x5e80

bool Decal::SaveEntity( FileObject* f )
{
// Data for World to recreate us
	*f << (int)ETypeDecal << Effect->Name << Name << Scale << (char) (0);

	if( !f->PutToken( DECAL_LOAD_SAVE_TOKEN ) ) return false;

	Entity::SaveEntity( f );

	*f << TextureName;
	*f << ScaleFactor << ConstColor << TimeToDie << NeverDie << SpawnMatrix << NumElems;

	for (int i=0; i<NumElems; i++)
	{
		*f << VertexArray[i].Pos;
		*f << VertexArray[i].Color;
		*f << VertexArray[i].TU;
	}

	return true;
}

bool Decal::LoadEntity( FileObject* f )
{
	if( !f->TestToken( DECAL_LOAD_SAVE_TOKEN ) )
	{
		GLog.Print( "Token test failure: Decal::LoadEntity()\n" );
		return false;
	}

	Entity::LoadEntity( f );

	String texName;
	*f >> texName;
	if (texName.Len())
		SetTexture(texName.AsChar());

	*f >> ScaleFactor >> ConstColor >> TimeToDie >> NeverDie >> SpawnMatrix >> NumElems;

	LocalBox.Reset();
	for (int i=0; i<NumElems; i++)
	{
		*f >> VertexArray[i].Pos;
		*f >> VertexArray[i].Color;
		*f >> VertexArray[i].TU;
		LocalBox += VertexArray[i].Pos;
	}

	if (NumElems==0)
		LocalBox += Vector(0,0,0);

	return true;
}

#ifndef TRANSGAMING
bool Decal::DemoReadEntity( FileObject *f )
{
	Entity::DemoReadEntity( f );
	return true;
}


bool Decal::DemoWriteEntity( FileObject *f )
{
	Entity::DemoWriteEntity( f );
	return true;
}

bool Decal::DemoReadFull( FileObject *f )
{
	Entity::DemoReadFull( f );

	String texName;
	*f >> texName;
	if (texName.Len())
		SetTexture(texName.AsChar());

	*f >> ScaleFactor >> ConstColor >> TimeToDie >> NeverDie;

	*f >> SpawnMatrix;

	*f >> NumElems;

	LocalBox.Reset();
	for (int i=0; i<NumElems; i++)
	{
		*f >> VertexArray[i].Pos;
		*f >> VertexArray[i].Color;
		*f >> VertexArray[i].TU;
		LocalBox += VertexArray[i].Pos;
	}

	if (NumElems==0)
		LocalBox += Vector(0,0,0);

	UpdateTransform();

	return true;
}


bool Decal::DemoWriteFull( FileObject *f )
{
	*f << (char)Effect->Name.Len();
	f->Write( Effect->Name.AsChar(), Effect->Name.Len() );
	*f << (char)Name.Len();
	f->Write( Name.AsChar(), Name.Len() );
	*f << Scale;
	*f << Flags;
	*f << TimeToDelete << FadeTime;
	*f << Sort;

//	Entity::DemoWriteFull( f );

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

	*f << TextureName;
	*f << ScaleFactor << ConstColor << TimeToDie << NeverDie;
	*f << SpawnMatrix;
	*f << NumElems;
	for (int i=0; i<NumElems; i++)
	{
		*f << VertexArray[i].Pos;
		*f << VertexArray[i].Color;
		*f << VertexArray[i].TU;
	}

	return true;
}
#endif

#ifdef PKTV_ENABLED
char *Decal::GetEffectName() const
{
	return ( char *)Effect->Name.AsChar();
}
#endif
