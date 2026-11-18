#ifndef FARATTACK_H_ALREADY_INCLUDED
#define FARATTACK_H_ALREADY_INCLUDED

#include "CAIState.h"

class CFarAttackState : public CAIState
{
public:
	CFarAttackState();
    virtual ~CFarAttackState();

    virtual void    onInit(CAIBrain *brain, Script &obj);
    virtual void    onUpdate(CAIBrain *brain, Script &obj);
    virtual void    onRelease(CAIBrain *brain, Script &obj);
    virtual float   evaluate(CAIBrain *brain, Script &obj);
};

#endif	// FARATTACK_H_ALREADY_INCLUDED
