#error this file is obselete

#ifndef __ParticleRender_h__
#define __ParticleRender_h__

class ParticleEmitter;
class ParticleEffect;

class ParticleRenderBase
{
public:
	ParticleRenderBase(int InMode) : Mode(InMode) {}

	virtual void SetState() = 0;
	virtual void Draw(ParticleEffect* Effect) = 0;

protected:
	int	Mode;
};

class ParticleRenderTnL	: public ParticleRenderBase
{
public:
	ParticleRenderTnL(int InMode);

	void SetState();
	void Draw(ParticleEffect* Effect);

private:
	void DrawBatched(const ParticleEmitter* Emitter);
	void DrawAnimated(const ParticleEmitter* Emitter);
};

#ifndef TRANSGAMING
class ParticleRenderVS : public ParticleRenderBase
{
public:
	ParticleRenderVS(int InMode);

	void SetState();
	void Draw(ParticleEffect* Effect);

private:
	void SetSpriteConstMemory();
	void SetSparkConstMemory();
};
#endif

#endif // __ParticleRender_h__


