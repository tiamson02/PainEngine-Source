#include "Engine.h"
#include <string.h>
#include "CAIBrain.h"
#include "CIdleState.h"
#include "Core/Math/GRandom.h"

CIdleState::CIdleState(){
    type = AISTATE_IDLE;
    name = CAIState::stateNames[type];
    lastTimeAmbientSound = 0;
    lastAmbient = CAIBrain::NIL_FLOAT;
    delay = CAIBrain::NIL_INT;
    GuardStill = false;
    timeChangeStillToFalse = CAIBrain::NIL_INT;
    OLDonlyWPmove = false;
    OLDdoNotUseWP = false;
    walked = false;
    newDestination.Set(0, 0, 0);
    newDestinationSet = false;
}

CIdleState::~CIdleState(){
}

void CIdleState::onInit(CAIBrain *brain, Script &obj){
    int     top = obj.GetTop();
    Script  actor = CAIBrain::getActor(obj);
    Script  aiParams = CAIBrain::getAiParams(obj);


    if (!actor["_isWalking"].GetBool())
        actor.Call("SetIdle", 0, NULL);

    
    delay = CAIBrain::NIL_INT;
    GuardStill = aiParams["GuardStill"].GetBool();
    timeChangeStillToFalse = CAIBrain::NIL_INT;
    OLDonlyWPmove = actor["onlyWPmove"].GetBool();
    OLDdoNotUseWP = actor["doNotUseWP"].GetBool();

    obj.SetTop(top);
    if (brain->walkArea == NULL)
        actor.SetBool("onlyWPmove", aiParams["useOnlyWaypointsWhenGuard"].GetBool());

    walked = false;
    newDestination.Set(0, 0, 0);
    newDestinationSet = false;

    obj.SetTop(top);
}

void CIdleState::onUpdate(CAIBrain *brain, Script &obj){
    Script  actor = CAIBrain::getActor(obj);
    Script  aiParams = CAIBrain::getAiParams(obj);
    Script  subclass = actor["s_SubClass"];
    int     top = obj.GetTop();
    bool    isWalking = actor["_isWalking"].GetBool();
    float   walkStep = aiParams["walkStep"].GetFloat();
    Entity *entity = CAIBrain::getEntity(actor);


    if (!isWalking &&  !strcasecmp(actor["_state"].GetString(), "ANIMATING")){
        if (timeChangeStillToFalse >= 0){
            timeChangeStillToFalse = CAIBrain::NIL_INT;
            GuardStill = false;
        }

        return;
    }
    
    if (lastTimeAmbientSound + 5 < brain->currentTime){	
        if (subclass.IsValid()){
            Script soundsDef = subclass["SoundsDefinitions"];

            if (soundsDef.IsValid() && soundsDef["ambients"].IsValid() && IRandom(1, 1000) < 4){
                lastTimeAmbientSound = brain->currentTime;
                actor.Call("PlaySoundHit", 0, "%s, %d, %d", "ambients", 10, 25);
            }
        }
    }


    if (timeChangeStillToFalse >= 0){
        timeChangeStillToFalse--;

        if (timeChangeStillToFalse < 0){
            timeChangeStillToFalse = CAIBrain::NIL_INT;
            GuardStill = false;
            //Game:Print("self.GuardStill END")
        }
    }

    if (aiParams["alwaysSee"].IsValid() && GuardStill && !isWalking){

        if (IRandom(1, 100) < 20){
            Vector pos = CAIBrain::getEntityPos(actor);


            actor.Call("RotateToVector", 0, "%f, %f, %f", pos.X, pos.Y, pos.Z);
        }

        return;
    }


    obj.SetTop(top);
    if (!GuardStill){
        if (!isWalking){
            float   stopAfterWalking = aiParams["stopAfterWalking"].GetFloat(-1, CAIBrain::NIL_FLOAT);
            Script  stopAfterWalkingTime = aiParams["stopAfterWalkingTime"];
            int     stopTime1 = stopAfterWalkingTime[1].GetInt();


            if (brain->walkArea == NULL){
                if (brain->currentTime < actor["_lastCantMoveTime"].GetFloat() + (2.0f/30.0f)){
                    //Game:Print("obstacle detected by idle goal")
                    int ang = IRandom(45, 60);


                    brain->walkStepLocal *= 0.65f;

                    if (brain->walkStepLocal < walkStep * 0.4f)
                        brain->walkStepLocal = walkStep * 0.4f;
                    
                    if (IRandom(1, 100) < 50)
                        ang = -ang;

                    if (entity){
                        int newTop = obj.GetTop();


                        actor.Call("Trace", 3, "%f, %f", actor["_SphereSize"].GetFloat() * FRandom(1.0f, 1.3f), ang);

                        bool b = obj.GetBool(newTop + 1);

                        if (b)
                            actor.Call("Rotate", 0, "%d", -ang);

                        else{
                            //if (debugMarek){ Game:Print("idle.state -> normal rot") }
                            actor.Call("Rotate", 0, "%d", ang);
                        }

                        obj.SetTop(newTop);
                    }
                }
                else{
                    float guardRange = aiParams["guardRange"].GetFloat();


                    if (guardRange > 0 && !newDestinationSet){
                        Vector pos = CAIBrain::getEntityPos(entity);
                        Vector test = brain->guardPos;


                        test.Y = pos.Y;

                        if (test.DistSquared(pos) > guardRange * guardRange){
                            //Game:Print("GUARD OVER")
                            float   rndx = FRandom(-guardRange, guardRange) * 0.5f;
                            float   rndz = FRandom(-guardRange, guardRange) * 0.5f;


                            newDestination.X = brain->guardPos.X + rndx;
                            newDestination.Y = pos.Y;
                            newDestination.Z = brain->guardPos.Z + rndz;
                            newDestinationSet = true;


                            if (stopAfterWalkingTime.IsValid() && stopTime1 && walked){
                                //Game:Print("STOP")
                                timeChangeStillToFalse = IRandom(stopTime1, stopAfterWalkingTime[2].GetInt());
                                GuardStill = true;
                                walked = false;

                                return;
                            }
                        }
                    }

                    if (stopAfterWalking != CAIBrain::NIL_FLOAT && FRandom(0.0f, 1.0f) <= stopAfterWalking && walked){
                        timeChangeStillToFalse = IRandom(stopTime1, stopAfterWalkingTime[2].GetInt());
                        GuardStill = true;
                        walked = false;

                        return;
                    }
                    else{
                        brain->walkStepLocal *= 1.1;

                        if (brain->walkStepLocal > walkStep)
                            brain->walkStepLocal = walkStep;

                        if (newDestinationSet){
                            //Game:Print("GUARD -> walk to near")
                            actor.Call("WalkTo", 0, "%f, %f, %f", newDestination.X, newDestination.Y, newDestination.Z);

                            newDestinationSet = false;
                        }
                        else{
                            float maxDist;

                            if (guardRange > 0)
                                maxDist = guardRange * 2.0f;
                            
                            brain->patrol(actor, false, true, maxDist);
                        }

                        walked = true;
                    }
                }
            }
            else{
                if (stopAfterWalking != CAIBrain::NIL_FLOAT && FRandom(0.0f, 1.0f) <= stopAfterWalking && walked){
                    timeChangeStillToFalse = IRandom(stopTime1, stopAfterWalkingTime[2].GetInt());
                    GuardStill = true;
                    walked = false;
                    
                    return;
                }
                else{
                    if (aiParams["walkAreaRandom"].GetBool()){
                        int size = brain->walkArea->getPointCount();
                        int rnd = IRandom(1, size);


                        if (size > 1 && rnd == brain->walkAreaNo){
                            if (rnd < size - 1)
                                rnd++;
                            else
                                rnd--;
                        }

                        brain->walkAreaNo = rnd;
                        Vector pnt = brain->walkArea->getPoint(rnd);


                        actor.Call("WalkTo", 0, "%f, %f, %f, %b", pnt.X, pnt.Y, pnt.Z, false);
                    }
                    else{
                        Vector pnt = brain->walkArea->getPoint(brain->walkAreaNo);


                        actor.Call("WalkTo", 0, "%f, %f, %f, %b", pnt.X, pnt.Y, pnt.Z, false);

                        brain->walkAreaNo++;

                        if (brain->walkAreaNo > brain->walkArea->getPointCount())
                            brain->walkAreaNo = 0;
                    }

                    walked = true;
                }
            }
        }
    }
    else{
        if (!actor["_isRotating"].GetBool() && !isWalking){
            if (lastAmbient + 1 < brain->currentTime){

                //the member actor.AmbientsFreq never seems to be defined in the script, so we'll set it to nil for now
                float ambientsFreq = CAIBrain::NIL_FLOAT;   //actor["AmbientsFreq"].GetFloat();


                if (ambientsFreq == CAIBrain::NIL_FLOAT)
                    ambientsFreq = 0.25f;

                //Game:Print("losowansko "..ambientsFreq)
                lastAmbient = brain->currentTime;
                if (FRandom(0.0f, 1.0f) < ambientsFreq){
                    Script  ambients = subclass["Ambients"];
                    int     newTop = ambients.GetTop();


                    if (ambients.IsValid()){
                        const char *animName = ambients[IRandom(1, ambients.GetCount())].GetString();


                        //Game:Print("set ambient "..animName)
                        //target animation is different than current => change it
                        if (strcasecmp(actor["Animation"].GetString(), animName)){
                            int newTop = actor.GetTop();


                            actor.Call("SetAnim", 1, "%s, %b", animName, false);

                            if (actor.GetBool(newTop + 1))
                                actor.SetString("_state", "ANIMATING");
                        }

                        obj.SetTop(top);
                        return;
                    }
                }
            }


            //clear the stack
            obj.SetTop(top);
            if (brain->rotate180AfterEndWalking){
                brain->rotate180AfterEndWalking = false;

                //Game:Print("rotate after")
                actor.Call("Rotate", 0, "%d", IRandom(150, 210));
            }

            if (delay){
                delay--;

                if (delay <= 0)
                    delay = CAIBrain::NIL_INT;
            }
            else{
                if (aiParams["guardAngleRandom"].GetBool()){
                    actor.Call("Rotate", 0, "%d", IRandom(-60, 60));

                    Script  guardChangePosition = aiParams["guardChangePosition"];
                    int     guardChange1 = guardChangePosition[1].GetInt();


                    if (guardChangePosition.IsValid() && guardChange1)
                        delay = IRandom(guardChange1, guardChangePosition[2].GetInt());
                    else
                        delay = IRandom(0, 40);
                }
                else{
                    Script  guardAngle = aiParams["guardAngle"];
                    int     guardAngle1 = guardAngle[1].GetInt();
                    int     guardAngle2 = guardAngle[2].GetInt();


                    if (guardAngle.IsValid() && guardAngle1){
                        int sign = 1;


                        if (IRandom(1, 100) < 50)
                            sign = -1;
                        
                        int newAngle = sign * IRandom(guardAngle1, guardAngle2);
                        

                        actor.Call("Rotate", 0, "%d", newAngle);

                        float ad = CAIBrain::angDist(actor["_angleDest"].GetFloat(), brain->guardAng);


                        if ((ad > guardAngle1 * (3.1415f/180.0f)) || (ad < guardAngle2 * (3.1415f/180.0f)))
                            actor.Call("RotateTo", 0, "%f", brain->guardAng * (180.0f/3.1415f) + IRandom(-guardAngle2, guardAngle2) * 0.2f);
                        
                        
                        Script  guardChangePosition = aiParams["guardChangePosition"];
                        int     guardChange1 = guardChangePosition[1].GetInt();


                        if (guardChangePosition.IsValid() && guardChange1)
                            delay = IRandom(guardChange1, guardChangePosition[2].GetInt());
                        else
                            delay = IRandom(0, 40);
                    }
                }
            }
        }
    }

    obj.SetTop(top);
}

void CIdleState::onRelease(CAIBrain *brain, Script &obj){
    int     top = obj.GetTop();
    Script  actor = CAIBrain::getActor(obj);
    Script  aiParams = CAIBrain::getAiParams(obj);

    brain->rotate180AfterEndWalking = false;
    actor.SetBool("onlyWPmove", OLDonlyWPmove);
    actor.SetBool("doNotUseWP", OLDdoNotUseWP);

    obj.SetTop(top);
}

float CIdleState::evaluate(CAIBrain *brain, Script &obj){
    return 0.01f;
}
