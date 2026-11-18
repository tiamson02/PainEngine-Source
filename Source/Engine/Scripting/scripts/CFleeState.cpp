#include "CFleeState.h"
#include "CAIBrain.h"
#include "Core/Math/GRandom.h"

CFleeState::CFleeState()
	: timer(CAIBrain::NIL_FLOAT)
	, rot(false)
{
	type = AISTATE_FLEE;
	name = CAIState::stateNames[type];
}

CFleeState::~CFleeState()
{
}

void CFleeState::onInit(CAIBrain *brain, Script & obj)
{
	ASSERT_AND_RETURN_IF_NOT( brain );

	//self.timer = brain.escape
	timer = brain->escape;

	//local actor = brain._Objactor
    Script actor = CAIBrain::getActor(obj);
	
	//local angle = math.random(100,260)
	float angle = IRandom(100, 260);

	//if not actor:WalkForward(math.random(14,24), true, angle, nil, nil, true) then
	obj.Call(actor, "WalkForward", 1, "%f %b %f %n %n %b", IRandom(14, 24), true, angle, true);
	if (!obj.GetBool())
	{
		//	angle = math.random(80,130)
		angle = IRandom(80, 130);

		//	if math.random(100) < 50 then
		if (gRand->Random() < 0.5f)
		{
		//		angle = -angle
			angle = -angle;
		}
		//	end
		//	--Game:Print("WP flee failed")

		//	actor:WalkForward(math.random(8,16), true, angle)
		obj.Call(actor, "WalkForward", 1, "%d %b %d", IRandom(8, 16), true, angle);
	}
	//else
	//	--Game:Print("WP flee OK")
	//end
	//--if debugMarek then Game:Print("flee ") end

	//self.rot =  false
	rot = false;
}

void CFleeState::onUpdate(CAIBrain *brain, Script & obj)
{
	ASSERT_AND_RETURN_IF_NOT(brain);

	//local actor = brain._Objactor
	Script actor = CAIBrain::getActor(obj);

	//if not actor._isWalking and not self.rot then
	const bool actorIsWalking = actor["_isWalking"].GetBool();
	if (! actorIsWalking && ! rot)
	{
	//	Patrol(actor, brain, true, true)
		brain->patrol(actor, true, true);

	//end
	}

	//if not actor._isWalking and brain._currentTime < actor._lastCantMoveTime + 2/30 then
	if ( ! actorIsWalking && brain->currentTime < actor["_lastCantMoveTime"].GetFloat() + 2.f / 30.f )
	{
	//	--if debugMarek then Game:Print("COLLISION while flee "..brain._currentTime..", last cm time = "..actor._lastCantMoveTime) end
	//	--if math.random(100) < 66 then
	//		self.rot = true
		rot = true;
	//	--end
	//end
	}
	//
	//if self.timer then
	if (timer > CAIBrain::NIL_FLOAT)
	{
	//	if self.timer > 0 then
		if (timer > 0)
		{
	//		self.timer = self.timer - 1
			timer--;
		}
	//	else
		else
		{
	//		self.timer = nil
			timer = CAIBrain::NIL_FLOAT;
	
	//		self.rot = true
			rot = true;

	//		actor:Stop()
			actor.Call("Stop", 0, NULL);

	//		--Game:Print("flee stop")
	//	end
		}
	//end
	}

	//if self.rot == true then
	if (rot)
	{
	//	actor:Rotate(math.random(135,235))
		actor.Call("Rotate", 0, "%d", IRandom(135, 235));

	//	brain.escape = false
		brain->escape = false;

	//	if brain._enemyLastSeenTime > 0 then
		if (brain->enemyLastSeenTime > 0)
		{
	//		brain._enemyLastSeenTime = brain._currentTime - 1			-- sztucznie, zeby robil hunt
			brain->enemyLastSeenTime = brain->currentTime - 1;
	//	end
		}
	//end
	}
}

void CFleeState::onRelease(CAIBrain *brain, Script & obj)
{
	ASSERT_AND_RETURN_IF_NOT(brain);
}

float CFleeState::evaluate(CAIBrain *brain, Script &obj)
{
	ASSERT_AND_RETURN_VAL_IF_NOT(brain, 0.f);

	//if brain.escape then
	if (brain->escape)
	{
	//	return 0.6666
		return 0.6666f;
	//end
	}
	//return 0.0
	return 0.f;
}
