// #pragma once
#ifndef _BILLBOARD_
#define _BILLBOARD_

#include "Render/RenderDevice.h"
#include "Engine/Entity.h"
#include "Render/ParticleSystem.h"

// ==========================================================================

#ifdef TRANSGAMING
//this value is the time interval (in seconds) between line traces for coronas.
//There is a raycast from the corona to the camera to determine if the corona
//should begin a fadeout or fadein if it becomes obstructed or unobstructed.
# define CORONAS_TRACE_INTERVAL 0.3f
#else
# define CORONAS_TRACE_INTERVAL 0.1f
#endif

class ENGINE_API Billboard : public Entity
{
public:
	enum
	{
		EBillboardCorona	= 1<<0,
	};

	int			BillboardType;
	int			BlendMode;

	String		TexName;
	Texture*	TexPtr;
#ifdef	TRANSGAMING
	Color_TG	Color;
#else
	Color		Color;
#endif
	
	float		Timer;
	float       FadeInTime;
    float       FadeOutTime;

	float       Alpha;
    float       AlphaMax;    
	
	float       Distance;
	float       MaxDistance;
	float       MinDistance;
	float       MaxRadius;
	float       MinRadius;
	float       OffDistance;
	float       TraceMargin;
	BOOL		IsVisible;
	BOOL		HitStatus;
#ifdef	TRANSGAMING
	Sprite_TG	Sprite;
#else
	Sprite		Sprite;
#endif	
	float		TimeDelta,TraceInterval;

public:
	Billboard();
	void SetCorona(bool enable);
	void RenderInitialize(RenderDevice* dev, MaterialSystem* matSys, const char* texSubPath);
	void RenderCleanup   (RenderDevice* dev, MaterialSystem* matSys);
	void Draw(RenderDevice* dev, int mode);
	void Tick(const float delta);
	void FadeTick(BOOL visible,float delta);
    void SetTexture(const char* fileName, int flags=0);
	void UpdateBBox();
    // Load/save
    virtual bool LoadEntity( FileObject* f );
    virtual bool SaveEntity( FileObject* f );

#ifndef TRANSGAMING
    // Demo recording
	virtual bool DemoReadEntity( FileObject *f );
	virtual bool DemoWriteEntity( FileObject *f );
	virtual bool DemoReadFull( FileObject *f );
	virtual bool DemoWriteFull( FileObject *f );
#endif
};



#endif //_BILLBOARD_

