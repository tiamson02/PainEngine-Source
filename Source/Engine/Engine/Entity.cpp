#include "Engine.h"
#include "Render/View.h"
#include "Physics/PhysicsWorld.h"
#include "Net/NetworkDevice2.h"

#include "Physics/PhysicsObject.h"
#include "Render/Video.h"
#include "Engine/Model.h"
#include "Engine/Mesh.h"
#include "Render/Light.h"
#include "Engine/World.h"

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

Entity::Entity(int type)
:   Object(), PO(0), Flags(0), Lighting(ELightingNone), Type(type), Sort(ESortOpaque), VisCounter(0), WorldLink(0)
,	LightCount(0), FileName(""), Translation(0,0,0), Parent(NULL)
,	TimeToDelete(-1),TestDeathZone(false), FadeTime(0.5f), mSynchroFlags(0)
,	DistanceFromCamera(0), KillDist(0.0F), mSynchroString(0), GIndex(0)
,	Demonic(false), WarpScreen(false), WaterSurf(false), ReflectSurf(false)
{
	ParentJoint = -1;
    GMemZero(&ZoneLinks[0],sizeof(ZoneLinks));
	SetIdentity();
	for (int i=0; i<MaxLights; i++)
	{
		Lights[i] = NULL;
		LightIntensity[i] = -1;
	}

	TransformByParent = true;
    KillByParent = true;
}

Entity::~Entity( void )
{
	if( mSynchroString ) delete [] mSynchroString;

	if (Env.DirLight) delete Env.DirLight;

	for (int i=0; i<Children.Size(); i++)
	{
		bool deleteMe = Children[i]->OnUnregister();
		Children[i]->Parent = NULL;
        if (deleteMe && Children[i]->KillByParent)
#ifdef	TRANSGAMING
			GEngine->pcfWorld->DeleteEntity(Children[i]);
#else
			GEngine->World->DeleteEntity(Children[i]);
#endif
	}
	Children.ClearArray();

	if (Parent) 
	{
		Parent->UnregisterChild(this);
		Parent = NULL;
	}

	if (Env.Link)
		Env.Link->RemoveEntity(this);

	RemovePhysicsObject();

	SetSynchroState( ESynchroDelete, 0, SERVER_ID, 0 );
}

float Entity::GetCamDistance() const
{ 
    return (GEngine->Video->GetActiveView()->LocalViewport.EyePosition - Box.GetCenter()).Magnitude(); 
}

void Entity::SetPosition(const Vector &v)
{
	if (v == Translation)
		return;
/*
	float x = v.X;
	unsigned int a = *(unsigned int *)(&x);
	a &= 0x7fffffff;
	if( a > 0x7f000000 )
		GLog.Print( "Entity position bug detected!!!\n" );
*/
	Translation = v;
	UpdateTransform();
}

void Entity::SetOrientation(const float angle)
{
	SetRotation(Quaternion(angle, Vector(0,-1,0)));
}

void Entity::SetRotation(const Quaternion& rotation)
{
	if (Rotation == rotation)
		return;
	Rotation = rotation;
	UpdateTransform();
}

void Entity::SetPositionAndRotation(const Vector &translation, const Quaternion& rotation)
{
	if (Translation == translation && Rotation == rotation)
		return;
/*
	float x = translation.X;
	unsigned int a = *(unsigned int *)(&x);
	a &= 0x7fffffff;
	if( a > 0x7f000000 )
		GLog.Print( "Entity position bug detected!!!\n" );
*/
	Translation = translation;
	Rotation = rotation;
	UpdateTransform();
}

void Entity::SetScale(float scale)
{
	Scale = scale;
	UpdateTransform();
}

void Entity::SetObjectTransform(const Matrix& mat)
{
	Matrix m = mat;
	Scale = m.X.Normal.Magnitude();
	Translation = m.W.Normal;
	float invScale = 1/Scale;

	m.X.Normal *= invScale;
	m.Y.Normal *= invScale;
	m.Z.Normal *= invScale;

	Rotation = Quaternion(m);

	UpdateTransform();	
}

#ifdef TRANSGAMING_USE_EDITOR
void Entity::RenderBoundingBox(int color)
{
	LineRender& lineBuf = GEngine->Video->LineBuf;
    RenderDevice* dev=GEngine->GetRenderDevice();

	dev->SetObjectTransform(GIdentityMatrix);
	lineBuf.DrawBox(Box, color);
}

void Entity::RenderLocalBoundingBox(int color)
{
	LineRender& lineBuf = GEngine->Video->LineBuf;
	RenderDevice* dev=GEngine->GetRenderDevice();

	dev->SetObjectTransform(&ObjectToWorld);
	lineBuf.DrawBox(LocalBox, color);
	dev->SetObjectTransform(GIdentityMatrix);
}
#endif

void Entity::EnableDraw(bool en)
{
	if (en)
		Flags &= ~Entity::EEntNoDraw;
	else
		Flags |= Entity::EEntNoDraw;
}

void Entity::CreatePhysicsObject( DWORD poType, float scale, int collisionGroup, bool dontAddToWorld )
{
	if( !GEngine->Physics ) return;
	if( PO ) delete PO;
#ifdef	TRANSGAMING
#ifdef TRANSGAMING_NO_HAVOK
	PO = NULL;
#else
	PO = GEngine->pcfWorld->Physics->CreatePhysicsObject( Translation, Rotation, poType, this, scale, collisionGroup, dontAddToWorld );
#endif //TRANSGAMING_NO_HAVOK
#else
	PO = GEngine->World->Physics->CreatePhysicsObject( Translation, Rotation, poType, this, scale, collisionGroup, dontAddToWorld );
#endif
}

void Entity::RemovePhysicsObject( void )
{
	if( PO ) delete PO;
	PO = 0;
}


bool Entity::SeesPoint( Vector& point )
{
	if( !PO || !GEngine->Game ) return false;
#ifdef	TRANSGAMING
#ifdef TRANSGAMING_NO_HAVOK
	return false;
#else
	return GEngine->pcfWorld->Physics->CalculatePawnToPointVisibility( PO, point );
#endif //TRANSGAMING_NO_HAVOK
#else
	return GEngine->World->Physics->CalculatePawnToPointVisibility( PO, point );
#endif
}

bool Entity::SeesEntity( Entity& entity )
{
	if( !PO || !GEngine->Game || !entity.PO ) return false;
#ifdef	TRANSGAMING
#ifdef TRANSGAMING_NO_HAVOK
	return false;
#else
	return GEngine->pcfWorld->Physics->CalculatePawnToEntityVisibility( PO, entity.PO );
#endif //TRANSGAMING_NO_HAVOK
#else
	return GEngine->World->Physics->CalculatePawnToEntityVisibility( PO, entity.PO );
#endif
}

#ifdef	TRANSGAMING
void Entity::SetEnvironmentAmbient(bool enable, const Color_TG& col) // enforce ambient
#else
void Entity::SetEnvironmentAmbient(bool enable, const Color& col) // enforce ambient
#endif
{
	Env.OverwriteAmbient = enable;
	Env.Ambient = col;
}

#ifdef	TRANSGAMING
const Color_TG Entity::GetEnvironmentAmbient()
#else
const Color Entity::GetEnvironmentAmbient()
#endif
{
	if (Env.OverwriteAmbient)
#ifdef	TRANSGAMING	
		return Color_TG(Env.Ambient[0],Env.Ambient[1],Env.Ambient[2],1.0f);

	Color_TG dstAmbient(0,0,0);
#else
		return Color(Env.Ambient[0],Env.Ambient[1],Env.Ambient[2],1.0f);
		
	Color dstAmbient(0,0,0);
#endif

	if (Env.Link && (Env.Link->Defines & Environment::EEnvAmbient))
		dstAmbient = Env.Link->AmbientColor;	
    else if(WorldLink)
		dstAmbient = WorldLink->Env.AmbientColor;


	if (Env.LightProgress<1)
	{
		float t = Env.LightProgressDelta/(1-Env.LightProgress);
		if (t<1)
		{
#ifdef	TRANSGAMING	
			Env.Ambient = Vector(BaseVector(Env.Ambient))*(1-t) + Vector(BaseVector(dstAmbient))*(t);
			return Color_TG(Env.Ambient[0],Env.Ambient[1],Env.Ambient[2],1.0f);
#else
			Env.Ambient = Vector(Env.Ambient)*(1-t) + Vector(dstAmbient)*(t);
			return Color(Env.Ambient[0],Env.Ambient[1],Env.Ambient[2],1.0f);
#endif			
		}
	}

	Env.Ambient = dstAmbient;
	return dstAmbient;
}

Light* Entity::GetEnvironmentDirLight()
{
	Light* dstLight;

	if (Env.Link && (Env.Link->Defines & Environment::EEnvDirLight))
		dstLight = &Env.Link->DirLight;	
	else if (WorldLink)
		dstLight = &WorldLink->Env.DirLight;
	else 
		return NULL;

	if (!Env.DirLight)
	{
        m_setTag("Light");

		Env.DirLight = new Light();
		Env.DirLight->SetType(Light::ELightDir);
	}

	if (Env.LightProgress<1)
	{
		float t = Env.LightProgressDelta/(1-Env.LightProgress);
		if (t<1)
		{
#ifdef	TRANSGAMING
			Env.DirColor = Env.DirColor*(1-t) + Vector(BaseVector(dstLight->Color))*t;
			Env.DirLight->Color		= Color_TG(Env.DirColor[0], Env.DirColor[1], Env.DirColor[2], 1.0f);
#else
			Env.DirColor = Env.DirColor*(1-t) + Vector(dstLight->Color)*t;
			Env.DirLight->Color		= Color(Env.DirColor[0], Env.DirColor[1], Env.DirColor[2], 1.0f);
#endif
			Env.DirLight->Intensity = Env.DirLight->Intensity*(1-t) + dstLight->Intensity*t;
			Env.DirLight->SetDirection(Env.DirLight->GetDirection()*(1-t) + dstLight->GetDirection()*t);

			return Env.DirLight;
		}
	}

	Env.DirColor = dstLight->Color;
	Env.DirLight->Color		= dstLight->Color;
	Env.DirLight->Intensity = dstLight->Intensity;
	Env.DirLight->SetDirection(dstLight->GetDirection());

	return dstLight;
}

void Entity::ResetLights()
{
	LightCount=0; 
	for (int i=0; i<MaxLights; ++i) 
		LightIntensity[i] = -1; 

	Light *light = GetEnvironmentDirLight();

	if (light)
		AddLight(light);
}

void Entity::AddLight(Light* light)
{
	if (light->Intensity <= 0)
		return;

	if (light->Flags & Light::ELightLitParent) {
		if (light->Parent != this) {
			return;
		}
		else {
			float intensity = light->GetAttIntensity(this);

			for (int i=0; i<LightCount; ++i)
			{
				if (!(Lights[i]->Flags & Light::ELightLitParent))
				{
					Lights[i]			= light;
					LightIntensity[i]	= intensity;
					LightCount=i+1;

					return;
				}
			}
		}
	}
	else {
		if (LightCount>0 && (Lights[0]->Flags & Light::ELightLitParent)) {
			return;
		}
	}

	float intensity = light->GetAttIntensity(this);

	if (light->Flags & Light::ELightDynamic)
	{
		if (Lighting == ELightingDLights || Lighting == ELightingAllLights)
			intensity = 1;
	}
	else 
	{
		if (Lighting == ELightingDLights)
			return;

		/*
		if ((Lighting == ELightingAllLights) && (light->Flags & Light::ELightFakeSpecular))
		{
			bool b = (AsMesh() && (AsMesh()->SpecularLights[0] || AsMesh()->SpecularLights[1]));
			if (!b)
				return;
		}
		*/

		if (Lighting == ELightingAllLights)
		{
			int fakeSpecular = (light->Flags & Light::ELightFakeSpecular);
			if (AsMesh()) 
			{
				WorldMesh* Mesh = AsMesh();
				if (!(Mesh->Flags & WorldMesh::EVertFormatNTU))
				{
					//if (!fakeSpecular)
					//	return;

					if (Mesh->SpecularLights[0] != light && Mesh->SpecularLights[1] != light)
						return;
				}
				else if (fakeSpecular)
					return;
			}
			else if (fakeSpecular)
				return;
		}
	}

	if (intensity<=0)
		return;

	if (light->EnvRefCount) {

		if (!Env.Link || Env.Link->DependentLights.Find(light)==-1)
			return;
	}

	for (int i=0; i<MaxLights; ++i)
	{
		if (intensity > LightIntensity[i])
		{
			for (int j=MaxLights-2; j>=i; --j)
			{
				Lights[j+1]			= Lights[j];
				LightIntensity[j+1]	= LightIntensity[j];
			}

			Lights[i]			= light;
			LightIntensity[i]	= intensity;

			if (LightCount<MaxLights)
				++LightCount;

			return;
		}
	}
}

int Entity::ComputeVSLights(int maxLights)
{
	const Vector& eyeVec = GEngine->Video->GetActiveView()->LocalViewport.EyePosition;
	Vector eyeDir = eyeVec - ObjectToWorld.GetTranslation();
	eyeDir.Normalize();

	for (int iLight=0; iLight<MaxLights; iLight++)
	{
//		GLog.Print( "iLight: %d, LightCount: %d\n", iLight, LightCount );
		if (iLight<LightCount && Lights[iLight])
		{
			Lights[iLight]->GetDirAndAttCol(this, VSLights[iLight].Dir, VSLights[iLight].Color);

			Vector halfDirVec = eyeDir + VSLights[iLight].Dir.Normal;
			halfDirVec.Normalize();


			VSLights[iLight].HalfDir = Plane(halfDirVec);
		}
		else
		{
			VSLights[iLight].Color = Plane(0,0,0,0);// RGB-color, A-attenuation
			VSLights[iLight].Dir = Plane(1,0,0,0);
			VSLights[iLight].HalfDir = Plane(1,0,0,0);
		}
	}

	if (LightCount > maxLights)
	{
		// Color.W holds Light intensity
		float dL0L2 = ( VSLights[maxLights-2].Color.W - VSLights[maxLights].Color.W );
		float dL1L2 = ( VSLights[maxLights-1].Color.W - VSLights[maxLights].Color.W );
		float factor = 1;

		if (dL0L2>F_EPS && dL0L2>dL1L2)
			factor = dL1L2/dL0L2;

		VSLights[maxLights-1].Color *= factor;
	}

	return min(maxLights, LightCount);
}

void Entity::Tick( const float timeDelta )
{
	if( Env.LightFadeTime > 0 )
	{
		Env.LightProgressDelta = timeDelta/Env.LightFadeTime;
		Env.LightProgress += Env.LightProgressDelta;
	}
}

#ifndef TRANSGAMING
inline 
#endif
void Entity::SetSynchroState( unsigned char newState, unsigned char updateClass, unsigned short specialClient, unsigned char specialClass )
{
	if( newState == mSynchroFlags ) return;
	if( GEngine )
	{
		if( GEngine->Net )
		{
			if( !GEngine->Net->IsServer() )
				return;	// Client should have no control over entities' synchro state
			GEngine->Net->SetSynchroState( this, newState );
		}
	}
	mSynchroFlags = newState;
}

#ifndef TRANSGAMING
inline
#endif
void Entity::UpdateTransform( bool updateInWorld )
{
	ObjectToWorld = ScaleMatrix(Scale) * Rotation.ToMatrix();
	ObjectToWorld.SetTranslation(Translation);
	if (Parent && TransformByParent)
	{
        Matrix m = ObjectToWorld;
		if (ParentJoint>-1 && Parent->AsModel())            
			ObjectToWorld = m * Parent->AsModel()->GetJointTransform(ParentJoint);
		else
			ObjectToWorld = m * Parent->ObjectToWorld;
	}

	Box = LocalBox.Transform(ObjectToWorld);

	if (updateInWorld && WorldLink)
		WorldLink->UpdateEntity(this);

	for(int i=0; i<Children.Size(); i++)
		if (Children[i]->TransformByParent)
			Children[i]->UpdateTransform();
}

#ifdef TRANSGAMING
void Entity::RegisterChild(Entity* child,bool transformByParent,char joint)
#else
inline void Entity::RegisterChild(Entity* child,bool transformByParent,char joint)
#endif
{        
    Entity* t = this;
    do
    {
        if( child == t )
            return;
        t = t->Parent;
    }
    while(t);

	if (child->Parent)
		child->Parent->UnregisterChild(child);

	child->Parent = this;
    child->TransformByParent = transformByParent;
    child->ParentJoint = joint;

	Children.Add(child);
}

#ifdef TRANSGAMING
void Entity::UnregisterChild(Entity* child)
#else
inline void Entity::UnregisterChild(Entity* child)
#endif
{
	for (int i=0; i<Children.Size(); i++)
		if (Children[i]==child)
		{
            child->OnUnregister();
            child->Parent = NULL;
            Children.Remove(i);
			return;
		}
}

#ifdef TRANSGAMING
void Entity::UnregisterAllChildren( int type )
#else
inline void Entity::UnregisterAllChildren( int type )
#endif
{
	for (int i=0; i<Children.Size(); i++)
	{
		if( ( type == -1 ) || ( type == Children[i]->Type ) )
		{
           Children[i]->OnUnregister();
           Children[i]->Parent = NULL;
           Children.FastRemove(i);
           i--;
		}
	}
}

#ifdef TRANSGAMING
void Entity::KillAllChildren( int type )
#else
inline void Entity::KillAllChildren( int type )
#endif
{
    for (int i=0; i<Children.Size(); i++)
    {
        if( ( type == -1 ) || ( type == Children[i]->Type ) )
        {
            bool deleteMe = Children[i]->OnUnregister();
            Children[i]->Parent = NULL;
            if (deleteMe && Children[i]->KillByParent)
#ifdef	TRANSGAMING
                GEngine->pcfWorld->DeleteEntity(Children[i]);
#else
                GEngine->World->DeleteEntity(Children[i]);
#endif
            Children.FastRemove(i);
            i--;
        }
    }
}

#ifdef TRANSGAMING
bool Entity::KillAllChildrenByName( const char * name) // Hmmm....weird.
#else
inline bool Entity::KillAllChildrenByName( const char * name)
#endif
{
    bool killed = false;
    for (int i=0; i<Children.Size(); i++)
    {
        if( Children[i]->Name.EqualsNoCase(name) || Children[i]->FileName.EqualsNoCase(name))
        {
            bool deleteMe = Children[i]->OnUnregister();
            Children[i]->Parent = NULL;
            if (deleteMe && Children[i]->KillByParent)
#ifdef	TRANSGAMING
                GEngine->pcfWorld->DeleteEntity(Children[i]);
#else
                GEngine->World->DeleteEntity(Children[i]);
#endif
            Children.FastRemove(i);
            i--;
            killed = true;
        }
    }
    return killed;
}

#ifdef TRANSGAMING
Entity* Entity::GetChildByName( const char * name)
#else
inline Entity* Entity::GetChildByName( const char * name)
#endif
{
    for (int i=0; i<Children.Size(); i++)
    {
        if( Children[i]->Name == String(name) || Children[i]->FileName == String(name))
            return Children[i];
    }
    return NULL;
}

#ifdef TRANSGAMING
void Entity::SetIdentity()
#else
inline void Entity::SetIdentity()
#endif
{
	Scale = 1.0f;
	Rotation = IdentityQuaternion();
	Translation = Vector(0,0,0);
	ObjectToWorld.SetIdentity();    // = IdentityMatrix();

	if (WorldLink)
		WorldLink->UpdateEntity(this);
}

#define ENTITY_LOAD_SAVE_TOKEN 0x5e53

bool Entity::LoadEntity( FileObject* f )
{
	if( !f->TestToken( ENTITY_LOAD_SAVE_TOKEN ) )
	{
		GLog.Print( "Token test failure: Entity::LoadEntity()\n" );
		return false;
	}

	*f >> (DWORD&)GIndex;

#ifdef	TRANSGAMING
	while( GEngine->pcfWorld->EntityGIndexes.Size() <= (int)GIndex )
		GEngine->pcfWorld->EntityGIndexes.Add(NULL);

    if (GIndex > 0)
        GEngine->pcfWorld->EntityGIndexes[GIndex] = this;
#else
	while( GEngine->World->EntityGIndexes.Size() <= (int)GIndex )
		GEngine->World->EntityGIndexes.Add(NULL);

    if (GIndex > 0)
        GEngine->World->EntityGIndexes[GIndex] = this;
#endif

    *f >> TimeToDelete >> FadeTime >> TestDeathZone  >> Sort >> Demonic >> Flags
	   >> WarpScreen >> WaterSurf >> ReflectSurf;

    
    char parent;
    *f >> parent;
    if (parent)
    {
        unsigned int parentindex;
        *f >> parentindex >> ParentJoint >> TransformByParent >> KillByParent;
        LightCount = parentindex; // temporary
    }
    

	return true;
}

bool Entity::SaveEntity( FileObject* f )
{
	if( !f->PutToken( ENTITY_LOAD_SAVE_TOKEN ) ) return false;

	*f << (DWORD)GIndex;
    *f << TimeToDelete << FadeTime << TestDeathZone << Sort << Demonic << Flags
	   << WarpScreen << WaterSurf << ReflectSurf;

    if (Parent)
    {
        *f << (char) 1;
        *f << Parent->GIndex << ParentJoint << TransformByParent << KillByParent;
    }
    else
      *f << (char) 0;

	return true;
}


#ifndef TRANSGAMING
bool Entity::DemoReadEntity( FileObject *f )
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
	else
	{
		if( contents & EDemoWriteParentGIndex )
		{
			GLog.Print( "Bad parent info - ParentGIndex\n" );
			f->Seek( 4, SEEK_CUR );
		}
		if( contents & EDemoWriteParentJoint )
		{
			GLog.Print( "Bad parent info - ParentJoint\n" );
			f->Seek( 1, SEEK_CUR );
		}
		if( contents & EDemoWriteTransformByParent )
		{
			GLog.Print( "Bad parent info - TransformByParent\n" );
			f->Seek( 1, SEEK_CUR );
		}
	}

	UpdateTransform(true);

	return true;
}


bool Entity::DemoCalcSize( int &size, DWORD &contents )
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

	return( contents != 0 );
}


bool Entity::DemoWriteEntity( FileObject *f )
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

	return true;
}


bool Entity::DemoReadFull( FileObject *f )
{
	return true;
}


bool Entity::DemoWriteFull( FileObject *f )
{
	*f << (char)FileName.Len();
	f->Write( FileName.AsChar(), FileName.Len() );
	*f << (char)Name.Len();
	f->Write( Name.AsChar(), Name.Len() );
	*f << Scale;
	*f << Flags;
	*f << TimeToDelete << FadeTime;
	*f << Sort;

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

	return true;
}
#endif

void Entity::Reset() // reset entity for reusing
{
    // destructor
    if( mSynchroString ) 
	{
        delete [] mSynchroString;
	    mSynchroString = 0;
	}

    if (Env.DirLight) 
	{
        delete Env.DirLight;
	    Env.DirLight = 0;
	}

    for( int i = 0; i < Children.Size(); ++i )
    {
		bool deleteMe = Children[i]->OnUnregister();
		Children[i]->Parent = NULL;
		if( deleteMe && Children[i]->KillByParent )
#ifdef	TRANSGAMING
			GEngine->pcfWorld->DeleteEntity( Children[i] );
#else
			GEngine->World->DeleteEntity( Children[i] );
#endif
    }
    Children.ClearArray();

    if (Parent) 
    {
        Parent->UnregisterChild(this);
        Parent = NULL;
    }

    if( Env.Link )
	{
        Env.Link->RemoveEntity(this);
		Env.Link = 0;
	}

    RemovePhysicsObject();

    SetSynchroState( Entity::ESynchroDelete, 0, SERVER_ID, 0 );
    mSynchroFlags = 0;

    // constructor
    VisCounter = 0; 
    WorldLink = NULL;
    LightCount = 0;
    Translation.Set(0,0,0);
    TimeToDelete = -1;
    TestDeathZone = false;
    
    FadeTime = 0.5f;
    DistanceFromCamera = 0;
    Demonic = false;

	EnableDraw(true);

    ParentJoint = -1;
    GMemZero(&ZoneLinks[0],sizeof(ZoneLinks));

    TransformByParent = true;
    KillByParent = true;

    GIndex = 0;

	if (AsMesh()) AsMesh()->SetupFlags();
}
