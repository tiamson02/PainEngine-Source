#ifndef CLOSEATTACK_H_ALREADY_INCLUDED
#define CLOSEATTACK_H_ALREADY_INCLUDED

#include "CAIState.h"

class CCloseAttackState : public CAIState
{
public:
	CCloseAttackState();
    virtual ~CCloseAttackState();

    virtual void    onInit(CAIBrain *brain, Script &obj);
    virtual void    onUpdate(CAIBrain *brain, Script &obj);
    virtual void    onRelease(CAIBrain *brain, Script &obj);
    virtual float   evaluate(CAIBrain *brain, Script &obj);
};

#endif	// CLOSEATTACK_H_ALREADY_INCLUDED
