#include "Engine.h"
#include "Engine/Environment.h"
#include "Engine/World.h"
#include "Engine/PCFSystem.h"
extern ENGINE_API PCFSystem* GEngine;

Environment::Environment(void) : Entity(ETypeEnv), AmbientColor(0,0,0,0), Defines(0), LightFadeTime(1)
{
	DirLight.SetType(Light::ELightDir);
}

Environment::~Environment(void)
{
	for (int i=0; i<Entities.Size(); ++i)
		Entities[i]->Env.Link = NULL;
}

void Environment::AddEntity(Entity* entity)
{
	Entities.Add(entity);
	entity->Env.Link = this;
	entity->Env.LightFadeTime = LightFadeTime;
	entity->Env.LightProgress = 0;
	entity->Env.LightProgressDelta = 0;
}

void Environment::RemoveEntity(Entity* entity)
{
	int idx = Entities.Find(entity);
	if (idx!=-1) {
		Entities.FastRemove(idx);
		entity->Env.Link = NULL;
		entity->Env.LightFadeTime	= LightFadeTime;
		entity->Env.LightProgress = 0;
		entity->Env.LightProgressDelta = 0;
	}
}

void Environment::AddLight(Light* light)
{
	if (light) {
		DependentLights.Add(light);
		light->EnvRefCount ++;
	}
}

void Environment::RemoveLight(Light* light)
{
	if (light) {

		int idx = DependentLights.Find(light);

		if (idx!=-1) {

			DependentLights.FastRemove(idx);

			if (WorldLink->PtrMap.Find(light)) 
			{
				light->EnvRefCount --;
			}
		}
	}
}

void Environment::RemoveLights()
{
	int size = DependentLights.Size();

	for (int i=size-1; i>=0; i--) {
		RemoveLight(DependentLights[i]);
	}

	DependentLights.ClearArray();
}

BOOL Environment::PointTest(const Vector& point) const
{
	if (!Box.Intersect(point))
		return false;

	if (BoundingVolume.PlaneCount!=0)
	{
		return BoundingVolume.PointTest(point);
	}

	return true;
}


void Environment::UpdateTransform(bool updateInWorld)
{
	Entity::UpdateTransform(false);

	if (Rotation==Quaternion(1,0,0,0))
	{
		if (BoundingVolume.PlaneCount != 0)
			BoundingVolume.PlaneCount = 0;
	}
	else
	{
		BoundingVolume = LocalBox;
		BoundingVolume.Transform(ObjectToWorld);
	}

	if (updateInWorld && WorldLink)
		WorldLink->UpdateEntity(this);

}

//============================================================================
#ifndef TRANSGAMING
bool Environment::DemoReadEntity( FileObject *f )
{
	Entity::DemoReadEntity( f );
	int numDepLights = 0;
	*f >> numDepLights;
	for( int i = 0; i < numDepLights; i++ )
	{
		int index;
		*f >> index;
#ifdef	TRANSGAMING
		Light *l = ( Light *)GEngine->pcfWorld->GetEntityByIndex( index, Entity::ETypeLight );
#else
		Light *l = ( Light *)GEngine->World->GetEntityByIndex( index, Entity::ETypeLight );
#endif
		if( l ) AddLight( l );
	}
	return true;
}


bool Environment::DemoWriteEntity( FileObject *f )
{
	int size = 0;
	DWORD contents = 0;
	if( !DemoCalcSize( size, contents ) )
		return true;

	size += 4 * ( DependentLights.Size() + 1 );

	*f << size;
	*f << contents;

	char parent = ( Parent == NULL ) ? 0 : 1;

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

	*f << DependentLights.Size();
	for( int i = 0; i < DependentLights.Size(); i++ )
		*f << DependentLights[i]->GIndex;
	
	return true;
}


bool Environment::DemoReadFull( FileObject *f )
{
	Entity::DemoReadFull( f );

	*f >> Defines;

	char len;
	char buf[255];

	*f >> len;
	f->Read( buf, len );
	buf[len] = 0;
	DirLight.FileName = String( buf );

	*f >> len;
	f->Read( buf, len );
	buf[len] = 0;
	DirLight.Name = String( buf );

	float scale;
	*f >> scale;
	DirLight.SetScale( scale );
	*f >> DirLight.Flags;
	*f >> DirLight.TimeToDelete >> DirLight.FadeTime;
	*f >> DirLight.Sort;

	DirLight.DemoReadFull( f );

	DWORD c;
    *f >> c;
	AmbientColor = c;

	*f >> FogParams.Mode;
	*f >> FogParams.Start;
	*f >> FogParams.End;
	*f >> FogParams.Density;
	*f >> FogParams.Color;
	*f >> FogParams.VolStart;
	*f >> FogParams.VolEnd;
	*f >> FogParams.VolColor;

    *f >> LightFadeTime;

	BYTE planeCount;
	*f >> planeCount;
	BoundingVolume.PlaneCount = planeCount;
	for( int i = 0; i < BoundingVolume.PlaneCount; i++ )
	{
		*f >> BoundingVolume.BoundingPlanes[i].X;
		*f >> BoundingVolume.BoundingPlanes[i].Y;
		*f >> BoundingVolume.BoundingPlanes[i].Z;
		*f >> BoundingVolume.BoundingPlanes[i].W;
	}

	int numDepLights = 0;
	*f >> numDepLights;
	for( int i = 0; i < numDepLights; i++ )
	{
		int index;
		*f >> index;
#ifdef	TRANSGAMING
		Light *l = ( Light *)GEngine->pcfWorld->GetEntityByIndex( index, Entity::ETypeLight );
#else
		Light *l = ( Light *)GEngine->World->GetEntityByIndex( index, Entity::ETypeLight );
#endif
		if( l ) AddLight( l );
	}

//    *f >> Water;

	UpdateTransform();

	return true;
}


bool Environment::DemoWriteFull( FileObject *f )
{
	Entity::DemoWriteFull( f );

	*f << Defines;

	DirLight.DemoWriteFull( f );

    *f << (DWORD)AmbientColor;

	*f << FogParams.Mode;
	*f << FogParams.Start;
	*f << FogParams.End;
	*f << FogParams.Density;
	*f << FogParams.Color;
	*f << FogParams.VolStart;
	*f << FogParams.VolEnd;
	*f << FogParams.VolColor;

    *f << LightFadeTime;

	*f << ( BYTE )BoundingVolume.PlaneCount;
	for( int i = 0; i < BoundingVolume.PlaneCount; i++ )
	{
		*f << BoundingVolume.BoundingPlanes[i].X;
		*f << BoundingVolume.BoundingPlanes[i].Y;
		*f << BoundingVolume.BoundingPlanes[i].Z;
		*f << BoundingVolume.BoundingPlanes[i].W;
	}

	*f << DependentLights.Size();
	for( int i = 0; i < DependentLights.Size(); i++ )
		*f << DependentLights[i]->GIndex;

//    *f << Water;

	return true;
}
#endif
