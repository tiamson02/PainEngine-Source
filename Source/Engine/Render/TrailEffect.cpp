#error this file is obselete

#include "Engine.h"
//============================================================================
// ParticleEffect
//============================================================================
TrailEffect::TrailEffect()
:   Entity(Entity::ETypeTrailFx)
{
}
//============================================================================
TrailEffect::~TrailEffect()
{
}
//============================================================================
// TrailSystem
//============================================================================
TrailSystem::TrailSystem()
{
}
//============================================================================
TrailSystem::~TrailSystem()
{
}
//============================================================================
TrailEffect* TrailSystem::CreateTrailEffect(const char *name)
{
	TrailEffect *   effect = TrailEffects.Find(name);
    char            buf[MAX_PATH];


	if (effect)
		return effect;

    m_setTag("Trail");
	effect = new TrailEffect();

    snprintf(buf, MAX_PATH, "../Data/Scripts/Trails/%s", name);
	BOOL loaded = LoadTrailEffect(*effect, buf);

	if (loaded)
		TrailEffects.Insert(name,*effect);

	return TrailEffects.Find(name);
}
//============================================================================
BOOL TrailSystem::LoadTrailEffect(TrailEffect& trail, const char *filename)
{
    ConfigFile cfg;
    if(cfg.Load(filename)) {

        const char* s,*section;
        section="General";
        s=cfg.GetString(section,"Texture");         if(s) trail.TextureName=s;
		s=cfg.GetString(section,"TexAnimFPS");      if(s) trail.TexAnimFPS=atoi(s);
        s=cfg.GetString(section,"Type");            if(s) trail.Type=atoi(s);
        s=cfg.GetString(section,"BlendMode");       if(s) trail.BlendMode=atoi(s);
		s=cfg.GetString(section,"LifeTime");		if(s) trail.LifeTime=atof(s);
		s=cfg.GetString(section,"Width");			if(s) trail.Width=atof(s);

        // fix blend mode
        switch (trail.BlendMode)
        {
        case 64:    trail.BlendMode=RenderState::EBlendFilter; break;
        case 128:   trail.BlendMode=RenderState::EBlendAdd; break;
        case 256:   trail.BlendMode=RenderState::EBlendTranslucent; break;
        case 512:   trail.BlendMode=RenderState::EBlendAlpha; break;
        }

		return TRUE;
	}
	return FALSE;
}
