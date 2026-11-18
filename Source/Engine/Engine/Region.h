// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/07 Krystian Galaj
//----------------------------------------------------------------------------

#include "Engine/Entity.h"

class ENGINE_API WorldRegion : public Entity
{
	void* phantomBodyHandle;
	String Name;
public:

	WorldRegion();
	~WorldRegion();

	void SetRegionName( const char *name )
	{
		Name = name;
	}

	bool CreateRegionFromPoints( DynamicArray<Vector>& points, float mass );	// Returns true if succeeded, false if failed

	void EventTrigger( bool entering, Entity* entity );

	virtual void Draw(RenderDevice* dev, int mode) {};
	void  Tick(const float timeDelta) {};
	void EnableDraw(bool) {}

	virtual void SetPosition(const Vector &v);
	virtual Vector& GetPosition();
};
