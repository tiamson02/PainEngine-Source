#include "Engine.h"
#include "CAIBrain.h"
#include "Scripting/Scripting.h"
#include "Core/Math/GRandom.h"
#include "Engine/PCFSystem.h"
#include "Engine/Entity.h"
#include "Engine/World.h"
#include "Physics/PhysicsObject.h"
#include "Core/Template.h"


extern Script GScript;
extern PCFSystem* GEngine;


//table of sound filenames to play when a bullet hits a wall
const char *CAIBrain::bulletHitWallSnd[CAIBrain::WALL_HIT_SOUND_COUNT] = {
        "bullet-stone",
        "bullet-stone2",
        "bullet-stone3",
        "bullet-stone4",
        "ricochet1",
        "ricochet2",
        "ricochet3",
        "ricochet4"};


//****************************** script interface functions ***************************//
void CAIBrain::registerScript(){
    static bool initialized = false;


    if (initialized)
        return;

    luaL_reg ai_lib[] = {
        {"New",         luaAI_New},  
        {"Release",     luaAI_Delete},
        {"RotateHead",  luaAI_RotateHead},
        {"PreUpdate",   luaAI_PreUpdate},
        {"AddState",    luaAI_AddState},
        {"WeaponFire",  luaAI_WeaponFire},
        {"Get",         luaAI_Get},
        {"Set",         luaAI_Set},
        {NULL,          NULL}
    };

    GScript.RegisterLibrary("AI", ai_lib);
    initialized = true;
}

int CAIBrain::luaAI_New(lua_State *state){

    return 0;
}

int CAIBrain::luaAI_Delete(lua_State *state){

    return 0;
}

int CAIBrain::luaAI_RotateHead(lua_State *state){

    return 0;
}

int CAIBrain::luaAI_PreUpdate(lua_State *state){

    return 0;
}

// @param	state name - string
int CAIBrain::luaAI_AddState(lua_State *state)
{
	Assert(state);
	Script L( state );

	TRACE("\n!!!! -------------------------------------------------- \n");
	TRACE("!!!! CAIBrain::luaAI_AddState");
	TRACE("\n!!!! -------------------------------------------------- \n");
	
	// @todo -- get "this" from lua state
	//// Get argument: state name.
	//String name = L.GetString();	
	//return addState( name.AsChar() ) ? 1 : 0;	// don't return actual pointer.
}

int CAIBrain::luaAI_WeaponFire(lua_State *state){

    return 0;
}

int CAIBrain::luaAI_Get(lua_State *state){

    return 0;
}

int CAIBrain::luaAI_Set(lua_State *state){

    return 0;
}

//******************************* utility script interface functions ********************************//
Script CAIBrain::getActor(Script &obj) {
    return obj["_Objactor"];
    //return _Objactor;
}

Script CAIBrain::getAiParams(Script &actor) {
    return actor["AiParams"];
}

Vector CAIBrain::getEntityPos(Script &actor) {
    Entity *e = getEntity(actor);

    return getEntityPos(e);
}

Vector CAIBrain::getEntityPos(Entity *e){
    if (e){
        if (e->PO)
            return e->PO->GetPawnFloorPos();
        else
            return e->GetPosition();
    }

    return Vector(0, 0, 0);
}

Entity *CAIBrain::getEntity(Script &actor){
    return getEntity(actor["_Entity"].GetInt());
}

Entity *CAIBrain::getEntity(int entityID){
    return (Entity *)GEngine->pcfWorld->GetEntityByIndex(entityID);
}


//******************************* constructors & destructor *****************************//
CAIBrain::CAIBrain()
	: currentGoal(NULL)
    , r_closestEnemy(NULL)
    , enemyLastSeenTime( CAIBrain::NIL_FLOAT )
    , enemyLastSeenVelocitySpeed(0)
    , distToNearestEnemy( CAIBrain::NIL_FLOAT )
    , enemyLastSeenOrientation(0)
    , escape(0.f)

    , _Objactor(NULL)
    , currentTime(0)
    , guardAng(0)

    , lastMissedTime( CAIBrain::NIL_FLOAT )
    , lastHitTime( CAIBrain::NIL_FLOAT )

    , walkAreaNo(0)
    , walkArea(NULL)

    , speed(0)

    , rotate180AfterEndWalking(false)
    , lastHitByEnemyType(ATKTYPE_NONE)
    , lastDamageTime( CAIBrain::NIL_FLOAT )
    , r_lastDamageWho(NULL)
    , Objhostage(NULL)
    , FarAttack_Prio(0.51f)
    , timeToIdle(5)
    , diffInangleAttackX(0)
    , diffInangleAttackY(0)
    , _confuseEnemy(NULL)
    , lastTimeHEAR(0)
	, walkStepLocal(0.f)
	, delayToHunt( CAIBrain::NIL_INT )
	, delayBeforeHunt( CAIBrain::NIL_INT )
{
    enemyLastSeenPoint.Set(0, 0, 0);
    enemyLastSeenVelocity.Set(0, 0, 0);
	guardPos.Set(0, 0, 0);
    velocity.Set(0, 0, 0);
    lastHitByEnemyPos.Set(0, 0, 0);

    //lastHitFriendTime =  CAIBrain::NIL_INT,
    //lastHitByEnemyTime =  CAIBrain::NIL_INT,
    //animNameFalling = NULL;
}

CAIBrain::~CAIBrain(){
    for (int i = 0; i < goals.Size(); i++)
        delete goals[i];
    
    goals.ClearArray();
}

//******************************* interface functions ***********************************//
bool CAIBrain::addState(const char *stateName){
    CAIState::StateType type = CAIState::getStateType(stateName);

    return addState(type);
}

bool CAIBrain::addState(CAIState::StateType type){
    CAIState *state = CAIState::newState(type);

	TRACE("\n--------------------------------------------------------- \n");
	TRACE("\n--------------------------------------------------------- \n");
	

    if (state)
	{
        int index = goals.Find(NULL);

        if (index < 0)
            index = goals.GrowArray(1);

        goals[index] = state;

		TRACE("\n\n\n\n>>>> CAIBrain::addState -- created state '%s'\n\n\n\n", state);
        return true;
    }

    else
	{
        TRACE("\n\n\n\nCAIBrain::addState(): ERROR-> could not create the state of type '%d'\n\n\n\n", type);
	}

    return false;
}

int CAIBrain::evaluateGoals(Script &obj){
    float   eval = -1;
    float   currentEval = -1;
    int     goal = 0;


    for (int i = 0; i < goals.Size(); i++){
        currentEval = goals[i]->evaluate(this, obj);

        if (currentEval > eval){
            eval = currentEval;
            goal = i;
        }
    }

    return goal;
}

void CAIBrain::updateSensors(){

}

void CAIBrain::rotateHead(float delta){

}

void CAIBrain::preUpdate(){

}

void CAIBrain::onApply(Script & obj)
{
	//local actor = self._Objactor
	Script actor = getActor(obj);

	//local aiParams = actor.AiParams
	Script aiParams = getAiParams(actor);

	//self._GuardPos.X = actor.Pos.X
	//self._GuardPos.Y = actor.Pos.Y
	//self._GuardPos.Z = actor.Pos.Z
	//if aiParams.walkArea and aiParams.walkArea ~= "" and type(aiParams.walkArea) == "string" then
	//	self._walkArea = rawget(getfenv(),aiParams.walkArea)
	//end

	//self._walkStepLocal = aiParams.walkStep
	walkStepLocal = aiParams["walkStep"].GetFloat();
}

void CAIBrain::onUpdate(){

}

void CAIBrain::weaponFire(float yDest, float lrSrcZ, float lrSrcY){

}

//function Patrol(actor, brain, run, useOnlyWP, maxDist, anim)
void CAIBrain::patrol(Script & actor, bool run, bool useOnlyWP, float maxDist, const char * animName)
{
    int top = actor.GetTop();

//	local aiParams = actor.AiParams
    Script aiParams = getAiParams(actor);

//	local movement = FRand(brain._walkStepLocal, brain._walkStepLocal * 2)
	float const movement = FRandom(walkStepLocal, walkStepLocal * 2.0f);

//	local ang = FRand(-12,12)
	float ang = FRandom(-12.0f, 12.0f);
//
//	if ENTITY.PO_Exist(actor._Entity) then
	Entity * entity = getEntity(actor);


	if (entity)
	{
//		local b = actor:Trace(movement + actor._SphereSize, ang)
		float const actorSphereSize = actor["_SphereSize"].GetFloat();
        bool        traceHit;


        actor.SetTop(top);
		actor.Call("Trace", 3, "%f %f", movement + actorSphereSize, ang);
		traceHit = actor.GetBool(top + 1);

//		if b then
		if (traceHit)
		{
//			--Game:Print("Patrol: Trace: nie mogie isc do przodu")
//			ang = math.random(45,60)
			ang = IRandom(45, 60);

//			local b = actor:Trace(movement, ang)
	
            actor.SetTop(top);
			actor.Call("Trace", 3, "%f %f", movement, ang);
			traceHit = actor.GetBool(top + 1);
//			if b then
			if (traceHit)
			{
//				ang = -ang
				ang = -ang;
//			end
			}
//		end
		}
//	end
	}
//--	Game:Print("Patrol: "..ang.." dist="..movement.." "..actor._groundy)
//	actor:WalkForward(movement, run, ang, maxDist, anim, useOnlyWP)
	
	// @todo clear stack?
    actor.Call("WalkForward", 0, "%f %b %d %f %s %b", movement, run, ang, maxDist, animName, useOnlyWP);
    actor.SetTop(top);
//end
}

//********************************** static utility functions ******************************//
float CAIBrain::angDist(float a1, float a2)
{
	const float pi_f = 3.1415926535897932384626433832795f;

    float d1 = fmodf(fabsf(a2 - a1), 2.f * pi_f);
	float d2 = 2.f * pi_f - d1;
	float m = Min(d1, d2);

    if (d1 < d2){
		if (a2 >= a1)
            m = d1;
        else
            m = -d1;
    }
    else{
		if (a2 >= a1)
            m = -d2;
        else
            m = d2;
    }

    return fmodf(m, pi_f * 2.f);

}
