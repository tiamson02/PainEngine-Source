#ifndef JUMPUP_H_ALREADY_INCLUDED
#define JUMPUP_H_ALREADY_INCLUDED

#include "CAIState.h"

class CJumpUpState : public CAIState
{
public:
	CJumpUpState();
    virtual ~CJumpUpState();

    virtual void    onInit(CAIBrain *brain, Script &obj);
    virtual void    onUpdate(CAIBrain *brain, Script &obj);
    virtual void    onRelease(CAIBrain *brain, Script &obj);
    virtual float   evaluate(CAIBrain *brain, Script &obj);
};

#endif	// JUMPUP_H_ALREADY_INCLUDED
