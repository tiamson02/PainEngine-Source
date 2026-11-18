#include "CHuntState.h"
#include "CAIBrain.h"
#include "Core/Math/GRandom.h"

class Entity;

CHuntState::CHuntState()
	: CAIState( AISTATE_HUNT, CAIState::stateNames[AISTATE_HUNT] )
	, huntTime( 10.f )
	, walk( CAIBrain::NIL_INT )
	, active( false )
	, substate( CAIBrain::NIL_INT )
	, delayTimer( CAIBrain::NIL_INT )
{
}

CHuntState::~CHuntState()
{
}

void CHuntState::onInit(CAIBrain *brain, Script &obj)
{
	ASSERT_AND_RETURN_IF_NOT( brain );

	//local actor = brain._Objactor
	Script actor = CAIBrain::getActor(obj);

	//--if actor.NeverMove then Game:Print("WARNING "..actor._Name.." want HUNT, but nevermove!!!!!!!!") end
	//--Game.freezeUpdate = true
	
	//local aiParams = actor.AiParams
	Script aiParams = CAIBrain::getAiParams(actor);

	// @note: [jaba] It looks like d, here is unused.
	//local d = Dist3D(brain._enemyLastSeenPoint.X, brain._enemyLastSeenPoint.Y, brain._enemyLastSeenPoint.Z, actor._groundx, actor._groundy, actor._groundz)

	//self.walk = 1
	walk = 1;

	//if ENTITY.PO_Exist(actor._Entity) then
	Entity * const entity = CAIBrain::getEntity(actor);
	if (entity)
	{
	//	local b,d,e = actor:Trace(2)
		actor.Call("Trace", 3, "%f", 2.f);

		const int	eid			= actor.GetInt(-3);
		const bool	traceHit	= actor.GetBool(-1);

	//	if b and e then
		if (traceHit && eid)
		{
	//		local obj = EntityToObject[e]
	//		if obj and obj._AIBrain and obj._Class == "CActor" then

// @todo: [jaba] How do we check whether the hit Actor has a 
// brain? Don't want AI aggro-ing on chairs.
// Can we access script-side EntityToObject using script env?

	//			--Game:Print("HUNT!!!!! przede mna stoi koles !!!!")
	//			self.delayTimer = math.random(15,35)
				delayTimer = IRandom(15, 35);

	//		end
	//	end
		}
	//end
	}

	//self.walk = 2
	walk = 2;

	//
	//if not actor._isWalking then
	if ( ! actor["_isWalking"].GetBool() )
	{
	//	actor:RotateToVector(brain._enemyLastSeenPoint.X, brain._enemyLastSeenPoint.Y, brain._enemyLastSeenPoint.Z)
		actor.Call(
			"RotateToVector", 0, 
			"%f %f %f", 
			brain->enemyLastSeenPoint.X, 
			brain->enemyLastSeenPoint.Y, 
			brain->enemyLastSeenPoint.Z);
	}
	//end

	//self.active = true	
	active = true;

	//self.substate = 0
	substate = 0;

	//self.delayTimer = nil
	delayTimer = CAIBrain::NIL_INT;
}

void CHuntState::onUpdate(CAIBrain *brain, Script &obj)
{
	ASSERT_AND_RETURN_IF_NOT( brain );

	//local actor = brain._Objactor
	Script actor = CAIBrain::getActor(obj);

	//local aiParams = actor.AiParams
	Script aiParams = CAIBrain::getAiParams(actor);

	const Vector & enemyLastSeenPoint = brain->enemyLastSeenPoint;
	const Vector & actorPos = CAIBrain::getEntityPos( actor );
	const float enemyDistSquared = enemyLastSeenPoint.DistSquared( actorPos );

	const bool runWhenHunting = ! aiParams["dontRunWhenHunt"].GetBool();
	aiParams.Pop();

	const float attackRange = aiParams["attackRange"].GetFloat();
	aiParams.Pop();

	const bool actorIsRotating = actor["_isRotating"].GetBool();
	actor.Pop();

	const float actorLastCantMoveTime = actor["_lastCantMoveTime"].GetFloat() + 3.f / 30.f;
	actor.Pop();

	//if self.walk then
	if ( walk )
	{
	//	if not actor._isRotating then
		if ( !actorIsRotating )
		{
	//		if self.walk == 2 then
			if ( 2 == walk )
			{
	//			local dist = Dist3D(brain._enemyLastSeenPoint.X, brain._enemyLastSeenPoint.Y, brain._enemyLastSeenPoint.Z, actor._groundx, actor._groundy, actor._groundz)

	//			--if debugMarek then Game:Print(actor._Name.." walk to last seen, dist = "..dist) end
	//			if dist > 2.2 then

				const float distThreshold = 2.2f;
				if (enemyDistSquared > distThreshold * distThreshold)
				{
	//				actor:WalkTo(brain._enemyLastSeenPoint.X, brain._enemyLastSeenPoint.Y, brain._enemyLastSeenPoint.Z, not aiParams.dontRunWhenHunt, nil, nil, true)

					obj.Call(
						actor, 
						"WalkTo", 
						0, 
						"%f %f %f %b %n %n %b", 
						enemyLastSeenPoint.X, enemyLastSeenPoint.Y, enemyLastSeenPoint.Z,
						runWhenHunting,
						true );

	//			else
				}
				else
				{
	//				actor:WalkForward(FRand(2.3, 3.5), false, FRand(-45,45),nil,nil,true)
					
					obj.Call(
						actor, 
						"WalkForward", 
						0, 
						"%f %b %f %n %n %b", 
						FRandom(2.3f, 3.5f),
						false,
						FRandom(-45.f, 45.f),
						true );

	//			end
				}
	//		end
			}

	//		self.walk = nil
			walk = CAIBrain::NIL_INT;
	//	end
		}

	//	return
		return;

	//end
	}

	const bool isWalking = actor["_isWalking"].GetBool();
	actor.Pop();

	//
	//if self.substate == 0 then
	if ( 0 == substate )
	{
	//	if self.delayTimer then
		if ( delayTimer )
		{
	//		self.delayTimer = self.delayTimer - 1
			delayTimer -= 1;

	//		--if debugMarek then Game:Print(actor._Name.." delay timer") end
	//		if self.delayTimer <= 0 then
			if ( delayTimer <= 0 )
			{
	//			self.delayTimer = nil
				delayTimer = CAIBrain::NIL_INT;

	//			local d = Dist3D(brain._enemyLastSeenPoint.X, brain._enemyLastSeenPoint.Y, brain._enemyLastSeenPoint.Z, actor._groundx, actor._groundy, actor._groundz)
	//			if (d > aiParams.attackRange) then

				if (enemyDistSquared > attackRange * attackRange)
				{

	//				--if debugMarek then Game:Print(actor._Name.." still close "..d) end
	//				actor:WalkTo(brain._enemyLastSeenPoint.X + FRand(-2,2), brain._enemyLastSeenPoint.Y, brain._enemyLastSeenPoint.Z + FRand(-2,2), not aiParams.dontRunWhenHunt)
	
					obj.Call(
						actor, 
						"WalkTo", 
						0, 
						"%f %f %f %b %n %n %b", 
						enemyLastSeenPoint.X + FRandom(-2.f, 2.f), 
						enemyLastSeenPoint.Y, 
						enemyLastSeenPoint.Z + FRandom(-2.f, 2.f),
						runWhenHunting );

	//			else
				}
				else
				{
	//				--if debugMarek then Game:Print(actor._Name.." close "..d) end
	//				actor:WalkTo(brain._enemyLastSeenPoint.X, brain._enemyLastSeenPoint.Y, brain._enemyLastSeenPoint.Z, not aiParams.dontRunWhenHunt)

					obj.Call(
						actor, 
						"WalkTo", 
						0, 
						"%f %f %f %b %n %n %b", 
						enemyLastSeenPoint.X, enemyLastSeenPoint.Y, enemyLastSeenPoint.Z,
						runWhenHunting );

	//			end
				}
	//		end
			}
		}
	//	else
		else
		{
	//		if not actor._isWalking --[[and self.active--]] then
			if ( !isWalking )
			{
	//			local d = Dist3D(brain._enemyLastSeenPoint.X, brain._enemyLastSeenPoint.Y, brain._enemyLastSeenPoint.Z, actor._groundx, actor._groundy, actor._groundz)
	//			if d < aiParams.attackRange then

				if ( enemyDistSquared < attackRange * attackRange )
				{
	//				if not actor._isRotating then
					if ( ! actorIsRotating )
					{
	//					if brain._enemyLastSeenVelocitySpeed < 0.1 then
						if ( brain->enemyLastSeenVelocitySpeed )
						{
	//						actor:RotateTo(brain._enemyLastSeenOrientation * 180/3.14)
	//					else
						}
						else
						{
	//						local v = Vector:New(brain._enemyLastSeenVelocity.X, brain._enemyLastSeenVelocity.Y, brain._enemyLastSeenVelocity.Z)
	//						v:Normalize()

							Vector v( brain->enemyLastSeenVelocity );
							v.Normalize();

	//						v.X = v.X + brain._enemyLastSeenPoint.X
	//						v.Y = v.Y + brain._enemyLastSeenPoint.Y
	//						v.Z = v.Z + brain._enemyLastSeenPoint.Z

							v += enemyLastSeenPoint;

	//						actor:RotateToVector(v.X, v.Y, v.Z) 
							actor.Call( "RotateToVector", 0, "%f %f %f", v.X, v.Y, v.Z );
	//					end
						}

	//					self.substate = 1
						substate = 1;

	//					self.delayTimer = math.random(60,95)
						delayTimer = IRandom(60, 95);
	//				end
					}
				}
	//			else

				else
				{
	//				if not aiParams.alwaysSee and not aiParams.huntPlayer then

					const bool aipAlwaysSee = aiParams["alwaysSee"].GetBool();
					aiParams.Pop();

					const bool aipHuntPlayer = aiParams["huntPlayer"].GetBool();
					aiParams.Pop();

					if ( ! aipAlwaysSee && !aipHuntPlayer )
					{
	//					actor:RotateToVector(brain._enemyLastSeenPoint.X, brain._enemyLastSeenPoint.Y, brain._enemyLastSeenPoint.Z)
						actor.Call( 
							"RotateToVector", 
							0, 
							"%f %f %f", 
							enemyLastSeenPoint.X, 
							enemyLastSeenPoint.Y, 
							enemyLastSeenPoint.Z );
	//				end
					}

	//				if brain._currentTime < actor._lastCantMoveTime + 3/30  then
					if (brain->currentTime < actorLastCantMoveTime + 3.f / 30.f)
					{
	//					self.substate = 2
						substate = 2;
	//				else
					}
					else
					{
	//					self.active = false
						active = false;

	//					brain._delayToHunt = math.random(5, 40)
						brain->delayToHunt = IRandom(5, 40);

	//					self.substate = 0
						substate = 0;

	//					self.walk = 2
						walk = 2;
	//					--if debugMarek then Game:Print(actor._Name.." hunt nie dotarl na miejsce "..d) end
	//				end
					}
				}
	//			end
			}
	//		end
		}
	//	end
	//else
	}
	else
	{
		const bool isRotating = actor["_isRotating"].GetBool();
		actor.Pop();

	//	if self.substate == 1 and not actor._isWalking then
		if ( 1 == substate && ! isWalking )
		{
	//		if self.delayTimer then
			if ( delayTimer )
			{
	//			self.delayTimer = self.delayTimer - 1
				delayTimer--;

	//			if self.delayTimer < 0 then
				if ( delayTimer < 0 )
				{
	//				--Game:Print(actor._Name.." koniec obracania sie w hunt")
	//				--
	//				self.active = false
					active = false;

	//				self.substate = 0
					substate = 0;

	//				brain._delayToHunt = math.random(5, 40)
					brain->delayToHunt = IRandom(5, 40);

	//				self.delayTimer = nil
					delayTimer = CAIBrain::NIL_INT;

	//				self.walk = 2
					walk = 2;
	//				--
	//			end
				}
			}
	//		else
			else
			{
	//			--Game:Print(actor._Name.." self acvite false")
	//			--
	//			self.active = false
				active = false;

	//			self.substate = 0
				substate = 0;

	//			self.walk = 2
				walk = 2;

	//			brain._delayToHunt = math.random(5, 40)
				brain->delayToHunt = IRandom(5, 40);
	//			--
	//		end
			}

	//		if not actor._isRotating then
			if ( ! isRotating )
			{
	//			if math.random(100) < 5 then
				if ( gRand->Random() < 0.05 )
				{
	//				actor:Rotate(math.random(-30, 30))
	//			end
				}
	//		end
			}
	//	end
		}

	//	if self.substate == 2 then
		if ( 2 == substate )
		{
	//		if not actor._isRotating then
			if ( !isRotating )
			{
	//			-- try again:
	//			--Game:Print(actor._Name.." try again hunt")
	//			--Game.freezeUpdate = true

	//			actor._lastCantMoveTime = actor._lastCantMoveTime - 1.0		-- fake
				actor.SetFloat( "_lastCantMoveTime", actorLastCantMoveTime - 1.f );

	//			actor:WalkTo(brain._enemyLastSeenPoint.X + FRand(-1,1), brain._enemyLastSeenPoint.Y, brain._enemyLastSeenPoint.Z + FRand(-1,1), not aiParams.dontRunWhenHunt)

				obj.Call(
					actor, 
					"WalkTo", 
					0, 
					"%f %f %f %b %n %n %b", 
					enemyLastSeenPoint.X + FRandom(-1.f, 1.f), 
					enemyLastSeenPoint.Y, 
					enemyLastSeenPoint.Z + FRandom(-1.f, 1.f),
					runWhenHunting );

	//			--[[if debugMarek then
	//				actor._debugW1 = brain._enemyLastSeenPoint.X
	//				actor._debugW2 = brain._enemyLastSeenPoint.Y
	//				actor._debugW3 = brain._enemyLastSeenPoint.Z
	//			end--]]

	//			self.delayTimer = math.random(50,100)
				delayTimer = IRandom(50, 100);

	//			self.substate = 1
				substate = 1;
	//		end
			}
	//	end
		}
	//end
	}
}

void CHuntState::onRelease(CAIBrain *brain, Script &obj)
{
	ASSERT_AND_RETURN_IF_NOT( brain );

	//self.walk = nil
	walk = CAIBrain::NIL_INT;

	//local actor = brain._Objactor
	Script actor = CAIBrain::getActor(obj);

	//local aiParams = actor.AiParams
	Script aiParams = CAIBrain::getAiParams(actor);
	
	//self.active = false
	active = false;

	//self.delayTimer = nil
	delayTimer = CAIBrain::NIL_INT;

	//brain._delayToHunt = math.random(5, 40)
	brain->delayToHunt = IRandom(5, 40);

	//--brain.lastHuntTime = brain._currentTime
	//if not actor._isWalking and not aiParams.alwaysSee then
	if ( !actor["_isWalking"].GetBool() && !aiParams["alwaysSee"].GetBool() )
	{
	//	actor:RotateToVector(brain._enemyLastSeenPoint.X, brain._enemyLastSeenPoint.Y, brain._enemyLastSeenPoint.Z)
		actor.Call(
			"RotateToVector", 0, 
			"%f %f %f", 
			brain->enemyLastSeenPoint.X, 
			brain->enemyLastSeenPoint.Y, 
			brain->enemyLastSeenPoint.Z);
	//end
	}
	//--Game:Print("HUNT release"..brain._currentTime)
}

float CHuntState::evaluate(CAIBrain *brain, Script &obj)
{
	ASSERT_AND_RETURN_VAL_IF_NOT(brain, 0.f);

	//if not brain.r_closestEnemy then		--  and Player.Health > 0
	if ( !brain->r_closestEnemy )
	{
		//	local actor = brain._Objactor
		Script actor = CAIBrain::getActor(obj);

		//	local aiParams = actor.AiParams
		Script aiParams = CAIBrain::getAiParams(actor);

		//	if brain._delayToHunt then
		if ( brain->delayToHunt > CAIBrain::NIL_INT )
		{
		//		brain._delayToHunt = brain._delayToHunt - 1
			brain->delayToHunt = brain->delayToHunt - 1;
		//		--Game:Print(actor._Name.." delay to hunt "..brain._delayToHunt)
		//		if not actor._isWalking then
			if ( ! actor["_isWalking"].GetBool() )
			{
		//			--Game:Print(actor._Name.." delay to hunt stopped")
		//			brain._delayToHunt = -1
				brain->delayToHunt = -1;
		//		end
			}
		//		if brain._delayToHunt < 0 then
			Assert( CAIBrain::NIL_INT < 0 );
			if (brain->delayToHunt < 0)	// [jaba] okay, because NIL_INT is < 0
			{
		//			brain._delayToHunt = nil
				brain->delayToHunt = CAIBrain::NIL_INT;
		//			--Game:Print(actor._Name.." end delay to hunt")
		//		end
			}
		//	end
		}

		//	if not brain._delayToHunt and not aiParams.WalkAreaWhenAttack then
		if ( brain->delayToHunt == CAIBrain::NIL_INT && ! aiParams["WalkAreaWhenAttack"].GetBool() )
		{
			return 0.1f;	
			
			// That's all this code does. No need for complex checks [jaba]
			// I've commented out the rest, but kept it in case we want to 
			// return different values, later.
/*			
		//		if brain._enemyLastSeenTime + self.huntTime > brain._currentTime and brain._distToNearestEnemy > aiParams.attackRange then
			if (brain->enemyLastSeenTime + huntTime > brain->currentTime &&
				brain->distToNearestEnemy > aiParams["attackRange"] )
			{
		//			return 0.1
				return 0.1f;
		//		end
			}
		//		if self.active then
			if (active)
			{
		//			return 0.1
				return 0.1f;
		//		end
			}
		
*/			
		//	end
		}
	//end
	}

	//return 0
	return 0.f;
}
