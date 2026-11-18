
#include "HavokPhysics.h"
#include "POHavokPart.h"
#include "HavokWorld.h"
#include "Net/UdpMessage.h"

#include "Net/NetworkDevice2.h"
#define NPROF(x)
#define NPROF2(x,y)

#include "Engine/PCFSystem.h"
extern ENGINE_API PCFSystem* GEngine;

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

POHavokPart::POHavokPart( void )
:	Body(0)
,	CollisionListener(0)
,	StepCheckPhantom(0)
,	World(0)
,	Actions(0)
{}

POHavokPart::~POHavokPart( void )
{
	if( Body )
		Body->removeReference();
	Body = 0;
}

havokAction* POHavokPart::GetAction( int type )
{
	ActionLink* action = Actions;
	while( action )
	{
		if( type == action->Action->getType() )
			return action->Action;
		action = action->Next;
	}
	return 0;
}

void POHavokPart::PutAction( havokAction* newAction )
{
	ActionLink* action = Actions;
	ActionLink* lastAction = 0;
	int type = newAction->getType();
	while( action )
	{
		if( type == action->Action->getType() )
		{
			if( action->Action->getSimulationIsland() && Body && Body->getSimulationIsland() )
				World->World->removeAction( action->Action );
			action->Action->removeReference();

			if( Body->getSimulationIsland() )
				World->World->addAction( newAction );
			action->Action = newAction;
			return;
		}
		lastAction = action;
		action = action->Next;
	}

	if( lastAction )
	{
        m_setTag("Physics");
		lastAction->Next = new ActionLink;
		if( Body->getSimulationIsland() )
			World->World->addAction( newAction );
		lastAction->Next->Action = newAction;
		lastAction->Next->Next = 0;
	}
	else
	{
        m_setTag("Physics");
		Actions = new ActionLink;
		if( Body->getSimulationIsland() )
			World->World->addAction( newAction );
		Actions->Action = newAction;
		Actions->Next = 0;
	}
}

void POHavokPart::ReconnectActionsToWorld( void )
{
	ActionLink* action = Actions;
	for( ; action; action = action->Next )
		if( !action->Action->getSimulationIsland() )
			World->World->addAction( action->Action );
}

int POHavokPart::NumberOfActions( void )
{
	int count = 0;
	ActionLink* action = Actions;
	for( ; action; action = action->Next )
		count++;
	return count;
}

void POHavokPart::RemoveAction( int type )
{
	ActionLink* action = Actions;
	ActionLink* lastAction = 0;
	for( ; action; lastAction = action, action = action->Next )
		if( type == action->Action->getType() )
		{
			if( action->Action->getSimulationIsland() && Body && Body->getSimulationIsland() )
				World->World->removeAction( action->Action );
			action->Action->removeReference();
			if( lastAction )
				lastAction->Next = action->Next;
			else
				Actions = action->Next;
			delete action;
			return;
		}
}

void POHavokPart::RemoveAllActionsForBodyDelete( void )
{
	ActionLink* action = Actions;
	while( action )
	{
		ActionLink* temp = action;
		action = action->Next;
		temp->Action->removeReference();
		delete temp;
	}
	Actions = 0;
}

void POHavokPart::WriteAction( int type, UdpMessage* message )
{
	havokAction* action = GetAction( type );
	message->WriteUChar( (unsigned char)type ); NPROF2(NPPOActionData,1);
	if( !action )
	{
		message->WriteBool( false ); NPROF2(NPPOActionData,1);
	}
	else
	{
		message->WriteBool( true ); NPROF2(NPPOActionData,1);
		switch( type )
		{
		case EActionAntiGravity:
			{
				havokAntiGravityAction* hkaction = (havokAntiGravityAction*)action;
				hkVector4& grav = hkaction->getGravity();
				message->WriteFloat( grav(0) ); NPROF2(NPPOActionData,4);
				message->WriteFloat( grav(1) ); NPROF2(NPPOActionData,4);
				message->WriteFloat( grav(2) ); NPROF2(NPPOActionData,4);
			}
			return;
		case EActionSpeedDamping:
			{
				havokSpeedDampingAction* hkaction = (havokSpeedDampingAction*)action;
				float f0, f1, f2;
				hkaction->getInfo( f0, f1, f2 );
				message->WriteFloat( f0 ); NPROF2(NPPOActionData,4);
				message->WriteFloat( f1 ); NPROF2(NPPOActionData,4);
				message->WriteFloat( f2 ); NPROF2(NPPOActionData,4);
			}
			return;
		case EActionMaintainLinearMovement:
			{
				havokMaintainLinearMovementAction* hkaction = (havokMaintainLinearMovementAction*)action;
				hkVector4 axis;
				hkVector4 position;
				bool soft;
				hkaction->getInfo( axis, position, soft );
				message->WriteFloat( axis(0) ); NPROF2(NPPOActionData,4);
				message->WriteFloat( axis(1) ); NPROF2(NPPOActionData,4);
				message->WriteFloat( axis(2) ); NPROF2(NPPOActionData,4);
				message->WriteFloat( position(0) ); NPROF2(NPPOActionData,4);
				message->WriteFloat( position(1) ); NPROF2(NPPOActionData,4);
				message->WriteFloat( position(2) ); NPROF2(NPPOActionData,4);
				message->WriteBool( soft ); NPROF2(NPPOActionData,1);
			}
			return;
		case EActionMaintainPosition:
			{
				havokMaintainPositionAction* hkaction = (havokMaintainPositionAction*)action;
				hkVector4 position;
				float strength;
				hkaction->getInfo( position, strength );
				message->WriteFloat( position(0) ); NPROF2(NPPOActionData,4);
				message->WriteFloat( position(1) ); NPROF2(NPPOActionData,4);
				message->WriteFloat( position(2) ); NPROF2(NPPOActionData,4);
				message->WriteFloat( strength ); NPROF2(NPPOActionData,4);
			}
			return;
		case EActionMaintainVelocity:
			{
				havokMaintainVelocityAction* hkaction = (havokMaintainVelocityAction*)action;
				hkVector4 velocity;
				float strength;
				hkaction->getInfo( velocity, strength );
				message->WriteFloat( velocity(0) ); NPROF2(NPPOActionData,4);
				message->WriteFloat( velocity(1) ); NPROF2(NPPOActionData,4);
				message->WriteFloat( velocity(2) ); NPROF2(NPPOActionData,4);
				message->WriteFloat( strength ); NPROF2(NPPOActionData,4);
			}
			return;
		case EActionMaintainAngularVelocity:
			{
				havokMaintainAngularVelocityAction* hkaction = (havokMaintainAngularVelocityAction*)action;
				hkVector4 angVelocity;
				float strength;
				hkaction->getInfo( angVelocity, strength );
				message->WriteFloat( angVelocity(0) ); NPROF2(NPPOActionData,4);
				message->WriteFloat( angVelocity(1) ); NPROF2(NPPOActionData,4);
				message->WriteFloat( angVelocity(2) ); NPROF2(NPPOActionData,4);
				message->WriteFloat( strength ); NPROF2(NPPOActionData,4);
			}
			return;
		case EActionTransporter:
			{
				havokTransporterAction* hkaction = (havokTransporterAction*)action;
				hkVector4 velocity;
				float strength;
				hkaction->getInfo( velocity, strength ); NPROF2(NPPOActionData,4);
				message->WriteFloat( velocity(0) ); NPROF2(NPPOActionData,4);
				message->WriteFloat( velocity(1) ); NPROF2(NPPOActionData,4);
				message->WriteFloat( velocity(2) ); NPROF2(NPPOActionData,4);
				message->WriteFloat( strength ); NPROF2(NPPOActionData,4);
			}
			return;
		default:
			Assert( type == EActionTransporter );	// this will fire
			return;
		}
	}
}

void POHavokPart::ReadAction( UdpMessage* message )
{
	int type = (int)message->ReadUChar();
	bool exists = message->ReadBool();

	havokAction* action = GetAction( type );
	if( exists )
	{
		if( action )
		{
			// We have to change it
			switch( type )
			{
			case EActionAntiGravity:
				{
					float f0 = message->ReadFloat();
					float f1 = message->ReadFloat();
					float f2 = message->ReadFloat();
					hkVector4 gravityToSet( f0, f1, f2 );
					havokAntiGravityAction* hkaction = (havokAntiGravityAction*)action;
					hkaction->setGravity( gravityToSet );
				}
				return;
			case EActionSpeedDamping:
				{
					float f0 = message->ReadFloat();
					float f1 = message->ReadFloat();
					float f2 = message->ReadFloat();
					havokSpeedDampingAction* hkaction = (havokSpeedDampingAction*)action;
					hkaction->setInfo( f0, f1, f2 );
				}
				return;
			case EActionMaintainLinearMovement:
				{
					hkVector4 axis;
					hkVector4 position;
					bool soft;
					axis(0) = message->ReadFloat();
					axis(1) = message->ReadFloat();
					axis(2) = message->ReadFloat();
					position(0) = message->ReadFloat();
					position(1) = message->ReadFloat();
					position(2) = message->ReadFloat();
					soft = message->ReadBool();
					havokMaintainLinearMovementAction* hkaction = (havokMaintainLinearMovementAction*)action;
					hkaction->setInfo( axis, position, soft );
				}
				return;
			case EActionMaintainPosition:
				{
					hkVector4 position;
					float strength;
					position(0) = message->ReadFloat();
					position(1) = message->ReadFloat();
					position(2) = message->ReadFloat();
					strength = message->ReadFloat();
					havokMaintainPositionAction* hkaction = (havokMaintainPositionAction*)action;
					hkaction->setInfo( position, strength );
				}
				return;
			case EActionMaintainVelocity:
				{
					hkVector4 velocity;
					float strength;
					velocity(0) = message->ReadFloat();
					velocity(1) = message->ReadFloat();
					velocity(2) = message->ReadFloat();
					strength = message->ReadFloat();
					havokMaintainVelocityAction* hkaction = (havokMaintainVelocityAction*)action;
					hkaction->setInfo( velocity, strength );
				}
				return;
			case EActionMaintainAngularVelocity:
				{
					hkVector4 angVelocity;
					float strength;
					angVelocity(0) = message->ReadFloat();
					angVelocity(1) = message->ReadFloat();
					angVelocity(2) = message->ReadFloat();
					strength = message->ReadFloat();
					havokMaintainAngularVelocityAction* hkaction = (havokMaintainAngularVelocityAction*)action;
					hkaction->setInfo( angVelocity, strength );
				}
				return;
			case EActionTransporter:
				{
					hkVector4 velocity;
					float strength;
					velocity(0) = message->ReadFloat();
					velocity(1) = message->ReadFloat();
					velocity(2) = message->ReadFloat();
					strength = message->ReadFloat();
					havokTransporterAction* hkaction = (havokTransporterAction*)action;
					hkaction->setInfo( velocity, strength );
				}
				return;
			default:
				Assert( type == EActionTransporter );	// this will fire
				return;
			}
		}
		else
		{
			// We have to add it
			switch( type )
			{
			case EActionAntiGravity:
				{
					float f0 = message->ReadFloat();
					float f1 = message->ReadFloat();
					float f2 = message->ReadFloat();
					hkVector4 gravityToSet( f0, f1, f2 );
                    m_setTag("Physics");
					havokAntiGravityAction* hkaction = new havokAntiGravityAction( Body, gravityToSet );
					PutAction( hkaction );
				}
				return;
			case EActionSpeedDamping:
				{
					float f0 = message->ReadFloat();
					float f1 = message->ReadFloat();
					float f2 = message->ReadFloat();
                    m_setTag("Physics");
					havokSpeedDampingAction* hkaction = new havokSpeedDampingAction( Body, f0, f1, f2 );
					PutAction( hkaction );
				}
				return;
			case EActionMaintainLinearMovement:
				{
					hkVector4 axis;
					hkVector4 position;
					bool soft;
					axis(0) = message->ReadFloat();
					axis(1) = message->ReadFloat();
					axis(2) = message->ReadFloat();
					position(0) = message->ReadFloat();
					position(1) = message->ReadFloat();
					position(2) = message->ReadFloat();
					soft = message->ReadBool();
                    m_setTag("Physics");
					havokMaintainLinearMovementAction* hkaction = new havokMaintainLinearMovementAction( Body, axis, soft );
					hkaction->setInfo( axis, position, soft );
					PutAction( hkaction );
				}
				return;
			case EActionMaintainPosition:
				{
					hkVector4 position;
					float strength;
					position(0) = message->ReadFloat();
					position(1) = message->ReadFloat();
					position(2) = message->ReadFloat();
					strength = message->ReadFloat();
                    m_setTag("Physics");
					havokMaintainPositionAction* hkaction = new havokMaintainPositionAction( Body, position, strength );
					PutAction( hkaction );
				}
				return;
			case EActionMaintainVelocity:
				{
					hkVector4 velocity;
					float strength;
					velocity(0) = message->ReadFloat();
					velocity(1) = message->ReadFloat();
					velocity(2) = message->ReadFloat();
					strength = message->ReadFloat();
                    m_setTag("Physics");
					havokMaintainVelocityAction* hkaction = new havokMaintainVelocityAction( Body, velocity, strength );
					PutAction( hkaction );
				}
				return;
			case EActionMaintainAngularVelocity:
				{
					hkVector4 angVelocity;
					float strength;
					angVelocity(0) = message->ReadFloat();
					angVelocity(1) = message->ReadFloat();
					angVelocity(2) = message->ReadFloat();
					strength = message->ReadFloat();
                    m_setTag("Physics");
					havokMaintainAngularVelocityAction* hkaction = new havokMaintainAngularVelocityAction( Body, angVelocity, strength );
					PutAction( hkaction );
				}
				return;
			case EActionTransporter:
				{
					hkVector4 velocity;
					float strength;
					velocity(0) = message->ReadFloat();
					velocity(1) = message->ReadFloat();
					velocity(2) = message->ReadFloat();
					strength = message->ReadFloat();
                    m_setTag("Physics");
					havokTransporterAction* hkaction = new havokTransporterAction( Body, velocity, strength );
					PutAction( hkaction );
				}
				return;
			default:
				Assert( type == EActionTransporter );	// this will fire
				return;
			}
		}
	}
	else			// We have to remove it
		RemoveAction( type );
}

#define POHAVOKPART_LOAD_SAVE_TOKEN 0x5e51

bool POHavokPart::LoadActions( FileObject* f )
{
	if( !f->TestToken( POHAVOKPART_LOAD_SAVE_TOKEN ) )
	{
		GLog.Print( "Token test failure: POHavokPart::LoadActions()\n" );
		return false;
	}

	RemoveAllActionsForBodyDelete();

	// Prepare collision listener if we have one
	unsigned char test;
	*f >> test;

	if( CollisionListener )
	{
		Body->removeCollisionListener( CollisionListener );
		delete CollisionListener;
		CollisionListener = 0;
	}

	if( test )	// a collision listener existed
	{
		Body->setProcessContactCallbackDelay(0);
		float minDelay, minStrength, minMass;
		*f >> minDelay >> minStrength >> minMass;
		//Assert( !CollisionListener ); XXX - Sokol (wysypywalo sie na trainstation - chyba obiekty maja domyslnie wlaczana kolizje na mapie)
        TRACE("POHavokPart::LoadActions(): creating collision listener with a delay of %g\n", minDelay);
        m_setTag("Physics");
		CollisionListener = new havokCollisionListener( Body, minDelay, minStrength, minMass );
	}

	int count;
	char type;
	*f >> count;

	for( int i = 0; i < count; ++i )
	{
		*f >> type;
		switch( type )
		{
		case EActionAntiGravity:
			{
				float f0, f1, f2;
				*f >> f0 >> f1 >> f2;
				hkVector4 gravityToSet( f0, f1, f2 );
                m_setTag("Physics");
				havokAntiGravityAction* hkaction = new havokAntiGravityAction( Body, gravityToSet );
				PutAction( hkaction );
			}
			break;
		case EActionSpeedDamping:
			{
				float f0, f1, f2;
				*f >> f0 >> f1 >> f2;
                m_setTag("Physics");
				havokSpeedDampingAction* hkaction = new havokSpeedDampingAction( Body, f0, f1, f2 );
				PutAction( hkaction );
			}
			break;
		case EActionMaintainLinearMovement:
			{
				float a0, a1, a2;
				*f >> a0 >> a1 >> a2;
				hkVector4 axis(a0,a1,a2);
				float p0, p1, p2;
				*f >> p0 >> p1 >> p2;
				hkVector4 position(p0,p1,p2);
				char soft;
				*f >> soft;
                m_setTag("Physics");
				havokMaintainLinearMovementAction* hkaction = new havokMaintainLinearMovementAction( Body, axis, (soft!=0) );
				hkaction->setInfo( axis, position, (soft!=0) );
				PutAction( hkaction );
			}
			break;
		case EActionMaintainPosition:
			{
				float p0, p1, p2;
				*f >> p0 >> p1 >> p2;
				hkVector4 position(p0,p1,p2);
				float strength;
				*f >> strength;
                m_setTag("Physics");
				havokMaintainPositionAction* hkaction = new havokMaintainPositionAction( Body, position, strength );
				PutAction( hkaction );
			}
			break;
		case EActionMaintainVelocity:
			{
				float v0, v1, v2;
				*f >> v0 >> v1 >> v2;
				hkVector4 velocity(v0,v1,v2);
				float strength;
				*f >> strength;
                m_setTag("Physics");
				havokMaintainVelocityAction* hkaction = new havokMaintainVelocityAction( Body, velocity, strength );
				PutAction( hkaction );
			}
			break;
		case EActionMaintainAngularVelocity:
			{
				float av0, av1, av2;
				*f >> av0 >> av1 >> av2;
				hkVector4 angVelocity(av0,av1,av2);
				float strength;
				*f >> strength;
                m_setTag("Physics");
				havokMaintainAngularVelocityAction* hkaction = new havokMaintainAngularVelocityAction( Body, angVelocity, strength );
				PutAction( hkaction );
			}
			break;
		case EActionTransporter:
			{
				float v0, v1, v2;
				*f >> v0 >> v1 >> v2;
				hkVector4 velocity(v0,v1,v2);
				float strength;
				*f >> strength;
                m_setTag("Physics");
				havokTransporterAction* hkaction = new havokTransporterAction( Body, velocity, strength );
				PutAction( hkaction );
			}
			break;
		default:
			Assert( type == EActionTransporter );	// this will fire an assert
			return false;
		}
	}
	return true;
}

bool POHavokPart::SaveActions( FileObject* f )
{
	if( !f->PutToken( POHAVOKPART_LOAD_SAVE_TOKEN ) ) return false;

	// Transfer collision listener
	unsigned char test = ( CollisionListener != 0 )?1:0;
	*f << test;
	if( test )
	{
		float minDelay, minStrength, minMass;
		CollisionListener->getInfo( minDelay, minStrength, minMass );
		*f << minDelay << minStrength << minMass;
	}

	// Determine count
	int count = NumberOfActions();
	*f << count;

	havokAction* action;
	char type;

	action = GetAction( EActionAntiGravity );
	if( action )
	{
		havokAntiGravityAction* hkaction = (havokAntiGravityAction*)action;
		hkVector4& grav = hkaction->getGravity();
		type = EActionAntiGravity;
		float f0 = grav(0), f1 = grav(1), f2 = grav(2);

		*f << type << f0 << f1 << f2;
		count--;
	}

	action = GetAction( EActionSpeedDamping );
	if( action )
	{
		havokSpeedDampingAction* hkaction = (havokSpeedDampingAction*)action;
		float f0, f1, f2;
		hkaction->getInfo( f0, f1, f2 );
		type = EActionSpeedDamping;

		*f << type << f0 << f1 << f2;
		count--;
	}

	action = GetAction( EActionMaintainLinearMovement );
	if( action )
	{
		havokMaintainLinearMovementAction* hkaction = (havokMaintainLinearMovementAction*)action;
		hkVector4 axis;
		hkVector4 position;
		bool soft;
		hkaction->getInfo( axis, position, soft );
		type = EActionMaintainLinearMovement;
		float a0 = axis(0), a1 = axis(1), a2 = axis(2);
		float p0 = position(0), p1 = position(1), p2 = position(2);

		*f << type << a0 << a1 << a2 << p0 << p1 << p2 << soft;
		count--;
	}

	action = GetAction( EActionMaintainPosition );
	if( action )
	{
		havokMaintainPositionAction* hkaction = (havokMaintainPositionAction*)action;
		hkVector4 position;
		float strength;
		hkaction->getInfo( position, strength );
		type = EActionMaintainPosition;
		float p0 = position(0), p1 = position(1), p2 = position(2);

		*f << type << p0 << p1 << p2 << strength;
		count--;
	}

	action = GetAction( EActionMaintainVelocity );
	if( action )
	{
		havokMaintainVelocityAction* hkaction = (havokMaintainVelocityAction*)action;
		hkVector4 velocity;
		float strength;
		hkaction->getInfo( velocity, strength );
		type = EActionMaintainVelocity;
		float v0 = velocity(0), v1 = velocity(1), v2 = velocity(2);

		*f << type << v0 << v1 << v2 << strength;
		count--;
	}

	action = GetAction( EActionMaintainAngularVelocity );
	if( action )
	{
		havokMaintainAngularVelocityAction* hkaction = (havokMaintainAngularVelocityAction*)action;
		hkVector4 angVelocity;
		float strength;
		hkaction->getInfo( angVelocity, strength );
		type = EActionMaintainAngularVelocity;
		float av0 = angVelocity(0), av1 = angVelocity(1), av2 = angVelocity(2);

		*f << type << av0 << av1 << av2 << strength;
		count--;
	}

	action = GetAction( EActionTransporter );
	if( action )
	{
		havokTransporterAction* hkaction = (havokTransporterAction*)action;
		hkVector4 velocity;
		float strength;
		hkaction->getInfo( velocity, strength );
		type = EActionTransporter;
		float v0 = velocity(0), v1 = velocity(1), v2 = velocity(2);

		*f << type << v0 << v1 << v2 << strength;
		count--;
	}

	Assert( !count );
	return !count;
}
