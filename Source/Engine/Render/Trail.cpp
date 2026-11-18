#include "Engine.h"
#include "Render/TrailSystem.h"
#include "Render/Trail.h"
#include "Render/View.h"
#include "Render/Video.h"
#include "Render/Material.h"
#include "Engine/Model.h"
#include "Engine/World.h"
#ifdef TRANSGAMING
# include "Render/ParticleSystem.h"
#endif

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

/* min and max macros */
#ifndef NOMINMAX
#ifndef max
#define max(a,b)   (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b)   (((a) < (b)) ? (a) : (b))
#endif
#endif  /* NOMINMAX */


#ifdef TRANSGAMING
const float Trail::ADDNODE_PERIOD_SCALE = 0.25f;
#endif


typedef struct  {
	Vector Position;
	Vector Tangent;
} TrailNode;

Trail::Trail(TrailEffect *effect)
:	Entity(Entity::ETypeTrail), TexturePtr(0), ConstColor(0xFFFFFFFF), Effect(effect), Time(0)
,	Positions(0), Times(0),ScaleFactor(1)
{
	MAX_ELEMS = 1;
	BonesNum = 1;
	PositionChanged=false;

	for (int i=0; i<MAX_BONES; ++i) {
		LastPosition[i].Set(0,0,0);
		LastNodePosition[i].Set(0,0,0);
	}

	HeadNode = 0;
	TailNode = 0;
	Sort = Entity::ESortTrans;
	LocalBox.Reset();
	LocalBox.Expand(effect->Width*ScaleFactor);
	Box = LocalBox.Transform(ObjectToWorld);

#ifdef TRANSGAMING
    currentNodeTime = 0;
#endif
}

Trail::~Trail()
{
	Free();
}


void Trail::SetScale(float scale)
{
	ScaleFactor = scale;
}

void Trail::SetState(RenderDevice* dev)
{
	TextureState st[2];
	st[0].Set(0,TextureState::ETexOpTexture,TextureState::ETexArgColor,TextureState::ETexFilterBilinear);
	st[0].AlphaOp = TextureState::ETexOpTexture;
	st[0].AddrMode = TextureState::ETexAddrClamp;

	st[1].Set(1,TextureState::ETexOpDisable);

	dev->SetTextureState(st,ARRAY_COUNT(st));

	RenderState state;
	state.SetDepthWrite(false);
	state.SetConstColor(ConstColor);
	state.SetBlendMode(Effect->BlendMode);
	state.SetCullMode(RenderState::ECullNone);
	//state.SetFillMode(RenderState::EFillLines);

	dev->SetState(state);
#ifdef TRANSGAMING_PS2
    ParticleSystem &partSys = GEngine->Video->ParticleSys;
    int             s[] = { partSys.PBuf.GetStream(), partSys.TBuf.GetStream() };


    dev->SetStreams(s, ARRAY_COUNT(s), EInvalidIndex);
    dev->SetShader(GEngine->Video->MaterialSys->Shaders.Fixed_DTU1, RenderDevice::EShaderVertex);
    dev->SetShader(EInvalidIndex, RenderDevice::EShaderPixel);
#elif defined(TRANSGAMING_PSP)
    dev->SetStream(GEngine->Video->ParticleSys.VBuf.GetStream());
	dev->SetShader(GEngine->Video->MaterialSys->Shaders.FixedSimple,RenderDevice::EShaderVertex);
	dev->SetShader(EInvalidIndex,RenderDevice::EShaderPixel);
#else
    dev->SetStream(GEngine->Video->TrailSys.VBuf.GetStream());
	dev->SetShader(GEngine->Video->MaterialSys->Shaders.FixedSimple,RenderDevice::EShaderVertex);
	dev->SetShader(EInvalidIndex,RenderDevice::EShaderPixel);
#endif
}

void Trail::Draw(RenderDevice* dev, int mode)
{
	if (!Positions)
		return;
	if (mode & View::EModeMultiPass)
	{
		dev->SetObjectTransform(&ObjectToWorld);
		int NumNodes = ((HeadNode-TailNode+MAX_ELEMS)%MAX_ELEMS);
		if (NumNodes<2) return;

#ifdef TRANSGAMING_MESH_WARNINGS
        TRACE("Trail::Draw(): rendering the trail '%s' with %d nodes\n", Name.AsChar(), NumNodes);
#endif
		Texture *ptr = TexturePtr ? TexturePtr : Effect->TexturePtr;

		if (Effect->FPS>0) {
			dev->SetTexture(0,ptr->FrameIds[int(Effect->LifeTime*Effect->FPS) % ptr->NumFrames]);
		}
		else {
			dev->SetTexture(0,ptr->DeviceId);
		}
		
#ifdef	TRANSGAMING
		dev->SetFogState(GEngine->pcfWorld->Env.FogParams); // ugly hack: switch to fp'ed fog
#else
		dev->SetFogState(GEngine->World->Env.FogParams); // ugly hack: switch to fp'ed fog
#endif
		SetState(dev);

		if (BonesNum<2)
		{
#ifdef TRANSGAMING_PS2
            ParticleSystem &partSys = GEngine->Video->ParticleSys;
            Vector *        v = partSys.PBuf.Lock(2 * NumNodes);
            TexCoord *      t = partSys.TBuf.Lock(2 * NumNodes);
            int             offset;

            Assert(v && t);
            if (v == NULL || t == NULL)
                return;
#elif defined(TRANSGAMING_PSP)
			SimpleVertex* v=GEngine->Video->ParticleSys.VBuf.Lock(2 * NumNodes);
			Assert(v);
			if (v == NULL)
                return;
#else
			SimpleVertex* v=GEngine->Video->TrailSys.VBuf.Lock(2*NumNodes);
			Assert(v);
			if (!v) return;
#endif

			TrailNode *TrailNodes = (TrailNode *)Positions;

			// calculate camera forward vector (this is crap, should get it from cameraToWorld.Z?)
			View* view = GEngine->Video->GetActiveView();
			Vector camPos = view->mCamera.GetPosition();

			int Elem=0;
			for (int i=TailNode; i!=HeadNode; i=(i+1)%MAX_ELEMS)
			{
				Vector Binormal = (TrailNodes[i].Position-camPos)^TrailNodes[i].Tangent;
				float BinormalLen = Binormal.Magnitude();
                if (BinormalLen>0)
                    Binormal *= 0.5f*Effect->Width*ScaleFactor/BinormalLen;
                else
                    Binormal.Set(0,0,0);

#ifdef TRANSGAMING_PS2
                v[0] = TrailNodes[i].Position + Binormal;
                v[1] = TrailNodes[i].Position - Binormal;
                t[0].U = t[1].U = (Time - Times[i]) / Effect->LifeTime;
                t[0].V = 0;
                t[1].V = 1;
                
                v += 2;
                t += 2;
#else
				v[Elem++] = SimpleVertex(TrailNodes[i].Position+Binormal,0xFFFFFFFF,TexCoord((Time-Times[i])/Effect->LifeTime,0));
				v[Elem++] = SimpleVertex(TrailNodes[i].Position-Binormal,0xFFFFFFFF,TexCoord((Time-Times[i])/Effect->LifeTime,1));
#endif
			}

#ifdef TRANSGAMING_PS2
            offset = partSys.PBuf.Unlock();

# ifdef DEBUG
            Assert(partSys.TBuf.Unlock() == offset);
# else
            partSys.TBuf.Unlock();
# endif
            partSys.CBuf.Resync(partSys.PBuf);
            dev->Draw(RenderDevice::EPrimTriStrip, offset, 2 * NumNodes - 2);
#elif defined(TRANSGAMING_PSP)
			dev->Draw(RenderDevice::EPrimTriStrip,GEngine->Video->ParticleSys.VBuf.Unlock(),2*NumNodes-2);
#else
			dev->Draw(RenderDevice::EPrimTriStrip,GEngine->Video->TrailSys.VBuf.Unlock(),2*NumNodes-2);
#endif
		}
		else
		{
#ifdef TRANSGAMING_PS2
            ParticleSystem &partSys = GEngine->Video->ParticleSys;
            int             offset;
#endif
            
            for (int b=0; b<BonesNum-1; b++)
			{
#ifdef TRANSGAMING_PS2
                Vector *    v = partSys.PBuf.Lock(2 * NumNodes);
                TexCoord *  t = partSys.TBuf.Lock(2 * NumNodes);

                Assert(v && t);
                if (v == NULL || t == NULL)
                    return;
#elif defined(TRANSGAMING_PSP)
                SimpleVertex * v = GEngine->Video->ParticleSys.VBuf.Lock(2 * NumNodes);
				Assert(v);
				if (v == NULL)
                    return;
#else
                SimpleVertex* v=GEngine->Video->TrailSys.VBuf.Lock(2*NumNodes);
				Assert(v);
				if (!v) return;
#endif

				float tu0=float(b)/(BonesNum-1);
				float tu1=float(b+1)/(BonesNum-1);

				int Elem=0;
				for (int i=TailNode; i!=HeadNode; i=(i+1)%MAX_ELEMS)
				{
#ifdef TRANSGAMING_PS2
                    memcpy(v, &Positions[b + i * BonesNum], sizeof(Vector) * 2);
                    t[0].U = t[1].U = (Time - Times[i]) / Effect->LifeTime;
                    t[0].V = tu0;
                    t[1].V = tu1;

                    v += 2;
                    t += 2;
#else
					v[Elem++] = SimpleVertex(Positions[b+i*BonesNum],  0xFFFFFFFF,TexCoord((Time-Times[i])/Effect->LifeTime,tu0));
					v[Elem++] = SimpleVertex(Positions[b+1+i*BonesNum],0xFFFFFFFF,TexCoord((Time-Times[i])/Effect->LifeTime,tu1));
#endif
				}

#ifdef TRANSGAMING_PS2
                offset = partSys.PBuf.Unlock();
# ifdef DEBUG
                Assert(partSys.TBuf.Unlock() == offset);
# else
                partSys.TBuf.Unlock();
# endif
                partSys.CBuf.Resync(partSys.PBuf);
                dev->Draw(RenderDevice::EPrimTriStrip, offset, 2 * NumNodes - 2);
#elif defined(TRANSGAMING_PSP)
				dev->Draw(RenderDevice::EPrimTriStrip,GEngine->Video->ParticleSys.VBuf.Unlock(),2*NumNodes-2);
#else
				dev->Draw(RenderDevice::EPrimTriStrip,GEngine->Video->TrailSys.VBuf.Unlock(),2*NumNodes-2);
#endif
            }
		}

#ifdef	TRANSGAMING
		dev->SetFogState(GEngine->pcfWorld->Env.FogParams.UseVSFog());
#else
		dev->SetFogState(GEngine->World->Env.FogParams.UseVSFog());
#endif
	}
}

void Trail::Tick(const float timeDelta)
{
	if (!Effect->ConstLen)
		Time += timeDelta;
	
#ifdef TRANSGAMING
    //prevent a node check from being performed every frame.  Simulate a constant 
    //framerate by controlling how often it is called.  The calling frequency is
    //dependant on the lifetime of the trail (although Effect->LifeTime isn't 
    //always expressed as time in seconds unfortunately)
    currentNodeTime += timeDelta;

    if (currentNodeTime > ADDNODE_PERIOD_SCALE * Effect->LifeTime){
        currentNodeTime = 0.0f;

        AddNode();
    }
#else
	AddNode();
#endif
    CutTail();
}

void Trail::UpdateTransform(bool updateInWorld)
{
	if (Parent && TransformByParent)
	{
		if (BonesNum==0) {
            if (Translation.X != 0.0f || Translation.Y != 0.0f || Translation.Z != 0.0f)
                LastPosition[0] = Parent->ObjectToWorld.GetTranslation() + Parent->ObjectToWorld.GetRotation().Transform(Translation);
            else
                LastPosition[0] = Parent->ObjectToWorld.GetTranslation();
		}	
		else {
            if (BonesNum == 1)
            {
                if (Translation.X != 0.0f || Translation.Y != 0.0f || Translation.Z != 0.0f)
                {
#ifdef TRANSGAMING
                    const Matrix &m = Parent->AsModel()->GetJointTransform(TrailBones[0]);
                    LastPosition[0] = m.GetTranslation() + m.GetRotation().Transform(Translation);
#else
                    Matrix * m = &Parent->AsModel()->GetJointTransform(TrailBones[0]);
                    LastPosition[0] = m->GetTranslation() + m->GetRotation().Transform(Translation);
#endif
                }
                else
                    LastPosition[0] = Parent->AsModel()->GetJointTransform(TrailBones[0]).GetTranslation();
            }
            else
                for (int i=0; i<BonesNum; i++)
                    LastPosition[i] = Parent->AsModel()->GetJointTransform(TrailBones[i]).GetTranslation();
		}
	}

	if (updateInWorld && WorldLink)
		WorldLink->UpdateEntity(this);
}

void Trail::SetTexture(const char *texName)
{
	TextureName = texName;

	if (TexturePtr)
	{
		GEngine->Video->MaterialSys->ReleaseTexture(TexturePtr);
	}


    char buf[MAX_PATH];


    snprintf(buf, MAX_PATH, "Trails/%s", texName);
	TexturePtr = GEngine->Video->MaterialSys->CreateTexture(buf);
}

void Trail::AddNode()
{
	if (!Positions) return;
	
	if (BonesNum<2)
	{
#ifdef TRANSGAMING_MESH_WARNINGS
        TRACE("Trail::AddNode(): adding a trail node to '%s'\n", Name.AsChar());
#endif
		int NewHeadNode;
		TrailNode *TrailNodes = (TrailNode *)Positions;
	    
		if ((LastNodePosition[0]-LastPosition[0]).Magnitude()<0.5f*Effect->Width*ScaleFactor)
		{
			return;
#ifndef TRANSGAMING
            NewHeadNode = HeadNode;
			HeadNode = (HeadNode+MAX_ELEMS-1)%MAX_ELEMS;
#endif
		}
		else
		{
			if (Effect->ConstLen)
				Time += (LastNodePosition[0]-LastPosition[0]).Magnitude();
			LastNodePosition[0] = LastPosition[0];
			NewHeadNode = (HeadNode+1)%MAX_ELEMS;
		}

		TrailNodes[HeadNode].Position = LastPosition[0];
		
		if (HeadNode!=TailNode)
		{
			int pidx=(MAX_ELEMS+HeadNode-1)%MAX_ELEMS;

			const Vector& ppos=TrailNodes[pidx].Position;

			TrailNodes[HeadNode].Tangent = (LastPosition[0]-ppos);

			if (TrailNodes[HeadNode].Tangent.Magnitude()<F_EPS)
				TrailNodes[HeadNode].Tangent=TrailNodes[pidx].Tangent;
		}
		else
			TrailNodes[HeadNode].Tangent = Vector(0,0,0);

		TrailNodes[HeadNode].Tangent.Normalize();

		Times[HeadNode] = Time;
		HeadNode = NewHeadNode;

		if (HeadNode == TailNode) 
			TailNode = (TailNode+1)%MAX_ELEMS;

		LocalBox += LastPosition[0];
	}
	else
	{
#ifdef TRANSGAMING_MESH_WARNINGS
        TRACE("Trail::AddNode(): adding a trail node on %d bones in '%s'\n", BonesNum, Name.AsChar());
#endif
		int NewHeadNode;
		if ((LastNodePosition[0]-LastPosition[0]).Magnitude()<0.25f*Effect->Width*ScaleFactor)
		{
			NewHeadNode = HeadNode;
			HeadNode = (HeadNode+MAX_ELEMS-1)%MAX_ELEMS;		
		}
		else
		{
			if (Effect->ConstLen)
				Time += (LastNodePosition[0]-LastPosition[0]).Magnitude();
			LastNodePosition[0] = LastPosition[0];
			NewHeadNode = (HeadNode+1)%MAX_ELEMS;
		}

		for (int i=0; i<BonesNum; i++)
		{
			Positions[BonesNum*HeadNode+i] = LastPosition[i];
			LocalBox += LastPosition[i];
		}

		Times[HeadNode] = Time;

		HeadNode = NewHeadNode;
		if (HeadNode == TailNode) 
			TailNode = (TailNode+1)%MAX_ELEMS;
	}
	Box=LocalBox;
}

void Trail::CutTail()
{
    for (int i=TailNode; i!=HeadNode; i=(i+1)%MAX_ELEMS)
    {
        if ((Time-Times[i])/Effect->LifeTime>1)
            TailNode=i;
        else 
            break;
    }

    if ((TailNode != HeadNode) && (Times[(HeadNode-1+MAX_ELEMS)%MAX_ELEMS]-Times[TailNode] > 2*Effect->LifeTime))
    {
        TailNode=(TailNode+1)%MAX_ELEMS;
    }

    Box.Reset();

    if (BonesNum<2)
    {
        TrailNode *TrailNodes = (TrailNode *)Positions;

        for (int i=TailNode; i!=HeadNode; i=(i+1)%MAX_ELEMS){
            Box += TrailNodes[i].Position;
        }
    }
    else
    {
        for (int j=0; j<BonesNum; j++){
            for (int i=TailNode; i!=HeadNode; i=(i+1)%MAX_ELEMS){
                Box += Positions[i*BonesNum+j];
            }
        }
    }

    if (BonesNum<2)
    {
        Box.Max += Effect->Width*ScaleFactor*0.5f;
        Box.Min -= Effect->Width*ScaleFactor*0.5f;
    }
    LocalBox = Box;

    if ((TailNode==HeadNode || ((TailNode+1)%MAX_ELEMS)==HeadNode) && !Parent)
    {
#ifdef  TRANSGAMING
        GEngine->pcfWorld->DeleteEntityDelayed(this);
#else
        GEngine->World->DeleteEntityDelayed(this);
#endif
	}
}

void Trail::AttachToBones(Entity* parent, const DynamicArray<String>& bones)
{
	parent->RegisterChild(this, true);

	if (parent->AsModel()) {
		BonesNum = bones.Size();
		Init(MAX_POSITIONS, max(BonesNum,2));
		for (int i=0; i<BonesNum; i++)
		{
			ParentJoint = TrailBones[i] = Parent->AsModel()->GetJointIndex(bones[i].AsChar());
		}
	}
	else {
		BonesNum = 0;
		Init(MAX_POSITIONS, 2);
	}
}

void Trail::Init(int elems, int vpn)
{
	Free();
	MAX_ELEMS = elems;
#ifndef TRANSGAMING
	Vpn		  = vpn;
#else
    currentNodeTime = 0;
#endif
    m_setTag("Trail");
	Positions = (Vector*)malloc(sizeof(Vector)*elems*vpn);
    m_setTag("Trail");
	Times	  = (float*)malloc(sizeof(float)*elems);
}

void Trail::Free()
{
	if (Positions)	free(Positions), Positions=0;
	if (Times)		free(Times), Times=0;
}

#define TRAIL_LOAD_SAVE_TOKEN 0x5e85

bool Trail::SaveEntity( FileObject* f )
{
// Data for World to recreate us
	*f << (int)ETypeTrail << Effect->Name << Name << Scale << (char) (0);

	if( !f->PutToken( TRAIL_LOAD_SAVE_TOKEN ) ) return false;

	Entity::SaveEntity( f );

	*f << TextureName;
	*f << Translation << ScaleFactor << ConstColor << Time << PositionChanged;
	*f << MAX_ELEMS << BonesNum << HeadNode << TailNode;

	int tracks = BonesNum < 2 ? 2 : BonesNum;

	for (int i=0; i<tracks; i++)
		*f << TrailBones[i];

	for (int i=0; i<tracks; i++)
		*f << LastPosition[i];

	for (int i=0; i<tracks; i++)
		*f << LastNodePosition[i];

	for (int i=TailNode; i!=HeadNode; i=(i+1)%MAX_ELEMS)
	{
		*f << Times[i];
		for (int j=0; j<tracks; j++)
			*f << Positions[j+i*tracks];
	}

	return true;
}

bool Trail::LoadEntity( FileObject* f )
{
	if( !f->TestToken( TRAIL_LOAD_SAVE_TOKEN ) )
	{
		GLog.Print( "Token test failure: Trail::LoadEntity()\n" );
		return false;
	}

	Entity::LoadEntity( f );

	String texName;
	*f >> texName;
	if (texName.Len())
		SetTexture(texName.AsChar());

	*f >> Translation >> ScaleFactor >> ConstColor >> Time >> PositionChanged;
	*f >> MAX_ELEMS >> BonesNum >> HeadNode >> TailNode;

	int tracks = BonesNum < 2 ? 2 : BonesNum;

	Init(MAX_ELEMS, tracks);

	for (int i=0; i<tracks; i++)
		*f >> TrailBones[i];

	for (int i=0; i<tracks; i++)
		*f >> LastPosition[i];

	for (int i=0; i<tracks; i++)
		*f >> LastNodePosition[i];

	for (int i=TailNode; i!=HeadNode; i=(i+1)%MAX_ELEMS)
	{
		*f >> Times[i];
		for (int j=0; j<tracks; j++)
			*f >> Positions[j+i*tracks];
	}

	return true;
}

#ifndef TRANSGAMING
bool Trail::DemoReadEntity( FileObject *f )
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

	if( contents & EDemoWriteTrailBonesNum )
		*f >> BonesNum;
	return true;
}


bool Trail::DemoCalcSize( int &size, DWORD &contents )
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

	if( BonesNum != mLastBonesNum )
	{
		size += sizeof( int );
		contents |= EDemoWriteTrailBonesNum;
	}

	return( contents != 0 );
}


bool Trail::DemoWriteEntity( FileObject *f )
{
	int size = 0;
	DWORD contents = 0;
	if( !DemoCalcSize( size, contents ) )
		return true;

	char parent = ( Parent == NULL ) ? 0 : 1;

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

	if( contents & EDemoWriteTrailBonesNum )
		*f << BonesNum;

	return true;
}


bool Trail::DemoReadFull( FileObject *f )
{
	Entity::DemoReadFull( f );

	String texName;
	*f >> texName;
	if (texName.Len())
		SetTexture(texName.AsChar());

	*f >> Translation >> ScaleFactor >> ConstColor >> Time >> PositionChanged;
	*f >> MAX_ELEMS >> BonesNum >> HeadNode >> TailNode;

	int tracks = BonesNum < 2 ? 2 : BonesNum;

	Init(MAX_ELEMS, tracks);

	for (int i=0; i<tracks; i++)
		*f >> TrailBones[i];

	for (int i=0; i<tracks; i++)
		*f >> LastPosition[i];

	for (int i=0; i<tracks; i++)
		*f >> LastNodePosition[i];

	for (int i=TailNode; i!=HeadNode; i=(i+1)%MAX_ELEMS)
	{
		*f >> Times[i];
		for (int j=0; j<tracks; j++)
			*f >> Positions[j+i*tracks];
	}

	return true;
}


bool Trail::DemoWriteFull( FileObject *f )
{
	Entity::DemoWriteFull( f );

	*f << TextureName;
	*f << Translation << ScaleFactor << ConstColor << Time << PositionChanged;
	*f << MAX_ELEMS << BonesNum << HeadNode << TailNode;

	int tracks = BonesNum < 2 ? 2 : BonesNum;

	for (int i=0; i<tracks; i++)
		*f << TrailBones[i];

	for (int i=0; i<tracks; i++)
		*f << LastPosition[i];

	for (int i=0; i<tracks; i++)
		*f << LastNodePosition[i];

	for (int i=TailNode; i!=HeadNode; i=(i+1)%MAX_ELEMS)
	{
		*f << Times[i];
		for (int j=0; j<tracks; j++)
			*f << Positions[j+i*tracks];
	}

	mLastBonesNum = BonesNum - 1;

	return true;
}
#endif

#ifdef PKTV_ENABLED
char *Trail::GetEffectName() const
{
	return ( char *)Effect->Name.AsChar();
}
#endif
