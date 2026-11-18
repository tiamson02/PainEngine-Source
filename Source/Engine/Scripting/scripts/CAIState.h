#ifndef  __CAISTATE_H__
# define __CAISTATE_H__

# include "Core/HashMap.h"
# include "Scripting/Scripting.h"
# include "Core/Math/Vector.h"

class CAIBrain;


class CAIState{
public:
    typedef enum{
        AISTATE_NONE = -1,

        AISTATE_BASE_STATES_START,
        AISTATE_ATTACK = AISTATE_BASE_STATES_START,
        AISTATE_CLOSEATTACK,
        AISTATE_FARATTACK,
        AISTATE_FLEE,
        AISTATE_HEAR,
        AISTATE_HUNT,
        AISTATE_IDLE,
        AISTATE_JUMP,
        AISTATE_JUMPUP,
        AISTATE_RUNTOPLAYER,
        AISTATE_SPIDERJUMP,
        AISTATE_THROW,
        AISTATE_BASE_STATES_END,


        AISTATE_CUSTOM_STATES_START = AISTATE_BASE_STATES_END,
        AISTATE_ALASTOR_IDLE = AISTATE_CUSTOM_STATES_START,
        AISTATE_ALASTOR_ATKFLY,
        AISTATE_ALASTOR_ATKFLAME,
        AISTATE_ALASTOR_AB,
        AISTATE_ALASTOR_GROUNDATK,
        AISTATE_ALASTOR_BREATH,
        AISTATE_ALASTORKING_GROUNDATK,

        AISTATE_AMPUTEE_IDLEBARF,
        AISTATE_AMPUTEE_IDLE,
        AISTATE_AMPUTEE_ATK,

        AISTATE_APOCZOMBIE_RUNTOPLAYER,

        AISTATE_BANSHEE_ATK,

        AISTATE_BAT_HEAR,
        AISTATE_BAT_ESCAPE,
        AISTATE_BAT_ATK,
        AISTATE_BAT_IDLE,

        AISTATE_BEAST_ROLL,

        AISTATE_BONES_WAKEUP,
        AISTATE_BONES_AB,

        AISTATE_DETO_ATK,

        AISTATE_EVILMONK_THROWANDDIE,
        AISTATE_EVILMONKV2_THROWIMMID,

        AISTATE_EXECUTIONER_SLICECORPSE,
        AISTATE_EXECUTIONERV3_SPIKEATK,

        AISTATE_FLYINGNUN_FLYAREA,
        AISTATE_FLYINGNUN_THROW,

        AISTATE_GIANT_IDLE,
        AISTATE_GIANT_IDLEANIM,
        AISTATE_GIANT_WALKANDSTRIKE,
        AISTATE_GIANT_SPAWN,
        AISTATE_GIANT_STRIKE,
        AISTATE_GIANT_SPIKES,
        AISTATE_GIANT_SUCKING,
        AISTATE_GIANT_ABREAK,

        AISTATE_GIRL_BURN,

        AISTATE_HELLANGEL_ROCKETJUMP,

        AISTATE_HELLBIKER_AB,
        AISTATE_HELLBIKER_DRINK,

        AISTATE_KRUSTYFLYING_IDLE,

        AISTATE_LEPERMONK_THROWCORPSE,

        AISTATE_LOKI_SPAWN,

        AISTATE_LUCIFER_IDLE,
        AISTATE_LUCIFER_ATK,
        AISTATE_LUCIFER_WALK,
        AISTATE_LUCIFER_CLOSEATK,

        AISTATE_MILITARYGUN_FIRE,

        AISTATE_NINJA1_TELEPORT,

        AISTATE_NUN2_SPECIALATK,

        AISTATE_OFFICER_CALLRAID,

        AISTATE_PANZERDEMON_IDLE,

        AISTATE_PANZERSPIDER_IDLE,
        AISTATE_PANZERSPIDER_PHASE2,

        AISTATE_PINOKIO_WAKEUP,

        AISTATE_PREACHER_ATK,
        AISTATE_PREACHER_THROWSTUFF,

        AISTATE_RAVEN_HEAR,
        AISTATE_RAVEN_IDLE,

        AISTATE_SAMURAIV2_AB,

        AISTATE_SKULL_TAKEHOSTAGE,

        AISTATE_SWAMP_IDLE,
        AISTATE_SWAMP_WATERBALL,
        AISTATE_SWAMP_HAND,
        AISTATE_SWAMP_GETFLAMES,
        AISTATE_SWAMP_AB,
        AISTATE_SWAMP_EXTINGUISHFIRE,

        AISTATE_TANK_IDLE,
        AISTATE_TANK_FIRE,

        AISTATE_THOR_IDLE,
        AISTATE_THOR_STRIKE,
        AISTATE_THOR_WALKANDSTRIKE,
        AISTATE_THOR_DROPHAMMER,
        AISTATE_THOR_IDLEANIM,

        AISTATE_VAMP_ATK,
        AISTATE_VAMP_ANIM,
        AISTATE_VAMP_WALK,
        AISTATE_VAMP_THROW,
        AISTATE_VAMP_EARTHQUAKE,

        AISTATE_WINGEDDEMON_IDLE,
        AISTATE_WINGEDDEMON_ATK,

        AISTATE_WITCH_FLYAREA,
        AISTATE_WITCH_THROW,
        AISTATE_CUSTOM_STATES_END,

        AISTATE_COUNT = AISTATE_CUSTOM_STATES_END,
    } StateType;

private:
    static CAIState *newBaseState(StateType type);
    static CAIState *newCustomState(StateType type);
    static CAIState *newCustomAlastorState(StateType type);
    static CAIState *newCustomAmputeeState(StateType type);
    static CAIState *newCustomBatState(StateType type);
    static CAIState *newCustomGiantState(StateType type);
    static CAIState *newCustomLuciferState(StateType type);
    static CAIState *newCustomSwampState(StateType type);
    static CAIState *newCustomThorState(StateType type);
    static CAIState *newCustomVampState(StateType type);
    static CAIState *newCustomBonesState(StateType type);
    static CAIState *newCustomEvilMonkState(StateType type);
    static CAIState *newCustomExecutionerState(StateType type);
    static CAIState *newCustomFlyingNunState(StateType type);
    static CAIState *newCustomHellBikerState(StateType type);
    static CAIState *newCustomPanzerSpiderState(StateType type);
    static CAIState *newCustomPreacherState(StateType type);
    static CAIState *newCustomRavenState(StateType type);
    static CAIState *newCustomTankState(StateType type);
    static CAIState *newCustomWingedDemonState(StateType type);
    static CAIState *newCustomWitchState(StateType type);

    static HashMap<String, StateType, 32> nameTable;

protected:

//    char        name[MAX_NAME_LEN];
	const char* name;
	StateType   type;

public:
	CAIState(const StateType a_type = AISTATE_NONE, const char * const a_name = "<none>");
    virtual ~CAIState();

    //************************* member access functions *********************************//
    StateType       getType() const;
    const char *    getName() const;

    //************************* event functions *****************************************//
    virtual void    onInit(CAIBrain *brain, Script &obj) = 0;
    virtual void    onUpdate(CAIBrain *brain, Script &obj) = 0;
    virtual void    onRelease(CAIBrain *brain, Script &obj) = 0;
    virtual float   evaluate(CAIBrain *brain, Script &obj) = 0;

    //************************* state object generation functions ***********************//
    static bool initializeNameTable();
    static CAIState *newState(StateType type);
    static StateType getStateType(const char *name);

	static const char * stateNames[ AISTATE_COUNT ];
};

//---------------------------------------------------------------------------
#define ASSERT_AND_RETURN_IF_NOT(cond)\
	Assert((cond));\
	if (!(cond))\
	{\
		return;\
	}

//---------------------------------------------------------------------------
#define ASSERT_AND_RETURN_VAL_IF_NOT(cond, val)\
	Assert((cond));\
	if (!(cond))\
	{\
		return (val);\
	}

#endif
