#ifndef RUNTOPLAYER_H_ALREADY_INCLUDED
#define RUNTOPLAYER_H_ALREADY_INCLUDED

#include "CAIState.h"

class CRunToPlayerState : public CAIState
{
public:
	CRunToPlayerState();
    virtual ~CRunToPlayerState();

    virtual void    onInit(CAIBrain *brain, Script &obj);
    virtual void    onUpdate(CAIBrain *brain, Script &obj);
    virtual void    onRelease(CAIBrain *brain, Script &obj);
    virtual float   evaluate(CAIBrain *brain, Script &obj);
};

#endif	// RUNTOPLAYER_H_ALREADY_INCLUDED
