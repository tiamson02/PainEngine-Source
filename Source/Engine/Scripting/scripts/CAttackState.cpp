#include "CAttackState.h"
#include "CAIBrain.h"
#include "Core/Math/GRandom.h"

CAttackState::CAttackState()
	: CAIState( AISTATE_ATTACK, CAIState::stateNames[ AISTATE_ATTACK ])
	, displacementTimer( 0.f )
	, lastTimeOnAttack( 0.f )
    , lastTimeOnAttackAmbient( 0.f )
	, attackDelay( CAIBrain::NIL_INT )
	, isFirstSeeAnim( false )
	, isFirstTime( false )
	, lastTimeWalkTo( CAIBrain::NIL_FLOAT )
//	, nameCurrAnim(0)
	, lastAttackAmbient( CAIBrain::NIL_INT )
	, delay( CAIBrain::NIL_FLOAT )
	, delayStart( CAIBrain::NIL_INT )
{
}

CAttackState::~CAttackState()
{
}

void CAttackState::onInit(CAIBrain *brain, Script &obj)
{
	ASSERT_AND_RETURN_IF_NOT( brain );

	//local actor = brain._Objactor
	//local aiParams = actor.AiParams

	Script actor = CAIBrain::getActor(obj);
	Script aiParams = CAIBrain::getAiParams(actor);

    Script subclass = actor["s_SubClass"];
    Script soundsDef = subclass["SoundsDefinitions"];

	//brain._delayToHunt = nil
	brain->delayToHunt = CAIBrain::NIL_INT;

	//self.displacementTimer = 0
	displacementTimer = 0;

	//actor._displacement = 0
	actor.SetInt("_displacement", 0);

	//self.attackDelay = nil
	attackDelay = CAIBrain::NIL_INT;

	//if aiParams.NearAttack_Priority and aiParams.NearAttack_Priority > 0.0 then

	const float aipNearAttack_Priority = aiParams["NearAttack_Priority"].GetFloat();
	aiParams.Pop();

	if ( aipNearAttack_Priority > 0.f )
	{
	//	brain.FarAttack_Prio = 0.0
		brain->FarAttack_Prio = 0.f;
	//end
	}

	//
	//self.firstSeeAnim = nil
	isFirstSeeAnim = false;

	//
	//if not self.firstTime then
	if ( ! isFirstSeeAnim )
	{
	//	if actor.s_SubClass.SoundsDefinitions.onAttackOnce then
		Script soundsDef_onAttackOnce = soundsDef["onAttackOnce"];
		if ( soundsDef_onAttackOnce.IsValid() )
		{
	//		actor:PlaySoundAndStopLast("onAttackOnce", nil, nil, true)
			obj.Call( 
				actor,
				"PlaySoundAndStopLast",
				0,
				"%s %n %n %b",
				"onAttackOnce",
				true);
	//	end
		}

	//	
	//	if aiParams.AnimOnSawEnemy then
		{
			const char * nameAnimOnSawEnemy = aiParams["AnimOnSawEnemy"].GetString();
			if ( nameAnimOnSawEnemy && strlen( nameAnimOnSawEnemy ))
			{
		//		local czyZtylu = math.atan2(brain.r_closestEnemy._groundx - actor._groundx, brain.r_closestEnemy._groundz - actor._groundz)
		//		local angleDist2 = AngDist(actor.angle, czyZtylu) * 180/math.pi
		//		if math.abs(angleDist2) < aiParams.viewAngle*0.5 then
		//			actor:RotateToVector(brain.r_closestEnemy._groundx, brain.r_closestEnemy._groundy, brain.r_closestEnemy._groundz)
		//			actor:Stop()
		//			actor:SetAnim(aiParams.AnimOnSawEnemy, false, FRand(0.9, 1.1))
		//			self._CurrAnim = aiParams.AnimOnSawEnemy
		//			self.firstSeeAnim = true
		//		end
		//	end
			}
			aiParams.Pop();	// nameAnimOnSawEnemy could now become invalid! Hence the extra scope.
		}

	//	self.firstTime = true
		isFirstTime = true;
	//else
	}
	else
	{
	//    if actor.s_SubClass.SoundsDefinitions.onAttack then
		Script soundsDef_onAttack = soundsDef["onAttack"];
		if ( soundsDef_onAttack.IsValid() )
		{
	//		if self.lastTimeOnAttack + 3 < brain._currentTime then
			if ( lastTimeOnAttack + 3.f < brain->currentTime )
			{
	//			self.lastTimeOnAttack = brain._currentTime
				lastTimeOnAttack = brain->currentTime;

	//			actor:PlaySoundAndStopLast("onAttack", nil, nil, true)
				obj.Call( 
					actor,
					"PlaySoundAndStopLast",
					0,
					"%s %n %n %b",
					"onAttackOnce",
					true);
	//		end
			}
	//	end
		}
	//end
	}

 //   self.lastTimeWalkTo = -100
	lastTimeWalkTo = CAIBrain::NIL_FLOAT;
}

static bool IsState( Script & actor, const char * stateName )
{
	const bool result = (0 == strcasecmp(stateName, actor["_state"].GetString()) );
	actor.Pop();
	return result;
}

void SetState( Script & actor, const char * stateName )
{
	actor.SetString("_state", stateName);
}

static bool IsWalking( Script & actor )
{
	const bool result = actor["_isWalking"].GetBool();
	actor.Pop();
	return result;
}

static bool IsAnimating( Script & actor )
{
	const bool result = actor["_isAnimating"].GetBool();
	actor.Pop();
	return result;
}

static bool IsRotatingWithAnim( Script & actor )
{
	const bool result = actor["_rotatingWithAnim"].GetBool();
	actor.Pop();
	return result;
}

void CAttackState::onUpdate(CAIBrain *brain, Script &obj)
{
	ASSERT_AND_RETURN_IF_NOT( brain );

	const char * stateAttacking = "ATTACKING";
	const char * stateIdle = "IDLE";

	//local actor = brain._Objactor
	Script actor = CAIBrain::getActor(obj);
	const int actorStackTop = actor.GetTop();

	const bool actorIsRotating = actor["_isRotating"].GetBool();
	const float actorSphereSize = actor["_SphereSize"].GetFloat();
	const Vector actorPos = CAIBrain::getEntityPos(actor);
	bool actorHasMovingCurve = actor["_HasMovingCurve"].GetBool();

	actor.SetTop( actorStackTop );

    Script subclass = actor["s_SubClass"];
    Script soundsDef = subclass["SoundsDefinitions"];

	//local aiParams = actor.AiParams
	Script aiParams = CAIBrain::getAiParams(actor);
	const int aipStackTop = aiParams.GetTop();

	const float aipAttackRange = aiParams["attackRange"].GetFloat();
	const bool aipContinuousAttack  = aiParams["continousAttack"].GetBool();

	aiParams.SetTop( aipStackTop );

//	if self.firstSeeAnim then
	if ( isFirstSeeAnim ) 
	{
		const char * actorAnimation = actor["Animation"].GetString();

//		if actor._isAnimating and actor.Animation == self._CurrAnim then
		if ( IsAnimating(actor) && nameCurrAnim == actorAnimation ) 
		{
//			return
			return;
		} 
		else 
		{
//			self.firstSeeAnim = nil
			isFirstSeeAnim = false;

//			self._CurrAnim = nil
			nameCurrAnim = "";
		}
		actor.Pop();
	}

//	if brain.r_closestEnemy then
	if ( brain->r_closestEnemy ) 
	{
		const Vector closestEnemyPos = CAIBrain::getEntityPos( brain->r_closestEnemy );

//		brain._delayBeforeHunt = 15
		brain->delayBeforeHunt = 15;

//		if aiParams.FarAttack_Priority then
		const float aipFarAttack_Priority = aiParams["FarAttack_Priority"].GetFloat();
		aiParams.Pop();

		if ( aipFarAttack_Priority ) 
		{
//			brain.FarAttack_Prio = brain.FarAttack_Prio + aiParams.FarAttack_Priority * FRand(0.005, 0.01)
			brain->FarAttack_Prio = brain->FarAttack_Prio + aipFarAttack_Priority * FRandom(0.005, 0.01);

//			if brain.FarAttack_Prio > 1.0 then
			if ( brain->FarAttack_Prio > 1.f ) {

//				brain.FarAttack_Prio = 1.0
				brain->FarAttack_Prio = 1.f;
			}
		}

//		self.displacementTimer = self.displacementTimer + 0.017
		displacementTimer += 0.017f;

//		if self.displacementTimer > 1 then
		if ( displacementTimer > 1.f ) 
		{
//			self.displacementTimer = 1
			displacementTimer = 1.f;
		}

//		if aiParams.attackRange < brain._distToNearestEnemy and (actor._state ~= "ATTACKING" or (actor._state == "ATTACKING" and aiParams.continousAttack)) then


		if (aipAttackRange < brain->distToNearestEnemy && 
			( ! IsState(actor, stateAttacking) || (IsState(actor, stateAttacking) && aipContinuousAttack)) ) 
		{
//			if actor.s_SubClass.SoundsDefinitions.attackAmbient then
			if ( subclass.IsValid() && soundsDef["attackAmbient"].IsValid() ) 
			{

//				if self.lastTimeOnAttackAmbient + aiParams.soundAmbientDelay < brain._currentTime and math.random(100) < 3 then
				const float aipSoundAmbientDelay = aiParams["soundAmbientDelay"].GetFloat();
				aiParams.Pop();

				if (lastTimeOnAttackAmbient + aipSoundAmbientDelay < brain->currentTime && 
					gRand->Random() < 0.03f ) 
				{

//					self._lastAttackAmbient = actor:PlaySoundHit("attackAmbient")
					actor.Call("PlaySoundHit", 1, "%s", "attackAmbient");
					lastAttackAmbient = actor.GetInt();
					actor.Pop();

//					if self._lastAttackAmbient then
					if ( lastAttackAmbient > 0 ) 
					{

//						self.lastTimeOnAttackAmbient = brain._currentTime
						lastTimeOnAttackAmbient = brain->currentTime;
					}
				}
			}


//			if self.attackDelay then
			if ( attackDelay > 0 ) 
			{
//				self.attackDelay = self.attackDelay - 1
				attackDelay -= 1;

//				if self.attackDelay == 0 then
				if ( attackDelay == 0 ) 
				{
//					local b,d,e = actor:Trace(actor._SphereSize * 1.8)

					const int numRetvals = 3;
					actor.Call("Trace", numRetvals, "%f", 1.8f * actorSphereSize );

					const bool	traceHit	= actor.GetBool(-1);	// b
					const int	entityId	= actor.GetInt(-3);		// e

					actor.Pop(numRetvals);

//					if b and e then
					if ( traceHit && entityId ) 
					{
//						local obj = EntityToObject[e]

						Script EntityToObject = obj["EntityToObject"];
						Script hitObject = EntityToObject[ entityId ];

//                      if obj then
                        if ( hitObject.IsValid() ) 
						{
//	                        if obj._AIBrain and obj._Class == "CActor" then

							const int hitObjectStackTop = hitObject.GetTop();

	                        if (hitObject["_AIBrain"].IsValid() && 
								! strcasecmp( hitObject["_Class"].GetString(), "CActor" ))
							{
//								actor:Stop()
								actor.Call("Stop", 0, "");

//		                        local anim = actor.s_SubClass.Ambients
								Script anim = subclass["Ambients"];

//		                        if anim and anim[1] then
		                        if ( anim.IsValid() && anim[1].IsValid() ) 
								{
//									anim = anim[math.random(1,table.getn(anim))]
									anim = anim[ IRandom(1, anim.GetObjLength()) ];

//									actor:SetAnim(anim,false)
									actor.Call("SetAnim", 0, "%s %b", anim.GetString(), false);

//									self.firstSeeAnim = true
									isFirstSeeAnim = true;

//									self._CurrAnim = anim
									nameCurrAnim = anim.GetString();
									anim.Pop();
                                } 
								else 
								{
//                                  self.attackDelay = math.random(20,45)
                                    attackDelay = IRandom(20,45);
								}

//		                        return
		                        return;
		                    }

							hitObject.SetTop( hitObjectStackTop );
		                }
					}
				}

//				if self.attackDelay <= 0 then
				if ( attackDelay <= 0 ) 
				{
//					self.attackDelay = nil
					attackDelay = CAIBrain::NIL_INT;
				}
			}
			else 
			{
//				self.delay = nil
				delay = CAIBrain::NIL_INT;

//				if not actor._isWalking and brain._currentTime < actor._lastCantMoveTime + 3/30 then

				const float actorLastCantMoveTime = actor["_lastCantMoveTime"].GetFloat();
				actor.Pop();

				if ( ! IsWalking(actor) &&  brain->currentTime < actorLastCantMoveTime + 3.f/30.f ) 
				{
//					local rnd = math.random(100)
//					if rnd < 50 then
					const float rnd = gRand->Random();
					if ( rnd < 0.5f ) 
					{
//						brain._delayBeforeHunt = math.random(15, 52)
						brain->delayBeforeHunt = IRandom(15, 52);

//						actor:WalkForward(brain._distToNearestEnemy * 0.5, true, math.random(-40, 40))
						actor.Call(
							"WalkForward", 
							0, 
							"%f %b %f %n %n %b", 
							0.5f * brain->distToNearestEnemy,
							true,
							FRandom(-40.f, 40.f));
					} 
					else 
					{
//						if rnd < 75 then
						if ( rnd < 0.75f ) 
						{
//							actor:WalkTo(actor._groundx+FRand(-4,4), actor._groundy, actor._groundz+FRand(-4,4), true)
							actor.Call(
								"WalkTo", 
								0, 
								"%f %f %f %b %n %n %b", 
								actorPos.X + FRandom(-4.f, 4.f), 
								actorPos.Y, 
								actorPos.Z + FRandom(-4.f, 4.f),
								true );

//							brain._delayBeforeHunt = math.random(15, 52)
							brain->delayBeforeHunt = IRandom(15, 52);
						} 
						else 
						{
//							brain.escape = math.random(36, 66)
							brain->escape = IRandom(36, 66);
						}
					}
//					return
					return;
				}

//				local findPathSpeed = aiParams.findPathSpeed
				float findPathSpeed;
				if ( aiParams["findPathSpeed"].IsValid() )
				{
					findPathSpeed = aiParams["findPathSpeed"].GetFloat();
				}
//				if not findPathSpeed then
				else
				{
//					findPathSpeed = 0.12
					findPathSpeed = 0.12f;
				}

//				if FRand(0.0, 1.0) < findPathSpeed or not actor._isAnimating then								-- narazie
				if ( gRand->Random() < findPathSpeed || ! IsAnimating(actor) ) 
				{
//					if aiParams.moveDisplacement > 0 and math.random(100) < 9 then
					const float aipMoveDisplacement = aiParams["moveDisplacement"].GetFloat();
					aiParams.Pop();

					if ( aipMoveDisplacement > 0.f && gRand->Random() < 0.09f )
					{
//						local old = actor._displacement
						float oldDisplacement;

//						if not old then
						if ( ! actor["_displacement"].IsValid() )
						{
//							Game:Print(actor._Name.." >>>>>> old == nil!")
							//Game:Print(actor._Name.." >>>>>> old == nil!")

//							old = 0
							oldDisplacement = 0.f;
						}
						actor.Pop();

//						actor._displacement = FRand(-aiParams.moveDisplacement * self.displacementTimer, aiParams.moveDisplacement * self.displacementTimer)
						const float actorDisplacement = 
							FRandom(
								-aipMoveDisplacement * displacementTimer, 
								aipMoveDisplacement * displacementTimer);

						actor.SetFloat("_displacement", actorDisplacement); 

//						local maxDiff = 3.0 + FRand(0.0,1.2)
						const float maxDiff = 3.f + FRandom(0.f, 1.2f);

//						local diff = actor._displacement - old
						const float diff = actor["_displacement"].GetFloat() - oldDisplacement;
						actor.Pop();

//						if diff > maxDiff then
						if ( diff > maxDiff ) 
						{
//							actor._displacement = old + maxDiff
							actor.SetFloat("_displacement", oldDisplacement + maxDiff );
						}

//						if diff < -maxDiff then
						if ( diff < -maxDiff ) 
						{
//							actor._displacement = old - maxDiff
							actor.SetFloat("_displacement", oldDisplacement - maxDiff );
						}
					}

//					if brain._currentTime < actor._lastCantMoveTime + 2/30 then
					if ( brain->currentTime < actorLastCantMoveTime + 2.f/30.f ) 
					{
//						local b,d,e = actor:Trace(2)
						const int numRetvals = 3;
						actor.Call("Trace", numRetvals, "%f", 2.f );

						const bool	traceHit = actor.GetBool(-1);	// b
						const int	entityId = actor.GetInt(-3);	// e

						actor.Pop(numRetvals);

//						if b and e then
						if ( traceHit && entityId ) 
						{
//							local obj = EntityToObject[e]
							Script EntityToObject = obj["EntityToObject"];
							Script hitObject = EntityToObject[ entityId ];

//                          if obj then
                            if ( hitObject.IsValid() ) 
							{
//                              if obj._AIBrain and obj._Class == "CActor" then
								const int hitObjectStackTop = hitObject.GetTop();
    
								if (hitObject["_AIBrain"].IsValid() && 
									! strcasecmp( hitObject["_Class"].GetString(), "CActor" ))
								{
//                                  self.attackDelay = math.random(30,60)
                                    attackDelay = IRandom(30,60);

//                                  return
                                    return;
                                }
								hitObject.SetTop( hitObjectStackTop );
                            }
						}

//						if actor._displacement then
						const int actorStackTop = actor.GetTop();
						if ( actor["_displacement"].IsValid() ) 
						{
//							actor._displacement = actor._displacement * 0.5
							actor.SetFloat("_displacement", 0.5f * actor["_displacement"].GetFloat()); 
						}
						actor.SetTop( actorStackTop );
					}

//					if actor._isWalking and actor.s_SubClass.rotate90R and not actor._isRotating then		-- oznacza, ze ma animacje obrotu
					if ( IsWalking(actor) && subclass["rotate90R"].IsValid() && !actorIsRotating ) 
					{
//						local angleAttack = math.atan2(brain.r_closestEnemy._groundx - actor._groundx, brain.r_closestEnemy._groundz - actor._groundz)
						const float attackAngle = atan2f( closestEnemyPos.X - actorPos.X, closestEnemyPos.Z - actorPos.Z );

//						local aDist = AngDist(actor.angle, angleAttack)
						const float actorAngle = actor["angle"].GetFloat();
						actor.Pop();

						const float aDist = CAIBrain::angDist(actorAngle, attackAngle);

//						if math.abs(aDist) > 90 * math.pi/180 then
						if ( fabs(aDist) > 90.f * LUA_PI / 180.f ) 
						{
//							actor:Stop()
							actor.Call("Stop", 0, "");
						}
					}

//					if not actor._isWalking and actor.s_SubClass.rotate90R then
					if ( ! IsWalking(actor) && subclass["rotate90R"].IsValid() ) 
					{
//						actor:RotateToVectorWithAnim(brain.r_closestEnemy._groundx, 0, brain.r_closestEnemy._groundz)
						actor.Call(
							"RotateToVectorWithAnim",
							0,
							"%f %f %f",
							closestEnemyPos.X, 0.f, closestEnemyPos.Z);

//						if not actor._isRotating then
						if ( ! actorIsRotating ) 
						{
//							actor:WalkTo(brain.r_closestEnemy._groundx, brain.r_closestEnemy._groundy, brain.r_closestEnemy._groundz, true)
							actor.Call(
								"WalkTo", 
								0, 
								"%f %f %f %b %n %n %b", 
								closestEnemyPos.X, closestEnemyPos.Y, closestEnemyPos.Z, 
								true );
						}
					} 
					else 
					{
//						if not actor._rotatingWithAnim or not actor._isRotating then
						if ( ! IsRotatingWithAnim(actor) || ! actorIsRotating ) 
						{
//							local dist = 9999
							float distSquared = 9999.f;

//							if actor._destx and actor._isWalking then
							if ( actor["_destx"].IsValid() && IsWalking(actor) ) 
							{
//								dist = Dist3D(actor._destx, actor._desty, actor._destz, brain.r_closestEnemy._groundx, brain.r_closestEnemy._groundy, brain.r_closestEnemy._groundz)
								const float actorDestx = actor["_destx"].GetFloat();
								const float actorDesty = actor["_desty"].GetFloat();
								const float actorDestz = actor["_destz"].GetFloat();

								distSquared = closestEnemyPos.DistSquared( Vector(actorDestx, actorDesty, actorDestz) );
							}

//							if dist > 0.3 or not actor._isWalking or math.random(100) < 10 then
							const float distThreshold = 0.3f;
							if (distSquared > distThreshold * distThreshold ||
								! IsWalking(actor) ||
								gRand->Random() < 0.1f ) 
							{
//								actor:WalkTo(brain.r_closestEnemy._groundx, brain.r_closestEnemy._groundy, brain.r_closestEnemy._groundz, true)
								actor.Call(
									"WalkTo", 
									0, 
									"%f %f %f %b %n %n %b", 
									closestEnemyPos.X, closestEnemyPos.Y, closestEnemyPos.Z, 
									true );
							}
						}
					}
				}
			}
		}
		else
		{
			int actorStackTop = actor.GetTop();

//			if actor._HasMovingCurve then				-- gdy jest za blisko gracza, a porusza sie z animacja, zeby go nie przepychal
			if ( actor["_HasMovingCurve"].GetBool() ) 
			{
//				if brain._distToNearestEnemy < actor._SphereSize * 2 and actor._state == "ATTACKING" then
				if ( brain->distToNearestEnemy < actorSphereSize * 2.f && IsState(actor, stateAttacking) ) 
				{
//					actor._HasMovingCurve = false
					actorHasMovingCurve = false;
					actor.SetBool("_HasMovingCurve", actorHasMovingCurve );
				}
			}

//			if actor._isWalking then
			if ( IsWalking(actor) ) 
			{
//				actor:Stop()
				actor.Call("Stop", 0, "");
			}

			actor.SetTop( actorStackTop );

//			if math.random(100) < 18 --[[and not actor._rotatingWithAnim--]] then		--and not aiParams.doNotRotateWhenShooting
			if ( gRand->Random() < 0.18f ) 
			{
//				local angle = math.atan2(brain.r_closestEnemy._groundx - actor._groundx, brain.r_closestEnemy._groundz - actor._groundz)
				const float attackAngle = atan2f( closestEnemyPos.X - actorPos.X, closestEnemyPos.Z - actorPos.Z );
				const float actorAngle = actor["angle"].GetFloat();
				actor.Pop();

//				local aDist = AngDist(actor.angle, angle)
				const float aDist = CAIBrain::angDist(actorAngle, attackAngle);

//				if math.abs(aDist) >= 30 * math.pi/180 then
				if ( fabs(aDist) >= 30.f * LUA_PI / 180.f ) 
				{
//					actor:RotateTo(angle * 180 / math.pi)
					actor.Call(
						"RotateTo",
						0,
						"%f", actorAngle * 180.f / LUA_PI);
				} 
			}

//			if (not actor._isAnimating or actor._state ~= "ATTACKING") and not actor._rotatingWithAnim then
			if ((! IsAnimating(actor) || ! IsState(actor, stateAttacking)) && 
				! IsRotatingWithAnim(actor) ) 
			{
//				if not self.delay then
				if ( delay <= 0.f ) 
				{
//					local angleAttack = math.atan2(brain.r_closestEnemy._groundx - actor._groundx, brain.r_closestEnemy._groundz - actor._groundz)
					const float attackAngle = atan2f( closestEnemyPos.X - actorPos.X, closestEnemyPos.Z - actorPos.Z );
					const float actorAngle = actor["angle"].GetFloat();
					actor.Pop();

//					local aDist = AngDist(actor.angle, angleAttack)
					const float aDist = CAIBrain::angDist(actorAngle, attackAngle);

//					local angleRange = aiParams.attackRangeAngle
//					if not angleRange then
//						angleRange = 120
//					end

//					angleRange = angleRange * 1.25		-- troche zapasu

					const float mysteriousTuningParameter = 1.25f;
					const float angleRange =
						mysteriousTuningParameter * aiParams["attackRangeAngle"].GetFloat(-1, 120.f);

					aiParams.Pop();

//					if math.abs(aDist) < angleRange*math.pi/180 then
					if ( fabs(aDist) < angleRange * LUA_PI / 180.f ) 
					{
//						actor._state = "ATTACKING"
						SetState( actor, stateAttacking );

						const int actorStackTop = actor.GetTop();
						const int aiParamsStackTop = aiParams.GetTop();

//						local att = actor.s_SubClass.NearAttacks        -- narazie
//						if att then

						if ( subclass["NearAttacks"].IsValid() ) 
						{
//							if aiParams.NearAttacks then
//								att = aiParams.NearAttacks

							Script att;
							const int attStackTop = att.GetTop();

							if ( aiParams["NearAttacks"].IsValid() ) 
							{
								att = aiParams["NearAttacks"].GetTable();
							}
							else
							{
								att = subclass["NearAttacks"].GetTable();
							}

//							local animName = att[math.random(1,table.getn(att))]
							const size_t attCount = att.GetObjLength();
							const char * animName = att[IRandom(1, attCount)].GetString();

//							actor:SetAnim(animName, aiParams.continousAttack)
							actor.Call("SetAnim", 0, "%s %b", animName, aipContinuousAttack);
							animName = 0;	// managed by LUA -- freed when stack sufficiently popped.

//							if self._lastAttackAmbient then
							if ( lastAttackAmbient ) 
							{
//								actor:StopLastSound(self._lastAttackAmbient)
								actor.Call("StopLastSound", 0, "%d", lastAttackAmbient );

//								self._lastAttackAmbient = nil
								lastAttackAmbient = CAIBrain::NIL_INT;	
							}

//							self.attackDelay = 5
							attackDelay = 5;

							att.SetTop( attStackTop );
						}

						actor.SetTop( actorStackTop );
						aiParams.SetTop( aiParamsStackTop );

//						PlayLogicSound("FIRE",actor._groundx,actor._groundy,actor._groundz,9,18,Player)--actor)	-- ### hack

						// @hack -- according to PCF. I don't know why [jaba].
						//PlayLogicSound("FIRE",actor._groundx,actor._groundy,actor._groundz,9,18,Player)--actor)	-- ### hack

//						self.delay = aiParams.delayBetweenAttacks * FRand(0.8, 1.2) + 2
						delay = aiParams["delayBetweenAttacks"].GetFloat() * FRandom(0.8f, 1.2f) + 2.f;
						aiParams.Pop();

//						self.delayStart = 1
						delayStart = 1;
					} 
				} 
				else 
				{
//					if self.delayStart then
					if ( delayStart > 0 ) 
					{
//						if aiParams.attackRangeMinDist then
						if ( aiParams["attackRangeMinDist"].IsValid() ) 
						{
//							if brain._distToNearestEnemy < aiParams.attackRangeMinDist then
							if ( brain->distToNearestEnemy < aiParams["attackRangeMinDist"].GetFloat() ) 
							{
//								brain.escape = math.random(aiParams.escapeTimeWhenToCloseToPlayer * 0.9, aiParams.escapeTimeWhenToCloseToPlayer * 1.1)
								const float aipEscapeTimeWhenCloseToPlayer = aiParams["escapeTimeWhenToCloseToPlayer"].GetFloat();
								aiParams.Pop();

								brain->escape = 
									FRandom(
										aipEscapeTimeWhenCloseToPlayer * 0.9f, 
										aipEscapeTimeWhenCloseToPlayer * 1.1f);

//								return
								return;
							}
						}

						const float chanceOfEscapeAfterFire = aiParams["chanceOfEscapeAfterFire"].GetFloat();
						aiParams.Pop();

//						if aiParams.chanceOfEscapeAfterFire > 0 then
						if ( chanceOfEscapeAfterFire > 0.f ) 
						{
//							if (brain._lastHitTime >= brain._lastMissedTime) and (math.random(100) < aiParams.chanceOfEscapeAfterFire*100) then
							if ((brain->lastHitTime >= brain->lastMissedTime) && 
								(gRand->Random() < chanceOfEscapeAfterFire) ) 
							{
//								brain.escape = math.random(aiParams.escapeTime * 0.7, aiParams.escapeTime * 1.2)
								const int escapeTime = aiParams["escapeTime"].GetInt();
								aiParams.Pop();
								brain->escape = FRandom(escapeTime * 0.7f, escapeTime * 1.2f);
							}
						}

//						self.delayStart = nil
						delayStart = CAIBrain::NIL_INT;

//						local d = self.delay
						float d = delay;

//						if d > 30 then
						if ( d > 30.f ) 
						{
//							d = 30
							d = 30.f;
						}

//						if d > 5 and math.random(100) < d then
						if ( d > 5.f && FRandom(1.f, 100.f) < d ) 
						{
//							local anim = actor.s_SubClass.Ambients
							Script anim = subclass["Ambients"].GetTable();

//		                    if anim and anim[2] then
		                    if ( anim.IsValid() and anim[2].IsValid() ) 
							{
//								anim = anim[math.random(2,table.getn(anim))]
								const int animCount = anim.GetObjLength();
								
								String animName = anim[ IRandom(2, animCount) ].GetString();
								anim.Pop();

//								actor:SetAnim(anim,false)
								actor.Call("SetAnim", 0, "%s %b", animName.AsChar(), false);

//								self.firstSeeAnim = true
								isFirstSeeAnim = true;

//								self._CurrAnim = anim
								nameCurrAnim = animName;

//								self.delay = nil
								delay = CAIBrain::NIL_FLOAT;

//								return
								return;
							}
						}
					}

//					self.delay = self.delay - 1
					delay -= 1;

//					if self.delay <= 0 then
					if ( delay <= 0.f ) 
					{
//						self.delay = nil
						delay = CAIBrain::NIL_FLOAT;
					}
				}
			}
		}
	} 	
	else	// 	if ( brain->r_closestEnemy ) 
	{
//		if (not actor._isWalking and actor._state ~= "ATTACKING") or not actor._isAnimating then
		if ((!IsWalking(actor) && !IsState(actor, stateAttacking) ) ||
			!IsAnimating(actor)) 
		{
//			brain._delayBeforeHunt = nil
			brain->delayBeforeHunt = CAIBrain::NIL_INT;

//          actor._state = "IDLE"
			SetState(actor, stateIdle);
		}

//		if brain._delayBeforeHunt then
		if ( brain->delayBeforeHunt > CAIBrain::NIL_INT ) 
		{
//			brain._delayBeforeHunt = brain._delayBeforeHunt - 1
			brain->delayBeforeHunt--;

//			if brain._delayBeforeHunt < 0 and actor._state ~= "ATTACKING" then
			if ( brain->delayBeforeHunt < 0 && !IsState(actor, stateAttacking)) 
			{
//				brain._delayBeforeHunt = nil
				brain->delayBeforeHunt = CAIBrain::NIL_INT;

//				actor._state = "IDLE"
				SetState(actor, stateIdle);
			}
		}

//		if aiParams.continousAttack then
		if ( aipContinuousAttack ) 
		{
//			actor._state = "IDLE"
			SetState(actor, stateIdle);
		}

//		self.state = 0
		//state = 0;	// @debug [jaba] what does this do? Unused in this class.

//		self.delay = nil
		delay = CAIBrain::NIL_INT;
	}
}

void CAttackState::onRelease(CAIBrain *brain, Script &obj)
{
	ASSERT_AND_RETURN_IF_NOT( brain );

	delay = CAIBrain::NIL_INT;

	//local actor = brain._Objactor
	Script actor = CAIBrain::getActor(obj);

	//local aiParams = actor.AiParams
	Script aiParams = CAIBrain::getAiParams(actor);

//	brain.target = nil
// @debug [jaba] not used?

//	actor._displacement = nil
	actor.SetInt("_displacement", CAIBrain::NIL_INT);

	//	actor._state = "UNKNOWN"
	SetState(actor, "UNKNOWN");
}

float CAttackState::evaluate(CAIBrain *brain, Script &obj)
{
	ASSERT_AND_RETURN_VAL_IF_NOT( brain, 0.f );

	const char * stateAttacking = "ATTACKING";
	Script actor = CAIBrain::getActor(obj);

	//if brain.r_closestEnemy or brain._Objactor._state == "ATTACKING" or self.firstSeeAnim then
	if (brain->r_closestEnemy || 
		IsState( actor, stateAttacking) ||
		isFirstSeeAnim
		)
	{
	//	return 0.5
		return 0.5f;
	}

	//if not brain.r_closestEnemy and brain._delayBeforeHunt then
	if(!brain->r_closestEnemy && brain->delayBeforeHunt > CAIBrain::NIL_INT)
	{
	//	return 0.5
		return 0.5f;
	}
	//return 0
	return 0.f;
}
