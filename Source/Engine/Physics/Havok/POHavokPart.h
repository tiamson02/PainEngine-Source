
// #pragma once
#ifndef _PCHAVOKPART_
#define _PCHAVOKPART_


#include "Physics/Havok/HavokPhysics.h"		// for havokCollisionListener

class hkRigidBody;
class havokCollisionListener;

class FileObject;
class havokAction;
class havokStepCheckPhantom;
class HavokWorld;
class UdpMessage;

struct ActionLink
{
	havokAction* Action;
	ActionLink* Next;
};

class POHavokPart
{
public:
	enum ActionType
	{
		EActionAntiGravity					= 0,
		EActionSpeedDamping					= 1,
		EActionMaintainLinearMovement		= 2,
		EActionMaintainPosition				= 3,
		EActionMaintainVelocity				= 4,
		EActionMaintainAngularVelocity		= 5,
		// empty number
		EActionTransporter					= 7,

		EActionLast							= 8		// this HAS to be the last one; it's not used
	};

	hkRigidBody*				Body;
	havokCollisionListener*		CollisionListener;
	havokStepCheckPhantom*		StepCheckPhantom;
	HavokWorld*					World;

	POHavokPart( void );
	~POHavokPart( void );

	havokAction* GetAction( int type );
	void PutAction( havokAction* newAction );
	void ReconnectActionsToWorld( void );
	void RemoveAction( int type );
	void RemoveAllActionsForBodyDelete( void );
	void WriteAction( int type, UdpMessage* message );
	void ReadAction( UdpMessage* message );
	int NumberOfActions( void );

	bool LoadActions( FileObject* f );
	bool SaveActions( FileObject* f );
protected:
	ActionLink*					Actions;
};



#endif //_PCHAVOKPART_

