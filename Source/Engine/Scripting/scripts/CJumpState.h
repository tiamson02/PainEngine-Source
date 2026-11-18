#ifndef JUMP_H_ALREADY_INCLUDED
#define JUMP_H_ALREADY_INCLUDED

#include "CAIState.h"

class CJumpState : public CAIState
{
public:
	CJumpState();
    virtual ~CJumpState();

    virtual void    onInit(CAIBrain *brain, Script &obj);
    virtual void    onUpdate(CAIBrain *brain, Script &obj);
    virtual void    onRelease(CAIBrain *brain, Script &obj);
    virtual float   evaluate(CAIBrain *brain, Script &obj);
};

#endif	// JUMP_H_ALREADY_INCLUDED
