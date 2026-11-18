
inline void Entity::RegisterChild(Entity* child,bool transformByParent,char joint)
{
	child->TransformByParent = transformByParent;
	child->ParentJoint = joint;

	if (child->Parent)
	{
		if (child->Parent == this)
			return;
		child->Parent->UnregisterChild(child);
	}
	child->Parent = this;

	Children.Add(child);
}

inline void Entity::UnregisterChild(Entity* child)
{
	for (int i=0; i<Children.Size(); i++)
		if (Children[i]==child)
		{
            child->OnUnregister();
            child->Parent = NULL;
            Children.Remove(i);
			return;
		}
}

inline void Entity::UnregisterAllChildren( int type )
{
	for (int i=0; i<Children.Size(); i++)
	{
		if( ( type == -1 ) || ( type == Children[i]->Type ) )
		{
           Children[i]->OnUnregister();
           Children[i]->Parent = NULL;
           Children.FastRemove(i);
           i--;
		}
	}
}

inline void Entity::KillAllChildren( int type )
{
    for (int i=0; i<Children.Size(); i++)
    {
        if( ( type == -1 ) || ( type == Children[i]->Type ) )
        {
            Children[i]->OnUnregister();
            Children[i]->Parent = NULL;
            WorldLink->DeleteEntityDelayed(Children[i]);
            Children.FastRemove(i);
            i--;
        }
    }
}

inline void Entity::SetIdentity()
{
	Scale = 1.0f;
	Rotation = IdentityQuaternion();
	Translation = Vector(0,0,0);
	ObjectToWorld.SetIdentity();    // = IdentityMatrix();

	if (WorldLink)
		WorldLink->UpdateEntity(this);
}
