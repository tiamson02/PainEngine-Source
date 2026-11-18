// #pragma once
#ifndef _LIGHT_
#define _LIGHT_

#include "Render/RenderDevice.h"
#include "Engine/Entity.h"

// ==========================================================================
class Environment;

#ifdef TRANSGAMING_WINDOWS
class ENGINE_API Light : public Entity
#else
class Light : public Entity
#endif
{
public:
	enum
	{
		ELightHasCorona		= (1<<20),
        ELightDynamic		= (1<<22),
        ELightLitParent		= (1<<23),
		ELightFakeSpecular	= (1<<24),
	};
	enum
	{
		ELightNone	= 0,
		ELightDir	= 1,
		ELightPoint	= 2,
		ELightSpot  = 3,
	};
	bool			Volumetric;
#ifdef	TRANSGAMING
	Color_TG		Color;
#else
	Color			Color;
#endif
	float			Intensity;
	float			FalloffAngleCos;
	int				EnvRefCount;
	int				Dindex; // index to dynamic lights array in World
	
	ConvexVolume	SpotFrustum;
	Matrix			ProjMatrix;
	Texture*		Projector;
	String			ProjectorName;
	bool			ImportantDynamic;	// can't be disabled by DynamicLights option in menu

protected:
	Vector	    	Direction;
	float			ConeAngleCos;

	int				LightType;
	float			Radius;
	float			StartFalloff;
	BoundingSphere	Sphere;

public:
    Light();
	~Light();

	void Draw(RenderDevice* dev, int mode);
	void UpdateTransform(bool updateInWorld);
	void EnableDynamic(bool en);
	void EnableFakeSpecular(bool en);
	void EnableDraw(bool) {}
	void UpdateProj();

	void SetProjectorTexture(const char *tex);
	void SetType(int type);
	void SetDirection(const Vector& vec);
	void SetConeAngleCos(float angle);
	void SetImportantDynamic(bool en);

	Matrix GetProjMatrix() const;
	void  GetDirAndAttCol(const Entity* entity, Plane& dir, Plane& col) const;
	float GetAttIntensity(const Entity* entity) const;
	int GetType() const				{ return LightType; }
	float GetRadius() const			{ return Radius; }
	Vector GetDirection() const		{ return Direction; }
	float GetConeAngleCos() const	{ return ConeAngleCos; }

	void SetRadius(const float radius);
	void SetFalloff(const float startFalloff);
	float GetStartFalloff() const { return StartFalloff; }

	BOOL Intersect(const BoundingBox& box);

	// Load/save
	virtual bool SaveEntity( FileObject* f );
	virtual bool LoadEntity( FileObject* f );

#ifndef TRANSGAMING
    // Demo recording
	virtual bool DemoReadEntity( FileObject *f );
	virtual bool DemoWriteEntity( FileObject *f );
	virtual bool DemoReadFull( FileObject *f );
	virtual bool DemoWriteFull( FileObject *f );
#endif
};


#endif //_LIGHT_

