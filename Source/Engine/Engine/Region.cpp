#include "Engine.h"

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

#include "Physics/PhysicsWorld.h"
#include "Engine/Region.h"

#include "Engine/PCFSystem.h"
extern ENGINE_API PCFSystem* GEngine;

#include "Engine/World.h"

WorldRegion::WorldRegion(void) : Entity(ETypeRegion)
{
	phantomBodyHandle = NULL;
	Name = "";
	Flags |= Entity::EEntNoDraw;
}


#ifdef	TRANSGAMING
WorldRegion::~WorldRegion(void)
{
	if( phantomBodyHandle != NULL )
	{
#ifndef TRANSGAMING_NO_HAVOK
		GEngine->pcfWorld->Physics->DestroyRegion( phantomBodyHandle );
#endif
		phantomBodyHandle = NULL;
	}
}

bool WorldRegion::CreateRegionFromPoints( DynamicArray<Vector>& points, float mass )
{
#ifndef TRANSGAMING_NO_HAVOK
	if( phantomBodyHandle != NULL )
		GEngine->pcfWorld->Physics->DestroyRegion( phantomBodyHandle );
	phantomBodyHandle = GEngine->pcfWorld->Physics->CreateRegionFromPoints( points, this, mass );
#endif
	return ( phantomBodyHandle != NULL );
}

void WorldRegion::EventTrigger( bool entering, Entity* entity )
{
	GScript.QueueMessage(entering?"REGION_ENTERED":"REGION_LEFT", "%e,%e", this, entity);
}

void WorldRegion::SetPosition(const Vector &v)
{
	if (v == Translation)
		return;

	Translation = v;
	UpdateTransform();
#ifndef TRANSGAMING_NO_HAVOK
	if( phantomBodyHandle != NULL )
		GEngine->pcfWorld->Physics->SetRegionPos( phantomBodyHandle, v );
#endif
}

Vector &WorldRegion::GetPosition()
{
#ifndef TRANSGAMING_NO_HAVOK
	if( phantomBodyHandle != NULL )
		Translation = GEngine->pcfWorld->Physics->GetRegionPos( phantomBodyHandle );
#endif
	return Translation;
}
#else
WorldRegion::~WorldRegion(void)
{
	if( phantomBodyHandle != NULL )
	{
		GEngine->World->Physics->DestroyRegion( phantomBodyHandle );
		phantomBodyHandle = NULL;
	}
}

bool WorldRegion::CreateRegionFromPoints( DynamicArray<Vector>& points, float mass )
{
	if( phantomBodyHandle != NULL )
		GEngine->World->Physics->DestroyRegion( phantomBodyHandle );

	phantomBodyHandle = GEngine->World->Physics->CreateRegionFromPoints( points, this, mass );
	return ( phantomBodyHandle != NULL );
}

void WorldRegion::EventTrigger( bool entering, Entity* entity )
{
	GScript.QueueMessage(entering?"REGION_ENTERED":"REGION_LEFT", "%e,%e", this, entity);
}

void WorldRegion::SetPosition(const Vector &v)
{
	if (v == Translation)
		return;

	Translation = v;
	UpdateTransform();
	if( phantomBodyHandle != NULL )
		GEngine->World->Physics->SetRegionPos( phantomBodyHandle, v );
}

Vector &WorldRegion::GetPosition()
{
	if( phantomBodyHandle != NULL )
		Translation = GEngine->World->Physics->GetRegionPos( phantomBodyHandle );
	return Translation;
}
#endif
