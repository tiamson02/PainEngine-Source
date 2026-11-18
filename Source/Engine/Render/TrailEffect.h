#error this file is obselete

class ENGINE_API TrailEffect : public Entity
{
	friend class TrailSystem;

    String		TextureName;
    Texture*	TexturePtr;
	float		TexAnimFPS;
    int			BlendMode;
	float		LifeTime;
	float		Width;


public:
    TrailEffect();
    ~TrailEffect();
};

class ENGINE_API TrailSystem
{
    typedef VertexBuffer<SimpleVertex,RenderDevice::ESimpleVertex/*,4096*/>  SimpleVertexBuffer;

	HashMap<String,TrailEffect>	TrailEffects;

public:
	TrailSystem();
	~TrailSystem();

    BOOL Init();
	TrailEffect* CreateTrailEffect(const char *name);

protected:
    BOOL LoadTrailEffect(TrailEffect& trailEffect, const char *name);
};