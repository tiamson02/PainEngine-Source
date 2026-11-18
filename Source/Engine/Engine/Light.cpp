#include "Engine.h"

void Light::Load(FileObject* file)
{
    Entity::Load(file);
    *file >> Color >> Intensity;
}

void Light::Save(FileObject* file)
{
    Entity::Save(file);
    *file << Color << Intensity;
}
