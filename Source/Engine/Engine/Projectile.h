#error This file is obselete

#ifndef _PROJECTILE_H_
#define _PROJECTILE_H_

enum
{
	EProjectileTimed = 1,
	EProjectileImpact = 2,
	EProjectileNoGravity = 4,
};

class Projectile
{
public:
	DWORD	Type;
	Entity*	MyEntity;

    Projectile() : Type(0),
		MyEntity( NULL ) {};
};

#endif