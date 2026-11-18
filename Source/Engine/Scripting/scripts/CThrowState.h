#ifndef THROW_H_ALREADY_INCLUDED
#define THROW_H_ALREADY_INCLUDED

#include "CAIState.h"

class CThrowState : public CAIState
{
public:
	CThrowState();
    virtual ~CThrowState();

    virtual void    onInit(CAIBrain *brain, Script &obj);
    virtual void    onUpdate(CAIBrain *brain, Script &obj);
    virtual void    onRelease(CAIBrain *brain, Script &obj);
    virtual float   evaluate(CAIBrain *brain, Script &obj);
};

#endif	// THROW_H_ALREADY_INCLUDED
