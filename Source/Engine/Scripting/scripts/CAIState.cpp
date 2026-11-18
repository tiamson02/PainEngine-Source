#include "Engine.h"
#include "CAIState.h"
#include "allStates.h"
#include "Core/Debug.h"
#include "Engine/Entity.h"
#include "Engine/PCFSystem.h"
#include "Engine/World.h"
#include "Physics/PhysicsObject.h"


extern PCFSystem *GEngine;

//******************************* Temporary and Configuration Macros ****************//
#ifdef DEBUG
# define NOT_IMPLEMENTED(name)  { TRACE("%s(): ERROR-> the AI states for '%s' are not implemented yet!\n", __FUNCTION__, #name); return NULL; }
#else
# define NOT_IMPLEMENTED(name)  return NULL
#endif

//******************************* Utility functions *********************************//
template<typename X>
inline bool INRANGE(X x, X min, X max){
    return x >= min && x <= max;
}

//*************************** static members **********************************************//
HashMap<String, CAIState::StateType, 32> CAIState::nameTable;

// @note [jaba] The names below *must* match the names used in script. Notice that there is no
// consistency to the capitalization.

// @todo [jaba] Make capitalization consistent. Get this system working first, as this 
// change could touch a lot of script code and level data.

const char * CAIState::stateNames[ AISTATE_COUNT ] = 
{
	"attack",
	"closeAttack",
	"FarAttack",
	"flee",
	"hear",
	"hunt",
	"idle",
	"Jump",
	"JumpUP",
	"runToPlayer",
	"spiderJump",
	"throw",

	"",	// AISTATE_ALASTOR_IDLE
	"",	// AISTATE_ALASTOR_ATKFLY
	"",	// AISTATE_ALASTOR_ATKFLAME
	"",	// AISTATE_ALASTOR_AB
	"",	// AISTATE_ALASTOR_GROUNDATK
	"",	// AISTATE_ALASTOR_BREATH
	"",	// AISTATE_ALASTORKING_GROUNDATK

	"",	// AISTATE_AMPUTEE_IDLEBARF
	"",	// AISTATE_AMPUTEE_IDLE
	"",	// AISTATE_AMPUTEE_ATK

	"",	// AISTATE_APOCZOMBIE_RUNTOPLAYER

	"",	// AISTATE_BANSHEE_ATK

	"",	// AISTATE_BAT_HEAR
	"",	// AISTATE_BAT_ESCAPE
	"",	// AISTATE_BAT_ATK
	"",	// AISTATE_BAT_IDLE

	"",	// AISTATE_BEAST_ROLL

	"",	// AISTATE_BONES_WAKEUP
	"",	// AISTATE_BONES_AB

	"",	// AISTATE_DETO_ATK

	"",	// AISTATE_EVILMONK_THROWANDDIE
	"",	// AISTATE_EVILMONKV2_THROWIMMID

	"",	// AISTATE_EXECUTIONER_SLICECORPSE
	"",	// AISTATE_EXECUTIONERV3_SPIKEATK

	"",	// AISTATE_FLYINGNUN_FLYAREA
	"",	// AISTATE_FLYINGNUN_THROW

	"",	// AISTATE_GIANT_IDLE
	"",	// AISTATE_GIANT_IDLEANIM
	"",	// AISTATE_GIANT_WALKANDSTRIKE
	"",	// AISTATE_GIANT_SPAWN
	"",	// AISTATE_GIANT_STRIKE
	"",	// AISTATE_GIANT_SPIKES
	"",	// AISTATE_GIANT_SUCKING
	"",	// AISTATE_GIANT_ABREAK

	"",	// AISTATE_GIRL_BURN

	"",	// AISTATE_HELLANGEL_ROCKETJUMP

	"",	// AISTATE_HELLBIKER_AB
	"",	// AISTATE_HELLBIKER_DRINK

	"",	// AISTATE_KRUSTYFLYING_IDLE

	"",	// AISTATE_LEPERMONK_THROWCORPSE

	"",	// AISTATE_LOKI_SPAWN

	"",	// AISTATE_LUCIFER_IDLE
	"",	// AISTATE_LUCIFER_ATK
	"",	// AISTATE_LUCIFER_WALK
	"",	// AISTATE_LUCIFER_CLOSEATK

	"",	// AISTATE_MILITARYGUN_FIRE

	"",	// AISTATE_NINJA1_TELEPORT

	"",	// AISTATE_NUN2_SPECIALATK

	"",	// AISTATE_OFFICER_CALLRAID

	"",	// AISTATE_PANZERDEMON_IDLE

	"",	// AISTATE_PANZERSPIDER_IDLE
	"",	// AISTATE_PANZERSPIDER_PHASE2

	"",	// AISTATE_PINOKIO_WAKEUP

	"",	// AISTATE_PREACHER_ATK
	"",	// AISTATE_PREACHER_THROWSTUFF

	"",	// AISTATE_RAVEN_HEAR
	"",	// AISTATE_RAVEN_IDLE

	"",	// AISTATE_SAMURAIV2_AB

	"",	// AISTATE_SKULL_TAKEHOSTAGE

	"",	// AISTATE_SWAMP_IDLE
	"",	// AISTATE_SWAMP_WATERBALL
	"",	// AISTATE_SWAMP_HAND
	"",	// AISTATE_SWAMP_GETFLAMES
	"",	// AISTATE_SWAMP_AB
	"",	// AISTATE_SWAMP_EXTINGUISHFIRE

	"",	// AISTATE_TANK_IDLE
	"",	// AISTATE_TANK_FIRE

	"",	// AISTATE_THOR_IDLE
	"",	// AISTATE_THOR_STRIKE
	"",	// AISTATE_THOR_WALKANDSTRIKE
	"",	// AISTATE_THOR_DROPHAMMER
	"",	// AISTATE_THOR_IDLEANIM

	"",	// AISTATE_VAMP_ATK
	"",	// AISTATE_VAMP_ANIM
	"",	// AISTATE_VAMP_WALK
	"",	// AISTATE_VAMP_THROW
	"",	// AISTATE_VAMP_EARTHQUAKE

	"",	// AISTATE_WINGEDDEMON_IDLE
	"",	// AISTATE_WINGEDDEMON_ATK

	"",	// AISTATE_WITCH_FLYAREA
	""	// AISTATE_WITCH_THROW
};

//*************************** state object creation functions *****************************//
//State Object Generation Functions:
//  These three functions generate a new state object to be added to a CAIBrain object.  This
//  could be done through an abstract factory object, but templates tend to explode in size
//  when compiled, and we're already on borrowed space.  For now, we'll keep it manual and 
//  simple.

//newState(): creates a new state object matching the type identified by <type>.  If the 
//  object could not be created (ie: no memory, not implemented, etc) NULL is returned.
//  Otherwise, a pointer to the newly created CAIState object is returned.  The caller
//  is responsible for deleting the returned object.
CAIState *CAIState::newState(StateType type){
    m_setTag("AIState");
    if (INRANGE(type, AISTATE_BASE_STATES_START, AISTATE_BASE_STATES_END))
        return newBaseState(type);

    if (INRANGE(type, AISTATE_CUSTOM_STATES_START, AISTATE_CUSTOM_STATES_END))
        return newCustomState(type);

    return NULL;
}

//newBaseState(): creates a new state object only for one of the base state types.  If
//  <type> is out of the range of the base types, NULL will be returned.  If the state
//  is invalid, unimplemented, or there is not enough memory left, NULL will also be
//  returned.  Otherwise, a pointer to the newly created CAIState object will be
//  returned.
CAIState *CAIState::newBaseState(StateType type){
    switch (type){
        case AISTATE_ATTACK:
			return new CAttackState();

		case AISTATE_FLEE:
			return new CFleeState();

        case AISTATE_HEAR:
			return new CHearState();

        case AISTATE_HUNT:
			return new CHuntState();

        case AISTATE_IDLE:
			return new CIdleState();

        case AISTATE_CLOSEATTACK:
        case AISTATE_FARATTACK:
        case AISTATE_JUMP:
        case AISTATE_JUMPUP:
        case AISTATE_RUNTOPLAYER:
        case AISTATE_SPIDERJUMP:
        case AISTATE_THROW:
        default:
            NOT_IMPLEMENTED(unknown);
    }

    return NULL;
}

CAIState *CAIState::newCustomState(StateType type){
    switch(type){
        case AISTATE_ALASTOR_IDLE:
        case AISTATE_ALASTOR_ATKFLY:
        case AISTATE_ALASTOR_ATKFLAME:
        case AISTATE_ALASTOR_AB:
        case AISTATE_ALASTOR_GROUNDATK:
        case AISTATE_ALASTOR_BREATH:
        case AISTATE_ALASTORKING_GROUNDATK:
            return newCustomAlastorState(type);

        case AISTATE_AMPUTEE_IDLEBARF:
        case AISTATE_AMPUTEE_IDLE:
        case AISTATE_AMPUTEE_ATK:
            return newCustomAmputeeState(type);

        case AISTATE_APOCZOMBIE_RUNTOPLAYER:
            NOT_IMPLEMENTED(apocZombie);

        case AISTATE_BANSHEE_ATK:
            NOT_IMPLEMENTED(banshee);

        case AISTATE_BAT_HEAR:
        case AISTATE_BAT_ESCAPE:
        case AISTATE_BAT_ATK:
        case AISTATE_BAT_IDLE:
            return newCustomBatState(type);

        case AISTATE_BEAST_ROLL:
            NOT_IMPLEMENTED(beast);

        case AISTATE_BONES_WAKEUP:
        case AISTATE_BONES_AB:
            return newCustomBonesState(type);

        case AISTATE_DETO_ATK:
            NOT_IMPLEMENTED(deto);

        case AISTATE_EVILMONK_THROWANDDIE:
        case AISTATE_EVILMONKV2_THROWIMMID:
            return newCustomEvilMonkState(type);

        case AISTATE_EXECUTIONER_SLICECORPSE:
        case AISTATE_EXECUTIONERV3_SPIKEATK:
            return newCustomExecutionerState(type);

        case AISTATE_FLYINGNUN_FLYAREA:
        case AISTATE_FLYINGNUN_THROW:
            return newCustomFlyingNunState(type);

        case AISTATE_GIANT_IDLE:
        case AISTATE_GIANT_IDLEANIM:
        case AISTATE_GIANT_WALKANDSTRIKE:
        case AISTATE_GIANT_SPAWN:
        case AISTATE_GIANT_STRIKE:
        case AISTATE_GIANT_SPIKES:
        case AISTATE_GIANT_SUCKING:
        case AISTATE_GIANT_ABREAK:
            return newCustomGiantState(type);

        case AISTATE_GIRL_BURN:
            NOT_IMPLEMENTED(girl);

        case AISTATE_HELLANGEL_ROCKETJUMP:
            NOT_IMPLEMENTED(hellAngel);

        case AISTATE_HELLBIKER_AB:
        case AISTATE_HELLBIKER_DRINK:
            return newCustomHellBikerState(type);

        case AISTATE_KRUSTYFLYING_IDLE:
            NOT_IMPLEMENTED(krustyFlying);

        case AISTATE_LEPERMONK_THROWCORPSE:
            NOT_IMPLEMENTED(leperMonk);

        case AISTATE_LOKI_SPAWN:
            NOT_IMPLEMENTED(loki);

        case AISTATE_LUCIFER_IDLE:
        case AISTATE_LUCIFER_ATK:
        case AISTATE_LUCIFER_WALK:
        case AISTATE_LUCIFER_CLOSEATK:
            return newCustomLuciferState(type);

        case AISTATE_MILITARYGUN_FIRE:
            NOT_IMPLEMENTED(militaryGun);

        case AISTATE_NINJA1_TELEPORT:
            NOT_IMPLEMENTED(ninja1);

        case AISTATE_NUN2_SPECIALATK:
            NOT_IMPLEMENTED(nun2);

        case AISTATE_OFFICER_CALLRAID:
            NOT_IMPLEMENTED(officer);

        case AISTATE_PANZERDEMON_IDLE:
            NOT_IMPLEMENTED(panzerDemon);

        case AISTATE_PANZERSPIDER_IDLE:
        case AISTATE_PANZERSPIDER_PHASE2:
            return newCustomPanzerSpiderState(type);

        case AISTATE_PINOKIO_WAKEUP:
            NOT_IMPLEMENTED(pinokio);

        case AISTATE_PREACHER_ATK:
        case AISTATE_PREACHER_THROWSTUFF:
            return newCustomPreacherState(type);

        case AISTATE_RAVEN_HEAR:
        case AISTATE_RAVEN_IDLE:
            return newCustomRavenState(type);

        case AISTATE_SAMURAIV2_AB:
            NOT_IMPLEMENTED(samuraiV2);

        case AISTATE_SKULL_TAKEHOSTAGE:
            NOT_IMPLEMENTED(skull);

        case AISTATE_SWAMP_IDLE:
        case AISTATE_SWAMP_WATERBALL:
        case AISTATE_SWAMP_HAND:
        case AISTATE_SWAMP_GETFLAMES:
        case AISTATE_SWAMP_AB:
        case AISTATE_SWAMP_EXTINGUISHFIRE:
            return newCustomSwampState(type);

        case AISTATE_TANK_IDLE:
        case AISTATE_TANK_FIRE:
            return newCustomTankState(type);

        case AISTATE_THOR_IDLE:
        case AISTATE_THOR_STRIKE:
        case AISTATE_THOR_WALKANDSTRIKE:
        case AISTATE_THOR_DROPHAMMER:
        case AISTATE_THOR_IDLEANIM:
            return newCustomThorState(type);

        case AISTATE_VAMP_ATK:
        case AISTATE_VAMP_ANIM:
        case AISTATE_VAMP_WALK:
        case AISTATE_VAMP_THROW:
        case AISTATE_VAMP_EARTHQUAKE:
            return newCustomVampState(type);

        case AISTATE_WINGEDDEMON_IDLE:
        case AISTATE_WINGEDDEMON_ATK:
            return newCustomWingedDemonState(type);

        case AISTATE_WITCH_FLYAREA:
        case AISTATE_WITCH_THROW:
            return newCustomWitchState(type);

        default:
            NOT_IMPLEMENTED(unknown);
    }

    return NULL;
}

CAIState *CAIState::newCustomAlastorState(StateType type){
    switch (type){
        case AISTATE_ALASTOR_IDLE:
        case AISTATE_ALASTOR_ATKFLY:
        case AISTATE_ALASTOR_ATKFLAME:
        case AISTATE_ALASTOR_AB:
        case AISTATE_ALASTOR_GROUNDATK:
        case AISTATE_ALASTOR_BREATH:
        case AISTATE_ALASTORKING_GROUNDATK:

        default:
            NOT_IMPLEMENTED(alastor);
    }

    return NULL;
}

CAIState *CAIState::newCustomAmputeeState(StateType type){
    switch (type){
        case AISTATE_AMPUTEE_IDLEBARF:
        case AISTATE_AMPUTEE_IDLE:
        case AISTATE_AMPUTEE_ATK:

        default:
            NOT_IMPLEMENTED(amputee);
    }

    return NULL;
}

CAIState *CAIState::newCustomBatState(StateType type){
    switch (type){
        case AISTATE_BAT_HEAR:
        case AISTATE_BAT_ESCAPE:
        case AISTATE_BAT_ATK:
        case AISTATE_BAT_IDLE:

        default:
            NOT_IMPLEMENTED(bat);
    }

    return NULL;
}

CAIState *CAIState::newCustomGiantState(StateType type){
    switch (type){
        case AISTATE_GIANT_IDLE:
        case AISTATE_GIANT_IDLEANIM:
        case AISTATE_GIANT_WALKANDSTRIKE:
        case AISTATE_GIANT_SPAWN:
        case AISTATE_GIANT_STRIKE:
        case AISTATE_GIANT_SPIKES:
        case AISTATE_GIANT_SUCKING:
        case AISTATE_GIANT_ABREAK:

        default:
            NOT_IMPLEMENTED(giant);
    }

    return NULL;
}

CAIState *CAIState::newCustomLuciferState(StateType type){
    switch (type){
        case AISTATE_LUCIFER_IDLE:
        case AISTATE_LUCIFER_ATK:
        case AISTATE_LUCIFER_WALK:
        case AISTATE_LUCIFER_CLOSEATK:

        default:
            NOT_IMPLEMENTED(lucifer);
    }

    return NULL;
}

CAIState *CAIState::newCustomSwampState(StateType type){
    switch (type){
        case AISTATE_SWAMP_IDLE:
        case AISTATE_SWAMP_WATERBALL:
        case AISTATE_SWAMP_HAND:
        case AISTATE_SWAMP_GETFLAMES:
        case AISTATE_SWAMP_AB:
        case AISTATE_SWAMP_EXTINGUISHFIRE:

        default:
            NOT_IMPLEMENTED(swamp);
    }

    return NULL;
}

CAIState *CAIState::newCustomThorState(StateType type){
    switch (type){
        case AISTATE_THOR_IDLE:
        case AISTATE_THOR_STRIKE:
        case AISTATE_THOR_WALKANDSTRIKE:
        case AISTATE_THOR_DROPHAMMER:
        case AISTATE_THOR_IDLEANIM:

        default:
            NOT_IMPLEMENTED(thor);
    }

    return NULL;
}

CAIState *CAIState::newCustomVampState(StateType type){
    switch (type){
        case AISTATE_VAMP_ATK:
        case AISTATE_VAMP_ANIM:
        case AISTATE_VAMP_WALK:
        case AISTATE_VAMP_THROW:
        case AISTATE_VAMP_EARTHQUAKE:

        default:
            NOT_IMPLEMENTED(vamp);
    }

    return NULL;
}

CAIState *CAIState::newCustomBonesState(StateType type){
    switch (type){
        case AISTATE_BONES_WAKEUP:
        case AISTATE_BONES_AB:

        default:
            NOT_IMPLEMENTED(bones);
    }

    return NULL;
}

CAIState *CAIState::newCustomEvilMonkState(StateType type){
    switch (type){
        case AISTATE_EVILMONK_THROWANDDIE:
        case AISTATE_EVILMONKV2_THROWIMMID:

        default:
            NOT_IMPLEMENTED(evilMonk);
    }

    return NULL;
}

CAIState *CAIState::newCustomExecutionerState(StateType type){
    switch (type){
        case AISTATE_EXECUTIONER_SLICECORPSE:
        case AISTATE_EXECUTIONERV3_SPIKEATK:

        default:
            NOT_IMPLEMENTED(executioner);
    }

    return NULL;
}

CAIState *CAIState::newCustomFlyingNunState(StateType type){
    switch (type){
        case AISTATE_FLYINGNUN_FLYAREA:
        case AISTATE_FLYINGNUN_THROW:

        default:
            NOT_IMPLEMENTED(flyingNun);
    }

    return NULL;
}

CAIState *CAIState::newCustomHellBikerState(StateType type){
    switch (type){
        case AISTATE_HELLBIKER_AB:
        case AISTATE_HELLBIKER_DRINK:

        default:
            NOT_IMPLEMENTED(hellBiker);
    }

    return NULL;
}

CAIState *CAIState::newCustomPanzerSpiderState(StateType type){
    switch (type){
        case AISTATE_PANZERSPIDER_IDLE:
        case AISTATE_PANZERSPIDER_PHASE2:

        default:
            NOT_IMPLEMENTED(panzerSpider);
    }

    return NULL;
}

CAIState *CAIState::newCustomPreacherState(StateType type){
    switch (type){
        case AISTATE_PREACHER_ATK:
        case AISTATE_PREACHER_THROWSTUFF:

        default:
            NOT_IMPLEMENTED(preacher);
    }

    return NULL;
}

CAIState *CAIState::newCustomRavenState(StateType type){
    switch (type){
        case AISTATE_RAVEN_HEAR:
        case AISTATE_RAVEN_IDLE:

        default:
            NOT_IMPLEMENTED(raven);
    }

    return NULL;
}

CAIState *CAIState::newCustomTankState(StateType type){
    switch (type){
        case AISTATE_TANK_IDLE:
        case AISTATE_TANK_FIRE:

        default:
            NOT_IMPLEMENTED(tank);
    }

    return NULL;
}

CAIState *CAIState::newCustomWingedDemonState(StateType type){
    switch (type){
        case AISTATE_WINGEDDEMON_IDLE:
        case AISTATE_WINGEDDEMON_ATK:

        default:
            NOT_IMPLEMENTED(wingedDemon);
    }

    return NULL;
}

CAIState *CAIState::newCustomWitchState(StateType type){
    switch (type){
        case AISTATE_WITCH_FLYAREA:
        case AISTATE_WITCH_THROW:

        default:
            NOT_IMPLEMENTED(witch);
    }

    return NULL;
}

bool CAIState::initializeNameTable(){
    static bool initialized = false;
    CAIState *  state;

    if (initialized)
        return true;


    nameTable.Clear();

    for (int i = AISTATE_BASE_STATES_START; i < AISTATE_COUNT; i++){
        state = newState((StateType)i);

        if (state){
            nameTable.Insert(state->name, state->type);

            delete state;
        }
    }

    initialized = true;
}

CAIState::StateType CAIState::getStateType(const char *name){
    StateType *type = nameTable.Find(name);

    if (type)
        return *type;

    return AISTATE_NONE;
}

//*************************** constructors & destructor ***********************************//
CAIState::CAIState(const StateType a_type, const char * const a_name)
: type(a_type)
, name(a_name)
{
}

CAIState::~CAIState(){}

//*************************** member access functions *************************************//
CAIState::StateType CAIState::getType() const{
    return type;
}

const char *CAIState::getName() const{
    return name;
}
