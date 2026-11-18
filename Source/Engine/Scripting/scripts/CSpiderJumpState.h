#ifndef SPIDERJUMP_H_ALREADY_INCLUDED
#define SPIDERJUMP_H_ALREADY_INCLUDED

#include "CAIState.h"

class CSpiderJumpState : public CAIState
{
public:
	CSpiderJumpState();
    virtual ~CSpiderJumpState();

    virtual void    onInit(CAIBrain *brain, Script &obj);
    virtual void    onUpdate(CAIBrain *brain, Script &obj);
    virtual void    onRelease(CAIBrain *brain, Script &obj);
    virtual float   evaluate(CAIBrain *brain, Script &obj);
};

#endif	// SPIDERJUMP_H_ALREADY_INCLUDED
