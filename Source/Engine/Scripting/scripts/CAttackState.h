#ifndef ATTACK_H_ALREADY_INCLUDED
#define ATTACK_H_ALREADY_INCLUDED

#include "CAIState.h"
#include "LeString.h"

class CAttackState : public CAIState
{
private:

public:
	CAttackState();
    virtual ~CAttackState();

    virtual void    onInit(CAIBrain *brain, Script &obj);
    virtual void    onUpdate(CAIBrain *brain, Script &obj);
    virtual void    onRelease(CAIBrain *brain, Script &obj);
    virtual float   evaluate(CAIBrain *brain, Script &obj);

	float	displacementTimer;
	float	lastTimeOnAttack;
    float	lastTimeOnAttackAmbient;
	int		attackDelay;
	bool	isFirstSeeAnim;
	bool	isFirstTime;
	float	lastTimeWalkTo;

	String  nameCurrAnim;

	int		lastAttackAmbient;
	float	delay;
	int		delayStart;
};

#endif	// ATTACK_H_ALREADY_INCLUDED
