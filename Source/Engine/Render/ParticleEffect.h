
// #pragma once
#ifndef _PARTICLEEFFECT_
#define _PARTICLEEFFECT_


class ParticleEmitter;

#include "Engine/Entity.h"

class ENGINE_API ParticleEffect : public Entity
{
public:
    static const int	MaxEmitters = 16;
    struct ENGINE_API EmitterDef 
    {
        ParticleEmitter*	Emitter;
        Vector				Pos;
        Vector				Rotation;
        float				Scale;
        Matrix				Transform;

        void SetupTransform();
    };

    EmitterDef			Emitters[MaxEmitters];
    int                 EmitterCount;

    float			VisibilityTimeOut;
    Vector			ParentOffset;
    bool			ParentRotation;
    Quaternion		ParentRotationOffset;

    Vector CameraOffset;

    bool	Active;
	bool	FixedTransform;
    bool	IsImmortal; // for editor

    ParticleEffect*	Next;

public:
    ParticleEffect();
    ~ParticleEffect();

    int AddEmitter(ParticleEmitter* emitter);
    void RemoveEmitter(int index);
	void SetupEmitters(bool forceLastPosition = false);
    void SetParentOffset(const Vector &offset,int joint, const Vector &camoffset = Vector(0.0f,0.0f,0.0f));
	void ComputeNewPosition();

    // Entity interface
    void RenderInitialize(RenderDevice* dev, MaterialSystem* matSys, const char* texSubPath=0);
    void RenderCleanup   (RenderDevice* dev, MaterialSystem* matSys);
    void Tick(const float deltaTime);
    void Draw(RenderDevice* /*dev*/, int mode);

    void SetPosition(const Vector& pos);
    void SetRotation(const Quaternion& rotation);
    void SetScale(float scale);

    //void SetEmitterTransform(int index, const Vector& pos, const Quaternion& rotation, float scale);

    void Deactivate(BOOL instantKill=FALSE);

    // local
    BOOL IsActive() const;
    void Restart();
    bool OnUnregister();
    // Load/save
    virtual bool LoadEntity( FileObject* f );
    virtual bool SaveEntity( FileObject* f );
	bool updateOnce;

#ifndef TRANSGAMING
	// Demo recording
	virtual bool DemoReadEntity( FileObject *f );
	virtual bool DemoWriteEntity( FileObject *f );
	virtual bool DemoReadFull( FileObject *f );
	virtual bool DemoWriteFull( FileObject *f );
#endif
};


#endif //_PARTICLEEFFECT_

