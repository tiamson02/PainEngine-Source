#include "CHearState.h"
#include "Core/Math/GRandom.h"
#include "CAIBrain.h"


CHearState::CHearState(){
	type = AISTATE_HEAR;
	name = CAIState::stateNames[type];

    lastTimeHEAR = -100;
    active = false;
    dest.Set(0, 0, 0);
    state = 0;
    rot = false;
}

CHearState::~CHearState(){
}

void CHearState::onInit(CAIBrain *brain, Script &obj){
    Script actor = CAIBrain::getActor(obj);


    active = true;

    actor.Call("RotateToVector", 0, "%f, %f, %f", dest.X, dest.Y, dest.Z);
    state = 0;
}

void CHearState::onUpdate(CAIBrain *brain, Script &obj){
    Script actor = CAIBrain::getActor(obj);


    if (state == 0 && !actor["_isRotating"].GetBool()){
        if (!rot)
			actor.Call("WalkTo", 0, "%f, %f, %f, %b", dest.X, dest.Y, dest.Z, true);

		state = 1;
    }

    if (state == 1 && !actor["_isWalking"].GetBool() && !actor["_isRotating"].GetBool())
		active = false;
}

void CHearState::onRelease(CAIBrain *brain, Script &obj){
    active = false;
    dest.Set(0, 0, 0);
}

float CHearState::evaluate(CAIBrain *brain, Script &obj){
    int     top = obj.GetTop();
    Script  actor = CAIBrain::getActor(obj);
    Script  aiParams = CAIBrain::getAiParams(actor);
    float   perception = aiParams["perception"].GetFloat();


    if (aiParams["alwaysSee"].GetBool() || aiParams["huntPlayer"].GetBool()){
        obj.SetTop(top);
        return 0.0f;
    }


    if (!actor["_isWalking"].GetBool() && brain->lastHitByEnemyType == CAIBrain::ATKTYPE_FIRE && !aiParams["WalkAreaWhenAttack"].GetBool()){
        obj.SetTop(top);
		actor.Call("WalkForward", 0, "%d, %b, %d", IRandom(1, 4), true,  IRandom(-45,45));
		brain->lastHitByEnemyPos.Set(0, 0, 0);
    }

    else{
        obj.SetTop(top);

        if (brain->r_closestEnemy == NULL && !brain->escape && strcasecmp(actor["_state"].GetString(), "ATTACKING")){
            if (IRandom(1, 100) < perception * 200){
                dest = brain->lastHitByEnemyPos;
				rot = false;

				if (aiParams["WalkAreaWhenAttack"].GetBool())
                    rot = true;

				brain->lastHitByEnemyPos.Set(0, 0, 0);

				actor.Call("Stop", 0, NULL);			
				return 0.15f;
            }

            else{
                if (lastTimeHEAR + 0.7f < brain->currentTime && IRandom(1, 100) < perception * 100){     //&& ActiveSounds.Size() > 0){

                    //clear the stack to make sure there's room
                    obj.SetTop(top);

                    //search the active sound list to find a sound near me
                    obj.Call("findNearbySound", 5, NULL);

                    //grab the return value(s) of the function
                    bool                    soundHeard = obj.GetBool(-1, false);
                    CAIBrain::LogicSound    soundType;
                    Vector                  soundPos;
                    
                    
                    if (soundHeard){
                        soundType = (CAIBrain::LogicSound)obj.GetInt(-5, CAIBrain::LOGICSND_NONE);
                        soundPos.Set(obj.GetFloat(-4, 0), obj.GetFloat(-3, 0), obj.GetFloat(-2, 0));

                        lastTimeHEAR = brain->currentTime;
						brain->lastTimeHEAR = brain->currentTime;
						
                        actor.Call("Stop", 0, NULL);

                        if (soundType == CAIBrain::LOGICSND_RICOCHET)
							brain->rotate180AfterEndWalking = true;

                        dest = soundPos;
						rot = false;


                        Vector  pos = CAIBrain::getEntityPos(actor);

						if (pos.DistSquared(dest) < 36 || aiParams["WalkAreaWhenAttack"].GetBool())
							rot = true;

						state = 0;

                        obj.SetTop(top);
						return 0.15f;
                    }
                }
            }
        }
    }


    obj.SetTop(top);
	if (active)
		return 0.15f;

	return 0.0f;
}
