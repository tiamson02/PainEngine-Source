#ifndef  __CAIBRAIN_H__
# define __CAIBRAIN_H__

# include <DynArray.h>
# include "CArea.h"
# include "CAIState.h"
# include "Scripting/Scripting.h"
# include "Core/Math/Vector.h"

#define LUA_PI F_PI
class Entity;

class CAIBrain{
public:

    //NOTE: these enum values are matched up with the members of the AttackTypes table in the script
    typedef enum{
        ATKTYPE_NONE =              0,
        ATKTYPE_SHOTGUN =           1,
        ATKTYPE_GRENADE =           2,
        ATKTYPE_ROCKET =            3,
        ATKTYPE_OUTOFLEVEL =        4,
        ATKTYPE_MINIGUN =           5,    
        ATKTYPE_FIRE =              6,
        ATKTYPE_EXPLOSION =         7,
        ATKTYPE_POISON =            8,
        ATKTYPE_BUBBLE =            9,
        ATKTYPE_STAKE =             10,
        ATKTYPE_PAINKILLER =        11,
        ATKTYPE_HITGROUND =         12,
        ATKTYPE_TELEFRAG =          13,
        ATKTYPE_SUICIDE =           14,    
        ATKTYPE_STICKYBOMB =        15,
        ATKTYPE_STEP =              16,
        ATKTYPE_AICLOSE =           17,
        ATKTYPE_AIFAR =             18,
        ATKTYPE_SHURIKEN =          19,
        ATKTYPE_PHYSICS =           20,
        ATKTYPE_STONE =             21,
        ATKTYPE_DEMON =             22,
        ATKTYPE_ELECTRO =           23,    
        ATKTYPE_PAINKILLERROTOR =   24,
        ATKTYPE_FIREBALL =          25,
        ATKTYPE_ITEMCOLLISION =     26,
        ATKTYPE_TANK =              27,
        ATKTYPE_LAVA =              28,
        ATKTYPE_CONSOLEKILL =       29,
        ATKTYPE_HURT_PENT =         30,
        ATKTYPE_RIFLE =             31,
        ATKTYPE_FLAMETHROWER =      32,
        ATKTYPE_FLAG =              33,
        ATKTYPE_BOLTSTICK =         34,
        ATKTYPE_HEATERBOMB =        35,
    } AttackType;

    typedef enum{ 
        LOGICSND_NONE       = 0,
        LOGICSND_EXPLOSION  = 10,
        LOGICSND_FIRE       = 8,
        LOGICSND_RICOCHET   = 6,
        LOGICSND_STEP       = 4,
        LOGICSND_ANIMAL     = 3,
        LOGICSND_RELOAD     = 2,
    } LogicSound;



    static const int WALL_HIT_SOUND_COUNT = 8;

    //table of sound filenames to play when a bullet hits a wall
    static const char *bulletHitWallSnd[WALL_HIT_SOUND_COUNT];


    static int luaAI_New(lua_State *state);
    static int luaAI_Delete(lua_State *state);
    static int luaAI_RotateHead(lua_State *state);
    static int luaAI_PreUpdate(lua_State *state);
    static int luaAI_AddState(lua_State *state);
    static int luaAI_WeaponFire(lua_State *state);
    static int luaAI_Get(lua_State *state);
    static int luaAI_Set(lua_State *state);

	// Used to convert script code like: 
	// self.delayTimer = nil, where otherwise delayTimer is an integer
	// @assume this must be a negative value, for checks like:
	// if (foo > 0) //assume non-nil.
	static const int	NIL_INT		= -100;	
	static const float	NIL_FLOAT	= -100.0f;

    DynamicArray<CAIState *>    goals;
    CAIState *                  currentGoal;
    Entity *                    r_closestEnemy;
    float                       enemyLastSeenTime;
    Vector                      enemyLastSeenPoint;
    Vector                      enemyLastSeenVelocity;
    float                       enemyLastSeenVelocitySpeed;
    float                       distToNearestEnemy;
    float                       enemyLastSeenOrientation;
    float                       escape;                         //--


	void *                      _Objactor;
	float                       currentTime;
	Vector                      guardPos;
	float                       guardAng;

	float                       lastMissedTime;
	float                       lastHitTime;

	int                         walkAreaNo;
	CArea *                     walkArea;

	Vector                      velocity;
	float                       speed;

    bool                        rotate180AfterEndWalking;
	//float _lastHitFriendTime = -100,
	AttackType                  lastHitByEnemyType;
	//float _lastHitByEnemyTime = -100,
	Vector                      lastHitByEnemyPos;
	float                       lastDamageTime;
	void *                      r_lastDamageWho;
	void *                      Objhostage;
	float                       FarAttack_Prio;
	int                         timeToIdle;
	//const char *                animNameFalling;
	float                       diffInangleAttackX;
	float                       diffInangleAttackY;
	void *                      _confuseEnemy;
    float                       lastTimeHEAR;
	float						walkStepLocal;
	int							delayToHunt;
	int							delayBeforeHunt;

    void        updateSensors();
    bool        addState(const char *name);
    bool        addState(CAIState::StateType type);
    int         evaluateGoals(Script &obj);

public:
    CAIBrain();
    ~CAIBrain();

    void            rotateHead(float delta);
    void            preUpdate();
	void		    onApply(Script & obj);
    void            onUpdate();
    void            weaponFire(float yDest, float lrSrcZ, float lrSrcY);
    void            patrol(Script & actor, bool run = false, bool useOnlyWP = false, float maxDist = 0.0f, const char * animName = NULL);

    static void     registerScript();

	static Script   getActor(Script &obj);
    static Script   getAiParams(Script &actor);
    static Vector   getEntityPos(Script &actor);
    static Vector   getEntityPos(Entity *e);
    static Entity * getEntity(Script &actor);
    static Entity * getEntity(int entityID);

    static float    angDist(float a1, float a2);
};


#endif
